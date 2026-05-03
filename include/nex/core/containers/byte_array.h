/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/linear.h"
#include "nex/base/string.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @class ByteArray
 * @brief Container class for binary data (bytes)
 * 
 * This class provides a convenient way to store and manipulate binary data.
 * It uses ArrayList<uint8> as the internal buffer to store bytes.
 * 
 * ByteArray supports:
 * - Construction from various data sources (arrays, vectors, strings, etc.)
 * - Append, remove, and modify operations
 * - Subarray extraction (left, right, mid)
 * - Conversion to/from various formats
 * - Comparison and search operations
 * 
 * @note ByteArray owns its data and manages memory automatically. It provides mutable access to the underlying bytes.
 *       For non-owning views into byte data, consider using ByteArrayView.
 * 
 * @see ByteArrayView for a non-owning view into byte data that does not support mutation or ownership semantics.
 * @see ArrayList<uint8> for the underlying container used by ByteArray.
 */
class NEX_EXPORT ByteArray {
public:
    // Type aliases for compatibility with standard container conventions
    using value_type = uint8;
    using size_type = usize;
    using difference_type = isize;
    using allocator_type = typename ArrayList<value_type>::allocator_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = ArrayList<value_type>::iterator;
    using const_iterator = ArrayList<value_type>::const_iterator;
    using reverse_iterator = ArrayList<value_type>::reverse_iterator;
    using const_reverse_iterator = ArrayList<value_type>::const_reverse_iterator;

private:
    // Internal buffer to store bytes
    ArrayList<value_type> buffer_;

public:
    // Special value representing "not found" for search operations
    static constexpr size_type npos = NEX_STD numeric_limits<size_type>::max();

public:
    ////// Constructors and assignment operators -----------------------------

    // Default constructor
    explicit ByteArray() = default;

    // Construct from size (filled with zeros)
    explicit ByteArray(size_type size) 
        : buffer_(size, 0) {}

    // Construct from size and fill value
    explicit ByteArray(size_type size, value_type fillValue) 
        : buffer_(size, fillValue) {}

    // Construct from C-style array
    ByteArray(const_pointer data, size_type size);

    // Construct from ArrayList<uint8>
    explicit ByteArray(const ArrayList<value_type>& data) 
        : buffer_(data) {}

    // Construct from StdString (treat as binary data)
    explicit ByteArray(const StdString& str);

    // Construct from C-string (treat as binary data)
    explicit ByteArray(const_char_ptr str, size_type len);

    // Copy constructor
    ByteArray(const ByteArray& other) 
        : buffer_(other.buffer_) {}

    // Copy assignment operator
    ByteArray& operator=(const ByteArray& other) {
        if (this != &other)
            buffer_ = other.buffer_;
        return *this;
    }

    // Move constructor
    ByteArray(ByteArray&& other) noexcept 
        : buffer_(NEX_STD move(other.buffer_)) {}

    // Move assignment operator
    ByteArray& operator=(ByteArray&& other) noexcept {
        if (this != &other)
            buffer_ = NEX_STD move(other.buffer_);
        return *this;
    }

    // Destructor
    ~ByteArray() = default;

    ////// Factory methods -----------------------------

    // Create from raw data
    static ByteArray fromRawData(const_void_ptr data, size_type size);

    // Create from HEX string
    static ByteArray fromHex(const StdString& hexString);

    // Create from Base64 string
    static ByteArray fromBase64(const StdString& base64String);

    // Create from ArrayList<uint8>
    static ByteArray fromArrayList(const ArrayList<value_type>& vec);

    ////// Conversion methods -----------------------------

    // Get raw data pointer
    pointer data() noexcept {
        return buffer_.data();
    }

    // Get raw data pointer (read-only)
    const_pointer data() const noexcept {
        return buffer_.data();
    }

    // Get raw data pointer as void* (for interoperability with C APIs)
    void_ptr rawData() noexcept {
        return static_cast<void_ptr>(buffer_.data());
    }

