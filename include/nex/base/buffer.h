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
    using pointer_type = Type*;
    using const_pointer_type = const Type*;
    using reference_type = Type&;
    using const_reference_type = const Type&;

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
    // which equals InlineCapacity if data_ == inlineBuffer_, otherwise equals the size of the heap allocation
    size_type capacity_ = inlineCapacity;

    // Gets a pointer to the inline buffer
    NEX_HIDDEN_FROM_ABI constexpr pointer_type inlineData() noexcept {
        return NEX_MEMORY_CAST<value_type>(inlineBuffer_);
    }

    // Returns true when we are using the inline buffer
    NEX_HIDDEN_FROM_ABI constexpr bool isInline() const noexcept {
        return data_ == inlineData();
    }

    // Copies the entire buffer from another SmallBuffer
    // Used in the copy constructor, when the current buffer is uninitialized
    NEX_HIDDEN_FROM_ABI constexpr void copyFrom(const SmallBuffer& other) {
        reserve(other.size_);
        NEX_COPY_RANGE(data_, other.data_, other.data_ + other.size_);
        size_ = other.size_;
    }

    // Copies the entire buffer from another SmallBuffer
    // Used in the copy assignment operator, when the current buffer is already initialized
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

    // Allocates a raw, uninitialized heap buffer for exactly cap elements
    static NEX_HIDDEN_FROM_ABI pointer_type allocate(size_type cap) noexcept(false) {
        return static_cast<pointer_type>(::operator new(cap * sizeof(value_type)));
    }

    // Frees a heap buffer (does NOT destroy elements — caller must do that first)
    static NEX_HIDDEN_FROM_ABI void deallocate(pointer_type ptr)
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        ::operator delete(static_cast<void*>(ptr));
    }

    // Grows capacity to at least requiredCapacity, migrating all live elements
    NEX_HIDDEN_FROM_ABI void grow(size_type requiredCapacity) {
        size_type newCapacity = Growth::grow(capacity_, requiredCapacity, maxSize());
        NEX_ASSERT(newCapacity >= requiredCapacity && newCapacity <= maxSize());
        pointer_type newData = allocate(newCapacity);
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
        capacity_ = newCapacity;
    }

