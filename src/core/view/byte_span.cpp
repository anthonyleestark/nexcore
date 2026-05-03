/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <algorithm>

#include "nex/core/view/byte_span.h"
#include "nex/base/assert_crash.h"

NEX_CORE_NAMESPACE_BEGIN

////// Accessors --------------------------------------------------

// Access byte at index (with bounds checking)
ByteSpan::const_reference ByteSpan::at(size_type pos) const {
    NEX_ASSERT_MSG(pos >= 0 && pos < size_, "Index out of range");
    return data_[pos];
}

////// Conversion methods -----------------------------

// Convert to ArrayList<uint8>
ArrayList<ByteSpan::value_type> ByteSpan::toArrayList() const {
    if (empty()) {
        return ArrayList<value_type>();
    }
    return ArrayList<value_type>(data_, data_ + size_);
}

////// Modifiers --------------------------------------------------

// Remove prefix (first n bytes)
void ByteSpan::removePrefix(size_type n) noexcept {
    if (n > size_) n = size_;
    data_ += n;
    size_ -= n;
}

// Remove suffix (last n bytes)
void ByteSpan::removeSuffix(size_type n) noexcept {
    if (n > size_) n = size_;
    size_ -= n;
}

// Swap with another view
void ByteSpan::swap(ByteSpan& other) noexcept {
    NEX_STD swap(data_, other.data_);
    NEX_STD swap(size_, other.size_);
}

////// Subspan operations -----------------------------

// Copy bytes to destination
ByteSpan::size_type ByteSpan::copy(pointer dest, size_type count, size_type pos /* = 0 */) const {
    NEX_ASSERT_MSG(pos >= 0 && pos <= size_, "Position out of range");
    size_type rlen = (NEX_STD min)(count, size_ - pos);
    if (rlen == 0) return 0;
    NEX_STD memcpy(dest, data_ + pos, rlen);
    return rlen;
}

// Get subview
ByteSpan ByteSpan::subspan(size_type pos /* = 0 */, size_type count /* = npos */) const {
    NEX_ASSERT_MSG(pos >= 0 && pos <= size_, "Position out of range");
    size_type rlen = (NEX_STD min)(count, size_ - pos);
    if (rlen == 0) return ByteSpan();
    return ByteSpan(data_ + pos, rlen);
}

// Get the left part of the view
ByteSpan ByteSpan::left(size_type count) const noexcept {
    if (count >= size_) return ByteSpan(data_, size_);
    return ByteSpan(data_, count);
}

// Get the right part of the view
ByteSpan ByteSpan::right(size_type count) const noexcept {
    if (count >= size_) return ByteSpan(data_, size_);
    return ByteSpan(data_ + size_ - count, count);
}

// Get the middle part of the view
ByteSpan ByteSpan::mid(size_type start, size_type count /* = npos */) const {
    if (start >= size_) return ByteSpan();
    size_type maxCount = size_ - start;
    size_type rlen = (NEX_STD min)(count, maxCount);
    return ByteSpan(data_ + start, rlen);
}

////// Search operations -----------------------------

// Find first occurrence of byte
ByteSpan::size_type 
ByteSpan::indexOf(value_type byte, ByteSpan::size_type pos /* = 0 */) const noexcept {
    if (pos >= size_) return ByteSpan::npos;
    const_byte_ptr result = static_cast<const_byte_ptr>(NEX_STD memchr(data_ + pos, byte, size_ - pos));
    return result ? static_cast<size_type>(result - data_) : ByteSpan::npos;
}

// Find first occurrence of subview
ByteSpan::size_type 
ByteSpan::indexOf(const ByteSpan& other, ByteSpan::size_type pos /* = 0 */) const noexcept {
    if (pos > size_ || other.size() > size_ - pos) return ByteSpan::npos;
    if (other.empty()) return pos;
    
    const_byte_ptr start = data_ + pos;
    const_byte_ptr end = data_ + size_ - other.size() + 1;
    
    for (const_byte_ptr p = start; p < end; ++p) {
        if (NEX_STD memcmp(p, other.data_, other.size_) == 0) {
            return static_cast<size_type>(p - data_);
        }
    }
    return ByteSpan::npos;
}

// Find last occurrence of byte
ByteSpan::size_type 
ByteSpan::lastIndexOf(value_type byte, ByteSpan::size_type pos /* = npos */) const noexcept {
    if (size_ == 0) return ByteSpan::npos;
    if (pos >= size_) pos = size_ - 1;
    
    for (size_type i = pos + 1; i > 0; --i) {
        if (data_[i - 1] == byte) {
            return i - 1;
        }
    }
    return ByteSpan::npos;
}

