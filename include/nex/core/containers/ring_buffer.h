/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <type_traits>
#include <iterator>
#include <utility>

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
    // Forward declaration of iterators
    class Iterator;
    class ConstIterator;
    using ReverseIterator = NEX_STD reverse_iterator<Iterator>;
    using ConstReverseIterator = NEX_STD reverse_iterator<ConstIterator>;

    // Type aliases for convenience
    using value_type = T;
    using size_type = usize;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = isize;
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using reverse_iterator = ReverseIterator;
    using const_reverse_iterator = ConstReverseIterator;

private:
    UniquePtr<value_type[]> buffer_;    // Internal buffer to store elements
    size_type capacity_;                // Maximum number of elements the buffer can hold
    size_type head_;                    // Index of the oldest element
    size_type tail_;                    // Index of the next insertion point
    size_type count_;                   // Current number of elements in the buffer

public:
    ////// Construction ------------------------------------------------

    // Constructor with specified capacity
    explicit RingBuffer(size_type capacity)
        : buffer_(NEX_STD make_unique<value_type[]>(capacity > 0 ? capacity : 1)), 
          capacity_(capacity > 0 ? capacity : 1), head_(0), tail_(0), count_(0) {}

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

    // Clear the buffer
    void clear() noexcept {
        // to avoid potential resource leaks for non-trivial types
        if constexpr (!NEX_STD is_trivially_destructible_v<value_type>) {
            for (size_type i = 0; i < count_; ++i) {
                buffer_[(head_ + i) % capacity_] = value_type(); // reset to default value
            }
        }
        head_ = 0; tail_ = 0; count_ = 0;
    }

    // Resize the buffer to a new capacity
    void resize(size_type newCapacity) {
        if (newCapacity == 0) return;
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
        resize(count_ > 0 ? count_ : 1);
    }

    ////// Element access ------------------------------------------------

    // Access the front element (oldest)
    reference front() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");            // debug runtime-check only
        return buffer_[head_];
    }

    // Access the front element (oldest; constant reference)
    const_reference front() const {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");            // debug runtime-check only
        return buffer_[head_];
    }

    // Access the back element (newest)
    reference back() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");            // debug runtime-check only
        return buffer_[(tail_ + capacity_ - 1) % capacity_];
    }

    // Access the back element (newest; constant reference)
    const_reference back() const {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");            // debug runtime-check only
        return buffer_[(tail_ + capacity_ - 1) % capacity_];
    }

    // Access element at specific index (0 is front, size-1 is back)
    reference at(size_type index) {
        NEX_ASSERT_MSG(index < count_, "Index out of bounds");  // debug runtime-check only
        return buffer_[(head_ + index) % capacity_];
    }

    // Access element at specific index (0 is front, size-1 is back; constant reference)
    const_reference at(size_type index) const {
        NEX_ASSERT_MSG(index < count_, "Index out of bounds");  // debug runtime-check only
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

    ////// Iterators ------------------------------------------------

    /**
     * @class RingBuffer::Iterator
     * @brief An iterator for traversing the elements of the RingBuffer in a linear fashion.
     * 
     * @details
     * The Iterator class provides a way to iterate through the elements of the RingBuffer from the front (oldest) 
     * to the back (newest). It supports standard iterator operations such as increment, decrement, dereference, 
     * and comparison. The iterator is designed to work seamlessly with range-based for loops and STL algorithms.
     * 
     * @note 
     * The iterator does not support concurrent modifications to the RingBuffer while iterating, and behavior 
     * is undefined if the buffer is modified (such as push/pop or clear) during iteration.
     * 
     * @see RingBuffer::ConstIterator for a read-only version of the iterator.
     */
    class Iterator {
    public:
        // Iterator traits
        using iterator_category = NEX_STD random_access_iterator_tag;
        using value_type = RingBuffer::value_type;
        using size_type = RingBuffer::size_type;
        using difference_type = RingBuffer::difference_type;
        using pointer = RingBuffer::pointer;
        using const_pointer = RingBuffer::const_pointer;
        using reference = RingBuffer::reference;
        using const_reference = RingBuffer::const_reference;

        // Allow ConstIterator to access private members of Iterator
        friend class ConstIterator;

        // Constructor
        constexpr explicit Iterator(RingBuffer* buffer, size_type index) 
            : buffer_(buffer), index_(index) {}

        // Dereference operators
        constexpr reference operator*() { return (*buffer_)[index_]; }
        constexpr pointer operator->() { return &(*buffer_)[index_]; }

        // Iterator operations
        constexpr Iterator& operator++() { ++index_; return *this; }
        constexpr Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }

        constexpr Iterator& operator--() { --index_; return *this; }
        constexpr Iterator operator--(int) { Iterator temp = *this; --(*this); return temp; }

        // Arithmetic operators for random access iterator
        constexpr Iterator operator+(difference_type offset) const { 
            return Iterator(buffer_, static_cast<size_type>(static_cast<difference_type>(index_) + offset)); 
        }
        constexpr Iterator& operator+=(difference_type n) { 
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) + n); 
            return *this; 
        }

        constexpr Iterator operator-(difference_type offset) const { 
            return Iterator(buffer_, static_cast<size_type>(static_cast<difference_type>(index_) - offset)); 
        }
        constexpr Iterator& operator-=(difference_type n) { 
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) - n); 
            return *this; 
        }

        // Friend function for addition with difference_type on the left
        friend constexpr Iterator operator+(difference_type n, const Iterator& it) { 
            return Iterator(it.buffer_, static_cast<size_type>(static_cast<difference_type>(it.index_) + n)); 
        }

        // Difference operator for random access iterator
        constexpr difference_type operator-(const Iterator& other) const { 
            return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_); 
        }

        // Subscript operators for random access iterator
        reference operator[](difference_type offset) { 
            return (*buffer_)[static_cast<size_type>(static_cast<difference_type>(index_) + offset)]; 
        }
        const_reference operator[](difference_type offset) const { 
            return (*buffer_)[static_cast<size_type>(static_cast<difference_type>(index_) + offset)]; 
        }

        // Equality operators for iterators
        constexpr bool operator==(const Iterator& other) const { 
            return buffer_ == other.buffer_ && index_ == other.index_; 
        }
        constexpr bool operator!=(const Iterator& other) const { return !(*this == other); }
        
        // Comparison operators for random access iterator
        constexpr bool operator<(const Iterator& other) const { 
            return buffer_ < other.buffer_ || (buffer_ == other.buffer_ && index_ < other.index_); 
        }
        constexpr bool operator>(const Iterator& other) const { 
            return buffer_ > other.buffer_ || (buffer_ == other.buffer_ && index_ > other.index_); 
        }
        constexpr bool operator<=(const Iterator& other) const { 
            return buffer_ < other.buffer_ || (buffer_ == other.buffer_ && index_ <= other.index_); 
        }
        constexpr bool operator>=(const Iterator& other) const { 
            return buffer_ > other.buffer_ || (buffer_ == other.buffer_ && index_ >= other.index_); 
        }

    private:
        RingBuffer* buffer_;
        size_type index_;
    };

    // Get an iterator to the beginning of the buffer
    constexpr Iterator begin() noexcept { return Iterator(this, 0); }

    // Get an iterator to the end of the buffer
    constexpr Iterator end() noexcept { return Iterator(this, count_); }

    /**
     * @class RingBuffer::ConstIterator
     * @brief A constant iterator for traversing the elements of the RingBuffer in a read-only manner.
     * 
     * @details
     * The ConstIterator class provides a way to iterate through the elements of the RingBuffer from the front (oldest) 
     * to the back (newest) without allowing modification of the elements. It supports standard iterator operations 
     * such as increment, decrement, dereference, and comparison. The const iterator is designed to work seamlessly 
     * with range-based for loops and STL algorithms that require read-only access.
     * 
     * @note 
     * The const iterator does not support concurrent modifications to the RingBuffer while iterating, and behavior 
     * is undefined if the buffer is modified (such as push/pop or clear) during iteration.
     * 
     * @see RingBuffer::Iterator for a read-write version of the iterator.
     */
    class ConstIterator {
    public:
        // Iterator traits
        using iterator_category = NEX_STD random_access_iterator_tag;
        using value_type = RingBuffer::value_type;
        using size_type = RingBuffer::size_type;
        using difference_type = RingBuffer::difference_type;
        using pointer = RingBuffer::const_pointer;
        using const_pointer = RingBuffer::const_pointer;
        using reference = RingBuffer::const_reference;
        using const_reference = RingBuffer::const_reference;

        // Constructor
        constexpr explicit ConstIterator(const RingBuffer* buffer, size_type index) 
            : buffer_(buffer), index_(index) {}

         // Dereference operators
        constexpr reference operator*() const { return (*buffer_)[index_]; }
        constexpr pointer operator->() const { return &(*buffer_)[index_]; }

        // Iterator operations
        constexpr ConstIterator& operator++() { ++index_; return *this; }
        constexpr ConstIterator operator++(int) { ConstIterator temp = *this; ++(*this); return temp; }

        constexpr ConstIterator& operator--() { --index_; return *this; }
        constexpr ConstIterator operator--(int) { ConstIterator temp = *this; --(*this); return temp; }

        // Arithmetic operators for random access iterator
        constexpr ConstIterator operator+(difference_type offset) const { 
            return ConstIterator(buffer_, static_cast<size_type>(static_cast<difference_type>(index_) + offset)); 
        }
        constexpr ConstIterator& operator+=(difference_type n) { 
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) + n); 
            return *this; 
        }

        constexpr ConstIterator operator-(difference_type offset) const { 
            return ConstIterator(buffer_, static_cast<size_type>(static_cast<difference_type>(index_) - offset)); 
        }
        constexpr ConstIterator& operator-=(difference_type n) { 
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) - n); 
            return *this; 
        }

        // Friend function for addition with difference_type on the left
        friend constexpr ConstIterator operator+(difference_type n, const ConstIterator& it) { 
            return ConstIterator(it.buffer_, static_cast<size_type>(static_cast<difference_type>(it.index_) + n)); 
        }

        // Difference operator for random access iterator
        constexpr difference_type operator-(const ConstIterator& other) const {
            return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_);
        }

        // Subscript operators for random access iterator
        reference operator[](difference_type offset) const { 
            return (*buffer_)[static_cast<size_type>(static_cast<difference_type>(index_) + offset)]; 
        }

        // Equality operators for const iterators
        constexpr bool operator==(const ConstIterator& other) const { 
            return buffer_ == other.buffer_ && index_ == other.index_; 
        }
        constexpr bool operator!=(const ConstIterator& other) const { return !(*this == other); }

        // Comparison operators for random access iterator
        constexpr bool operator<(const ConstIterator& other) const { 
            return buffer_ < other.buffer_ || (buffer_ == other.buffer_ && index_ < other.index_); 
        }
        constexpr bool operator>(const ConstIterator& other) const { 
            return buffer_ > other.buffer_ || (buffer_ == other.buffer_ && index_ > other.index_); 
        }
        constexpr bool operator<=(const ConstIterator& other) const { 
            return buffer_ < other.buffer_ || (buffer_ == other.buffer_ && index_ <= other.index_); 
        }
        constexpr bool operator>=(const ConstIterator& other) const { 
            return buffer_ > other.buffer_ || (buffer_ == other.buffer_ && index_ >= other.index_); 
        }

    private:
        const RingBuffer* buffer_;
        size_type index_;
    };

    // Get a constant iterator to the beginning of the buffer
    constexpr ConstIterator begin() const noexcept { return ConstIterator(this, 0); }

    // Get a constant iterator to the end of the buffer
    constexpr ConstIterator end() const noexcept { return ConstIterator(this, count_); }

    // Get a constant iterator to the beginning of the buffer
    constexpr ConstIterator cbegin() const noexcept { return ConstIterator(this, 0); }

    // Get a constant iterator to the end of the buffer
    constexpr ConstIterator cend() const noexcept { return ConstIterator(this, count_); }

    // Get a reverse iterator to the beginning of the reversed buffer (i.e., end of the normal buffer)
    constexpr ReverseIterator rbegin() noexcept { return ReverseIterator(end()); }

    // Get a reverse iterator to the end of the reversed buffer (i.e., beginning of the normal buffer)
    constexpr ReverseIterator rend() noexcept { return ReverseIterator(begin()); }

    // Get a const reverse iterator to the beginning of the reversed buffer (i.e., end of the normal buffer)
    constexpr ConstReverseIterator rbegin() const noexcept { return ConstReverseIterator(end()); }

    // Get a const reverse iterator to the end of the reversed buffer (i.e., beginning of the normal buffer)
    constexpr ConstReverseIterator rend() const noexcept { return ConstReverseIterator(begin()); }

    // Get a const reverse iterator to the beginning of the reversed buffer (i.e., end of the normal buffer)
    constexpr ConstReverseIterator crbegin() const noexcept { return ConstReverseIterator(cend()); }

    // Get a const reverse iterator to the end of the reversed buffer (i.e., beginning of the normal buffer)
    constexpr ConstReverseIterator crend() const noexcept { return ConstReverseIterator(cbegin()); }

    ////// Modifiers ------------------------------------------------

    // Add an element to the back of the buffer
    bool pushBack(const value_type& value) {
        if (full()) {
            if constexpr (Policy == OverflowPolicy::Reject) {
                return false;                               // Reject new element
            } else {
                buffer_[tail_] = value;                     // Overwrite oldest element
                tail_ = (tail_ + 1) % capacity_;            // Move tail forward
                head_ = tail_;                              // Oldest element follows inserted element
                return true;
            }
        }
        buffer_[tail_] = value;                             // Add new element
        tail_ = (tail_ + 1) % capacity_;                    // Move tail forward
        ++count_;
        return true;
    }

    // Add an element to the back of the buffer (move semantics)
    bool pushBack(value_type&& value) {
        if (full()) {
            if constexpr (Policy == OverflowPolicy::Reject) {
                return false;                               // Reject new element
            } else {
                buffer_[tail_] = NEX_STD move(value);       // Overwrite oldest element
                tail_ = (tail_ + 1) % capacity_;            // Move tail forward
                head_ = tail_;                              // Oldest element follows inserted element
                return true;
            }
        }
        buffer_[tail_] = NEX_STD move(value);               // Add new element using move
        tail_ = (tail_ + 1) % capacity_;                    // Move tail forwards
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
};

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
template <typename T, usize Capacity, OverflowPolicy Policy = OverflowPolicy::Overwrite>
requires (Capacity > 0)
class NEX_EXPORT StaticRingBuffer {
public:
    // Forward declaration of iterators
    class Iterator;
    class ConstIterator;
    using ReverseIterator = NEX_STD reverse_iterator<Iterator>;
    using ConstReverseIterator = NEX_STD reverse_iterator<ConstIterator>;

