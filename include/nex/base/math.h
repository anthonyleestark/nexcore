/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"
#include "nex/base/memory.h"
#include "nex/base/invoke.h"
#include "nex/base/utility.h"
#include "nex/base/type_concepts.h"
#include "nex/base/iterator.h"
#include "nex/base/ranges.h"
#include "nex/base/init.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// Min / max functions
// =================================================================================

/**
 * @brief Returns the minimum of two values.
 * @tparam Type The type of the values to compare.
 * @param a The first value.
 * @param b The second value.
 * @return The minimum of the two values.
 */
template <typename Type>
    requires(LessThanComparable<Type, Type>)
NEX_NODISCARD constexpr const Type& minOf(const Type& a NEX_LIFETIMEBOUND, const Type& b NEX_LIFETIMEBOUND)
    noexcept(noexcept(meta::_minOf(a, b))) {
    return meta::_minOf(a, b);
}

/**
 * @brief Returns the maximum of two values.
 * @tparam Type The type of the values to compare.
 * @param a The first value.
 * @param b The second value.
 * @return The maximum of the two values.
 */
template <typename Type>
    requires(GreaterThanComparable<Type, Type>)
NEX_NODISCARD constexpr const Type& maxOf(const Type& a NEX_LIFETIMEBOUND, const Type& b NEX_LIFETIMEBOUND)
    noexcept(noexcept(meta::_maxOf(a, b))) {
    return meta::_maxOf(a, b);
}

/**
 * @brief Returns the minimum element in a range [first, last).
 * @tparam Iter The type of the iterator.
 * @tparam Sent The type of the sentinel.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @return An iterator to the minimum element in the range.
 */
template <InputIterator Iter, typename Sent>
    requires(SentinelFor<Sent, Iter> &&
             LessThanComparable<IteratorValueType<Iter>, IteratorValueType<Iter>>)
NEX_NODISCARD constexpr Iter minElement(Iter first, Sent last) {
    if (first == last) {
        return first;
    }

    Iter minElem = first;
    while (++first != last) {
        minElem = meta::_minOf(*minElem, *first) == *minElem ? minElem : first;
    }
    return minElem;
}

/**
 * @brief Returns the minimum element in a range.
 * @tparam Range The type of the range-like object.
 * @param range The range-like object to search for the minimum element.
 * @return An iterator to the minimum element in the range.
 */
template <ranges::Range Range>
    requires(LessThanComparable<ranges::RangeValueT<Range>, ranges::RangeValueT<Range>>)
NEX_NODISCARD constexpr auto minElement(Range&& range)
    -> decltype(ranges::begin(range)) {
    return minElement(ranges::begin(range), ranges::end(range));
}

/**
 * @brief Returns the minimum element in an initializer list.
 * @tparam ElementType The type of the elements in the initializer list.
 * @param list The initializer list to search for the minimum element.
 * @return A reference to the minimum element in the initializer list.
 */
template <typename ElementType>
    requires(LessThanComparable<ElementType, ElementType>)
NEX_NODISCARD constexpr const ElementType& minOf(InitList<ElementType> list)
    noexcept(noexcept(meta::_minOf(declval<const ElementType&>(), declval<const ElementType&>()))) {
    NEX_ASSERT_MSG(list.size() > 0, "Error: Cannot find the minimum of an empty list.");
    return *minElement(list.begin(), list.end());
}

/**
 * @brief Returns the maximum element in a range [first, last).
 * @tparam Iter The type of the iterator.
 * @tparam Sent The type of the sentinel.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @return An iterator to the maximum element in the range.
 */
template <InputIterator Iter, typename Sent>
    requires(SentinelFor<Sent, Iter> &&
             GreaterThanComparable<IteratorValueType<Iter>, IteratorValueType<Iter>>)
NEX_NODISCARD constexpr Iter maxElement(Iter first, Sent last) {
    if (first == last) {
        return first;
    }

    Iter maxElem = first;
    while (++first != last) {
        maxElem = meta::_maxOf(*maxElem, *first) == *maxElem ? maxElem : first;
    }
    return maxElem;
}

