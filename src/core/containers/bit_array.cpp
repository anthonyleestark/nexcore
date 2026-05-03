/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <algorithm>
#include <utility>

#include "nex/core/containers/bit_array.h"
#include "nex/base/assert_crash.h"

NEX_CORE_NAMESPACE_BEGIN

////// Helper functions for bit manipulation ------------------------

// Clear unused bits in the last byte
void BitArray::clearUnusedBits() noexcept {
    if (bitCount_ > 0) {
        size_type lastByteIndex = byteIndex(bitCount_ - 1);
        size_type lastBitOffset = bitOffset(bitCount_ - 1);
        if (lastBitOffset < bitsPerByte() - 1) {
            // Clear bits beyond the last valid bit
            block_type mask = static_cast<block_type>((1U << (lastBitOffset + 1)) - 1);
            buffer_[lastByteIndex] &= mask;
        }
    }
}

////// Constructors --------------------------------------------------

// Construct from size (all bits initialized to false)
BitArray::BitArray(size_type size) : bitCount_(size) {
    if (size > 0) {
        buffer_.resize(bufferSizeForBits(size), 0);
    }
}

// Construct from size and fill value
BitArray::BitArray(size_type size, value_type fillValue) : bitCount_(size) {
    if (size > 0) {
        block_type fillByte = fillValue ? 0xFF : 0x00;
        buffer_.resize(bufferSizeForBits(size), fillByte);
        clearUnusedBits();
    }
}

// Copy assignment operator
BitArray& BitArray::operator=(const BitArray& other) {
    if (this != &other) {
        buffer_ = other.buffer_;
        bitCount_ = other.bitCount_;
    }
    return *this;
}

// Move constructor
BitArray::BitArray(BitArray&& other) noexcept 
    : buffer_(NEX_STD move(other.buffer_)), bitCount_(other.bitCount_) {
    other.bitCount_ = 0;
}

// Move assignment operator
BitArray& BitArray::operator=(BitArray&& other) noexcept {
    if (this != &other) {
        buffer_ = NEX_STD move(other.buffer_);
        bitCount_ = other.bitCount_;
        other.bitCount_ = 0;
    }
    return *this;
}

////// Capacity --------------------------------------------------

// Resize the buffer with fill value
void BitArray::resize(size_type newSize, value_type fillValue) {
    size_type oldSize = bitCount_;
    bitCount_ = newSize;
    
    if (newSize == 0) {
        buffer_.clear();
        return;
    }
    
    size_type requiredBytes = bufferSizeForBits(newSize);
    size_type currentBytes = buffer_.size();
    
    if (requiredBytes > currentBytes) {
        // Need to grow
        block_type fillByte = fillValue ? 0xFF : 0x00;
        buffer_.resize(requiredBytes, fillByte);
    } else if (requiredBytes < currentBytes) {
        // Need to shrink
        buffer_.resize(requiredBytes);
    }
    
    // If we're extending, fill new bits
    if (newSize > oldSize && fillValue) {
        // New bits are already filled by resize, but we need to handle
        // the case where we're extending within the same byte
        size_type oldByteCount = bufferSizeForBits(oldSize);
        if (oldByteCount == requiredBytes && oldSize > 0) {
            // Extending within the same byte
            size_type startBit = oldSize;
            size_type endBit = newSize;
            for (size_type i = startBit; i < endBit; ++i) {
                setBit(i);
            }
        }
    }
    
    clearUnusedBits();
}

// Clear the buffer
void BitArray::clear() noexcept {
    buffer_.clear();
    bitCount_ = 0;
}

////// Conversion and serialization ------------------------------

// Create BitArray from Vector of booleans (ArrayList<bool>)
BitArray BitArray::fromVector(const ArrayList<value_type>& vec) noexcept {
    BitArray result(vec.size());
    for (size_type i = 0; i < vec.size(); ++i) {
        result.setBit(i, vec[i]);
    }
    return result;
}

