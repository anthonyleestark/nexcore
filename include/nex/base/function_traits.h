/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/fwd.h"
#include "nex/base/casts.h"
#include "nex/base/invoke.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Extracts information about a function type, such as its return type and argument types.
 * @tparam Callable The function type or callable object to extract traits from.
 * @details
 * This template can be used to deduce the return type and argument types of various kinds of functions,
 * including regular functions, function pointers, member function pointers, and callable objects like lambdas.
 * It provides a uniform interface to access the return type and argument types of these functions.
 */
template <typename Callable>
struct FunctionTraits : FunctionTraits<decltype(&meta::RemoveReference<Callable>::operator())> {};

/**
 * @brief Specialization of FunctionTraits for regular function types
 * that extracts information about their return type and argument types.
 * @tparam Ret The return type of the function.
 * @tparam Args The argument types of the function.
 */
template <typename Ret, typename... Args>
struct FunctionTraits<Ret(Args...)> {
    using ReturnType = Ret;
    static constexpr usize argCount = sizeof...(Args);

    /**
     * @brief Represents the type of the N-th argument of the function.
     * @tparam Index The index of the argument whose type is being extracted.
     */
    template <usize Index>
    using Arg = TupleElementT<Index, Tuple<Args...>>;
};

/**
 * @brief Specialization of FunctionTraits for function pointer types
 * that extracts information about their return type and argument types.
 * @tparam Ret The return type of the function.
 * @tparam Args The argument types of the function.
 */
template <typename Ret, typename... Args>
struct FunctionTraits<Ret(*)(Args...)> : FunctionTraits<Ret(Args...)> {};

/**
 * @brief Specialization of FunctionTraits for function reference types
 * that extracts information about their return type and argument types.
 * @tparam Ret The return type of the function.
 * @tparam Args The argument types of the function.
 */
template <typename Ret, typename... Args>
struct FunctionTraits<Ret(&)(Args...)> : FunctionTraits<Ret(Args...)> {};

/**
 * @brief Specialization of FunctionTraits for rvalue function reference types
 * that extracts information about their return type and argument types.
 * @tparam Ret The return type of the function.
 * @tparam Args The argument types of the function.
 */
template <typename Ret, typename... Args>
struct FunctionTraits<Ret(&&)(Args...)> : FunctionTraits<Ret(Args...)> {};

/**
 * @brief Specialization of FunctionTraits for member function pointer types
 * that extracts information about their return type and argument types.
 * @tparam Class The class type that the member function belongs to.
 * @tparam Ret The return type of the member function.
 * @tparam Args The argument types of the member function.
 */ 
template <typename Class, typename Ret, typename... Args>
struct FunctionTraits<Ret(Class::*)(Args...)> : FunctionTraits<Ret(Args...)> {
    using ClassType = Class;
};

/**
 * @brief Specialization of FunctionTraits for const member function pointer types
 * that extracts information about their return type and argument types.
 * @tparam Class The class type that the member function belongs to.
 * @tparam Ret The return type of the member function.
 * @tparam Args The argument types of the member function.
 */
template <typename Class, typename Ret, typename... Args>
struct FunctionTraits<Ret(Class::*)(Args...) const> : FunctionTraits<Ret(Args...)> {
    using ClassType = Class;
};

/**
 * @brief Specialization of FunctionTraits for noexcept member function pointer types
 * that extracts information about their return type and argument types.
 * @tparam Class The class type that the member function belongs to.
 * @tparam Ret The return type of the member function.
 * @tparam Args The argument types of the member function.
 */
template <typename Class, typename Ret, typename... Args>
struct FunctionTraits<Ret(Class::*)(Args...) noexcept> : FunctionTraits<Ret(Args...)> {
    using ClassType = Class;
};

/**
 * @brief Specialization of FunctionTraits for const noexcept member function pointer types
 * that extracts information about their return type and argument types.
 * @tparam Class The class type that the member function belongs to.
 * @tparam Ret The return type of the member function.
 * @tparam Args The argument types of the member function.
 */
template <typename Class, typename Ret, typename... Args>
struct FunctionTraits<Ret(Class::*)(Args...) const noexcept> : FunctionTraits<Ret(Args...)> {
    using ClassType = Class;
};

/**
 * @brief Represents the return type of a function Func.
 * @tparam Func The function type whose return type is being extracted.
 */
template <typename Func>
using FunctionReturnType = typename FunctionTraits<Func>::ReturnType;

/**
 * @brief Represents the type of the N-th argument of a function Func.
 * @tparam Func The function type whose argument type is being extracted.
 * @tparam Index The index of the argument whose type is being extracted.
 */
template <typename Func, usize Index>
using FunctionArg = typename FunctionTraits<Func>::template Arg<Index>;

/**
 * @brief Represents the number of arguments of a function Func.
 * @tparam Func The function type whose argument count is being extracted.
 */
template <typename Func>
inline constexpr usize FunctionArgCount = FunctionTraits<Func>::argCount;

NEX_NAMESPACE_END