    // Get raw data pointer as const void* (read-only, for interoperability with C APIs)
    const_void_ptr rawData() const noexcept {
        return static_cast<const_void_ptr>(buffer_.data());
    }

    // Convert to ArrayList<uint8>
    ArrayList<value_type> toArrayList() const noexcept { return buffer_; }

    // Convert to StdString (treat as binary data)
    StdString toStdString() const;

    // Convert to HEX string
    StdString toHex() const;

    // Convert to Base64 string
    StdString toBase64() const;

    ////// Iterator support -----------------------------

    // Get iterator to the beginning of the array
    iterator begin() noexcept {
        return buffer_.begin();
    }

    // Get const iterator to the beginning of the array
    const_iterator begin() const noexcept {
        return buffer_.begin();
    }

    // Get const iterator to the beginning of the array
    const_iterator cbegin() const noexcept {
        return buffer_.cbegin();
    }

    // Get iterator to the end of the array
    iterator end() noexcept {
        return buffer_.end();
    }

    // Get const iterator to the end of the array
    const_iterator end() const noexcept {
        return buffer_.end();
    }

    // Get const iterator to the end of the array
    const_iterator cend() const noexcept {
        return buffer_.cend();
    }

    // Get reverse iterator to the beginning of the reversed array
    reverse_iterator rbegin() noexcept {
        return buffer_.rbegin();
    }

    // Get const reverse iterator to the beginning of the reversed array
    const_reverse_iterator rbegin() const noexcept {
        return buffer_.rbegin();
    }

    // Get const reverse iterator to the beginning of the reversed array
    const_reverse_iterator crbegin() const noexcept {
        return buffer_.crbegin();
    }

    // Get reverse iterator to the end of the reversed array
    reverse_iterator rend() noexcept {
        return buffer_.rend();
    }

    // Get const reverse iterator to the end of the reversed array
    const_reverse_iterator rend() const noexcept {
        return buffer_.rend();
    }

    // Get const reverse iterator to the end of the reversed array
    const_reverse_iterator crend() const noexcept {
        return buffer_.crend();
    }

    ////// Capacity and size-related operations -----------------------------

    // Get size of the array
    size_type size() const noexcept {
        return buffer_.size();
    }

    // Get length (same as size)
    size_type length() const noexcept {
        return buffer_.size();
    }

    // Check if array is empty
    bool empty() const noexcept {
        return buffer_.empty();
    }

    // Get size in bytes
    size_type sizeBytes() const noexcept {
        return buffer_.size() * sizeof(value_type);
    }

    // Get maximum possible size
    size_type max_size() const noexcept {
        return buffer_.max_size();
    }

    // Reserve capacity
    void reserve(size_type capacity) {
        buffer_.reserve(capacity);
    }

    // Get capacity
    size_type capacity() const noexcept {
        return buffer_.capacity();
    }

    // Resize the array
    void resize(size_type newSize) {
        buffer_.resize(newSize);
    }

    // Resize the array with fill value
    void resize(size_type newSize, value_type fillValue) {
        buffer_.resize(newSize, fillValue);
    }

    // Clear the array
    void clear() noexcept {
        buffer_.clear();
    }

    ////// Element access -----------------------------

    // Get the first byte
    reference front() noexcept {
        return buffer_.front();
    }

    // Get the first byte (read-only)
    const_reference front() const noexcept {
        return buffer_.front();
    }

    // Get the last byte
    reference back() noexcept {
        return buffer_.back();
    }

    // Get the last byte (read-only)
    const_reference back() const noexcept {
        return buffer_.back();
    }

    // Access byte at index (with bounds checking)
    reference at(size_type index) {
        return buffer_.at(index);
    }

    // Access byte at index (with bounds checking, read-only)
    const_reference at(size_type index) const {
        return buffer_.at(index);
    }

    // Access byte at index (no bounds checking)
    reference operator[](size_type index) noexcept {
        return buffer_[index];
    }

    // Access byte at index (no bounds checking, read-only)
    const_reference operator[](size_type index) const noexcept {
        return buffer_[index];
    }

