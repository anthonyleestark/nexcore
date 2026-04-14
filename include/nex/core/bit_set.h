/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <initializer_list>
#include <type_traits>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/linear.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @class BitSet
 * @brief A class that represents a fixed-size set of bits (boolean values) with various operations 
 *        for manipulation and querying.
 * 
 * @details
 * The BitSet is a fixed-size, contiguous sequence of N bits. Unlike its dynamic sibling, BitBuffer, the size of 
 * a BitSet is determined at compile-time via template parameters.
 * It is designed to be a high-performance alternative to arrays of booleans or manual bitmasking with integers. 
 * By packing bits into internal storage "words," it provides a memory-efficient way to handle sets of flags, 
 * mathematical sets, or binary states without the overhead of heap allocation.
 * 
 * Key Characteristics:
 * - Zero Heap Allocation: Data is stored directly within the object (usually on the stack). This makes creation 
 *   and destruction nearly instantaneous.
 * - Compile-Time Safety: Uses C++ constraints (like requires (N > 0)) and static_assert to catch sizing errors 
 *   before the program even runs.
 * - Adaptive Storage: Internally selects the most efficient word size (e.g., uint8, uint16, uint32, or uint64) 
 *   based on the value of N to minimize memory padding.
 * - Cache Friendly: Because it is stored contiguously and often fits entirely within a single CPU cache line, 
 *   it offers superior performance for linear scans and bitwise logic.
 * 
 * @tparam N The number of bits in the BitSet (must be greater than 0).
 * 
 * @see BitBuffer for a dynamic alternative, and ArrayList<bool> for a more flexible but less memory-efficient option.
 */
template <usize N> requires (N > 0)
class NEX_EXPORT BitSet {
public:
    // Choose the smallest native word size that fits N, up to 64-bit.
    using Word = NEX_STD conditional_t<N <= 8,  uint8,
                 NEX_STD conditional_t<N <= 16, uint16,
                 NEX_STD conditional_t<N <= 32, uint32, 
                                                uint64>>>;

    // Constants
    static constexpr usize BitsPerWord = sizeof(Word) * 8;
    static constexpr usize WordCount = (N + BitsPerWord - 1) / BitsPerWord;
    static constexpr usize TotalBits = WordCount * BitsPerWord;     // Total bits in the buffer (may be >= N)
    static constexpr usize npos = static_cast<usize>(-1);           // Special value for "not found"

private:
    // Internal buffer to store bits
    Word buffer_[WordCount] = {0};

public:
    // Get the number of bits per byte
    static constexpr usize bitsPerByte() noexcept { return 8; }

    // Get the number of bits per word
    static constexpr usize bitsPerWord() noexcept { return BitsPerWord; }

    // Get the index of the byte in the buffer for a given bit index
    static constexpr usize byteIndex(usize bitIndex) noexcept {
        return bitIndex / bitsPerByte();
    }

    // Get the index of the word in the buffer for a given bit index
    static constexpr usize wordIndex(usize bitIndex) noexcept {
        return bitIndex / bitsPerWord();
    }

    // Get the offset of the bit in the byte for a given bit index
    static constexpr usize bitOffset(usize bitIndex) noexcept {
        return bitIndex % bitsPerByte();
    }

    // Get the offset of the bit in the word for a given bit index
    static constexpr usize wordBitOffset(usize bitIndex) noexcept {
        return bitIndex % bitsPerWord();
    }

    // Get the mask for a given bit index
    static constexpr Word bitMask(usize bitIndex) noexcept {
        return static_cast<Word>(1U << wordBitOffset(bitIndex));
    }

    // Clear unused bits in the last byte
    void clearUnusedBits() noexcept {
        // Since N is already ensured to be > 0, 
        // we can safely calculate the last byte index and bit offset without additional checks.
        usize lastByteIndex = byteIndex(N - 1);
        usize lastBitOffset = bitOffset(N - 1);
        if (lastBitOffset < bitsPerByte() - 1) {
            // Clear bits beyond the last valid bit
            uint8 mask = static_cast<uint8>((1U << (lastBitOffset + 1)) - 1);
            buffer_[lastByteIndex] &= mask;
        }
    }

    ////// Constructors and assignment operators ------------------------------

