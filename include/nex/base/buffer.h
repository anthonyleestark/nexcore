/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/limits.h"
#include "nex/base/block.h"
#include "nex/base/growth.h"
#include "nex/base/memory.h"
#include "nex/base/algorithm.h"

NEX_NAMESPACE_BEGIN

/**
 * @class FixedBuffer
 * @brief A fixed-size buffer that can hold a specified number of elements of a given type.
 * 
 * @details
 * The FixedBuffer class provides a fixed-size buffer that can hold a specified number of elements of a given type.
 * It allows for efficient storage and manipulation of elements without dynamic memory allocation. The buffer is
 * implemented using raw aligned storage, and elements are constructed and destroyed in place as needed.
 * 
 * @tparam Type The type of elements to be stored in the buffer.
 * @tparam Capacity The maximum number of elements the buffer can hold. Must be greater than 0.
 */
template <
    typename Type,
    usize Capacity
> requires (Capacity > 0)
class NEX_HIDDEN_FROM_ABI FixedBuffer {
public:
    using value_type = Type;
    using size_type = usize;
    using pointer_type = value_type*;
    using const_pointer_type = const value_type*;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;

private:
    // Raw aligned storage that can hold Capacity elements of type Type without constructing them.
    NEX_ALIGNAS(value_type) byte buffer_[sizeof(value_type) * Capacity];

    // Current number of elements in the buffer
    size_type size_ = 0;

public:
    // Gets access to the underlying buffer (read/write).
    NEX_HIDDEN_FROM_ABI constexpr pointer_type data() noexcept {
        return NEX_MEMORY_CAST<value_type>(buffer_);
    }

    // Gets access to the underlying buffer (read-only).
    NEX_HIDDEN_FROM_ABI constexpr const_pointer_type data() const noexcept {
        return NEX_MEMORY_CAST<const value_type>(buffer_);
    }

    // Returns the current number of elements in the buffer
    NEX_HIDDEN_FROM_ABI constexpr size_type size() const noexcept { return size_; }

    // Determines whether the buffer is empty (contains no elements)
    NEX_HIDDEN_FROM_ABI constexpr bool empty() const noexcept { return size_ == 0; }

    // Returns the capacity of the buffer (maximum number of elements it can hold)
    NEX_HIDDEN_FROM_ABI constexpr size_type capacity() const noexcept { return Capacity; }

    // Returns the maximum number of elements the buffer can hold (same as capacity)
    NEX_HIDDEN_FROM_ABI static constexpr size_type maxSize() noexcept { return Capacity; }

private:
    // Validate whether a count of elements can fit within the buffer's capacity
    NEX_HIDDEN_FROM_ABI constexpr bool canFit(size_type count) const { return count <= Capacity; }

    // Copies the entire buffer from another FixedBuffer
    // Used in the copy constructor, when the current buffer is uninitialized
    template <typename OtherValue, usize OtherCapacity>
        requires (meta::IsConvertibleV<OtherValue, value_type> && OtherCapacity <= Capacity)
    NEX_HIDDEN_FROM_ABI constexpr void copyFrom(const FixedBuffer<OtherValue, OtherCapacity>& other) {
        if (canFit(other.size_)) {
            NEX_COPY_RANGE(data(), other.data(), other.data() + other.size_);
            size_ = other.size_;
        }
    }

    // Copies the entire buffer from another FixedBuffer
    // Used in the copy assignment operator, when the current buffer is already initialized
    template <typename OtherValue, usize OtherCapacity>
        requires (meta::IsConvertibleV<OtherValue, value_type> && OtherCapacity <= Capacity)
    NEX_HIDDEN_FROM_ABI constexpr void copyAssignFrom(const FixedBuffer<OtherValue, OtherCapacity>& other) {
        if (canFit(other.size_)) {
            if (other.size_ <= size_) {
                // Copy common elements and destroy remaining part
                NEX_ASSIGN_RANGE(data(), other.data(), other.data() + other.size_);
                NEX_DESTROY_RANGE(data() + other.size_, data() + size_);
            } else {
                // Copy common elements and construct new elements
                pointer_type commonEnd = other.data() + size_;
                NEX_ASSIGN_RANGE(data(), other.data(), commonEnd);
                NEX_COPY_RANGE(data() + size_, commonEnd, other.data() + other.size_);
            }
            size_ = other.size_;
        }
    }

    // Moves the entire buffer from another FixedBuffer (used in move constructor and move assignment)
    template <typename OtherValue, usize OtherCapacity>
        requires (meta::IsConvertibleV<OtherValue, value_type> && OtherCapacity <= Capacity)
    NEX_HIDDEN_FROM_ABI constexpr void moveFrom(FixedBuffer<OtherValue, OtherCapacity>&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (canFit(other.size_)) {
            NEX_RELOCATE_RANGE(data(), other.data(), other.data() + other.size_);
            size_ = other.size_;
            other.size_ = 0; // Reset the size of the moved-from buffer
        }
    }

public:
    // Default constructor: initializes an empty buffer
    NEX_HIDDEN_FROM_ABI constexpr FixedBuffer() noexcept = default;

    // Constructs a FixedBuffer with a specified number of default-constructed elements
    NEX_HIDDEN_FROM_ABI constexpr FixedBuffer(size_type count)
        noexcept(meta::IsNothrowDefaultConstructibleV<value_type>) {
        resize(count);
    }

