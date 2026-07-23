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

NEX_NAMESPACE_BEGIN

/**
 * @class Array
 * @brief A fixed-size array that provides a contiguous block of memory for elements.
 *
 * @details
 * The Array class is a lightweight container that manages a fixed-size array of elements.
 * It provides access to the underlying data, size, and capacity, along with standard
 * container operations such as iteration and element access. The size of the array is
 * determined at compile time and cannot be changed at runtime.
 *
 * @tparam ElementType The type of elements stored in the array.
 * @tparam Capacity The maximum number of elements the array can hold. Must be greater than zero.
 */
template <
    typename ElementType,
    usize Capacity = 8
> requires (Capacity > 0)
class NEX_API Array {
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
    using buffer_type = FixedBuffer<value_type, Capacity>;

private:
    // The underlying buffer that manages storage and element lifetime.
    buffer_type buffer_;

public:
    // Default constructor initializes an empty Array with inline storage.
    constexpr Array() noexcept = default;

    // Constructs an Array with count default-initialized elements.
    constexpr explicit Array(size_type count)
        : buffer_(count) {}

    // Constructs an Array with count copies of value.
    constexpr Array(size_type count, const_reference value)
        : buffer_(count, value) {}

    // Constructs an Array from a range specified by a pair of iterators.
    template <typename InputIt,
              typename = EnableIf<
                  IsConvertibleV<IteratorValueType<InputIt>, value_type>>>
    constexpr Array(InputIt first, InputIt last) {
        NEX_ASSERT_MSG(static_cast<size_type>(last - first) <= Capacity, "Error: Range exceeds array capacity");
        buffer_.append(first, last);
    }

    // Constructs an Array from an initializer list of values.
    constexpr Array(InitList<value_type> init) {
        NEX_ASSERT_MSG(init.size() <= Capacity, "Error: Initializer list exceeds array capacity");
        buffer_.append(init.begin(), init.end());
    }

    // Constructs an Array from a Span of values.
    constexpr Array(Span<value_type> span) {
        NEX_ASSERT_MSG(span.size() <= Capacity, "Error: Span exceeds array capacity");
        buffer_.append(span.begin(), span.end());
    }

    // Constructs an Array from a Span of const values.
    constexpr Array(Span<const value_type> span) {
        NEX_ASSERT_MSG(span.size() <= Capacity, "Error: Span exceeds array capacity");
        buffer_.append(span.begin(), span.end());
    }

    // Default copy semantics
    constexpr Array(const Array&) = default;
    constexpr Array& operator=(const Array&) = default;

    // Default move semantics
    constexpr Array(Array&&) noexcept(IsNothrowMoveConstructibleV<value_type>) = default;
    constexpr Array& operator=(Array&&) noexcept(IsNothrowMoveConstructibleV<value_type>) = default;

    // Assignment from initializer list
    constexpr Array& operator=(InitList<value_type> init) {
        NEX_ASSERT_MSG(init.size() <= Capacity, "Error: Initializer list exceeds array capacity");
        for (size_type i = 0; i < init.size(); ++i) {
            buffer_[i] = *(init.begin() + i);
        }
        return *this;
    }

    // Fills the entire array with the specified value.
    constexpr void fill(const_reference value) noexcept {
        for (size_type i = 0; i < Capacity; ++i) {
            buffer_[i] = value;
        }
    }

    // Swaps the contents of this array with another array of the same type and capacity.
    constexpr void swap(Array& other) noexcept(IsNothrowMoveConstructibleV<value_type>) {
        buffer_.swap(other.buffer_);
    }

    // Finds the first occurrence of value in the Array and returns an iterator to it.
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

    // Determines whether the Array contains the specified value.
    template <typename AnyValueType>
        requires (IsConvertibleV<AnyValueType, value_type>)
    NEX_NODISCARD constexpr bool contains(const AnyValueType& value) {
        return find(value) != end();
    }

