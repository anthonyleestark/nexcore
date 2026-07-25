/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/memory.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Assign elements from [first, last) to initialized storage at dest
 * @tparam ValueType The type of the elements to be assigned.
 * @param dest Pointer to the destination where elements will be assigned.
 * @param first Pointer to the first element in the source range.
 * @param last Pointer to one past the last element in the source range.
 */
template <typename ValueType>
constexpr void assign(ValueType* dest, const ValueType* first, const ValueType* last) noexcept
    requires(meta::IsTriviallyCopyAssignableV<ValueType>) {
    NEX_MEMCPY(dest, first, (last - first));
}

/**
 * @brief Assign elements from [first, last) to initialized storage at dest
 * @tparam ValueType The type of the elements to be assigned.
 * @param dest Pointer to the destination where elements will be assigned.
 * @param first Pointer to the first element in the source range.
 * @param last Pointer to one past the last element in the source range.
 */
template <typename ValueType>
constexpr void assign(ValueType* dest, const ValueType* first, const ValueType* last)
    noexcept(meta::IsNothrowCopyAssignableV<ValueType>) {
    ValueType* current = dest;
    for (; first != last; ++first, ++current) {
        *current = *first;
    }
}

/**
 * @brief Fill a range of memory with a specified value.
 * @tparam ValueType The type of the elements to be filled.
 * @tparam SizeType The type used for the count of elements.
 * @param dest Pointer to the destination where elements will be filled.
 * @param count The number of elements to fill.
 * @param value The value to fill the elements with.
 */
template <typename ValueType, typename SizeType = usize>
constexpr void fill(ValueType* dest, SizeType count, const ValueType& value) noexcept
    requires(meta::IsTriviallyCopyAssignableV<ValueType>) {
    NEX_MEMSET(dest, value, count * sizeof(ValueType));
}

/**
 * @brief Fill a range of memory with a specified value.
 * @tparam ValueType The type of the elements to be filled.
 * @tparam SizeType The type used for the count of elements.
 * @param dest Pointer to the destination where elements will be filled.
 * @param count The number of elements to fill.
 * @param value The value to fill the elements with.
 */
template <typename ValueType, typename SizeType = usize>
constexpr void fill(ValueType* dest, SizeType count, const ValueType& value)
    noexcept(meta::IsNothrowCopyAssignableV<ValueType>) {
    for (SizeType i = 0; i < count; ++i) {
        dest[i] = value;
    }
}

/**
 * @brief Copy elements from [first, last) to uninitialized storage at dest
 * @tparam ValueType The type of the elements to be copied.
 * @param dest Pointer to the destination where elements will be copied.
 * @param first Pointer to the first element in the source range.
 * @param last Pointer to one past the last element in the source range.
 */
template <typename ValueType>
constexpr void copy(ValueType* dest, const ValueType* first, const ValueType* last) noexcept
    requires(meta::IsTriviallyCopyConstructibleV<ValueType>) {
    NEX_MEMCPY(dest, first, (last - first));
}

/**
 * @brief Copy elements from [first, last) to uninitialized storage at dest
 * @tparam ValueType The type of the elements to be copied.
 * @param dest Pointer to the destination where elements will be copied.
 * @param first Pointer to the first element in the source range.
 * @param last Pointer to one past the last element in the source range.
 */
template <typename ValueType>
constexpr void copy(ValueType* dest, const ValueType* first, const ValueType* last)
    noexcept(meta::IsNothrowCopyConstructibleV<ValueType>) {
    ValueType* current = dest;
    for (; first != last; ++first, ++current) {
        NEX_CONSTRUCT_AT(current, *first);
    }
}

/**
 * @brief Relocate elements from [first, last) to uninitialized storage at dest
 * @tparam ValueType The type of the elements to be relocated.
 * @param dest Pointer to the destination where elements will be relocated.
 * @param first Pointer to the first element in the source range.
 * @param last Pointer to one past the last element in the source range.
 */
template <typename ValueType>
constexpr void relocate(ValueType* dest, ValueType* first, ValueType* last) noexcept
    requires(meta::IsTriviallyMoveConstructibleV<ValueType>) {
    NEX_MEMMOVE(dest, first, (last - first));
}

/**
 * @brief Relocate elements from [first, last) to uninitialized storage at dest
 * @tparam ValueType The type of the elements to be relocated.
 * @param dest Pointer to the destination where elements will be relocated.
 * @param first Pointer to the first element in the source range.
 * @param last Pointer to one past the last element in the source range.
 */
template <typename ValueType>
constexpr void relocate(ValueType* dest, ValueType* first, ValueType* last) 
    noexcept(meta::IsNothrowMoveConstructibleV<ValueType> && meta::IsNothrowDestructibleV<ValueType>) {
    ValueType* current = dest;
    ValueType* begin = first;
    for (; first != last; ++first, ++current) {
        NEX_CONSTRUCT_AT(current, NEX_MOVE(*first));
    }
    destroy(begin, last);
}

/**
 * @brief Destroy elements in range [first, last)
 * @tparam ValueType The type of the elements to be destroyed.
 * @param first Pointer to the first element in the range.
 * @param last Pointer to one past the last element in the range.
 */
template <typename ValueType>
constexpr void destroy(ValueType* first, ValueType* last) noexcept
    requires(meta::IsTriviallyDestructibleV<ValueType>) {
    // No-op for trivially destructible types
}

/**
 * @brief Destroy elements in range [first, last)
 * @tparam ValueType The type of the elements to be destroyed.
 * @param first Pointer to the first element in the range.
 * @param last Pointer to one past the last element in the range.
 */
template <typename ValueType>
constexpr void destroy(ValueType* first, ValueType* last)
    noexcept(meta::IsNothrowDestructibleV<ValueType>)
    requires(!meta::IsTriviallyDestructibleV<ValueType>) {
    for (; first != last; ++first) {
        first->~ValueType();
    }
}

// ======================================================================================
// Define macros for memory manipulation for ranges of objects
// ======================================================================================

// Assign elements from [first, last) to initialized storage at dest
#define NEX_ASSIGN_RANGE(dest, first, last) \
    NEX_PREPEND_NAMESPACE(assign(dest, first, last))

// Fill a range of memory with a specified value.
#define NEX_FILL_RANGE(dest, count, value) \
    NEX_PREPEND_NAMESPACE(fill(dest, count, value))

// Copy elements from [first, last) to uninitialized storage at dest
#define NEX_COPY_RANGE(dest, first, last) \
    NEX_PREPEND_NAMESPACE(copy(dest, first, last))

// Relocate elements from [first, last) to uninitialized storage at dest
#define NEX_RELOCATE_RANGE(dest, first, last) \
    NEX_PREPEND_NAMESPACE(relocate(dest, first, last))

// Destroy elements in range [first, last)
#define NEX_DESTROY_RANGE(first, last) \
    NEX_PREPEND_NAMESPACE(destroy(first, last))

NEX_NAMESPACE_END
