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
// Non-modifying sequence operations
// =================================================================================

/**
 * @brief Determines if all elements in the range [first, last) satisfy the predicate.
 * @tparam Iter The type of the iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param pred The predicate function to apply to each element.
 * @return true if all elements satisfy the predicate, false otherwise.
 */
template <InputIterator Iter, class Predicate>
    requires(Invocable<Predicate, IterRefOf<Iter>>)
NEX_NODISCARD constexpr bool allOf(Iter first, Iter last, Predicate pred)
    noexcept(noexcept(NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first)))) {
    for (; first != last; ++first) {
        if (!NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Determines if all elements in the range satisfy the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to check.
 * @param pred The predicate function to apply to each element.
 * @return true if all elements satisfy the predicate, false otherwise.
 */
template <ranges::Range Range, class Predicate>
    requires(Invocable<Predicate, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr bool allOf(Range&& range, Predicate pred)
    noexcept(noexcept(allOf(ranges::begin(range), ranges::end(range), pred))) {
    return allOf(ranges::begin(range), ranges::end(range), pred);
}

/**
 * @brief Determines if any element in the range [first, last) satisfies the predicate.
 * @tparam Iter The type of the iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param pred The predicate function to apply to each element.
 * @return true if any element satisfies the predicate, false otherwise.
 */
template <InputIterator Iter, class Predicate>
    requires(Invocable<Predicate, IterRefOf<Iter>>)
NEX_NODISCARD constexpr bool anyOf(Iter first, Iter last, Predicate pred)
    noexcept(noexcept(NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first)))) {
    for (; first != last; ++first) {
        if (NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Determines if any element in the range satisfies the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to check.
 * @param pred The predicate function to apply to each element.
 * @return true if any element satisfies the predicate, false otherwise.
 */
template <ranges::Range Range, class Predicate>
    requires(Invocable<Predicate, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr bool anyOf(Range&& range, Predicate pred)
    noexcept(noexcept(anyOf(ranges::begin(range), ranges::end(range), pred))) {
    return anyOf(ranges::begin(range), ranges::end(range), pred);
}

/**
 * @brief Determines if no elements in the range [first, last) satisfy the predicate.
 * @tparam Iter The type of the iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param pred The predicate function to apply to each element.
 * @return true if no elements satisfy the predicate, false otherwise.
 */
template <InputIterator Iter, class Predicate>
    requires(Invocable<Predicate, IterRefOf<Iter>>)
NEX_NODISCARD constexpr bool noneOf(Iter first, Iter last, Predicate pred)
    noexcept(noexcept(NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first)))) {
    for (; first != last; ++first) {
        if (NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Determines if no elements in the range satisfy the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to check.
 * @param pred The predicate function to apply to each element.
 * @return true if no elements satisfy the predicate, false otherwise.
 */
template <ranges::Range Range, class Predicate>
    requires(Invocable<Predicate, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr bool noneOf(Range&& range, Predicate pred)
    noexcept(noexcept(noneOf(ranges::begin(range), ranges::end(range), pred))) {
    return noneOf(ranges::begin(range), ranges::end(range), pred);
}

/**
 * @brief Finds the first occurrence of a value in the range [first, last).
 * @tparam Iter The type of the iterator.
 * @tparam Type The type of the value to find.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param value The value to find.
 * @return An iterator to the first occurrence of the value, or last if not found.
 */
template <InputIterator Iter, class Type>
    requires(EqualityComparableWith<IterRefOf<Iter>, const Type&>)
NEX_NODISCARD constexpr Iter find(Iter first, Iter last, const Type& value)
    noexcept(noexcept(*first == value)) {
    for (; first != last; ++first) {
        if (*first == value) {
            return first;
        }
    }
    return last;
}

/**
 * @brief Finds the first occurrence of a value in the range.
 * @tparam Range The type of the range-like object.
 * @tparam Type The type of the value to find.
 * @param range The range-like object to search.
 * @param value The value to find.
 * @return An iterator to the first occurrence of the value, or end of the range if not found.
 */
template <ranges::Range Range, class Type>
    requires(EqualityComparableWith<ranges::RangeReferenceT<Range>, const Type&>)
NEX_NODISCARD constexpr auto find(Range&& range, const Type& value)
    noexcept(noexcept(find(ranges::begin(range), ranges::end(range), value)))
    -> decltype(ranges::begin(range)) {
    return find(ranges::begin(range), ranges::end(range), value);
}

/**
 * @brief Finds the first element in the range [first, last) that satisfies the predicate.
 * @tparam Iter The type of the iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param pred The predicate function to apply to each element.
 * @return An iterator to the first element that satisfies the predicate, or last if not found.
 */
template <InputIterator Iter, class Predicate>
    requires(Invocable<Predicate, IterRefOf<Iter>>)
NEX_NODISCARD constexpr Iter findIf(Iter first, Iter last, Predicate pred)
    noexcept(noexcept(NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first)))) {
    for (; first != last; ++first) {
        if (NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            return first;
        }
    }
    return last;
}

/**
 * @brief Finds the first element in the range that satisfies the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to search.
 * @param pred The predicate function to apply to each element.
 * @return An iterator to the first element that satisfies the predicate, or end of the range if not found.
 */
template <ranges::Range Range, class Predicate>
    requires(Invocable<Predicate, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr auto findIf(Range&& range, Predicate pred)
    noexcept(noexcept(findIf(ranges::begin(range), ranges::end(range), pred)))
    -> decltype(ranges::begin(range)) {
    return findIf(ranges::begin(range), ranges::end(range), pred);
}

/**
 * @brief Finds the first element in the range [first, last) that does not satisfy the predicate.
 * @tparam Iter The type of the iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param pred The predicate function to apply to each element.
 * @return An iterator to the first element that does not satisfy the predicate, or last if all elements satisfy it.
 */
template <InputIterator Iter, class Predicate>
    requires(Invocable<Predicate, IterRefOf<Iter>>)
NEX_NODISCARD constexpr Iter findIfNot(Iter first, Iter last, Predicate pred)
    noexcept(noexcept(NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first)))) {
    for (; first != last; ++first) {
        if (!NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            return first;
        }
    }
    return last;
}

/**
 * @brief Finds the first element in the range that does not satisfy the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to search.
 * @param pred The predicate function to apply to each element.
 * @return An iterator to the first element that does not satisfy the predicate, or end of the range if all elements satisfy it.
 */
template <ranges::Range Range, class Predicate>
    requires(Invocable<Predicate, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr auto findIfNot(Range&& range, Predicate pred)
    noexcept(noexcept(findIfNot(ranges::begin(range), ranges::end(range), pred)))
    -> decltype(ranges::begin(range)) {
    return findIfNot(ranges::begin(range), ranges::end(range), pred);
}

/**
 * @brief Determines if the range [first, last) contains the specified value.
 * @tparam Iter The type of the iterator.
 * @tparam Type The type of the value to check for.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param value The value to check for.
 * @return true if the value is found in the range, false otherwise.
 */
template <InputIterator Iter, class Type>
    requires(EqualityComparableWith<IterRefOf<Iter>, const Type&>)
NEX_NODISCARD constexpr bool contains(Iter first, Iter last, const Type& value)
    noexcept(noexcept(find(first, last, value))) {
    return find(first, last, value) != last;
}

/**
 * @brief Determines if the range contains the specified value.
 * @tparam Range The type of the range-like object.
 * @tparam Type The type of the value to check for.
 * @param range The range-like object to check.
 * @param value The value to check for.
 * @return true if the value is found in the range, false otherwise.
 */
template <ranges::Range Range, class Type>
    requires(EqualityComparableWith<ranges::RangeReferenceT<Range>, const Type&>)
NEX_NODISCARD constexpr bool contains(Range&& range, const Type& value)
    noexcept(noexcept(contains(ranges::begin(range), ranges::end(range), value))) {
    return contains(ranges::begin(range), ranges::end(range), value);
}

/**
 * @brief Determines if the range [first, last) contains an element that satisfies the predicate.
 * @tparam Iter The type of the iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param pred The predicate function to apply to each element.
 * @return true if an element satisfying the predicate is found in the range, false otherwise.
 */
template <InputIterator Iter, class Predicate>
    requires(Invocable<Predicate, IterRefOf<Iter>>)
NEX_NODISCARD constexpr bool containsIf(Iter first, Iter last, Predicate pred)
    noexcept(noexcept(findIf(first, last, pred))) {
    return findIf(first, last, pred) != last;
}

/**
 * @brief Determines if the range contains an element that satisfies the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to check.
 * @param pred The predicate function to apply to each element.
 * @return true if an element satisfying the predicate is found in the range, false otherwise.
 */
template <ranges::Range Range, class Predicate>
    requires(Invocable<Predicate, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr bool containsIf(Range&& range, Predicate pred)
    noexcept(noexcept(containsIf(ranges::begin(range), ranges::end(range), pred))) {
    return containsIf(ranges::begin(range), ranges::end(range), pred);
}

// findLast(...)
// findLastIf(...)
// findLastIfNot(...)

// =================================================================================
// Counting functions
// =================================================================================

/**
 * @brief Counts the number of occurrences of a value in the range [first, last).
 * @tparam Iter The type of the iterator.
 * @tparam Type The type of the value to count.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param value The value to count.
 * @return The number of occurrences of the value in the range.
 */
template <InputIterator Iter, class Type>
    requires(EqualityComparableWith<IterRefOf<Iter>, const Type&>)
NEX_NODISCARD constexpr usize count(Iter first, Iter last, const Type& value)
    noexcept(noexcept(*first == value)) {
    usize count = 0;
    for (; first != last; ++first) {
        if (*first == value) {
            ++count;
        }
    }
    return count;
}

/**
 * @brief Counts the number of occurrences of a value in the range.
 * @tparam Range The type of the range-like object.
 * @tparam Type The type of the value to count.
 * @param range The range-like object to search.
 * @param value The value to count.
 * @return The number of occurrences of the value in the range.
 */
template <ranges::Range Range, class Type>
    requires(EqualityComparableWith<ranges::RangeReferenceT<Range>, const Type&>)
NEX_NODISCARD constexpr usize count(Range&& range, const Type& value)
    noexcept(noexcept(count(ranges::begin(range), ranges::end(range), value))) {
    return count(ranges::begin(range), ranges::end(range), value);
}

/**
 * @brief Counts the number of elements in the range [first, last) that satisfy the predicate.
 * @tparam Iter The type of the iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param pred The predicate function to apply to each element.
 * @return The number of elements that satisfy the predicate in the range.
 */
template <InputIterator Iter, class Predicate>
    requires(Invocable<Predicate, IterRefOf<Iter>>)
NEX_NODISCARD constexpr usize countIf(Iter first, Iter last, Predicate pred)
    noexcept(noexcept(NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first)))) {
    usize count = 0;
    for (; first != last; ++first) {
        if (NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter> >(*first))) {
            ++count;
        }
    }
    return count;
}

/**
 * @brief Counts the number of elements in the range that satisfy the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to search.
 * @param pred The predicate function to apply to each element.
 * @return The number of elements that satisfy the predicate in the range.
 */
template <ranges::Range Range, class Predicate>
    requires(Invocable<Predicate, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr usize countIf(Range&& range, Predicate pred)
    noexcept(noexcept(countIf(ranges::begin(range), ranges::end(range), pred))) {
    return countIf(ranges::begin(range), ranges::end(range), pred);
}

// =================================================================================
// Mismatch / equality / comparison
// =================================================================================

/**
 * @brief Compares two ranges [first1, last1) and [first2, first2 + (last1 - first1)) for equality.
 * @tparam Iter1 The type of the first iterator.
 * @tparam Iter2 The type of the second iterator.
 * @param first1 The beginning of the first range.
 * @param last1 The end of the first range.
 * @param first2 The beginning of the second range.
 * @return true if the ranges are equal, false otherwise.
 */
template <InputIterator Iter1, InputIterator Iter2>
    requires(EqualityComparableWith<IterRefOf<Iter1>, IterRefOf<Iter2>>)
NEX_NODISCARD constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2)
    noexcept(noexcept(*first1 == *first2)) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Compares two ranges [first1, last1) and [first2, last2) for equality.
 * @tparam Iter1 The type of the first iterator.
 * @tparam Iter2 The type of the second iterator.
 * @param first1 The beginning of the first range.
 * @param last1 The end of the first range.
 * @param first2 The beginning of the second range.
 * @param last2 The end of the second range.
 * @return true if the ranges are equal, false otherwise.
 */
template <InputIterator Iter1, InputIterator Iter2>
    requires(EqualityComparableWith<IterRefOf<Iter1>, IterRefOf<Iter2>>)
NEX_NODISCARD constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2, Iter2 last2)
    noexcept(noexcept(*first1 == *first2)) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return (first1 == last1) && (first2 == last2);
}

/**
 * @brief Compares two ranges [first1, last1) and [first2, first2 + (last1 - first1)) for equality
 *        using a binary predicate.
 * @tparam Iter1 The type of the first iterator.
 * @tparam Iter2 The type of the second iterator.
 * @tparam BinaryPredicate The type of the binary predicate function.
 * @param first1 The beginning of the first range.
 * @param last1 The end of the first range.
 * @param first2 The beginning of the second range.
 * @param pred The binary predicate function to compare elements.
 * @return true if the ranges are equal according to the predicate, false otherwise.
 */
template <InputIterator Iter1, InputIterator Iter2, class BinaryPredicate>
    requires(Invocable<BinaryPredicate, IterRefOf<Iter1>, IterRefOf<Iter2>>)
NEX_NODISCARD constexpr bool equal(Iter1 first1, Iter1 last1, Iter2 first2, BinaryPredicate pred)
    noexcept(noexcept(NEX_INVOKE(pred, *first1, *first2))) {
    for (; first1 != last1; ++first1, ++first2) {
        if (!NEX_INVOKE(pred, *first1, *first2)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Compares two ranges for equality.
 * @tparam Iter1 The type of the first iterator.
 * @tparam Iter2 The type of the second iterator.
 * @param first1 The beginning of the first range.
 * @param last1 The end of the first range.
 * @param first2 The beginning of the second range.
 * @param last2 The end of the second range.
 * @return true if the ranges are equal, false otherwise.
 */
template <ranges::Range Range1, ranges::Range Range2>
    requires(EqualityComparableWith<ranges::RangeReferenceT<Range1>, ranges::RangeReferenceT<Range2>>)
NEX_NODISCARD constexpr bool equal(Range1&& range1, Range2&& range2)
    noexcept(
        noexcept(equal(ranges::begin(range1), ranges::end(range1), ranges::begin(range2), ranges::end(range2)))
    ) {
    return equal(ranges::begin(range1), ranges::end(range1), ranges::begin(range2), ranges::end(range2));
}

/**
 * @brief Compares two ranges for equality using a binary predicate.
 * @tparam Range1 The type of the first range-like object.
 * @tparam Range2 The type of the second range-like object.
 * @tparam BinaryPredicate The type of the binary predicate function.
 * @param range1 The first range-like object to compare.
 * @param range2 The second range-like object to compare.
 * @param pred The binary predicate function to compare elements.
 * @return true if the ranges are equal according to the predicate, false otherwise.
 */
template <ranges::Range Range1, ranges::Range Range2, class BinaryPredicate>
    requires(Invocable<BinaryPredicate, ranges::RangeReferenceT<Range1>, ranges::RangeReferenceT<Range2>>)
NEX_NODISCARD constexpr bool equal(Range1&& range1, Range2&& range2, BinaryPredicate pred)
    noexcept(
        noexcept(equal(ranges::begin(range1), ranges::end(range1), ranges::begin(range2), ranges::end(range2), pred))
    ) {
    return equal(ranges::begin(range1), ranges::end(range1), ranges::begin(range2), ranges::end(range2), pred);
}

// mismatch(...)

// lexicographicalCompare(...)
// lexicographicalCompareThreeWay(...)

// =================================================================================
// Modification algorithms
// =================================================================================

/**
 * @brief Copies elements from the range [first, last) to the destination range starting at dest.
 * @tparam Iter The type of the input iterator.
 * @tparam Sent The type of the sentinel for the input range.
 * @tparam OutputIter The type of the output iterator.
 * @param first The beginning of the input range.
 * @param last The end of the input range.
 * @param dest The beginning of the destination range.
 * @return An iterator to the end of the destination range after copying.
 */
template <InputIterator Iter, class Sent, class OutputIter>
    requires(SentinelFor<Sent, Iter> && IndirectlyWritable<OutputIter, IterRefOf<Iter>>)
NEX_NODISCARD constexpr OutputIter copy(Iter first, Sent last, OutputIter dest)
    noexcept(noexcept(*dest = *first)) {
    for (; first != last; ++first, ++dest) {
        *dest = *first;
    }
    return dest;
}

/**
 * @brief Copies elements from the range to the destination range starting at dest.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @param range The range-like object to copy from.
 * @param dest The beginning of the destination range.
 * @return An iterator to the end of the destination range after copying.
 */
template <ranges::Range Range, class OutputIter>
    requires(IndirectlyWritable<OutputIter, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr OutputIter copy(Range&& range, OutputIter dest)
    noexcept(noexcept(copy(ranges::begin(range), ranges::end(range), dest))) {
    return copy(ranges::begin(range), ranges::end(range), dest);
}

/**
 * @brief Copies elements from the range [first, last) to the destination range starting at dest
 *        if they satisfy the predicate.
 * @tparam Iter The type of the input iterator.
 * @tparam Sent The type of the sentinel for the input range.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the input range.
 * @param last The end of the input range.
 * @param dest The beginning of the destination range.
 * @param pred The predicate function to determine which elements to copy.
 * @return An iterator to the end of the destination range after copying.
 */
template <InputIterator Iter, class Sent, class OutputIter, class Predicate>
    requires(SentinelFor<Sent, Iter> && IndirectlyWritable<OutputIter, IterRefOf<Iter>>)
NEX_NODISCARD constexpr OutputIter copyIf(Iter first, Sent last, OutputIter dest, Predicate pred)
    noexcept(noexcept(NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) &&
             noexcept(*dest = *first)) {
    for (; first != last; ++first) {
        if (NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            *dest = *first;
            ++dest;
        }
    }
    return dest;
}

/**
 * @brief Copies elements from the range to the destination range starting at dest
 *        if they satisfy the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to copy from.
 * @param dest The beginning of the destination range.
 * @param pred The predicate function to determine which elements to copy.
 * @return An iterator to the end of the destination range after copying.
 */
template <ranges::Range Range, class OutputIter, class Predicate>
    requires(IndirectlyWritable<OutputIter, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr OutputIter copyIf(Range&& range, OutputIter dest, Predicate pred)
    noexcept(noexcept(copyIf(ranges::begin(range), ranges::end(range), dest, pred))) {
    return copyIf(ranges::begin(range), ranges::end(range), dest, pred);
}

/**
 * @brief Copies a specified number of elements from the range [first, first + count) to
 *        the destination range starting at dest.
 * @tparam Iter The type of the input iterator.
 * @tparam Size The type of the count of elements to copy.
 * @tparam OutputIter The type of the output iterator.
 * @param first The beginning of the input range.
 * @param count The number of elements to copy.
 * @param dest The beginning of the destination range.
 * @return An iterator to the end of the destination range after copying.
 */
template <InputIterator Iter, class Size, class OutputIter>
    requires(IndirectlyWritable<OutputIter, IterRefOf<Iter>> && Integral<Size>)
NEX_NODISCARD constexpr OutputIter copyN(Iter first, Size count, OutputIter dest)
    noexcept(noexcept(*dest = *first)) {
    using IntegralSize = decltype(convertToIntegral(count));
    for (IntegralSize i = 0; i < convertToIntegral(count); ++i, ++first, ++dest) {
        *dest = *first;
    }
    return dest;
}

/**
 * @brief Copies a specified number of elements from the range to the destination range starting at dest.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @param range The range-like object to copy from.
 * @param dest The beginning of the destination range.
 * @return An iterator to the end of the destination range after copying.
 */
template <ranges::Range Range, class OutputIter>
    requires(IndirectlyWritable<OutputIter, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr OutputIter copyN(Range&& range, OutputIter dest)
    noexcept(noexcept(copyN(ranges::begin(range), ranges::size(range), dest))) {
    return copyN(ranges::begin(range), ranges::size(range), dest);
}

/**
 * @brief Moves elements from the range [first, last) to the destination range starting at dest.
 * @tparam Iter The type of the input iterator.
 * @tparam Sent The type of the sentinel for the input range.
 * @tparam OutputIter The type of the output iterator.
 * @param first The beginning of the input range.
 * @param last The end of the input range.
 * @param dest The beginning of the destination range.
 * @return An iterator to the end of the destination range after moving.
 */
template <InputIterator Iter, class Sent, class OutputIter>
NEX_NODISCARD constexpr OutputIter move(Iter first, Sent last, OutputIter dest)
    noexcept(noexcept(*dest = ranges::iterMove(first)))
    requires(SentinelFor<Sent, Iter> &&
             IndirectlyWritable<
                OutputIter,
                decltype(ranges::iterMove(first))
             >) {
    for (; first != last; ++first, ++dest) {
        *dest = ranges::iterMove(first);
    }
    return dest;
}

/**
 * @brief Moves elements from the range to the destination range starting at dest.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @param range The range-like object to move from.
 * @param dest The beginning of the destination range.
 * @return An iterator to the end of the destination range after moving.
 */
template <ranges::Range Range, class OutputIter>
NEX_NODISCARD constexpr OutputIter move(Range&& range, OutputIter dest)
    noexcept(noexcept(move(ranges::begin(range), ranges::end(range), dest))) {
    return move(ranges::begin(range), ranges::end(range), dest);
}

/**
 * @brief Moves elements from the range [first, last) to the destination range starting at dest
 *        if they satisfy the predicate.
 * @tparam Iter The type of the input iterator.
 * @tparam Sent The type of the sentinel for the input range.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the input range.
 * @param last The end of the input range.
 * @param dest The beginning of the destination range.
 * @param pred The predicate function to determine which elements to move.
 * @return An iterator to the end of the destination range after moving.
 */
template <InputIterator Iter, class Sent, class OutputIter, class Predicate>
NEX_NODISCARD constexpr OutputIter moveIf(Iter first, Sent last, OutputIter dest, Predicate pred)
    noexcept(noexcept(NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) &&
             noexcept(*dest = ranges::iterMove(first)))
    requires(SentinelFor<Sent, Iter> &&
             IndirectlyWritable<
                OutputIter,
                decltype(ranges::iterMove(first))
             >) {
    for (; first != last; ++first) {
        if (NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            *dest = ranges::iterMove(first);
            ++dest;
        }
    }
    return dest;
}

/**
 * @brief Moves elements from the range to the destination range starting at dest
 *        if they satisfy the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to move from.
 * @param dest The beginning of the destination range.
 * @param pred The predicate function to determine which elements to move.
 * @return An iterator to the end of the destination range after moving.
 */
template <ranges::Range Range, class OutputIter, class Predicate>
NEX_NODISCARD constexpr OutputIter moveIf(Range&& range, OutputIter dest, Predicate pred)
    noexcept(noexcept(moveIf(ranges::begin(range), ranges::end(range), dest, pred))) {
    return moveIf(ranges::begin(range), ranges::end(range), dest, pred);
}

/**
 * @brief Moves a specified number of elements from the range [first, first + count) to
 *        the destination range starting at dest.
 * @tparam Iter The type of the input iterator.
 * @tparam Size The type of the count of elements to move.
 * @tparam OutputIter The type of the output iterator.
 * @param first The beginning of the input range.
 * @param count The number of elements to move.
 * @param dest The beginning of the destination range.
 * @return An iterator to the end of the destination range after moving.
 */
template <InputIterator Iter, class Size, class OutputIter>
NEX_NODISCARD constexpr OutputIter moveN(Iter first, Size count, OutputIter dest)
    noexcept(noexcept(*dest = ranges::iterMove(first)))
    requires(IndirectlyWritable<
                OutputIter,
                decltype(ranges::iterMove(first))
             > &&
             Integral<Size>) {
    using IntegralSize = decltype(convertToIntegral(count));
    for (IntegralSize i = 0; i < convertToIntegral(count); ++i, ++first, ++dest) {
        *dest = ranges::iterMove(first);
    }
    return dest;
}

/**
 * @brief Moves a specified number of elements from the range to the destination range starting at dest.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @param range The range-like object to move from.
 * @param dest The beginning of the destination range.
 * @return An iterator to the end of the destination range after moving.
 */
template <ranges::Range Range, class OutputIter>
NEX_NODISCARD constexpr OutputIter moveN(Range&& range, OutputIter dest)
    noexcept(noexcept(moveN(ranges::begin(range), ranges::size(range), dest))) {
    return moveN(ranges::begin(range), ranges::size(range), dest);
}

/**
 * @brief Fills the range [first, last) with the specified value.
 * @tparam Iter The type of the iterator.
 * @tparam Sent The type of the sentinel for the range.
 * @tparam Type The type of the value to fill with.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param value The value to fill the range with.
 */
template <ForwardIterator Iter, class Sent, class Type>
    requires(SentinelFor<Sent, Iter> && IndirectlyWritable<Iter, const Type&>)
constexpr void fill(Iter first, Sent last, const Type& value)
    noexcept(noexcept(*first = value)) {
    for (; first != last; ++first) {
        *first = value;
    }
}

/**
 * @brief Fills the range with the specified value.
 * @tparam Range The type of the range-like object.
 * @tparam Type The type of the value to fill with.
 * @param range The range-like object to fill.
 * @param value The value to fill the range with.
 */
template <ranges::Range Range, class Type>
constexpr void fill(Range&& range, const Type& value)
    noexcept(noexcept(fill(ranges::begin(range), ranges::end(range), value))) {
    fill(ranges::begin(range), ranges::end(range), value);
}

/**
 * @brief Fills the range [first, first + count) with the specified value.
 * @tparam Iter The type of the iterator.
 * @tparam Size The type of the count of elements to fill.
 * @tparam Type The type of the value to fill with.
 * @param first The beginning of the range.
 * @param count The number of elements to fill.
 * @param value The value to fill the range with.
 */
template <ForwardIterator Iter, class Size, class Type>
    requires(IndirectlyWritable<Iter, const Type&> && Integral<Size>)
constexpr void fillN(Iter first, Size count, const Type& value)
    noexcept(noexcept(*first = value)) {
    using IntegralSize = decltype(convertToIntegral(count));
    for (IntegralSize i = 0; i < convertToIntegral(count); ++i, ++first) {
        *first = value;
    }
}

/**
 * @brief Fills the range with the specified value.
 * @tparam Range The type of the range-like object.
 * @tparam Type The type of the value to fill with.
 * @param range The range-like object to fill.
 * @param value The value to fill the range with.
 */
template <ranges::Range Range, class Type>
constexpr void fillN(Range&& range, const Type& value)
    noexcept(noexcept(fillN(ranges::begin(range), ranges::size(range), value))) {
    fillN(ranges::begin(range), ranges::size(range), value);
}

/**
 * @brief Applies a unary operation to each element in the range [first, last) and writes the result
 *        to the destination range starting at dest.
 * @tparam Iter The type of the input iterator.
 * @tparam Sent The type of the sentinel for the input range.
 * @tparam OutputIter The type of the output iterator.
 * @tparam UnaryOperation The type of the unary operation function.
 * @param first The beginning of the input range.
 * @param last The end of the input range.
 * @param dest The beginning of the destination range.
 * @param op The unary operation function to apply to each element.
 * @return An iterator to the end of the destination range after applying the operation.
 */
template <InputIterator Iter, class Sent, class OutputIter, class UnaryOperation>
NEX_NODISCARD constexpr OutputIter transform(Iter first, Sent last, OutputIter dest, UnaryOperation op)
    noexcept(
        noexcept(NEX_INVOKE(op, NEX_FORWARD<IterRefOf<Iter>>(*first))) &&
        noexcept(*dest = NEX_INVOKE(op, NEX_FORWARD<IterRefOf<Iter>>(*first)))
    )
    requires(
        SentinelFor<Sent, Iter> &&
        IndirectlyWritable<
            OutputIter,
            decltype(NEX_INVOKE(op, *first))
        >) {
    for (; first != last; ++first, ++dest) {
        *dest = NEX_INVOKE(op, NEX_FORWARD<IterRefOf<Iter>>(*first));
    }
    return dest;
}

/**
 * @brief Applies a unary operation to each element in the range and writes the result
 *        to the destination range starting at dest.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @tparam UnaryOperation The type of the unary operation function.
 * @param range The range-like object to transform.
 * @param dest The beginning of the destination range.
 * @param op The unary operation function to apply to each element.
 * @return An iterator to the end of the destination range after applying the operation.
 */
template <ranges::Range Range, class OutputIter, class UnaryOperation>
NEX_NODISCARD constexpr OutputIter transform(Range&& range, OutputIter dest, UnaryOperation op)
    noexcept(noexcept(transform(ranges::begin(range), ranges::end(range), dest, op)))
    requires(
        IndirectlyWritable<
            OutputIter,
            decltype(NEX_INVOKE(op, *ranges::begin(range)))
        >
    ) {
    return transform(ranges::begin(range), ranges::end(range), dest, op);
}

/**
 * @brief Replaces all occurrences of oldValue in the range [first, last) with newValue.
 * @tparam Iter The type of the iterator.
 * @tparam Sent The type of the sentinel for the range.
 * @tparam Type The type of the values to replace.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param oldValue The value to be replaced.
 * @param newValue The value to replace with.
 */
template <ForwardIterator Iter, class Sent, class Type>
    requires(SentinelFor<Sent, Iter> && IndirectlyWritable<Iter, const Type&>)
constexpr void replace(Iter first, Sent last, const Type& oldValue, const Type& newValue)
    noexcept(noexcept(*first = newValue)) {
    for (; first != last; ++first) {
        if (*first == oldValue) {
            *first = newValue;
        }
    }
}

/**
 * @brief Replaces all occurrences of oldValue in the range with newValue.
 * @tparam Range The type of the range-like object.
 * @tparam Type The type of the values to replace.
 * @param range The range-like object to modify.
 * @param oldValue The value to be replaced.
 * @param newValue The value to replace with.
 */
template <ranges::Range Range, class Type>
    requires(IndirectlyWritable<ranges::RangeReferenceT<Range>, const Type&>)
constexpr void replace(Range&& range, const Type& oldValue, const Type& newValue)
    noexcept(noexcept(replace(ranges::begin(range), ranges::end(range), oldValue, newValue))) {
    replace(ranges::begin(range), ranges::end(range), oldValue, newValue);
}

/**
 * @brief Replaces all elements in the range [first, last) that satisfy the predicate with newValue.
 * @tparam Iter The type of the iterator.
 * @tparam Sent The type of the sentinel for the range.
 * @tparam Predicate The type of the predicate function.
 * @tparam Type The type of the value to replace with.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param pred The predicate function to determine which elements to replace.
 * @param newValue The value to replace with.
 */
template <ForwardIterator Iter, class Sent, class Predicate, class Type>
    requires(SentinelFor<Sent, Iter> && IndirectlyWritable<Iter, const Type&>)
constexpr void replaceIf(Iter first, Sent last, Predicate pred, const Type& newValue)
    noexcept(noexcept(*first = newValue)) {
    for (; first != last; ++first) {
        if (NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            *first = newValue;
        }
    }
}

/**
 * @brief Replaces all elements in the range that satisfy the predicate with newValue.
 * @tparam Range The type of the range-like object.
 * @tparam Predicate The type of the predicate function.
 * @tparam Type The type of the value to replace with.
 * @param range The range-like object to modify.
 * @param pred The predicate function to determine which elements to replace.
 * @param newValue The value to replace with.
 */
template <ranges::Range Range, class Predicate, class Type>
    requires(IndirectlyWritable<ranges::RangeReferenceT<Range>, const Type&>)
constexpr void replaceIf(Range&& range, Predicate pred, const Type& newValue)
    noexcept(noexcept(replaceIf(ranges::begin(range), ranges::end(range), pred, newValue))) {
    replaceIf(ranges::begin(range), ranges::end(range), pred, newValue);
}

/**
 * @brief Copies elements from the range [first, last) to the destination range starting at dest,
 *        replacing all occurrences of oldValue with newValue.
 * @tparam Iter The type of the input iterator.
 * @tparam Sent The type of the sentinel for the input range.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Type The type of the values to replace.
 * @param first The beginning of the input range.
 * @param last The end of the input range.
 * @param dest The beginning of the destination range.
 * @param oldValue The value to be replaced.
 * @param newValue The value to replace with.
 * @return An iterator to the end of the destination range after copying and replacing.
 */
template <InputIterator Iter, class Sent, class OutputIter, class Type>
    requires(
        SentinelFor<Sent, Iter> &&
        IndirectlyWritable<OutputIter, IterReferenceT<Iter>> &&
        IndirectlyWritable<OutputIter, const Type&>
    )
NEX_NODISCARD constexpr OutputIter replaceCopy(
    Iter first,
    Sent last,
    OutputIter dest,
    const Type& oldValue,
    const Type& newValue
) noexcept(noexcept(*dest = newValue) && noexcept(*dest = *first)) {
    for (; first != last; ++first, ++dest) {
        *dest = (*first == oldValue) ? newValue : *first;
    }
    return dest;
}

/**
 * @brief Copies elements from the range to the destination range starting at dest,
 *        replacing all occurrences of oldValue with newValue.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Type The type of the values to replace.
 * @param range The range-like object to copy from.
 * @param dest The beginning of the destination range.
 * @param oldValue The value to be replaced.
 * @param newValue The value to replace with.
 * @return An iterator to the end of the destination range after copying and replacing.
 */
template <ranges::Range Range, class OutputIter, class Type>
    requires(
        IndirectlyWritable<OutputIter, ranges::RangeReferenceT<Range>> &&
        IndirectlyWritable<OutputIter, const Type&>
    )
NEX_NODISCARD constexpr OutputIter replaceCopy(
    Range&& range,
    OutputIter dest,
    const Type& oldValue,
    const Type& newValue
) noexcept(
    noexcept(replaceCopy(ranges::begin(range), ranges::end(range), dest, oldValue, newValue))
) {
    return replaceCopy(ranges::begin(range), ranges::end(range), dest, oldValue, newValue);
}

/**
 * @brief Copies elements from the range [first, last) to the destination range starting at dest,
 *        replacing all elements that satisfy the predicate with newValue.
 * @tparam Iter The type of the input iterator.
 * @tparam Sent The type of the sentinel for the input range.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Predicate The type of the predicate function.
 * @tparam Type The type of the value to replace with.
 * @param first The beginning of the input range.
 * @param last The end of the input range.
 * @param dest The beginning of the destination range.
 * @param pred The predicate function to determine which elements to replace.
 * @param newValue The value to replace with.
 * @return An iterator to the end of the destination range after copying and replacing.
 */
template <InputIterator Iter, class Sent, class OutputIter, class Predicate, class Type>
    requires(
        SentinelFor<Sent, Iter> &&
        IndirectlyWritable<OutputIter, IterReferenceT<Iter>> &&
        IndirectlyWritable<OutputIter, const Type&>
    )
NEX_NODISCARD constexpr OutputIter replaceCopyIf(
    Iter first,
    Sent last,
    OutputIter dest,
    Predicate pred,
    const Type& newValue
) noexcept(
    noexcept(NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) &&
    noexcept(*dest = newValue) &&
    noexcept(*dest = *first)
) {
    for (; first != last; ++first, ++dest) {
        *dest = NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first)) ? newValue : *first;
    }
    return dest;
}

/**
 * @brief Copies elements from the range to the destination range starting at dest,
 *        replacing all elements that satisfy the predicate with newValue.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Predicate The type of the predicate function.
 * @tparam Type The type of the value to replace with.
 * @param range The range-like object to copy from.
 * @param dest The beginning of the destination range.
 * @param pred The predicate function to determine which elements to replace.
 * @param newValue The value to replace with.
 * @return An iterator to the end of the destination range after copying and replacing.
 */
template <ranges::Range Range, class OutputIter, class Predicate, class Type>
    requires(
        IndirectlyWritable<OutputIter, ranges::RangeReferenceT<Range>> &&
        IndirectlyWritable<OutputIter, const Type&>
    )
NEX_NODISCARD constexpr OutputIter replaceCopyIf(
    Range&& range,
    OutputIter dest,
    Predicate pred,
    const Type& newValue
) noexcept(
    noexcept(replaceCopyIf(ranges::begin(range), ranges::end(range), dest, pred, newValue))
) {
    return replaceCopyIf(ranges::begin(range), ranges::end(range), dest, pred, newValue);
}

/**
 * @brief Generates values using a generator function and assigns them to the range [first, last).
 * @tparam Iter The type of the iterator.
 * @tparam Sent The type of the sentinel for the range.
 * @tparam Generator The type of the generator function.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param gen The generator function to produce values.
 */
template <ForwardIterator Iter, class Sent, class Generator>
    requires(
        SentinelFor<Sent, Iter> &&
        Invocable<Generator> &&
        IndirectlyWritable<Iter, InvokeResultT<Generator>>
    )
constexpr void generate(Iter first, Sent last, Generator gen)
    noexcept(
        noexcept(NEX_INVOKE(gen)) &&
        noexcept(*first = NEX_INVOKE(gen))
    ) {
    for (; first != last; ++first) {
        *first = NEX_INVOKE(gen);
    }
}

/**
 * @brief Generates values using a generator function and assigns them to the range.
 * @tparam Range The type of the range-like object.
 * @tparam Generator The type of the generator function.
 * @param range The range-like object to fill with generated values.
 * @param gen The generator function to produce values.
 */
template <ranges::Range Range, class Generator>
    requires(
        Invocable<Generator> &&
        IndirectlyWritable<ranges::RangeIteratorT<Range>, InvokeResultT<Generator>>
    )
constexpr void generate(Range&& range, Generator gen)
    noexcept(noexcept(generate(ranges::begin(range), ranges::end(range), gen))) {
    generate(ranges::begin(range), ranges::end(range), gen);
}

/**
 * @brief Generates a specified number of values using a generator function
 *        and assigns them to the range [first, first + count).
 * @tparam Iter The type of the iterator.
 * @tparam Size The type of the count of elements to generate.
 * @tparam Generator The type of the generator function.
 * @param first The beginning of the range.
 * @param count The number of values to generate.
 * @param gen The generator function to produce values.
 */
template <ForwardIterator Iter, class Size, class Generator>
    requires(
        Invocable<Generator> &&
        IndirectlyWritable<Iter, InvokeResultT<Generator>> &&
        Integral<Size>
    )
constexpr void generateN(Iter first, Size count, Generator gen)
    noexcept(
        noexcept(NEX_INVOKE(gen)) &&
        noexcept(*first = NEX_INVOKE(gen))
    ) {
    using IntegralSize = decltype(convertToIntegral(count));
    for (IntegralSize i = 0; i < convertToIntegral(count); ++i, ++first) {
        *first = NEX_INVOKE(gen);
    }
}

/**
 * @brief Generates a specified number of values using a generator function
 *        and assigns them to the range.
 * @tparam Range The type of the range-like object.
 * @tparam Generator The type of the generator function.
 * @param range The range-like object to fill with generated values.
 * @param gen The generator function to produce values.
 */
template <ranges::Range Range, class Generator>
    requires(
        Invocable<Generator> &&
        IndirectlyWritable<ranges::RangeIteratorT<Range>, InvokeResultT<Generator>>
    )
constexpr void generateN(Range&& range, Generator gen)
    noexcept(noexcept(generateN(ranges::begin(range), ranges::size(range), gen))) {
    generateN(ranges::begin(range), ranges::size(range), gen);
}

/**
 * @brief Removes all occurrences of value from the range [first, last) and returns an iterator
 *        to the new end of the range.
 * @tparam Iter The type of the iterator.
 * @tparam Sent The type of the sentinel for the range.
 * @tparam Type The type of the value to remove.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param value The value to remove from the range.
 * @return An iterator to the new end of the range after removing occurrences of value.
 */
template <ForwardIterator Iter, class Sent, class Type>
    requires(SentinelFor<Sent, Iter> && IndirectlyWritable<Iter, IterRefOf<Iter>>)
NEX_NODISCARD constexpr Iter remove(Iter first, Sent last, const Type& value)
    noexcept(noexcept(*first = *first)) {
    Iter result = first;
    for (; first != last; ++first) {
        if (!(*first == value)) {
            *result = *first;
            ++result;
        }
    }
    return result;
}

/**
 * @brief Removes all occurrences of value from the range and returns an iterator
 *        to the new end of the range.
 * @tparam Range The type of the range-like object.
 * @tparam Type The type of the value to remove.
 * @param range The range-like object to modify.
 * @param value The value to remove from the range.
 * @return An iterator to the new end of the range after removing occurrences of value.
 */
template <ranges::Range Range, class Type>
    requires(IndirectlyWritable<ranges::RangeIteratorT<Range>, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr auto remove(Range&& range, const Type& value)
    noexcept(noexcept(remove(ranges::begin(range), ranges::end(range), value)))
    -> decltype(ranges::begin(range)) {
    return remove(ranges::begin(range), ranges::end(range), value);
}

/**
 * @brief Removes all elements from the range [first, last) that satisfy the predicate
 *        and returns an iterator to the new end of the range.
 * @tparam Iter The type of the iterator.
 * @tparam Sent The type of the sentinel for the range.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the range.
 * @param last The end of the range.
 * @param pred The predicate function to determine which elements to remove.
 * @return An iterator to the new end of the range after removing elements that satisfy the predicate.
 */
template <ForwardIterator Iter, class Sent, class Predicate>
    requires(SentinelFor<Sent, Iter> && IndirectlyWritable<Iter, IterRefOf<Iter>>)
NEX_NODISCARD constexpr Iter removeIf(Iter first, Sent last, Predicate pred)
    noexcept(noexcept(*first = *first)) {
    Iter result = first;
    for (; first != last; ++first) {
        if (!NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            *result = *first;
            ++result;
        }
    }
    return result;
}

/**
 * @brief Removes all elements from the range that satisfy the predicate
 *        and returns an iterator to the new end of the range.
 * @tparam Range The type of the range-like object.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to modify.
 * @param pred The predicate function to determine which elements to remove.
 * @return An iterator to the new end of the range after removing elements that satisfy the predicate.
 */
template <ranges::Range Range, class Predicate>
    requires(IndirectlyWritable<ranges::RangeIteratorT<Range>, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr auto removeIf(Range&& range, Predicate pred)
    noexcept(noexcept(removeIf(ranges::begin(range), ranges::end(range), pred)))
    -> decltype(ranges::begin(range)) {
    return removeIf(ranges::begin(range), ranges::end(range), pred);
}

/**
 * @brief Copies elements from the range [first, last) to the destination range starting at dest,
 *        excluding all occurrences of value.
 * @tparam Iter The type of the input iterator.
 * @tparam Sent The type of the sentinel for the input range.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Type The type of the value to exclude.
 * @param first The beginning of the input range.
 * @param last The end of the input range.
 * @param dest The beginning of the destination range.
 * @param value The value to exclude from copying.
 * @return An iterator to the end of the destination range after copying and excluding occurrences of value.
 */
template <InputIterator Iter, class Sent, class OutputIter, class Type>
    requires(SentinelFor<Sent, Iter> && IndirectlyWritable<OutputIter, IterRefOf<Iter>>)
NEX_NODISCARD constexpr OutputIter removeCopy(Iter first, Sent last, OutputIter dest, const Type& value)
    noexcept(noexcept(*dest = *first)) {
    for (; first != last; ++first) {
        if (!(*first == value)) {
            *dest = *first;
            ++dest;
        }
    }
    return dest;
}

/**
 * @brief Copies elements from the range to the destination range starting at dest,
 *        excluding all occurrences of value.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Type The type of the value to exclude.
 * @param range The range-like object to copy from.
 * @param dest The beginning of the destination range.
 * @param value The value to exclude from copying.
 * @return An iterator to the end of the destination range after copying and excluding occurrences of value.
 */
template <ranges::Range Range, class OutputIter, class Type>
    requires(IndirectlyWritable<OutputIter, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr OutputIter removeCopy(Range&& range, OutputIter dest, const Type& value)
    noexcept(noexcept(removeCopy(ranges::begin(range), ranges::end(range), dest, value))) {
    return removeCopy(ranges::begin(range), ranges::end(range), dest, value);
}

/**
 * @brief Copies elements from the range [first, last) to the destination range starting at dest,
 *        excluding all elements that satisfy the predicate.
 * @tparam Iter The type of the input iterator.
 * @tparam Sent The type of the sentinel for the input range.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Predicate The type of the predicate function.
 * @param first The beginning of the input range.
 * @param last The end of the input range.
 * @param dest The beginning of the destination range.
 * @param pred The predicate function to determine which elements to exclude from copying.
 * @return An iterator to the end of the destination range after copying and excluding elements that satisfy the predicate.
 */
template <InputIterator Iter, class Sent, class OutputIter, class Predicate>
    requires(SentinelFor<Sent, Iter> && IndirectlyWritable<OutputIter, IterRefOf<Iter>>)
NEX_NODISCARD constexpr OutputIter removeCopyIf(Iter first, Sent last, OutputIter dest, Predicate pred)
    noexcept(noexcept(*dest = *first)) {
    for (; first != last; ++first) {
        if (!NEX_INVOKE(pred, NEX_FORWARD<IterRefOf<Iter>>(*first))) {
            *dest = *first;
            ++dest;
        }
    }
    return dest;
}

/**
 * @brief Copies elements from the range to the destination range starting at dest,
 *        excluding all elements that satisfy the predicate.
 * @tparam Range The type of the range-like object.
 * @tparam OutputIter The type of the output iterator.
 * @tparam Predicate The type of the predicate function.
 * @param range The range-like object to copy from.
 * @param dest The beginning of the destination range.
 * @param pred The predicate function to determine which elements to exclude from copying.
 * @return An iterator to the end of the destination range after copying and excluding elements that satisfy the predicate.
 */
template <ranges::Range Range, class OutputIter, class Predicate>
    requires(IndirectlyWritable<OutputIter, ranges::RangeReferenceT<Range>>)
NEX_NODISCARD constexpr OutputIter removeCopyIf(Range&& range, OutputIter dest, Predicate pred)
    noexcept(noexcept(removeCopyIf(ranges::begin(range), ranges::end(range), dest, pred))) {
    return removeCopyIf(ranges::begin(range), ranges::end(range), dest, pred);
}

// =================================================================================
// Swapping / permutation
// =================================================================================

/**
 * @brief Swaps elements between two ranges [first1, last1) and [first2, last2).
 * @tparam Iter1 The type of the first input iterator.
 * @tparam Sent1 The type of the sentinel for the first range.
 * @tparam Iter2 The type of the second input iterator.
 * @tparam Sent2 The type of the sentinel for the second range.
 * @param first1 The beginning of the first range.
 * @param last1 The end of the first range.
 * @param first2 The beginning of the second range.
 * @param last2 The end of the second range.
 * @return An iterator to the end of the second range after swapping.
 */
template <ForwardIterator Iter1, class Sent1, class Iter2, class Sent2>
    requires(SentinelFor<Sent1, Iter1> && SentinelFor<Sent2, Iter2> &&
             IndirectlySwappable<Iter1, Iter2>)
NEX_NODISCARD constexpr Iter2 swapRanges(Iter1 first1, Sent1 last1, Iter2 first2, Sent2 last2)
    noexcept(noexcept(ranges::iterSwap(first1, first2))) {
    while (first1 != last1 && first2 != last2) {
        ranges::iterSwap(first1, first2);
        ++first1;
        ++first2;
    }
    return first2;
}

/**
 * @brief Swaps elements between two ranges.
 * @tparam Range1 The type of the first range-like object.
 * @tparam Range2 The type of the second range-like object.
 * @param range1 The first range-like object to swap from.
 * @param range2 The second range-like object to swap from.
 * @return An iterator to the end of the second range after swapping.
 */
template <ranges::Range Range1, class Range2>
    requires(SentinelFor<ranges::SentinelT<Range1>, ranges::RangeIteratorT<Range1>> &&
             SentinelFor<ranges::SentinelT<Range2>, ranges::RangeIteratorT<Range2>> &&
             IndirectlySwappable<ranges::RangeIteratorT<Range1>, ranges::RangeIteratorT<Range2>>)
NEX_NODISCARD constexpr auto swapRanges(Range1&& range1, Range2&& range2)
    noexcept(
        noexcept(swapRanges(
            ranges::begin(range1), ranges::end(range1),
            ranges::begin(range2), ranges::end(range2)
        ))
    )
    -> decltype(ranges::begin(range2)) {
    return swapRanges(
        ranges::begin(range1), ranges::end(range1),
        ranges::begin(range2), ranges::end(range2)
    );
}

// reverse(...)
// reverseCopy(...)

// rotate(...)
// rotateCopy(...)

// shiftLeft(...)
// shiftRight(...)

// =================================================================================
// Binary search
// =================================================================================

// lowerBound(...)
// upperBound(...)
// equalRange(...)
// binarySearch(...)

// =================================================================================
// Partitioning
// =================================================================================

// isPartitioned(...)
// partition(...)
// partitionCopy(...)
// stablePartition(...)
// partitionPoint(...)

// =================================================================================
// Sorting
// =================================================================================

// isSorted(...)
// isSortedUntil(...)

// sort(...)

// stableSort(...)
// partialSort(...)
// partialSortCopy(...)
// nthElement(...)

// =================================================================================
// Heap algorithms
// =================================================================================

// makeHeap(...)
// pushHeap(...)
// popHeap(...)
// sortHeap(...)
// isHeap(...)
// isHeapUntil(...)

// =================================================================================
// Set algorithms
// =================================================================================

// setUnion(...)
// setIntersection(...)
// setDifference(...)
// setSymmetricDifference(...)
// includes(...)

// =================================================================================
// Merge algorithms
// =================================================================================

// merge(...)
// inplaceMerge(...)

// =================================================================================
// Numeric algorithms
// =================================================================================

// accumulate(...)
// reduce(...)
// innerProduct(...)

// transformReduce(...)
// transformExclusiveScan(...)
// transformInclusiveScan(...)
// exclusiveScan(...)
// inclusiveScan(...)
// adjacentDifference(...)
// partialSum(...)

// =================================================================================
// Permutation algorithms
// =================================================================================

// nextPermutation(...)
// prevPermutation(...)
// isPermutation(...)

// =================================================================================
// Sampling / random algorithms
// =================================================================================

// sample(...)
// shuffle(...)
// randomShuffle(...) // legacy, deprecated in C++14, removed in C++17

// =================================================================================
// Advanced search algorithms
// =================================================================================

// search(...)
// searchN(...)
// findEnd(...)
// findFirstOf(...)
// adjacentFind(...)

// =================================================================================
// Advanced sequence comparison algorithms
// =================================================================================

// startsWith(...)
// endsWith(...)

NEX_NAMESPACE_END