    // Constructs a FixedBuffer with a specified number of elements, each initialized to a given value
    template <typename OtherValue>
        requires (meta::IsConvertibleV<OtherValue, value_type>)
    NEX_HIDDEN_FROM_ABI constexpr FixedBuffer(size_type count, const OtherValue& value)
        noexcept(meta::IsNothrowConstructibleV<value_type, const OtherValue&>) {
        resize(count, value);
    }

    // Copy constructor: creates a FixedBuffer by copying another FixedBuffer
    template <typename OtherValue, usize OtherCapacity>
        requires (meta::IsConvertibleV<OtherValue, value_type> && OtherCapacity <= Capacity)
    NEX_HIDDEN_FROM_ABI constexpr FixedBuffer(const FixedBuffer<OtherValue, OtherCapacity>& other)
        noexcept(meta::IsNothrowCopyConstructibleV<value_type>) {
        copyFrom(other);
    }

    // Copy assignment operator: assigns the contents of another FixedBuffer to this FixedBuffer
    template <typename OtherValue, usize OtherCapacity>
        requires (meta::IsConvertibleV<OtherValue, value_type> && OtherCapacity <= Capacity)
    NEX_HIDDEN_FROM_ABI constexpr FixedBuffer& operator=(const FixedBuffer<OtherValue, OtherCapacity>& other)
        noexcept(meta::IsNothrowCopyConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (this != &other) {
            copyAssignFrom(other);
        }
        return *this;
    }

    // Move constructor: creates a FixedBuffer by moving another FixedBuffer
    template <typename OtherValue, usize OtherCapacity>
        requires (meta::IsConvertibleV<OtherValue, value_type> && OtherCapacity <= Capacity)
    NEX_HIDDEN_FROM_ABI constexpr FixedBuffer(FixedBuffer<OtherValue, OtherCapacity>&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type>) {
        moveFrom(NEX_MOVE(other));
    }

    // Move assignment operator: assigns the contents of another FixedBuffer to this FixedBuffer
    template <typename OtherValue, usize OtherCapacity>
        requires (meta::IsConvertibleV<OtherValue, value_type> && OtherCapacity <= Capacity)
    NEX_HIDDEN_FROM_ABI constexpr FixedBuffer& operator=(FixedBuffer<OtherValue, OtherCapacity>&& other) 
        noexcept(meta::IsNothrowMoveConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (this != &other && canFit(other.size_)) {
            reset();
            moveFrom(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor: destroys all elements in the buffer
    NEX_HIDDEN_FROM_ABI constexpr ~FixedBuffer()
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        reset();
    }

    // Resizes the buffer to contain a specified number of elements, default-constructing new elements if necessary
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count)
        noexcept(meta::IsNothrowDefaultConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (canFit(count)) {
            if (count < size_) {
                // Destroy excess elements
                NEX_DESTROY_RANGE(data() + count, data() + size_);
            } else if (count > size_) {
                // Default-construct new elements
                for (size_type i = size_; i < count; ++i) {
                    NEX_CONSTRUCT_AT(data() + i);
                }
            }
            size_ = count;
        }
    }

    // Resizes the buffer to contain a specified number of elements,
    // initializing new elements to a given value if necessary
    template <typename OtherValue>
        requires (meta::IsConvertibleV<OtherValue, value_type>)
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count, const OtherValue& value)
        noexcept(meta::IsNothrowConstructibleV<value_type, const OtherValue&> &&
                 meta::IsNothrowDestructibleV<value_type>) {
        if (canFit(count)) {
            if (count < size_) {
                // Destroy excess elements
                NEX_DESTROY_RANGE(data() + count, data() + size_);
            } else if (count > size_) {
                // Construct new elements with the given value
                for (size_type i = size_; i < count; ++i) {
                    NEX_CONSTRUCT_AT(data() + i, value);
                }
            }
            size_ = count;
        }
    }