    // Default constructor (all bits initialized to false)
    constexpr BitSet() = default;

    // Construct from initializer list of booleans
    constexpr BitSet(NEX_STD initializer_list<bool> init) {
        static_assert(init.size() <= N, 
                "Initializer list size cannot exceed the size of the BitSet"); // compile-time check
        usize i = 0;
        for (bool value : init) {
            setBit(i, value);
            ++i;
        }
        clearUnusedBits();
    }

    // Construct from count and fill value (fill first 'count' bits with 'value', rest are false)
    constexpr explicit BitSet(usize count, bool value) noexcept {
        NEX_ASSERT(count <= N, "Count cannot exceed the size of the BitSet"); // debug runtime-check only
        clear(); // Ensure all bits are cleared first
        if (value) {
            for (usize i = 0; i < count; ++i) {
                setBit(i, true);
            }
            clearUnusedBits();
        }
    }

    // Default copy constructor and copy assignment operator
    NEX_DEFAULT_COPY(BitSet);

    // Default move constructor and move assignment operator
    NEX_DEFAULT_MOVE(BitSet);

    // Default destructor
    ~BitSet() = default;

    ////// Capacity and size management ------------------------------

    // Get size of the BitSet (number of bits)
    constexpr usize size() const noexcept { return N; }

    // Get length of the BitSet (same as size)
    constexpr usize length() const noexcept { return N; }

    // Check if the BitSet is empty (always false since N > 0)
    constexpr bool empty() const noexcept { return false; }

