/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <span>
#include <iterator>

#include "nex/base/macros.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @class   Span
 * @brief   A non-owning view over a contiguous sequence of bytes
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
    using pointer = const T*;
    using const_pointer = const T*;
    using reference = const T&;
    using const_reference = const T&;
    using iterator = const T*;
    using const_iterator = const T*;
    using reverse_iterator = NEX_STD reverse_iterator<const_iterator>;
    using const_reverse_iterator = NEX_STD reverse_iterator<const_iterator>;

public:
    // Constructor
    Span(const T* buffer, usize size) 
        : buffer_(buffer, size) {}

    // Copy constructor and assignment
    NEX_DEFAULT_COPY(Span);

    // Move constructor and assignment
    NEX_DEFAULT_MOVE(Span);

    ////// Accessors -----------------------

    // Get data pointer
    const T* data() const { return buffer_.data(); }

    // Get size (number of elements)
    constexpr usize size() const { return buffer_.size(); }

    // Get size in bytes
    constexpr usize sizeBytes() const { return size() * sizeof(T); }

    // Get front element
    const T& front() const { return buffer_.front(); }

    // Get back element
    const T& back() const { return buffer_.back(); }

    // Check if empty
    constexpr bool empty() const { return buffer_.empty(); }

    // Get length (number of elements; same as size)
    constexpr usize length() const { return buffer_.size(); }

    // Get element at index
    const T& at(usize index) const { return buffer_.at(index); }

    // Subscript operator
    const T& operator[](usize index) const { return buffer_[index]; }

    ////// Subspan and slicing -----------------------

    // Slice the span
    Span<T> slice(usize start, usize length) const {
        return Span<T>(buffer_.data() + start, length);
    }

    // Create subspan from offset and contains count elements
    Span<T> subspan(usize offset, usize count) const {
        return Span<T>(buffer_.data() + offset, count);
    }

    // Get the first count elements as a subspan
    Span<T> first(usize count) const {
        return Span<T>(buffer_.data(), count);
    }

    // Get the last count elements as a subspan
    Span<T> last(usize count) const {
        return Span<T>(buffer_.data() + buffer_.size() - count, count);
    }

    ////// Iterators -----------------------

    // Get iterator to the beginning
    constexpr const_iterator begin() const noexcept {
        return buffer_.data();
    }

    // Get iterator to the end
    constexpr const_iterator end() const noexcept {
        return buffer_.data() + buffer_.size();
    }

    // Get reverse iterator to the beginning
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    // Get reverse iterator to the end
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    ////// Manipulation -----------------------

    // Clear the span
    void clear() noexcept {
        buffer_ = NEX_STD span<const T>();
    }

private:
    // Use STL std::span as underlying buffer
    NEX_STD span<const T> buffer_;
};

NEX_NAMESPACE_END
