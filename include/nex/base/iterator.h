/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <iterator>
#include <algorithm>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/concepts.h"
#include "nex/base/traits.h"

NEX_NAMESPACE_BEGIN

// ============================================================================
// Iterator tags
// ============================================================================

/// An iterator tag that indicates an input iterator, 
/// which can be used to read values through the iterator.
using InputIteratorTag = NEX_STD input_iterator_tag;

/// An iterator tag that indicates an output iterator, 
/// which can be used to write values through the iterator.
using OutputIteratorTag = NEX_STD output_iterator_tag;

/// An iterator tag that indicates a forward iterator, 
/// which can be used to read and write values in a single-pass sequence.
using ForwardIteratorTag = NEX_STD forward_iterator_tag;

/// An iterator tag that indicates a bidirectional iterator, 
/// which can be used to read and write values in a sequence that can be traversed in both directions.
using BidirectionalIteratorTag = NEX_STD bidirectional_iterator_tag;

/// An iterator tag that indicates a random access iterator, 
/// which can be used to read and write values in a sequence that supports constant-time access to any element.
using RandomAccessIteratorTag = NEX_STD random_access_iterator_tag;

/// An iterator tag that indicates a contiguous iterator, 
/// which can be used to read and write values in a sequence that is stored contiguously in memory.
using ContiguousIteratorTag = NEX_STD contiguous_iterator_tag;

// ============================================================================
// Iterator adaptors
// ============================================================================

/// An iterator adaptor that reverses the direction of iteration for a given iterator type It.
template <typename It>
using ReverseIterator = NEX_STD reverse_iterator<It>;

/// An iterator adaptor that allows moving elements from a given iterator type It.
template <typename It>
using MoveIterator = NEX_STD move_iterator<It>;

/// An iterator adaptor that allows inserting elements into a container at a specified position.
template <typename It>
using InsertIterator = NEX_STD insert_iterator<It>;

/// An iterator adaptor that allows inserting elements at the end of a container.
template <typename It>
using BackInsertIterator = NEX_STD back_insert_iterator<It>;

/// An iterator adaptor that allows inserting elements at the front of a container.
template <typename It>
using FrontInsertIterator = NEX_STD front_insert_iterator<It>;

// ============================================================================
// Container-specific iterators
// ============================================================================

/// An iterator type that provides mutable access to the elements of a container type.
template<typename Container>
using IterOf = typename Container::iterator;

/// An iterator type that provides const access to the elements of a container type.
template<typename Container>
using ConstIterOf = typename Container::const_iterator;

/// An iterator adaptor that provides reverse iteration for a given container type.
template <typename Container>
using RevIterOf = ReverseIterator<typename Container::iterator>;

/// An iterator adaptor that provides reverse iteration for a given container type, using const iterators.
template <typename Container>
using ConstRevIterOf = ReverseIterator<typename Container::const_iterator>;

// ============================================================================
// Iterator trait aliases
// ============================================================================

/// The iterator traits type for iterator It.
template <typename It>
using IterTraitsOf = IteratorTraits<It>;

/// The value type produced by dereferencing iterator It.
template <typename It>
using IterValueOf = IteratorValueType<It>;

/// The reference type returned by dereferencing iterator It.
template <typename It>
using IterRefOf = IteratorReference<It>;

/// The pointer type associated with iterator It.
template <typename It>
using IterPtrOf = IteratorPointer<It>;

/// The difference type associated with iterator It.
template <typename It>
using IterDiffOf = IteratorDifferenceType<It>;

/// The iterator category associated with iterator It.
template <typename It>
using IterCategoryOf = IteratorCategory<It>;

// ============================================================================
// Range-based iterator aliases
// ============================================================================

/// The iterator type returned by begin() for a mutable range type.
template<typename Range>
using RangeIterOf = decltype(NEX_STD ranges::begin(NEX_STD declval<Range&>()));

/// The iterator type returned by begin() for a const range type.
template <typename Range>
using RangeConstIterOf = decltype(NEX_STD ranges::begin(NEX_STD declval<const Range&>()));

