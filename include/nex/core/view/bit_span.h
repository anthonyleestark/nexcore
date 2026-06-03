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
 * @class BitSpan
 * @brief Read-only non-owning view into contiguous packed bit data.
 * 
 * This class provides a lightweight view over binary bit data stored in a packed byte buffer.
 * It stores a pointer to the backing byte buffer, a bit offset within the first byte, and a bit
 * count. It does not allocate, copy, own, or extend the lifetime of the data.
 * 
 * BitSpan supports:
 * - Construction from raw byte data, void pointers, and Vec<uint8>
 * - Bit-granular subspan operations (subspan, left, right, mid)
 * - Read-only bit access and iteration
 * - Comparison operations
 * - Counting, searching, and prefix/suffix checks
 * 
 * @note BitSpan does not own the underlying data. The user must ensure that the data remains valid
 *       for the lifetime of the BitSpan.
 * 
 * @note Bits within each byte are stored LSB-first, matching the layout used by BitArray and BitSet.
 *       That is, bit index 0 corresponds to the least-significant bit of the first backing byte.
 * 
 * @warning BitSpan intentionally exposes read-only access. Use BitArray when ownership or mutation
 *          is required.
 * 
 * @see BitArray for an owning dynamic bit container that can create and consume BitSpan values.
 * @see BitSet for a fixed-size, compile-time bit container.
 */
class NEX_API BitSpan {
public:
    // Forward declaration of ConstIterator
    class ConstIterator;

    // Type aliases for compatibility with standard container conventions
    using value_type = bool;
    using block_type = uint8;
    using size_type = usize;
    using difference_type = isize;
    using const_reference = value_type;
    using reference = value_type;
    using pointer = void;
    using const_pointer = void;
    using iterator = ConstIterator;
    using const_iterator = ConstIterator;
    using reverse_iterator = NEX_STD reverse_iterator<ConstIterator>;
    using const_reverse_iterator = NEX_STD reverse_iterator<ConstIterator>;

private:
    // Pointer to the backing byte buffer (non-owning)
    const block_type* data_;

    // Bit offset of the first bit within data_[0] (0-7)
    size_type bitOffset_;

    // Total number of bits in this view
    size_type bitCount_;

public:
    // Special value representing "not found" for search operations
    static constexpr size_type npos = NEX_STD numeric_limits<size_type>::max();

public:
    ////// Bit manipulation helper functions -----------------------------

    // Get the number of bits per byte
    static constexpr size_type bitsPerByte() noexcept { return 8; }

    // Get the byte index within the buffer for a given bit index
    static constexpr size_type byteIndex(size_type bitIndex) noexcept {
        return bitIndex / bitsPerByte();
    }

    // Get the bit offset within its byte for a given bit index
    static constexpr size_type bitOffset(size_type bitIndex) noexcept {
        return bitIndex % bitsPerByte();
    }

    // Get the LSB-first bitmask for a given bit index
    static constexpr block_type bitMask(size_type bitIndex) noexcept {
        return static_cast<block_type>(1U << bitOffset(bitIndex));
    }

    // Calculate the number of backing bytes required to hold a given number of bits
    static constexpr size_type bufferSizeForBits(size_type bitCount) noexcept {
        return (bitCount + bitsPerByte() - 1) / bitsPerByte();
    }

    ////// Constructors -----------------------------

    // Default constructor (empty view)
    constexpr BitSpan() noexcept
        : data_(nullptr), bitOffset_(0), bitCount_(0) {}

    // Construct from byte pointer and bit count (bit-zero-aligned start)
    constexpr BitSpan(const block_type* data, size_type bitCount) noexcept
        : data_(data), bitOffset_(0), bitCount_(bitCount) {}

    // Construct from void pointer and bit count (bit-zero-aligned start)
    constexpr BitSpan(const_void_ptr data, size_type bitCount) noexcept
        : data_(static_cast<const block_type*>(data)), bitOffset_(0), bitCount_(bitCount) {}

