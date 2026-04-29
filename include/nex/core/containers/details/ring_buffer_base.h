/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <type_traits>
#include <iterator>
#include <utility>
#include <cstring>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/wrappers.h"
#include "nex/base/assert_crash.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @namespace ring_buffer::details
 * @brief Internal namespace for ring buffer implementations and related utilities.
 */
namespace ring_buffer::details {

    /**
     * @enum OverflowPolicy
     * @brief Defines policies for handling overflow situations in data structures like RingBuffer.
     */
    enum class OverflowPolicy { 
        Overwrite,      ///< When the buffer is full, new elements will overwrite the oldest ones.
        Reject          ///< When the buffer is full, new elements will be rejected and not added to the buffer.
    };

    /**
     * @struct DynamicStorage
     * @brief A template structure to manage dynamic storage for the RingBuffer.
     * @tparam T The type of elements stored in the buffer.
     */
    template <typename T>
    struct NEX_INTERNAL DynamicStorage {
        using value_type = T;

        UniquePtr<value_type[]> buffer;                 // Dynamic array to hold the elements
        usize capacity = 0;                             // Total capacity of the buffer
        static constexpr bool is_dynamic = true;        // Flag to indicate dynamic storage

        // Constructor to initialize dynamic storage with a specified capacity
        DynamicStorage(usize cap) 
            : buffer(NEX_STD make_unique<value_type[]>(cap > 0 ? cap : 1)), capacity(cap > 0 ? cap : 1) {}
    };

    /**
     * @struct StaticStorage
     * @brief A template structure to manage static storage for the StaticRingBuffer.
     * @tparam T The type of elements stored in the buffer.
     * @tparam Capacity The maximum number of elements the buffer can hold (must be greater than 0).
     */
    template <typename T, usize Capacity>
    requires (Capacity > 0)
    struct NEX_INTERNAL StaticStorage {
        // Compile-time check for valid capacity
        static_assert(Capacity > 0, "Capacity must be greater than 0");

        using value_type = T;

        value_type buffer[Capacity] = {};               // Static array to hold the elements (default-initialized)
        static constexpr usize capacity = Capacity;     // Total capacity of the buffer
        static constexpr bool is_dynamic = false;       // Flag to indicate static storage

        // Default constructor
        StaticStorage() = default;
    };

    /**
     * @class RingBufferBase
     * @brief A base class template for implementing ring buffers with different storage types and overflow policies.
     * 
     * @tparam T The type of elements stored in the buffer.
     * @tparam Storage The storage type (DynamicStorage or StaticStorage) that manages the underlying buffer.
     * @tparam Policy The policy for handling overflow situations.
     */
    template <typename T, typename Storage, OverflowPolicy Policy>
    class NEX_INTERNAL RingBufferBase : private Storage {
    public:
        // Forward declaration of iterators
        class Iterator;
        class ConstIterator;
        using ReverseIterator = NEX_STD reverse_iterator<Iterator>;
        using ConstReverseIterator = NEX_STD reverse_iterator<ConstIterator>;

        // Type aliases for convenience
        using value_type = typename Storage::value_type;
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

    protected:
        size_type head_ = 0;        // Index of the next element to be read
        size_type tail_ = 0;        // Index of the next element to be written
        size_type count_ = 0;       // Current number of elements in the buffer

    public:
        ////// Construction -----------------------------------------------

        // Constructor for dynamic storage
        explicit RingBufferBase(size_type capacity) requires (Storage::is_dynamic) : Storage(capacity) {}

        // Constructor for static storage
        RingBufferBase() requires (!Storage::is_dynamic) = default;

        ////// Capacity and size management -----------------------------------------------

        // Get the current number of elements in the buffer
        constexpr size_type size() const noexcept { return count_; }

        // Get the maximum capacity of the buffer
        constexpr size_type capacity() const noexcept { return Storage::capacity; }

        // Check if the buffer is empty
        constexpr bool empty() const noexcept { return count_ == 0; }

        // Check if the buffer is full
        constexpr bool full() const noexcept { return count_ == Storage::capacity; }

        // Clear the buffer
        void clear() noexcept {
            // to avoid potential resource leaks for non-trivial types
            if constexpr (!NEX_STD is_trivially_destructible_v<value_type>) {
                for (size_type i = 0; i < count_; ++i) {
                    Storage::buffer[(head_ + i) % Storage::capacity] = value_type(); // reset to default value
                }
            }
            head_ = 0; tail_ = 0; count_ = 0;
        }

