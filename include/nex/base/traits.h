/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <type_traits>
#include <iterator>

#include "nex/base/macros.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @typedef IsArithmetic
 * @brief A type trait that checks if a type Type is an arithmetic type (i.e., integral or floating-point).
 */
template <class Type>
using IsArithmetic = NEX_STD is_arithmetic<Type>;

/**
 * @typedef IsIntegral
 * @brief A type trait that checks if a type Type is an integral type (e.g., int, char, etc.).
 */
template <class Type>
using IsIntegral = NEX_STD is_integral<Type>;

/**
 * @typedef IsFloatingPoint
 * @brief A type trait that checks if a type Type is a floating-point type (i.e., float, double, or long double).
 */
template <class Type>
using IsFloatingPoint = NEX_STD is_floating_point<Type>;

/**
 * @typedef IsPointer
 * @brief A type trait that checks if a type Type is a pointer type.
 */
template <typename Type>
using IsPointer = NEX_STD is_pointer<Type>;

/**
 * @typedef IsArray
 * @brief A type trait that checks if a type Type is an array type.
 */
template <typename Type>
using IsArray = NEX_STD is_array<Type>;

/**
 * @typedef IsSame
 * @brief A type trait that checks if two types Type1 and Type2 are the same type.
 */
template <typename Type1, typename Type2>
using IsSame = NEX_STD is_same<Type1, Type2>;

/**
 * @typedef IsBaseOf
 * @brief A type trait that checks if a type Base is a base class of a type Derived.
 */
template <typename Base, typename Derived>
using IsBaseOf = NEX_STD is_base_of<Base, Derived>;

/**
 * @typedef IsConstructible
 * @brief A type trait that checks if a type Type can be constructed with the specified argument types Args.
 */
template <typename Type, typename... Args>
using IsConstructible = NEX_STD is_constructible<Type, Args...>;

/**
 * @typedef IsCopyable
 * @brief A type trait that checks if a type Type is copy constructible.
 */
template <typename Type>
using IsCopyable = NEX_STD is_copy_constructible<Type>;

/**
 * @typedef IsMoveable
 * @brief A type trait that checks if a type Type is move constructible.
 */
template <typename Type>
using IsMoveable = NEX_STD is_move_constructible<Type>;

/**
 * @typedef RemoveConst
 * @brief A type alias that removes the const qualifier from a type Type.
 */
template <typename Type>
using RemoveConst = typename NEX_STD remove_const<Type>::type;

/**
 * @typedef RemoveCv
 * @brief A type alias that removes const and volatile qualifiers from a type Type.
 */
template <typename Type>
using RemoveCv = NEX_STD remove_cv_t<Type>;

/**
 * @typedef RemoveReference
 * @brief A type alias that removes reference qualifiers from a type Type.
 */
template <typename Type>
using RemoveReference = NEX_STD remove_reference_t<Type>;

/**
 * @typedef RemoveCvref
 * @brief A type alias that removes const, volatile, and reference qualifiers from a type Type.
 */
template <typename Type>
using RemoveCvref = RemoveCv<RemoveReference<Type>>;

/**
 * @typedef Decay
 * @brief A type alias for the decayed type of a given type Type.
 */
template <typename Type>
using Decay = NEX_STD decay_t<Type>;

/**
 * @typedef EnableIf
 * @brief A type trait that enables a template only if a boolean condition is true.
 */
template <bool Condition, typename Type = void>
using EnableIf = typename NEX_STD enable_if<Condition, Type>::type;

/**
 * @typedef RequiresNumeric
 * @brief A type trait that enables a template only if the type Type is a numeric type (arithmetic type).
 */
template <typename Type>
using RequiresNumeric = EnableIf<IsArithmetic<RemoveCvref<Type>>>;

/**
 * @typedef RequiresIntegral
 * @brief A type trait that enables a template only if the type Type is an integral type.
 */
template <typename Type>
using RequiresIntegral = EnableIf<IsIntegral<RemoveCvref<Type>>>;

