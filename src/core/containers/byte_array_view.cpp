/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <algorithm>

#include "nex/core/containers/byte_array_view.h"
#include "nex/core/containers/byte_array.h"
#include "nex/base/assert_crash.h"

NEX_CORE_NAMESPACE_BEGIN

// Constructor from ByteArray
ByteArrayView::ByteArrayView(const ByteArray& arr) noexcept
    : data_(arr.data()), size_(arr.size()) {}

// Access byte at index (with bounds checking)
ByteArrayView::const_reference ByteArrayView::at(size_type pos) const {
    NEX_ASSERT_MSG(pos >= 0 && pos < size_, "Index out of range");
    return data_[pos];
}

// Convert to ByteArray
ByteArray ByteArrayView::toByteArray() const {
    if (empty()) {
        return ByteArray();
    }
    return ByteArray(data_, size_);
}

////// Modifiers --------------------------------------------------

// Remove prefix (first n bytes)
void ByteArrayView::removePrefix(size_type n) noexcept {
    if (n > size_) n = size_;
    data_ += n;
    size_ -= n;
}

// Remove suffix (last n bytes)
void ByteArrayView::removeSuffix(size_type n) noexcept {
    if (n > size_) n = size_;
    size_ -= n;
}

// Swap with another view
void ByteArrayView::swap(ByteArrayView& other) noexcept {
    NEX_STD swap(data_, other.data_);
    NEX_STD swap(size_, other.size_);
}

////// Operations --------------------------------------------------

// Copy bytes to destination
ByteArrayView::size_type ByteArrayView::copy(pointer dest, size_type count, size_type pos /* = 0 */) const {
    NEX_ASSERT_MSG(pos >= 0 && pos <= size_, "Position out of range");
    size_type rlen = (NEX_STD min)(count, size_ - pos);
    NEX_STD memcpy(dest, data_ + pos, rlen);
    return rlen;
}

// Get subview
ByteArrayView ByteArrayView::substr(size_type pos /* = 0 */, size_type count /* = npos */) const {
    NEX_ASSERT_MSG(pos >= 0 && pos <= size_, "Position out of range");
    size_type rlen = (NEX_STD min)(count, size_ - pos);
    return ByteArrayView(data_ + pos, rlen);
}

// Get the left part of the view
ByteArrayView ByteArrayView::left(size_type count) const noexcept {
    if (count >= size_) return ByteArrayView(data_, size_);
    return ByteArrayView(data_, count);
}

// Get the right part of the view
ByteArrayView ByteArrayView::right(size_type count) const noexcept {
    if (count >= size_) return ByteArrayView(data_, size_);
    return ByteArrayView(data_ + size_ - count, count);
}

// Get the middle part of the view
ByteArrayView ByteArrayView::mid(size_type start, size_type count /* = npos */) const {
    if (start >= size_) return ByteArrayView();
    size_type maxCount = size_ - start;
    size_type rlen = (NEX_STD min)(count, maxCount);
    return ByteArrayView(data_ + start, rlen);
}

// Compare with another view
int32 ByteArrayView::compare(const ByteArrayView& other) const noexcept {
    size_type rlen = (NEX_STD min)(size_, other.size_);
    int32 result = NEX_STD memcmp(data_, other.data_, rlen);
    if (result != 0) return result;
    if (size_ < other.size_) return -1;
    if (size_ > other.size_) return 1;
    return 0;
}

// Find first occurrence of byte
ByteArrayView::size_type 
ByteArrayView::indexOf(value_type byte, ByteArrayView::size_type pos /* = 0 */) const noexcept {
    if (pos >= size_) return ByteArrayView::npos;
    const_byte_ptr result = static_cast<const_byte_ptr>(NEX_STD memchr(data_ + pos, byte, size_ - pos));
    return result ? static_cast<size_type>(result - data_) : ByteArrayView::npos;
}

// Find first occurrence of subview
ByteArrayView::size_type 
ByteArrayView::indexOf(const ByteArrayView& other, ByteArrayView::size_type pos /* = 0 */) const noexcept {
    if (pos > size_ || other.size() > size_ - pos) return ByteArrayView::npos;
    if (other.empty()) return pos;
    
    const_byte_ptr start = data_ + pos;
    const_byte_ptr end = data_ + size_ - other.size() + 1;
    
    for (const_byte_ptr p = start; p < end; ++p) {
        if (NEX_STD memcmp(p, other.data_, other.size_) == 0) {
            return static_cast<size_type>(p - data_);
        }
    }
    return ByteArrayView::npos;
}

