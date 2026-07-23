/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"
#include "nex/base/assert_crash.h"
#include "nex/base/buffer.h"
#include "nex/base/init.h"
#include "nex/base/traits.h"
#include "nex/base/iterator.h"
#include "nex/base/span.h"
#include "nex/core/containers/array.h"

NEX_NAMESPACE_BEGIN

/**
 * @class Vec
 * @brief A dynamically-sized vector that provides a contiguous block of memory for elements.
 * 
 * @details
 * The Vec class is a dynamic array that can grow as needed. It provides access to the underlying data, size,
 * and capacity, along with standard container operations such as iteration and element access. The size of the vector
 * can be changed at runtime, and it automatically manages memory allocation and deallocation.
 *
 * @tparam ElementType The type of elements stored in the vector.
 * @tparam Growth The growth policy for the dynamic buffer. Defaults to DoubleGrowth.
 */
template <
    typename ElementType,
    GrowthPolicy Growth = DoubleGrowth
>
class NEX_API Vec {
public:
    using value_type = ElementType;
    using size_type = usize;
    using difference_type = isize;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;
    using buffer_type = DynamicBuffer<value_type, Growth>;

private:
    // The underlying buffer that manages storage and element lifetime.
    buffer_type buffer_;

    // Returns the index of the element pointed to by pos, relative to the beginning of the vector.
    NEX_HIDDEN_FROM_ABI constexpr size_type indexOf(const_iterator pos) const noexcept {
        return static_cast<size_type>(pos - cbegin());
    }

public:
    // Default constructor initializes an empty Vec with inline storage.
    constexpr Vec() noexcept = default;

    // Constructs a Vec with count default-initialized elements.
    constexpr explicit Vec(size_type count)
        : buffer_(count) {}

    // Constructs a Vec with count copies of value.
    constexpr Vec(size_type count, const_reference value)
        : buffer_(count, value) {}

    // Constructs a Vec from an initializer list of values.
    constexpr Vec(InitList<value_type> init) {
        reserve(init.size());
        for (const value_type& value : init) {
            pushBack(value);
        }
    }

    // Constructs a Vec from a pair of iterators.
    template <typename InputIt,
              typename = EnableIf<
                  IsConvertibleV<IteratorValueType<InputIt>, value_type>>>
    constexpr Vec(InputIt first, InputIt last) {
        assign(first, last);
    }

    // Constructs a Vec from a Span of values.
    constexpr Vec(Span<value_type> span) {
        assign(span.begin(), span.end());
    }

    // Constructs a Vec from a Span of const values.
    constexpr Vec(Span<const value_type> span) {
        assign(span.begin(), span.end());
    }

    // Constructs a Vec from an Array
    template <usize Capacity>
    constexpr Vec(const Array<value_type, Capacity>& arr) {
        assign(arr.begin(), arr.end());
    }

    // Default copy semantics
    constexpr Vec(const Vec&) = default;
    constexpr Vec& operator=(const Vec&) = default;

    // Default move semantics
    constexpr Vec(Vec&&) noexcept(IsNothrowMoveConstructibleV<value_type>) = default;
    constexpr Vec& operator=(Vec&&) noexcept(IsNothrowMoveConstructibleV<value_type>) = default;

    // Assignment from initializer list 
    constexpr Vec& operator=(InitList<value_type> init) {
        assign(init);
        return *this;
    }

    // Assigns count copies of value to the Vec, replacing its current contents.
    constexpr void assign(size_type count, const_reference value) {
        clear();
        buffer_.resize(count, value);
    }

    // Assigns the elements in the range [first, last) to the Vec, replacing its current contents.
    template <typename InputIt,
              typename = EnableIf<
                  IsConvertibleV<IteratorValueType<InputIt>, value_type>>>
    constexpr void assign(InputIt first, InputIt last) {
        clear();
        for (; first != last; ++first) {
            pushBack(*first);
        }
    }

    // Assigns the elements in the initializer list to the Vec, replacing its current contents.
    constexpr void assign(InitList<value_type> init) {
        clear();
        reserve(init.size());
        for (const value_type& value : init) {
            pushBack(value);
        }
    }

    // Returns an iterator to the beginning of the Vec.
    constexpr iterator begin() noexcept { return buffer_.data(); }

    // Returns a const iterator to the beginning of the Vec.
    constexpr const_iterator begin() const noexcept { return buffer_.data(); }

    // Returns a const iterator to the beginning of the Vec.
    constexpr const_iterator cbegin() const noexcept { return buffer_.data(); }

    // Returns an iterator to the end of the Vec.
    constexpr iterator end() noexcept { return begin() + size(); }