// Convert to Vector of booleans (ArrayList<bool>)
ArrayList<BitArray::value_type> BitArray::toVector() const noexcept {
    ArrayList<value_type> result(bitCount_);
    for (size_type i = 0; i < bitCount_; ++i) {
        result[i] = testBit(i);
    }
    return result;
}

////// Element access --------------------------------------------------

// Test bit at index (returns true if set, false if clear)
bool BitArray::testBit(size_type index) const noexcept {
    if (index >= bitCount_) return false;
    size_type byteIdx = byteIndex(index);
    size_type bitOff = bitOffset(index);
    return (buffer_[byteIdx] & (1U << bitOff)) != 0;
}

// Set bit at index to true
void BitArray::setBit(size_type index) noexcept {
    if (index >= bitCount_) return;
    size_type byteIdx = byteIndex(index);
    size_type bitOff = bitOffset(index);
    buffer_[byteIdx] |= (1U << bitOff);
}

// Set bit at index to specified value
void BitArray::setBit(size_type index, value_type value) noexcept {
    if (value) {
        setBit(index);
    } else {
        clearBit(index);
    }
}

// Clear bit at index (set to false)
void BitArray::clearBit(size_type index) noexcept {
    if (index >= bitCount_) return;
    size_type byteIdx = byteIndex(index);
    size_type bitOff = bitOffset(index);
    buffer_[byteIdx] &= ~(1U << bitOff);
}

// Toggle bit at index
void BitArray::toggleBit(size_type index) noexcept {
    if (index >= bitCount_) return;
    size_type byteIdx = byteIndex(index);
    size_type bitOff = bitOffset(index);
    buffer_[byteIdx] ^= (1U << bitOff);
}

// Access bit at index (with bounds checking)
BitArray::const_reference BitArray::at(size_type index) const {
    NEX_ASSERT_MSG(index >= 0 && index < bitCount_, "Index out of range");
    return testBit(index);
}

////// Modifiers --------------------------------------------------

// Fill all bits with value
BitArray& BitArray::fill(value_type value) {
    block_type fillByte = value ? 0xFF : 0x00;
    NEX_STD fill(buffer_.begin(), buffer_.end(), fillByte);
    clearUnusedBits();
    return *this;
}

// Fill bits in range with value
BitArray& BitArray::fill(value_type value, size_type start, size_type count) {
    if (start >= bitCount_) return *this;
    size_type end = NEX_STD min(start + count, bitCount_);
    for (size_type i = start; i < end; ++i) {
        setBit(i, value);
    }
    return *this;
}

////// Bitwise operations --------------------------------------------------

// Bitwise AND
BitArray& BitArray::operator&=(const BitArray& other) {
    size_type minSize = NEX_STD min(bitCount_, other.bitCount_);
    size_type minBytes = NEX_STD min(buffer_.size(), other.buffer_.size());
    
    for (size_type i = 0; i < minBytes; ++i) {
        buffer_[i] &= other.buffer_[i];
    }
    
    // Clear remaining bytes if this buffer is larger
    if (buffer_.size() > minBytes) {
        NEX_STD fill(buffer_.begin() + minBytes, buffer_.end(), static_cast<block_type>(0));
    }
    
    // Adjust bit count to minimum
    bitCount_ = minSize;
    clearUnusedBits();
    return *this;
}

// Bitwise OR
BitArray& BitArray::operator|=(const BitArray& other) {
    if (other.bitCount_ > bitCount_) {
        resize(other.bitCount_, false);
    }
    
    size_type minBytes = NEX_STD min(buffer_.size(), other.buffer_.size());
    for (size_type i = 0; i < minBytes; ++i) {
        buffer_[i] |= other.buffer_[i];
    }
    
    clearUnusedBits();
    return *this;
}

// Bitwise XOR
BitArray& BitArray::operator^=(const BitArray& other) {
    if (other.bitCount_ > bitCount_) {
        resize(other.bitCount_, false);
    }
    
    size_type minBytes = NEX_STD min(buffer_.size(), other.buffer_.size());
    for (size_type i = 0; i < minBytes; ++i) {
        buffer_[i] ^= other.buffer_[i];
    }
    
    clearUnusedBits();
    return *this;
}