    // Clears the buffer, destroying all elements and resetting the size to zero
    NEX_HIDDEN_FROM_ABI constexpr void clear()
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        if (size_ == 0) return;
        NEX_DESTROY_RANGE(data(), data() + size_);
        size_ = 0;
    }

    // Resets the buffer, destroying all elements and resetting the size to zero (same as clear)
    NEX_HIDDEN_FROM_ABI constexpr void reset()
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        if (size_ == 0) return;
        clear();
    }

    // Appends elements from a given range which is specified by a pointer to its first element
    // and a size to the end of the buffer.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void append(const OtherValue* src, size_type count)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        NEX_ASSERT(src != nullptr || count == 0);
        if (src == nullptr || count == 0) return;
        if (canFit(size_ + count)) {
            NEX_COPY_RANGE(data() + size_, src, src + count);
            size_ += count;
        }
    }

    // Appends elements from a given range which is specified by a pointer to its first element
    // and a pointer to its last element (exclusive) to the end of the buffer.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void append(const OtherValue* first, const OtherValue* last)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        NEX_ASSERT(first != nullptr && last != nullptr && first <= last);
        append(first, static_cast<size_type>(last - first));
    }

    // Appends elements from a given range which is specified by a pointer to its first element
    // and a size to the end of the buffer. This overload is for rvalue references.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void appendMove(OtherValue* src, size_type count)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        NEX_ASSERT(src != nullptr || count == 0);
        if (src == nullptr || count == 0) return;
        if (canFit(size_ + count)) {
            for (size_type i = 0; i < count; ++i) {
                NEX_CONSTRUCT_AT(data() + size_ + i, NEX_MOVE(src[i]));
            }
            size_ += count;
        }
    }

    // Appends an element constructed in place at the end of the buffer with perfect forwarding
    // of constructor arguments.
    template <typename... Args>
        requires (meta::IsConstructibleV<value_type, Args...>)
    NEX_HIDDEN_FROM_ABI constexpr void appendConstruct(Args&&... args) {
        if (canFit(size_ + 1)) {
            NEX_CONSTRUCT_AT(data() + size_, NEX_FORWARD<Args>(args)...);
            ++size_;
        }
    }

    // Removes elements from the buffer starting at index and spanning count elements,
    // shifting subsequent elements down.
    NEX_HIDDEN_FROM_ABI constexpr void remove(size_type index, size_type count = 1) {
        NEX_ASSERT(index < size_ && count > 0 && index + count <= size_);
        pointer_type start = data() + index;
        pointer_type end = start + count;
        NEX_DESTROY_RANGE(start, end);
        NEX_RELOCATE_RANGE(start, end, start);
        size_ -= count;
    }

    // Swaps contents with another FixedBuffer (no exceptions thrown).
    NEX_HIDDEN_FROM_ABI constexpr void swap(FixedBuffer& other) noexcept(
        meta::IsNothrowMoveConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (this == &other) return;
        FixedBuffer<value_type, Capacity> tmp;
        tmp.moveFrom(NEX_MOVE(*this));
        moveFrom(NEX_MOVE(other));
        other.moveFrom(NEX_MOVE(tmp));
    }

    // Returns a MemoryBlock representing the buffer's data (read/write).
    NEX_HIDDEN_FROM_ABI constexpr MemoryBlock block() noexcept {
        return MemoryBlock(data(), size_ * sizeof(value_type));
    }

    // Returns a ConstMemoryBlock representing the buffer's data (read-only).
    NEX_HIDDEN_FROM_ABI constexpr ConstMemoryBlock cblock() const noexcept {
        return ConstMemoryBlock(data(), size_ * sizeof(value_type));
    }

    // Gets access to the element at index (read/write, with bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type operator[](size_type pos) noexcept {
        NEX_ASSERT_MSG(pos < size_, "Error: Index out of bounds");
        return data()[pos];
    }

    // Gets access to the element at index (read-only, with bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type operator[](size_type pos) const noexcept {
        NEX_ASSERT_MSG(pos < size_, "Error: Index out of bounds");
        return data()[pos];
    }

    // Gets access to the element at index (read/write, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type front() noexcept {
        return data()[0];
    }

    // Gets access to the element at index (read-only, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type front() const noexcept {
        return data()[0];
    }

    // Gets access to the element at index (read/write, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type back() noexcept {
        return data()[size_ - 1];
    }

    // Gets access to the element at index (read-only, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type back() const noexcept {
        return data()[size_ - 1];
    }
};

/**
 * @class DynamicBuffer
 * @brief A dynamic buffer that can grow as needed.
 * 
 * @details
 * The DynamicBuffer class is a dynamic buffer that can grow as needed. It allows for efficient storage and manipulation
 * of elements with dynamic memory allocation. The buffer grows according to the specified growth policy when more space
 * is required.
 * 
 * @tparam Type The type of elements stored in the buffer.
 * @tparam Growth The growth policy to use when resizing the buffer. Default is DoubleGrowth.
 */
template <
    typename Type,
    GrowthPolicy Growth = DoubleGrowth
>
class NEX_HIDDEN_FROM_ABI DynamicBuffer {
public:
    using value_type = Type;
    using size_type = usize;
    using pointer_type = value_type*;
    using const_pointer_type = const value_type*;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;

private:
    // Pointer to the data buffer
    pointer_type data_ = nullptr;

    // Current number of elements in the buffer
    size_type size_ = 0;

    // Current capacity of the buffer
    size_type capacity_ = 0;

    // Copies the entire buffer from another DynamicBuffer
    // Used in the copy constructor, when the current buffer is uninitialized.
    NEX_HIDDEN_FROM_ABI constexpr void copyFrom(const DynamicBuffer& other) {
        reserve(other.size_);
        NEX_COPY_RANGE(data_, other.data_, other.data_ + other.size_);
        size_ = other.size_;
    }

    // Copies the entire buffer from another DynamicBuffer
    // Used in the copy assignment operator, when the current buffer is already initialized.
    NEX_HIDDEN_FROM_ABI constexpr void copyAssignFrom(const DynamicBuffer& other) {
        if (other.size_ > capacity_) {
            // Not enough capacity, need to allocate new storage
            reset();
            reserve(other.size_);
            NEX_COPY_RANGE(data_, other.data_, other.data_ + other.size_);
        } else if (other.size_ <= size_) {
            // Copy common elements and destroy remaining part
            NEX_ASSIGN_RANGE(data_, other.data_, other.data_ + other.size_);
            NEX_DESTROY_RANGE(data_ + other.size_, data_ + size_);
        } else {
            // Copy common elements and construct new elements
            pointer_type commonEnd = other.data_ + size_;
            NEX_ASSIGN_RANGE(data_, other.data_, commonEnd);
            NEX_COPY_RANGE(data_ + size_, commonEnd, other.data_ + other.size_);
        }
        size_ = other.size_;
    }