    // Type aliases for convenience
    using value_type = T;
    using size_type = usize;
    using difference_type = isize;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using reverse_iterator = ReverseIterator;
    using const_reverse_iterator = ConstReverseIterator;

private:
    value_type buffer_[Capacity] = {};      // Internal buffer to store elements (default-initialized)
    size_type head_ = 0;                    // Index of the oldest element
    size_type tail_ = 0;                    // Index of the next insertion point
    size_type count_ = 0;                   // Current number of elements in the buffer

public:
    ////// Construction ------------------------------------------------

    // Default constructor
    constexpr StaticRingBuffer() = default;

    // Default copy and move semantics
    NEX_DEFAULT_COPY(StaticRingBuffer);
    NEX_DEFAULT_MOVE(StaticRingBuffer);
    
    // Destructor
    ~StaticRingBuffer() = default;

    ////// Capacity and size management -----------------------------------------------

    // Get the current number of elements in the buffer
    constexpr size_type size() const noexcept { return count_; }

    // Get the maximum capacity of the buffer
    constexpr size_type capacity() const noexcept { return Capacity; }

    // Check if the buffer is empty
    constexpr bool empty() const noexcept { return count_ == 0; }

    // Check if the buffer is full
    constexpr bool full() const noexcept { return count_ == Capacity; }

