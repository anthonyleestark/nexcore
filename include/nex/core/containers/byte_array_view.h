/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <limits>
#include <iterator>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/linear.h"
#include "nex/base/string.h"

NEX_CORE_NAMESPACE_BEGIN

// Forward declaration of ByteArray for conversion constructor
class ByteArray;

/**
 * @class ByteArrayView
 * @brief Non-owning view into binary data (bytes), similar to std::span<uint8>
 * 
 * This class provides a lightweight, non-owning view into binary data.
 * It stores a pointer to the data and its length, without owning the data.
 * 
 * ByteArrayView supports:
 * - Construction from ByteArray, ArrayList<uint8>, uint8*, and other data sources
 * - Subarray operations (substr, left, right, mid)
 * - Byte access and iteration
 * - Comparison operations
 * - Conversion to ByteArray when needed
 * 
 * @note ByteArrayView does not own the underlying data. The user must ensure
 *       that the data remains valid for the lifetime of the ByteArrayView.
 * 
 * @warning Modifying the underlying data through a ByteArrayView is not allowed and may lead to undefined behavior. 
 *          Use ByteArray for mutable byte arrays.
 * 
 * @see ByteArray for an owning byte array class that supports mutation and ownership semantics.
 * @see Span<uint8> for a standard library alternative, but note that std::span is not available in C++17 
 *      and may not be suitable for all use cases.
 */
class NEX_EXPORT ByteArrayView {
public:
    // Type aliases
    using value_type = uint8;
    using pointer = const_byte_ptr;
    using const_pointer = const_byte_ptr;
    using reference = const uint8&;
    using const_reference = const uint8&;
    using iterator = const_byte_ptr;
    using const_iterator = const_byte_ptr;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = usize;
    using difference_type = isize;

private:
    // Pointer to the data (non-owning)
    const_byte_ptr data_;

    // Length of the view
    usize size_;

public:
    // Special value representing "not found" for search operations
    static constexpr size_type npos = static_cast<size_type>(-1);

public:
    ////// Constructors -----------------------------

    // Default constructor (empty view)
    constexpr ByteArrayView() noexcept : data_(nullptr), size_(0) {}
    
    // Construct from pointer and length
    constexpr ByteArrayView(const_byte_ptr data, usize size) noexcept
        : data_(data), size_(size) {}
    
    // Construct from pointer and length (unsigned char)
    constexpr ByteArrayView(const unsigned char* data, usize size) noexcept
        : data_(reinterpret_cast<const_byte_ptr>(data)), size_(size) {}
    
    // Construct from void pointer and length
    constexpr ByteArrayView(const_void_ptr data, usize size) noexcept
        : data_(static_cast<const_byte_ptr>(data)), size_(size) {}
    
    // Construct from ArrayList<uint8>
    constexpr ByteArrayView(const ArrayList<uint8>& vec) noexcept
        : data_(vec.data()), size_(vec.size()) {}
    
    // Construct from ByteArray
    ByteArrayView(const ByteArray& arr) noexcept;
    
    // Copy constructor
    constexpr ByteArrayView(const ByteArrayView& other) noexcept = default;
    
    // Copy assignment operator
    ByteArrayView& operator=(const ByteArrayView& other) noexcept = default;
    
    // Destructor
    ~ByteArrayView() = default;

    ////// Iterator support -----------------------------

    // Get iterator to the beginning of the view
    constexpr const_iterator begin() const noexcept { return data_; }

    // Get const iterator to the beginning of the view
    constexpr const_iterator cbegin() const noexcept { return data_; }

    // Get iterator to the end of the view
    constexpr const_iterator end() const noexcept { return data_ + size_; }

    // Get const iterator to the end of the view
    constexpr const_iterator cend() const noexcept { return data_ + size_; }
    
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

    ////// Element access -----------------------------

    // Access byte at index (with bounds checking)
    const_reference at(usize pos) const;
    
