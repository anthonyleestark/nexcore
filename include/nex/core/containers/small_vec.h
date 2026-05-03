/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/linear.h"
#include "nex/base/assert_crash.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @class SmallVec
 * @brief An optimized dynamic array with inline (stack-allocated) small-buffer storage.
 *
 * @details
 * SmallVec<T, N> behaves like a Vec<T> (std::vector<T>) but avoids heap allocation for up to
 * N elements by keeping them directly inside the object (the "inline buffer"). When the number
 * of elements exceeds N the container transparently switches to heap storage and grows with the
 * same amortized O(1) push_back strategy as Vec.
 *
 * Optimisation characteristics:
 * - **Zero heap allocation** for ≤ N elements — creation, destruction, and copy/move are all
 *   stack operations.
 * - **Single allocation** once the inline buffer is exhausted; subsequent growth re-allocates
 *   at double capacity (matching Vec growth policy).
 * - **Contiguous storage** in both modes — data() always returns a pointer to a contiguous
 *   sequence, enabling the same memcpy / SIMD tricks as Vec.
 * - **Standard iterator interface** — random-access iterators, range-for, and all algorithms
 *   that accept begin()/end() pairs work without modification.
 *
 * @tparam T The element type. Must be movable.
 * @tparam N The inline buffer capacity (must be > 0).
 *
 * @note SmallVec is not allocator-aware. Heap storage uses the global operator new/delete.
 *
 * @note Iterators and pointers into a SmallVec are invalidated by any operation that changes the
 *       size or reallocates the buffer (push_back when size == capacity, insert, resize, reserve).
 *       This matches the std::vector contract.
 *
 * @see Vec for a pure heap-based dynamic array.
 * @see Array for a fixed-size stack array.
 */
template <typename T, usize N>
requires (N > 0)
class NEX_EXPORT SmallVec {
public:
    // Type aliases for compatibility with standard container conventions
    using value_type = T;
    using size_type = usize;
    using difference_type = isize;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = NEX_STD reverse_iterator<iterator>;
    using const_reverse_iterator = NEX_STD reverse_iterator<const_iterator>;

public:
    // Inline buffer capacity constant
    static constexpr size_type inlineCapacity = N;

private:
    ////// Inline buffer -------------------------------------------------

    // Raw aligned storage that can hold N objects of type T without constructing them.
    NEX_ALIGNAS(T) NEX_STD byte inlineBuffer_[sizeof(T) * N];

    ////// Heap storage --------------------------------------------------

    // Used when size_ > N.  Null when in inline mode.
    pointer heapData_ = nullptr;
    size_type heapCapacity_ = 0;

    ////// Common state --------------------------------------------------
    size_type size_ = 0;

    ////// Private helpers -----------------------------------------------

    // Returns true when we are using the inline buffer
    bool isInline() const noexcept { return heapData_ == nullptr; }

    // Get pointer to the first element (works in both modes)
    pointer storagePtr() noexcept {
        return heapData_ ? heapData_ : reinterpret_cast<pointer>(inlineBuffer_);
    }

    // Get pointer to the first element (works in both modes; read-only)
    const_pointer storagePtr() const noexcept {
        return heapData_ ? heapData_ : reinterpret_cast<const_pointer>(inlineBuffer_);
    }

    // Move elements [first, last) to uninitialized storage at dest (source is left destroyed)
    static void moveRangeUninit(pointer dest, pointer first, pointer last) noexcept(
        NEX_STD is_nothrow_move_constructible_v<T>) {
        for (; first != last; ++first, ++dest) {
            ::new (static_cast<void*>(dest)) T(NEX_STD move(*first));
            first->~T();
        }
    }

    // Copy elements [first, last) to uninitialized storage at dest
    static void copyRangeUninit(pointer dest, const_pointer first, const_pointer last) {
        pointer cur = dest;
        try {
            for (; first != last; ++first, ++cur) {
                ::new (static_cast<void*>(cur)) T(*first);
            }
        } catch (...) {
            // Destroy successfully constructed elements before re-throwing
            while (cur != dest) {
                --cur;
                cur->~T();
            }
            throw;
        }
    }

