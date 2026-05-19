/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <span>
#include <iterator>

#include "nex/base/macros.h"
#include "nex/base/types.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @class Span
 * @brief A non-owning view over a contiguous sequence of bytes
 * 
 * This class represents a lightweight, non-owning view over a contiguous
 * sequence of bytes. It provides access to the underlying data and its size,
 * without managing the memory itself.
 * 
 * @details
 * The Span class is useful for passing around byte buffers without copying
 * the data. It can be constructed from a pointer to bytes and a size, and
 * provides methods to access the data and its length.
 * 
 * Example usage:
 * ```
 * const uint8* buffer = ...;
 * usize size = ...;
 * Span span(buffer, size);
 * // Use span.data() and span.size()
 * ```
 */
template <typename T>
class NEX_EXPORT Span {
public:
    // Type aliases for compatibility with standard container conventions
    using value_type = T;
    using size_type = usize;
    using difference_type = isize;
    using pointer = const value_type*;
    using const_pointer = const value_type*;
    using reference = const value_type&;
    using const_reference = const value_type&;
    using iterator = const value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = NEX_STD reverse_iterator<const_iterator>;
    using const_reverse_iterator = NEX_STD reverse_iterator<const_iterator>;

public:
    // Constructor
    Span(const_pointer buffer, size_type size) 
        : buffer_(buffer, size) {}

    // Copy constructor and assignment
    NEX_DEFAULT_COPY(Span);

    // Move constructor and assignment
    NEX_DEFAULT_MOVE(Span);

    ////// Size and capacity -----------------------

    // Get size (number of elements)
    constexpr size_type size() const { return buffer_.size(); }

    // Get size in bytes
    constexpr size_type sizeBytes() const { return size() * sizeof(value_type); }

    // Get length (number of elements; same as size)
    constexpr size_type length() const { return buffer_.size(); }

    // Check if empty
    constexpr bool empty() const { return buffer_.empty(); }

    ////// Accessors -----------------------

    // Get data pointer
    const_pointer data() const { return buffer_.data(); }

    // Get the first element
    const_reference front() const { return buffer_.front(); }

    // Get the last element
    const_reference back() const { return buffer_.back(); }

    // Get the element at index (with bounds checking)
    const_reference at(size_type index) const { return buffer_.at(index); }

    // Get the element at index (without bounds checking)
    const_reference operator[](size_type index) const { return buffer_[index]; }

    ////// Subspan and slicing -----------------------

    // Create a subspan from start index and length
    Span<T> slice(size_type start, size_type length) const {
        return Span<T>(buffer_.data() + start, length);
    }

    // Create subspan from offset and contains count elements
    Span<T> subspan(size_type offset, size_type count) const {
        return Span<T>(buffer_.data() + offset, count);
    }

    // Get the first count elements as a subspan
    Span<T> first(size_type count) const {
        return Span<T>(buffer_.data(), count);
    }

    // Get the last count elements as a subspan
    Span<T> last(size_type count) const {
        return Span<T>(buffer_.data() + buffer_.size() - count, count);
    }

    ////// Iterators -----------------------

    // Get iterator to the beginning
    constexpr iterator begin() const noexcept {
        return buffer_.data();
    }

    // Get constant iterator to the beginning
    constexpr const_iterator cbegin() const noexcept {
        return buffer_.data();
    }

    // Get iterator to the end
    constexpr iterator end() const noexcept {
        return buffer_.data() + buffer_.size();
    }

    // Get constant iterator to the end
    constexpr const_iterator cend() const noexcept {
        return buffer_.data() + buffer_.size();
    }

    // Get reverse iterator to the beginning
    constexpr reverse_iterator rbegin() const noexcept {
        return reverse_iterator(end());
    }

    // Get reverse iterator to the end
    constexpr reverse_iterator rend() const noexcept {
        return reverse_iterator(begin());
    }

    // Get constant reverse iterator to the beginning
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    // Get constant reverse iterator to the end
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

private:
    // Use STL span as underlying buffer
    NEX_STD span<const value_type> buffer_;
};

NEX_CORE_NAMESPACE_END

/**
 * @brief Alias-lifting to make Span available in the public API namespace without the core qualifier
 * @details 
 * This allows users to use nex::Span instead of nex::core::Span, while still keeping the implementation 
 * details hidden in the core namespace.
 */

NEX_NAMESPACE_BEGIN

/**
 * @class Span
 * @brief A non-owning view over a contiguous sequence of bytes
 */
template <typename T>
NEX_ALIAS_TYPE_FROM_LAYER(core, Span)

NEX_NAMESPACE_END
