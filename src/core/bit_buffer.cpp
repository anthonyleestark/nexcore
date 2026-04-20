/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <algorithm>
#include <utility>

#include "nex/core/bit_buffer.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

////// Helper functions for bit manipulation ------------------------

// Clear unused bits in the last byte
void BitBuffer::clearUnusedBits() noexcept {
    if (bitCount_ > 0) {
        usize lastByteIndex = byteIndex(bitCount_ - 1);
        usize lastBitOffset = bitOffset(bitCount_ - 1);
        if (lastBitOffset < bitsPerByte() - 1) {
            // Clear bits beyond the last valid bit
            uint8 mask = static_cast<uint8>((1U << (lastBitOffset + 1)) - 1);
            buffer_[lastByteIndex] &= mask;
        }
    }
}

////// Constructors --------------------------------------------------

// Construct from size (all bits initialized to false)
BitBuffer::BitBuffer(usize size) : bitCount_(size) {
    if (size > 0) {
        buffer_.resize(bufferSizeForBits(size), 0);
    }
}

// Construct from size and fill value
BitBuffer::BitBuffer(usize size, bool fillValue) : bitCount_(size) {
    if (size > 0) {
        uint8 fillByte = fillValue ? 0xFF : 0x00;
        buffer_.resize(bufferSizeForBits(size), fillByte);
        clearUnusedBits();
    }
}

// Copy assignment operator
BitBuffer& BitBuffer::operator=(const BitBuffer& other) {
    if (this != &other) {
        buffer_ = other.buffer_;
        bitCount_ = other.bitCount_;
    }
    return *this;
}

// Move constructor
BitBuffer::BitBuffer(BitBuffer&& other) noexcept 
    : buffer_(NEX_STD move(other.buffer_)), bitCount_(other.bitCount_) {
    other.bitCount_ = 0;
}

// Move assignment operator
BitBuffer& BitBuffer::operator=(BitBuffer&& other) noexcept {
    if (this != &other) {
        buffer_ = NEX_STD move(other.buffer_);
        bitCount_ = other.bitCount_;
        other.bitCount_ = 0;
    }
    return *this;
}

////// Capacity --------------------------------------------------

// Resize the buffer with fill value
void BitBuffer::resize(usize newSize, bool fillValue) {
    usize oldSize = bitCount_;
    bitCount_ = newSize;
    
    if (newSize == 0) {
        buffer_.clear();
        return;
    }
    
    usize requiredBytes = bufferSizeForBits(newSize);
    usize currentBytes = buffer_.size();
    
    if (requiredBytes > currentBytes) {
        // Need to grow
        uint8 fillByte = fillValue ? 0xFF : 0x00;
        buffer_.resize(requiredBytes, fillByte);
    } else if (requiredBytes < currentBytes) {
        // Need to shrink
        buffer_.resize(requiredBytes);
    }
    
    // If we're extending, fill new bits
    if (newSize > oldSize && fillValue) {
        // New bits are already filled by resize, but we need to handle
        // the case where we're extending within the same byte
        usize oldByteCount = bufferSizeForBits(oldSize);
        if (oldByteCount == requiredBytes && oldSize > 0) {
            // Extending within the same byte
            usize startBit = oldSize;
            usize endBit = newSize;
            for (usize i = startBit; i < endBit; ++i) {
                setBit(i);
            }
        }
    }
    
    clearUnusedBits();
}

// Clear the buffer
void BitBuffer::clear() noexcept {
    buffer_.clear();
    bitCount_ = 0;
}

////// Conversion and serialization ------------------------------

// Create BitBuffer from Vector of booleans (ArrayList<bool>)
BitBuffer BitBuffer::fromVector(const ArrayList<bool>& vec) noexcept {
    BitBuffer result(vec.size());
    for (usize i = 0; i < vec.size(); ++i) {
        result.setBit(i, vec[i]);
    }
    return result;
}

