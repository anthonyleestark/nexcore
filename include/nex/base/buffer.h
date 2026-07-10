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
#include "nex/base/memory.h"

NEX_NAMESPACE_BEGIN

/**
 * @enum GrowthPolicy
 * @brief Defines the growth policy for dynamic buffer resizing.
 * 
 * @details
 * The GrowthPolicy enumeration specifies how a dynamic buffer should grow when its capacity is exceeded.
 * - `Double`: The buffer's capacity is doubled when it needs to grow. 
 *             This is a common strategy that provides amortized constant time complexity for insertions.
 * - `Linear`: The buffer's capacity is increased by a fixed amount (typically 1) when it needs to grow.
 */
enum class GrowthPolicy {
    Double,     // Double the capacity when growing
    Linear      // Increase capacity by a fixed amount when growing
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
 * @tparam Policy The growth policy to use when resizing the buffer. Default is GrowthPolicy::Double.
 */
template <
    typename Type, 
    usize InlineCapacity = 16,
    GrowthPolicy Policy = GrowthPolicy::Double
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

    // Get a pointer to the inline buffer
    NEX_HIDDEN_FROM_ABI constexpr pointer_type inlineData() noexcept {
        return NEX_MEMORY_CAST<value_type>(inlineBuffer_);
    }

    // Returns true when we are using the inline buffer
    NEX_HIDDEN_FROM_ABI constexpr bool isInline() const noexcept {
        return data_ == inlineData();
    }

    // Assign elements from [first, last) to initialized storage at dest
    // by directly copying the whole memory range, used when the value type is trivially copy assignable
    static NEX_HIDDEN_FROM_ABI constexpr
    void assignRange(pointer_type dest, const_pointer_type first, const_pointer_type last) noexcept
        requires(meta::IsTriviallyCopyAssignableV<value_type>) {
        NEX_MEMCPY(dest, first, (last - first) * sizeof(value_type));
    }

    // Assign elements from [first, last) to initialized storage at dest
    // used when the value type is not trivially copy assignable
    static NEX_HIDDEN_FROM_ABI constexpr 
    void assignRange(pointer_type dest, const_pointer_type first, const_pointer_type last)
        noexcept(meta::IsNothrowCopyAssignableV<value_type>) {
        pointer_type current = dest;
        for (; first != last; ++first, ++current) {
            *current = *first;
        }
    }

    // Copy elements [first, last) to uninitialized storage at dest,
    // by directly copying the whole memory range, used when the value type is trivially copy constructible
    static NEX_HIDDEN_FROM_ABI constexpr
    void copyRange(pointer_type dest, const_pointer_type first, const_pointer_type last) noexcept
        requires(meta::IsTriviallyCopyConstructibleV<value_type>) {
        NEX_MEMCPY(dest, first, (last - first) * sizeof(value_type));
    }

    // Copy elements [first, last) to uninitialized storage at dest
    // used when the value type is not trivially copy constructible
    static NEX_HIDDEN_FROM_ABI constexpr
    void copyRange(pointer_type dest, const_pointer_type first, const_pointer_type last) 
        noexcept(meta::IsNothrowCopyConstructibleV<value_type>) {
        pointer_type current = dest;
        for (; first != last; ++first, ++current) {
            NEX_CONSTRUCT_AT(current, *first);
        }
    }

    // Relocate elements [first, last) to uninitialized storage at dest,
    // by directly moving the whole memory range, used when the value type is trivially move constructible
    static NEX_HIDDEN_FROM_ABI constexpr
    void relocateRange(pointer_type dest, pointer_type first, pointer_type last) noexcept
        requires(meta::IsTriviallyMoveConstructibleV<value_type>) {
        NEX_MEMMOVE(dest, first, (last - first) * sizeof(value_type));
    }

    // Relocate elements [first, last) to uninitialized storage at dest,
    // used when the value type is not trivially move constructible
    static NEX_HIDDEN_FROM_ABI constexpr
    void relocateRange(pointer_type dest, pointer_type first, pointer_type last) 
        noexcept(meta::IsNothrowMoveConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        pointer_type current = dest;
        pointer_type begin = first;
        for (; first != last; ++first, ++current) {
            NEX_CONSTRUCT_AT(current, NEX_MOVE(*first));
        }
        destroyRange(begin, last);
    }

    // Destroy elements in range [first, last),
    // used when the value type is trivially destructible
    static NEX_HIDDEN_FROM_ABI constexpr 
    void destroyRange(pointer_type first, pointer_type last) noexcept
        requires(meta::IsTriviallyDestructibleV<value_type>) {
        // No-op for trivially destructible types
    }

