/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/linear.h"

NEX_NAMESPACE_BEGIN

/**
 * @class BitBuffer
 * @brief A class that represents a buffer of bits (boolean values) with various operations 
 *        for manipulation and querying.
 * @details
 * The BitBuffer is a dynamic, heap-allocated container designed for the efficient storage and manipulation of 
 * individual bits. Unlike a fixed-size BitSet, the BitBuffer can grow or shrink at runtime, making it ideal for 
 * scenarios where the total number of bits is unknown upfront or varies over time.
 * It bridges the gap between a high-level list and a low-level memory buffer, providing random-access performance 
 * while supporting sequential operations.
 * 
 * Key Characteristics:
 * - Dynamic Resizing: Automatically manages internal memory. When the capacity is reached, it reallocates (typically 
 *   doubling in size) to ensure O(1) amortized insertion.
 * - Memory Efficiency: Bits are "packed" into underlying storage units (uint8 words). This ensures that the memory 
 *   footprint is exactly 1/8th the size of a boolean array.
 * - Random Access: Supports O(1) time complexity for retrieving or flipping a bit at any specific index.
 * - Contiguous Storage: Maintains data in a single block of memory, ensuring high cache locality for linear scans 
 *   and iterations.
 * 
 * @see BitSet for a fixed-size alternative, and ArrayList<bool> for a more flexible but less memory-efficient option.
 */
class NEX_EXPORT BitBuffer {
private:
    // Internal buffer to store bits (8 bits per byte)
    ArrayList<uint8> buffer_;
    
    // Number of bits (may be less than buffer_.size() * 8)
    usize bitCount_;

public:
    ////// Helper functions for bit manipulation ------------------------------

    // Get the number of bits per byte
    static constexpr usize bitsPerByte() noexcept { return 8; }
    
    // Get the index of the byte in the buffer for a given bit index
    static constexpr usize byteIndex(usize bitIndex) noexcept {
        return bitIndex / bitsPerByte();
    }
    
    // Get the offset of the bit in the byte for a given bit index
    static constexpr usize bitOffset(usize bitIndex) noexcept {
        return bitIndex % bitsPerByte();
    }
    
    // Get the mask for a given bit index
    static constexpr uint8 bitMask(usize bitIndex) noexcept {
        return static_cast<uint8>(1U << bitOffset(bitIndex));
    }
    
    // Calculate required buffer size for a given number of bits
    static constexpr usize bufferSizeForBits(usize bitCount) noexcept {
        return (bitCount + bitsPerByte() - 1) / bitsPerByte();
    }
    
    // Clear unused bits in the last byte
    void clearUnusedBits() noexcept;

    ////// Constructors and assignment operators ------------------------------

    // Default constructor
    explicit BitBuffer() : bitCount_(0) {}
    
    // Construct from size (all bits initialized to false)
    explicit BitBuffer(usize size);
    
    // Construct from size and fill value
    explicit BitBuffer(usize size, bool fillValue);
    
    // Construct from another BitBuffer
    BitBuffer(const BitBuffer& other) : buffer_(other.buffer_), bitCount_(other.bitCount_) {}
    
    // Copy assignment operator
    BitBuffer& operator=(const BitBuffer& other);
    
    // Move constructor
    BitBuffer(BitBuffer&& other) noexcept;
    
    // Move assignment operator
    BitBuffer& operator=(BitBuffer&& other) noexcept;
    
    // Destructor
    ~BitBuffer() = default;

    ////// Capacity and size management ------------------------------

    // Get size of the buffer (number of bits)
    constexpr usize size() const noexcept {
        return bitCount_;
    }
    
    // Get length of the buffer (same as size)
    constexpr usize length() const noexcept {
        return bitCount_;
    }
    
    // Check if the buffer is empty
    constexpr bool empty() const noexcept {
        return bitCount_ == 0;
    }
    
    // Reserve capacity (in bytes)
    void reserve(usize capacity) {
        buffer_.reserve(capacity);
    }
    
    // Get capacity (in bytes)
    constexpr usize capacity() const noexcept {
        return buffer_.capacity();
    }
    
    // Resize the buffer
    void resize(usize newSize) {
        resize(newSize, false);
    }
    
    // Resize the buffer with fill value
    void resize(usize newSize, bool fillValue);
    
    // Clear the buffer
    void clear() noexcept;

    ////// Conversion --------------------------------------

    // Create BitBuffer from Vector of booleans (ArrayList<bool>)
    static BitBuffer fromVector(const ArrayList<bool>& vec) noexcept;

    // Convert to Vector of booleans (ArrayList<bool>)
    ArrayList<bool> toVector() const noexcept;

    ////// Element access and modification ------------------------------

    // Test bit at index (returns true if set, false if clear)
    bool testBit(usize index) const noexcept;
    
    // Set bit at index to true
    void setBit(usize index) noexcept;
    
    // Set bit at index to specified value
    void setBit(usize index, bool value) noexcept;
    
    // Clear bit at index (set to false)
    void clearBit(usize index) noexcept;
    
    // Toggle bit at index
    void toggleBit(usize index) noexcept;
    
    // Access bit at index (with bounds checking)
    bool at(usize index) const;
    
    // Access bit at index (no bounds checking)
    bool operator[](usize index) const noexcept {
        return testBit(index);
    }
    
    ////// Set bit at index using operator (returns reference-like proxy) ------------------------------

