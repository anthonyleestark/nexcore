/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <functional>
#include <iterator>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"

NEX_NAMESPACE_BEGIN

/**
 * @class ByteSpan
 * @brief Read-only non-owning view into contiguous byte data.
 * 
 * This class provides a lightweight view over binary data. It stores only a pointer
 * and a length; it does not allocate, copy, own, or extend the lifetime of the data.
 * 
 * ByteSpan supports:
 * - Construction from raw byte data, void pointers, and Vec<uint8>
 * - Subarray operations (substr, left, right, mid)
 * - Read-only byte access and iteration
 * - Comparison operations
 * - Copying into caller-provided writable storage
 * 
 * @note ByteSpan does not own the underlying data. The user must ensure
 *       that the data remains valid for the lifetime of the ByteSpan.
 * 
 * @warning ByteSpan intentionally exposes read-only access. Use ByteArray when ownership
 *          or mutation is required.
 * 
 * @see ByteArray for an owning byte array class that can create and consume ByteSpan values.
 */
class NEX_API ByteSpan {
public:
    // Type aliases for compatibility with standard container conventions
    using value_type = uint8;
    using size_type = usize;
    using difference_type = isize;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = const value_type&;
    using const_reference = const value_type&;
    using iterator = const_pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = NEX_STD reverse_iterator<iterator>;
    using const_reverse_iterator = NEX_STD reverse_iterator<const_iterator>;

private:
    // Pointer to the data (non-owning)
    const_pointer data_;

    // Length of the view
    size_type size_;

public:
    // Special value representing "not found" for search operations
    static constexpr size_type npos = NumericLimits<size_type>::max();

public:
    ////// Constructors -----------------------------

    // Default constructor (empty view)
    constexpr ByteSpan() noexcept : data_(nullptr), size_(0) {}

    // Construct from pointer and length
    constexpr ByteSpan(const_pointer data, size_type size) noexcept
        : data_(data), size_(size) {}

    // Construct from void pointer and length
    constexpr ByteSpan(const_void_ptr data, size_type size) noexcept
        : data_(static_cast<const_pointer>(data)), size_(size) {}

    // Copy constructor
    constexpr ByteSpan(const ByteSpan& other) noexcept = default;

    // Copy assignment operator
    ByteSpan& operator=(const ByteSpan& other) noexcept = default;

    // Destructor
    ~ByteSpan() = default;

    ////// Iterator support -----------------------------

    // Get iterator to the beginning of the view
    constexpr const_iterator begin() const noexcept { return data_; }

    // Get const iterator to the beginning of the view
    constexpr const_iterator cbegin() const noexcept { return data_; }

    // Get iterator to the end of the view
    constexpr const_iterator end() const noexcept { return data_ ? data_ + size_ : data_; }

    // Get const iterator to the end of the view
    constexpr const_iterator cend() const noexcept { return end(); }

    // Get reverse iterator to the beginning of the reversed view
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    // Get const reverse iterator to the beginning of the reversed view
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    // Get reverse iterator to the end of the reversed view
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // Get const reverse iterator to the end of the reversed view
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    ////// Accessors --------------------------------------------------

    // Access byte at index (with bounds checking)
    const_reference at(size_type pos) const;

    // Access byte at index (no bounds checking)
    constexpr const_reference operator[](size_type pos) const noexcept {
        return data_[pos];
    }

    // Access first byte
    constexpr const_reference front() const noexcept {
        return data_[0];
    }

    // Access last byte
    constexpr const_reference back() const noexcept {
        return data_[size_ - 1];
    }

    // Get pointer to underlying data
    constexpr const_pointer data() const noexcept { return data_; }

    // Get pointer to raw data
    constexpr const_pointer getRawData() const noexcept { return data_; }

    // Get pointer as void*
    constexpr const_void_ptr rawData() const noexcept {
        return static_cast<const_void_ptr>(data_);
    }

    ////// Capacity and size-related operations -----------------------------

    // Get size of the view
    constexpr size_type size() const noexcept { return size_; }

    // Get length of the view (same as size)
    constexpr size_type length() const noexcept { return size_; }

    // Get size in bytes
    constexpr size_type sizeBytes() const noexcept { return size_ * sizeof(value_type); }

    // Check if view is empty
    constexpr bool empty() const noexcept { return size_ == 0; }

    // Get maximum possible size
    constexpr size_type maxSize() const noexcept {
        return (NumericLimits<size_type>::max() / sizeof(value_type)) - 1;
    }

    ////// Modifiers -----------------------------