    // Destroy elements in range [first, last)
    static NEX_HIDDEN_FROM_ABI constexpr 
    void destroyRange(pointer_type first, pointer_type last)
        noexcept(meta::IsNothrowDestructibleV<value_type>)
        requires(!meta::IsTriviallyDestructibleV<value_type>) {
        for (; first != last; ++first) {
            first->~value_type();
        }
    }

    // Copy the entire buffer from another SmallBuffer
    // used in copy constructor, when the current buffer is uninitialized
    NEX_HIDDEN_FROM_ABI constexpr void copyFrom(const SmallBuffer& other)
        noexcept(meta::IsNothrowCopyConstructibleV<value_type>) {
        reserve(other.size_);
        copyRange(data_, other.data_, other.data_ + other.size_);
        size_ = other.size_;
    }

    // Copy the entire buffer from another SmallBuffer
    // used in copy assignment operator, when the current buffer is already initialized
    NEX_HIDDEN_FROM_ABI constexpr void copyAssignFrom(const SmallBuffer& other)
        noexcept(meta::IsNothrowCopyConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (other.size_ > capacity_) {
            // Not enough capacity, need to allocate new storage
            reset();
            reserve(other.size_);
            copyRange(data_, other.data_, other.data_ + other.size_);
        } else if (other.size_ <= size_) {
            // Copy common elements and destroy remaining part
            assignRange(data_, other.data_, other.data_ + other.size_);
            destroyRange(data_ + other.size_, data_ + size_);
        } else {
            // Copy common elements and construct new elements
            pointer_type commonEnd = other.data_ + size_;
            assignRange(data_, other.data_, commonEnd);
            copyRange(data_ + size_, commonEnd, other.data_ + other.size_);
        }
        size_ = other.size_;
    }

    // Move the entire buffer from another SmallBuffer (used in move constructor and move assignment)
    NEX_HIDDEN_FROM_ABI constexpr void moveFrom(SmallBuffer&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type> && meta::IsNothrowDestructibleV<value_type>) {
        if (other.isInline()) {
            // Cannot steal the inline buffer -> move elements individually
            relocateRange(data_, other.data_, other.data_ + other.size_);
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
    }

    // Allocate a raw, uninitialized heap buffer for exactly cap elements
    static NEX_HIDDEN_FROM_ABI pointer_type allocate(size_type cap) noexcept(false) {
        return static_cast<pointer_type>(::operator new(cap * sizeof(value_type)));
    }

    // Free a heap buffer (does NOT destroy elements — caller must do that first)
    static NEX_HIDDEN_FROM_ABI void deallocate(pointer_type ptr)
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        ::operator delete(static_cast<void*>(ptr));
    }

    // Calculate the next capacity based on the growth policy
    static NEX_HIDDEN_FROM_ABI constexpr 
    size_type nextCapacity(size_type current, size_type required) {
        if constexpr (Policy == GrowthPolicy::Double) {
            size_type newCapacity = current > 0 ? current : 1;
            while (newCapacity < required) {
                newCapacity *= 2;
            }
            return newCapacity;
        } else { // Linear growth
            return required;
        }
    }