        // Resize the buffer to a new capacity
        void resize(size_type newCapacity) requires (Storage::is_dynamic) {
            if (newCapacity == 0) return;
            if (newCapacity == Storage::capacity) return; // No change needed

            // Create a new buffer with the new capacity
            UniquePtr<value_type[]> new_buffer = NEX_STD make_unique<value_type[]>(newCapacity);
            size_type new_count = count_ < newCapacity ? count_ : newCapacity;
            size_type old_capacity = Storage::capacity;

            // Copy existing elements to the new buffer (for trivially copyable types)
            if constexpr (NEX_STD is_trivially_copyable_v<value_type>) {
                size_type first_count = new_count;
                if (head_ + first_count > old_capacity) {
                    first_count = old_capacity - head_;
                }

                // Copy the first contiguous block
                if (first_count > 0) {
                    NEX_STD memcpy(
                        new_buffer.get(), 
                        Storage::buffer.get() + head_, 
                        first_count * sizeof(value_type)
                    );
                }

                // Copy the second block if needed
                size_type second_count = new_count - first_count;
                if (second_count > 0) {
                    NEX_STD memcpy(
                        new_buffer.get() + first_count, 
                        Storage::buffer.get(), 
                        second_count * sizeof(value_type)
                    );
                }
            } else {
                // For non-trivially copyable types, we need to move or copy elements individually
                for (size_type i = 0; i < new_count; ++i) {
                    if constexpr (NEX_STD is_nothrow_move_assignable_v<value_type> 
                                    || !NEX_STD is_copy_assignable_v<value_type>) {
                        // Move elements if it's safe to do so
                        new_buffer[i] = NEX_STD move(Storage::buffer[(head_ + i) % old_capacity]);
                    } else {
                        // Otherwise, copy elements
                        new_buffer[i] = Storage::buffer[(head_ + i) % old_capacity];
                    }
                }
            }

            // Replace old buffer with new buffer
            Storage::buffer = NEX_STD move(new_buffer);

            // Update capacity and reset indices
            Storage::capacity = newCapacity;
            head_ = 0;
            tail_ = new_count % Storage::capacity;
            count_ = new_count;
        }

        // Shrink the buffer to fit the current number of elements (releases unused memory)
        void shrinkToFit() requires (Storage::is_dynamic) {
            if (count_ == Storage::capacity) return; // Already at optimal size
            resize(count_ > 0 ? count_ : 1);
        }

        ////// Element access ------------------------------------------------

        // Access the front element (oldest)
        reference front() {
            NEX_ASSERT_MSG(!empty(), "Buffer is empty");            // debug runtime-check only
            return Storage::buffer[head_];
        }

        // Access the front element (oldest; constant reference)
        const_reference front() const {
            NEX_ASSERT_MSG(!empty(), "Buffer is empty");            // debug runtime-check only
            return Storage::buffer[head_];
        }

        // Access the back element (newest)
        reference back() {
            NEX_ASSERT_MSG(!empty(), "Buffer is empty");            // debug runtime-check only
            return Storage::buffer[(tail_ + Storage::capacity - 1) % Storage::capacity];
        }

        // Access the back element (newest; constant reference)
        const_reference back() const {
            NEX_ASSERT_MSG(!empty(), "Buffer is empty");            // debug runtime-check only
            return Storage::buffer[(tail_ + Storage::capacity - 1) % Storage::capacity];
        }

        // Access element at specific index (0 is front, size-1 is back)
        reference at(size_type index) {
            NEX_ASSERT_MSG(index < count_, "Index out of bounds");  // debug runtime-check only
            return Storage::buffer[(head_ + index) % Storage::capacity];
        }

        // Access element at specific index (0 is front, size-1 is back; constant reference)
        const_reference at(size_type index) const {
            NEX_ASSERT_MSG(index < count_, "Index out of bounds");  // debug runtime-check only
            return Storage::buffer[(head_ + index) % Storage::capacity];
        }

        // Access element at specific index without bounds checking
        reference operator[](size_type index) {
            return Storage::buffer[(head_ + index) % Storage::capacity];
        }

        // Access element at specific index without bounds checking (constant reference)
        const_reference operator[](size_type index) const {
            return Storage::buffer[(head_ + index) % Storage::capacity];
        }

        ////// Iterators ------------------------------------------------