// Find last occurrence of byte
ByteArrayView::size_type 
ByteArrayView::lastIndexOf(value_type byte, ByteArrayView::size_type pos /* = npos */) const noexcept {
    if (size_ == 0) return ByteArrayView::npos;
    if (pos >= size_) pos = size_ - 1;
    
    for (size_type i = pos + 1; i > 0; --i) {
        if (data_[i - 1] == byte) {
            return i - 1;
        }
    }
    return ByteArrayView::npos;
}

// Find last occurrence of subview
ByteArrayView::size_type 
ByteArrayView::lastIndexOf(ByteArrayView other, ByteArrayView::size_type pos /* = npos */) const noexcept {
    if (other.size() > size_) return ByteArrayView::npos;
    if (other.empty()) return (NEX_STD min)(pos, size_);

    size_type searchLen = size_ - other.size();
    if (pos > searchLen) pos = searchLen;

    for (size_type i = pos + 1; i > 0; --i) {
        size_type start = i - 1;
        if (NEX_STD memcmp(data_ + start, other.data_, other.size_) == 0) {
            return start;
        }
    }
    return ByteArrayView::npos;
}

// Find first occurrence of any byte in the set
ByteArrayView::size_type 
ByteArrayView::findFirstOf(ByteArrayView other, ByteArrayView::size_type pos /* = 0 */) const noexcept {
    for (size_type i = pos; i < size_; ++i) {
        if (other.indexOf(data_[i]) != ByteArrayView::npos) {
            return i;
        }
    }
    return ByteArrayView::npos;
}

// Find first occurrence of any byte not in the set
ByteArrayView::size_type ByteArrayView::findFirstNotOf(ByteArrayView other, 
    ByteArrayView::size_type pos /* = 0 */) const noexcept {
    for (size_type i = pos; i < size_; ++i) {
        if (other.indexOf(data_[i]) == ByteArrayView::npos) {
            return i;
        }
    }
    return ByteArrayView::npos;
}

// Find last occurrence of any byte in the set
ByteArrayView::size_type 
ByteArrayView::findLastOf(ByteArrayView other, ByteArrayView::size_type pos /* = npos */) const noexcept {
    if (size_ == 0) return ByteArrayView::npos;
    if (pos >= size_) pos = size_ - 1;

    for (size_type i = pos + 1; i > 0; --i) {
        if (other.indexOf(data_[i - 1]) != ByteArrayView::npos) {
            return i - 1;
        }
    }
    return ByteArrayView::npos;
}

// Find last occurrence of any byte not in the set
ByteArrayView::size_type 
ByteArrayView::findLastNotOf(ByteArrayView other, ByteArrayView::size_type pos /* = npos */) const noexcept {
    if (size_ == 0) return ByteArrayView::npos;
    if (pos >= size_) pos = size_ - 1;

    for (size_type i = pos + 1; i > 0; --i) {
        if (other.indexOf(data_[i - 1]) == ByteArrayView::npos) {
            return i - 1;
        }
    }
    return ByteArrayView::npos;
}

// Check if view starts with prefix
bool ByteArrayView::startsWith(ByteArrayView prefix) const noexcept {
    return size_ >= prefix.size_ &&
            NEX_STD memcmp(data_, prefix.data_, prefix.size_) == 0;
}

// Check if view starts with byte
bool ByteArrayView::startsWith(value_type byte) const noexcept {
    return !empty() && front() == byte;
}

// Check if view ends with suffix
bool ByteArrayView::endsWith(ByteArrayView suffix) const noexcept {
    return size_ >= suffix.size_ &&
            NEX_STD memcmp(data_ + size_ - suffix.size_, suffix.data_, suffix.size_) == 0;
}

// Check if view ends with byte
bool ByteArrayView::endsWith(value_type byte) const noexcept {
    return !empty() && back() == byte;
}

// Check if view contains subview
bool ByteArrayView::contains(ByteArrayView other) const noexcept {
    return indexOf(other) != ByteArrayView::npos;
}

// Check if view contains byte
bool ByteArrayView::contains(value_type byte) const noexcept {
    return indexOf(byte) != ByteArrayView::npos;
}

// Count occurrences of byte
ByteArrayView::size_type ByteArrayView::count(value_type byte) const noexcept {
    size_type cnt = 0;
    for (size_type i = 0; i < size_; ++i) {
        if (data_[i] == byte) ++cnt;
    }
    return cnt;
}

NEX_CORE_NAMESPACE_END
