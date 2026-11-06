/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/export.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/fwd.h"
#include "nex/base/casts.h"
#include "nex/base/utility.h"
#include "nex/base/type_concepts.h"
#include "nex/base/type_traits.h"

NEX_NAMESPACE_BEGIN

/**
 * @struct Pair
 * @brief  A simple structure that holds two values of potentially different types.
 * 
 * @details
 * The Pair structure is a lightweight container that holds two values, referred to as `first` and `second`.
 * It is commonly used to represent a pair of related values, such as key-value pairs in associative containers,
 * or to return multiple values from a function. The types of the two elements can be different, allowing for
 * flexibility in usage.
 * 
 * @tparam First  The type of the first element.
 * @tparam Second The type of the second element.
 * 
 * @see Tuple for a more general-purpose container that can hold an arbitrary number of values.
 */
template <class First, class Second>
struct NEX_API Pair {
    using first_type = First;
    using second_type = Second;

    first_type first;       // The first element of the Pair.
    second_type second;     // The second element of the Pair.

    NEX_HIDDEN_FROM_ABI constexpr Pair(const Pair&) = default;
    NEX_HIDDEN_FROM_ABI constexpr Pair(Pair&&) = default;

    // Default constructor initializes both elements using their default constructors.
    NEX_HIDDEN_FROM_ABI constexpr Pair()
        noexcept(
            IsNothrowDefaultConstructibleV<first_type> &&
            IsNothrowDefaultConstructibleV<second_type>
        ) : first(), second() {}

    // Constructor that initializes the first and second elements with the provided values.
    NEX_HIDDEN_FROM_ABI constexpr Pair(const first_type& a, const second_type& b)
        noexcept(
            IsNothrowCopyConstructibleV<first_type> &&
            IsNothrowCopyConstructibleV<second_type>
        ) : first(a), second(b) {}

    // Constructor that initializes the first and second elements with the provided values of potentially
    // different types, if they are constructible to the respective member types.
    template <class Type1, class Type2>
        requires (IsConstructibleV<first_type, Type1> && IsConstructibleV<second_type, Type2>)
    NEX_HIDDEN_FROM_ABI constexpr Pair(Type1&& a, Type2&& b)
        noexcept(
            IsNothrowConstructibleV<first_type, Type1> &&
            IsNothrowConstructibleV<second_type, Type2>
        ) : first(NEX_FORWARD<Type1>(a)), second(NEX_FORWARD<Type2>(b)) {}

    // Copy constructor that allows constructing a Pair from another Pair with potentially different
    // element types, if they are constructible to the respective member types.
    template <class Type1, class Type2>
        requires (IsConstructibleV<first_type, Type1> && IsConstructibleV<second_type, Type2>)
    NEX_HIDDEN_FROM_ABI constexpr Pair(Pair<Type1, Type2>& other)
        noexcept(
            IsNothrowConstructibleV<first_type, Type1&> &&
            IsNothrowConstructibleV<second_type, Type2&>
        ) : first(other.first), second(other.second) {}

    // Copy constructor that allows constructing a Pair from another Pair with potentially different
    // element types, if they are constructible to the respective member types.
    template <class Type1, class Type2>
        requires (IsConstructibleV<first_type, Type1> && IsConstructibleV<second_type, Type2>)
    NEX_HIDDEN_FROM_ABI constexpr Pair(const Pair<Type1, Type2>& other)
        noexcept(
            IsNothrowConstructibleV<first_type, const Type1&> &&
            IsNothrowConstructibleV<second_type, const Type2&>
        ) : first(other.first), second(other.second) {}

    // Move constructor that allows constructing a Pair from another Pair with potentially different
    // element types, if they are constructible to the respective member types.
    template <class Type1, class Type2>
        requires (IsConstructibleV<first_type, Type1> && IsConstructibleV<second_type, Type2>)
    NEX_HIDDEN_FROM_ABI constexpr Pair(Pair<Type1, Type2>&& other)
        noexcept(
            IsNothrowConstructibleV<first_type, Type1&&> &&
            IsNothrowConstructibleV<second_type, Type2&&>
        ) : first(NEX_FORWARD<Type1>(other.first)), second(NEX_FORWARD<Type2>(other.second)) {}