    // Construct from byte pointer, a bit offset into that pointer, and a bit count.
    // The byte pointer is advanced to absorb any full bytes in bitStartOffset, so that
    // data_[0] is always the byte containing bit index 0 of this view.
    constexpr BitSpan(const block_type* data, size_type bitStartOffset, size_type bitCount) noexcept
        : data_(data + bitStartOffset / bitsPerByte()),
          bitOffset_(bitStartOffset % bitsPerByte()),
          bitCount_(bitCount) {}

    // Construct from a dynamic array of bytes (Vec<block_type>) and explicit bit count
    BitSpan(const Vec<block_type>& buffer, size_type bitCount) noexcept
        : data_(buffer.data()), bitOffset_(0), bitCount_(bitCount) {}

    // Copy constructor
    constexpr BitSpan(const BitSpan& other) noexcept = default;

    // Copy assignment operator
    BitSpan& operator=(const BitSpan& other) noexcept = default;

    // Destructor
    ~BitSpan() = default;

    ////// ConstIterator --------------------------------------------------

    /**
     * @class ConstIterator
     * @brief Read-only random-access iterator over the bits of a BitSpan.
     * 
     * @details
     * Internally tracks an absolute bit index from the start of the backing byte buffer,
     * allowing it to work correctly across byte boundaries regardless of the span's bitOffset.
     * 
     * The dereference operator reads the bit at the current position from the underlying
     * byte buffer using LSB-first ordering.
     */
    class ConstIterator {
    public:
        // Iterator traits
        using iterator_category = NEX_STD random_access_iterator_tag;
        using value_type = BitSpan::value_type;
        using difference_type = BitSpan::difference_type;
        using reference = BitSpan::const_reference;
        using pointer = void;

        // Construct a ConstIterator from a byte buffer pointer and an absolute bit index
        constexpr ConstIterator(const block_type* data = nullptr, size_type absoluteIndex = 0) noexcept
            : data_(data), absoluteIndex_(absoluteIndex) {}

        // Dereference operator: returns the bit value at the current position
        reference operator*() const noexcept {
            return (data_[absoluteIndex_ / 8] &
                    static_cast<block_type>(1U << (absoluteIndex_ % 8))) != 0;
        }

        // Indexed dereference
        reference operator[](difference_type offset) const noexcept {
            size_type idx = static_cast<size_type>(
                static_cast<difference_type>(absoluteIndex_) + offset);
            return (data_[idx / 8] & static_cast<block_type>(1U << (idx % 8))) != 0;
        }

        // Increment / decrement
        ConstIterator& operator++() noexcept { ++absoluteIndex_; return *this; }
        ConstIterator operator++(int32) noexcept { ConstIterator tmp = *this; ++(*this); return tmp; }

        ConstIterator& operator--() noexcept { --absoluteIndex_; return *this; }
        ConstIterator operator--(int32) noexcept { ConstIterator tmp = *this; --(*this); return tmp; }

        // Arithmetic operators for random-access iteration
        ConstIterator& operator+=(difference_type offset) noexcept {
            absoluteIndex_ = static_cast<size_type>(
                static_cast<difference_type>(absoluteIndex_) + offset);
            return *this;
        }
        ConstIterator operator+(difference_type offset) const noexcept {
            ConstIterator tmp = *this;
            tmp += offset;
            return tmp;
        }

        ConstIterator& operator-=(difference_type offset) noexcept {
            absoluteIndex_ = static_cast<size_type>(
                static_cast<difference_type>(absoluteIndex_) - offset);
            return *this;
        }
        ConstIterator operator-(difference_type offset) const noexcept {
            ConstIterator tmp = *this;
            tmp -= offset;
            return tmp;
        }

        // Difference between two iterators
        difference_type operator-(const ConstIterator& other) const noexcept {
            return static_cast<difference_type>(absoluteIndex_) -
                   static_cast<difference_type>(other.absoluteIndex_);
        }

        // Support offset + iterator syntax
        friend ConstIterator operator+(difference_type offset, const ConstIterator& it) noexcept {
            return it + offset;
        }

