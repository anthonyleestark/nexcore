/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <iterator>
#include <limits>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/linear.h"

NEX_NAMESPACE_BEGIN

/**
 * @class BitArray
 * @brief A class that represents a dynamic array of bits (boolean values) with various operations 
 *        for manipulation and querying.
 * @details
 * The BitArray is a dynamic, heap-allocated container designed for the efficient storage and manipulation of 
 * individual bits. Unlike a fixed-size BitSet, the BitArray can grow or shrink at runtime, making it ideal for 
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
 * @see BitSet for a fixed-size alternative, and Vec<bool> for a more flexible but less memory-efficient option.
 */
class NEX_EXPORT BitArray {
public:
    // Forward declarations for nested types
    class BitReference;
    class Iterator;
    class ConstIterator;

    // Type aliases for compatibility with standard container conventions
    using value_type = bool;
    using block_type = uint8;
    using size_type = usize;
    using difference_type = isize;
    using reference = BitReference;
    using const_reference = value_type;
    using pointer = void;
    using const_pointer = void;
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using reverse_iterator = NEX_STD reverse_iterator<iterator>;
    using const_reverse_iterator = NEX_STD reverse_iterator<const_iterator>;

private:
    // Internal buffer to store bits (8 bits per byte)
    Vec<block_type> buffer_;
    
    // Number of bits (may be less than buffer_.size() * 8)
    size_type bitCount_;

public:
    // Special value representing "not found" for search operations
    static constexpr size_type npos = NEX_STD numeric_limits<size_type>::max();

    ////// Helper functions for bit manipulation ------------------------------

    // Get the number of bits per byte
    static constexpr size_type bitsPerByte() noexcept { return 8; }
    
    // Get the index of the byte in the buffer for a given bit index
    static constexpr size_type byteIndex(size_type bitIndex) noexcept {
        return bitIndex / bitsPerByte();
    }
    
    // Get the offset of the bit in the byte for a given bit index
    static constexpr size_type bitOffset(size_type bitIndex) noexcept {
        return bitIndex % bitsPerByte();
    }
    
    // Get the mask for a given bit index
    static constexpr block_type bitMask(size_type bitIndex) noexcept {
        return static_cast<block_type>(1U << bitOffset(bitIndex));
    }
    
    // Calculate required buffer size for a given number of bits
    static constexpr size_type bufferSizeForBits(size_type bitCount) noexcept {
        return (bitCount + bitsPerByte() - 1) / bitsPerByte();
    }
    
    // Clear unused bits in the last byte
    void clearUnusedBits() noexcept;

    ////// Constructors and assignment operators ------------------------------

    // Default constructor
    explicit BitArray() : bitCount_(0) {}
    
    // Construct from size (all bits initialized to false)
    explicit BitArray(size_type size);
    
    // Construct from size and fill value
    explicit BitArray(size_type size, value_type fillValue);
    
    // Construct from another BitArray
    BitArray(const BitArray& other) : buffer_(other.buffer_), bitCount_(other.bitCount_) {}
    
    // Copy assignment operator
    BitArray& operator=(const BitArray& other);
    
    // Move constructor
    BitArray(BitArray&& other) noexcept;
    
    // Move assignment operator
    BitArray& operator=(BitArray&& other) noexcept;
    
    // Destructor
    ~BitArray() = default;

    ////// Capacity and size management ------------------------------

    // Get size of the BitArray (number of bits)
    constexpr size_type size() const noexcept {
        return bitCount_;
    }
    
    // Get length of the BitArray (same as size)
    constexpr size_type length() const noexcept {
        return bitCount_;
    }
    
    // Check if the BitArray is empty
    constexpr bool empty() const noexcept {
        return bitCount_ == 0;
    }
    
    // Reserve capacity (in bits)
    void reserve(size_type capacity) {
        buffer_.reserve(bufferSizeForBits(capacity));
    }

    // Get capacity (in bits)
    size_type capacity() const noexcept {
        return buffer_.capacity() * bitsPerByte();
    }

    // Reserve capacity (in bytes)
    void reserveBytes(size_type capacity) {
        buffer_.reserve(capacity);
    }
    
    // Get capacity (in bytes)
    size_type capacityBytes() const noexcept {
        return buffer_.capacity();
    }

    // Get size in bytes used by storage
    size_type sizeBytes() const noexcept {
        return buffer_.size();
    }

    // Get maximum possible size
    size_type maxSize() const noexcept {
        return buffer_.max_size() * bitsPerByte();
    }
    
    // Resize the BitArray (newSize is number of bits)
    void resize(size_type newSize) {
        resize(newSize, false);
    }
    
