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

NEX_NAMESPACE_BEGIN

// =================================================================================
// Declval utility for unevaluated contexts
// =================================================================================

/**
 * @brief Provides a way to obtain a value of type `Type` in unevaluated contexts.
 * @tparam Type The type for which to obtain a value.
 * @return A value of type `Type` in unevaluated contexts.
 * @note This function is intended for use in unevaluated contexts, such as `decltype`, and should not be used
 *       in evaluated contexts. Using it in evaluated contexts will result in a compile-time error.
 */
template <class Type>
NEX_HIDDEN_FROM_ABI decltype(meta::declval<Type&>()) declval() noexcept {
    static_assert(!meta::IsSameV<Type, Type>,
        "Error: declval() can only be used in an unevaluated context. "
        "It's likely that your current usage is trying to extract a value from the function.");
}

// =================================================================================
// Utilities for integral conversion and type manipulation
// =================================================================================

/**
 * @brief Converts a value of various integral types to a common integral type.
 * @tparam Type The type of the value to convert.
 * @param value The value to convert.
 * @return The converted integral value.
 * @note This function is overloaded for various integral types, including int32, uint32, int64, uint64,
 *       slong, ulong, longlong, ulonglong, and optionally int128 and uint128 if supported by the compiler.
 */

// Overloaded function for int32
NEX_HIDDEN_FROM_ABI inline constexpr int32 convertToIntegral(int32 value) noexcept { return value; }

// Overloaded function for uint32
NEX_HIDDEN_FROM_ABI inline constexpr uint32 convertToIntegral(uint32 value) noexcept { return value; }

// Overloaded function for int64
NEX_HIDDEN_FROM_ABI inline constexpr int64 convertToIntegral(int64 value) noexcept { return value; }

// Overloaded function for uint64
NEX_HIDDEN_FROM_ABI inline constexpr uint64 convertToIntegral(uint64 value) noexcept { return value; }

// Overloaded function for slong
NEX_HIDDEN_FROM_ABI inline constexpr slong convertToIntegral(slong value) noexcept { return value; }

// Overloaded function for ulong
NEX_HIDDEN_FROM_ABI inline constexpr ulong convertToIntegral(ulong value) noexcept { return value; }

// Overloaded function for longlong
NEX_HIDDEN_FROM_ABI inline constexpr longlong convertToIntegral(longlong value) noexcept { return value; }

// Overloaded function for ulonglong
NEX_HIDDEN_FROM_ABI inline constexpr ulonglong convertToIntegral(ulonglong value) noexcept { return value; }

#if NEX_HAS_BUILTIN_INT128
    // Overloaded function for int128
    NEX_HIDDEN_FROM_ABI inline constexpr int128 convertToIntegral(int128 value) noexcept { return value; }

    // Overloaded function for uint128
    NEX_HIDDEN_FROM_ABI inline constexpr uint128 convertToIntegral(uint128 value) noexcept { return value; }
#endif  // ^^NEX_HAS_BUILTIN_INT128

/**
 * @brief Converts a floating-point value to an integral type.
 * @tparam Floating The floating-point type of the value to convert.
 * @param value The floating-point value to convert.
 * @return The converted integral value (as longlong).
 * @note This function is only enabled for floating-point types and performs a static cast to longlong.
 */
template <meta::FloatingPoint Floating>
NEX_HIDDEN_FROM_ABI inline constexpr longlong convertToIntegral(Floating value) {
    return static_cast<longlong>(value);
}

// Helper struct to conditionally provide the underlying type of an enumeration type.
template <class Type, bool = meta::IsEnumV<Type>>
struct NEX_HIDDEN_FROM_ABI SFINAEUnderlyingType {
    using type = meta::UnderlyingTypeT<Type>;
    using promoted_type = decltype(((type)1) + 0);
};

// Specialization of SFINAEUnderlyingType for non-enumeration types, which does not provide a type member.
template <class Type>
struct NEX_HIDDEN_FROM_ABI SFINAEUnderlyingType<Type, false> {};

