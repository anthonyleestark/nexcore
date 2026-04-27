/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <type_traits>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/linear.h"
#include "nex/base/wrappers.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @enum OverflowPolicy
 * @brief Defines policies for handling overflow situations in data structures like RingBuffer.
 */
enum class OverflowPolicy { 
    Overwrite,      ///< When the buffer is full, new elements will overwrite the oldest ones.
    Reject          ///< When the buffer is full, new elements will be rejected and not added to the buffer.
};

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
template <typename T, OverflowPolicy Policy = OverflowPolicy::Overwrite>
class NEX_EXPORT RingBuffer {
public:
    // Type aliases for convenience
    using value_type = T;
    using size_type = usize;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

private:
    // Internal buffer to store elements
    UniquePtr<value_type[]> buffer_;
    size_type capacity_;                // Maximum number of elements the buffer can hold
    size_type head_;                    // Index of the oldest element
    size_type tail_;                    // Index of the next insertion point
    size_type count_;                    // Current number of elements in the buffer

public:
    ////// Construction ------------------------------------------------

    // Constructor with specified capacity
    explicit RingBuffer(size_type capacity)
        : buffer_(NEX_STD make_unique<value_type[]>(capacity)), 
          capacity_(capacity), head_(0), tail_(0), count_(0) {
        NEX_ASSERT_MSG(capacity > 0, "Capacity must be greater than 0"); // debug runtime-check only
    }

    // Destructor
    ~RingBuffer() = default;

    // Copy constructor
    RingBuffer(const RingBuffer& other)
        : capacity_(other.capacity_), head_(other.head_), tail_(other.tail_), count_(other.count_) {
        buffer_ = NEX_STD make_unique<value_type[]>(capacity_);
        for (size_type i = 0; i < capacity_; ++i) {
            buffer_[i] = other.buffer_[i];
        }
    }

    // Copy assignment operator
    RingBuffer& operator=(const RingBuffer& other) {
        if (this != &other) {
            buffer_ = NEX_STD make_unique<value_type[]>(other.capacity_);
            capacity_ = other.capacity_;
            head_ = other.head_;
            tail_ = other.tail_;
            count_ = other.count_;
            for (size_type i = 0; i < capacity_; ++i) {
                buffer_[i] = other.buffer_[i];
            }
        }
        return *this;
    }

    // Move constructor
    RingBuffer(RingBuffer&& other) noexcept
        : buffer_(NEX_STD move(other.buffer_)), capacity_(other.capacity_), 
          head_(other.head_), tail_(other.tail_), count_(other.count_) {}

    // Move assignment operator
    RingBuffer& operator=(RingBuffer&& other) noexcept {
        if (this != &other) {
            buffer_ = NEX_STD move(other.buffer_);
            capacity_ = other.capacity_;
            head_ = other.head_;
            tail_ = other.tail_;
            count_ = other.count_;
        }
        return *this;
    }

    ////// Capacity and size management -----------------------------------------------

    // Get the current number of elements in the buffer
    constexpr size_type size() const noexcept { return count_; }

    // Get the maximum capacity of the buffer
    constexpr size_type capacity() const noexcept { return capacity_; }

    // Check if the buffer is empty
    constexpr bool empty() const noexcept { return count_ == 0; }

    // Check if the buffer is full
    constexpr bool full() const noexcept { return count_ == capacity_; }

    // Clear the buffer (only make it logically empty, does not release memory)
    void clear() noexcept {
        head_ = 0; tail_ = 0; count_ = 0;
        // to avoid memory leaks, we can reset all elements to default value if the value_type is an object type
        if constexpr (NEX_STD is_object_v<value_type>) {
            for (size_type i = 0; i < capacity_; ++i) {
                buffer_[i] = value_type();
            }
        }
    }