    // Resize the BitArray with fill value
    void resize(size_type newSize, value_type fillValue);
    
    // Clear the BitArray (set size to 0, does not deallocate memory)
    void clear() noexcept;

    ////// Conversion --------------------------------------

    // Create BitArray from a dynamic array of booleans (Vec<bool>)
    static BitArray fromVec(const Vec<value_type>& vec) noexcept;

    // Convert to dynamic array of booleans (Vec<bool>)
    Vec<value_type> toVec() const noexcept;

    ////// Element access and modification ------------------------------

    // Test bit at index (returns true if set, false if clear)
    bool testBit(size_type index) const noexcept;
    
    // Set bit at index to true
    void setBit(size_type index) noexcept;
    
    // Set bit at index to specified value
    void setBit(size_type index, value_type value) noexcept;
    
    // Clear bit at index (set to false)
    void clearBit(size_type index) noexcept;
    
    // Toggle bit at index
    void toggleBit(size_type index) noexcept;
    
    // Access bit at index (with bounds checking)
    const_reference at(size_type index) const;
    
    // Access bit at index (no bounds checking)
    const_reference operator[](size_type index) const noexcept {
        return testBit(index);
    }
    
    ////// Set bit at index using operator (returns reference-like proxy) ------------------------------

    /**
     * @class BitReference
     * @brief Proxy class for reference-like access to bits
     * 
     * @details
     * This class allows for setting bits using operator[] with a reference-like syntax.
     * It holds a pointer to the BitArray and the index of the bit it represents.
     * The assignment operator sets the bit to the assigned value, and the conversion operator 
     * allows reading the bit value.
     * 
     * @note This proxy class is used to enable syntax like bitArray[index] = true; to set bits, 
     *       and bool value = bitArray[index]; to read bits.
     * @warning This proxy class does not support all reference-like operations and is intended for 
     *          simple assignment and reading. Use with caution for more complex expressions.
     */
    class BitReference {
    private:
        // Pointer to the BitArray
        BitArray* buffer_;

        // Index of the bit in the BitArray
        size_type index_;
        
    public:
        // Constructor
        BitReference(BitArray* buffer, size_type index) 
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
    reference operator[](size_type index) noexcept {
        return BitReference(this, index);
    }

    ////// Iterator support ------------------------------

    /**
     * @class Iterator
     * @brief Random-access iterator over mutable BitArray bits.
     * 
     * @details
     * This iterator allows for iterating over the bits of the BitArray and modifying them.
     * It supports all standard random-access iterator operations, including increment, decrement,
     * arithmetic, and comparison. The dereference operator returns a reference-like proxy (BitReference)
     * that allows for setting the bit value directly through the iterator.
     * 
     * @note 
     * Modifying the BitArray (such as resizing) while iterating may invalidate the iterator and lead to 
     * undefined behavior.
     * 
     * @see ConstIterator for a read-only version of the iterator.
     */
    class Iterator {
    public:
        // Iterator traits
        using iterator_category = NEX_STD random_access_iterator_tag;
        using value_type = BitArray::value_type;
        using difference_type = BitArray::difference_type;
        using pointer = void;
        using reference = BitArray::reference;

        // Allow ConstIterator to access private members
        friend class ConstIterator;

        // Constructs an iterator for the given BitArray and index
        constexpr Iterator(BitArray* buffer = nullptr, size_type index = 0) noexcept
            : buffer_(buffer), index_(index) {}

        // Dereference operators
        reference operator*() const noexcept { return (*buffer_)[index_]; }
        reference operator[](difference_type offset) const noexcept {
            return (*buffer_)[static_cast<size_type>(static_cast<difference_type>(index_) + offset)];
        }

        // Iterator operations
        Iterator& operator++() noexcept { ++index_; return *this; }
        Iterator operator++(int) noexcept { Iterator tmp = *this; ++(*this); return tmp; }
        
        Iterator& operator--() noexcept { --index_; return *this; }
        Iterator operator--(int) noexcept { Iterator tmp = *this; --(*this); return tmp; }

        // Athmetic operators for random access iterator
        Iterator& operator+=(difference_type offset) noexcept {
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) + offset);
            return *this;
        }
        Iterator operator+(difference_type offset) const noexcept {
            Iterator tmp = *this;
            tmp += offset;
            return tmp;
        }

