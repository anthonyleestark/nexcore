/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <algorithm>

#include "nex/base/namespace.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"
#include "nex/base/assert_crash.h"
#include "nex/base/buffer.h"
#include "nex/base/init.h"
#include "nex/base/traits.h"
#include "nex/base/iterator.h"
#include "nex/core/containers/vector.h"

NEX_NAMESPACE_BEGIN

/**
 * @class SmallVec
 * @brief A small, inline-optimized dynamic vector that can grow as needed.
 *
 * @details
 * SmallVec provides the familiar vector interface while delegating inline storage,
 * heap growth, and element lifetime management to SmallBuffer. It stores up to
 * InlineCapacity elements inline and uses GrowthPolicy::Double after that.
 *
 * @tparam ElementType The element type.
 * @tparam InlineCapacity The number of elements stored inline. Must be greater than zero.
 * @tparam Growth The growth policy to use when resizing the vector. Default is DoubleGrowth.
 */
template <
    typename ElementType,
    usize InlineCapacity = 8,
    GrowthPolicy Growth = DoubleGrowth
> requires (InlineCapacity > 0)
class NEX_API SmallVec {
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
    using buffer_type = SmallBuffer<value_type, InlineCapacity, Growth>;

    // Inline buffer capacity constant
    static constexpr size_type inlineCapacity = InlineCapacity;

private:
    // The underlying buffer that manages storage and element lifetime.
    buffer_type buffer_;

    // Returns the index of the element pointed to by pos, relative to the beginning of the vector.
    NEX_HIDDEN_FROM_ABI constexpr size_type indexOf(const_iterator pos) const noexcept {
        return static_cast<size_type>(pos - cbegin());
    }

public:
    // Default constructor initializes an empty SmallVec with inline storage.
    constexpr SmallVec() noexcept = default;

    // Constructs a SmallVec with count default-initialized elements.
    constexpr explicit SmallVec(size_type count)
        : buffer_(count) {}

    // Constructs a SmallVec with count copies of value.
    constexpr SmallVec(size_type count, const_reference value)
        : buffer_(count, value) {}

    // Constructs a SmallVec from an initializer list of values.
    constexpr SmallVec(InitList<value_type> init) {
        reserve(init.size());
        for (const value_type& value : init) {
            pushBack(value);
        }
    }

    // Constructs a SmallVec from a pair of iterators.
    template <typename InputIt,
              typename = EnableIf<
                  IsConvertibleV<IteratorValueType<InputIt>, value_type>>>
    constexpr SmallVec(InputIt first, InputIt last) {
        assign(first, last);
    }

    // Default copy semantics
    constexpr SmallVec(const SmallVec&) = default;
    constexpr SmallVec(SmallVec&&) noexcept(IsNothrowMoveConstructibleV<value_type>) = default;

    // Default move semantics
    constexpr SmallVec& operator=(const SmallVec&) = default;
    constexpr SmallVec& operator=(SmallVec&&) noexcept(IsNothrowMoveConstructibleV<value_type>) = default;

    // Assignment from initializer list 
    constexpr SmallVec& operator=(InitList<value_type> init) {
        assign(init);
        return *this;
    }

    // Assigns count copies of value to the SmallVec, replacing its current contents.
    constexpr void assign(size_type count, const_reference value) {
        clear();
        buffer_.resize(count, value);
    }

    // Assigns the elements in the range [first, last) to the SmallVec, replacing its current contents.
    template <typename InputIt,
              typename = EnableIf<
                  IsConvertibleV<IteratorValueType<InputIt>, value_type>>>
    constexpr void assign(InputIt first, InputIt last) {
        clear();
        for (; first != last; ++first) {
            pushBack(*first);
        }
    }

    // Assigns the elements in the initializer list to the SmallVec, replacing its current contents.
    constexpr void assign(InitList<value_type> init) {
        clear();
        reserve(init.size());
        for (const value_type& value : init) {
            pushBack(value);
        }
    }

    // Returns an iterator to the beginning of the SmallVec.
    constexpr iterator begin() noexcept { return buffer_.data(); }
    // Returns a const iterator to the beginning of the SmallVec.
    constexpr const_iterator begin() const noexcept { return buffer_.data(); }
    // Returns a const iterator to the beginning of the SmallVec.
    constexpr const_iterator cbegin() const noexcept { return buffer_.data(); }

    // Returns an iterator to the end of the SmallVec.
    constexpr iterator end() noexcept { return begin() + size(); }
    // Returns a const iterator to the end of the SmallVec.
    constexpr const_iterator end() const noexcept { return begin() + size(); }
    // Returns a const iterator to the end of the SmallVec.
    constexpr const_iterator cend() const noexcept { return cbegin() + size(); }

    // Returns a reverse iterator to the beginning of the reversed SmallVec.
    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    // Returns a const reverse iterator to the beginning of the reversed SmallVec.
    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    // Returns a const reverse iterator to the beginning of the reversed SmallVec.
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    // Returns a reverse iterator to the end of the reversed SmallVec.
    constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    // Returns a const reverse iterator to the end of the reversed SmallVec.
    constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    // Returns a const reverse iterator to the end of the reversed SmallVec.
    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

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
    constexpr reference operator[](size_type pos) noexcept { return buffer_[pos]; }
    // Returns a const reference to the element at the specified position, without bounds checking.
    constexpr const_reference operator[](size_type pos) const noexcept { return buffer_[pos]; }