/**
 * @brief Converts an enumeration value to its underlying integral type.
 * @tparam Type The enumeration type of the value to convert.
 * @param value The enumeration value to convert.
 * @return The converted integral value (as the underlying type of the enumeration).
 * @note This function is only enabled for enumeration types and uses `meta::UnderlyingTypeT` to determine
 *       the underlying integral type of the enumeration.
 */
template <class Type>
inline NEX_HIDDEN_FROM_ABI constexpr
typename SFINAEUnderlyingType<Type>::promoted_type convertToIntegral(Type value) {
    return value;
}

// =================================================================================
// Traits to retrieve the extent of an array type at a specified dimension
// =================================================================================

#if NEX_HAS_BUILTIN(__array_extent)

    // Retrieve the extent (size) of an array type at a specified dimension.
    template <class Type, usize Dimension = 0>
    struct Extent : meta::IntegralConstant<usize, __array_extent(Type, Dimension)> {};

    // Convenience variable template to retrieve the extent of an array type at a specified dimension.
    template <class Type, unsigned Dimension = 0>
    inline constexpr usize ExtentV = __array_extent(Type, Dimension);

#else

    // Primary template for Extent, which defaults to 0 for non-array types.
    template <class Type, unsigned Dimension = 0>
    struct Extent : public meta::IntegralConstant<usize, 0> {};

    // Specialization of Extent for unbounded array types (Type[]), which retrieves the extent recursively.
    template <class Type>
    struct Extent<Type[], 0> : public meta::IntegralConstant<usize, 0> {};

    // Specialization of Extent for unbounded array types (Type[]) at a specified dimension,
    // which retrieves the extent recursively.
    template <class Type, unsigned Dimension>
    struct Extent<Type[], Dimension>
        : public meta::IntegralConstant<usize, Extent<Type, Dimension - 1>::value> {};

    // Specialization of Extent for bounded array types (Type[N]), which retrieves the extent at dimension 0.
    template <class Type, usize N>
    struct Extent<Type[N], 0> : public meta::IntegralConstant<usize, N> {};

    // Specialization of Extent for bounded array types (Type[N]) at a specified dimension,
    // which retrieves the extent recursively.
    template <class Type, usize N, unsigned Dimension>
    struct Extent<Type[N], Dimension>
        : public meta::IntegralConstant<usize, Extent<Type, Dimension - 1>::value> {};

    // Convenience variable template to retrieve the extent of an array type at a specified dimension.
    template <class Type, unsigned Dimension = 0>
    inline constexpr usize ExtentV = Extent<Type, Dimension>::value;

#endif // ^^NEX_HAS_BUILTIN(__array_extent)

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

template <usize Index, class Type>
struct TupleElement<Index, const Type> {
    using type = const typename TupleElement<Index, Type>::type;
};

template <usize Index, class Type>
struct TupleElement<Index, volatile Type> {
    using type = volatile typename TupleElement<Index, Type>::type;
};

template <usize Index, class Type>
struct TupleElement<Index, const volatile Type> {
    using type = const volatile typename TupleElement<Index, Type>::type;
};

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
typename TupleElement<Index, Pair<First, Second>>::type& get(Pair<First, Second>&) noexcept;

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
const typename TupleElement<Index, Pair<First, Second>>::type& get(const Pair<First, Second>&) noexcept;

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
typename TupleElement<Index, Pair<First, Second>>::type&& get(Pair<First, Second>&&) noexcept;

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
const typename TupleElement<Index, Pair<First, Second>>::type&& get(const Pair<First, Second>&&) noexcept;

// =================================================================================
// Forward declaration of the Tuple class template and related utilities
// =================================================================================

// Forward declaration of the Tuple class template,
// which represents a fixed-size collection of heterogeneous values.
template <class...>
struct Tuple;

// Forward declaration of the TupleSize struct template, which provides the size of a Tuple.
template <class>
struct TupleSize;