        /**
         * @class RingBufferBase::Iterator
         * @brief An iterator for traversing the elements of the ring buffer in a linear fashion.
         * 
         * @details
         * The Iterator class provides a way to iterate through the elements of the ring buffer from the front (oldest) 
         * to the back (newest). It supports standard iterator operations such as increment, decrement, dereference, 
         * and comparison. The iterator is designed to work seamlessly with range-based for loops and STL algorithms.
         * 
         * @note 
         * The iterator does not support concurrent modifications to the ring buffer while iterating, and behavior 
         * is undefined if the buffer is modified (such as push/pop or clear) during iteration.
         * 
         * @see RingBufferBase::ConstIterator for a read-only version of the iterator.
         */
        class Iterator {
        public:
            // Iterator traits
            using iterator_category = NEX_STD random_access_iterator_tag;
            using value_type = RingBufferBase::value_type;
            using size_type = RingBufferBase::size_type;
            using difference_type = RingBufferBase::difference_type;
            using pointer = RingBufferBase::pointer;
            using const_pointer = RingBufferBase::const_pointer;
            using reference = RingBufferBase::reference;
            using const_reference = RingBufferBase::const_reference;

            // Allow ConstIterator to access private members of Iterator
            friend class ConstIterator;

            // Constructor
            constexpr explicit Iterator(RingBufferBase* buffer, size_type index, size_type countFromStart = 0) 
                : buffer_(buffer), index_(index), countFromStart_(countFromStart) {}

            // Dereference operators
            constexpr reference operator*() { return buffer_->Storage::buffer[index_]; }
            constexpr pointer operator->() { return &buffer_->Storage::buffer[index_]; }

            // Iterator operations
            constexpr Iterator& operator++() { 
                index_ = (index_ + 1) % buffer_->capacity();
                ++countFromStart_;
                return *this; 
            }
            constexpr Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }

            constexpr Iterator& operator--() { 
                index_ = (index_ + buffer_->capacity() - 1) % buffer_->capacity();
                --countFromStart_;
                return *this; 
            }
            constexpr Iterator operator--(int) { Iterator temp = *this; --(*this); return temp; }

            // Arithmetic operators for random access iterator
            constexpr Iterator operator+(difference_type offset) const { 
                Iterator temp = *this;
                temp += offset;
                return temp;
            }
            constexpr Iterator& operator+=(difference_type n) { 
                index_ = offsetIndex(n);
                countFromStart_ = static_cast<size_type>(static_cast<difference_type>(countFromStart_) + n);
                return *this; 
            }

            constexpr Iterator operator-(difference_type offset) const { 
                Iterator temp = *this;
                temp -= offset;
                return temp;
            }
            constexpr Iterator& operator-=(difference_type n) { 
                index_ = offsetIndex(-n);
                countFromStart_ = static_cast<size_type>(static_cast<difference_type>(countFromStart_) - n);
                return *this; 
            }

            // Friend function for addition with difference_type on the left
            friend constexpr Iterator operator+(difference_type n, const Iterator& it) { 
                return it + n; 
            }

            // Difference operator for random access iterator
            constexpr difference_type operator-(const Iterator& other) const { 
                return static_cast<difference_type>(countFromStart_) 
                        - static_cast<difference_type>(other.countFromStart_); 
            }

            // Subscript operators for random access iterator
            reference operator[](difference_type offset) { 
                return buffer_->Storage::buffer[offsetIndex(offset)]; 
            }
            const_reference operator[](difference_type offset) const { 
                return buffer_->Storage::buffer[offsetIndex(offset)]; 
            }

            // Equality operators for iterators
            constexpr bool operator==(const Iterator& other) const { 
                return buffer_ == other.buffer_ && countFromStart_ == other.countFromStart_; 
            }
            constexpr bool operator!=(const Iterator& other) const { return !(*this == other); }
            
            // Comparison operators for random access iterator
            constexpr bool operator<(const Iterator& other) const { 
                return buffer_ < other.buffer_ 
                        || (buffer_ == other.buffer_ && countFromStart_ < other.countFromStart_); 
            }
            constexpr bool operator>(const Iterator& other) const { 
                return buffer_ > other.buffer_ 
                        || (buffer_ == other.buffer_ && countFromStart_ > other.countFromStart_); 
            }
            constexpr bool operator<=(const Iterator& other) const { 
                return buffer_ < other.buffer_ 
                        || (buffer_ == other.buffer_ && countFromStart_ <= other.countFromStart_); 
            }
            constexpr bool operator>=(const Iterator& other) const { 
                return buffer_ > other.buffer_ 
                        || (buffer_ == other.buffer_ && countFromStart_ >= other.countFromStart_); 
            }