    // Move constructor that allows constructing a Pair from another Pair with potentially different
    // element types, if they are constructible to the respective member types.
    template <class Type1, class Type2>
        requires (IsConstructibleV<first_type, Type1> && IsConstructibleV<second_type, Type2>)
    NEX_HIDDEN_FROM_ABI constexpr Pair(const Pair<Type1, Type2>&& other)
        noexcept(
            IsNothrowConstructibleV<first_type, const Type1&&> &&
            IsNothrowConstructibleV<second_type, const Type2&&>
        ) : first(NEX_MOVE(other.first)), second(NEX_MOVE(other.second)) {}

    // Copy assignment operator that allows assigning from another Pair if the element types are copy assignable.
    NEX_HIDDEN_FROM_ABI constexpr Pair& operator=(
        ConditionalT<
            IsCopyAssignableV<first_type> && IsCopyAssignableV<second_type>,
            Pair&,
            meta::NAT
        > const& other
    ) noexcept(
        IsNothrowCopyAssignableV<first_type> &&
        IsNothrowCopyAssignableV<second_type>
    ) {
        first  = other.first;
        second = other.second;
        return *this;
    }

    // Move assignment operator that allows assigning from another Pair if the element types are move assignable.
    NEX_HIDDEN_FROM_ABI constexpr Pair& operator=(
        ConditionalT<
            IsMoveAssignableV<first_type> && IsMoveAssignableV<second_type>,
            Pair&,
            meta::NAT
        >&& other
    ) noexcept(
        IsNothrowMoveAssignableV<first_type> &&
        IsNothrowMoveAssignableV<second_type>
    ) {
        first  = NEX_FORWARD<first_type>(other.first);
        second = NEX_FORWARD<second_type>(other.second);
        return *this;
    }

    // Copy assignment operator that allows assigning from another Pair with potentially different
    // element types, if they are assignable to the respective member types.
    template <class Type1, class Type2>
        requires (IsAssignableV<first_type&, Type1> && IsAssignableV<second_type&, Type2>)
    NEX_HIDDEN_FROM_ABI constexpr Pair& operator=(const Pair<Type1, Type2>& other)
        noexcept(
            IsNothrowAssignableV<first_type, const Type1&> &&
            IsNothrowAssignableV<second_type, const Type2&>
        ) {
        first  = other.first;
        second = other.second;
        return *this;
    }

    // Move assignment operator that allows assigning from another Pair with potentially different
    // element types, if they are assignable to the respective member types.
    template <class Type1, class Type2>
        requires (IsAssignableV<first_type&, Type1> && IsAssignableV<second_type&, Type2>)
    NEX_HIDDEN_FROM_ABI constexpr Pair& operator=(Pair<Type1, Type2>&& other)
        noexcept(
            IsNothrowAssignableV<first_type, Type1&&> &&
            IsNothrowAssignableV<second_type, Type2&&>
        ) {
        first  = NEX_FORWARD<Type1>(other.first);
        second = NEX_FORWARD<Type2>(other.second);
        return *this;
    }

    // Copy assignment operator that allows assigning from another Pair with the same element types,
    // returning a const reference to the current Pair. This is useful for const-correctness.
    template <class = void>
    NEX_HIDDEN_FROM_ABI constexpr const Pair& operator=(const Pair& other) const
        noexcept(IsNothrowCopyAssignableV<const first_type> &&
                 IsNothrowCopyAssignableV<const second_type>)
        requires(IsCopyAssignableV<const first_type> &&
                 IsCopyAssignableV<const second_type>) {
        first  = other.first;
        second = other.second;
        return *this;
    }

    // Move assignment operator that allows assigning from another Pair with the same element types,
    // returning a const reference to the current Pair. This is useful for const-correctness.
    template <class = void>
    NEX_HIDDEN_FROM_ABI constexpr const Pair& operator=(Pair&& other) const
        noexcept(IsNothrowAssignableV<const first_type&, first_type> &&
                 IsNothrowAssignableV<const second_type&, second_type>)
        requires(IsAssignableV<const first_type&, first_type> &&
                 IsAssignableV<const second_type&, second_type>) {
        first  = NEX_FORWARD<first_type>(other.first);
        second = NEX_FORWARD<second_type>(other.second);
        return *this;
    }

    // Copy assignment operator that allows assigning from another Pair with potentially different element types,
    // returning a const reference to the current Pair. This is useful for const-correctness.
    template <class Type1, class Type2>
    NEX_HIDDEN_FROM_ABI constexpr const Pair& operator=(const Pair<Type1, Type2>& other) const
        requires(IsAssignableV<const first_type&, const Type1&> &&
                 IsAssignableV<const second_type&, const Type2&>) {
        first  = other.first;
        second = other.second;
        return *this;
    }

