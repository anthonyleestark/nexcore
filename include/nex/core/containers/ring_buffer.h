/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/core/containers/details/ring_buffer_base.h"

NEX_NAMESPACE_BEGIN

/**
 * @class RingBuffer
 * @brief A dynamic-size circular buffer that efficiently manages a collection of elements in a FIFO manner.
 * 
 * @details
 * The RingBuffer is a data structure that uses a single, dynamic-size buffer as if it were connected end-to-end. 
 * It maintains two indices (head and tail) to track the start and end of the buffer, allowing for efficient 
 * addition and removal of elements without needing to shift data. The RingBuffer supports two overflow policies: 
 * Overwrite (where new elements overwrite the oldest ones when the buffer is full) and Reject (where new elements 
 * are rejected when the buffer is full).
 * 
 * Key Characteristics:
 * - Dynamic Capacity: The size of the buffer is determined at construction and can be changed if needed.
 * - Efficient Memory Usage: By using a circular approach, it minimizes memory overhead and avoids fragmentation.
 * - Fast Operations: Both adding and removing elements operate in constant time O(1), making it suitable for 
 *   performance-critical applications like real-time data processing or buffering.
 * - Flexible Overflow Handling: The choice between overwriting old data or rejecting new data allows for adaptability 
 *   based on application needs.
 * 
 * @tparam T The type of elements stored in the RingBuffer.
 */
template <
    typename T, 
    ring_buffer::details::OverflowPolicy Policy = ring_buffer::details::OverflowPolicy::Overwrite
>
using RingBuffer = ring_buffer::details::RingBufferBase<T, ring_buffer::details::DynamicStorage<T>, Policy>;

/**
 * @class StaticRingBuffer
 * @brief A fixed-size circular buffer that efficiently manages a collection of elements in a FIFO manner.
 * 
 * @details
 * The StaticRingBuffer is a data structure that uses a single, fixed-size buffer as if it were connected end-to-end. 
 * It maintains two indices (head and tail) to track the start and end of the buffer, allowing for efficient addition 
 * and removal of elements without needing to shift data. The StaticRingBuffer supports two overflow policies: 
 * Overwrite (where new elements overwrite the oldest ones when the buffer is full) and Reject (where new elements 
 * are rejected when the buffer is full).
 * 
 * Key Characteristics:
 * - Fixed Capacity: The size of the buffer is determined at compile time and cannot be changed at runtime.
 * - Zero Dynamic Memory Allocation: The buffer is allocated on the stack or as part of a larger structure, eliminating
 *   the need for dynamic memory management and reducing overhead.
 * - Efficient Memory Usage: By using a circular approach, it minimizes memory overhead and avoids fragmentation.
 * - Fast Operations: Both adding and removing elements operate in constant time O(1), making it suitable for 
 *   performance-critical applications like real-time data processing or buffering.
 * - Flexible Overflow Handling: The choice between overwriting old data or rejecting new data allows for adaptability 
 *   based on application needs.
 * 
 * @tparam T The type of elements stored in the StaticRingBuffer.
 * @tparam Capacity The maximum number of elements the buffer can hold (must be greater than 0).
 * @tparam Policy The policy for handling overflow situations (default is Overwrite).
 * 
 * @note
 * The StaticRingBuffer is designed for scenarios where a fixed-size buffer is sufficient and dynamic memory allocation 
 * is not desired or possible. It is ideal for embedded systems, real-time applications, or any situation where 
 * predictable memory usage is important.
 * 
 * @see RingBuffer for a dynamic-size version of the circular buffer.
 */
template <
    typename T, 
    usize Capacity, 
    ring_buffer::details::OverflowPolicy Policy = ring_buffer::details::OverflowPolicy::Overwrite
>
requires (Capacity > 0) // Capacity must be greater than 0
using StaticRingBuffer = ring_buffer::details::RingBufferBase<T, ring_buffer::details::StaticStorage<T, Capacity>, Policy>;

NEX_NAMESPACE_END
