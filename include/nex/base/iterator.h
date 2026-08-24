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

// =================================================================================
// Ranges utility for moving iterators [ranges::iterMove()]
// =================================================================================

NEX_SUBNAMESPACE_BEGIN(ranges)

NEX_SUBNAMESPACE_BEGIN(_iter_move)

void iterMove() = delete;

// Determines whether Type supports unqualified iterMove operations.
template <class Type>
concept UnqualifiedIterMove =
    meta::ClassOrEnum<RemoveCvrefT<Type>> &&
    requires(Type&& t) {
        iterMove(NEX_MOVE<Type>(t));
    };

// Determines whether Type supports move dereference operations.
template <class Type>
concept MoveDeref =
    !UnqualifiedIterMove<Type> &&
    requires(Type&& t) {
        *t;
        requires(IsLvalueReferenceV<decltype(*t)>);
    };

// Determines whether Type supports just dereference operations.
template <class Type>
concept JustDeref =
    !UnqualifiedIterMove<Type> &&
    !MoveDeref<Type> &&
    requires(Type&& t) {
        *t;
        requires(!IsLvalueReferenceV<decltype(*t)>);
    };

struct Fn {
    /**
     * @brief Returns the result of moving the value pointed to by the given iterator.
     * @tparam Iter The type of the iterator.
     * @param i The iterator to move the value from.
     * @return The result of moving the value pointed to by the iterator.
     * @note
     * This function attempts to move the value pointed to by the given iterator `i`.
     * It checks if the iterator supports unqualified `iterMove` operations, in which case
     * it calls `iterMove` directly.
     */
    template <class Iter>
        requires UnqualifiedIterMove<Iter>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr decltype(auto) operator()(Iter&& i) const
        noexcept(noexcept(iterMove(NEX_FORWARD<Iter>(i)))) {
        return iterMove(NEX_FORWARD<Iter>(i));
    }

    /**
     * @brief Returns the result of moving the value pointed to by the given iterator.
     * @tparam Iter The type of the iterator.
     * @param i The iterator to move the value from.
     * @return The result of moving the value pointed to by the iterator.
     * @note
     * This function attempts to move the value pointed to by the given iterator `i`.
     * It checks if the iterator supports move dereference operations and moves
     * the value using `NEX_MOVE`.
     */
    template <class Iter>
        requires MoveDeref<Iter>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Iter&& i) const
        noexcept(noexcept(NEX_MOVE(*NEX_FORWARD<Iter>(i)))) -> decltype(NEX_MOVE(*NEX_FORWARD<Iter>(i))) {
        return NEX_MOVE(*NEX_FORWARD<Iter>(i));
    }

    /**
     * @brief Returns the result of dereferencing the given iterator.
     * @tparam Iter The type of the iterator.
     * @param i The iterator to dereference.
     * @return The result of dereferencing the iterator.
     * @note
     * This function attempts to dereference the given iterator `i`.
     * It checks if the iterator supports just dereference operations and returns
     * the result of dereferencing it.
     */
    template <class Iter>
        requires JustDeref<Iter>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Iter&& i) const
        noexcept(noexcept(*NEX_FORWARD<Iter>(i))) -> decltype(*NEX_FORWARD<Iter>(i)) {
        return *NEX_FORWARD<Iter>(i);
    }
};

NEX_SUBNAMESPACE_END(_iter_move)

/**
 * @brief Returns the result of moving the value pointed to by the given iterator.
 * @tparam Iter The type of the iterator.
 * @param i The iterator to move the value from.
 * @return The result of moving the value pointed to by the iterator.
 * @note
 * This function attempts to move the value pointed to by the given iterator `i`.
 * It checks if the iterator supports unqualified `iterMove` operations, in which case
 * it calls `iterMove` directly. If not, it checks if the iterator supports move dereference
 * operations and moves the value using `NEX_MOVE`. If neither of these conditions is met,
 * it falls back to just dereferencing the iterator and returning the result.
 */
inline constexpr auto iterMove = _iter_move::Fn{};

NEX_SUBNAMESPACE_END(ranges)

// Represents the rvalue reference type obtained by moving the value pointed to by an iterator.
template <Dereferenceable Type>
    requires requires(Type& t) {
        { ranges::iterMove(t) } -> Referenceable;
    }
using IterRvalueReferenceT = decltype(ranges::iterMove(declval<Type&>()));

// =================================================================================
// Concepts for iterators, ranges, and containers
// =================================================================================

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

// Implementation detail for IndirectlyReadable concept.
template <class In>
concept IndirectlyReadableImpl =
    requires(const In i) {
        typename IteratorValueType<In>;
        typename IterReferenceT<In>;
        typename IterRvalueReferenceT<In>;
        { *i } -> SameAs<IterReferenceT<In>>;
        { ranges::iterMove(i) } -> SameAs<IterRvalueReferenceT<In>>;
    } &&
    CommonReferenceWith<IterReferenceT<In>&&, IteratorValueType<In>&> &&
    CommonReferenceWith<IterReferenceT<In>&&, IterRvalueReferenceT<In>&&> &&
    CommonReferenceWith<IterRvalueReferenceT<In>&&, const IteratorValueType<In>&>;