    // Returns a reference to the first element.
    constexpr reference front() noexcept { return buffer_.front(); }
    // Returns a const reference to the first element.
    constexpr const_reference front() const noexcept { return buffer_.front(); }

    // Returns a reference to the last element.
    constexpr reference back() noexcept { return buffer_.back(); }
    // Returns a const reference to the last element.
    constexpr const_reference back() const noexcept { return buffer_.back(); }

    // Returns a pointer to the underlying array serving as element storage.
    constexpr pointer data() noexcept { return buffer_.data(); }
    // Returns a const pointer to the underlying array serving as element storage.
    constexpr const_pointer data() const noexcept { return buffer_.data(); }

    // Returns the number of elements in the SmallVec.
    constexpr size_type size() const noexcept { return buffer_.size(); }
    // Returns the number of elements in the SmallVec.
    constexpr size_type length() const noexcept { return size(); }

    // Returns true if the SmallVec contains no elements.
    constexpr bool empty() const noexcept { return buffer_.empty(); }

    // Returns the capacity of the SmallVec.
    constexpr size_type capacity() const noexcept { return buffer_.capacity(); }

    // Returns the maximum number of elements the SmallVec can hold.
    constexpr size_type maxSize() const noexcept { return buffer_type::maxSize(); }

    // Returns true if the SmallVec is using inline storage, false if it has allocated heap storage.
    constexpr bool usingInlineStorage() const noexcept { return buffer_.usingInlineStorage(); }

    /**
     * @brief Reserves storage for at least newCapacity elements.
     * @note 
     * If newCapacity is greater than the current capacity, new storage is allocated and
     * existing elements are moved to the new storage.
     */
    constexpr void reserve(size_type newCapacity) { buffer_.reserve(newCapacity); }

    // Reduces the capacity of the SmallVec to fit its size, potentially freeing heap storage.
    constexpr void shrinkToFit() { buffer_.shrinkToFit(); }

    // Adds a new element to the end of the SmallVec, copying the value.
    constexpr void pushBack(const_reference value) { buffer_.append(&value, 1); }

    // Adds a new element to the end of the SmallVec, moving the value.
    constexpr void pushBack(value_type&& value) { buffer_.appendMove(&value, 1); }

    // Constructs a new element in place at the end of the SmallVec with the given arguments.
    template <typename... Args>
        requires(meta::IsConstructibleV<value_type, Args...>)
    constexpr reference emplaceBack(Args&&... args) {
        buffer_.appendConstruct(NEX_FORWARD<Args>(args)...);
        return back();
    }

    // Removes the last element from the SmallVec.
    constexpr void popBack() noexcept(IsNothrowDestructibleV<value_type>) {
        NEX_ASSERT_MSG(!empty(), "popBack called on empty SmallVec");
        buffer_.remove(buffer_.size() - 1, 1);
    }

    /**
     * @brief Resizes the SmallVec to contain count elements.
     * @note
     * If the current size is less than count, additional default-inserted elements are appended.
     * If the current size is greater than count, the SmallVec is reduced to its first count elements.
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

    // Removes all elements from the SmallVec, leaving it with a size of 0.
    constexpr void clear() noexcept { buffer_.clear(); }

    // Swaps the contents of this SmallVec with another SmallVec.
    constexpr void swap(SmallVec& other) noexcept(noexcept(buffer_.swap(other.buffer_))) {
        buffer_.swap(other.buffer_);
    }

    // Converts the SmallVec to a standard vector.
    constexpr Vec<value_type> toVec() const {
        return Vec<value_type>(begin(), end());
    }

    // Creates a SmallVec from a standard vector.
    static constexpr SmallVec fromVec(const Vec<value_type>& vec) {
        return SmallVec(vec.begin(), vec.end());
    }

    // Comparison operators for SmallVec, allowing lexicographical comparison of elements.
    friend constexpr bool operator==(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        if (lhs.size() != rhs.size()) return false;
        for (size_type index = 0; index < lhs.size(); ++index) {
            if (!(lhs[index] == rhs[index])) return false;
        }
        return true;
    }

    // Inequality operator for SmallVec, returns true if the two SmallVecs are not equal.
    friend constexpr bool operator!=(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return !(lhs == rhs);
    }

    // Less-than operator for SmallVec, performs lexicographical comparison of elements.
    friend constexpr bool operator<(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return NEX_STD lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    // Less-than-or-equal operator for SmallVec, returns true if lhs is less than or equal to rhs.
    friend constexpr bool operator<=(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return !(rhs < lhs);
    }

    // Greater-than operator for SmallVec, returns true if lhs is greater than rhs.
    friend constexpr bool operator>(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return rhs < lhs;
    }

    // Greater-than-or-equal operator for SmallVec, returns true if lhs is greater than or equal to rhs.
    friend constexpr bool operator>=(const SmallVec& lhs, const SmallVec& rhs) noexcept {
        return !(lhs < rhs);
    }
};

NEX_NAMESPACE_END