    // Moves the entire buffer from another DynamicBuffer (used in move constructor and move assignment)
    NEX_HIDDEN_FROM_ABI constexpr void moveFrom(DynamicBuffer&& other) noexcept {
        // Steals the buffer
        // Transfer ownership of the buffer from other to this - no need to copy elements
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Relocates the buffer to a new memory location, moving all live elements and deallocating the old buffer.
    NEX_HIDDEN_FROM_ABI constexpr void relocateTo(pointer_type newData) noexcept(false) {
        if (size_ > 0) {
            try {
                NEX_RELOCATE_RANGE(newData, data_, data_ + size_);
            } catch (...) {
                deallocate(newData);
                throw;
            }
        }
        deallocate(data_);
        data_ = newData;
    }

    // Allocates a raw, uninitialized heap buffer for exactly cap elements.
    static NEX_HIDDEN_FROM_ABI pointer_type allocate(size_type cap) noexcept(false) {
        return static_cast<pointer_type>(::operator new(cap * sizeof(value_type)));
    }

    // Frees a heap buffer (does NOT destroy elements — caller must do that first)
    static NEX_HIDDEN_FROM_ABI void deallocate(pointer_type ptr)
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        ::operator delete(static_cast<void*>(ptr));
    }

    // Grows capacity to at least requiredCapacity, migrating all live elements.
    NEX_HIDDEN_FROM_ABI void grow(size_type requiredCapacity) {
        NEX_ASSERT(requiredCapacity > capacity_);
        size_type newCapacity = Growth::grow(capacity_, requiredCapacity, maxSize());
        pointer_type newData = allocate(newCapacity);
        relocateTo(newData);
        capacity_ = newCapacity;
    }

public:
    // Default constructor: initializes an empty buffer
    NEX_HIDDEN_FROM_ABI constexpr DynamicBuffer() noexcept = default;

    // Constructs a DynamicBuffer with a specified number of default-constructed elements
    NEX_HIDDEN_FROM_ABI constexpr DynamicBuffer(size_type count) noexcept(false) {
        resize(count);
    }

    // Constructs a DynamicBuffer with a specified number of elements, each initialized to a given value
    template <typename OtherValue>
        requires (meta::IsConvertibleV<OtherValue, value_type>)
    NEX_HIDDEN_FROM_ABI constexpr DynamicBuffer(size_type count, const OtherValue& value) noexcept(false) {
        resize(count, value);
    }

    // Copy constructor: creates a DynamicBuffer by copying another DynamicBuffer
    NEX_HIDDEN_FROM_ABI constexpr DynamicBuffer(const DynamicBuffer& other) noexcept(false) {
        copyFrom(other);
    }

    // Copy assignment operator: assigns the contents of another DynamicBuffer to this DynamicBuffer
    NEX_HIDDEN_FROM_ABI constexpr DynamicBuffer& operator=(const DynamicBuffer& other) noexcept(false) {
        if (this != &other) {
            copyAssignFrom(other);
        }
        return *this;
    }

    // Move constructor: creates a DynamicBuffer by moving another DynamicBuffer
    NEX_HIDDEN_FROM_ABI constexpr DynamicBuffer(DynamicBuffer&& other) noexcept {
        moveFrom(NEX_MOVE(other));
    }

    // Move assignment operator: assigns the contents of another DynamicBuffer to this DynamicBuffer
    NEX_HIDDEN_FROM_ABI constexpr DynamicBuffer& operator=(DynamicBuffer&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (this != &other) {
            reset();
            moveFrom(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor: destroys all elements in the buffer and deallocates memory
    NEX_HIDDEN_FROM_ABI constexpr ~DynamicBuffer()
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        reset();
    }

    // Returns the current number of elements in the buffer.
    NEX_HIDDEN_FROM_ABI constexpr size_type size() const noexcept { return size_; }

    // Returns the current capacity of the buffer (number of elements it can hold without resizing).
    NEX_HIDDEN_FROM_ABI constexpr size_type capacity() const noexcept { return capacity_; }

    // Returns the maximum number of elements the buffer can hold (limited by system memory).
    NEX_HIDDEN_FROM_ABI static constexpr size_type maxSize() noexcept {
        return NumericLimits<size_type>::max() / sizeof(value_type);
    }

    // Reserves storage for at least newCapacity elements (no-op if already sufficient).
    // If newCapacity is greater than the current capacity, the buffer is reallocated.
    NEX_HIDDEN_FROM_ABI constexpr void reserve(size_type newCapacity) {
        if (newCapacity > capacity_) {
            grow(newCapacity);
        }
    }

    // Resizes the buffer to contain a specified number of elements, default-constructing new elements if necessary.
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count) noexcept(false) {
        if (count > capacity_) {
            grow(count);
        }
        if (count < size_) {
            // Destroy excess elements
            NEX_DESTROY_RANGE(data_ + count, data_ + size_);
        } else if (count > size_) {
            // Default-construct new elements
            for (size_type i = size_; i < count; ++i) {
                NEX_CONSTRUCT_AT(data_ + i);
            }
        }
        size_ = count;
    }

    // Resizes the buffer to contain a specified number of elements,
    // initializing new elements to a given value if necessary.
    template <typename OtherValue>
        requires (meta::IsConvertibleV<OtherValue, value_type>)
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count, const OtherValue& value) noexcept(false) {
        if (count < size_) {
            // Destroy excess elements
            NEX_DESTROY_RANGE(data_ + count, data_ + size_);
        } else if (count > size_) {
            reserve(count);
            // Construct new elements with the given value
            for (size_type i = size_; i < count; ++i) {
                NEX_CONSTRUCT_AT(data_ + i, value);
            }
        }
        size_ = count;
    }