    // Check if all bits are set to false
    constexpr bool allFalse() const noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            if (buffer_[i] != 0) return false;
        }
        return true;
    }

    ////// Conversion -------------------------------

    // Create a BitSet from a Vector of booleans (truncates if vector is larger than N)
    static BitSet fromVector(const ArrayList<bool>& vec) noexcept {
        BitSet result;
        usize count = vec.size() < N ? vec.size() : N;
        for (usize i = 0; i < count; ++i) {
            result.setBit(i, vec[i]);
        }
        result.clearUnusedBits();
        return result;
    }

    // Convert BitSet to a Vector of booleans
    ArrayList<bool> toVector() const noexcept {
        ArrayList<bool> vec;
        vec.reserve(N);
        for (usize i = 0; i < N; ++i) {
            vec.push_back(testBit(i));
        }
        return vec;
    }

    ////// Element access and modification ------------------------------

    // Test bit at index (returns true if set, false if clear)
    constexpr bool testBit(usize index) const noexcept {
        if (index >= N) return false;
        return (buffer_[wordIndex(index)] & bitMask(index)) != 0;
    }

    // Set bit at index to true
    constexpr void setBit(usize index) noexcept {
        if (index >= N) return;
        buffer_[wordIndex(index)] |= bitMask(index);
    }

    // Set bit at index to false
    constexpr void clearBit(usize index) noexcept {
        if (index >= N) return;
        buffer_[wordIndex(index)] &= ~bitMask(index);
    }

    // Set bit at index to specified value
    constexpr void setBit(usize index, bool value) noexcept {
        if (value) {
            setBit(index);
        } else {
            clearBit(index);
        }
    }

    // Clear all bits (set to false)
    constexpr void clear() noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            buffer_[i] = 0;
        }
    }

    // Flip bit at index (toggle its value)
    constexpr void toggleBit(usize index) noexcept {
        if (index >= N) return;
        buffer_[wordIndex(index)] ^= bitMask(index);
    }

    // Access bit at index for reading (with bounds checking)
    constexpr bool at(usize index) const noexcept {
        NEX_ASSERT(index < N, "Index out of bounds"); // debug runtime-check only
        if (index >= N) return false;
        return testBit(index);
    }

    // Access bit at index for reading (without bounds checking)
    constexpr bool operator[](usize index) const noexcept {
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
     * @note This proxy class is used to enable syntax like bitSet[index] = true; to set bits, 
     *       and bool value = bitSet[index]; to read bits.
     * @warning This proxy class does not support all reference-like operations and is intended for 
     *          simple assignment and reading. Use with caution for more complex expressions.
     */
    class BitReference {
    private:
        // Pointer to the BitSet
        BitSet* bitSet_;

        // Index of the bit in the BitSet
        usize index_;
        
    public:
        // Constructor
        BitReference(BitSet* bitSet, usize index) 
            : bitSet_(bitSet), index_(index) {}
        
        // Assignment operator to set the bit value
        BitReference& operator=(bool value) noexcept {
            bitSet_->setBit(index_, value);
            return *this;
        }
        
        // Assignment operator to copy from another BitReference
        BitReference& operator=(const BitReference& other) noexcept {
            bitSet_->setBit(index_, other.bitSet_->testBit(other.index_));
            return *this;
        }
        
        // Conversion operator to read the bit value
        operator bool() const noexcept {
            return bitSet_->testBit(index_);
        }
        
        // Compound assignment operators for bitwise operations
        BitReference& operator|=(bool value) noexcept {
            if (value) bitSet_->setBit(index_);
            return *this;
        }
        
        // Compound assignment operator for bitwise AND
        BitReference& operator&=(bool value) noexcept {
            if (!value) bitSet_->clearBit(index_);
            return *this;
        }
        
        // Compound assignment operator for bitwise XOR
        BitReference& operator^=(bool value) noexcept {
            if (value) bitSet_->toggleBit(index_);
            return *this;
        }
        
        // Bitwise NOT operator to invert the bit value
        bool operator~() const noexcept {
            return !bitSet_->testBit(index_);
        }
    };
    
    // Access bit at index for setting (returns reference-like proxy)
    BitReference operator[](usize index) noexcept {
        return BitReference(this, index);
    }

    ////// Modifiers ------------------------------

    // Fill the BitSet with a specified value (true or false)
    BitSet& fill(bool value) noexcept {
        if (value) {
            for (usize i = 0; i < WordCount; ++i) {
                buffer_[i] = static_cast<Word>(~0U); // Set all bits
            }
            clearUnusedBits();  // Clear unused bits in the last word
        } else {
            clear();            // Clear all bits
        }
        return *this;
    }

    // Fill bits in range with specified value
    BitSet& fill(bool value, usize start, usize count) noexcept {
        if (start >= N || count == 0) return *this;     // No bits to fill
        if (start + count > N) count = N - start;       // Adjust count to fit within bounds
        for (usize i = start; i < start + count; ++i) {
            setBit(i, value);
        }
        return *this;
    }

    // Set all bits to true
    BitSet& setAll() noexcept {
        return fill(true);
    }

    // Clear all bits to false
    BitSet& clearAll() noexcept {
        clear();
        return *this;
    }

    // Flip all bits (invert their values)
    BitSet& flipAll() noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            buffer_[i] = ~buffer_[i];
        }
        clearUnusedBits();  // Clear unused bits in the last word
        return *this;
    }

    // Flip bits in range (invert their values)
    BitSet& flip(bool value, usize start, usize count) noexcept {
        if (start >= N || count == 0) return *this;     // No bits to flip
        if (start + count > N) count = N - start;       // Adjust count to fit within bounds
        for (usize i = start; i < start + count; ++i) {
            if (value) {
                setBit(i, !testBit(i)); // Flip the bit
            } else {
                setBit(i, testBit(i));  // Keep the bit unchanged
            }
        }
        return *this;
    }

    // Swap contents with another BitSet of the same size
    void swap(BitSet& other) noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            Word temp = buffer_[i];
            buffer_[i] = other.buffer_[i];
            other.buffer_[i] = temp;
        }
    }

    ////// Bitwise operations ------------------------------

    // Bitwise AND
    BitSet& operator&=(const BitSet& other) noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            buffer_[i] &= other.buffer_[i];
        }
        return *this;
    }

    // Bitwise OR
    BitSet& operator|=(const BitSet& other) noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            buffer_[i] |= other.buffer_[i];
        }
        return *this;
    }

    // Bitwise XOR
    BitSet& operator^=(const BitSet& other) noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            buffer_[i] ^= other.buffer_[i];
        }
        return *this;
    }

    // Bitwise NOT (invert all bits)
    BitSet operator~() const noexcept {
        BitSet result;
        for (usize i = 0; i < WordCount; ++i) {
            result.buffer_[i] = ~buffer_[i];
        }
        result.clearUnusedBits();  // Clear unused bits in the last word
        return result;
    }

    ////// Non-member bitwise operators ------------------------------

    // Bitwise AND
    friend BitSet operator&(const BitSet& a, const BitSet& b) noexcept {
        BitSet result;
        for (usize i = 0; i < WordCount; ++i) {
            result.buffer_[i] = a.buffer_[i] & b.buffer_[i];
        }
        return result;
    }

    // Bitwise OR
    friend BitSet operator|(const BitSet& a, const BitSet& b) noexcept {
        BitSet result;
        for (usize i = 0; i < WordCount; ++i) {
            result.buffer_[i] = a.buffer_[i] | b.buffer_[i];
        }
        return result;
    }

    // Bitwise XOR
    friend BitSet operator^(const BitSet& a, const BitSet& b) noexcept {
        BitSet result;
        for (usize i = 0; i < WordCount; ++i) {
            result.buffer_[i] = a.buffer_[i] ^ b.buffer_[i];
        }
        return result;
    }

    ////// Sub-array operations ------------------------------

    // Get left part of the BitSet
    BitSet left(usize count) const noexcept {
        if (count >= N) return *this;
        BitSet result;
        for (usize i = 0; i < count; ++i) {
            result.setBit(i, testBit(i));
        }
        return result;
    }

    // Get right part of the BitSet
    BitSet right(usize count) const noexcept {
        if (count >= N) return *this;
        BitSet result;
        usize start = N - count;
        for (usize i = 0; i < count; ++i) {
            result.setBit(i, testBit(start + i));
        }
        return result;
    }

    // Get middle part of the BitSet
    BitSet mid(usize start, usize count) const noexcept {
        if (start >= N || count == 0) return BitSet();
        if (start + count > N) count = N - start;
        BitSet result;
        for (usize i = 0; i < count; ++i) {
            result.setBit(i, testBit(start + i));
        }
        return result;
    }

    ////// Search operations ------------------------------

    // Find the first set bit (returns index or N if not found)
    usize findFirst(bool value = true) const noexcept {
        for (usize i = 0; i < N; ++i) {
            if (testBit(i) == value) {
                return i;
            }
        }
        return npos; // Not found
    }

    // Count number of bits set to true
    usize count(bool value = true) const noexcept {
        return value ? countTrue() : countFalse();
    }

    // Count number of bits set to true
    usize countTrue() const noexcept {
        usize count = 0;
        for (usize i = 0; i < WordCount; ++i) {
            count += NEX_STD popcount(buffer_[i]);
        }
        return count;
    }

    // Count number of bits set to false
    usize countFalse() const noexcept {
        return N - countTrue();
    }

    // Check if any bit is set to true
    bool any() const noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            if (buffer_[i] != 0) return true;
        }
        return false;
    }

    // Check if all bits are set to true
    bool all() const noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            if (buffer_[i] != static_cast<Word>(~0U)) return false;
        }
        return true;
    }

    // Check if no bits are set to true
    bool none() const noexcept {
        return !any();
    }

    ////// Comparison operations ------------------------------

    // Compare with another BitSet
    int32 compare(const BitSet& other) const noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            if (buffer_[i] < other.buffer_[i]) return -1;
            if (buffer_[i] > other.buffer_[i]) return 1;
        }
        return 0; // Equal
    }

    // Equality operator
    bool operator==(const BitSet& other) const noexcept {
        for (usize i = 0; i < WordCount; ++i) {
            if (buffer_[i] != other.buffer_[i]) return false;
        }
        return true;
    }

    // Inequality operator
    bool operator!=(const BitSet& other) const noexcept {
        return !(*this == other);
    }

    // Less-than operator
    bool operator<(const BitSet& other) const noexcept {
        return compare(other) < 0;
    }

    // Less-than-or-equal operator
    bool operator<=(const BitSet& other) const noexcept {
        return compare(other) <= 0;
    }

    // Greater-than operator
    bool operator>(const BitSet& other) const noexcept {
        return compare(other) > 0;
    }

    // Greater-than-or-equal operator
    bool operator>=(const BitSet& other) const noexcept {
        return compare(other) >= 0;
    }
};

NEX_NAMESPACE_END