// Convert to Vector of booleans (ArrayList<bool>)
ArrayList<bool> BitBuffer::toVector() const noexcept {
    ArrayList<bool> result(bitCount_);
    for (usize i = 0; i < bitCount_; ++i) {
        result[i] = testBit(i);
    }
    return result;
}

////// Element access --------------------------------------------------

// Test bit at index (returns true if set, false if clear)
bool BitBuffer::testBit(usize index) const noexcept {
    if (index >= bitCount_) return false;
    usize byteIdx = byteIndex(index);
    usize bitOff = bitOffset(index);
    return (buffer_[byteIdx] & (1U << bitOff)) != 0;
}

// Set bit at index to true
void BitBuffer::setBit(usize index) noexcept {
    if (index >= bitCount_) return;
    usize byteIdx = byteIndex(index);
    usize bitOff = bitOffset(index);
    buffer_[byteIdx] |= (1U << bitOff);
}

// Set bit at index to specified value
void BitBuffer::setBit(usize index, bool value) noexcept {
    if (value) {
        setBit(index);
    } else {
        clearBit(index);
    }
}

// Clear bit at index (set to false)
void BitBuffer::clearBit(usize index) noexcept {
    if (index >= bitCount_) return;
    usize byteIdx = byteIndex(index);
    usize bitOff = bitOffset(index);
    buffer_[byteIdx] &= ~(1U << bitOff);
}

// Toggle bit at index
void BitBuffer::toggleBit(usize index) noexcept {
    if (index >= bitCount_) return;
    usize byteIdx = byteIndex(index);
    usize bitOff = bitOffset(index);
    buffer_[byteIdx] ^= (1U << bitOff);
}

// Access bit at index (with bounds checking)
bool BitBuffer::at(usize index) const {
    NEX_ASSERT_MSG(index >= 0 && index < bitCount_, "Index out of range");
    return testBit(index);
}

////// Modifiers --------------------------------------------------

// Fill all bits with value
BitBuffer& BitBuffer::fill(bool value) {
    uint8 fillByte = value ? 0xFF : 0x00;
    NEX_STD fill(buffer_.begin(), buffer_.end(), fillByte);
    clearUnusedBits();
    return *this;
}

// Fill bits in range with value
BitBuffer& BitBuffer::fill(bool value, usize start, usize count) {
    if (start >= bitCount_) return *this;
    usize end = NEX_STD min(start + count, bitCount_);
    for (usize i = start; i < end; ++i) {
        setBit(i, value);
    }
    return *this;
}

////// Bitwise operations --------------------------------------------------

// Bitwise AND
BitBuffer& BitBuffer::operator&=(const BitBuffer& other) {
    usize minSize = NEX_STD min(bitCount_, other.bitCount_);
    usize minBytes = NEX_STD min(buffer_.size(), other.buffer_.size());
    
    for (usize i = 0; i < minBytes; ++i) {
        buffer_[i] &= other.buffer_[i];
    }
    
    // Clear remaining bytes if this buffer is larger
    if (buffer_.size() > minBytes) {
        NEX_STD fill(buffer_.begin() + minBytes, buffer_.end(), static_cast<uint8>(0));
    }
    
    // Adjust bit count to minimum
    bitCount_ = minSize;
    clearUnusedBits();
    return *this;
}

// Bitwise OR
BitBuffer& BitBuffer::operator|=(const BitBuffer& other) {
    if (other.bitCount_ > bitCount_) {
        resize(other.bitCount_, false);
    }
    
    usize minBytes = NEX_STD min(buffer_.size(), other.buffer_.size());
    for (usize i = 0; i < minBytes; ++i) {
        buffer_[i] |= other.buffer_[i];
    }
    
    clearUnusedBits();
    return *this;
}