    /**
     * @class BitReference
     * @brief Proxy class for reference-like access to bits
     * 
     * @details
     * This class allows for setting bits using operator[] with a reference-like syntax.
     * It holds a pointer to the BitBuffer and the index of the bit it represents.
     * The assignment operator sets the bit to the assigned value, and the conversion operator 
     * allows reading the bit value.
     * 
     * @note This proxy class is used to enable syntax like bitBuffer[index] = true; to set bits, 
     *       and bool value = bitBuffer[index]; to read bits.
     * @warning This proxy class does not support all reference-like operations and is intended for 
     *          simple assignment and reading. Use with caution for more complex expressions.
     */
    class BitReference {
    private:
        // Pointer to the BitBuffer
        BitBuffer* buffer_;

        // Index of the bit in the BitBuffer
        usize index_;
        
    public:
        // Constructor
        BitReference(BitBuffer* buffer, usize index) 
            : buffer_(buffer), index_(index) {}
        
        // Assignment operator to set the bit value
        BitReference& operator=(bool value) noexcept {
            buffer_->setBit(index_, value);
            return *this;
        }
        
        // Assignment operator to copy from another BitReference
        BitReference& operator=(const BitReference& other) noexcept {
            buffer_->setBit(index_, other.buffer_->testBit(other.index_));
            return *this;
        }
        
        // Conversion operator to read the bit value
        operator bool() const noexcept {
            return buffer_->testBit(index_);
        }
        
        // Compound assignment operators for bitwise operations
        BitReference& operator|=(bool value) noexcept {
            if (value) buffer_->setBit(index_);
            return *this;
        }
        
        // Compound assignment operator for bitwise AND
        BitReference& operator&=(bool value) noexcept {
            if (!value) buffer_->clearBit(index_);
            return *this;
        }
        
        // Compound assignment operator for bitwise XOR
        BitReference& operator^=(bool value) noexcept {
            if (value) buffer_->toggleBit(index_);
            return *this;
        }
        
        // Bitwise NOT operator to invert the bit value
        bool operator~() const noexcept {
            return !buffer_->testBit(index_);
        }
    };
    
    // Access bit at index for setting (returns reference-like proxy)
    BitReference operator[](usize index) noexcept {
        return BitReference(this, index);
    }

    ////// Modifiers ------------------------------

    // Fill all bits with value
    BitBuffer& fill(bool value);
    
    // Fill bits in range with value
    BitBuffer& fill(bool value, usize start, usize count);
    
    // Set all bits
    BitBuffer& setAll() {
        return fill(true);
    }
    
    // Clear all bits
    BitBuffer& clearAll() {
        return fill(false);
    }
    
    // Swap with another BitBuffer
    void swap(BitBuffer& other) noexcept {
        buffer_.swap(other.buffer_);
        NEX_STD swap(bitCount_, other.bitCount_);
    }

    ////// Bitwise operations ------------------------------

    // Bitwise AND
    BitBuffer& operator&=(const BitBuffer& other);
    
    // Bitwise OR
    BitBuffer& operator|=(const BitBuffer& other);
    
    // Bitwise XOR
    BitBuffer& operator^=(const BitBuffer& other);
    
    // Bitwise NOT (invert all bits)
    BitBuffer operator~() const;
    
    ////// Non-member bitwise operators ------------------------------

    // Bitwise AND
    friend BitBuffer operator&(const BitBuffer& a, const BitBuffer& b);
    
    // Bitwise OR
    friend BitBuffer operator|(const BitBuffer& a, const BitBuffer& b);
    
    // Bitwise XOR
    friend BitBuffer operator^(const BitBuffer& a, const BitBuffer& b);

    ////// Sub-array operations ------------------------------

    // Get left part of the buffer
    BitBuffer left(usize count) const noexcept {
        if (count == 0) return BitBuffer();
        if (count >= bitCount_) return *this;
        return mid(0, count);
    }
    
    // Get right part of the buffer
    BitBuffer right(usize count) const noexcept {
        if (count == 0) return BitBuffer();
        if (count >= bitCount_) return *this;
        usize start = bitCount_ - count;
        return mid(start, count);
    }
    
    // Get middle part of the buffer
    BitBuffer mid(usize start, usize count = NEX_STD numeric_limits<usize>::max()) const noexcept;

    ////// Search operations ------------------------------

    // Count number of bits set to true
    usize count(bool value = true) const noexcept {
        return value ? countTrue() : countFalse();
    }
    
    // Count number of bits set to true
    usize countTrue() const noexcept;
    
    // Count number of bits set to false
    usize countFalse() const noexcept;
    
    // Check if any bit is set
    bool any() const noexcept;
    
    // Check if all bits are set
    bool all() const noexcept;
    
    // Check if no bits are set
    bool none() const noexcept {
        return !any();
    }

    ////// Comparison operations ------------------------------

    // Compare with another BitBuffer
    int32 compare(const BitBuffer& other) const noexcept;
    
    // Equality operator
    bool operator==(const BitBuffer& other) const noexcept;
    
    // Inequality operator
    bool operator!=(const BitBuffer& other) const noexcept {
        return !(*this == other);
    }
    
    // Less-than operator
    bool operator<(const BitBuffer& other) const noexcept {
        return compare(other) < 0;
    }
    
    // Less-than-or-equal operator
    bool operator<=(const BitBuffer& other) const noexcept {
        return compare(other) <= 0;
    }
    
    // Greater-than operator
    bool operator>(const BitBuffer& other) const noexcept {
        return compare(other) > 0;
    }
    
    // Greater-than-or-equal operator
    bool operator>=(const BitBuffer& other) const noexcept {
        return compare(other) >= 0;
    }
};

NEX_NAMESPACE_END