    // Destroy elements in range [first, last)
    static void destroyRange(pointer first, pointer last) noexcept {
        for (; first != last; ++first) {
            first->~T();
        }
    }

    // Allocate a raw, uninitialized heap buffer for exactly cap elements
    static pointer allocateHeap(size_type cap) {
        return static_cast<pointer>(::operator new(cap * sizeof(T)));
    }

    // Free a heap buffer (does NOT destroy elements — caller must do that first)
    static void freeHeap(pointer p) noexcept {
        ::operator delete(static_cast<void*>(p));
    }

    // Grow capacity to at least requiredCapacity, migrating all live elements
    void growTo(size_type requiredCapacity) {
        size_type newCap = heapCapacity_ == 0 ? (NEX_STD max)(N * 2, requiredCapacity)
                                               : (NEX_STD max)(heapCapacity_ * 2, requiredCapacity);
        pointer newData = allocateHeap(newCap);
        try {
            moveRangeUninit(newData, storagePtr(), storagePtr() + size_);
        } catch (...) {
            freeHeap(newData);
            throw;
        }
        if (heapData_) {
            freeHeap(heapData_);
        }
        heapData_ = newData;
        heapCapacity_ = newCap;
    }

public:
    ////// Constructors and assignment operators ----------------------------------

    // Default constructor (empty container, no heap allocation)
    SmallVec() noexcept = default;

    // Construct with count default-inserted elements
    explicit SmallVec(size_type count) {
        resize(count);
    }

    // Construct with count copies of value
    SmallVec(size_type count, const_reference value) {
        resize(count, value);
    }

    // Construct from initializer list
    SmallVec(NEX_STD initializer_list<T> init) {
        reserve(init.size());
        for (const T& v : init) {
            push_back(v);
        }
    }

    // Construct from an iterator range [first, last)
    template <typename InputIt,
              typename = NEX_STD enable_if_t<
                  NEX_STD is_convertible_v<
                      typename NEX_STD iterator_traits<InputIt>::value_type, T>>>
    SmallVec(InputIt first, InputIt last) {
        for (; first != last; ++first) {
            push_back(*first);
        }
    }

    // Copy constructor
    SmallVec(const SmallVec& other) {
        reserve(other.size_);
        copyRangeUninit(storagePtr(), other.storagePtr(), other.storagePtr() + other.size_);
        size_ = other.size_;
    }

    // Copy assignment operator
    SmallVec& operator=(const SmallVec& other) {
        if (this == &other) return *this;
        assign(other.begin(), other.end());
        return *this;
    }

    // Move constructor
    SmallVec(SmallVec&& other) noexcept(NEX_STD is_nothrow_move_constructible_v<T>) {
        if (other.isInline()) {
            // Cannot steal the inline buffer — move elements individually
            moveRangeUninit(storagePtr(), other.storagePtr(), other.storagePtr() + other.size_);
            size_ = other.size_;
            other.size_ = 0;
        } else {
            // Steal the heap pointer
            heapData_ = other.heapData_;
            heapCapacity_ = other.heapCapacity_;
            size_ = other.size_;
            other.heapData_ = nullptr;
            other.heapCapacity_ = 0;
            other.size_ = 0;
        }
    }

    // Move assignment operator
    SmallVec& operator=(SmallVec&& other) noexcept(NEX_STD is_nothrow_move_constructible_v<T>) {
        if (this == &other) return *this;
        clear();
        if (other.isInline()) {
            if (!isInline()) {
                freeHeap(heapData_);
                heapData_ = nullptr;
                heapCapacity_ = 0;
            }
            moveRangeUninit(storagePtr(), other.storagePtr(), other.storagePtr() + other.size_);
            size_ = other.size_;
            other.size_ = 0;
        } else {
            if (!isInline()) freeHeap(heapData_);
            heapData_ = other.heapData_;
            heapCapacity_ = other.heapCapacity_;
            size_ = other.size_;
            other.heapData_ = nullptr;
            other.heapCapacity_ = 0;
            other.size_ = 0;
        }
        return *this;
    }

    // Assign from initializer list
    SmallVec& operator=(NEX_STD initializer_list<T> init) {
        assign(init);
        return *this;
    }

