/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @class  Range
 * @brief  Half-open range represented by a start offset and a length.
 *
 * @details
 * The range covers values in [start, start + length).
 * This representation is useful for slices, indexes, byte windows, and other offset/count APIs.
 */
template <typename ValueType, typename SizeType = meta::MakeUnsignedT<ValueType>>
class NEX_API Range {
public:
    using value_type = ValueType;
    using size_type = SizeType;
    using difference_type = meta::MakeSignedT<size_type>;

    // Ensure that ValueType and SizeType are compatible for arithmetic operations
    static_assert(requires(value_type a, size_type b) { 
        { a + b } -> meta::SameAs<value_type>; 
    });

private:
    value_type start_ = {};     // Starting offset or index
    size_type length_ = {};     // Length or count of the range

public:
    // Default constructor
    constexpr Range() noexcept = default;

    // Constructs a range with a specified start offset and length
    constexpr Range(value_type start, size_type length) noexcept
        : start_(start), length_(length) {}

    // Get the size of the range, which is the number of elements it covers
    NEX_NODISCARD constexpr size_type size() const noexcept {
        return length_;
    }

    // Get the length of the range, which is the number of elements it covers
    NEX_NODISCARD constexpr size_type length() const noexcept {
        return length_;
    }

    // Determine if the range is empty (length is zero)
    NEX_NODISCARD constexpr bool empty() const noexcept 
        requires requires(size_type len) { 
            { len == size_type{} } -> meta::ConvertibleTo<bool>; 
        }
    {
        return static_cast<bool>(length() == size_type{});
    }

    // Get the starting offset of the range
    NEX_NODISCARD constexpr value_type start() const noexcept {
        return start_;
    }

    // Get the end offset of the range (exclusive)
    NEX_NODISCARD constexpr value_type end() const noexcept {
        return start() + length();
    }

    // Determine if the range contains a specific value
    template <typename TargetType>
    NEX_NODISCARD constexpr bool contains(const TargetType& value) const noexcept
        requires requires(TargetType target, value_type origin) {
            { target >= origin } -> meta::ConvertibleTo<bool>;
            { target <  origin } -> meta::ConvertibleTo<bool>;
        }
    {
        return static_cast<bool>(value >= start()) && static_cast<bool>(value < end());
    }

    // Determine if the range contains another range
    template <typename OtherValueType, typename OtherSizeType>
    NEX_NODISCARD constexpr bool contains(const Range<OtherValueType, OtherSizeType>& other) const noexcept 
        requires 
            requires(OtherValueType otherStart, value_type thisStart) {
                { otherStart >= thisStart } -> meta::ConvertibleTo<bool>;
            } && requires(OtherValueType otherEnd, value_type thisEnd) {
                { otherEnd <= thisEnd } -> meta::ConvertibleTo<bool>;
            }
    {
        return static_cast<bool>(other.start() >= start()) && static_cast<bool>(other.end() <= end());
    }

    // Determine if the range intersects with another range
    template <typename OtherValueType, typename OtherSizeType>
    NEX_NODISCARD constexpr bool intersects(const Range<OtherValueType, OtherSizeType>& other) const noexcept 
        requires 
            requires(value_type thisStart, OtherValueType otherEnd) {
                { thisStart < otherEnd } -> meta::ConvertibleTo<bool>;
            } && requires(OtherValueType otherStart, value_type thisEnd) {
                { otherStart < thisEnd } -> meta::ConvertibleTo<bool>;
            }
    {
        return static_cast<bool>(start() < other.end()) && static_cast<bool>(other.start() < end());
    }

    // Get the intersection of the range with another range
    template <typename OtherValueType, typename OtherSizeType>
    NEX_NODISCARD constexpr Range intersection(const Range<OtherValueType, OtherSizeType>& other) const noexcept 
        requires 
            requires(value_type thisStart, OtherValueType otherStart) {
                { thisStart > otherStart } -> meta::ConvertibleTo<bool>;
            } && requires(value_type thisEnd, OtherValueType otherEnd) {
                { thisEnd < otherEnd } -> meta::ConvertibleTo<bool>;
            }
    {
        const value_type newStart = start() > other.start() ? start() : other.start();
        const value_type newEnd = end() < other.end() ? end() : other.end();
        return (newEnd > newStart) 
            ? Range(newStart, static_cast<size_type>(newEnd - newStart))
            : Range(newStart, size_type{});
    }

    // Equality operator for comparing two ranges of the same type
    constexpr bool operator==(const Range&) const noexcept = default;

    // Equality operator for comparing two ranges of different types
    template <typename OtherValueType, typename OtherSizeType>
    constexpr bool operator==(const Range<OtherValueType, OtherSizeType>& other) const noexcept 
        requires 
            requires(value_type thisStart, OtherValueType otherStart) {
                { thisStart == otherStart } -> meta::ConvertibleTo<bool>;
            } && requires(size_type thisLen, OtherSizeType otherLen) {
                { thisLen == otherLen } -> meta::ConvertibleTo<bool>;
            }
    {
        return static_cast<bool>(start() == other.start()) && static_cast<bool>(length() == other.length());
    }
};

// =================================================================================
// Common range type aliases for convenience
// =================================================================================

// Integral range types with start and length of the same type
using IndexRange = Range<usize, usize>;        // Represents a range of indices for arrays or containers
using SignedRange = Range<isize, isize>;       // Represents a range of signed offsets or indexes

// Represents a range of byte offsets and lengths of logical memory blocks, 
// useful for memory management and buffer operations
using ByteRange = Range<usize, usize>;

// Represents a range of pointer offsets and lengths, 
// useful for pointer arithmetic and memory access
template <typename Type>
using PointerRange = Range<Type*, usize>;

NEX_NAMESPACE_END
