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
 * @typedef EnableIf
 * @brief A type trait that enables a template only if a boolean condition is true.
 */
template <bool B, typename T = void>
using EnableIf = typename NEX_STD enable_if<B, T>::type;

/**
 * @typedef requires_numeric
 * @brief A type trait that enables a template only if the type T is a numeric type (arithmetic type).
 */
template <typename T>
using RequiresNumeric = EnableIf<NEX_STD is_arithmetic_v<remove_cvref_t<T>>>;

/**
 * @typedef Decay
 * @brief A type alias for the decayed type of a given type T.
 */
template <typename T>
using Decay = NEX_STD decay_t<T>;

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
 * @typedef RemoveCvref
 * @brief A type alias that removes const, volatile, and reference qualifiers from a type T.
 */
template <typename T>
using RemoveCvref = NEX_STD remove_cv_t<NEX_STD remove_reference_t<T>>;

/**
 * @typedef Element
 * @brief A type alias that extracts the element type from a container type T.
 * @note The type T must have a nested value_type typedef, which is common for standard library containers.
 */
template <typename T>
using Element = typename RemoveCvref<T>::value_type;

/**
 * @typedef MemberType
 * @brief A type trait that extracts the type of a member from a pointer to member.
 */
template <typename T, typename MemberT>
using MemberType = MemberT;

/**
 * @typedef MemberPointer
 * @brief A type trait that represents a pointer to a member within a struct/class.
 */
template <typename T, typename MemberT>
using MemberPointer = MemberT T::*;

/**
 * @typedef MemberReference
 * @brief A type trait that represents a reference to a member within a struct/class.
 */
template <typename T, auto MemberPtr>
using MemberReference = decltype(NEX_STD declval<T&>().*MemberPtr);

/**
 * @typedef ConstMemberReference
 * @brief A type trait that represents a const reference to a member within a struct/class.
 */
template <typename T, auto MemberPtr>
using ConstMemberReference = decltype(NEX_STD declval<const T&>().*MemberPtr);

/**
 * @typedef MemberRvalueReference
 * @brief A type trait that represents an rvalue reference to a member within a struct/class.
 */
template <typename T, auto MemberPtr>
using MemberRvalueReference = decltype(NEX_STD declval<T&&>().*MemberPtr);

/**
 * @fn IsCallable
 * @brief A type trait that checks if a type T is callable with specific argument types.
 */
template <typename T, typename... Args>
inline constexpr bool IsCallable = NEX_STD is_invocable_v<T, Args...>;

/**
 * @struct IsIterable
 * @brief A type trait that checks if a type T is iterable (i.e., has begin() and end() functions).
 */
template <typename T, typename = void>
struct IsIterable : NEX_STD false_type {};

/**
 * @struct IsIterable (specialization)
 * @brief A specialization of IsIterable that checks if a type T has valid begin() and end() functions, 
 *        indicating that it is iterable.
 */
template <typename T>
struct IsIterable<T, NEX_STD void_t<decltype(NEX_STD begin(NEX_STD declval<T&>())), 
                                    decltype(NEX_STD end(NEX_STD declval<T&>()))>> 
    : NEX_STD true_type {};

// Forward declaration of FunctionTraits for function type introspection
template <typename T>
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