    // Move assignment operator that allows assigning from another Pair with potentially different element types,
    // returning a const reference to the current Pair. This is useful for const-correctness.
    template <class Type1, class Type2>
    NEX_HIDDEN_FROM_ABI constexpr const Pair& operator=(Pair<Type1, Type2>&& other) const
        requires(IsAssignableV<const first_type&, Type1> &&
                 IsAssignableV<const second_type&, Type2>) {
        first  = NEX_FORWARD<Type1>(other.first);
        second = NEX_FORWARD<Type2>(other.second);
        return *this;
    }

    // Exchange the contents of this Pair with another Pair.
    NEX_HIDDEN_FROM_ABI constexpr void swap(Pair& other)
        noexcept(IsNothrowSwappableV<first_type> && IsNothrowSwappableV<second_type>) {
        NEX_SWAP(first, other.first);
        NEX_SWAP(second, other.second);
    }

    // Exchanges the contents of this Pair with another Pair (const version),
    // allowing for swapping with const-qualified Pairs.
    NEX_HIDDEN_FROM_ABI constexpr void swap(Pair& other) const
        noexcept(IsNothrowSwappableV<const first_type> && IsNothrowSwappableV<const second_type>) {
        NEX_SWAP(first, other.first);
        NEX_SWAP(second, other.second);
    }
};

// Deduction guide for Pair to allow automatic type deduction from constructor arguments.
template <class First, class Second>
Pair(First, Second) -> Pair<First, Second>;

/**
 * @brief Compares two Pair objects for equality.
 * @tparam First1 The type of the first element in the first Pair.
 * @tparam Second1 The type of the second element in the first Pair.
 * @tparam First2 The type of the first element in the second Pair.
 * @tparam Second2 The type of the second element in the second Pair.
 * @param lhs The first Pair to compare.
 * @param rhs The second Pair to compare.
 * @return true if both the first and second elements of the Pairs are equal; false otherwise.
 * @note This function is only enabled if the first and second elements of both Pairs are comparable for equality.
 */
