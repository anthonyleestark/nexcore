/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <algorithm>

#include "nex/core/view/bit_span.h"
#include "nex/base/assert_crash.h"

NEX_CORE_NAMESPACE_BEGIN

////// Bit accessors --------------------------------------------------

// Access bit at index (with bounds checking)
BitSpan::const_reference BitSpan::at(size_type index) const {
    NEX_ASSERT_MSG(index < bitCount_, "Index out of range");
    return testBit(index);
}

////// Conversion methods -----------------------------

// Convert to dynamic array of booleans (Vec<bool>)
Vec<BitSpan::value_type> BitSpan::toVec() const noexcept {
    Vec<value_type> vec;
    vec.reserve(bitCount_);
    for (size_type i = 0; i < bitCount_; ++i) {
        vec.push_back(testBit(i));
    }
    return vec;
}

////// Modifiers --------------------------------------------------

// Remove the first n bits from the view
void BitSpan::removePrefix(size_type n) noexcept {
    if (n > bitCount_) n = bitCount_;
    size_type absNew = bitOffset_ + n;
    data_ += absNew / bitsPerByte();
    bitOffset_ = absNew % bitsPerByte();
    bitCount_ -= n;
}

// Remove the last n bits from the view
void BitSpan::removeSuffix(size_type n) noexcept {
    if (n > bitCount_) n = bitCount_;
    bitCount_ -= n;
}

// Swap with another BitSpan
void BitSpan::swap(BitSpan& other) noexcept {
    NEX_STD swap(data_, other.data_);
    NEX_STD swap(bitOffset_, other.bitOffset_);
    NEX_STD swap(bitCount_, other.bitCount_);
}

////// Subspan operations -----------------------------

// Get a subview starting at pos with count bits
BitSpan BitSpan::subspan(size_type pos /* = 0 */, size_type count /* = npos */) const {
    NEX_ASSERT_MSG(pos <= bitCount_, "Position out of range");
    size_type rlen = (NEX_STD min)(count, bitCount_ - pos);
    if (rlen == 0) return BitSpan();
    size_type absStart = bitOffset_ + pos;
    return BitSpan(data_ + absStart / bitsPerByte(), absStart % bitsPerByte(), rlen);
}

// Get the left part of the view (first count bits)
BitSpan BitSpan::left(size_type count) const noexcept {
    if (count == 0) return BitSpan();
    if (count >= bitCount_) return *this;
    return BitSpan(data_, bitOffset_, count);
}

// Get the right part of the view (last count bits)
BitSpan BitSpan::right(size_type count) const noexcept {
    if (count == 0) return BitSpan();
    if (count >= bitCount_) return *this;
    size_type absStart = bitOffset_ + (bitCount_ - count);
    return BitSpan(data_ + absStart / bitsPerByte(), absStart % bitsPerByte(), count);
}

// Get the middle part of the view (count bits starting at start)
BitSpan BitSpan::mid(size_type start, size_type count /* = npos */) const {
    if (start >= bitCount_) return BitSpan();
    size_type maxCount = bitCount_ - start;
    size_type rlen = (NEX_STD min)(count, maxCount);
    size_type absStart = bitOffset_ + start;
    return BitSpan(data_ + absStart / bitsPerByte(), absStart % bitsPerByte(), rlen);
}

////// Search operations -----------------------------

// Find the first occurrence of a bit value starting at pos
BitSpan::size_type BitSpan::indexOf(value_type bit, size_type pos /* = 0 */) const noexcept {
    for (size_type i = pos; i < bitCount_; ++i) {
        if (testBit(i) == bit) return i;
    }
    return npos;
}

// Find the first occurrence of a bit pattern starting at pos
BitSpan::size_type BitSpan::indexOf(BitSpan pattern, size_type pos /* = 0 */) const noexcept {
    if (pattern.empty()) return pos <= bitCount_ ? pos : npos;
    if (pattern.size() > bitCount_ || pos > bitCount_ - pattern.size()) return npos;

    size_type limit = bitCount_ - pattern.size();
    for (size_type i = pos; i <= limit; ++i) {
        bool found = true;
        for (size_type j = 0; j < pattern.size(); ++j) {
            if (testBit(i + j) != pattern.testBit(j)) {
                found = false;
                break;
            }
        }
        if (found) return i;
    }
    return npos;
}