    // Returns a const iterator to the end of the Vec.
    constexpr const_iterator end() const noexcept { return begin() + size(); }

    // Returns a const iterator to the end of the Vec.
    constexpr const_iterator cend() const noexcept { return cbegin() + size(); }

    // Returns a reverse iterator to the beginning of the reversed Vec.
    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    // Returns a const reverse iterator to the beginning of the reversed Vec.
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    // Returns a const reverse iterator to the beginning of the reversed Vec.
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    // Returns a reverse iterator to the end of the reversed Vec.
    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    // Returns a const reverse iterator to the end of the reversed Vec.
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // Returns a const reverse iterator to the end of the reversed Vec.
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    // Returns a reference to the element at the specified position, with bounds checking.
    constexpr reference at(size_type pos) {
        NEX_ASSERT_MSG(pos < size(), "Index out of range");
        return buffer_[pos];
    }

    // Returns a const reference to the element at the specified position, with bounds checking.
    constexpr const_reference at(size_type pos) const {
        NEX_ASSERT_MSG(pos < size(), "Index out of range");
        return buffer_[pos];
    }

    // Returns a reference to the element at the specified position, without bounds checking.
    NEX_NODISCARD constexpr reference operator[](size_type pos) noexcept { return buffer_[pos]; }

    // Returns a const reference to the element at the specified position, without bounds checking.
    NEX_NODISCARD constexpr const_reference operator[](size_type pos) const noexcept { return buffer_[pos]; }

    // Returns a reference to the first element.
    NEX_NODISCARD constexpr reference front() noexcept { return buffer_.front(); }

    // Returns a const reference to the first element.
    NEX_NODISCARD constexpr const_reference front() const noexcept { return buffer_.front(); }

    // Returns a reference to the last element.
    NEX_NODISCARD constexpr reference back() noexcept { return buffer_.back(); }

    // Returns a const reference to the last element.
    NEX_NODISCARD constexpr const_reference back() const noexcept { return buffer_.back(); }

    // Returns a pointer to the underlying array serving as element storage.
    NEX_NODISCARD constexpr pointer data() noexcept { return buffer_.data(); }

    // Returns a const pointer to the underlying array serving as element storage.
    NEX_NODISCARD constexpr const_pointer data() const noexcept { return buffer_.data(); }

    // Returns the number of elements in the Vec.
    NEX_NODISCARD constexpr size_type size() const noexcept { return buffer_.size(); }

    // Returns the number of elements in the Vec.
    NEX_NODISCARD constexpr size_type length() const noexcept { return size(); }

    // Returns true if the Vec contains no elements.
    NEX_NODISCARD constexpr bool empty() const noexcept { return buffer_.empty(); }

    // Returns the capacity of the Vec.
    NEX_NODISCARD constexpr size_type capacity() const noexcept { return buffer_.capacity(); }

    // Returns the maximum number of elements the Vec can hold.
    NEX_NODISCARD constexpr size_type maxSize() const noexcept { return buffer_type::maxSize(); }

    /**
     * @brief Reserves storage for at least newCapacity elements.
     * @note 
     * If newCapacity is greater than the current capacity, new storage is allocated and
     * existing elements are moved to the new storage.
     */
    constexpr void reserve(size_type newCapacity) { buffer_.reserve(newCapacity); }

    // Reduces the capacity of the Vec to fit its size, potentially freeing heap storage.
    constexpr void shrinkToFit() { buffer_.shrinkToFit(); }

    // Adds a new element to the end of the Vec, copying the value.
    constexpr void pushBack(const_reference value) { buffer_.append(&value, 1); }

    // Adds a new element to the end of the Vec, moving the value.
    constexpr void pushBack(value_type&& value) { buffer_.appendMove(&value, 1); }

    // Constructs a new element in place at the end of the Vec with the given arguments.
    template <typename... Args>
        requires(meta::IsConstructibleV<value_type, Args...>)
    constexpr reference emplaceBack(Args&&... args) {
        buffer_.appendConstruct(NEX_FORWARD<Args>(args)...);
        return back();
    }

    // Appends the elements in the range [first, last) to the end of the Vec.
    template <typename InputIt,
              typename = EnableIf<
                  IsConvertibleV<IteratorValueType<InputIt>, value_type>>>
    constexpr void append(InputIt first, InputIt last) {
        auto count = static_cast<size_type>(last - first);
        NEX_ASSERT_MSG(count <= maxSize() - size(), "Error: Appending would exceed maximum size");
        reserve(size() + count);
        for (; first != last; ++first) {
            pushBack(*first);
        }
    }

