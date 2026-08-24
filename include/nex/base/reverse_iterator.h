/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/utility.h"
#include "nex/base/type_concepts.h"
#include "nex/base/type_traits.h"
#include "nex/base/iterator_traits.h"
#include "nex/base/iterator.h"

NEX_NAMESPACE_BEGIN

/**
 * @class ReverseIterator
 * @brief Iterator adaptor that traverses a range in reverse order.
 *
 * @details
 * `ReverseIterator` wraps a bidirectional iterator and reverses its traversal direction.
 * Incrementing the reverse iterator moves the underlying iterator backward, while
 * decrementing it moves the underlying iterator forward.
 *
 * The adaptor preserves the bidirectional capability of the underlying iterator.
 * When the underlying iterator satisfies `RandomAccessIterator`, the reverse iterator
 * also provides random-access operations such as addition, subtraction, indexing,
 * and ordering comparisons.
 *
 * A reverse iterator does not preserve the `ContiguousIterator` property, even when
 * the underlying iterator is contiguous, because the reverse iterator's current
 * position does not directly correspond to the address represented by its `base()`
 * iterator.
 *
 * @tparam Iter The type of the underlying bidirectional iterator.
 *
 * @note
 * The `base()` iterator refers to the position immediately after the element
 * referenced by the reverse iterator. Therefore, dereferencing a reverse iterator
 * is equivalent to dereferencing the iterator obtained by decrementing `base()`.
 */
template <BidirectionalIterator Iter>
class ReverseIterator {
public:
    using iterator_type = Iter;
    using traits_type = IteratorTraits<Iter>;

    using self = ReverseIterator<Iter>;

    using value_type = IteratorValueType<Iter>;
    using difference_type = IteratorDifferenceType<Iter>;
    using pointer = IteratorPointerType<Iter>;
    using reference = IteratorReferenceType<Iter>;

private:
    Iter current;   // The underlying iterator that the reverse iterator wraps around.

public:
    // Default constructor that initializes the reverse iterator to a default-constructed state.
    NEX_HIDDEN_FROM_ABI constexpr ReverseIterator() : current() {}

    // Constructs a reverse iterator explicitly from an iterator.
    NEX_HIDDEN_FROM_ABI constexpr explicit ReverseIterator(Iter it) : current(it) {}

    // Constructs a reverse iterator from another reverse iterator of a different type.
    template <class OtherIter>
        requires (SameAs<Iter, OtherIter> || ConvertibleTo<OtherIter, Iter>)
    NEX_HIDDEN_FROM_ABI constexpr ReverseIterator(const ReverseIterator<OtherIter>& other)
        : current(other.base()) {}

    // Assigns a reverse iterator from another reverse iterator of a different type.
    template <class OtherIter>
        requires (SameAs<Iter, OtherIter> || ConvertibleTo<OtherIter, Iter> || IsAssignableV<Iter&, OtherIter const&>)
    NEX_HIDDEN_FROM_ABI constexpr ReverseIterator& operator=(const ReverseIterator<OtherIter>& other) {
        current = other.base();
        return *this;
    }

    // Returns the underlying iterator.
    NEX_HIDDEN_FROM_ABI constexpr Iter base() const { return current; }

    // Dereference operator that returns a reference to the element pointed to by the reverse iterator.
    NEX_HIDDEN_FROM_ABI constexpr reference operator*() const {
        Iter tmp = current;
        return *--tmp;
    }

    // Arrow operator that returns a pointer to the element pointed to by the reverse iterator.
    NEX_HIDDEN_FROM_ABI constexpr pointer operator->() const {
        Iter tmp = current;
        --tmp;
        if constexpr (IsPointerV<Iter>) {
            return tmp;
        } else {
            return tmp.operator->();
        }
    }

    // Pre-increment operator that moves the reverse iterator to the next element.
    NEX_HIDDEN_FROM_ABI constexpr self& operator++() {
        --current;
        return *this;
    }

    // Post-increment operator that moves the reverse iterator to the next element and returns the previous state.
    NEX_HIDDEN_FROM_ABI constexpr self operator++(int) {
        self tmp(*this);
        --current;
        return tmp;
    }

    // Pre-decrement operator that moves the reverse iterator to the previous element.
    NEX_HIDDEN_FROM_ABI constexpr self& operator--() {
        ++current;
        return *this;
    }

    // Post-decrement operator that moves the reverse iterator to the previous element and returns the previous state.
    NEX_HIDDEN_FROM_ABI constexpr self operator--(int) {
        self tmp(*this);
        ++current;
        return tmp;
    }