/**
 * @brief Retrieves a reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_HIDDEN_FROM_ABI constexpr
typename TupleElement<Index, Tuple<Types...>>::type& get(Tuple<Types...>&) noexcept;

/**
 * @brief Retrieves a const reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A const reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_HIDDEN_FROM_ABI constexpr
const typename TupleElement<Index, Tuple<Types...>>::type& get(const Tuple<Types...>&) noexcept;

/**
 * @brief Retrieves an rvalue reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return An rvalue reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_HIDDEN_FROM_ABI constexpr
typename TupleElement<Index, Tuple<Types...>>::type&& get(Tuple<Types...>&&) noexcept;

/**
 * @brief Retrieves a const rvalue reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A const rvalue reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_HIDDEN_FROM_ABI constexpr
const typename TupleElement<Index, Tuple<Types...>>::type&& get(const Tuple<Types...>&&) noexcept;

// =================================================================================
// Exchange for replacing the value of an object and returning the old value
// =================================================================================

/**
 * @brief Replaces the value of an object with a new value and returns the old value.
 * @tparam Type1 The type of the object to exchange.
 * @tparam Type2 The type of the new value to assign to the object.
 * @param obj The object whose value is to be replaced.
 * @param newValue The new value to assign to the object.
 * @return The old value of the object before the exchange.
 * @note This function is noexcept if both Type1 is nothrow move constructible
 *       and Type1 is nothrow assignable from Type2.
 */
template <class Type1, class Type2 = Type1>
inline NEX_HIDDEN_FROM_ABI constexpr Type1 exchange(Type1& obj, Type2&& newValue)
    noexcept(meta::IsNothrowMoveConstructibleV<Type1> && meta::IsNothrowAssignableV<Type1&, Type2>) {
    Type1 oldValue = NEX_MOVE(obj);
    obj            = NEX_FORWARD<Type2>(newValue);
    return oldValue;
}

// Replaces the value of an object with a new value and returns the old value.
#define NEX_EXCHANGE(obj, newValue) \
    NEX_PREPEND_NAMESPACE(exchange(obj, newValue))

// =================================================================================
// Swap functions and related type traits for determining swappability of types
// =================================================================================

// Determines if two types are swappable with each other.
// By default, this trait evaluates to false for all types.
template <class Type1, class Type2, class = void>
inline const bool IsSwappableWithV = false;

// Internal trait to determine if a type is swappable with itself.
template <class Type>
inline const bool _IsSwappableV = IsSwappableWithV<Type&, Type&>;

// Determines if two types are nothrow swappable with each other.
// By default, this trait evaluates to false for all types.
template <class Type1, class Type2, bool = IsSwappableWithV<Type1, Type2> >
inline const bool IsNothrowSwappableWithV = false;

// Internal trait to determine if a type is nothrow swappable with itself.
template <class Type>
inline const bool _IsNothrowSwappableV = IsNothrowSwappableWithV<Type&, Type&>;

// Represents the result type for the swap function,
// which is only enabled if the type is move constructible and move assignable.
template <class Type>
using SwapResultT NEX_NODEBUG =
    meta::EnableIfT<meta::IsMoveConstructibleV<Type> && meta::IsMoveAssignableV<Type>>;

// Forward declaration of the swap function for a type.
template <class Type>
    requires (_IsSwappableV<Type>)
inline NEX_HIDDEN_FROM_ABI constexpr SwapResultT<Type> swap(Type& x, Type& y)
    noexcept(meta::IsNothrowMoveConstructibleV<Type>&& meta::IsNothrowMoveAssignableV<Type>);

// Forward declaration of the swap function for arrays of a given type and size.
template <class Type, usize Count>
    requires (Count > 0 && _IsSwappableV<Type>)
inline NEX_HIDDEN_FROM_ABI constexpr void swap(Type (&a)[Count], Type (&b)[Count])
    noexcept(_IsNothrowSwappableV<Type>);

