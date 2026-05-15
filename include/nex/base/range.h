/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @struct Range
 * @brief  Half-open range represented by a start offset and a length.
 *
 * @details
 * The range covers values in [start, start + length).
 * This representation is useful for slices, indexes, byte windows, and other offset/count APIs.
 */
template <typename T>
struct Range {
    T start = {};       // Starting offset or index
    T length = {};      // Length or count of the range

    // Default constructor
    constexpr Range() noexcept = default;

    // Constructor with start and length parameters
    constexpr Range(T startValue, T lengthValue) noexcept
        : start(startValue), length(lengthValue) {}

    // Get the end offset of the range (exclusive)
    constexpr T end() const noexcept {
        return start + length;
    }

    // Check if the range is empty (length is zero)
    constexpr bool empty() const noexcept {
        return length == T{};
    }

    // Check if the range contains a specific value
    constexpr bool contains(T value) const noexcept {
        return value >= start && value < end();
    }

    // Check if the range contains another range
    constexpr bool contains(Range other) const noexcept {
        return other.start >= start && other.end() <= end();
    }

    // Check if the range intersects with another range
    constexpr bool intersects(Range other) const noexcept {
        return start < other.end() && other.start < end();
    }

    // Get the intersection of the range with another range
    constexpr Range intersection(Range other) const noexcept {
        const T newStart = start > other.start ? start : other.start;
        const T newEnd = end() < other.end() ? end() : other.end();
        return newEnd > newStart ? Range(newStart, newEnd - newStart) : Range(newStart, T{});
    }

    // Equality operator
    constexpr bool operator==(const Range&) const noexcept = default;
};

// ============================================================================
// Type aliases for common range types
// ============================================================================

using IndexRange = Range<usize>;        // Range for container indexes and counts
using SignedRange = Range<isize>;       // Range for signed offsets
using ByteRange = Range<usize>;         // Range for byte offsets and lengths

NEX_NAMESPACE_END