// Determines whether Type can be read through indirection.
template <class In>
concept IndirectlyReadable = IndirectlyReadableImpl<RemoveCvrefT<In>>;

// Determines whether Type can be written through Out.
template <class Out, class Type>
concept IndirectlyWritable = requires(Out&& out, Type&& value) {
    *out                                          = NEX_FORWARD<Type>(value); // not required to be equality-preserving
    *NEX_FORWARD<Out>(out)                        = NEX_FORWARD<Type>(value); // not required to be equality-preserving
    const_cast<const IterReferenceT<Out>&&>(*out) = NEX_FORWARD<Type>(value); // not required to be equality-preserving
    const_cast<const IterReferenceT<Out>&&>(*NEX_FORWARD<Out>(out)) =
        NEX_FORWARD<Type>(value); // not required to be equality-preserving
};

// Determines whether the type In can be moved to the type Out through indirection.
template <class In, class Out>
concept IndirectlyMovable =
    IndirectlyReadable<In> &&
    IndirectlyWritable<Out, IterRvalueReferenceT<In>>;

// Determines whether the type In can be moved and stored to the type Out through indirection.
template <class In, class Out>
concept IndirectlyMovableStorable =
    IndirectlyMovable<In, Out> &&
    IndirectlyWritable<Out, IteratorValueType<In>> &&
    Movable<IteratorValueType<In>> &&
    ConstructibleFrom<IteratorValueType<In>, IterRvalueReferenceT<In>> &&
    AssignableFrom<IteratorValueType<In>&, IterRvalueReferenceT<In>>;

// Determines whether the type In can be copied to the type Out through indirection.
template <class In, class Out>
concept IndirectlyCopyable = IndirectlyReadable<In> && IndirectlyWritable<Out, IterReferenceT<In>>;

// Determines whether the type In can be copied and stored to the type Out through indirection.
template <class In, class Out>
concept IndirectlyCopyableStorable =
    IndirectlyCopyable<In, Out> &&
    IndirectlyWritable<Out, IteratorValueType<In>&> &&
    IndirectlyWritable<Out, const IteratorValueType<In>&> &&
    IndirectlyWritable<Out, IteratorValueType<In>&&> &&
    IndirectlyWritable<Out, const IteratorValueType<In>&&> &&
    Copyable<IteratorValueType<In>> &&
    ConstructibleFrom<IteratorValueType<In>, IterReferenceT<In>> &&
    AssignableFrom<IteratorValueType<In>&, IterReferenceT<In>>;

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

// Determines whether SizedSentinelFor is disabled for the given types.
// By default, it is disabled for all types, but it can be specialized
// for specific types to enable SizedSentinelFor.
template <class, class>
inline constexpr bool DisableSizedSentinelFor = false;

// Determines whether Sentinel can terminate and measure distance for Iterator.
template <typename Sentinel, typename Iter>
concept SizedSentinelFor =
    SentinelFor<Sentinel, Iter> &&
    !DisableSizedSentinelFor<RemoveCvT<Sentinel>, RemoveCvT<Iter>> &&
    requires(const Iter& iterator, const Sentinel& sentinel) {
        { sentinel - iterator } -> SameAs<typename IteratorTraits<Iter>::difference_type>;
        { iterator - sentinel } -> SameAs<typename IteratorTraits<Iter>::difference_type>;
    };

// =================================================================================
// Ranges utility for swapping iterators [ranges::iterSwap()]
// =================================================================================

NEX_SUBNAMESPACE_BEGIN(ranges)

NEX_SUBNAMESPACE_BEGIN(_iter_swap)

template <class Iter1, class Iter2>
void iterSwap(Iter1, Iter2) = delete;

// Determines whether Iter1 and Iter2 support unqualified iterSwap operations.
template <class Iter1, class Iter2>
concept UnqualifiedIterSwap =
    (meta::ClassOrEnum<RemoveCvrefT<Iter1>> || meta::ClassOrEnum<RemoveCvrefT<Iter2>>) &&
    requires(Iter1&& x, Iter2&& y) {
        iterSwap(NEX_FORWARD<Iter1>(x), NEX_FORWARD<Iter2>(y));
    };

// Determines whether Iter1 and Iter2 are readable and swappable through indirection.
template <class Iter1, class Iter2>
concept ReadableSwappable =
    IndirectlyReadable<Iter1> && IndirectlyReadable<Iter2> &&
    SwappableWith<IterReferenceT<Iter1>, IterReferenceT<Iter2>>;

struct Fn {
    /**
     * @brief Swaps the values pointed to by the given iterators.
     * @tparam Iter1 The type of the first iterator.
     * @tparam Iter2 The type of the second iterator.
     * @param x The first iterator.
     * @param y The second iterator.
     * @note
     * This function attempts to swap the values pointed to by the given iterators `x` and `y`.
     * It checks if the iterators support unqualified `iterSwap` operations, in which case
     * it calls `iterSwap` directly. If not, it checks if the iterators are readable and swappable
     * through indirection, in which case it swaps the values using `ranges::swap`. If neither of
     * these conditions is met, it falls back to moving and storing the values through indirection.
     */
    template <class Iter1, class Iter2>
        requires UnqualifiedIterSwap<Iter1, Iter2>
    NEX_HIDDEN_FROM_ABI constexpr void operator()(Iter1&& x, Iter2&& y) const
        noexcept(noexcept(iterSwap(NEX_FORWARD<Iter1>(x), NEX_FORWARD<Iter2>(y)))) {
        (void)iterSwap(NEX_FORWARD<Iter1>(x), NEX_FORWARD<Iter2>(y));
    }