    // Addition operator that returns a new reverse iterator advanced by n positions.
    NEX_HIDDEN_FROM_ABI constexpr self operator+(difference_type n) const
        requires(RandomAccessIterator<Iter>) {
        return self(current - n);
    }

    // Addition assignment operator that advances the reverse iterator by n positions.
    NEX_HIDDEN_FROM_ABI constexpr self& operator+=(difference_type n)
        requires(RandomAccessIterator<Iter>) {
        current -= n;
        return *this;
    }

    // Subtraction operator that returns a new reverse iterator moved back by n positions.
    NEX_HIDDEN_FROM_ABI constexpr self operator-(difference_type n) const
        requires(RandomAccessIterator<Iter>) {
        return self(current + n);
    }

    // Subtraction assignment operator that moves the reverse iterator back by n positions.
    NEX_HIDDEN_FROM_ABI constexpr self& operator-=(difference_type n)
        requires(RandomAccessIterator<Iter>) {
        current += n;
        return *this;
    }

    // Subscript operator that returns a reference to the element at the specified position.
    NEX_HIDDEN_FROM_ABI constexpr reference operator[](difference_type n) const
        requires(RandomAccessIterator<Iter>) {
        return *(*this + n);
    }

    // Returns the result of moving the value pointed to by the given reverse iterator.
    NEX_HIDDEN_FROM_ABI friend constexpr
    IterRvalueReferenceT<Iter> iterMove(const self& it)
        noexcept(IsNothrowCopyConstructibleV<Iter> && noexcept(ranges::iterMove(--declval<Iter&>()))) {
        auto tmp = it.base();
        return ranges::iterMove(--tmp);
    }

    // Swaps the values pointed to by the given reverse iterators.
    template <class OtherIter>
        requires (IndirectlySwappable<Iter, OtherIter>)
    NEX_HIDDEN_FROM_ABI friend constexpr void iterSwap(const self& x, const ReverseIterator<OtherIter>& y)
        noexcept(IsNothrowCopyConstructibleV<Iter> && IsNothrowCopyConstructibleV<OtherIter> &&
            noexcept(ranges::iterSwap(--declval<Iter&>(), --declval<OtherIter&>()))) {
        auto tmpX = x.base();
        auto tmpY = y.base();
        ranges::iterSwap(--tmpX, --tmpY);
    }
};

// Represents the value type of a reverse iterator.
template <class Iter>
using RevIterValueType = typename ReverseIterator<Iter>::value_type;

// Represents the difference type of a reverse iterator.
template <class Iter>
using RevIterDifferenceType = typename ReverseIterator<Iter>::difference_type;

// Represents the pointer type of a reverse iterator.
template <class Iter>
using RevIterPointerType = typename ReverseIterator<Iter>::pointer;

// Represents the reference type of a reverse iterator.
template <class Iter>
using RevIterReferenceType = typename ReverseIterator<Iter>::reference;

/**
 * @brief Compares two reverse iterators for equality.
 * @tparam Iter1 The type of the first reverse iterator.
 * @tparam Iter2 The type of the second reverse iterator.
 * @param x The first reverse iterator.
 * @param y The second reverse iterator.
 * @return `true` if the underlying iterators of `x` and `y` are equal, `false` otherwise.
 */
template <class Iter1, class Iter2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator==(const ReverseIterator<Iter1>& x, const ReverseIterator<Iter2>& y)
    requires requires {
        { x.base() == y.base() } -> ConvertibleTo<bool>;
    } {
    return x.base() == y.base();
}

/**
 * @brief Compares two reverse iterators for inequality.
 * @tparam Iter1 The type of the first reverse iterator.
 * @tparam Iter2 The type of the second reverse iterator.
 * @param x The first reverse iterator.
 * @param y The second reverse iterator.
 * @return `true` if the underlying iterators of `x` and `y` are not equal, `false` otherwise.
 */
template <class Iter1, class Iter2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator!=(const ReverseIterator<Iter1>& x, const ReverseIterator<Iter2>& y)
    requires requires {
        { x.base() != y.base() } -> ConvertibleTo<bool>;
    } {
    return x.base() != y.base();
}

/**
 * @brief Compares two reverse iterators for less-than ordering.
 * @tparam Iter1 The type of the first reverse iterator.
 * @tparam Iter2 The type of the second reverse iterator.
 * @param x The first reverse iterator.
 * @param y The second reverse iterator.
 * @return `true` if the underlying iterator of `x` is greater than that of `y`, `false` otherwise.
 */
