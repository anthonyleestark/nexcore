/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/utility.h"
#include "nex/base/type_concepts.h"
#include "nex/base/type_traits.h"
#include "nex/base/iterator_traits.h"
#include "nex/base/pointer_traits.h"

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

// Represents the iterator traits type for iterator Iter.
template <typename Iter>
using IterTraitsOf = IteratorTraits<Iter>;

// Represents the value type produced by dereferencing iterator Iter.
template <typename Iter>
using IterValueOf = IteratorValueType<Iter>;

// Represents the reference type returned by dereferencing iterator Iter.
template <typename Iter>
using IterRefOf = IteratorReferenceType<Iter>;

// Represents the pointer type associated with iterator Iter.
template <typename Iter>
using IterPtrOf = IteratorPointerType<Iter>;

// Represents the difference type associated with iterator Iter.
template <typename Iter>
using IterDiffOf = IteratorDifferenceType<Iter>;

// ===========================================================================
// Range and iterator utilities
// ===========================================================================

/**
 * @brief Returns an iterator to the beginning of an array of a specified size.
 * @tparam Type The type of the array elements.
 * @tparam Capacity The size of the array.
 * @param array The array to get the beginning iterator for.
 * @return An iterator to the beginning of the array.
 */
template <class Type, usize Capacity>
NEX_NODISCARD constexpr Type* begin(Type (&array)[Capacity]) noexcept {
    return array;
}

/**
 * @brief Returns an iterator to the end of an array of a specified size.
 * @tparam Type The type of the array elements.
 * @tparam Capacity The size of the array.
 * @param array The array to get the end iterator for.
 * @return An iterator to one past the last element of the array.
 */
template <class Type, usize Capacity>
NEX_NODISCARD constexpr Type* end(Type (&array)[Capacity]) noexcept {
    return array + Capacity;
}

/**
 * @brief Returns an iterator to the beginning of a container or range.
 * @tparam Container The type of the container or range.
 * @param cont The container or range to get the beginning iterator for.
 * @return An iterator to the beginning of the container or range.
 */
template <typename Container>
NEX_NODISCARD constexpr auto begin(Container& cont) -> decltype(cont.begin()) {
    return cont.begin();
}

/**
 * @brief Returns a const iterator to the beginning of a container or range.
 * @tparam Container The type of the container or range.
 * @param cont The container or range to get the beginning iterator for.
 * @return A const iterator to the beginning of the container or range.
 */
template <typename Container>
NEX_NODISCARD constexpr auto begin(const Container& cont) -> decltype(cont.begin()) {
    return cont.begin();
}

/**
 * @brief Returns an iterator to the end of a container or range.
 * @tparam Container The type of the container or range.
 * @param cont The container or range to get the end iterator for.
 * @return An iterator to one past the last element of the container or range.
 */
template <typename Container>
NEX_NODISCARD constexpr auto end(Container& cont) -> decltype(cont.end()) {
    return cont.end();
}

/**
 * @brief Returns a const iterator to the end of a container or range.
 * @tparam Container The type of the container or range.
 * @param cont The container or range to get the end iterator for.
 * @return A const iterator to one past the last element of the container or range.
 */
template <typename Container>
NEX_NODISCARD constexpr auto end(const Container& cont) -> decltype(cont.end()) {
    return cont.end();
}

/**
 * @brief Returns a const iterator to the beginning of a container or range.
 * @tparam Container The type of the container or range.
 * @param cont The container or range to get the beginning iterator for.
 * @return A const iterator to the beginning of the container or range.
 */
template <typename Container>
NEX_NODISCARD constexpr 
auto cbegin(const Container& cont) noexcept(noexcept(begin(cont))) -> decltype(begin(cont)) {
    return begin(cont);
}

/**
 * @brief Returns a const iterator to the end of a container or range.
 * @tparam Container The type of the container or range.
 * @param cont The container or range to get the end iterator for.
 * @return A const iterator to one past the last element of the container or range.
 */
template <typename Container>
NEX_NODISCARD constexpr 
auto cend(const Container& cont) noexcept(noexcept(end(cont))) -> decltype(end(cont)) {
    return end(cont);
}

