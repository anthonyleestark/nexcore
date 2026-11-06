/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// Forward declarations for functional classes
// =================================================================================

// Forward declaration of the Reference class template,
// which is a wrapper that makes references "assignable" and "copyable".
template <class>
class Reference;

// Forward declaration for the Hash struct template, which provides a hash function for a given type.
template <class>
struct Hash;

// =================================================================================
// Forward declarations of Array class and related utilities
// =================================================================================

// Default capacity for Array if not specified
inline constexpr usize DefaultArrayCapacity = 8;

// Forward declaration of the Array class template with a specified element type and capacity.
// The capacity must be greater than 0.
template <
    typename ElementType,
    usize Capacity = DefaultArrayCapacity
> requires (Capacity > 0)
class Array;

/**
 * @brief Retrieves a reference to the element at the specified index in an Array.
 * @tparam Index The index of the element to retrieve.
 * @tparam ElementType The type of elements stored in the Array.
 * @tparam Capacity The capacity of the Array (must be greater than 0).
 * @param array The Array from which to retrieve the element.
 * @return A reference to the element at the specified index.
 * @note This function is only enabled if the specified index is within the bounds of the Array's capacity.
 */
template <usize Index, typename ElementType, usize Capacity>
    requires (Capacity > 0 && Index < Capacity)
NEX_HIDDEN_FROM_ABI constexpr ElementType& get(Array<ElementType, Capacity>&) noexcept;

/**
 * @brief Retrieves a const reference to the element at the specified index in an Array.
 * @tparam Index The index of the element to retrieve.
 * @tparam ElementType The type of elements stored in the Array.
 * @tparam Capacity The capacity of the Array (must be greater than 0).
 * @param array The Array from which to retrieve the element.
 * @return A const reference to the element at the specified index.
 * @note This function is only enabled if the specified index is within the bounds of the Array's capacity.
 */
template <usize Index, typename ElementType, usize Capacity>
    requires (Capacity > 0 && Index < Capacity)
NEX_HIDDEN_FROM_ABI constexpr const ElementType& get(const Array<ElementType, Capacity>&) noexcept;

/**
 * @brief Retrieves an rvalue reference to the element at the specified index in an Array.
 * @tparam Index The index of the element to retrieve.
 * @tparam ElementType The type of elements stored in the Array.
 * @tparam Capacity The capacity of the Array (must be greater than 0).
 * @param array The Array from which to retrieve the element.
 * @return An rvalue reference to the element at the specified index.
 * @note This function is only enabled if the specified index is within the bounds of the Array's capacity.
 */
template <usize Index, typename ElementType, usize Capacity>
    requires (Capacity > 0 && Index < Capacity)
NEX_HIDDEN_FROM_ABI constexpr ElementType&& get(Array<ElementType, Capacity>&&) noexcept;

/**
 * @brief Retrieves a const rvalue reference to the element at the specified index in an Array.
 * @tparam Index The index of the element to retrieve.
 * @tparam ElementType The type of elements stored in the Array.
 * @tparam Capacity The capacity of the Array (must be greater than 0).
 * @param array The Array from which to retrieve the element.
 * @return A const rvalue reference to the element at the specified index.
 * @note This function is only enabled if the specified index is within the bounds of the Array's capacity.
 */
template <usize Index, typename ElementType, usize Capacity>
    requires (Capacity > 0 && Index < Capacity)
NEX_HIDDEN_FROM_ABI constexpr const ElementType&& get(const Array<ElementType, Capacity>&&) noexcept;

// Determines if a given type is an instance of the Array class template.
// By default, this trait evaluates to false for all types.
template <class>
inline NEX_HIDDEN_FROM_ABI constexpr bool IsArrayClassV = false;

// Specialization of the IsArrayClassV trait for the Array class template, which evaluates to true.
template <typename ElementType, usize Capacity>
inline NEX_HIDDEN_FROM_ABI constexpr bool IsArrayClassV<Array<ElementType, Capacity>> = true;