    // Appends the elements in the initializer list to the end of the Vec.
    constexpr void append(InitList<value_type> init) {
        append(init.begin(), init.end());
    }

    // Appends another Vec to the end of this Vec.
    constexpr void append(const Vec& other) {
        append(other.begin(), other.end());
    }

    // Appends a Span to the end of this Vec.
    constexpr void append(const Span<value_type>& span) {
        append(span.begin(), span.end());
    }

    // Appends a Span of const values to the end of this Vec.
    constexpr void append(const Span<const value_type>& span) {
        append(span.begin(), span.end());
    }

    // Removes the last element from the Vec.
    constexpr void popBack() noexcept(IsNothrowDestructibleV<value_type>) {
        NEX_ASSERT_MSG(!empty(), "popBack called on empty Vec");
        buffer_.remove(buffer_.size() - 1, 1);
    }

    /**
     * @brief Resizes the Vec to contain count elements.
     * @note
     * If the current size is less than count, additional default-inserted elements are appended.
     * If the current size is greater than count, the Vec is reduced to its first count elements.
     */
    constexpr void resize(size_type count) { buffer_.resize(count); }
    constexpr void resize(size_type count, const_reference value) { buffer_.resize(count, value); }

    // Inserts a copy of value before the element at pos.
    constexpr iterator insert(const_iterator pos, const_reference value) {
        return emplace(pos, value);
    }

    // Inserts a moved value before the element at pos.
    constexpr iterator insert(const_iterator pos, value_type&& value) {
        return emplace(pos, NEX_MOVE(value));
    }

    // Inserts count copies of value before the element at pos.
    constexpr iterator insert(const_iterator pos, size_type count, const_reference value) {
        const size_type index = indexOf(pos);
        NEX_ASSERT_MSG(index <= size(), "Iterator out of range");
        for (size_type offset = 0; offset < count; ++offset) {
            emplace(cbegin() + index + offset, value);
        }
        return begin() + index;
    }

    // Inserts elements from the range [first, last) before the element at pos.
    constexpr iterator insert(const_iterator pos, const_iterator first, const_iterator last) {
        const size_type index = indexOf(pos);
        NEX_ASSERT_MSG(index <= size(), "Iterator out of range");
        size_type current = index;
        for (const_iterator it = first; it != last; ++it) {
            emplace(cbegin() + current, *it);
            ++current;
        }
        return begin() + index;
    }

    // Inserts elements from the range [first, last) before the element at pos.
    constexpr iterator insert(const_iterator pos, InitList<value_type> init) {
        const size_type index = indexOf(pos);
        NEX_ASSERT_MSG(index <= size(), "Iterator out of range");
        size_type current = index;
        for (const value_type& value : init) {
            emplace(cbegin() + current, value);
            ++current;
        }
        return begin() + index;
    }

    // Inserts elements from the range [first, last) before the element at pos.
    template <typename... Args>
    constexpr iterator emplace(const_iterator pos, Args&&... args) {
        const size_type index = indexOf(pos);
        NEX_ASSERT_MSG(index <= size(), "Iterator out of range");

        if (index == size()) {
            emplaceBack(NEX_FORWARD<Args>(args)...);
            return end() - 1;
        }

        buffer_.append(&back(), 1);
        pointer values = data();
        for (size_type current = size() - 1; current > index; --current) {
            values[current] = NEX_MOVE(values[current - 1]);
        }
        NEX_DESTROY_AT(values + index);
        NEX_CONSTRUCT_AT(values + index, NEX_FORWARD<Args>(args)...);
        return begin() + index;
    }

    // Removes the element at pos, returning an iterator to the next element.
    constexpr iterator erase(const_iterator pos) noexcept(
        IsNothrowMoveAssignableV<value_type> && IsNothrowDestructibleV<value_type>) {
        return erase(pos, pos + 1);
    }

    // Removes the elements in the range [first, last), returning an iterator to the next element after the last removed.
    constexpr iterator erase(const_iterator first, const_iterator last) noexcept(
        IsNothrowMoveAssignableV<value_type> && IsNothrowDestructibleV<value_type>) {
        const size_type firstIndex = indexOf(first);
        const size_type lastIndex = indexOf(last);
        NEX_ASSERT_MSG(firstIndex <= lastIndex && lastIndex <= size(), "Iterator range out of bounds");

        const size_type erased = lastIndex - firstIndex;
        if (erased == 0) return begin() + firstIndex;

        pointer values = data();
        for (size_type current = firstIndex; current + erased < size(); ++current) {
            values[current] = NEX_MOVE(values[current + erased]);
        }
        buffer_.remove(size() - erased, erased);
        return begin() + firstIndex;
    }