    // Destructor
    ~SmallVec() {
        destroyRange(storagePtr(), storagePtr() + size_);
        if (heapData_) freeHeap(heapData_);
    }

    ////// Assignment helpers ----------------------------------

    // Replace contents with count copies of value
    void assign(size_type count, const_reference value) {
        clear();
        resize(count, value);
    }

    // Replace contents from an iterator range [first, last)
    template <typename InputIt,
              typename = NEX_STD enable_if_t<
                  NEX_STD is_convertible_v<
                      typename NEX_STD iterator_traits<InputIt>::value_type, T>>>
    void assign(InputIt first, InputIt last) {
        clear();
        for (; first != last; ++first) {
            push_back(*first);
        }
    }

    // Replace contents with an initializer list
    void assign(NEX_STD initializer_list<T> init) {
        clear();
        reserve(init.size());
        for (const T& v : init) {
            push_back(v);
        }
    }

    ////// Iterator support ----------------------------------

    // Get iterator to the beginning
    iterator begin() noexcept { return storagePtr(); }

    // Get const iterator to the beginning
    const_iterator begin() const noexcept { return storagePtr(); }

    // Get const iterator to the beginning
    const_iterator cbegin() const noexcept { return storagePtr(); }

    // Get iterator to the end
    iterator end() noexcept { return storagePtr() + size_; }

    // Get const iterator to the end
    const_iterator end() const noexcept { return storagePtr() + size_; }

    // Get const iterator to the end
    const_iterator cend() const noexcept { return storagePtr() + size_; }

    // Get reverse iterator to the beginning of the reversed view
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    // Get const reverse iterator to the beginning of the reversed view
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    // Get const reverse iterator to the beginning of the reversed view
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    // Get reverse iterator to the end of the reversed view
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    // Get const reverse iterator to the end of the reversed view
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    // Get const reverse iterator to the end of the reversed view
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    ////// Element access ----------------------------------

    // Access element at index with bounds checking
    reference at(size_type pos) {
        NEX_ASSERT_MSG(pos < size_, "Index out of range");
        return storagePtr()[pos];
    }

    // Access element at index with bounds checking (read-only)
    const_reference at(size_type pos) const {
        NEX_ASSERT_MSG(pos < size_, "Index out of range");
        return storagePtr()[pos];
    }

    // Access element at index (no bounds checking)
    reference operator[](size_type pos) noexcept { return storagePtr()[pos]; }

    // Access element at index (no bounds checking, read-only)
    const_reference operator[](size_type pos) const noexcept { return storagePtr()[pos]; }

    // Access the first element
    reference front() noexcept { return storagePtr()[0]; }

    // Access the first element (read-only)
    const_reference front() const noexcept { return storagePtr()[0]; }

    // Access the last element
    reference back() noexcept { return storagePtr()[size_ - 1]; }

    // Access the last element (read-only)
    const_reference back() const noexcept { return storagePtr()[size_ - 1]; }

    // Get pointer to the underlying contiguous storage
    pointer data() noexcept { return storagePtr(); }

    // Get pointer to the underlying contiguous storage (read-only)
    const_pointer data() const noexcept { return storagePtr(); }

    ////// Capacity and size ----------------------------------

    // Get the number of elements
    size_type size() const noexcept { return size_; }

    // Get the number of elements (same as size)
    size_type length() const noexcept { return size_; }

    // Check if the container is empty
    bool empty() const noexcept { return size_ == 0; }

    // Get the current storage capacity
    size_type capacity() const noexcept {
        return heapData_ ? heapCapacity_ : N;
    }

    // Get the maximum possible number of elements
    size_type maxSize() const noexcept {
        return (NEX_STD numeric_limits<size_type>::max() / sizeof(T)) - 1;
    }

    // Check whether the container is currently using the inline buffer
    bool usingInlineStorage() const noexcept { return isInline(); }

    // Reserve storage for at least newCapacity elements (no-op if already sufficient)
    void reserve(size_type newCapacity) {
        if (newCapacity <= capacity()) return;
        growTo(newCapacity);
    }