    // Access byte at index (no bounds checking)
    constexpr const_reference operator[](usize pos) const noexcept {
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
    
    // Get maximum possible size
    constexpr size_type maxSize() const noexcept {
        return (std::numeric_limits<size_type>::max() / sizeof(value_type)) - 1;
    }
    
    // Check if view is empty
    constexpr bool empty() const noexcept { return size_ == 0; }

    ////// Modifiers -----------------------------

    // Remove prefix (first n bytes)
    void removePrefix(usize n) noexcept;
    
    // Remove suffix (last n bytes)
    void removeSuffix(usize n) noexcept;
    
    // Swap with another view
    void swap(ByteArrayView& other) noexcept;

    ////// Operations -----------------------------

    // Copy bytes to destination
    size_type copy(byte_ptr dest, size_type count, size_type pos = 0) const;
    
    // Get subview
    ByteArrayView substr(size_type pos = 0, size_type count = npos) const;
    
    // Get left part of the view
    ByteArrayView left(size_type count) const noexcept;
    
    // Get right part of the view
    ByteArrayView right(size_type count) const noexcept;
    
    // Get middle part of the view
    ByteArrayView mid(size_type start, size_type count = npos) const;
    
    // Compare with another view
    int32 compare(const ByteArrayView& other) const noexcept;
    
    // Compare substring with another view
    int32 compare(size_type pos, size_type count, const ByteArrayView& other) const {
        return substr(pos, count).compare(other);
    }
    
    // Find first occurrence of byte
    size_type indexOf(uint8 byte, size_type pos = 0) const noexcept;
    
    // Find first occurrence of subview
    size_type indexOf(const ByteArrayView& other, size_type pos = 0) const noexcept;
    
    // Find last occurrence of byte
    size_type lastIndexOf(uint8 byte, size_type pos = npos) const noexcept;
    
    // Find last occurrence of subview
    size_type lastIndexOf(ByteArrayView other, size_type pos = npos) const noexcept;
    
    // Find first occurrence of any byte in the set
    size_type findFirstOf(ByteArrayView other, size_type pos = 0) const noexcept;
    
    // Find first occurrence of any byte not in the set
    size_type findFirstNotOf(ByteArrayView other, size_type pos = 0) const noexcept;
    
    // Find last occurrence of any byte in the set
    size_type findLastOf(ByteArrayView other, size_type pos = npos) const noexcept;
    
    // Find last occurrence of any byte not in the set
    size_type findLastNotOf(ByteArrayView other, size_type pos = npos) const noexcept;
    
    // Check if view starts with prefix
    bool startsWith(ByteArrayView prefix) const noexcept;
    
    // Check if view starts with byte
    bool startsWith(uint8 byte) const noexcept;
    
    // Check if view ends with suffix
    bool endsWith(ByteArrayView suffix) const noexcept;
    
    // Check if view ends with byte
    bool endsWith(uint8 byte) const noexcept;
    
    // Check if view contains subview
    bool contains(ByteArrayView other) const noexcept;
    
    // Check if view contains byte
    bool contains(uint8 byte) const noexcept;
    
    // Count occurrences of byte
    size_type count(uint8 byte) const noexcept;

    ////// Conversion methods -----------------------------

    // Convert to ByteArray (creates a copy)
    ByteArray toByteArray() const;
    
    // Convert to ArrayList<uint8>
    ArrayList<uint8> toArrayList() const {
        return ArrayList<uint8>(data_, data_ + size_);
    }

    ////// Comparison operators -----------------------------

    // Equality operator
    friend bool operator==(ByteArrayView lhs, ByteArrayView rhs) noexcept {
        return lhs.compare(rhs) == 0;
    }
    
    // Inequality operator
    friend bool operator!=(ByteArrayView lhs, ByteArrayView rhs) noexcept {
        return !(lhs == rhs);
    }
    
    // Less-than operator
    friend bool operator<(ByteArrayView lhs, ByteArrayView rhs) noexcept {
        return lhs.compare(rhs) < 0;
    }
    
    // Less-than-or-equal operator
    friend bool operator<=(ByteArrayView lhs, ByteArrayView rhs) noexcept {
        return !(rhs < lhs);
    }
    
    // Greater-than operator
    friend bool operator>(ByteArrayView lhs, ByteArrayView rhs) noexcept {
        return rhs < lhs;
    }
    
    // Greater-than-or-equal operator
    friend bool operator>=(ByteArrayView lhs, ByteArrayView rhs) noexcept {
        return !(lhs < rhs);
    }
};

// Hash support (for use in unordered containers)
struct ByteArrayViewHash {
    usize operator()(ByteArrayView view) const noexcept {
        // Simple hash function
        usize hash = 0;
        for (usize i = 0; i < view.size(); ++i) {
            hash = hash * 31 + static_cast<usize>(view[i]);
        }
        return hash;
    }
};

NEX_CORE_NAMESPACE_END

// Hash specialization for ByteArrayView
NEX_STD_BEGIN

template<>
struct hash<NEX_PREPEND_CORE_NAMESPACE(ByteArrayView)> {
    size_t operator()(NEX_PREPEND_CORE_NAMESPACE(ByteArrayView) view) const noexcept {
        return NEX_PREPEND_CORE_NAMESPACE(ByteArrayViewHash){}(view);
    }
};

NEX_STD_END