    // Grow capacity to at least requiredCapacity, migrating all live elements
    NEX_HIDDEN_FROM_ABI void grow(size_type requiredCapacity) {
        size_type newCapacity = nextCapacity(capacity_, requiredCapacity);
        NEX_ASSERT(newCapacity >= requiredCapacity);
        pointer_type newData = allocate(newCapacity);
        if (size_ > 0) {
            try {
                relocateRange(newData, data_, data_ + size_);
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
    // Default constructor
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer() noexcept = default;

    // Constructs a SmallBuffer with a given count, default-constructing elements
    NEX_HIDDEN_FROM_ABI constexpr explicit SmallBuffer(size_type count) {
        resize(count);
    }

    // Constructs a SmallBuffer with a given count, copy-constructing elements from value
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer(size_type count, const_reference_type value) {
        resize(count, value);
    }

    // Copy constructor
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer(const SmallBuffer& other) {
        copyFrom(other);
    }

    // Copy assignment operator
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer& operator=(const SmallBuffer& other) {
        if (this != &other) {
            copyAssignFrom(other);
        }
        return *this;
    }

    // Move constructor
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer(SmallBuffer&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type>) {
        moveFrom(NEX_MOVE(other));
    }

    // Move assignment operator
    NEX_HIDDEN_FROM_ABI constexpr SmallBuffer& operator=(SmallBuffer&& other)
        noexcept(meta::IsNothrowMoveConstructibleV<value_type>) {
        if (this != &other) {
            reset();
            moveFrom(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor
    NEX_HIDDEN_FROM_ABI constexpr ~SmallBuffer() 
        noexcept(meta::IsNothrowDestructibleV<value_type>) {
        destroyRange(data_, data_ + size_);
        if (!isInline()) deallocate(data_);
    }

    // Get the number of elements
    NEX_HIDDEN_FROM_ABI constexpr size_type size() const noexcept { return size_; }

    // Check if the container is empty
    NEX_HIDDEN_FROM_ABI constexpr bool empty() const noexcept { return size_ == 0; }

    // Get the current storage capacity
    NEX_HIDDEN_FROM_ABI constexpr size_type capacity() const noexcept { return capacity_; }

    // Reserve storage for at least newCapacity elements (no-op if already sufficient)
    NEX_HIDDEN_FROM_ABI constexpr void reserve(size_type newCapacity) {
        if (newCapacity > capacity()) {
            grow(newCapacity);
        }
    }

    // Resize to count elements (new elements are default-constructed)
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count) {
        if (count < size_) {
            destroyRange(data_ + count, data_ + size_);
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i) {
                NEX_CONSTRUCT_AT((data_ + i));
            }
        }
        size_ = count;
    }

    // Resize to count elements, filling new slots with value
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type count, const_reference_type value) {
        if (count < size_) {
            destroyRange(data_ + count, data_ + size_);
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i) {
                NEX_CONSTRUCT_AT((data_ + i), value);
            }
        }
        size_ = count;
    }

    // Reduce the heap allocation to fit the current size (no effect in inline mode)
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
                relocateRange(inl, data_, data_ + size_);
            } catch (...) {
                destroyRange(inl, inl + size_);
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
                relocateRange(newData, data_, data_ + size_);
            } catch (...) {
                deallocate(newData);
                throw;
            }
            deallocate(data());
            data_ = newData;
            capacity_ = size_;
        }
    }

    // Remove all elements (does not release memory)
    NEX_HIDDEN_FROM_ABI constexpr void clear() noexcept {
        destroyRange(data_, data_ + size_);
        size_ = 0;
    }

    // Reset the buffer to the default state (using inline storage), releasing heap memory if used
    NEX_HIDDEN_FROM_ABI constexpr void reset() noexcept {
        clear();
        if (!isInline()) {
            deallocate(data_);
            data_ = inlineData();
            capacity_ = inlineCapacity;
        }
    }

    // Swap contents with another SmallBuffer (no exceptions thrown)
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

    // Get the maximum possible number of elements
    static NEX_HIDDEN_FROM_ABI constexpr size_type maxSize() noexcept {
        return NumericLimits<size_type>::max() / sizeof(value_type);
    }

    // Check whether the container is currently using the inline buffer
    NEX_HIDDEN_FROM_ABI constexpr bool usingInlineStorage() const noexcept { return isInline(); }

    // Get pointer to the underlying contiguous storage
    NEX_HIDDEN_FROM_ABI constexpr pointer_type data() noexcept { return data_; }

    // Get pointer to the underlying contiguous storage (read-only)
    NEX_HIDDEN_FROM_ABI constexpr const_pointer_type data() const noexcept { return data_; }

    // Get the memory block representing the buffer's data
    NEX_HIDDEN_FROM_ABI constexpr MemoryBlock block() noexcept {
        return MemoryBlock(data(), size_ * sizeof(value_type));
    }

    // Get the memory block representing the buffer's data (read-only)
    NEX_HIDDEN_FROM_ABI constexpr ConstMemoryBlock cblock() const noexcept {
        return ConstMemoryBlock(data(), size_ * sizeof(value_type));
    }

    // Access element at index (no bounds checking)
    NEX_HIDDEN_FROM_ABI constexpr reference_type operator[](size_type pos) noexcept {
        return data_[pos];
    }

    // Access element at index (no bounds checking, read-only)
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type operator[](size_type pos) const noexcept {
        return data_[pos];
    }

    // Access the first element
    NEX_HIDDEN_FROM_ABI constexpr reference_type front() noexcept {
        return data_[0];
    }

    // Access the first element (read-only)
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type front() const noexcept {
        return data_[0];
    }

    // Access the last element
    NEX_HIDDEN_FROM_ABI constexpr reference_type back() noexcept {
        return data_[size_ - 1];
    }

    // Access the last element (read-only)
    NEX_HIDDEN_FROM_ABI constexpr const_reference_type back() const noexcept {
        return data_[size_ - 1];
    }
};

NEX_NAMESPACE_END