        private:
            RingBufferBase* buffer_;        // Pointer to the ring buffer being iterated
            size_type index_;               // Current index in the underlying buffer (physical ring-buffer slot)
            size_type countFromStart_;      // Number of elements from the start of the buffer (logical position)

            // Calculate the wrapped index based on an offset
            constexpr size_type offsetIndex(difference_type offset) const {
                const difference_type cap = static_cast<difference_type>(buffer_->capacity());
                difference_type wrapped = (static_cast<difference_type>(index_) + offset) % cap;
                if (wrapped < 0) wrapped += cap;
                return static_cast<size_type>(wrapped);
            }
        };

        // Get an iterator to the beginning of the buffer
        constexpr Iterator begin() noexcept { return Iterator(this, head_, 0); }

        // Get an iterator to the end of the buffer
        constexpr Iterator end() noexcept { return Iterator(this, tail_, count_); }

        /**
         * @class RingBufferBase::ConstIterator
         * @brief A constant iterator for traversing the elements of the ring buffer in a read-only manner.
         * 
         * @details
         * The ConstIterator class provides a way to iterate through the elements of the ring buffer from the front (oldest) 
         * to the back (newest) without allowing modification of the elements. It supports standard iterator operations 
         * such as increment, decrement, dereference, and comparison. The const iterator is designed to work seamlessly 
         * with range-based for loops and STL algorithms that require read-only access.
         * 
         * @note 
         * The const iterator does not support concurrent modifications to the ring buffer while iterating, and behavior 
         * is undefined if the buffer is modified (such as push/pop or clear) during iteration.
         * 
         * @see RingBufferBase::Iterator for a read-write version of the iterator.
         */
        class ConstIterator {
        public:
            // Iterator traits
            using iterator_category = NEX_STD random_access_iterator_tag;
            using value_type = RingBufferBase::value_type;
            using size_type = RingBufferBase::size_type;
            using difference_type = RingBufferBase::difference_type;
            using pointer = RingBufferBase::const_pointer;
            using const_pointer = RingBufferBase::const_pointer;
            using reference = RingBufferBase::const_reference;
            using const_reference = RingBufferBase::const_reference;

            // Constructor
            constexpr explicit ConstIterator(const RingBufferBase* buffer, size_type index, size_type countFromStart = 0) 
                : buffer_(buffer), index_(index), countFromStart_(countFromStart) {}

            // Construct a const iterator from a mutable iterator
            constexpr ConstIterator(const Iterator& other)
                : buffer_(other.buffer_), index_(other.index_), countFromStart_(other.countFromStart_) {}

            // Dereference operators
            constexpr reference operator*() const { return buffer_->Storage::buffer[index_]; }
            constexpr pointer operator->() const { return &buffer_->Storage::buffer[index_]; }

            // Iterator operations
            constexpr ConstIterator& operator++() { 
                index_ = (index_ + 1) % buffer_->capacity();
                ++countFromStart_;
                return *this; 
            }
            constexpr ConstIterator operator++(int) { ConstIterator temp = *this; ++(*this); return temp; }

            constexpr ConstIterator& operator--() { 
                index_ = (index_ + buffer_->capacity() - 1) % buffer_->capacity();
                --countFromStart_;
                return *this; 
            }
            constexpr ConstIterator operator--(int) { ConstIterator temp = *this; --(*this); return temp; }

            // Arithmetic operators for random access iterator
            constexpr ConstIterator operator+(difference_type offset) const { 
                ConstIterator temp = *this;
                temp += offset;
                return temp;
            }
            constexpr ConstIterator& operator+=(difference_type n) { 
                index_ = offsetIndex(n);
                countFromStart_ = static_cast<size_type>(static_cast<difference_type>(countFromStart_) + n);
                return *this; 
            }

            constexpr ConstIterator operator-(difference_type offset) const { 
                ConstIterator temp = *this;
                temp -= offset;
                return temp;
            }
            constexpr ConstIterator& operator-=(difference_type n) { 
                index_ = offsetIndex(-n);
                countFromStart_ = static_cast<size_type>(static_cast<difference_type>(countFromStart_) - n);
                return *this; 
            }

            // Friend function for addition with difference_type on the left
            friend constexpr ConstIterator operator+(difference_type n, const ConstIterator& it) { 
                return it + n; 
            }

            // Difference operator for random access iterator
            constexpr difference_type operator-(const ConstIterator& other) const {
                return static_cast<difference_type>(countFromStart_) 
                        - static_cast<difference_type>(other.countFromStart_);
            }