    // Clear the buffer
    constexpr void clear() noexcept {
        // to avoid potential resource leaks for non-trivial types
        if constexpr (!NEX_STD is_trivially_destructible_v<value_type>) {
            for (size_type i = 0; i < count_; ++i) {
                buffer_[(head_ + i) % Capacity] = value_type(); // reset to default value
            }
        }
        head_ = 0; tail_ = 0; count_ = 0;
    }

    ////// Element access ------------------------------------------------

    // Access the front element (oldest)
    constexpr reference front() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");                // debug runtime-check only
        return buffer_[head_];
    }

    // Access the front element (oldest; constant reference)
    constexpr const_reference front() const {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");                // debug runtime-check only
        return buffer_[head_];
    }

    // Access the back element (newest)
    constexpr reference back() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");                // debug runtime-check only
        return buffer_[(tail_ + Capacity - 1) % Capacity];
    }

    // Access the back element (newest; constant reference)
    constexpr const_reference back() const {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");                // debug runtime-check only
        return buffer_[(tail_ + Capacity - 1) % Capacity];
    }

    // Access element at specific index (0 is front, size-1 is back)
    constexpr reference at(size_type index) {
        NEX_ASSERT_MSG(index < count_, "Index out of bounds");      // debug runtime-check only
        return buffer_[(head_ + index) % Capacity];
    }

    // Access element at specific index (0 is front, size-1 is back; constant reference)
    constexpr const_reference at(size_type index) const {
        NEX_ASSERT_MSG(index < count_, "Index out of bounds");      // debug runtime-check only
        return buffer_[(head_ + index) % Capacity];
    }

    // Access element at specific index without bounds checking
    constexpr reference operator[](size_type index) {
        return buffer_[(head_ + index) % Capacity];
    }

    // Access element at specific index without bounds checking (constant reference)
    constexpr const_reference operator[](size_type index) const {
        return buffer_[(head_ + index) % Capacity];
    }

    ////// Iterators ------------------------------------------------

    /**
     * @class StaticRingBuffer::Iterator
     * @brief An iterator for traversing the elements of the StaticRingBuffer in a linear fashion.
     * 
     * @details
     * The Iterator class provides a way to iterate through the elements of the StaticRingBuffer from the front (oldest) 
     * to the back (newest). It supports standard iterator operations such as increment, decrement, dereference, 
     * and comparison. The iterator is designed to work seamlessly with range-based for loops and STL algorithms.
     * 
     * @note 
     * The iterator does not support concurrent modifications to the StaticRingBuffer while iterating, and behavior 
     * is undefined if the buffer is modified (such as push/pop or clear) during iteration.
     * 
     * @see StaticRingBuffer::ConstIterator for a read-only version of the iterator.
     */
    class Iterator {
    public:
        // Iterator traits
        using iterator_category = NEX_STD random_access_iterator_tag;
        using value_type = StaticRingBuffer::value_type;
        using size_type = StaticRingBuffer::size_type;
        using difference_type = StaticRingBuffer::difference_type;
        using pointer = StaticRingBuffer::pointer;
        using const_pointer = StaticRingBuffer::const_pointer;
        using reference = StaticRingBuffer::reference;
        using const_reference = StaticRingBuffer::const_reference;

        // Allow ConstIterator to access private members of Iterator
        friend class ConstIterator;

        // Constructor
        constexpr explicit Iterator(StaticRingBuffer* buffer, size_type index) 
            : buffer_(buffer), index_(index) {}

        // Dereference operators
        constexpr reference operator*() { return (*buffer_)[index_]; }
        constexpr pointer operator->() { return &(*buffer_)[index_]; }

        // Iterator operations
        constexpr Iterator& operator++() { ++index_; return *this; }
        constexpr Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }

        constexpr Iterator& operator--() { --index_; return *this; }
        constexpr Iterator operator--(int) { Iterator temp = *this; --(*this); return temp; }

        // Arithmetic operators for random access iterator
        constexpr Iterator operator+(difference_type offset) const { 
            return Iterator(buffer_, static_cast<size_type>(static_cast<difference_type>(index_) + offset)); 
        }
        constexpr Iterator& operator+=(difference_type n) { 
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) + n); 
            return *this; 
        }
        constexpr Iterator operator-(difference_type offset) const { 
            return Iterator(buffer_, static_cast<size_type>(static_cast<difference_type>(index_) - offset)); 
        }
        constexpr Iterator& operator-=(difference_type n) { 
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) - n); 
            return *this; 
        }

        // Friend function for addition with difference_type on the left
        friend constexpr Iterator operator+(difference_type n, const Iterator& it) {
            return Iterator(it.buffer_, static_cast<size_type>(static_cast<difference_type>(it.index_) + n));
        }

        // Difference operator for random access iterator
        constexpr difference_type operator-(const Iterator& other) const {
            return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_);
        }

        // Subscript operators for random access iterator
        constexpr reference operator[](difference_type offset) { 
            return (*buffer_)[static_cast<size_type>(static_cast<difference_type>(index_) + offset)]; 
        }
        constexpr const_reference operator[](difference_type offset) const { 
            return (*buffer_)[static_cast<size_type>(static_cast<difference_type>(index_) + offset)]; 
        }

        // Equality operators for iterators
        constexpr bool operator==(const Iterator& other) const { 
            return buffer_ == other.buffer_ && index_ == other.index_; 
        }
        constexpr bool operator!=(const Iterator& other) const { return !(*this == other); }

        // Comparison operators for random access iterator
        constexpr bool operator<(const Iterator& other) const { 
            return buffer_ < other.buffer_ || (buffer_ == other.buffer_ && index_ < other.index_); 
        }
        constexpr bool operator>(const Iterator& other) const { 
            return buffer_ > other.buffer_ || (buffer_ == other.buffer_ && index_ > other.index_); 
        }
        constexpr bool operator<=(const Iterator& other) const { 
            return buffer_ < other.buffer_ || (buffer_ == other.buffer_ && index_ <= other.index_); 
        }
        constexpr bool operator>=(const Iterator& other) const { 
            return buffer_ > other.buffer_ || (buffer_ == other.buffer_ && index_ >= other.index_); 
        }

    private:
        StaticRingBuffer* buffer_;
        size_type index_;
    };

    // Get an iterator to the beginning of the buffer
    constexpr Iterator begin() noexcept { return Iterator(this, 0); }

    // Get an iterator to the end of the buffer
    constexpr Iterator end() noexcept { return Iterator(this, count_); }

    /**
     * @class StaticRingBuffer::ConstIterator
     * @brief A constant iterator for traversing the elements of the StaticRingBuffer in a read-only manner.
     * 
     * @details
     * The ConstIterator class provides a way to iterate through the elements of the StaticRingBuffer from the front (oldest)
     * to the back (newest) without allowing modification of the elements. It supports standard iterator operations
     * such as increment, decrement, dereference, and comparison. The const iterator is designed to work seamlessly with 
     * range-based for loops and STL algorithms that require read-only access.
     * 
     * @note 
     * The const iterator does not support concurrent modifications to the StaticRingBuffer while iterating, and behavior 
     * is undefined if the buffer is modified (such as push/pop or clear) during iteration.
     * 
     * @see StaticRingBuffer::Iterator for a read-write version of the iterator.
     */
    class ConstIterator {
    public:
        // Iterator traits
        using iterator_category = NEX_STD random_access_iterator_tag;
        using value_type = StaticRingBuffer::value_type;
        using size_type = StaticRingBuffer::size_type;
        using difference_type = StaticRingBuffer::difference_type;
        using pointer = StaticRingBuffer::const_pointer;
        using const_pointer = StaticRingBuffer::const_pointer;
        using reference = StaticRingBuffer::const_reference;
        using const_reference = StaticRingBuffer::const_reference;

        // Constructor
        constexpr explicit ConstIterator(const StaticRingBuffer* buffer, size_type index) 
            : buffer_(buffer), index_(index) {}
        constexpr ConstIterator(const Iterator& other) 
            : buffer_(other.buffer_), index_(other.index_) {}

        // Dereference operators
        constexpr reference operator*() const { return (*buffer_)[index_]; }
        constexpr pointer operator->() const { return &(*buffer_)[index_]; }

        // Iterator operations
        constexpr ConstIterator& operator++() { ++index_; return *this; }
        constexpr ConstIterator operator++(int) { ConstIterator temp = *this; ++(*this); return temp; }
        constexpr ConstIterator& operator--() { --index_; return *this; }
        constexpr ConstIterator operator--(int) { ConstIterator temp = *this; --(*this); return temp; }

        // Arithmetic operators for random access iterator
        constexpr ConstIterator operator+(difference_type offset) const { 
            return ConstIterator(buffer_, static_cast<size_type>(static_cast<difference_type>(index_) + offset)); 
        }
        constexpr ConstIterator& operator+=(difference_type n) { 
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) + n); 
            return *this; 
        }
        constexpr ConstIterator operator-(difference_type offset) const { 
            return ConstIterator(buffer_, static_cast<size_type>(static_cast<difference_type>(index_) - offset)); 
        }
        constexpr ConstIterator& operator-=(difference_type n) { 
            index_ = static_cast<size_type>(static_cast<difference_type>(index_) - n); 
            return *this; 
        }

        // Friend function for addition with difference_type on the left
        friend constexpr ConstIterator operator+(difference_type n, const ConstIterator& it) {
            return ConstIterator(it.buffer_, static_cast<size_type>(static_cast<difference_type>(it.index_) + n));
        }

        // Difference operator for random access iterator
        constexpr difference_type operator-(const ConstIterator& other) const {
            return static_cast<difference_type>(index_) - static_cast<difference_type>(other.index_);
        }

        // Subscript operators for random access iterator
        constexpr reference operator[](difference_type offset) const { 
            return (*buffer_)[static_cast<size_type>(static_cast<difference_type>(index_) + offset)]; 
        }

        // Equality operators for const iterators
        constexpr bool operator==(const ConstIterator& other) const { 
            return buffer_ == other.buffer_ && index_ == other.index_; 
        }
        constexpr bool operator!=(const ConstIterator& other) const { return !(*this == other); }

        // Comparison operators for random access iterator
        constexpr bool operator<(const ConstIterator& other) const { 
            return buffer_ < other.buffer_ || (buffer_ == other.buffer_ && index_ < other.index_); 
        }
        constexpr bool operator>(const ConstIterator& other) const { 
            return buffer_ > other.buffer_ || (buffer_ == other.buffer_ && index_ > other.index_); 
        }
        constexpr bool operator<=(const ConstIterator& other) const { 
            return buffer_ < other.buffer_ || (buffer_ == other.buffer_ && index_ <= other.index_); 
        }
        constexpr bool operator>=(const ConstIterator& other) const { 
            return buffer_ > other.buffer_ || (buffer_ == other.buffer_ && index_ >= other.index_); 
        }

    private:
        const StaticRingBuffer* buffer_;
        size_type index_;
    };

    // Get a constant iterator to the beginning of the buffer
    constexpr ConstIterator begin() const noexcept { return ConstIterator(this, 0); }

    // Get a constant iterator to the end of the buffer
    constexpr ConstIterator end() const noexcept { return ConstIterator(this, count_); }

    // Get a constant iterator to the beginning of the buffer
    constexpr ConstIterator cbegin() const noexcept { return ConstIterator(this, 0); }

    // Get a constant iterator to the end of the buffer
    constexpr ConstIterator cend() const noexcept { return ConstIterator(this, count_); }

    // Get a reverse iterator to the beginning of the reversed buffer (i.e., end of the normal buffer)
    constexpr ReverseIterator rbegin() noexcept { return ReverseIterator(end()); }

    // Get a reverse iterator to the end of the reversed buffer (i.e., beginning of the normal buffer)
    constexpr ReverseIterator rend() noexcept { return ReverseIterator(begin()); }

    // Get a const reverse iterator to the beginning of the reversed buffer (i.e., end of the normal buffer)
    constexpr ConstReverseIterator rbegin() const noexcept { return ConstReverseIterator(end()); }

    // Get a const reverse iterator to the end of the reversed buffer (i.e., beginning of the normal buffer)
    constexpr ConstReverseIterator rend() const noexcept { return ConstReverseIterator(begin()); }

    // Get a const reverse iterator to the beginning of the reversed buffer (i.e., end of the normal buffer)
    constexpr ConstReverseIterator crbegin() const noexcept { return ConstReverseIterator(cend()); }

    // Get a const reverse iterator to the end of the reversed buffer (i.e., beginning of the normal buffer)
    constexpr ConstReverseIterator crend() const noexcept { return ConstReverseIterator(cbegin()); }

    ////// Modifiers ------------------------------------------------

    // Add an element to the back of the buffer
    constexpr bool pushBack(const value_type& value) {
        if (full()) {
            if constexpr (Policy == OverflowPolicy::Reject) {
                return false;                           // Reject new element
            } else {
                buffer_[tail_] = value;                 // Overwrite oldest element
                tail_ = (tail_ + 1) % Capacity;         // Move tail forward
                head_ = tail_;                          // Oldest element follows inserted element
                return true;
            }
        }
        buffer_[tail_] = value;                         // Add new element
        tail_ = (tail_ + 1) % Capacity;                 // Move tail forward
        ++count_;
        return true;
    }

    // Add an element to the back of the buffer (move version)
    constexpr bool pushBack(value_type&& value) {
        if (full()) {
            if constexpr (Policy == OverflowPolicy::Reject) {
                return false;                           // Reject new element
            } else {
                buffer_[tail_] = NEX_STD move(value);   // Overwrite oldest element
                tail_ = (tail_ + 1) % Capacity;         // Move tail forward
                head_ = tail_;                          // Oldest element follows inserted element
                return true;
            }
        }
        buffer_[tail_] = NEX_STD move(value);           // Add new element using move semantics
        tail_ = (tail_ + 1) % Capacity;                 // Move tail forward
        ++count_;
        return true;
    }

    // Remove the front element from the buffer
    constexpr void popFront() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");    // debug runtime-check only
        head_ = (head_ + 1) % Capacity;                 // Move head forward
        --count_;
    }

    // Remove the back element from the buffer
    constexpr void popBack() {
        NEX_ASSERT_MSG(!empty(), "Buffer is empty");    // debug runtime-check only
        tail_ = (tail_ + Capacity - 1) % Capacity;      // Move tail backward
        --count_;
    }
};

NEX_NAMESPACE_END