/// The iterator type returned by end() for a mutable range type.
template <typename Range>
using RangeSentinelOf = decltype(NEX_STD ranges::end(NEX_STD declval<Range&>()));

/// The iterator type returned by end() for a const range type.
template <typename Range>
using RangeConstSentinelOf = decltype(NEX_STD ranges::end(NEX_STD declval<const Range&>()));

/// The reverse iterator type for a container's mutable iterator.
template <typename Range>
using RangeRevIterOf = ReverseIterator<RangeIterOf<Range>>;

/// The reverse iterator type for a container's const iterator.
template <typename Range>
using RangeConstRevIterOf = ReverseIterator<RangeConstIterOf<Range>>;

// ===========================================================================
// Iterator utilities
// ===========================================================================

/// Returns an iterator to the beginning of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto begin(Container& cont) -> decltype(cont.begin()) {
    return NEX_STD begin(cont);
}

/// Returns a const iterator to the beginning of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto begin(const Container& cont) -> decltype(cont.begin()) {
    return NEX_STD begin(cont);
}

/// Returns an iterator to the end of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto end(Container& cont) -> decltype(cont.end()) {
    return NEX_STD end(cont);
}

/// Returns a const iterator to the end of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto end(const Container& cont) -> decltype(cont.end()) {
    return NEX_STD end(cont);
}

/// Return a const iterator to the beginning of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto 
cbegin(const Container& cont) noexcept(noexcept(NEX_STD begin(cont))) 
    -> decltype(NEX_STD begin(cont)) {
    return NEX_STD begin(cont);
}

/// Return a const iterator to the end of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto 
cend(const Container& cont) noexcept(noexcept(NEX_STD end(cont))) -> decltype(NEX_STD end(cont)) {
    return NEX_STD end(cont);
}

/// Returns a reverse iterator to the beginning of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto rbegin(Container& cont) -> decltype(cont.rbegin()) {
    return NEX_STD rbegin(cont);
}

/// Return a const reverse iterator to the beginning of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto rbegin(const Container& cont) -> decltype(cont.rbegin()) {
    return NEX_STD rbegin(cont);
}

/// Returns a reverse iterator to the end of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto rend(Container& cont) -> decltype(cont.rend()) {
    return NEX_STD rend(cont);
}

/// Return a const reverse iterator to the end of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto rend(const Container& cont) -> decltype(cont.rend()) {
    return NEX_STD rend(cont);
}

/// Return a const reverse iterator to the beginning of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto crbegin(const Container& cont) -> decltype(NEX_STD crbegin(cont)) {
    return NEX_STD crbegin(cont);
}

/// Return a const reverse iterator to the end of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto crend(const Container& cont) -> decltype(NEX_STD crend(cont)) {
    return NEX_STD crend(cont);
}

/// Returns the distance between two iterators, which is the number of elements between them.
template <typename InputIter, typename InputIter>
NEX_NODISCARD constexpr auto distance(InputIter first, InputIter last) 
    -> decltype(NEX_STD distance(first, last, IterCategoryOf<InputIter>{})) {
    return NEX_STD distance(first, last, IterCategoryOf<InputIter>{});
}

/// Advances an iterator by a specified distance, which can be positive or negative.
template <typename InputIter, typename Distance>
constexpr void advance(InputIter& it, Distance n) {
    NEX_STD advance(it, n);
}

/// Returns the next iterator after advancing it by a specified distance, which can be positive or negative.
template <typename InputIter, typename Distance = IterDiffOf<InputIter>>
NEX_NODISCARD constexpr auto next(InputIter it, Distance n = 1) -> decltype(NEX_STD next(it, n)) {
    return NEX_STD next(it, n);
}

/// Returns the previous iterator after retreating it by a specified distance, which can be positive or negative.
template <typename InputIter, typename Distance = IterDiffOf<InputIter>>
NEX_NODISCARD constexpr auto prev(InputIter it, Distance n = 1) -> decltype(NEX_STD prev(it, n)) {
    return NEX_STD prev(it, n);
}

/// Visits each element in the range defined by the iterators first and last, 
/// applying a specified function to each element.
template <typename InputIter, typename Func>
constexpr auto forEach(InputIter first, InputIter last, Func func) 
    -> decltype(NEX_STD for_each(first, last, func)) {
    return NEX_STD for_each(first, last, func);
}