    // Resize the buffer to a new capacity
    void resize(size_type newCapacity) {
        if (newCapacity == capacity_) return; // No change needed

        // Create a new buffer with the new capacity
        UniquePtr<value_type[]> new_buffer = NEX_STD make_unique<value_type[]>(newCapacity);
        size_type new_count = count_ < newCapacity ? count_ : newCapacity;

        // Copy existing elements to the new buffer
        for (size_type i = 0; i < new_count; ++i) {
            new_buffer[i] = buffer_[(head_ + i) % capacity_];
        }

        // Replace old buffer with new buffer
        buffer_ = NEX_STD move(new_buffer);

        // Update capacity and reset indices
        capacity_ = newCapacity;
        head_ = 0;
        tail_ = new_count % capacity_;
        count_ = new_count;
    }

    // Shrink the buffer to fit the current number of elements (releases unused memory)
    void shrinkToFit() {
        if (count_ == capacity_) return; // Already at optimal size
        resize(count_);
    }

    ////// Element access ------------------------------------------------

    // Access the front element (oldest)
    reference front() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty"); // debug runtime-check only
        return buffer_[head_];
    }

    // Access the front element (oldest; constant reference)
    const_reference front() const {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty"); // debug runtime-check only
        return buffer_[head_];
    }

    // Access the back element (newest)
    reference back() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty"); // debug runtime-check only
        return buffer_[(tail_ + capacity_ - 1) % capacity_];
    }

    // Access the back element (newest; constant reference)
    const_reference back() const {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty"); // debug runtime-check only
        return buffer_[(tail_ + capacity_ - 1) % capacity_];
    }

    // Access element at specific index (0 is front, size-1 is back)
    reference at(size_type index) {
        NEX_ASSERT_MSG(index < count_, "Index out of bounds"); // debug runtime-check only
        return buffer_[(head_ + index) % capacity_];
    }

    // Access element at specific index (0 is front, size-1 is back; constant reference)
    const_reference at(size_type index) const {
        NEX_ASSERT_MSG(index < count_, "Index out of bounds"); // debug runtime-check only
        return buffer_[(head_ + index) % capacity_];
    }

    // Access element at specific index without bounds checking
    reference operator[](size_type index) {
        return buffer_[(head_ + index) % capacity_];
    }

    // Access element at specific index without bounds checking (constant reference)
    const_reference operator[](size_type index) const {
        return buffer_[(head_ + index) % capacity_];
    }

    ////// Modifiers ------------------------------------------------

    // Add an element to the back of the buffer
    bool pushBack(const value_type& value) {
        if (full()) {
            if constexpr (Policy == OverflowPolicy::Reject) {
                return false;                               // Reject new element
            } else {
                tail_ = (tail_ + 1) % capacity_;            // Move tail forward
            }
        }
        buffer_[head_] = value;                             // Add new element
        head_ = (head_ + 1) % capacity_;                    // Move head forward
        ++count_;
        return true;
    }

    // Add an element to the back of the buffer (move semantics)
    bool pushBack(value_type&& value) {
        if (full()) {
            if constexpr (Policy == OverflowPolicy::Reject) {
                return false;                               // Reject new element
            } else {
                tail_ = (tail_ + 1) % capacity_;            // Move tail forward
            }
        }
        buffer_[head_] = NEX_STD move(value);               // Add new element using move
        head_ = (head_ + 1) % capacity_;                    // Move head forwards
        ++count_;
        return true;
    }

    // Remove the front element from the buffer
    void popFront() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");        // debug runtime-check only
        head_ = (head_ + 1) % capacity_;                    // Move head forward
        --count_;
    }

    // Remove the back element from the buffer
    void popBack() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");        // debug runtime-check only
        tail_ = (tail_ + capacity_ - 1) % capacity_;        // Move tail backward
        --count_;
    }

    // Clear the buffer and releae memory
    void reset() {
        clear();
        buffer_.reset();    // Release memory
    }
};

NEX_NAMESPACE_END