// Specialization of the IsSwappableWithV trait for types that can be swapped with each other.
template <class Type1, class Type2>
inline const bool IsSwappableWithV<
        Type1,
        Type2,
        meta::VoidT<
            decltype(swap(declval<Type1>(), declval<Type2>())),
            decltype(swap(declval<Type2>(), declval<Type1>()))
        >
    > = true;

// Specialization of the IsNothrowSwappableWithV trait for types that can be nothrow swapped with each other.
template <class Type1, class Type2>
inline const bool IsNothrowSwappableWithV<Type1, Type2, true> =
    noexcept(swap(declval<Type1>(), declval<Type2>())) &&
    noexcept(swap(declval<Type2>(), declval<Type1>()));

// Determines if two types are swappable with each other.
template <class Type1, class Type2>
struct IsSwappableWith : meta::BoolConstant<IsSwappableWithV<Type1, Type2>> {};

// Determines if a type is swappable with itself.
template <class Type>
inline constexpr bool IsSwappableV =
    IsSwappableWithV<meta::AddLvalueReferenceT<Type>, meta::AddLvalueReferenceT<Type>>;

// Determines if a type is nothrow swappable with itself.
template <class Type>
struct IsSwappable : meta::BoolConstant<IsSwappableV<Type>> {};

// Determines if two types are nothrow swappable with each other.
template <class Type1, class Type2>
struct IsNothrowSwappableWith : meta::BoolConstant<IsNothrowSwappableWithV<Type1, Type2>> {};

// Determines if a type is nothrow swappable with itself.
template <class Type>
inline constexpr bool IsNothrowSwappableV =
    IsNothrowSwappableWithV<meta::AddLvalueReferenceT<Type>, meta::AddLvalueReferenceT<Type>>;

// Determines if a type is nothrow swappable with itself.
template <class Type>
struct IsNothrowSwappable : meta::BoolConstant<IsNothrowSwappableV<Type>> {};

/**
 * @brief Swaps the values of two objects of the same type.
 * @tparam Type The type of the objects to swap.
 * @param x The first object to swap.
 * @param y The second object to swap.
 * @note This function is only enabled if the type is swappable.
 *       It is noexcept if the type is nothrow move constructible and nothrow move assignable.
 * @see IsSwappable, IsNothrowSwappable, IsSwappableWith, IsNothrowSwappableWith
 */
template <class Type>
    requires (_IsSwappableV<Type>)
inline NEX_HIDDEN_FROM_ABI constexpr SwapResultT<Type> swap(Type& x, Type& y)
    noexcept(meta::IsNothrowMoveConstructibleV<Type>&& meta::IsNothrowMoveAssignableV<Type>) {
    Type tmp(NEX_MOVE(x));
    x = NEX_MOVE(y);
    y = NEX_MOVE(tmp);
}

/**
 * @brief Swaps the values of two arrays of the same type and size.
 * @tparam Type The type of the elements in the arrays.
 * @tparam Count The number of elements in the arrays (must be greater than 0).
 * @param a The first array to swap.
 * @param b The second array to swap.
 * @note This function is only enabled if the element type is swappable.
 *       It is noexcept if the element type is nothrow swappable.
 * @see IsSwappable, IsNothrowSwappable, IsSwappableWith, IsNothrowSwappableWith
 */
template <class Type, usize Count>
    requires (Count > 0 && _IsSwappableV<Type>)
inline NEX_HIDDEN_FROM_ABI constexpr void swap(Type (&a)[Count], Type (&b)[Count])
    noexcept(_IsNothrowSwappableV<Type>) {
    for (usize i = 0; i < Count; ++i) {
        swap(a[i], b[i]);
    }
}

// Swaps the values of two objects of the same type,
// or two arrays of the same type and size, if the type is swappable.
#define NEX_SWAP \
    NEX_PREPEND_NAMESPACE(swap)

