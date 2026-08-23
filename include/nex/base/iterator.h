/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/init.h"
#include "nex/base/type_concepts.h"
#include "nex/base/type_traits.h"
#include "nex/base/iterator_traits.h"

NEX_NAMESPACE_BEGIN

// ============================================================================
// Concepts for iterators, ranges, and containers
// ============================================================================

// Determines whether Type supports weak increment operations.
template <typename Type>
concept WeaklyIncrementable =
    !SameAs<Type, bool> &&
    Movable<Type> &&
    requires(Type& iter) {
        typename IteratorTraits<meta::RemoveCvrefT<Type>>::difference_type;
        { ++iter } -> SameAs<Type&>;
        iter++;
    };

// Determines whether Type supports regular increment operations.
template <typename Type>
concept Incrementable =
    Regular<Type> &&
    WeaklyIncrementable<Type> &&
    requires(Type& iter) {
        { iter++ } -> SameAs<Type>;
    };

// Determines whether Type can be read through indirection.
template <typename Type>
concept IndirectlyReadable = NEX_STD indirectly_readable<Type>;

// Determines whether Value can be written through Out.
template <typename Out, typename Value>
concept IndirectlyWritable = NEX_STD indirectly_writable<Out, Value>;

// Determines whether Type is an input or output iterator.
template <typename Iter>
concept Iterator =
    WeaklyIncrementable<Iter> &&
    Dereferenceable<Iter>;

// Determines whether Type is an input iterator.
template <typename Type>
concept InputIterator =
    Iterator<Type> &&
    EqualityComparable<Type> &&
    IndirectlyReadable<Type>;

// Determines whether Type can write Value through iterator assignment.
template <typename Type, typename Value>
concept OutputIterator =
    Iterator<Type> &&
    IndirectlyWritable<Type, Value>;

// Determines whether Type is a forward iterator.
template <typename Type>
concept ForwardIterator =
    InputIterator<Type> &&
    Incrementable<Type>;

// Determines whether Type is a bidirectional iterator.
template <typename Type>
concept BidirectionalIterator =
    ForwardIterator<Type> &&
    requires(Type& iter) {
        { --iter } -> SameAs<Type&>;
        iter--;
    };

// Determines whether Type is a random-access iterator.
template <typename Type>
concept RandomAccessIterator =
    BidirectionalIterator<Type> &&
    requires(
        Type iter,
        typename IteratorTraits<Type>::difference_type n) {
        { iter += n } -> SameAs<Type&>;
        { iter -= n } -> SameAs<Type&>;
        { iter + n } -> SameAs<Type>;
        { n + iter } -> SameAs<Type>;
        { iter - n } -> SameAs<Type>;
        { iter[n] };
        { iter - iter } -> SameAs<typename IteratorTraits<Type>::difference_type>;
    };

// Determines whether Type is a contiguous iterator.
template <typename Type>
concept ContiguousIterator =
    RandomAccessIterator<Type> &&
    meta::IsLvalueReferenceV<IterReferenceT<Type>> &&
    SameAs<IterValueOf<Type>, meta::RemoveCvrefT<IterReferenceT<Type>>> &&
    requires(const Type& iter) {
        { NEX_TO_ADDRESS(iter) } -> SameAs<meta::AddPointerT<IterReferenceT<Type>>>;
    };

// Determines whether Sentinel can terminate iteration for Iterator.
template <typename Sentinel, typename Iter>
concept SentinelFor =
    Semiregular<Sentinel> &&
    Iterator<Iter> &&
    WeaklyEqualityComparableWith<Sentinel, Iter>;

// Determines whether Sentinel can terminate and measure distance for Iterator.
template <typename Sentinel, typename Iter>
concept SizedSentinelFor =
    SentinelFor<Sentinel, Iter> &&
    requires(const Iter& iterator, const Sentinel& sentinel) {
        { sentinel - iterator } -> SameAs<typename IteratorTraits<Iter>::difference_type>;
        { iterator - sentinel } -> SameAs<typename IteratorTraits<Iter>::difference_type>;
    };

// ============================================================================
// Container-specific iterators
// ============================================================================

// An iterator type that provides mutable access to the elements of a container type.
template<typename Container>
using IterOf = typename Container::iterator;

// An iterator type that provides const access to the elements of a container type.
template<typename Container>
using ConstIterOf = typename Container::const_iterator;

// ============================================================================
// Iterator trait aliases
// ============================================================================

// The iterator traits type for iterator It.
template <typename It>
using IterTraitsOf = IteratorTraits<It>;

// The value type produced by dereferencing iterator It.
template <typename It>
using IterValueOf = IteratorValueType<It>;

// The reference type returned by dereferencing iterator It.
template <typename It>
using IterRefOf = IteratorReferenceType<It>;

// The pointer type associated with iterator It.
template <typename It>
using IterPtrOf = IteratorPointerType<It>;

// The difference type associated with iterator It.
template <typename It>
using IterDiffOf = IteratorDifferenceType<It>;

// ===========================================================================
// Iterator utilities
// ===========================================================================

// Returns an iterator to the beginning of an array of a specified size.
template <class Type, usize Capacity>
NEX_NODISCARD constexpr Type* begin(Type (&array)[Capacity]) noexcept {
    return array;
}

// Returns an iterator to the end of an array of a specified size.
template <class Type, usize Capacity>
NEX_NODISCARD constexpr Type* end(Type (&array)[Capacity]) noexcept {
    return array + Capacity;
}

// Returns an iterator to the beginning of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto begin(Container& cont) -> decltype(cont.begin()) {
    return cont.begin();
}

// Returns a const iterator to the beginning of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto begin(const Container& cont) -> decltype(cont.begin()) {
    return cont.begin();
}

// Returns an iterator to the end of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto end(Container& cont) -> decltype(cont.end()) {
    return cont.end();
}

// Returns a const iterator to the end of a container or range.
template <typename Container>
NEX_NODISCARD constexpr auto end(const Container& cont) -> decltype(cont.end()) {
    return cont.end();
}

// Return a const iterator to the beginning of a container or range.
template <typename Container>
NEX_NODISCARD constexpr 
auto cbegin(const Container& cont) noexcept(noexcept(begin(cont))) -> decltype(begin(cont)) {
    return begin(cont);
}

// Return a const iterator to the end of a container or range.
template <typename Container>
NEX_NODISCARD constexpr 
auto cend(const Container& cont) noexcept(noexcept(end(cont))) -> decltype(end(cont)) {
    return end(cont);
}

NEX_NAMESPACE_END