// Find last occurrence of subview
ByteSpan::size_type 
ByteSpan::lastIndexOf(ByteSpan other, ByteSpan::size_type pos /* = npos */) const noexcept {
    if (other.size() > size_) return ByteSpan::npos;
    if (other.empty()) return (NEX_STD min)(pos, size_);

    size_type searchLen = size_ - other.size();
    if (pos > searchLen) pos = searchLen;

    for (size_type i = pos + 1; i > 0; --i) {
        size_type start = i - 1;
        if (NEX_STD memcmp(data_ + start, other.data_, other.size_) == 0) {
            return start;
        }
    }
    return ByteSpan::npos;
}

// Find first occurrence of any byte in the set
ByteSpan::size_type 
ByteSpan::findFirstOf(ByteSpan other, ByteSpan::size_type pos /* = 0 */) const noexcept {
    for (size_type i = pos; i < size_; ++i) {
        if (other.indexOf(data_[i]) != ByteSpan::npos) {
            return i;
        }
    }
    return ByteSpan::npos;
}

// Find first occurrence of any byte not in the set
ByteSpan::size_type ByteSpan::findFirstNotOf(ByteSpan other, 
    ByteSpan::size_type pos /* = 0 */) const noexcept {
    for (size_type i = pos; i < size_; ++i) {
        if (other.indexOf(data_[i]) == ByteSpan::npos) {
            return i;
        }
    }
    return ByteSpan::npos;
}

// Find last occurrence of any byte in the set
ByteSpan::size_type 
ByteSpan::findLastOf(ByteSpan other, ByteSpan::size_type pos /* = npos */) const noexcept {
    if (size_ == 0) return ByteSpan::npos;
    if (pos >= size_) pos = size_ - 1;

    for (size_type i = pos + 1; i > 0; --i) {
        if (other.indexOf(data_[i - 1]) != ByteSpan::npos) {
            return i - 1;
        }
    }
    return ByteSpan::npos;
}

// Find last occurrence of any byte not in the set
ByteSpan::size_type 
ByteSpan::findLastNotOf(ByteSpan other, ByteSpan::size_type pos /* = npos */) const noexcept {
    if (size_ == 0) return ByteSpan::npos;
    if (pos >= size_) pos = size_ - 1;

    for (size_type i = pos + 1; i > 0; --i) {
        if (other.indexOf(data_[i - 1]) == ByteSpan::npos) {
            return i - 1;
        }
    }
    return ByteSpan::npos;
}

// Check if view starts with prefix
bool ByteSpan::startsWith(ByteSpan prefix) const noexcept {
    if (prefix.empty()) return true;
    return size_ >= prefix.size_ &&
            NEX_STD memcmp(data_, prefix.data_, prefix.size_) == 0;
}

// Check if view starts with byte
bool ByteSpan::startsWith(value_type byte) const noexcept {
    return !empty() && front() == byte;
}

// Check if view ends with suffix
bool ByteSpan::endsWith(ByteSpan suffix) const noexcept {
    if (suffix.empty()) return true;
    return size_ >= suffix.size_ &&
            NEX_STD memcmp(data_ + size_ - suffix.size_, suffix.data_, suffix.size_) == 0;
}

// Check if view ends with byte
bool ByteSpan::endsWith(value_type byte) const noexcept {
    return !empty() && back() == byte;
}

// Check if view contains subview
bool ByteSpan::contains(ByteSpan other) const noexcept {
    return indexOf(other) != ByteSpan::npos;
}

// Check if view contains byte
bool ByteSpan::contains(value_type byte) const noexcept {
    return indexOf(byte) != ByteSpan::npos;
}

// Count occurrences of byte
ByteSpan::size_type ByteSpan::count(value_type byte) const noexcept {
    size_type cnt = 0;
    for (size_type i = 0; i < size_; ++i) {
        if (data_[i] == byte) ++cnt;
    }
    return cnt;
}

////// Comparison methods and operators -----------------------------

// Lexicographical compare with another ByteSpan (byte by byte)
int32 ByteSpan::compare(const ByteSpan& other) const noexcept {
    size_type rlen = (NEX_STD min)(size_, other.size_);
    if (rlen > 0) {
        int32 result = NEX_STD memcmp(data_, other.data_, rlen);
        if (result != 0) return result;
    }
    if (size_ < other.size_) return -1;
    if (size_ > other.size_) return 1;
    return 0;
}

NEX_CORE_NAMESPACE_END