    // Shrinks the buffer's capacity to fit its current size, deallocating any excess memory.
    NEX_HIDDEN_FROM_ABI constexpr void shrinkToFit() noexcept(false) {
        if (size_ < capacity_) {
            if (size_ == 0) {
                reset();
            } else {
                pointer_type newData = allocate(size_);
                relocateTo(newData);
                capacity_ = size_;
            }
        }
    }

    // Clears the buffer, destroying all elements and resetting the size to zero.
    NEX_HIDDEN_FROM_ABI constexpr void clear() noexcept(meta::IsNothrowDestructibleV<value_type>) {
        if (size_ == 0) return;
        NEX_DESTROY_RANGE(data_, data_ + size_);
        size_ = 0;
    }

    // Resets the buffer, destroying all elements and deallocating memory.
    NEX_HIDDEN_FROM_ABI constexpr void reset() noexcept(meta::IsNothrowDestructibleV<value_type>) {
        clear();
        deallocate(data_);
        data_ = nullptr;
        capacity_ = 0;
    }

    // Appends elements from a given range which is specified by a pointer to its first element
    // and a size to the end of the buffer, growing if necessary.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void append(const OtherValue* src, size_type count) noexcept(false)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        NEX_ASSERT(src != nullptr || count == 0);
        if (src == nullptr || count == 0) return;
        if (size_ + count > capacity_) {
            grow(size_ + count);
        }
        NEX_COPY_RANGE(data_ + size_, src, src + count);
        size_ += count;
    }

    // Appends elements from a given range which is specified by a pointer to its first element
    // and a pointer to its last element (exclusive) to the end of the buffer, growing if necessary.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void append(const OtherValue* first, const OtherValue* last) noexcept(false)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        NEX_ASSERT(first != nullptr && last != nullptr && first <= last);
        append(first, static_cast<size_type>(last - first));
    }

    // Appends elements from a given range which is specified by a pointer to its first element
    // and a size to the end of the buffer, the source elements are moved, growing if necessary.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void appendMove(OtherValue* src, size_type count) noexcept(false)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        NEX_ASSERT(src != nullptr || count == 0);
        if (src == nullptr || count == 0) return;
        if (size_ + count > capacity_) {
            grow(size_ + count);
        }
        for (size_type i = 0; i < count; ++i) {
            NEX_CONSTRUCT_AT(data_ + size_ + i, NEX_MOVE(src[i]));
        }
        size_ += count;
    }

    // Appends an element constructed in place at the end of the buffer with perfect forwarding
    // of constructor arguments, growing if necessary.
    template <typename... Args>
        requires (meta::IsConstructibleV<value_type, Args...>)
    NEX_HIDDEN_FROM_ABI constexpr void appendConstruct(Args&&... args) noexcept(false) {
        if (size_ + 1 > capacity_) {
            grow(size_ + 1);
        }
        NEX_CONSTRUCT_AT(data_ + size_, NEX_FORWARD<Args>(args)...);
        ++size_;
    }

    // Removes elements from the buffer starting at index and spanning count elements,
    // shifting subsequent elements down.
    NEX_HIDDEN_FROM_ABI constexpr void remove(size_type index, size_type count = 1)
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        NEX_ASSERT(index < size_ && count > 0 && index + count <= size_);
        pointer_type start = data_ + index;
        pointer_type end = start + count;
        NEX_DESTROY_RANGE(start, end);
        NEX_RELOCATE_RANGE(start, end, start);
        size_ -= count;
    }

    // Swaps contents with another DynamicBuffer (no exceptions thrown).
    NEX_HIDDEN_FROM_ABI constexpr void swap(DynamicBuffer& other) noexcept {
        if (this == &other) return;
        DynamicBuffer<value_type, Growth> tmp;
        tmp.moveFrom(NEX_MOVE(*this));
        moveFrom(NEX_MOVE(other));
        other.moveFrom(NEX_MOVE(tmp));
    }

    // Gets access to the underlying contiguous storage.
    NEX_HIDDEN_FROM_ABI constexpr pointer_type data() noexcept { return data_; }

    // Gets access to the underlying contiguous storage (read-only).
    NEX_HIDDEN_FROM_ABI constexpr const_pointer_type data() const noexcept { return data_; }

    // Returns a MemoryBlock representing the buffer's data (read/write).
    NEX_HIDDEN_FROM_ABI constexpr MemoryBlock block() noexcept {
        return MemoryBlock(data(), size_ * sizeof(value_type));
    }

    // Returns a ConstMemoryBlock representing the buffer's data (read-only).
    NEX_HIDDEN_FROM_ABI constexpr ConstMemoryBlock cblock() const noexcept {
        return ConstMemoryBlock(data(), size_ * sizeof(value_type));
    }

    // Gets access to the element at index (with bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type operator[](size_type pos) noexcept {
        NEX_ASSERT_MSG(pos < size_, "Error: Index out of bounds");
        return data_[pos];
    }

    // Gets access to the element at index (read-only, with bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type operator[](size_type pos) const noexcept {
        NEX_ASSERT_MSG(pos < size_, "Error: Index out of bounds");
        return data_[pos];
    }

    // Gets access to the first element of the buffer (read/write, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type front() noexcept {
        return data_[0];
    }

    // Gets access to the first element of the buffer (read-only, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type front() const noexcept {
        return data_[0];
    }

    // Gets access to the last element of the buffer (read/write, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type back() noexcept {
        return data_[size_ - 1];
    }

    // Gets access to the last element of the buffer (read-only, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type back() const noexcept {
        return data_[size_ - 1];
    }
};