        // Equality
        bool operator==(const ConstIterator& other) const noexcept {
            return data_ == other.data_ && absoluteIndex_ == other.absoluteIndex_;
        }

        // Inequality
        bool operator!=(const ConstIterator& other) const noexcept { return !(*this == other); }

        // Ordering
        bool operator<(const ConstIterator& other) const noexcept {
            return absoluteIndex_ < other.absoluteIndex_;
        }
        bool operator<=(const ConstIterator& other) const noexcept {
            return absoluteIndex_ <= other.absoluteIndex_;
        }
        bool operator>(const ConstIterator& other) const noexcept {
            return absoluteIndex_ > other.absoluteIndex_;
        }
        bool operator>=(const ConstIterator& other) const noexcept {
            return absoluteIndex_ >= other.absoluteIndex_;
        }

    private:
        const block_type* data_;
        size_type absoluteIndex_;
    };

    ////// Iterator support -----------------------------

    // Get const iterator to the beginning of the view
    constexpr const_iterator begin() const noexcept {
        return ConstIterator(data_, bitOffset_);
    }

    // Get const iterator to the beginning of the view
    constexpr const_iterator cbegin() const noexcept {
        return ConstIterator(data_, bitOffset_);
    }

    // Get const iterator to the end of the view
    constexpr const_iterator end() const noexcept {
        return ConstIterator(data_, bitOffset_ + bitCount_);
    }

    // Get const iterator to the end of the view
    constexpr const_iterator cend() const noexcept {
        return ConstIterator(data_, bitOffset_ + bitCount_);
    }

    // Get const reverse iterator to the beginning of the reversed view
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    // Get const reverse iterator to the beginning of the reversed view
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    // Get const reverse iterator to the end of the reversed view
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // Get const reverse iterator to the end of the reversed view
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    ////// Bit accessors --------------------------------------------------

    // Test bit at index (with bounds checking)
    const_reference at(size_type index) const;

    // Test bit at index (no bounds checking)
    constexpr const_reference operator[](size_type index) const noexcept {
        return testBit(index);
    }

    // Test bit at index (no bounds checking)
    constexpr const_reference testBit(size_type index) const noexcept {
        size_type abs = bitOffset_ + index;
        return (data_[abs / bitsPerByte()] & bitMask(abs)) != 0;
    }

    // Access the first bit
    constexpr const_reference front() const noexcept {
        return testBit(0);
    }

    // Access the last bit
    constexpr const_reference back() const noexcept {
        return testBit(bitCount_ - 1);
    }

    ////// Raw data access -----------------------------

    // Get pointer to the backing byte buffer
    constexpr const block_type* data() const noexcept { return data_; }

    // Get pointer to the backing byte buffer
    constexpr const block_type* getRawData() const noexcept { return data_; }

    // Get pointer to the backing byte buffer as void*
    constexpr const_void_ptr rawData() const noexcept {
        return static_cast<const_void_ptr>(data_);
    }

    // Get the bit offset of the first bit within data_[0] (0-7)
    constexpr size_type startBitOffset() const noexcept { return bitOffset_; }

    ////// Capacity and size-related operations -----------------------------

    // Get the number of bits in this view
    constexpr size_type size() const noexcept { return bitCount_; }

    // Get the number of bits in this view (same as size)
    constexpr size_type length() const noexcept { return bitCount_; }

    // Get the number of backing bytes spanned by this view (may include partial bytes at either end)
    constexpr size_type sizeBytes() const noexcept {
        return bufferSizeForBits(bitOffset_ + bitCount_);
    }

    // Check if the view is empty
    constexpr bool empty() const noexcept { return bitCount_ == 0; }

    // Get the maximum representable size
    constexpr size_type maxSize() const noexcept {
        return NEX_STD numeric_limits<size_type>::max() - 1;
    }

    ////// Conversion methods -----------------------------

    // Convert to dynamic array of booleans (Vec<bool>)
    Vec<value_type> toVec() const noexcept;

    ////// Modifiers -----------------------------

    // Remove the first n bits from the view
    void removePrefix(size_type n) noexcept;