// Bitwise XOR
BitBuffer& BitBuffer::operator^=(const BitBuffer& other) {
    if (other.bitCount_ > bitCount_) {
        resize(other.bitCount_, false);
    }
    
    usize minBytes = NEX_STD min(buffer_.size(), other.buffer_.size());
    for (usize i = 0; i < minBytes; ++i) {
        buffer_[i] ^= other.buffer_[i];
    }
    
    clearUnusedBits();
    return *this;
}

// Bitwise NOT (invert all bits)
BitBuffer BitBuffer::operator~() const {
    BitBuffer result = *this;
    for (usize i = 0; i < result.buffer_.size(); ++i) {
        result.buffer_[i] = ~result.buffer_[i];
    }
    result.clearUnusedBits();
    return result;
}

// Non-member bitwise operators
BitBuffer operator&(const BitBuffer& a, const BitBuffer& b) {
    BitBuffer result = a;
    result &= b;
    return result;
}

BitBuffer operator|(const BitBuffer& a, const BitBuffer& b) {
    BitBuffer result = a;
    result |= b;
    return result;
}

BitBuffer operator^(const BitBuffer& a, const BitBuffer& b) {
    BitBuffer result = a;
    result ^= b;
    return result;
}


////// Sub-array operations --------------------------------------------------

// Get middle part of the buffer
BitBuffer BitBuffer::mid(usize start, usize count /* = NEX_STD numeric_limits<usize>::max() */) const noexcept {
    if (start >= bitCount_) return BitBuffer();
    usize maxCount = bitCount_ - start;
    usize actualCount = (count == NEX_STD numeric_limits<usize>::max()) 
                            ? maxCount : NEX_STD min(count, maxCount);
    
    BitBuffer result(actualCount);
    for (usize i = 0; i < actualCount; ++i) {
        result.setBit(i, testBit(start + i));
    }
    return result;
}

////// Search operations --------------------------------------------------

// Count number of bits set to true
usize BitBuffer::countTrue() const noexcept {
    usize count = 0;
    for (usize i = 0; i < bitCount_; ++i) {
        if (testBit(i)) {
            ++count;
        }
    }
    return count;
}

// Count number of bits set to false
usize BitBuffer::countFalse() const noexcept {
    return bitCount_ - countTrue();
}

// Check if any bit is set
bool BitBuffer::any() const noexcept {
    for (usize i = 0; i < buffer_.size(); ++i) {
        if (buffer_[i] != 0) {
            return true;
        }
    }
    return false;
}

// Check if all bits are set
bool BitBuffer::all() const noexcept {
    if (bitCount_ == 0) return true;
    for (usize i = 0; i < bitCount_; ++i) {
        if (!testBit(i)) {
            return false;
        }
    }
    return true;
}

////// Comparison operations --------------------------------------------------

// Compare with another BitBuffer
int32 BitBuffer::compare(const BitBuffer& other) const noexcept {
    usize minSize = NEX_STD min(bitCount_, other.bitCount_);
    
    // Compare common bits
    for (usize i = 0; i < minSize; ++i) {
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
bool BitBuffer::operator==(const BitBuffer& other) const noexcept {
    if (bitCount_ != other.bitCount_) return false;
    if (bitCount_ == 0) return true;
    
    // Compare bytes (except last byte which may have unused bits)
    usize fullBytes = bitCount_ / bitsPerByte();
    if (fullBytes > 0) {
        if (NEX_STD memcmp(buffer_.data(), other.buffer_.data(), fullBytes) != 0) {
            return false;
        }
    }
    
    // Compare remaining bits in the last byte
    usize remainingBits = bitCount_ % bitsPerByte();
    if (remainingBits > 0) {
        usize lastByteIndex = byteIndex(bitCount_ - 1);
        uint8 mask = static_cast<uint8>((1U << remainingBits) - 1);
        if ((buffer_[lastByteIndex] & mask) != (other.buffer_[lastByteIndex] & mask)) {
            return false;
        }
    }
    
    return true;
}

NEX_NAMESPACE_END