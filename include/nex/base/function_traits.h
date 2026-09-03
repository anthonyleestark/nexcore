/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/casts.h"
#include "nex/base/invoke.h"
#include "nex/base/fwd.h"

NEX_NAMESPACE_BEGIN

/// Extracts information about a function type, such as its return type and argument types. 
/// This primary template is designed to handle callable objects (e.g., lambdas, functors) 
/// by deducing their operator() member function.
template <typename Type>
struct FunctionTraits : FunctionTraits<decltype(&RemoveReference<Type>::operator())> {};

/// Extracts information about a function type, such as its return type and argument types.
template <typename R, typename... Args>
struct FunctionTraits<R(Args...)> {
    using ReturnType = R;
    static constexpr usize argCount = sizeof...(Args);

    template <usize N>
    using Arg = TupleElementT<N, Tuple<Args...>>;
};

/// A specialization of FunctionTraits for function pointer types, 
/// which inherits from the primary template to extract the return type and argument types.
template <typename R, typename... Args>
struct FunctionTraits<R(*)(Args...)> : FunctionTraits<R(Args...)> {};

/// A specialization of FunctionTraits for function reference types, 
/// which inherits from the primary template to extract the return type and argument types.
template <typename R, typename... Args>
struct FunctionTraits<R(&)(Args...)> : FunctionTraits<R(Args...)> {};

/// A specialization of FunctionTraits for rvalue function reference types, 
/// which inherits from the primary template to extract the return type and argument types.
template <typename R, typename... Args>
struct FunctionTraits<R(&&)(Args...)> : FunctionTraits<R(Args...)> {};

/// A specialization of FunctionTraits for member function pointer types, 
/// which inherits from the primary template to extract the return type and argument types, 
/// and also defines the ClassType representing the class that the member function belongs to.
template <typename Class, typename R, typename... Args>
struct FunctionTraits<R(Class::*)(Args...)> : FunctionTraits<R(Args...)> {
    using ClassType = Class;
};

/// A specialization of FunctionTraits for const member function pointer types, 
/// which inherits from the primary template to extract the return type and argument types, 
/// and also defines the ClassType representing the class that the member function belongs to.
template <typename Class, typename R, typename... Args>
struct FunctionTraits<R(Class::*)(Args...) const> : FunctionTraits<R(Args...)> {
    using ClassType = Class;
};

/// A specialization of FunctionTraits for noexcept member function pointer types, 
/// which inherits from the primary template to extract the return type and argument types, 
/// and also defines the ClassType representing the class that the member function belongs to.
template <typename Class, typename R, typename... Args>
struct FunctionTraits<R(Class::*)(Args...) noexcept> : FunctionTraits<R(Args...)> {
    using ClassType = Class;
};

/// A specialization of FunctionTraits for const noexcept member function pointer types, 
/// which inherits from the primary template to extract the return type and argument types, 
/// and also defines the ClassType representing the class that the member function belongs to.
template <typename Class, typename R, typename... Args>
struct FunctionTraits<R(Class::*)(Args...) const noexcept> : FunctionTraits<R(Args...)> {
    using ClassType = Class;
};

/// Represents the return type of a function Fn.
template <typename Fn>
using FunctionReturnType = typename FunctionTraits<Fn>::ReturnType;

/// Represents the type of the N-th argument of a function Fn.
template <typename Fn, usize N>
using FunctionArg = typename FunctionTraits<Fn>::template Arg<N>;

/// Represents the number of arguments of a function Fn.
template <typename Fn>
inline constexpr usize FunctionArgCount = FunctionTraits<Fn>::argCount;

NEX_NAMESPACE_END