/**
 * @class SmallBuffer
 * @brief A small, inline-optimized dynamic buffer that can grow as needed.
 * 
 * @details
 * The SmallBuffer class is a dynamic buffer that optimizes for small sizes by using inline storage.
 * It can hold a specified number of elements in an internal buffer without heap allocation.
 * When the number of elements exceeds the inline capacity, it allocates memory on the heap and grows
 * according to the specified growth policy.
 * 
 * @tparam Type The type of elements stored in the buffer.
 * @tparam InlineCapacity The number of elements that can be stored inline without heap allocation (must be > 0).
 * @tparam Growth The growth policy to use when resizing the buffer. Default is DoubleGrowth.
 */
template <
    typename Type, 
    usize InlineCapacity = 16,
    GrowthPolicy Growth = DoubleGrowth
> requires (InlineCapacity > 0)
class NEX_HIDDEN_FROM_ABI SmallBuffer {
public:
    using value_type = Type;
    using size_type = usize;
    using pointer_type = value_type*;
    using const_pointer_type = const value_type*;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;

    // Inline buffer capacity constant
    static constexpr size_type inlineCapacity = InlineCapacity;

private:
    // Raw aligned storage that can hold InlineCapacity elements of type Type without constructing them.
    NEX_ALIGNAS(value_type) byte inlineBuffer_[sizeof(value_type) * inlineCapacity];

    // Pointer to the data buffer
    // which points to inlineBuffer_ when size_ <= InlineCapacity, otherwise points to heap allocated memory.
    pointer_type data_ = inlineData();

    // Current number of elements in the buffer
    // which is the real size of the buffer, no matter if it is using inlineBuffer_ or heap allocated memory.
    size_type size_ = 0;

    // Current capacity of the buffer
    // which equals InlineCapacity if data_ == inlineBuffer_, otherwise equals the size of the heap allocation.
    size_type capacity_ = inlineCapacity;

    // Gets a pointer to the inline buffer.
    NEX_HIDDEN_FROM_ABI constexpr pointer_type inlineData() noexcept {
        return NEX_MEMORY_CAST<value_type>(inlineBuffer_);
    }

    // Returns true when we are using the inline buffer.
    NEX_HIDDEN_FROM_ABI constexpr bool isInline() const noexcept {
        return data_ == inlineData();
    }

    // Copies the entire buffer from another SmallBuffer.
    // Used in the copy constructor, when the current buffer is uninitialized.
    NEX_HIDDEN_FROM_ABI constexpr void copyFrom(const SmallBuffer& other) {
        reserve(other.size_);
        NEX_COPY_RANGE(data_, other.data_, other.data_ + other.size_);
        size_ = other.size_;
    }

    // Copies the entire buffer from another SmallBuffer.
    // Used in the copy assignment operator, when the current buffer is already initialized.
    NEX_HIDDEN_FROM_ABI constexpr void copyAssignFrom(const SmallBuffer& other) {
        if (other.size_ > capacity_) {
            // Not enough capacity, need to allocate new storage
            reset();
            reserve(other.size_);
            NEX_COPY_RANGE(data_, other.data_, other.data_ + other.size_);
        } else if (other.size_ <= size_) {
            // Copy common elements and destroy remaining part
            NEX_ASSIGN_RANGE(data_, other.data_, other.data_ + other.size_);
            NEX_DESTROY_RANGE(data_ + other.size_, data_ + size_);
        } else {
            // Copy common elements and construct new elements
            pointer_type commonEnd = other.data_ + size_;
            NEX_ASSIGN_RANGE(data_, other.data_, commonEnd);
            NEX_COPY_RANGE(data_ + size_, commonEnd, other.data_ + other.size_);
        }
        size_ = other.size_;
    }

    // Moves the entire buffer from another SmallBuffer (used in move constructor and move assignment)
    NEX_HIDDEN_FROM_ABI constexpr void moveFrom(SmallBuffer&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (other.isInline()) {
            // Cannot steal the inline buffer -> move elements individually
            NEX_RELOCATE_RANGE(data_, other.data_, other.data_ + other.size_);
            size_ = other.size_;
            other.size_ = 0;
        } else {
            // Steal the heap data buffer
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = other.inlineData();
            other.size_ = 0;
            other.capacity_ = inlineCapacity;
        }
        NEX_ASSERT(other.isInline());
    }

    // Relocates the buffer to a new memory location, moving all live elements and deallocating the old buffer.
    NEX_HIDDEN_FROM_ABI constexpr void relocateTo(pointer_type newData) noexcept(false) {
        if (size_ > 0) {
            try {
                NEX_RELOCATE_RANGE(newData, data_, data_ + size_);
            } catch (...) {
                deallocate(newData);
                throw;
            }
        }
        if (!isInline()) {
            deallocate(data_);
        }
        data_ = newData;
    }