        Iterator& operator-=(difference_type offset) noexcept {
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) - offset);
            return *this;
        }
        Iterator operator-(difference_type offset) const noexcept {
            Iterator tmp = *this;
            tmp -= offset;
            return tmp;
        }

        // Difference operator for random access iterator
        difference_type operator-(const Iterator& other) const noexcept {
            return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_);
        }

        // Friend function for addition with difference_type on the left
        friend Iterator operator+(difference_type offset, const Iterator& it) noexcept {
            return it + offset;
        }

        // Equality operator
        bool operator==(const Iterator& other) const noexcept {
            return buffer_ == other.buffer_ && index_ == other.index_;
        }

        // Inequality operator
        bool operator!=(const Iterator& other) const noexcept { return !(*this == other); }

        // Comparison operators for random access iterator
        bool operator<(const Iterator& other) const noexcept { return index_ < other.index_; }
        bool operator<=(const Iterator& other) const noexcept { return index_ <= other.index_; }
        bool operator>(const Iterator& other) const noexcept { return index_ > other.index_; }
        bool operator>=(const Iterator& other) const noexcept { return index_ >= other.index_; }

    private:
        BitArray* buffer_;
        size_type index_;
    };

    /**
     * @class ConstIterator
     * @brief Random-access iterator over read-only BitArray bits.
     * 
     * @details
     * This iterator allows for iterating over the bits of the BitArray without modifying them.
     * It supports all standard random-access iterator operations, including increment, decrement,
     * arithmetic, and comparison. The dereference operator returns the value of the bit at the 
     * current position.
     * 
     * @note 
     * Modifying the BitArray (such as resizing) while iterating may invalidate the iterator and lead to 
     * undefined behavior.
     * 
     * @see Iterator for a mutable version of the iterator.
     */
    class ConstIterator {
    public:
        // Iterator traits
        using iterator_category = NEX_STD random_access_iterator_tag;
        using value_type = BitArray::value_type;
        using difference_type = BitArray::difference_type;
        using reference = BitArray::const_reference;
        using pointer = void;

        // Constructor for ConstIterator
        constexpr ConstIterator(const BitArray* buffer = nullptr, size_type index = 0) noexcept
            : buffer_(buffer), index_(index) {}

        // Allow conversion from Iterator to ConstIterator
        constexpr ConstIterator(const Iterator& it) noexcept
            : buffer_(it.buffer_), index_(it.index_) {}

        // Dereference operators
        reference operator*() const noexcept { return (*buffer_)[index_]; }
        reference operator[](difference_type offset) const noexcept {
            return (*buffer_)[static_cast<size_type>(static_cast<difference_type>(index_) + offset)];
        }

        // Iterator operations
        ConstIterator& operator++() noexcept { ++index_; return *this; }
        ConstIterator operator++(int) noexcept { ConstIterator tmp = *this; ++(*this); return tmp; }

        ConstIterator& operator--() noexcept { --index_; return *this; }
        ConstIterator operator--(int) noexcept { ConstIterator tmp = *this; --(*this); return tmp; }

        // Arithmetic operators for random access iterator
        ConstIterator& operator+=(difference_type offset) noexcept {
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) + offset);
            return *this;
        }
        ConstIterator operator+(difference_type offset) const noexcept {
            ConstIterator tmp = *this;
            tmp += offset;
            return tmp;
        }

        ConstIterator& operator-=(difference_type offset) noexcept {
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) - offset);
            return *this;
        }
        ConstIterator operator-(difference_type offset) const noexcept {
            ConstIterator tmp = *this;
            tmp -= offset;
            return tmp;
        }

        // Difference operator for random access iterator
        difference_type operator-(const ConstIterator& other) const noexcept {
            return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_);
        }

        // Friend function for addition with difference_type on the left
        friend ConstIterator operator+(difference_type offset, const ConstIterator& it) noexcept {
            return it + offset;
        }

        // Equality operator
        bool operator==(const ConstIterator& other) const noexcept {
            return buffer_ == other.buffer_ && index_ == other.index_;
        }

        // Inequality operator
        bool operator!=(const ConstIterator& other) const noexcept { return !(*this == other); }

        // Comparison operators for random access iterator
        bool operator<(const ConstIterator& other) const noexcept { return index_ < other.index_; }
        bool operator<=(const ConstIterator& other) const noexcept { return index_ <= other.index_; }
        bool operator>(const ConstIterator& other) const noexcept { return index_ > other.index_; }
        bool operator>=(const ConstIterator& other) const noexcept { return index_ >= other.index_; }

    private:
        const BitArray* buffer_;
        size_type index_;
    };

    // Get iterator to the beginning of the BitArray
    iterator begin() noexcept { return iterator(this, 0); }

    // Get constant iterator to the beginning of the BitArray
    const_iterator begin() const noexcept { return const_iterator(this, 0); }

    // Get constant iterator to the beginning of the BitArray (same as begin() const)
    const_iterator cbegin() const noexcept { return const_iterator(this, 0); }

    // Get iterator to the end of the BitArray
    iterator end() noexcept { return iterator(this, bitCount_); }

    // Get constant iterator to the end of the BitArray
    const_iterator end() const noexcept { return const_iterator(this, bitCount_); }

    // Get constant iterator to the end of the BitArray (same as end() const)
    const_iterator cend() const noexcept { return const_iterator(this, bitCount_); }

    // Get reverse iterator to the beginning of the reversed BitArray
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    // Get constant reverse iterator to the beginning of the reversed BitArray
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    // Get constant reverse iterator to the beginning of the reversed BitArray (same as rbegin() const)
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    // Get reverse iterator to the end of the reversed BitArray
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    // Get constant reverse iterator to the end of the reversed BitArray
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    // Get constant reverse iterator to the end of the reversed BitArray (same as rend() const)
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    // Get reference to the first bit
    reference front() noexcept { return (*this)[0]; }

    // Get reference to the first bit (read-only)
    const_reference front() const noexcept { return (*this)[0]; }

    // Get reference to the last bit
    reference back() noexcept { return (*this)[bitCount_ - 1]; }

    // Get reference to the last bit (read-only)
    const_reference back() const noexcept { return (*this)[bitCount_ - 1]; }

    ////// Modifiers ------------------------------

    // Fill all bits with value
    BitArray& fill(value_type value);
    
    // Fill bits in range with value
    BitArray& fill(value_type value, size_type start, size_type count);
    
    // Set all bits
    BitArray& setAll() {
        return fill(true);
    }
    
    // Clear all bits
    BitArray& clearAll() {
        return fill(false);
    }
    
    // Swap with another BitArray
    void swap(BitArray& other) noexcept {
        buffer_.swap(other.buffer_);
        NEX_STD swap(bitCount_, other.bitCount_);
    }

    ////// Bitwise operations ------------------------------

    // Bitwise AND
    BitArray& operator&=(const BitArray& other);
    
    // Bitwise OR
    BitArray& operator|=(const BitArray& other);
    
    // Bitwise XOR
    BitArray& operator^=(const BitArray& other);
    
    // Bitwise NOT (invert all bits)
    BitArray operator~() const;
    
    ////// Non-member bitwise operators ------------------------------

    // Bitwise AND
    friend BitArray operator&(const BitArray& a, const BitArray& b);
    
    // Bitwise OR
    friend BitArray operator|(const BitArray& a, const BitArray& b);
    
    // Bitwise XOR
    friend BitArray operator^(const BitArray& a, const BitArray& b);

    ////// Sub-array operations ------------------------------

    // Get left part of the buffer
    BitArray left(size_type count) const noexcept {
        if (count == 0) return BitArray();
        if (count >= bitCount_) return *this;
        return mid(0, count);
    }
    
    // Get right part of the buffer
    BitArray right(size_type count) const noexcept {
        if (count == 0) return BitArray();
        if (count >= bitCount_) return *this;
        size_type start = bitCount_ - count;
        return mid(start, count);
    }
    
    // Get middle part of the buffer
    BitArray mid(size_type start, size_type count = npos) const noexcept;

    ////// Search operations ------------------------------

    // Count number of bits set to true
    size_type count(value_type value = true) const noexcept {
        return value ? countTrue() : countFalse();
    }
    
    // Count number of bits set to true
    size_type countTrue() const noexcept;
    
    // Count number of bits set to false
    size_type countFalse() const noexcept;
    
    // Check if any bit is set
    bool any() const noexcept;
    
    // Check if all bits are set
    bool all() const noexcept;
    
    // Check if no bits are set
    bool none() const noexcept {
        return !any();
    }

    ////// Comparison operations ------------------------------

    // Compare with another BitArray
    int32 compare(const BitArray& other) const noexcept;
    
    // Equality operator
    bool operator==(const BitArray& other) const noexcept;
    
    // Inequality operator
    bool operator!=(const BitArray& other) const noexcept {
        return !(*this == other);
    }
    
    // Less-than operator
    bool operator<(const BitArray& other) const noexcept {
        return compare(other) < 0;
    }
    
    // Less-than-or-equal operator
    bool operator<=(const BitArray& other) const noexcept {
        return compare(other) <= 0;
    }
    
    // Greater-than operator
    bool operator>(const BitArray& other) const noexcept {
        return compare(other) > 0;
    }
    
    // Greater-than-or-equal operator
    bool operator>=(const BitArray& other) const noexcept {
        return compare(other) >= 0;
    }
};

NEX_NAMESPACE_END