/**
 * @brief Returns the maximum element in a range.
 * @tparam Range The type of the range-like object.
 * @param range The range-like object to search for the maximum element.
 * @return An iterator to the maximum element in the range.
 */
template <ranges::Range Range>
    requires(GreaterThanComparable<ranges::RangeValueT<Range>, ranges::RangeValueT<Range>>)
NEX_NODISCARD constexpr auto maxElement(Range&& range)
    -> decltype(ranges::begin(range)) {
    return maxElement(ranges::begin(range), ranges::end(range));
}

/**
 * @brief Returns the maximum element in an initializer list.
 * @tparam ElementType The type of the elements in the initializer list.
 * @param list The initializer list to search for the maximum element.
 * @return A reference to the maximum element in the initializer list.
 */
template <typename ElementType>
    requires(GreaterThanComparable<ElementType, ElementType>)
NEX_NODISCARD constexpr const ElementType& maxOf(InitList<ElementType> list)
    noexcept(noexcept(meta::_maxOf(declval<const ElementType&>(), declval<const ElementType&>()))) {
    NEX_ASSERT_MSG(list.size() > 0, "Error: Cannot find the maximum of an empty list.");
    return *maxElement(list.begin(), list.end());
}

/**
 * @brief Returns a pair containing the minimum and maximum of two values.
 * @tparam Type The type of the values to compare.
 * @param a The first value.
 * @param b The second value.
 * @return A pair containing the minimum and maximum of the two values.
 */
template <typename Type>
    requires(LessThanComparable<Type, Type>)
NEX_NODISCARD constexpr
Pair<const Type&, const Type&> minMaxOf(const Type& a NEX_LIFETIMEBOUND, const Type& b NEX_LIFETIMEBOUND)
    noexcept(noexcept(meta::_minOf(a, b)) && noexcept(meta::_maxOf(a, b))) {
    return Pair<const Type&, const Type&>{meta::_minOf(a, b), meta::_maxOf(a, b)};
}

/**
 * @brief Returns a pair containing the minimum and maximum elements in a range [first, last).
 * @tparam Iter The type of the iterator.
 * @tparam Sent The type of the sentinel.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @return A pair containing iterators to the minimum and maximum elements in the range.
 */
template <InputIterator Iter, typename Sent>
    requires(SentinelFor<Sent, Iter>)
NEX_NODISCARD constexpr Pair<Iter, Iter> minMaxElement(Iter first, Sent last) {
    if (first == last) {
        return Pair<Iter, Iter>{first, first};
    }

    Iter minElem = first;
    Iter maxElem = first;
    while (++first != last) {
        minElem = meta::_minOf(*minElem, *first) == *minElem ? minElem : first;
        maxElem = meta::_maxOf(*maxElem, *first) == *maxElem ? maxElem : first;
    }
    return Pair<Iter, Iter>{minElem, maxElem};
}

/**
 * @brief Returns a pair containing the minimum and maximum elements in a range.
 * @tparam Range The type of the range-like object.
 * @param range The range-like object to search for the minimum and maximum elements.
 * @return A pair containing iterators to the minimum and maximum elements in the range.
 */
template <ranges::Range Range>
NEX_NODISCARD constexpr auto minMaxElement(Range&& range)
    -> Pair<decltype(ranges::begin(range)), decltype(ranges::begin(range))> {
    return minMaxElement(ranges::begin(range), ranges::end(range));
}

// =================================================================================
// Clamp function
// =================================================================================

/**
 * @brief Clamps a value between a low and high bound.
 * @tparam Type The type of the value to clamp.
 * @param value The value to clamp.
 * @param low The lower bound.
 * @param high The upper bound.
 * @return The clamped value.
 */
template <typename Type>
    requires(LessThanComparable<Type, Type>)
NEX_NODISCARD constexpr
const Type& clamp(const Type& value NEX_LIFETIMEBOUND,
                  const Type& low NEX_LIFETIMEBOUND,
                  const Type& high NEX_LIFETIMEBOUND)
    noexcept(noexcept(meta::_minOf(low, high))) {
    NEX_ASSERT_MSG(low <= high, "Error: Invalid range for clamp. Low must be less than or equal to high.");
    return meta::_minOf(meta::_maxOf(value, low), high);
}

NEX_NAMESPACE_END