    // Allocates a raw, uninitialized heap buffer for exactly cap elements.
    static NEX_HIDDEN_FROM_ABI pointer_type allocate(size_type cap) noexcept(false) {
        return static_cast<pointer_type>(::operator new(cap * sizeof(value_type)));
    }

    // Frees a heap buffer (does NOT destroy elements — caller must do that first)
    static NEX_HIDDEN_FROM_ABI void deallocate(pointer_type ptr)
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        ::operator delete(static_cast<void*>(ptr));
    }

    // Grows capacity to at least requiredCapacity, migrating all live elements.
    NEX_HIDDEN_FROM_ABI void grow(size_type requiredCapacity) {
        size_type newCapacity = Growth::grow(capacity_, requiredCapacity, maxSize());
        NEX_ASSERT(newCapacity >= requiredCapacity && newCapacity <= maxSize());
        pointer_type newData = allocate(newCapacity);
        relocateTo(newData);
        capacity_ = newCapacity;
    }

public:
    // Default constructor initializes an empty SmallBuffer with inline storage.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer() noexcept = default;

    // Constructs a SmallBuffer with a given count, default-constructing elements.
    NEX_HIDDEN_FROM_ABI constexpr explicit SmallBuffer(size_type count) noexcept(false) {
        resize(count);
    }

    // Constructs a SmallBuffer with a given count, copy-constructing elements from value.
    template <typename OtherValue>
        requires(meta::IsConvertibleV<OtherValue, value_type>)
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer(size_type count, const OtherValue& value) noexcept(false) {
        resize(count, value);
    }

    // Copy constructor: creates a SmallBuffer by copying another SmallBuffer.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer(const SmallBuffer& other) noexcept(false) {
        copyFrom(other);
    }

    // Copy assignment operator: assigns the contents of another SmallBuffer to this one.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer& operator=(const SmallBuffer& other) noexcept(false) {
        if (this != &other) {
            copyAssignFrom(other);
        }
        return *this;
    }

    // Move constructor: creates a SmallBuffer by moving another SmallBuffer.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer(SmallBuffer&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type>) {
        moveFrom(NEX_MOVE(other));
    }

    // Move assignment operator: moves the contents of another SmallBuffer to this one.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer& operator=(SmallBuffer&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type>) {
        if (this != &other) {
            reset();
            moveFrom(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor: destroys all elements and releases heap memory if used.
    NEX_HIDDEN_FROM_ABI constexpr ~SmallBuffer() 
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        NEX_DESTROY_RANGE(data_, data_ + size_);
        if (!isInline()) deallocate(data_);
    }

    // Returns the current number of elements in the buffer.
    NEX_HIDDEN_FROM_ABI constexpr size_type size() const noexcept { return size_; }

    // Determines whether the buffer is empty (contains no elements).
    NEX_HIDDEN_FROM_ABI constexpr bool empty() const noexcept { return size_ == 0; }

    // Returns the current capacity of the buffer (maximum number of elements it can hold).
    NEX_HIDDEN_FROM_ABI constexpr size_type capacity() const noexcept { return capacity_; }

    // Returns the maximum number of elements the buffer can hold (limited by size_type).
    NEX_HIDDEN_FROM_ABI static constexpr size_type maxSize() noexcept {
        return NumericLimits<size_type>::max() / sizeof(value_type);
    }

    // Reserves storage for at least newCapacity elements (no-op if already sufficient).
    // If newCapacity is greater than the current capacity, the buffer is reallocated.
    NEX_HIDDEN_FROM_ABI constexpr void reserve(size_type newCapacity) {
        if (newCapacity > capacity()) {
            grow(newCapacity);
        }
    }

    // Resizes to count elements, default-constructing new elements if necessary.
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count) {
        if (count < size_) {
            // Destroy excess elements
            NEX_DESTROY_RANGE(data_ + count, data_ + size_);
        } else if (count > size_) {
            reserve(count);
            // Default-construct new elements
            for (size_type i = size_; i < count; ++i) {
                NEX_CONSTRUCT_AT((data_ + i));
            }
        }
        size_ = count;
    }

    // Resizes to count elements, copy-constructing new elements from value if necessary.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count, const OtherValue& value)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        if (count < size_) {
            // Destroy excess elements
            NEX_DESTROY_RANGE(data_ + count, data_ + size_);
        } else if (count > size_) {
            reserve(count);
            // Construct new elements with the given value
            for (size_type i = size_; i < count; ++i) {
                NEX_CONSTRUCT_AT((data_ + i), value);
            }
        }
        size_ = count;
    }

    // Shrinks the buffer to fit its current size, releasing any excess heap memory if used.
    NEX_HIDDEN_FROM_ABI constexpr void shrinkToFit() {
        if (isInline()) return;
        if (size_ == 0) {
            deallocate(data_);
            data_ = inlineData();
            capacity_ = inlineCapacity;
            return;
        }
        if (size_ <= inlineCapacity) {
            // Move elements back to inline buffer
            relocateTo(inlineData());
            capacity_ = inlineCapacity;
            return;
        }
        if (size_ < capacity_) {
            pointer_type newData = allocate(size_);
            relocateTo(newData);
            capacity_ = size_;
        }
    }

    // Clears the buffer, destroying all elements and resetting the size to zero.
    NEX_HIDDEN_FROM_ABI constexpr void clear() noexcept {
        if (size_ == 0) return;
        NEX_DESTROY_RANGE(data_, data_ + size_);
        size_ = 0;
    }

    // Resets the buffer to the default state, destroying all elements and releasing heap memory if used.
    NEX_HIDDEN_FROM_ABI constexpr void reset() noexcept {
        clear();
        if (!isInline()) {
            deallocate(data_);
            data_ = inlineData();
            capacity_ = inlineCapacity;
        }
    }

    // Appends elements from a given range which is specified by a pointer to its first element
    // and a size to the end of the buffer, growing if necessary.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void append(const OtherValue* src, size_type count)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        NEX_ASSERT(src != nullptr || count == 0);
        if (src == nullptr || count == 0) return;
        reserve(size_ + count);
        NEX_COPY_RANGE(data_ + size_, src, src + count);
        size_ += count;
    }

    // Appends elements from a given range which is specified by a pointer to its first element
    // and a pointer to its last element (exclusive) to the end of the buffer, growing if necessary.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void append(const OtherValue* first, const OtherValue* last)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        NEX_ASSERT(first != nullptr && last != nullptr && first <= last);
        append(first, static_cast<size_type>(last - first));
    }

    // Appends elements from a given range which is specified by a pointer to its first element
    // and a size to the end of the buffer, the source elements are moved, growing if necessary.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void appendMove(OtherValue* src, size_type count)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        NEX_ASSERT(src != nullptr || count == 0);
        if (src == nullptr || count == 0) return;
        reserve(size_ + count);
        for (size_type i = 0; i < count; ++i) {
            NEX_CONSTRUCT_AT(data_ + size_ + i, NEX_MOVE(src[i]));
        }
        size_ += count;
    }

    // Appends an element constructed in place at the end of the buffer with perfect forwarding
    // of constructor arguments, growing if necessary.
    template <typename... Args>
        requires (meta::IsConstructibleV<value_type, Args...>)
    NEX_HIDDEN_FROM_ABI constexpr void appendConstruct(Args&&... args) {
        reserve(size_ + 1);
        NEX_CONSTRUCT_AT(data_ + size_, NEX_FORWARD<Args>(args)...);
        ++size_;
    }

    // Removes elements from the buffer starting at index and spanning count elements,
    // shifting subsequent elements down.
    NEX_HIDDEN_FROM_ABI constexpr void remove(size_type index, size_type count = 1) {
        NEX_ASSERT(index < size_ && count > 0 && index + count <= size_);
        pointer_type start = data_ + index;
        pointer_type end = start + count;
        NEX_DESTROY_RANGE(start, end);
        NEX_RELOCATE_RANGE(start, end, start);
        size_ -= count;
    }

    // Swaps contents with another SmallBuffer (no exceptions thrown).
    NEX_HIDDEN_FROM_ABI constexpr void swap(SmallBuffer& other) noexcept(
        meta::IsNothrowMoveConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (this == &other) return;
        if (!isInline() && !other.isInline()) {
            // Both are on heap -> swap pointers and sizes
            NEX_SWAP(data_, other.data_);
            NEX_SWAP(size_, other.size_);
            NEX_SWAP(capacity_, other.capacity_);
        } else {
            // At least one is using inline storage -> use a temporary buffer to swap
            SmallBuffer tmp;
            tmp.moveFrom(NEX_MOVE(*this));
            moveFrom(NEX_MOVE(other));
            other.moveFrom(NEX_MOVE(tmp));
        }
    }

    // Checks whether the container is currently using the inline buffer.
    NEX_HIDDEN_FROM_ABI constexpr bool usingInlineStorage() const noexcept { return isInline(); }

    // Gets access to the underlying contiguous storage.
    NEX_HIDDEN_FROM_ABI constexpr pointer_type data() noexcept { return data_; }

    // Gets access to the underlying contiguous storage (read-only).
    NEX_HIDDEN_FROM_ABI constexpr const_pointer_type data() const noexcept { return data_; }

    // Returns a MemoryBlock representing the buffer's data (read/write).
    NEX_HIDDEN_FROM_ABI constexpr MemoryBlock block() noexcept {
        return MemoryBlock(data_, size_ * sizeof(value_type));
    }

    // Returns a ConstMemoryBlock representing the buffer's data (read-only).
    NEX_HIDDEN_FROM_ABI constexpr ConstMemoryBlock cblock() const noexcept {
        return ConstMemoryBlock(data_, size_ * sizeof(value_type));
    }

    // Gets access to the element at index (with bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type operator[](size_type pos) noexcept {
        NEX_ASSERT_MSG(pos < size_, "Error: Index out of bounds");
        return data_[pos];
    }

    // Gets access to the element at index (read-only, with bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type operator[](size_type pos) const noexcept {
        NEX_ASSERT_MSG(pos < size_, "Error: Index out of bounds");
        return data_[pos];
    }

    // Gets access to the first element of the buffer (read/write, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type front() noexcept {
        return data_[0];
    }

    // Gets access to the first element of the buffer (read-only, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type front() const noexcept {
        return data_[0];
    }

    // Gets access to the last element of the buffer (read/write, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type back() noexcept {
        return data_[size_ - 1];
    }

    // Gets access to the last element of the buffer (read-only, no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type back() const noexcept {
        return data_[size_ - 1];
    }
};

NEX_NAMESPACE_END