    // Reduce the heap allocation to fit the current size (no effect in inline mode)
    void shrinkToFit() {
        if (isInline()) return;
        if (size_ == 0) {
            freeHeap(heapData_);
            heapData_ = nullptr;
            heapCapacity_ = 0;
            return;
        }
        if (size_ <= N) {
            // Move elements back to inline buffer
            pointer inl = reinterpret_cast<pointer>(inlineBuffer_);
            moveRangeUninit(inl, heapData_, heapData_ + size_);
            freeHeap(heapData_);
            heapData_ = nullptr;
            heapCapacity_ = 0;
            return;
        }
        if (size_ < heapCapacity_) {
            pointer newData = allocateHeap(size_);
            moveRangeUninit(newData, heapData_, heapData_ + size_);
            freeHeap(heapData_);
            heapData_ = newData;
            heapCapacity_ = size_;
        }
    }

    ////// Modifiers ----------------------------------

    // Append a copy of value
    void push_back(const_reference value) {
        if (size_ == capacity()) growTo(size_ + 1);
        ::new (static_cast<void*>(storagePtr() + size_)) T(value);
        ++size_;
    }

    // Append a moved value
    void push_back(T&& value) {
        if (size_ == capacity()) growTo(size_ + 1);
        ::new (static_cast<void*>(storagePtr() + size_)) T(NEX_STD move(value));
        ++size_;
    }

    // Construct an element in place at the end
    template <typename... Args>
    reference emplace_back(Args&&... args) {
        if (size_ == capacity()) growTo(size_ + 1);
        T* ptr = ::new (static_cast<void*>(storagePtr() + size_)) T(NEX_STD forward<Args>(args)...);
        ++size_;
        return *ptr;
    }

    // Remove the last element
    void pop_back() noexcept {
        NEX_ASSERT_MSG(size_ > 0, "pop_back called on empty SmallVec");
        --size_;
        storagePtr()[size_].~T();
    }

