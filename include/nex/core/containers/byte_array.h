/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <limits>
#include <algorithm>
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
private:
    // Internal buffer to store bytes
    ArrayList<uint8> buffer_;

public:
    ////// Constructors and assignment operators -----------------------------

    // Default constructor
    explicit ByteArray() = default;

    // Construct from size (filled with zeros)
    explicit ByteArray(usize size) : buffer_(size, 0) {}

    // Construct from size and fill value
    explicit ByteArray(usize size, uint8 fillValue) : buffer_(size, fillValue) {}

    // Construct from C-style array
    ByteArray(const_byte_ptr data, usize size);

    // Construct from ArrayList<uint8>
    explicit ByteArray(const ArrayList<uint8>& data) : buffer_(data) {}

    // Construct from StdString (treat as binary data)
    explicit ByteArray(const StdString& str);

    // Construct from C-string (treat as binary data)
    explicit ByteArray(const_char_ptr str, usize len);

    // Copy constructor
    ByteArray(const ByteArray& other) : buffer_(other.buffer_) {}

    // Copy assignment operator
    ByteArray& operator=(const ByteArray& other);

    // Move constructor
    ByteArray(ByteArray&& other) noexcept;

    // Move assignment operator
    ByteArray& operator=(ByteArray&& other) noexcept;

    // Destructor
    ~ByteArray() = default;

    ////// Factory methods -----------------------------

    // Create from raw data
    static ByteArray fromRawData(const_void_ptr data, usize size);

    // Create from HEX string
    static ByteArray fromHex(const StdString& hexString);

    // Create from Base64 string
    static ByteArray fromBase64(const StdString& base64String);

    // Create from ArrayList<uint8>
    static ByteArray fromArrayList(const ArrayList<uint8>& vec);

    ////// Conversion methods -----------------------------

    // Convert to ArrayList<uint8>
    ArrayList<uint8> toArrayList() const noexcept {
        return buffer_;
    }

    // Convert to StdString (treat as binary data)
    StdString toStdString() const;

    // Convert to HEX string
    StdString toHex() const;

    // Convert to Base64 string
    StdString toBase64() const;

    // Get raw data pointer
    const_byte_ptr data() const noexcept {
        return buffer_.data();
    }

    // Get raw data pointer (non-const)
    byte_ptr data() noexcept {
        return buffer_.data();
    }

    // Get raw data pointer as void*
    const_void_ptr rawData() const noexcept {
        return static_cast<const_void_ptr>(buffer_.data());
    }

    // Get raw data pointer as void* (non-const)
    void_ptr rawData() noexcept {
        return static_cast<void_ptr>(buffer_.data());
    }

    ////// Capacity and size-related operations -----------------------------

    // Get size of the array
    constexpr usize size() const noexcept {
        return buffer_.size();
    }

    // Get length (same as size)
    constexpr usize length() const noexcept {
        return buffer_.size();
    }

    // Check if array is empty
    constexpr bool empty() const noexcept {
        return buffer_.empty();
    }

    // Reserve capacity
    void reserve(usize capacity) {
        buffer_.reserve(capacity);
    }

    // Get capacity
    constexpr usize capacity() const noexcept {
        return buffer_.capacity();
    }

    // Resize the array
    void resize(usize newSize) {
        buffer_.resize(newSize);
    }

    // Resize the array with fill value
    void resize(usize newSize, uint8 fillValue) {
        buffer_.resize(newSize, fillValue);
    }

    // Clear the array
    void clear() noexcept {
        buffer_.clear();
    }

    ////// Element access -----------------------------

    // Access byte at index (with bounds checking)
    uint8& at(usize index) {
        return buffer_.at(index);
    }

    // Access byte at index (with bounds checking, read-only)
    const uint8& at(usize index) const {
        return buffer_.at(index);
    }

    // Access byte at index (no bounds checking)
    uint8& operator[](usize index) noexcept {
        return buffer_[index];
    }

    // Access byte at index (no bounds checking, read-only)
    const uint8& operator[](usize index) const noexcept {
        return buffer_[index];
    }

    // Get byte at index
    uint8 getAt(usize index) const {
        if (index >= buffer_.size()) return 0;
        return buffer_[index];
    }

    // Set byte at index
    void setAt(usize index, uint8 value) noexcept {
        if (index < buffer_.size()) {
            buffer_[index] = value;
        }
    }

    // Get the first byte
    uint8 front() const noexcept {
        return buffer_.empty() ? 0 : buffer_.front();
    }

    // Get the last byte
    uint8 back() const noexcept {
        return buffer_.empty() ? 0 : buffer_.back();
    }

    ////// Modifiers -----------------------------

    // Append another ByteArray
    ByteArray& append(const ByteArray& other);

    // Append raw data
    ByteArray& append(const_byte_ptr data, usize size);

    // Append single byte
    ByteArray& appendByte(uint8 byte) {
        buffer_.push_back(byte);
        return *this;
    }

    // Append single byte (alias)
    ByteArray& pushBack(uint8 byte) {
        buffer_.push_back(byte);
        return *this;
    }

    // Prepend another ByteArray
    ByteArray& prepend(const ByteArray& other);

    // Prepend raw data
    ByteArray& prepend(const_byte_ptr data, usize size);

    // Remove all occurrences of a byte
    ByteArray& remove(uint8 byte);

    // Remove bytes at position
    int32 removeAt(usize pos, usize count = 1);

    // Insert bytes at position
    ByteArray& insert(usize pos, const ByteArray& other);
    ByteArray& insert(usize pos, const_byte_ptr data, usize size);
    ByteArray& insert(usize pos, uint8 byte);

    // Replace bytes
    ByteArray& replace(usize pos, usize count, const ByteArray& other);
    ByteArray& replace(usize pos, usize count, const_byte_ptr data, usize size);

    ////// Subarray operations -----------------------------

    // Get the left part of the array
    ByteArray left(usize count) const;

    // Get the right part of the array
    ByteArray right(usize count) const;

    // Get the middle part of the array
    ByteArray mid(usize start, usize count = NEX_STD numeric_limits<usize>::max()) const;

    ////// Search operations -----------------------------

    // Find first occurrence of byte
    usize indexOf(uint8 byte, usize from = 0) const;

    // Find last occurrence of byte
    usize lastIndexOf(uint8 byte, usize from = NEX_STD numeric_limits<usize>::max()) const;

    // Find first occurrence of subarray
    usize indexOf(const ByteArray& other, usize from = 0) const;

    // Check if array contains byte
    bool contains(uint8 byte) const;

    // Check if array contains subarray
    bool contains(const ByteArray& other) const;

    // Count occurrences of byte
    usize count(uint8 byte) const;

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
    ByteArray& fill(uint8 value);

    // Fill array with value in range
    ByteArray& fill(uint8 value, usize start, usize count);

    // Reverse the array
    ByteArray& reverse();

    // Swap with another ByteArray
    void swap(ByteArray& other) noexcept {
        buffer_.swap(other.buffer_);
    }

    // Truncate to new length
    ByteArray& truncate(usize newLength) {
        if (newLength < buffer_.size()) {
            buffer_.resize(newLength);
        }
        return *this;
    }
};

NEX_CORE_NAMESPACE_END