    // Removes count elements starting from index, with bounds checking.
    constexpr iterator erase(size_type index, size_type count = 1) noexcept(
        IsNothrowMoveAssignableV<value_type> && IsNothrowDestructibleV<value_type>) {
        NEX_ASSERT_MSG(index + count <= size(), "Index and count out of bounds");
        return erase(cbegin() + index, cbegin() + index + count);
    }

    // Removes the first occurrence of value from the Vec, if it exists.
    constexpr bool remove(const_reference value) noexcept(
        IsNothrowMoveAssignableV<value_type> && IsNothrowDestructibleV<value_type>) {
        for (size_type index = 0; index < size(); ++index) {
            if (operator[](index) == value) {
                erase(cbegin() + index);
                return true;
            }
        }
        return false;
    }

    // Removes all occurrences of value from the Vec, if they exist.
    constexpr size_type removeAll(const_reference value) noexcept(
        IsNothrowMoveAssignableV<value_type> && IsNothrowDestructibleV<value_type>) {
        size_type removedCount = 0;
        for (size_type index = 0; index < size(); ++index) {
            if (operator[](index) == value) {
                erase(cbegin() + index);
                ++removedCount;
                --index; // Adjust index since the elements have shifted left
            }
        }
        return removedCount;
    }

    // Removes all elements from the Vec, leaving it with a size of 0.
    constexpr void clear() noexcept { buffer_.clear(); }

    // Swaps the contents of this Vec with another Vec.
    constexpr void swap(Vec& other) noexcept(noexcept(buffer_.swap(other.buffer_))) {
        buffer_.swap(other.buffer_);
    }

    // Finds the first occurrence of value in the Vec and returns an iterator to it.
    // If the value is not found, returns end().
    template <typename AnyValueType>
        requires (IsConvertibleV<AnyValueType, value_type>)
    constexpr iterator find(const AnyValueType& value) {
        for (auto it = begin(); it != end(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }

    // Determines whether the Vec contains the specified value.
    template <typename AnyValueType>
        requires (IsConvertibleV<AnyValueType, value_type>)
    NEX_NODISCARD constexpr bool contains(const AnyValueType& value) {
        return find(value) != end();
    }

    // Creates a Vec from an Array of values.
    template <usize Size>
    static constexpr Vec fromArray(const Array<value_type, Size>& array) {
        return Vec(array.begin(), array.end());
    }

    // Converts the Vec to an Array of values.
    template <usize Size>
    constexpr Array<value_type, Size> asArray() const {
        NEX_ASSERT_MSG(size() == Size, "Error: Size mismatch");
        Array<value_type, Size> array;
        for (size_type index = 0; index < Size; ++index) {
            array[index] = operator[](index);
        }
        return array;
    }

    // Converts the Vec to a span, providing a view of the elements without copying.
    constexpr Span<value_type> asSpan() noexcept {
        return Span<value_type>(data(), size());
    }

    // Converts the Vec to a const span, providing a view of the elements without copying.
    constexpr Span<const value_type> asSpan() const noexcept {
        return Span<const value_type>(data(), size());
    }

    // Comparison operators for Vec, allowing lexicographical comparison of elements.
    friend constexpr bool operator==(const Vec& lhs, const Vec& rhs) noexcept {
        if (lhs.size() != rhs.size()) return false;
        for (size_type index = 0; index < lhs.size(); ++index) {
            if (!(lhs[index] == rhs[index])) return false;
        }
        return true;
    }

    // Inequality operator for Vec, returns true if the two SmallVecs are not equal.
    friend constexpr bool operator!=(const Vec& lhs, const Vec& rhs) noexcept {
        return !(lhs == rhs);
    }

    // Less-than operator for Vec, performs lexicographical comparison of elements.
    friend constexpr bool operator<(const Vec& lhs, const Vec& rhs) noexcept {
        return NEX_STD lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    // Less-than-or-equal operator for Vec, returns true if lhs is less than or equal to rhs.
    friend constexpr bool operator<=(const Vec& lhs, const Vec& rhs) noexcept {
        return !(rhs < lhs);
    }

    // Greater-than operator for Vec, returns true if lhs is greater than rhs.
    friend constexpr bool operator>(const Vec& lhs, const Vec& rhs) noexcept {
        return rhs < lhs;
    }

    // Greater-than-or-equal operator for Vec, returns true if lhs is greater than or equal to rhs.
    friend constexpr bool operator>=(const Vec& lhs, const Vec& rhs) noexcept {
        return !(lhs < rhs);
    }
};

NEX_NAMESPACE_END