    ////// Modifiers -----------------------------

    // Append another ByteArray
    ByteArray& append(const ByteArray& other);

    // Append raw data
    ByteArray& append(const_pointer data, size_type size);

    // Append single byte
    ByteArray& appendByte(value_type byte) {
        buffer_.push_back(byte);
        return *this;
    }

    // Append single byte
    void pushBack(value_type byte) {
        buffer_.push_back(byte);
    }

    // Remove the last byte
    void popBack() {
        buffer_.pop_back();
    }

    // Prepend another ByteArray
    ByteArray& prepend(const ByteArray& other);

    // Prepend raw data
    ByteArray& prepend(const_pointer data, size_type size);

    // Remove all occurrences of a byte
    ByteArray& remove(value_type byte);

    // Remove bytes at position
    int32 removeAt(size_type pos, size_type count = 1);

    // Insert bytes at position (from another ByteArray)
    ByteArray& insert(size_type pos, const ByteArray& other);

    // Insert raw data at position
    ByteArray& insert(size_type pos, const_pointer data, size_type size);

    // Insert single byte at position
    ByteArray& insert(size_type pos, value_type byte);

    // Replace bytes at position with another ByteArray
    ByteArray& replace(size_type pos, size_type count, const ByteArray& other);

    // Replace bytes at position with raw data
    ByteArray& replace(size_type pos, size_type count, const_pointer data, size_type size);

    ////// Subarray operations -----------------------------

    // Get the left part of the array
    ByteArray left(size_type count) const;

    // Get the right part of the array
    ByteArray right(size_type count) const;

    // Get the middle part of the array
    ByteArray mid(size_type start, size_type count = npos) const;

    ////// Search operations -----------------------------

    // Find first occurrence of byte
    size_type indexOf(value_type byte, size_type from = 0) const;

    // Find last occurrence of byte
    size_type lastIndexOf(value_type byte, size_type from = npos) const;

    // Find first occurrence of subarray
    size_type indexOf(const ByteArray& other, size_type from = 0) const;

    // Check if array contains byte
    bool contains(value_type byte) const;

    // Check if array contains subarray
    bool contains(const ByteArray& other) const;

    // Count occurrences of byte
    size_type count(value_type byte) const;

    ////// Comparison ------------------------------

    // Compare with another ByteArray
    int32 compare(const ByteArray& other) const noexcept;

    // Equality operator
    bool operator==(const ByteArray& other) const noexcept;

    // Inequality operator
    bool operator!=(const ByteArray& other) const noexcept {
        return !(*this == other);
    }

    ////// Lexicographical comparison operators ------------------------------

    // Less-than operator
    bool operator<(const ByteArray& other) const noexcept {
        return compare(other) < 0;
    }

    // Less-than or equal to operator
    bool operator<=(const ByteArray& other) const noexcept {
        return compare(other) <= 0;
    }

    // Greater-than operator
    bool operator>(const ByteArray& other) const noexcept {
        return compare(other) > 0;
    }

    // Greater-than or equal to operator
    bool operator>=(const ByteArray& other) const noexcept {
        return compare(other) >= 0;
    }

    ////// Concatenation operators ------------------------------

    // Concatenation operator
    ByteArray operator+(const ByteArray& other) const;

    // Concatenation assignment operator
    ByteArray& operator+=(const ByteArray& other) {
        return append(other);
    }

    ////// Utility operations ------------------------------

    // Fill array with value
    ByteArray& fill(value_type value);

    // Fill array with value starting from position for count bytes
    ByteArray& fill(value_type value, size_type start, size_type count);

    // Reverse the array
    ByteArray& reverse();

    // Swap with another ByteArray
    void swap(ByteArray& other) noexcept {
        buffer_.swap(other.buffer_);
    }

    // Truncate to new length
    ByteArray& truncate(size_type newLength) {
        if (newLength < buffer_.size()) {
            buffer_.resize(newLength);
        }
        return *this;
    }
};

NEX_CORE_NAMESPACE_END