template <class First1, class Second1, class First2, class Second2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator==(const Pair<First1, Second1>& lhs, const Pair<First2, Second2>& rhs)
    requires requires {
        { lhs.first == rhs.first } -> BooleanTestable;
        { lhs.second == rhs.second } -> BooleanTestable;
    } {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

/**
 * @brief Compares two Pair objects for inequality.
 * @tparam First1 The type of the first element in the first Pair.
 * @tparam Second1 The type of the second element in the first Pair.
 * @tparam First2 The type of the first element in the second Pair.
 * @tparam Second2 The type of the second element in the second Pair.
 * @param lhs The first Pair to compare.
 * @param rhs The second Pair to compare.
 * @return true if either the first or second elements of the Pairs are not equal; false otherwise.
 * @note This function is only enabled if the first and second elements of both Pairs are comparable for inequality.
 */
template <class First1, class Second1, class First2, class Second2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator!=(const Pair<First1, Second1>& lhs, const Pair<First2, Second2>& rhs)
    requires requires {
        { lhs.first != rhs.first } -> BooleanTestable;
        { lhs.second != rhs.second } -> BooleanTestable;
    } {
    return !(lhs == rhs);
}

/**
 * @brief Compares two Pair objects for less-than ordering.
 * @tparam First1 The type of the first element in the first Pair.
 * @tparam Second1 The type of the second element in the first Pair.
 * @tparam First2 The type of the first element in the second Pair.
 * @tparam Second2 The type of the second element in the second Pair.
 * @param lhs The first Pair to compare.
 * @param rhs The second Pair to compare.
 * @return true if the first Pair is less than the second Pair; false otherwise.
 * @note This function is only enabled if the first and second elements of both Pairs are comparable for ordering.
 */
template <class First1, class Second1, class First2, class Second2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator<(const Pair<First1, Second1>& lhs, const Pair<First2, Second2>& rhs)
    requires requires {
        { lhs.first < rhs.first } -> BooleanTestable;
        { lhs.second < rhs.second } -> BooleanTestable;
    } {
    return (lhs.first < rhs.first) || (!(rhs.first < lhs.first) && (lhs.second < rhs.second));
}

/**
 * @brief Compares two Pair objects for greater-than ordering.
 * @tparam First1 The type of the first element in the first Pair.
 * @tparam Second1 The type of the second element in the first Pair.
 * @tparam First2 The type of the first element in the second Pair.
 * @tparam Second2 The type of the second element in the second Pair.
 * @param lhs The first Pair to compare.
 * @param rhs The second Pair to compare.
 * @return true if the first Pair is greater than the second Pair; false otherwise.
 * @note This function is only enabled if the first and second elements of both Pairs are comparable for ordering.
 */
template <class First1, class Second1, class First2, class Second2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator>(const Pair<First1, Second1>& lhs, const Pair<First2, Second2>& rhs)
    requires requires {
        { rhs.first < lhs.first } -> BooleanTestable;
        { rhs.second < lhs.second } -> BooleanTestable;
    } {
    return rhs < lhs;
}

/**
 * @brief Compares two Pair objects for less-than-or-equal-to ordering.
 * @tparam First1 The type of the first element in the first Pair.
 * @tparam Second1 The type of the second element in the first Pair.
 * @tparam First2 The type of the first element in the second Pair.
 * @tparam Second2 The type of the second element in the second Pair.
 * @param lhs The first Pair to compare.
 * @param rhs The second Pair to compare.
 * @return true if the first Pair is less than or equal to the second Pair; false otherwise.
 * @note This function is only enabled if the first and second elements of both Pairs are comparable for ordering.
 */
template <class First1, class Second1, class First2, class Second2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator<=(const Pair<First1, Second1>& lhs, const Pair<First2, Second2>& rhs)
    requires requires {
        { rhs.first < lhs.first } -> BooleanTestable;
        { rhs.second < lhs.second } -> BooleanTestable;
    } {
    return !(rhs < lhs);
}

/**
 * @brief Compares two Pair objects for greater-than-or-equal-to ordering.
 * @tparam First1 The type of the first element in the first Pair.
 * @tparam Second1 The type of the second element in the first Pair.
 * @tparam First2 The type of the first element in the second Pair.
 * @tparam Second2 The type of the second element in the second Pair.
 * @param lhs The first Pair to compare.
 * @param rhs The second Pair to compare.
 * @return true if the first Pair is greater than or equal to the second Pair; false otherwise.
 * @note This function is only enabled if the first and second elements of both Pairs are comparable for ordering.
 */
template <class First1, class Second1, class First2, class Second2>
NEX_HIDDEN_FROM_ABI inline constexpr
bool operator>=(const Pair<First1, Second1>& lhs, const Pair<First2, Second2>& rhs)
    requires requires {
        { lhs.first < rhs.first } -> BooleanTestable;
        { lhs.second < rhs.second } -> BooleanTestable;
    } {
    return !(lhs < rhs);
}

/**
 * @brief Swaps the contents of two Pair objects.
 * @tparam First The type of the first element in the Pair.
 * @tparam Second The type of the second element in the Pair.
 * @param lhs The first Pair to swap.
 * @param rhs The second Pair to swap.
 * @note This function is only enabled if both the first and second elements of the Pairs are swappable.
 */
template <class First, class Second>
    requires(IsSwappableV<First> && IsSwappableV<Second>)
NEX_HIDDEN_FROM_ABI inline constexpr void swap(Pair<First, Second>& lhs, Pair<First, Second>& rhs)
    noexcept(IsNothrowSwappableV<First> && IsNothrowSwappableV<Second>) {
    lhs.swap(rhs);
}

/**
 * @brief Creates a Pair object from the provided values.
 * @tparam Type1 The type of the first value.
 * @tparam Type2 The type of the second value.
 * @param first The first value to store in the Pair.
 * @param second The second value to store in the Pair.
 * @return A Pair object containing the provided values.
 */
template <class Type1, class Type2>
NEX_HIDDEN_FROM_ABI constexpr auto makePair(Type1&& first, Type2&& second)
    noexcept(
        IsNothrowConstructibleV<UnwrapRefDecayT<Type1>, Type1&&> &&
        IsNothrowConstructibleV<UnwrapRefDecayT<Type2>, Type2&&>
    ) -> Pair<UnwrapRefDecayT<Type1>, UnwrapRefDecayT<Type2>>
{
    return Pair<UnwrapRefDecayT<Type1>, UnwrapRefDecayT<Type2>>(
        NEX_FORWARD<Type1>(first),
        NEX_FORWARD<Type2>(second)
    );
}

// Specialization of TupleElement for Pair, allowing access to the first and second elements by index.
// The index must be 0 or 1, corresponding to the first and second elements of the Pair.
template <usize Index, class First, class Second>
struct NEX_HIDDEN_FROM_ABI TupleElement<Index, Pair<First, Second>> {
    static_assert(Index < 2, "Error: Index out of bounds for Pair");
    using type NEX_NODEBUG = ConditionalT<Index == 0, First, Second>;
};

/**
 * @struct GetPairHelper
 * @brief Helper struct template for retrieving elements from a Pair by index.
 *        Specialized for Index 0 and 1 to access the first and second elements, respectively.
 * @tparam Index The index of the element to retrieve from the Pair. Must be 0 or 1.
 */
template <usize Index>
struct NEX_INTERNAL GetPairHelper {
    static_assert(Index < 2, "Error: Index out of bounds for Pair");
};

/**
 * @struct GetPairHelper<0>
 * @brief Specialization of GetPairHelper for Index 0, which retrieves the first element of a Pair.
 * @tparam First The type of the first element in the Pair.
 * @tparam Second The type of the second element in the Pair.
 */
template <>
struct NEX_INTERNAL GetPairHelper<0> {
    template <class First, class Second>
    static constexpr First& get(Pair<First, Second>& pair) noexcept {
        return pair.first;
    }

    template <class First, class Second>
    static constexpr const First& get(const Pair<First, Second>& pair) noexcept {
        return pair.first;
    }

    template <class First, class Second>
    static constexpr First&& get(Pair<First, Second>&& pair) noexcept {
        return NEX_FORWARD<First>(pair.first);
    }

    template <class First, class Second>
    static constexpr const First&& get(const Pair<First, Second>&& pair) noexcept {
        return NEX_FORWARD<const First>(pair.first);
    }
};

/**
 * @struct GetPairHelper<1>
 * @brief Specialization of GetPairHelper for Index 1, which retrieves the second element of a Pair.
 * @tparam First The type of the first element in the Pair.
 * @tparam Second The type of the second element in the Pair.
 */
template <>
struct NEX_INTERNAL GetPairHelper<1> {
    template <class First, class Second>
    static constexpr Second& get(Pair<First, Second>& pair) noexcept {
        return pair.second;
    }

    template <class First, class Second>
    static constexpr const Second& get(const Pair<First, Second>& pair) noexcept {
        return pair.second;
    }

    template <class First, class Second>
    static constexpr Second&& get(Pair<First, Second>&& pair) noexcept {
        return NEX_FORWARD<Second>(pair.second);
    }

    template <class First, class Second>
    static constexpr const Second&& get(const Pair<First, Second>&& pair) noexcept {
        return NEX_FORWARD<const Second>(pair.second);
    }
};

/**
 * @brief Retrieves a reference to the element at the specified index in a Pair.
 * @tparam Index The index of the element to retrieve (0 for first, 1 for second).
 * @tparam First The type of the first element in the Pair.
 * @tparam Second The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the element.
 * @return A reference to the element at the specified index.
 * @note This function is only enabled if the specified index is 0 or 1, corresponding to
 *       the first and second elements of the Pair.
 */
template <usize Index, class First, class Second>
    requires (Index < 2)
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
TupleElementT<Index, Pair<First, Second>>& get(Pair<First, Second>& pair) noexcept {
    return GetPairHelper<Index>::get(pair);
}

/**
 * @brief Retrieves a const reference to the element at the specified index in a Pair.
 * @tparam Index The index of the element to retrieve (0 for first, 1 for second).
 * @tparam First The type of the first element in the Pair.
 * @tparam Second The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the element.
 * @return A const reference to the element at the specified index.
 * @note This function is only enabled if the specified index is 0 or 1, corresponding to
 *       the first and second elements of the Pair.
 */
template <usize Index, class First, class Second>
    requires (Index < 2)
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
const TupleElementT<Index, Pair<First, Second>>& get(const Pair<First, Second>& pair) noexcept {
    return GetPairHelper<Index>::get(pair);
}

/**
 * @brief Retrieves an rvalue reference to the element at the specified index in a Pair.
 * @tparam Index The index of the element to retrieve (0 for first, 1 for second).
 * @tparam First The type of the first element in the Pair.
 * @tparam Second The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the element.
 * @return An rvalue reference to the element at the specified index.
 * @note This function is only enabled if the specified index is 0 or 1, corresponding to
 *       the first and second elements of the Pair.
 */
template <usize Index, class First, class Second>
    requires (Index < 2)
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
TupleElementT<Index, Pair<First, Second>>&& get(Pair<First, Second>&& pair) noexcept {
    return GetPairHelper<Index>::get(NEX_MOVE(pair));
}

/**
 * @brief Retrieves a const rvalue reference to the element at the specified index in a Pair.
 * @tparam Index The index of the element to retrieve (0 for first, 1 for second).
 * @tparam First The type of the first element in the Pair.
 * @tparam Second The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the element.
 * @return A const rvalue reference to the element at the specified index.
 * @note This function is only enabled if the specified index is 0 or 1, corresponding to
 *       the first and second elements of the Pair.
 */
template <usize Index, class First, class Second>
    requires (Index < 2)
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
const TupleElementT<Index, Pair<First, Second>>&& get(const Pair<First, Second>&& pair) noexcept {
    return GetPairHelper<Index>::get(NEX_MOVE(pair));
}

/**
 * @brief Retrieves a reference to the first element of a Pair.
 * @tparam Type1 The type of the first element in the Pair.
 * @tparam Type2 The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the first element.
 * @return A reference to the first element of the Pair.
 */
template <class Type1, class Type2>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr Type1& get(Pair<Type1, Type2>& pair) noexcept {
    return pair.first;
}

/**
 * @brief Retrieves a const reference to the first element of a Pair.
 * @tparam Type1 The type of the first element in the Pair.
 * @tparam Type2 The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the first element.
 * @return A const reference to the first element of the Pair.
 */
template <class Type1, class Type2>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr const Type1& get(const Pair<Type1, Type2>& pair) noexcept {
    return pair.first;
}

/**
 * @brief Retrieves an rvalue reference to the first element of a Pair.
 * @tparam Type1 The type of the first element in the Pair.
 * @tparam Type2 The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the first element.
 * @return An rvalue reference to the first element of the Pair.
 */
template <class Type1, class Type2>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr Type1&& get(Pair<Type1, Type2>&& pair) noexcept {
    return NEX_FORWARD<Type1>(pair.first);
}

/**
 * @brief Retrieves a const rvalue reference to the first element of a Pair.
 * @tparam Type1 The type of the first element in the Pair.
 * @tparam Type2 The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the first element.
 * @return A const rvalue reference to the first element of the Pair.
 */
template <class Type1, class Type2>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr const Type1&& get(const Pair<Type1, Type2>&& pair) noexcept {
    return NEX_FORWARD<const Type1>(pair.first);
}

/**
 * @brief Retrieves a reference to the second element of a Pair.
 * @tparam Type1 The type of the first element in the Pair.
 * @tparam Type2 The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the second element.
 * @return A reference to the second element of the Pair.
 */
template <class Type2, class Type1>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr Type2& get(Pair<Type1, Type2>& pair) noexcept {
    return pair.second;
}

/**
 * @brief Retrieves a const reference to the second element of a Pair.
 * @tparam Type1 The type of the first element in the Pair.
 * @tparam Type2 The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the second element.
 * @return A const reference to the second element of the Pair.
 */
template <class Type2, class Type1>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr const Type2& get(const Pair<Type1, Type2>& pair) noexcept {
    return pair.second;
}

/**
 * @brief Retrieves an rvalue reference to the second element of a Pair.
 * @tparam Type1 The type of the first element in the Pair.
 * @tparam Type2 The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the second element.
 * @return An rvalue reference to the second element of the Pair.
 */
template <class Type2, class Type1>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr Type2&& get(Pair<Type1, Type2>&& pair) noexcept {
    return NEX_FORWARD<Type2>(pair.second);
}

/**
 * @brief Retrieves a const rvalue reference to the second element of a Pair.
 * @tparam Type1 The type of the first element in the Pair.
 * @tparam Type2 The type of the second element in the Pair.
 * @param pair The Pair from which to retrieve the second element.
 * @return A const rvalue reference to the second element of the Pair.
 */
template <class Type2, class Type1>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr const Type2&& get(const Pair<Type1, Type2>&& pair) noexcept {
    return NEX_FORWARD<const Type2>(pair.second);
}

NEX_NAMESPACE_END