    // Remove prefix (first n bytes)
    void removePrefix(size_type n) noexcept;

    // Remove suffix (last n bytes)
    void removeSuffix(size_type n) noexcept;

    // Swap with another view
    void swap(ByteSpan& other) noexcept;

    ////// Subspan operations -----------------------------

    // Copy bytes to destination
    size_type copy(pointer dest, size_type count, size_type pos = 0) const;

    // Get subview
    ByteSpan subspan(size_type pos = 0, size_type count = npos) const;

    // Get the left part of the view
    ByteSpan left(size_type count) const noexcept;

    // Get the right part of the view
    ByteSpan right(size_type count) const noexcept;

    // Get the middle part of the view
    ByteSpan mid(size_type start, size_type count = npos) const;

    ////// Search operations -----------------------------

    // Find first occurrence of byte
    size_type indexOf(value_type byte, size_type pos = 0) const noexcept;

    // Find first occurrence of subview
    size_type indexOf(const ByteSpan& other, size_type pos = 0) const noexcept;

    // Find last occurrence of byte
    size_type lastIndexOf(value_type byte, size_type pos = npos) const noexcept;

    // Find last occurrence of subview
    size_type lastIndexOf(ByteSpan other, size_type pos = npos) const noexcept;

    // Find first occurrence of any byte in the set
    size_type findFirstOf(ByteSpan other, size_type pos = 0) const noexcept;

    // Find first occurrence of any byte not in the set
    size_type findFirstNotOf(ByteSpan other, size_type pos = 0) const noexcept;

    // Find last occurrence of any byte in the set
    size_type findLastOf(ByteSpan other, size_type pos = npos) const noexcept;

    // Find last occurrence of any byte not in the set
    size_type findLastNotOf(ByteSpan other, size_type pos = npos) const noexcept;

    // Check if view starts with prefix
    bool startsWith(ByteSpan prefix) const noexcept;

    // Check if view starts with byte
    bool startsWith(value_type byte) const noexcept;

    // Check if view ends with suffix
    bool endsWith(ByteSpan suffix) const noexcept;

    // Check if view ends with byte
    bool endsWith(value_type byte) const noexcept;

    // Check if view contains subview
    bool contains(ByteSpan other) const noexcept;

    // Check if view contains byte
    bool contains(value_type byte) const noexcept;

    // Count occurrences of byte
    size_type count(value_type byte) const noexcept;

    ////// Comparison methods and operators -----------------------------

    // Lexicographical compare with another ByteSpan (byte by byte)
    int32 compare(const ByteSpan& other) const noexcept;

    // Lexicographical compare with a subview of this ByteSpan
    int32 compare(size_type pos, size_type count, const ByteSpan& other) const {
        return subspan(pos, count).compare(other);
    }

    // Equality operator
    friend bool operator==(ByteSpan lhs, ByteSpan rhs) noexcept {
        return lhs.compare(rhs) == 0;
    }

    // Inequality operator
    friend bool operator!=(ByteSpan lhs, ByteSpan rhs) noexcept {
        return !(lhs == rhs);
    }

    // Less-than operator
    friend bool operator<(ByteSpan lhs, ByteSpan rhs) noexcept {
        return lhs.compare(rhs) < 0;
    }

    // Less-than-or-equal operator
    friend bool operator<=(ByteSpan lhs, ByteSpan rhs) noexcept {
        return !(rhs < lhs);
    }

    // Greater-than operator
    friend bool operator>(ByteSpan lhs, ByteSpan rhs) noexcept {
        return rhs < lhs;
    }

    // Greater-than-or-equal operator
    friend bool operator>=(ByteSpan lhs, ByteSpan rhs) noexcept {
        return !(lhs < rhs);
    }
};

// Hash support (for use in hash-based containers)
struct ByteSpanHash {
    usize operator()(ByteSpan view) const noexcept {
        // Simple hash function
        usize hash = 0;
        for (ByteSpan::size_type i = 0; i < view.size(); ++i) {
            hash = hash * 31 + static_cast<usize>(view[i]);
        }
        return hash;
    }
};

NEX_NAMESPACE_END

// Hash specialization for ByteSpan
NEX_STD_BEGIN

template<>
struct hash<NEX_PREPEND_NAMESPACE(ByteSpan)> {
    size_t operator()(NEX_PREPEND_NAMESPACE(ByteSpan) view) const noexcept {
        return NEX_PREPEND_NAMESPACE(ByteSpanHash){}(view);
    }
};

NEX_STD_END