// Bitwise NOT (invert all bits)
BitArray BitArray::operator~() const {
    BitArray result = *this;
    for (size_type i = 0; i < result.buffer_.size(); ++i) {
        result.buffer_[i] = ~result.buffer_[i];
    }
    result.clearUnusedBits();
    return result;
}

// Non-member bitwise operators
BitArray operator&(const BitArray& a, const BitArray& b) {
    BitArray result = a;
    result &= b;
    return result;
}

BitArray operator|(const BitArray& a, const BitArray& b) {
    BitArray result = a;
    result |= b;
    return result;
}

BitArray operator^(const BitArray& a, const BitArray& b) {
    BitArray result = a;
    result ^= b;
    return result;
}


////// Sub-array operations --------------------------------------------------

// Get middle part of the buffer
BitArray BitArray::mid(size_type start, size_type count /* = npos */) const noexcept {
    if (start >= bitCount_) return BitArray();
    size_type maxCount = bitCount_ - start;
    size_type actualCount = (count == npos) 
                            ? maxCount : NEX_STD min(count, maxCount);
    
    BitArray result(actualCount);
    for (size_type i = 0; i < actualCount; ++i) {
        result.setBit(i, testBit(start + i));
    }
    return result;
}

////// Search operations --------------------------------------------------

// Count number of bits set to true
BitArray::size_type BitArray::countTrue() const noexcept {
    size_type count = 0;
    for (size_type i = 0; i < bitCount_; ++i) {
        if (testBit(i)) {
            ++count;
        }
    }
    return count;
}

// Count number of bits set to false
BitArray::size_type BitArray::countFalse() const noexcept {
    return bitCount_ - countTrue();
}

// Check if any bit is set
bool BitArray::any() const noexcept {
    for (size_type i = 0; i < buffer_.size(); ++i) {
        if (buffer_[i] != 0) {
            return true;
        }
    }
    return false;
}

// Check if all bits are set
bool BitArray::all() const noexcept {
    if (bitCount_ == 0) return true;
    for (size_type i = 0; i < bitCount_; ++i) {
        if (!testBit(i)) {
            return false;
        }
    }
    return true;
}

////// Comparison operations --------------------------------------------------

// Compare with another BitArray
int32 BitArray::compare(const BitArray& other) const noexcept {
    size_type minSize = NEX_STD min(bitCount_, other.bitCount_);
    
    // Compare common bits
    for (size_type i = 0; i < minSize; ++i) {
        bool thisBit = testBit(i);
        bool otherBit = other.testBit(i);
        if (thisBit != otherBit) {
            return thisBit ? 1 : -1;
        }
    }
    
    // If sizes differ, the shorter one is "less"
    if (bitCount_ < other.bitCount_) return -1;
    if (bitCount_ > other.bitCount_) return 1;
    return 0;
}

// Comparison operators
bool BitArray::operator==(const BitArray& other) const noexcept {
    if (bitCount_ != other.bitCount_) return false;
    if (bitCount_ == 0) return true;
    
    // Compare bytes (except last byte which may have unused bits)
    size_type fullBytes = bitCount_ / bitsPerByte();
    if (fullBytes > 0) {
        if (NEX_STD memcmp(buffer_.data(), other.buffer_.data(), fullBytes) != 0) {
            return false;
        }
    }
    
    // Compare remaining bits in the last byte
    size_type remainingBits = bitCount_ % bitsPerByte();
    if (remainingBits > 0) {
        size_type lastByteIndex = byteIndex(bitCount_ - 1);
        block_type mask = static_cast<block_type>((1U << remainingBits) - 1);
        if ((buffer_[lastByteIndex] & mask) != (other.buffer_[lastByteIndex] & mask)) {
            return false;
        }
    }
    
    return true;
}

NEX_CORE_NAMESPACE_END