/**
 * @brief Advances an input iterator by a specified number of steps.
 * @tparam InputIter The type of the input iterator.
 * @param it The input iterator to advance.
 * @param n The number of steps to advance the iterator.
 */
template <InputIterator InputIter>
NEX_HIDDEN_FROM_ABI constexpr
void advanceImpl(InputIter& it, typename IteratorTraits<InputIter>::difference_type n) {
    for (; n > 0; --n) {
        ++it;  // Advance the iterator forward by incrementing it.
    }
}

/**
 * @brief Advances a bidirectional iterator by a specified number of steps.
 * @tparam BiDirIter The type of the bidirectional iterator.
 * @param it The bidirectional iterator to advance.
 * @param n The number of steps to advance the iterator. Can be negative for backward movement.
 */
template <BidirectionalIterator BiDirIter>
NEX_HIDDEN_FROM_ABI constexpr
void advanceImpl(BiDirIter& it, typename IteratorTraits<BiDirIter>::difference_type n) {
    if (n >= 0) {
        for (; n > 0; --n) {
            ++it;  // Advance the iterator forward by incrementing it.
        }
    } else {
        for (; n < 0; ++n) {
            --it;  // Move the iterator backward by decrementing it.
        }
    }
}

/**
 * @brief Advances a random access iterator by a specified number of steps.
 * @tparam RandIter The type of the random access iterator.
 * @param it The random access iterator to advance.
 * @param n The number of steps to advance the iterator. Can be negative for backward movement.
 */
template <RandomAccessIterator RandIter>
NEX_HIDDEN_FROM_ABI constexpr
void advanceImpl(RandIter& it, typename IteratorTraits<RandIter>::difference_type n) {
    it += n;  // Advance the iterator by adding the difference `n` to it.
}


/**
 * @brief Advances an iterator by a specified number of steps.
 * @tparam InputIter The type of the input iterator.
 * @tparam Distance The type of the distance to advance the iterator.
 * @param it The iterator to advance.
 * @param origN The number of steps to advance the iterator. Can be negative for bidirectional iterators.
 * @note 
 * This function advances the given iterator `it` by `origN` steps. If `origN` is positive, the iterator is advanced forward;
 * if `origN` is negative and the iterator is bidirectional, it is moved backward. The function ensures that the advancement
 * respects the capabilities of the iterator type.
 */
template <
    class InputIter,
    class Distance,
    class IntegralDistance = decltype(convertToIntegral(meta::declval<Distance>()))
> requires(Integral<IntegralDistance>)
NEX_HIDDEN_FROM_ABI constexpr void advance(InputIter& it, Distance origN) {
    using Difference = IteratorDifferenceType<InputIter>;
    Difference n = static_cast<Difference>(convertToIntegral(origN));
    if constexpr (!BidirectionalIterator<InputIter>) {
        NEX_ASSERT_MSG(n >= 0, "Error: Can only pass a negative `n` with a BidirectionalIterator.");
    }
    advanceImpl(it, n);
}

/**
 * @brief Returns the previous iterator by decrementing the given bidirectional iterator by `n` steps.
 * @tparam Iter The type of the bidirectional iterator.
 * @param it The bidirectional iterator to decrement.
 * @param n The number of steps to decrement the iterator.
 * @return The previous iterator obtained by decrementing `it` by `n` steps.
 */
template <BidirectionalIterator Iter>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr
Iter prev(Iter it, IteratorDifferenceType<Iter> n) {
    advance(it, -n);
    return it;
}

/**
 * @brief Returns the previous iterator by decrementing the given bidirectional iterator by one step.
 * @tparam Iter The type of the bidirectional iterator.
 * @param it The bidirectional iterator to decrement.
 * @return The previous iterator obtained by decrementing `it` by one step.
 */
template <BidirectionalIterator Iter>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Iter prev(Iter it) {
    return prev(NEX_MOVE(it), 1);
}

/**
 * @brief Returns the next iterator by incrementing the given input iterator.
 * @tparam InputIter The type of the input iterator.
 * @param it The input iterator to increment.
 * @return The next iterator obtained by incrementing `it`.
 */
template <InputIterator InputIter>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr
InputIter next(InputIter it, IteratorDifferenceType<InputIter> n = 1) {
    advance(it, n);
    return it;
}

NEX_NAMESPACE_END