// Internal implementation of the AssignableFrom concept,
// which determines if a type `To` can be assigned from a type `From`.
template <typename To, typename From>
concept _AssignableFrom =
    meta::IsLvalueReferenceV<To> &&
    meta::CommonReferenceWith<
        const meta::RemoveReferenceT<To>&,
        const meta::RemoveReferenceT<From>&> &&
    requires(To lhs, From&& rhs) {
        { lhs = NEX_FORWARD<From>(rhs) } -> meta::SameAs<To>;
    };

NEX_SUBNAMESPACE_BEGIN(ranges)

NEX_SUBNAMESPACE_BEGIN(_swap)

template <class Type>
void swap(Type&, Type&) = delete;

// Determines if two types can be swapped with each other using unqualified swap.
template <class Type1, class Type2>
concept UnqualifiedSwappableWith =
    (meta::ClassOrEnum<meta::RemoveCvrefT<Type1>> || meta::ClassOrEnum<meta::RemoveCvrefT<Type2>>) &&
    requires(Type1&& t, Type2&& u) {
        swap(NEX_FORWARD<Type1>(t), NEX_FORWARD<Type2>(u));
    };

struct Fn;

// Determines if two array types can be swapped with each other using a custom swap function.
template <class Type1, class Type2, usize Size>
concept SwappableArrays =
    !UnqualifiedSwappableWith<Type1 (&)[Size], Type2 (&)[Size]> &&
    ExtentV<Type1> == ExtentV<Type2> &&
    requires(Type1 (&t)[Size], Type2 (&u)[Size], const Fn& swapFn) {
        swapFn(t[0], u[0]);
    };

// Determines if a type is exchangeable,
// meaning it can be swapped with itself using the exchange function.
template <class Type>
concept Exchangeable =
    !UnqualifiedSwappableWith<Type&, Type&> &&
    meta::MoveConstructible<Type> &&
    meta::IsAssignableV<Type&, Type>;

struct Fn {
    // `S` is `(void)swap(E1, E2)`* if `E1` or `E2` has class or enumeration type and...
    // *The name `swap` is used here unqualified.
    template <class Type1, class Type2>
        requires UnqualifiedSwappableWith<Type1, Type2>
    NEX_HIDDEN_FROM_ABI constexpr void operator()(Type1&& x, Type2&& y) const
        noexcept(noexcept(swap(NEX_FORWARD<Type1>(x), NEX_FORWARD<Type2>(y)))) {
        swap(NEX_FORWARD<Type1>(x), NEX_FORWARD<Type2>(y));
    }

    // Otherwise, if `E1` and `E2` are lvalues of array types with equal extent and...
    template <class Type1, class Type2, usize Size>
        requires SwappableArrays<Type1, Type2, Size>
    NEX_HIDDEN_FROM_ABI constexpr void operator()(Type1 (&arr1)[Size], Type2 (&arr2)[Size]) const
        noexcept(noexcept((*this)(*arr1, *arr2))) {
        for (size_t __i = 0; __i < Size; ++__i) {
            (*this)(arr1[__i], arr2[__i]);
        }
    }

    // Otherwise, if `E1` and `E2` are lvalues of the same type `T` that models...
    template <Exchangeable Type>
    NEX_HIDDEN_FROM_ABI constexpr void operator()(Type& x, Type& y) const
        noexcept(meta::IsNothrowMoveConstructibleV<Type> && meta::IsNothrowMoveAssignableV<Type>) {
        y = NEX_EXCHANGE(x, NEX_MOVE(y));
    }
};

NEX_SUBNAMESPACE_END(_swap)

/**
 * @brief Swaps the values of two objects of the same type, or two arrays of the same type and size.
 * @tparam Type The type of the objects or arrays to swap.
 * @param x The first object or array to swap.
 * @param y The second object or array to swap.
 * @note This function is only enabled if the type is swappable.
 *       It is noexcept if the type is nothrow move constructible and nothrow move assignable.
 * @see IsSwappable, IsNothrowSwappable, IsSwappableWith, IsNothrowSwappableWith
 */
inline constexpr auto swap = _swap::Fn{};

NEX_SUBNAMESPACE_END(ranges)

NEX_NAMESPACE_END