    // Remove the last n bits from the view
    void removeSuffix(size_type n) noexcept;

    // Swap with another BitSpan
    void swap(BitSpan& other) noexcept;

    ////// Subspan operations -----------------------------

    // Get a subview starting at pos with count bits
    BitSpan subspan(size_type pos = 0, size_type count = npos) const;

    // Get the left part of the view (first count bits)
    BitSpan left(size_type count) const noexcept;

    // Get the right part of the view (last count bits)
    BitSpan right(size_type count) const noexcept;

    // Get the middle part of the view (count bits starting at start)
    BitSpan mid(size_type start, size_type count = npos) const;

    ////// Search operations -----------------------------

    // Find the first occurrence of a bit value starting at pos
    size_type indexOf(value_type bit, size_type pos = 0) const noexcept;

    // Find the first occurrence of a bit pattern starting at pos
    size_type indexOf(BitSpan pattern, size_type pos = 0) const noexcept;

    // Find the last occurrence of a bit value, searching backwards from pos
    size_type lastIndexOf(value_type bit, size_type pos = npos) const noexcept;

    // Find the last occurrence of a bit pattern, searching backwards from pos
    size_type lastIndexOf(BitSpan pattern, size_type pos = npos) const noexcept;

    // Check if the view starts with the given bit value
    bool startsWith(value_type bit) const noexcept;

    // Check if the view starts with the given bit pattern
    bool startsWith(BitSpan prefix) const noexcept;

    // Check if the view ends with the given bit value
    bool endsWith(value_type bit) const noexcept;

    // Check if the view ends with the given bit pattern
    bool endsWith(BitSpan suffix) const noexcept;

    // Check if the view contains the given bit value
    bool contains(value_type bit) const noexcept;

    // Check if the view contains the given bit pattern
    bool contains(BitSpan pattern) const noexcept;

    // Count the number of bits equal to the given value
    size_type count(value_type bit = true) const noexcept;

    ////// Boolean aggregate queries -----------------------------

    // Check if any bit is set to true
    bool any() const noexcept;

    // Check if all bits are set to true
    bool all() const noexcept;

    // Check if no bits are set to true
    bool none() const noexcept { return !any(); }

    ////// Comparison methods and operators -----------------------------

    // Lexicographic comparison with another BitSpan (bit by bit)
    int32 compare(const BitSpan& other) const noexcept;

    // Equality operator
    friend bool operator==(BitSpan lhs, BitSpan rhs) noexcept {
        return lhs.compare(rhs) == 0;
    }

    // Inequality operator
    friend bool operator!=(BitSpan lhs, BitSpan rhs) noexcept {
        return !(lhs == rhs);
    }

    // Less-than operator
    friend bool operator<(BitSpan lhs, BitSpan rhs) noexcept {
        return lhs.compare(rhs) < 0;
    }

    // Less-than-or-equal operator
    friend bool operator<=(BitSpan lhs, BitSpan rhs) noexcept {
        return !(rhs < lhs);
    }

    // Greater-than operator
    friend bool operator>(BitSpan lhs, BitSpan rhs) noexcept {
        return rhs < lhs;
    }

    // Greater-than-or-equal operator
    friend bool operator>=(BitSpan lhs, BitSpan rhs) noexcept {
        return !(lhs < rhs);
    }
};

// Hash support (for use in hash-based containers)
struct BitSpanHash {
    usize operator()(BitSpan view) const noexcept {
        usize hash = 0;
        for (BitSpan::size_type i = 0; i < view.size(); ++i) {
            hash = hash * 31 + static_cast<usize>(view[i]);
        }
        return hash;
    }
};

NEX_NAMESPACE_END

// Hash specialization for BitSpan
NEX_STD_BEGIN

template<>
struct hash<NEX_PREPEND_NAMESPACE(BitSpan)> {
    size_t operator()(NEX_PREPEND_NAMESPACE(BitSpan) view) const noexcept {
        return NEX_PREPEND_NAMESPACE(BitSpanHash){}(view);
    }
};

NEX_STD_END