    // Returns an iterator to the beginning of the Array.
    constexpr iterator begin() noexcept { return buffer_.data(); }

    // Returns a const iterator to the beginning of the Array.
    constexpr const_iterator begin() const noexcept { return buffer_.data(); }

    // Returns a const iterator to the beginning of the Array.
    constexpr const_iterator cbegin() const noexcept { return buffer_.data(); }

    // Returns an iterator to the end of the Array.
    constexpr iterator end() noexcept { return begin() + size(); }

    // Returns a const iterator to the end of the Array.
    constexpr const_iterator end() const noexcept { return begin() + size(); }

    // Returns a const iterator to the end of the Array.
    constexpr const_iterator cend() const noexcept { return cbegin() + size(); }

    // Returns a reverse iterator to the beginning of the reversed Array.
    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    // Returns a const reverse iterator to the beginning of the reversed Array.
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    // Returns a const reverse iterator to the beginning of the reversed Array.
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    // Returns a reverse iterator to the end of the reversed Array.
    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    // Returns a const reverse iterator to the end of the reversed Array.
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // Returns a const reverse iterator to the end of the reversed Array.
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    // Returns a reference to the element at the specified position, with bounds checking.
    NEX_NODISCARD constexpr reference at(size_type pos) {
        NEX_ASSERT_MSG(pos < size(), "Index out of range");
        return buffer_[pos];
    }

    // Returns a const reference to the element at the specified position, with bounds checking.
    NEX_NODISCARD constexpr const_reference at(size_type pos) const {
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

    // Returns the number of elements in the Array.
    NEX_NODISCARD constexpr size_type size() const noexcept { return buffer_.size(); }

    // Returns the number of elements in the Array.
    NEX_NODISCARD constexpr size_type length() const noexcept { return size(); }

    // Returns true if the Array contains no elements.
    NEX_NODISCARD constexpr bool empty() const noexcept { return buffer_.empty(); }

    // Returns the capacity of the Array.
    NEX_NODISCARD constexpr size_type capacity() const noexcept { return buffer_.capacity(); }

    // Returns the maximum number of elements the Array can hold.
    NEX_NODISCARD constexpr size_type maxSize() const noexcept { return buffer_type::maxSize(); }

    // Converts the Array to a span, providing a view of the elements without copying.
    NEX_NODISCARD constexpr Span<value_type> asSpan() noexcept {
        return Span<value_type>(data(), size());
    }

    // Converts the Array to a const span, providing a view of the elements without copying.
    NEX_NODISCARD constexpr Span<const value_type> asSpan() const noexcept {
        return Span<const value_type>(data(), size());
    }

    // Comparison operators for Array, allowing lexicographical comparison of elements.
    friend constexpr bool operator==(const Array& lhs, const Array& rhs) noexcept {
        for (size_type index = 0; index < lhs.size(); ++index) {
            if (!(lhs[index] == rhs[index])) return false;
        }
        return true;
    }

    // Inequality operator for Array, returns true if the two Arrays are not equal.
    friend constexpr bool operator!=(const Array& lhs, const Array& rhs) noexcept {
        return !(lhs == rhs);
    }

    // Less-than operator for Array, performs lexicographical comparison of elements.
    friend constexpr bool operator<(const Array& lhs, const Array& rhs) noexcept {
        return NEX_STD lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    // Less-than-or-equal operator for Array, returns true if lhs is less than or equal to rhs.
    friend constexpr bool operator<=(const Array& lhs, const Array& rhs) noexcept {
        return !(rhs < lhs);
    }

    // Greater-than operator for Array, returns true if lhs is greater than rhs.
    friend constexpr bool operator>(const Array& lhs, const Array& rhs) noexcept {
        return rhs < lhs;
    }

    // Greater-than-or-equal operator for Array, returns true if lhs is greater than or equal to rhs.
    friend constexpr bool operator>=(const Array& lhs, const Array& rhs) noexcept {
        return !(lhs < rhs);
    }
};

NEX_NAMESPACE_END