    /**
     * @brief Swaps the values pointed to by the given iterators.
     * @tparam Iter1 The type of the first iterator.
     * @tparam Iter2 The type of the second iterator.
     * @param x The first iterator.
     * @param y The second iterator.
     * @note
     * This function attempts to swap the values pointed to by the given iterators `x` and `y`.
     * It checks if the iterators are readable and swappable through indirection, in which case
     * it swaps the values using `ranges::swap`. If not, it falls back to moving and storing
     * the values through indirection.
     */
    template <class Iter1, class Iter2>
        requires(!UnqualifiedIterSwap<Iter1, Iter2>) && ReadableSwappable<Iter1, Iter2>
    NEX_HIDDEN_FROM_ABI constexpr void operator()(Iter1&& x, Iter2&& y) const
        noexcept(noexcept(ranges::swap(*NEX_FORWARD<Iter1>(x), *NEX_FORWARD<Iter2>(y)))) {
        ranges::swap(*NEX_FORWARD<Iter1>(x), *NEX_FORWARD<Iter2>(y));
    }

    /**
     * @brief Swaps the values pointed to by the given iterators.
     * @tparam Iter1 The type of the first iterator.
     * @tparam Iter2 The type of the second iterator.
     * @param x The first iterator.
     * @param y The second iterator.
     * @note
     * This function attempts to swap the values pointed to by the given iterators `x` and `y`.
     * It checks if the iterators can be moved and stored through indirection, in which case
     * it moves and stores the values to perform the swap. If not, it falls back to moving
     * and storing the values through indirection.
     */
    template <class Iter1, class Iter2>
        requires(!UnqualifiedIterSwap<Iter1, Iter2> &&
                 !ReadableSwappable<Iter1, Iter2>) &&
                 IndirectlyMovableStorable<Iter1, Iter2> &&
                 IndirectlyMovableStorable<Iter2, Iter1>
    NEX_HIDDEN_FROM_ABI constexpr void operator()(Iter1&& x, Iter2&& y) const
        noexcept(noexcept(IterValueOf<Iter2>(ranges::iterMove(y))) &&
                 noexcept(*y = ranges::iterMove(x)) &&
                 noexcept(*NEX_FORWARD<Iter1>(x) = declval<IterValueOf<Iter2>>())) {
        IterValueOf<Iter2> old(ranges::iterMove(y));
        *y                     = ranges::iterMove(x);
        *NEX_FORWARD<Iter1>(x) = NEX_MOVE(old);
    }
};

NEX_SUBNAMESPACE_END(_iter_swap)

/**
 * @brief Swaps the values pointed to by the given iterators.
 * @tparam Iter1 The type of the first iterator.
 * @tparam Iter2 The type of the second iterator.
 * @param x The first iterator.
 * @param y The second iterator.
 * @note
 * This function attempts to swap the values pointed to by the given iterators `x` and `y`.
 * It checks if the iterators support unqualified `iterSwap` operations, in which case
 * it calls `iterSwap` directly. If not, it checks if the iterators are readable and swappable
 * through indirection, in which case it swaps the values using `ranges::swap`. If neither of
 * these conditions is met, it falls back to moving and storing the values through indirection.
 */
inline constexpr auto iterSwap = _iter_swap::Fn{};

NEX_SUBNAMESPACE_END(ranges)

// =================================================================================
// Iterator-swapping concept
// =================================================================================

template <class Iter1, class Iter2 = Iter1>
concept IndirectlySwappable =
    IndirectlyReadable<Iter1> &&
    IndirectlyReadable<Iter2> &&
    requires(const Iter1 iter1, const Iter2 iter2) {
        ranges::iterSwap(iter1, iter1);
        ranges::iterSwap(iter2, iter2);
        ranges::iterSwap(iter1, iter2);
        ranges::iterSwap(iter2, iter1);
    };

// =================================================================================
// Container-specific iterators
// =================================================================================

// An iterator type that provides mutable access to the elements of a container type.
template<typename Container>
using IterOf = typename Container::iterator;

// An iterator type that provides const access to the elements of a container type.
template<typename Container>
using ConstIterOf = typename Container::const_iterator;

// =================================================================================
// Iterator trait aliases
// =================================================================================

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

// =================================================================================
// Iterator utility functions
// =================================================================================

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
void advanceImpl(InputIter& it, IteratorDifferenceType<InputIter> n) {
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
void advanceImpl(BiDirIter& it, IteratorDifferenceType<BiDirIter> n) {
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
void advanceImpl(RandIter& it, IteratorDifferenceType<RandIter> n) {
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