/// Visits each element in a container or range, applying a specified function to each element.
/// This is a convenience overload that deduces the iterator types from the container.
template <typename Container>
constexpr auto forEach(Container& cont, auto&& func) 
    -> decltype(NEX_STD for_each(NEX_STD begin(cont), NEX_STD end(cont), func)) {
    return NEX_STD for_each(NEX_STD begin(cont), NEX_STD end(cont), func);
}

/// Returns an insert iterator that inserts elements at a specified position in a container.
/// The position is specified by an iterator which points to the element before which new elements 
/// will be inserted.
template <typename Container>
NEX_NODISCARD constexpr auto inserter(Container& cont, RangeIterOf<Container> it) 
    -> decltype(NEX_STD inserter(cont, it)) {
    return NEX_STD inserter(cont, it);
}

/// Returns an insert iterator that inserts elements at the beginning of a container.
template <typename Container>
NEX_NODISCARD constexpr auto frontInserter(Container& cont) -> decltype(NEX_STD front_inserter(cont)) {
    return NEX_STD front_inserter(cont);
}

/// Returns an insert iterator that inserts elements at the end of a container.
template <typename Container>
NEX_NODISCARD constexpr auto backInserter(Container& cont) -> decltype(NEX_STD back_inserter(cont)) {
    return NEX_STD back_inserter(cont);
}

/// Returns the size of a container or range, which is the number of elements it contains.
template <typename Container>
NEX_NODISCARD constexpr auto size(const Container& cont) -> decltype(cont.size()) {
    return cont.size();
}

/// Returns the signed size of a container or range, which is the number of elements it contains, 
/// represented as a signed integer type.
template <typename Container>
NEX_NODISCARD constexpr auto ssize(const Container& cont)
    -> CommonType<isize, MakeSigned<decltype(cont.size())>> {
    return static_cast<ptrdiff_t>(cont.size());
}

/// Returns the size of a built-in array, which is the number of elements it contains.
template <class Type, usize N>
NEX_NODISCARD constexpr usize size(const Type (&array)[N]) noexcept {
    return N;
}

/// Returns the signed size of a built-in array, which is the number of elements it contains, 
/// represented as a signed integer type.
template <class Type, isize N>
NEX_NODISCARD constexpr isize ssize(const Type (&array)[N]) noexcept {
    return static_cast<isize>(N);
}

/// Checks whether a container or range is empty, which means it contains no elements.
template <typename Container>
NEX_NODISCARD constexpr auto empty(const Container& cont) -> decltype(cont.empty()) {
    return cont.empty();
}

/// Checks whether a built-in array is empty, which means it contains no elements.
template <typename Type, usize N>
NEX_NODISCARD constexpr bool empty(const Type (&array)[N]) noexcept {
    return N == 0;
}

/// Checks whether an initializer list is empty, which means it contains no elements.
template <typename Element>
NEX_NODISCARD constexpr bool empty(NEX_STD initializer_list<Element> il) noexcept {
    return il.size() == 0;
}

/// Returns a pointer to the block of memory containing the elements of a container or range, 
/// which can be used for direct access to the elements.
template <typename Container>
NEX_NODISCARD constexpr auto data(Container& cont) -> decltype(cont.data()) {
    return cont.data();
}

/// Returns a pointer to the block of memory containing the elements of a container or range, 
/// which can be used for direct access to the elements.
template <typename Container>
NEX_NODISCARD constexpr auto data(const Container& cont) -> decltype(cont.data()) {
    return cont.data();
}

/// Returns a pointer to the first element of a built-in array, 
/// which can be used for direct access to the elements.
template <typename Type, usize N>
NEX_NODISCARD constexpr Type* data(Type (&array)[N]) noexcept {
    return array;
}

/// Returns a pointer to the first element of an initializer list, 
/// which can be used for direct access to the elements.
template <typename Element>
NEX_NODISCARD constexpr const Element* data(NEX_STD initializer_list<Element> il) noexcept {
    return il.begin();
}

NEX_NAMESPACE_END