template <class Iter1, class Iter2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator<(const ReverseIterator<Iter1>& x, const ReverseIterator<Iter2>& y)
    requires (RandomAccessIterator<Iter1> && RandomAccessIterator<Iter2>) &&
        requires {
            { x.base() > y.base() } -> ConvertibleTo<bool>;
        } {
    return x.base() > y.base();
}

/**
 * @brief Compares two reverse iterators for less-than-or-equal ordering.
 * @tparam Iter1 The type of the first reverse iterator.
 * @tparam Iter2 The type of the second reverse iterator.
 * @param x The first reverse iterator.
 * @param y The second reverse iterator.
 * @return `true` if the underlying iterator of `x` is greater than or equal to that of `y`, `false` otherwise.
 */
template <class Iter1, class Iter2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator<=(const ReverseIterator<Iter1>& x, const ReverseIterator<Iter2>& y)
    requires (RandomAccessIterator<Iter1> && RandomAccessIterator<Iter2>) &&
        requires {
            { x.base() >= y.base() } -> ConvertibleTo<bool>;
        } {
    return x.base() >= y.base();
}

/**
 * @brief Compares two reverse iterators for greater-than ordering.
 * @tparam Iter1 The type of the first reverse iterator.
 * @tparam Iter2 The type of the second reverse iterator.
 * @param x The first reverse iterator.
 * @param y The second reverse iterator.
 * @return `true` if the underlying iterator of `x` is less than that of `y`, `false` otherwise.
 */
template <class Iter1, class Iter2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator>(const ReverseIterator<Iter1>& x, const ReverseIterator<Iter2>& y)
    requires (RandomAccessIterator<Iter1> && RandomAccessIterator<Iter2>) &&
        requires {
            { x.base() < y.base() } -> ConvertibleTo<bool>;
        } {
    return x.base() < y.base();
}

/**
 * @brief Compares two reverse iterators for greater-than-or-equal ordering.
 * @tparam Iter1 The type of the first reverse iterator.
 * @tparam Iter2 The type of the second reverse iterator.
 * @param x The first reverse iterator.
 * @param y The second reverse iterator.
 * @return `true` if the underlying iterator of `x` is less than or equal to that of `y`, `false` otherwise.
 */
template <class Iter1, class Iter2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator>=(const ReverseIterator<Iter1>& x, const ReverseIterator<Iter2>& y)
    requires (RandomAccessIterator<Iter1> && RandomAccessIterator<Iter2>) &&
        requires {
            { x.base() <= y.base() } -> ConvertibleTo<bool>;
        } {
    return x.base() <= y.base();
}

/**
 * @brief Computes the difference between two reverse iterators.
 * @tparam Iter1 The type of the first reverse iterator.
 * @tparam Iter2 The type of the second reverse iterator.
 * @param x The first reverse iterator.
 * @param y The second reverse iterator.
 * @return The difference between the underlying iterators of `y` and `x`.
 */
template <class Iter1, class Iter2>
    requires (RandomAccessIterator<Iter1> && RandomAccessIterator<Iter2>)
NEX_HIDDEN_FROM_ABI inline constexpr
auto operator-(const ReverseIterator<Iter1>& x, const ReverseIterator<Iter2>& y)
    -> decltype(y.base() - x.base()) {
    return y.base() - x.base();
}

/**
 * @brief Adds a difference to a reverse iterator.
 * @tparam Iter The type of the reverse iterator.
 * @param n The difference to add.
 * @param x The reverse iterator.
 * @return A new reverse iterator that is `n` positions ahead of `x`.
 */
template <class Iter>
NEX_HIDDEN_FROM_ABI inline constexpr
ReverseIterator<Iter> operator+(RevIterDifferenceType<Iter> n, const ReverseIterator<Iter>& x) {
    return ReverseIterator<Iter>(x.base() - n);
}

// Disable sized sentinel for reverse iterators if the underlying iterators do not support it.
template <class Iter1, class Iter2>
    requires(!SizedSentinelFor<Iter1, Iter2>)
inline constexpr bool DisableSizedSentinelFor<ReverseIterator<Iter1>, ReverseIterator<Iter2>> = true;

/**
 * @brief Creates a reverse iterator from a given iterator.
 * @tparam Iter The type of the iterator.
 * @param it The iterator to create a reverse iterator from.
 * @return A reverse iterator that wraps around the given iterator.
 */
template <class Iter>
NEX_HIDDEN_FROM_ABI inline constexpr ReverseIterator<Iter> makeReverseIterator(Iter it) {
    return ReverseIterator<Iter>(it);
}

NEX_NAMESPACE_END