    // Resize to count elements (new elements are default-constructed)
    void resize(size_type count) {
        if (count < size_) {
            destroyRange(storagePtr() + count, storagePtr() + size_);
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i) {
                ::new (static_cast<void*>(storagePtr() + i)) T();
            }
        }
        size_ = count;
    }

    // Resize to count elements, filling new slots with value
    void resize(size_type count, const_reference value) {
        if (count < size_) {
            destroyRange(storagePtr() + count, storagePtr() + size_);
        } else if (count > size_) {
            reserve(count);
            for (size_type i = size_; i < count; ++i) {
                ::new (static_cast<void*>(storagePtr() + i)) T(value);
            }
        }
        size_ = count;
    }

    // Insert a copy of value before pos
    iterator insert(const_iterator pos, const_reference value) {
        return emplace(pos, value);
    }

    // Insert a moved value before pos
    iterator insert(const_iterator pos, T&& value) {
        return emplace(pos, NEX_STD move(value));
    }

    // Insert count copies of value before pos
    iterator insert(const_iterator pos, size_type count, const_reference value) {
        if (count == 0) return const_cast<iterator>(pos);
        size_type idx = static_cast<size_type>(pos - cbegin());
        NEX_ASSERT_MSG(idx <= size_, "Iterator out of range");
        size_type newSize = size_ + count;
        reserve(newSize);
        pointer base = storagePtr();
        // Shift elements right to make room
        for (size_type i = newSize - 1; i >= idx + count; --i) {
            if (i < size_) {
                ::new (static_cast<void*>(base + i)) T(NEX_STD move(base[i - count]));
                base[i - count].~T();
            } else {
                ::new (static_cast<void*>(base + i)) T(NEX_STD move(base[i - count]));
                base[i - count].~T();
            }
        }
        // Construct new elements
        for (size_type i = idx; i < idx + count; ++i) {
            ::new (static_cast<void*>(base + i)) T(value);
        }
        size_ = newSize;
        return begin() + static_cast<difference_type>(idx);
    }

    // Insert elements from initializer list before pos
    iterator insert(const_iterator pos, NEX_STD initializer_list<T> init) {
        size_type idx = static_cast<size_type>(pos - cbegin());
        for (const T& v : init) {
            insert(cbegin() + static_cast<difference_type>(idx), v);
            ++idx;
        }
        return begin() + static_cast<difference_type>(idx - init.size());
    }

    // Construct an element in place before pos
    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        size_type idx = static_cast<size_type>(pos - cbegin());
        NEX_ASSERT_MSG(idx <= size_, "Iterator out of range");
        if (size_ == capacity()) growTo(size_ + 1);
        pointer base = storagePtr();
        if (idx < size_) {
            // Shift elements right by one
            ::new (static_cast<void*>(base + size_)) T(NEX_STD move(base[size_ - 1]));
            for (size_type i = size_ - 1; i > idx; --i) {
                base[i] = NEX_STD move(base[i - 1]);
            }
            base[idx].~T();
        }
        ::new (static_cast<void*>(base + idx)) T(NEX_STD forward<Args>(args)...);
        ++size_;
        return begin() + static_cast<difference_type>(idx);
    }

    // Erase the element at pos
    iterator erase(const_iterator pos) noexcept(NEX_STD is_nothrow_move_assignable_v<T>) {
        return erase(pos, pos + 1);
    }

    // Erase elements in range [first, last)
    iterator erase(const_iterator first, const_iterator last) noexcept(
        NEX_STD is_nothrow_move_assignable_v<T>) {
        size_type idxFirst = static_cast<size_type>(first - cbegin());
        size_type idxLast = static_cast<size_type>(last - cbegin());
        NEX_ASSERT_MSG(idxFirst <= idxLast && idxLast <= size_, "Iterator range out of bounds");
        pointer base = storagePtr();
        size_type eraseCount = idxLast - idxFirst;
        if (eraseCount == 0) return begin() + static_cast<difference_type>(idxFirst);
        // Shift remaining elements left
        for (size_type i = idxFirst; i + eraseCount < size_; ++i) {
            base[i] = NEX_STD move(base[i + eraseCount]);
        }
        // Destroy the now-excess tail elements
        destroyRange(base + size_ - eraseCount, base + size_);
        size_ -= eraseCount;
        return begin() + static_cast<difference_type>(idxFirst);
    }

    // Remove all elements (does not release memory)
    void clear() noexcept {
        destroyRange(storagePtr(), storagePtr() + size_);
        size_ = 0;
    }

    // Swap contents with another SmallVec
    void swap(SmallVec& other) noexcept(NEX_STD is_nothrow_move_constructible_v<T>) {
        if (this == &other) return;
        if (!isInline() && !other.isInline()) {
            // Both on heap: swap pointers
            NEX_STD swap(heapData_, other.heapData_);
            NEX_STD swap(heapCapacity_, other.heapCapacity_);
            NEX_STD swap(size_, other.size_);
            return;
        }
        // At least one is inline — move element by element through a temporary
        SmallVec tmp(NEX_STD move(*this));
        *this = NEX_STD move(other);
        other = NEX_STD move(tmp);
    }

    ////// Conversion ----------------------------------

    // Convert to a heap-owning dynamic array (Vec<T>)
    Vec<T> toVec() const {
        return Vec<T>(begin(), end());
    }

    // Construct a SmallVec from a heap-owning dynamic array (Vec<T>)
    static SmallVec fromVec(const Vec<T>& vec) {
        SmallVec result;
        result.reserve(vec.size());
        for (const T& v : vec) {
            result.push_back(v);
        }
        return result;
    }

    ////// Comparison operators ----------------------------------

    // Equality
    friend bool operator==(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        if (lhs.size_ != rhs.size_) return false;
        for (size_type i = 0; i < lhs.size_; ++i) {
            if (!(lhs[i] == rhs[i])) return false;
        }
        return true;
    }

    // Inequality
    friend bool operator!=(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return !(lhs == rhs);
    }

    // Lexicographic less-than
    friend bool operator<(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return NEX_STD lexicographical_compare(lhs.begin(), lhs.end(),
                                               rhs.begin(), rhs.end());
    }

    // Lexicographic less-than-or-equal
    friend bool operator<=(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return !(rhs < lhs);
    }

    // Lexicographic greater-than
    friend bool operator>(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return rhs < lhs;
    }

    // Lexicographic greater-than-or-equal
    friend bool operator>=(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return !(lhs < rhs);
    }
};

NEX_CORE_NAMESPACE_END