            // Subscript operators for random access iterator
            reference operator[](difference_type offset) const { 
                return buffer_->Storage::buffer[offsetIndex(offset)]; 
            }

            // Equality operators for const iterators
            constexpr bool operator==(const ConstIterator& other) const { 
                return buffer_ == other.buffer_ && countFromStart_ == other.countFromStart_; 
            }
            constexpr bool operator!=(const ConstIterator& other) const { return !(*this == other); }

            // Comparison operators for random access iterator
            constexpr bool operator<(const ConstIterator& other) const { 
                return buffer_ < other.buffer_ 
                        || (buffer_ == other.buffer_ && countFromStart_ < other.countFromStart_); 
            }
            constexpr bool operator>(const ConstIterator& other) const { 
                return buffer_ > other.buffer_ 
                        || (buffer_ == other.buffer_ && countFromStart_ > other.countFromStart_); 
            }
            constexpr bool operator<=(const ConstIterator& other) const { 
                return buffer_ < other.buffer_ 
                        || (buffer_ == other.buffer_ && countFromStart_ <= other.countFromStart_); 
            }
            constexpr bool operator>=(const ConstIterator& other) const { 
                return buffer_ > other.buffer_ 
                        || (buffer_ == other.buffer_ && countFromStart_ >= other.countFromStart_); 
            }

        private:
            const RingBufferBase* buffer_;  // Pointer to the ring buffer being iterated
            size_type index_;               // Current index in the underlying buffer (physical ring-buffer slot)
            size_type countFromStart_;      // Number of elements from the start of the buffer (logical position)
            
            // Calculate the wrapped index based on an offset
            constexpr size_type offsetIndex(difference_type offset) const {
                const difference_type cap = static_cast<difference_type>(buffer_->capacity());
                difference_type wrapped = (static_cast<difference_type>(index_) + offset) % cap;
                if (wrapped < 0) wrapped += cap;
                return static_cast<size_type>(wrapped);
            }
        };

        // Get a constant iterator to the beginning of the buffer
        constexpr ConstIterator begin() const noexcept { return ConstIterator(this, head_, 0); }

        // Get a constant iterator to the end of the buffer
        constexpr ConstIterator end() const noexcept { return ConstIterator(this, tail_, count_); }

        // Get a constant iterator to the beginning of the buffer
        constexpr ConstIterator cbegin() const noexcept { return ConstIterator(this, head_, 0); }

        // Get a constant iterator to the end of the buffer
        constexpr ConstIterator cend() const noexcept { return ConstIterator(this, tail_, count_); }

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
                    return false;                                           // Reject new element
                } else {
                    Storage::buffer[tail_] = value;                         // Overwrite oldest element
                    tail_ = (tail_ + 1) % Storage::capacity;                // Move tail forward
                    head_ = tail_;                                          // Oldest element follows inserted element
                    return true;
                }
            }
            Storage::buffer[tail_] = value;                                 // Add new element
            tail_ = (tail_ + 1) % Storage::capacity;                        // Move tail forward
            ++count_;
            return true;
        }

        // Add an element to the back of the buffer (move version)
        bool pushBack(value_type&& value) {
            if (full()) {
                if constexpr (Policy == OverflowPolicy::Reject) {
                    return false;                                           // Reject new element
                } else {
                    Storage::buffer[tail_] = NEX_STD move(value);           // Overwrite oldest element
                    tail_ = (tail_ + 1) % Storage::capacity;                // Move tail forward
                    head_ = tail_;                                          // Oldest element follows inserted element
                    return true;
                }
            }
            Storage::buffer[tail_] = NEX_STD move(value);                   // Add new element using move semantics
            tail_ = (tail_ + 1) % Storage::capacity;                        // Move tail forward
            ++count_;
            return true;
        }

        // Remove the front element from the buffer
        void popFront() {
            NEX_ASSERT_MSG(!empty(), "Buffer is empty");                    // debug runtime-check only
            head_ = (head_ + 1) % Storage::capacity;                        // Move head forward
            --count_;
        }

        // Remove the back element from the buffer
        void popBack() {
            NEX_ASSERT_MSG(!empty(), "Buffer is empty");                    // debug runtime-check only
            tail_ = (tail_ + Storage::capacity - 1) % Storage::capacity;    // Move tail backward
            --count_;
        }
    };

} // namespace ring_buffer

NEX_CORE_NAMESPACE_END