// =================================================================================
// TupleElement template for accessing elements of an Array / Pair / Tuple by index
// =================================================================================

// Primary template for the TupleElement struct.
template <usize Index, class Type>
struct TupleElement;

// Specialization of TupleElement for const-qualified types.
template <usize Index, class Type>
struct TupleElement<Index, const Type> {
    using type = const typename TupleElement<Index, Type>::type;
};

// Specialization of TupleElement for volatile-qualified types.
template <usize Index, class Type>
struct TupleElement<Index, volatile Type> {
    using type = volatile typename TupleElement<Index, Type>::type;
};

// Specialization of TupleElement for const-volatile-qualified types.
template <usize Index, class Type>
struct TupleElement<Index, const volatile Type> {
    using type = const volatile typename TupleElement<Index, Type>::type;
};

// Trait alias to retrieve the type of the element at a specified index in a Tuple.
template <usize Index, class... Types>
using TupleElementT = typename TupleElement<Index, Types...>::type;

// =================================================================================
// Forward declaration of the Pair class template and related utilities
// =================================================================================

// Forward declaration of the Pair class template, which represents a pair of values.
template <class, class>
struct Pair;

// Determines if a given type is an instance of the Pair class template.
// By default, this trait evaluates to false for all types.
template <class>
inline NEX_HIDDEN_FROM_ABI constexpr bool IsPairV = false;

// Specialization of the IsPairV trait for the Pair class template, which evaluates to true.
template <typename First, typename Second>
inline NEX_HIDDEN_FROM_ABI constexpr bool IsPairV<Pair<First, Second>> = true;

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
NEX_HIDDEN_FROM_ABI constexpr
TupleElementT<Index, Pair<First, Second>>& get(Pair<First, Second>&) noexcept;

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
NEX_HIDDEN_FROM_ABI constexpr
const TupleElementT<Index, Pair<First, Second>>& get(const Pair<First, Second>&) noexcept;

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
NEX_HIDDEN_FROM_ABI constexpr
TupleElementT<Index, Pair<First, Second>>&& get(Pair<First, Second>&&) noexcept;

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
NEX_HIDDEN_FROM_ABI constexpr
const TupleElementT<Index, Pair<First, Second>>&& get(const Pair<First, Second>&&) noexcept;

// =================================================================================
// Forward declaration of the Tuple class template and related utilities
// =================================================================================

// Forward declaration of the Tuple class template,
// which represents a fixed-size collection of heterogeneous values.
template <class...>
struct Tuple;

// Forward declaration of the TupleSize struct template,
// which provides the size of a Tuple.
template <class>
struct TupleSize;

// Forward declaration of the TupleTypes struct template,
// which provides the types of the elements in a Tuple.
template <class... Types>
struct TupleTypes;

/**
 * @brief Retrieves a reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_HIDDEN_FROM_ABI constexpr
TupleElementT<Index, Tuple<Types...>>& get(Tuple<Types...>&) noexcept;

/**
 * @brief Retrieves a const reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A const reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_HIDDEN_FROM_ABI constexpr
const TupleElementT<Index, Tuple<Types...>>& get(const Tuple<Types...>&) noexcept;

/**
 * @brief Retrieves an rvalue reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return An rvalue reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_HIDDEN_FROM_ABI constexpr
TupleElementT<Index, Tuple<Types...>>&& get(Tuple<Types...>&&) noexcept;

/**
 * @brief Retrieves a const rvalue reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A const rvalue reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_HIDDEN_FROM_ABI constexpr
const TupleElementT<Index, Tuple<Types...>>&& get(const Tuple<Types...>&&) noexcept;

// =================================================================================
// Forward declarations for string-related types.
// =================================================================================

// Forward declaration of the String class.
class String;

// Forward declaration of the StringView class.
class StringView;

NEX_NAMESPACE_END