// Find the last occurrence of a bit value, searching backwards from pos
BitSpan::size_type BitSpan::lastIndexOf(value_type bit, size_type pos /* = npos */) const noexcept {
    if (bitCount_ == 0) return npos;
    if (pos >= bitCount_) pos = bitCount_ - 1;
    for (size_type i = pos + 1; i > 0; --i) {
        if (testBit(i - 1) == bit) return i - 1;
    }
    return npos;
}

// Find the last occurrence of a bit pattern, searching backwards from pos
BitSpan::size_type BitSpan::lastIndexOf(BitSpan pattern, size_type pos /* = npos */) const noexcept {
    if (pattern.empty()) return (NEX_STD min)(pos, bitCount_);
    if (pattern.size() > bitCount_) return npos;

    size_type searchLen = bitCount_ - pattern.size();
    if (pos > searchLen) pos = searchLen;

    for (size_type i = pos + 1; i > 0; --i) {
        size_type start = i - 1;
        bool found = true;
        for (size_type j = 0; j < pattern.size(); ++j) {
            if (testBit(start + j) != pattern.testBit(j)) {
                found = false;
                break;
            }
        }
        if (found) return start;
    }
    return npos;
}

// Check if the view starts with the given bit value
bool BitSpan::startsWith(value_type bit) const noexcept {
    return !empty() && testBit(0) == bit;
}

// Check if the view starts with the given bit pattern
bool BitSpan::startsWith(BitSpan prefix) const noexcept {
    if (prefix.size() > bitCount_) return false;
    for (size_type i = 0; i < prefix.size(); ++i) {
        if (testBit(i) != prefix.testBit(i)) return false;
    }
    return true;
}

// Check if the view ends with the given bit value
bool BitSpan::endsWith(value_type bit) const noexcept {
    return !empty() && testBit(bitCount_ - 1) == bit;
}

// Check if the view ends with the given bit pattern
bool BitSpan::endsWith(BitSpan suffix) const noexcept {
    if (suffix.size() > bitCount_) return false;
    size_type offset = bitCount_ - suffix.size();
    for (size_type i = 0; i < suffix.size(); ++i) {
        if (testBit(offset + i) != suffix.testBit(i)) return false;
    }
    return true;
}

// Check if the view contains the given bit value
bool BitSpan::contains(value_type bit) const noexcept {
    return indexOf(bit) != npos;
}

// Check if the view contains the given bit pattern
bool BitSpan::contains(BitSpan pattern) const noexcept {
    return indexOf(pattern) != npos;
}

// Count the number of bits equal to the given value
BitSpan::size_type BitSpan::count(value_type bit /* = true */) const noexcept {
    size_type result = 0;
    for (size_type i = 0; i < bitCount_; ++i) {
        if (testBit(i) == bit) ++result;
    }
    return result;
}

////// Boolean aggregate queries -----------------------------

// Check if any bit is set to true
bool BitSpan::any() const noexcept {
    return indexOf(true) != npos;
}

// Check if all bits are set to true
bool BitSpan::all() const noexcept {
    return indexOf(false) == npos;
}

////// Comparison methods -----------------------------

// Lexicographic comparison with another BitSpan (bit by bit)
int32 BitSpan::compare(const BitSpan& other) const noexcept {
    size_type minSize = (NEX_STD min)(bitCount_, other.bitCount_);
    for (size_type i = 0; i < minSize; ++i) {
        bool a = testBit(i);
        bool b = other.testBit(i);
        if (a != b) return a ? 1 : -1;
    }
    if (bitCount_ < other.bitCount_) return -1;
    if (bitCount_ > other.bitCount_) return 1;
    return 0;
}

NEX_CORE_NAMESPACE_END