/**
 * @typedef RequiresFloatingPoint
 * @brief A type trait that enables a template only if the type Type is a floating-point type.
 */
template <typename Type>
using RequiresFloatingPoint = EnableIf<IsFloatingPoint<RemoveCvref<Type>>>;

/**
 * @typedef RequiresPointer
 * @brief A type trait that enables a template only if the type Type is a pointer type.
 */
template <typename Type>
using RequiresPointer = EnableIf<IsPointer<RemoveCvref<Type>>>;

/**
 * @typedef InvokeResult
 * @brief A type alias for the result type of invoking a callable with specific arguments.
 */
template<typename Fn, typename... Args>
using InvokeResult = NEX_STD invoke_result_t<Fn, Args...>;

/**
 * @typedef ResolvedReturnType
 * @brief A type alias for the resolved return type of invoking a callable with specific arguments.
 */
template <typename Fn, typename... Args>
using ResolvedReturnType = Decay<InvokeResult<Fn, Args...>>;

/**
 * @typedef Element
 * @brief A type alias that extracts the element type from a container type Type.
 * @note The type Type must have a nested value_type typedef, which is common for standard library containers.
 */
template <typename Type>
using Element = typename RemoveCvref<Type>::value_type;

/**
 * @typedef MemberType
 * @brief A type trait that extracts the type of a member from a pointer to member.
 */
template <typename Type, typename MemberT>
using MemberType = MemberT;

/**
 * @typedef MemberPointer
 * @brief A type trait that represents a pointer to a member within a struct/class.
 */
template <typename Type, typename MemberT>
using MemberPointer = MemberT Type::*;

/**
 * @typedef MemberReference
 * @brief A type trait that represents a reference to a member within a struct/class.
 */
template <typename Type, auto MemberPtr>
using MemberReference = decltype(NEX_STD declval<Type&>().*MemberPtr);

/**
 * @typedef ConstMemberReference
 * @brief A type trait that represents a const reference to a member within a struct/class.
 */
template <typename Type, auto MemberPtr>
using ConstMemberReference = decltype(NEX_STD declval<const Type&>().*MemberPtr);

/**
 * @typedef MemberRvalueReference
 * @brief A type trait that represents an rvalue reference to a member within a struct/class.
 */
template <typename Type, auto MemberPtr>
using MemberRvalueReference = decltype(NEX_STD declval<Type&&>().*MemberPtr);

/**
 * @fn IsCallable
 * @brief A type trait that checks if a type Type is callable with specific argument types.
 */
template <typename Type, typename... Args>
inline constexpr bool IsCallable = NEX_STD is_invocable_v<Type, Args...>;

/**
 * @struct IsIterable
 * @brief A type trait that checks if a type Type is iterable (i.e., has begin() and end() functions).
 */
template <typename Type, typename = void>
struct IsIterable : NEX_STD false_type {};

/**
 * @struct IsIterable (specialization)
 * @brief A specialization of IsIterable that checks if a type Type has valid begin() and end() functions, 
 *        indicating that it is iterable.
 */
template <typename Type>
struct IsIterable<
    Type, 
    NEX_STD void_t<
        decltype(NEX_STD begin(NEX_STD declval<Type&>())), 
        decltype(NEX_STD end(NEX_STD declval<Type&>()))
    >
> : NEX_STD true_type {};

// Forward declaration of FunctionTraits for function type introspection
template <typename Type>
struct FunctionTraits;

/**
 * @struct FunctionTraits
 * @brief A type trait that extracts information about a function type, such as its return type 
 *        and argument types.
 */
template <typename R, typename... Args>
struct FunctionTraits<R(*)(Args...)> {
    using ReturnType = R;
    static constexpr usize argCount = sizeof...(Args);
    
    template <usize N>
    using Arg = NEX_STD tuple_element_t<N, NEX_STD tuple<Args...>>;
};

NEX_NAMESPACE_END