public:
    // Default constructor initializes an empty SmallBuffer with inline storage.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer() noexcept = default;

    // Constructs a SmallBuffer with a given count, default-constructing elements.
    NEX_HIDDEN_FROM_ABI constexpr explicit SmallBuffer(size_type count) {
        resize(count);
    }

    // Constructs a SmallBuffer with a given count, copy-constructing elements from value.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer(size_type count, const OtherValue& value)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        resize(count, value);
    }

    // Copy constructor.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer(const SmallBuffer& other) {
        copyFrom(other);
    }

    // Copy assignment operator.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer& operator=(const SmallBuffer& other) {
        if (this != &other) {
            copyAssignFrom(other);
        }
        return *this;
    }

    // Move constructor.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer(SmallBuffer&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type>) {
        moveFrom(NEX_MOVE(other));
    }

    // Move assignment operator.
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer& operator=(SmallBuffer&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type>) {
        if (this != &other) {
            reset();
            moveFrom(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor.
    NEX_HIDDEN_FROM_ABI constexpr ~SmallBuffer() 
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        NEX_DESTROY_RANGE(data_, data_ + size_);
        if (!isInline()) deallocate(data_);
    }

    // Gets the number of elements.
    NEX_HIDDEN_FROM_ABI constexpr size_type size() const noexcept { return size_; }

    // Checks if the container is empty.
    NEX_HIDDEN_FROM_ABI constexpr bool empty() const noexcept { return size_ == 0; }

    // Gets the current storage capacity.
    NEX_HIDDEN_FROM_ABI constexpr size_type capacity() const noexcept { return capacity_; }

    // Reserves storage for at least newCapacity elements (no-op if already sufficient).
    NEX_HIDDEN_FROM_ABI constexpr void reserve(size_type newCapacity) {
        if (newCapacity > capacity()) {
            grow(newCapacity);
        }
    }

    // Resizes to count elements (new elements are default-constructed).
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count) {
        if (count < size_) {
            NEX_DESTROY_RANGE(data_ + count, data_ + size_);
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i) {
                NEX_CONSTRUCT_AT((data_ + i));
            }
        }
        size_ = count;
    }

    // Resizes to count elements, filling new slots with value.
    template <typename OtherValue>
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count, const OtherValue& value)
        requires(meta::IsConvertibleV<OtherValue, value_type>) {
        if (count < size_) {
            NEX_DESTROY_RANGE(data_ + count, data_ + size_);
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i) {
                NEX_CONSTRUCT_AT((data_ + i), value);
            }
        }
        size_ = count;
    }

    // Reduces the heap allocation to fit the current size (no effect in inline mode).
    NEX_HIDDEN_FROM_ABI constexpr void shrinkToFit() {
        if (isInline()) return;
        if (size_ == 0) {
            deallocate(data());
            data_ = inlineData();
            capacity_ = inlineCapacity;
            return;
        }
        if (size_ <= inlineCapacity) {
            // Move elements back to inline buffer
            pointer_type inl = inlineData();
            try {
                NEX_RELOCATE_RANGE(inl, data_, data_ + size_);
            } catch (...) {
                NEX_DESTROY_RANGE(inl, inl + size_);
                throw;
            }
            deallocate(data());
            data_ = inlineData();
            capacity_ = inlineCapacity;
            return;
        }
        if (size_ < capacity_) {
            pointer_type newData = allocate(size_);
            try {
                NEX_RELOCATE_RANGE(newData, data_, data_ + size_);
            } catch (...) {
                deallocate(newData);
                throw;
            }
            deallocate(data());
            data_ = newData;
            capacity_ = size_;
        }
    }

    // Removes all elements (does not release memory).
    NEX_HIDDEN_FROM_ABI constexpr void clear() noexcept {
        NEX_DESTROY_RANGE(data_, data_ + size_);
        size_ = 0;
    }

    // Resets the buffer to the default state (using inline storage), releasing heap memory if used.
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
    // and a size to the end of the buffer, growing if necessary. This overload is for rvalue references.
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
        NEX_MOVE_RANGE(start, end, start);
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

    // Gets the maximum possible number of elements.
    static NEX_HIDDEN_FROM_ABI constexpr size_type maxSize() noexcept {
        return NumericLimits<size_type>::max() / sizeof(value_type);
    }

    // Checks whether the container is currently using the inline buffer.
    NEX_HIDDEN_FROM_ABI constexpr bool usingInlineStorage() const noexcept { return isInline(); }

    // Gets pointer to the underlying contiguous storage.
    NEX_HIDDEN_FROM_ABI constexpr pointer_type data() noexcept { return data_; }

    // Gets pointer to the underlying contiguous storage (read-only).
    NEX_HIDDEN_FROM_ABI constexpr const_pointer_type data() const noexcept { return data_; }

    // Gets the memory block representing the buffer's data.
    NEX_HIDDEN_FROM_ABI constexpr MemoryBlock block() noexcept {
        return MemoryBlock(data(), size_ * sizeof(value_type));
    }

    // Gets the memory block representing the buffer's data (read-only).
    NEX_HIDDEN_FROM_ABI constexpr ConstMemoryBlock cblock() const noexcept {
        return ConstMemoryBlock(data(), size_ * sizeof(value_type));
    }

    // Accesses element at index (no bounds checking).
    NEX_HIDDEN_FROM_ABI constexpr reference_type operator[](size_type pos) noexcept {
        return data_[pos];
    }

    // Accesses element at index (no bounds checking, read-only).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type operator[](size_type pos) const noexcept {
        return data_[pos];
    }

    // Accesses the first element.
    NEX_HIDDEN_FROM_ABI constexpr reference_type front() noexcept {
        return data_[0];
    }

    // Accesses the first element (read-only).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type front() const noexcept {
        return data_[0];
    }

    // Accesses the last element.
    NEX_HIDDEN_FROM_ABI constexpr reference_type back() noexcept {
        return data_[size_ - 1];
    }

    // Accesses the last element (read-only).
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type back() const noexcept {
        return data_[size_ - 1];
    }
};

NEX_NAMESPACE_END
