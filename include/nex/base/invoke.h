/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/compiler.h"
#include "nex/base/export.h"
#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/casts.h"
#include "nex/base/reference.h"

NEX_NAMESPACE_BEGIN

#if NEX_HAS_BUILTIN(__builtin_invoke)

    // Internal implementation of InvokeResult using the compiler builtin __builtin_invoke.
    template <class, class... Args>
    struct NEX_HIDDEN_FROM_ABI _InvokeResultImpl {};

    // Specialization of _InvokeResultImpl for the case where __builtin_invoke is valid for the given arguments.
    template <class... Args>
    struct NEX_HIDDEN_FROM_ABI _InvokeResultImpl<
        meta::VoidT<decltype(__builtin_invoke(meta::declval<Args>()...))>, Args...> {
        using type NEX_NODEBUG = decltype(__builtin_invoke(meta::declval<Args>()...));
    };

    // Internal implementation of InvokeResult.
    template <class... _Args>
    using InvokeResultImpl NEX_NODEBUG = _InvokeResultImpl<void, _Args...>;

    // Alias template for easier usage of InvokeResultImpl 
    // to obtain the result type of invoking a callable with given arguments.
    template <class... Args>
    using InvokeResultTImpl NEX_NODEBUG = typename InvokeResultImpl<Args...>::type;

    // Internal implementation of the invoke operation using the compiler builtin __builtin_invoke.
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr 
    InvokeResultTImpl<Args...> _invokeImpl(Args&&... args) 
        noexcept(noexcept(__builtin_invoke(NEX_FORWARD<Args>(args)...))) {
        return __builtin_invoke(NEX_FORWARD<Args>(args)...);
    }

    // Internal implementation of IsInvocable
    template <class Void, class... Args>
    inline const bool _IsInvocableImpl = false;

    // Specialization of _IsInvocableImpl for the case where the invocation is valid.
    template <class... Args>
    inline const bool _IsInvocableImpl<meta::VoidT<InvokeResultTImpl<Args...> >, Args...> = true;

    // Internal implementation of IsInvocableV
    template <class... Args>
    inline const bool _IsInvocableVImpl = _IsInvocableImpl<void, Args...>;

    // Trait to check if a callable can be invoked with the given arguments.
    template <class... Args>
    struct IsInvocable : meta::IntegralConstant<bool, _IsInvocableVImpl<Args...>> {};

    // Trait to check if a callable can be invoked with the given arguments and return type.
    template <class Ret, bool, class... Args>
    inline const bool _IsInvocableRImpl = false;

    // Specialization of _IsInvocableRImpl for the case 
    // where the invocation is valid and the return type is convertible.
    template <class Ret, class... Args>
    inline const bool _IsInvocableRImpl<Ret, true, Args...> =
        meta::IsConvertibleV<InvokeResultTImpl<Args...>, Ret> || meta::IsVoidV<Ret>;

    // Internal implementation of IsInvocableRV
    template <class Ret, class... Args>
    inline const bool _IsInvocableRV = _IsInvocableRImpl<Ret, _IsInvocableVImpl<Args...>, Args...>;

    // Internal implementation of IsNothrowInvocable
    template <bool _IsInvocable, class... Args>
    inline const bool _IsNothrowInvocableImpl = false;

    // Specialization of _IsNothrowInvocableImpl for the case where the invocation is valid.
    template <class... Args>
    inline const bool _IsNothrowInvocableImpl<true, Args...> = noexcept(__builtin_invoke(std::declval<Args>()...));

    // Internal implementation of IsNothrowInvocableV
    template <class... Args>
    inline const bool _IsNothrowInvocableV = _IsNothrowInvocableImpl<_IsInvocableVImpl<Args...>, Args...>;

    // Internal implementation of IsNothrowInvocableR
    template <bool _IsInvocable, class Ret, class... Args>
    inline const bool _IsNothrowInvocableRImpl = false;

    // Specialization of _IsNothrowInvocableRImpl for the case where the invocation is valid
    template <class Ret, class... Args>
    inline const bool _IsNothrowInvocableRImpl<true, Ret, Args...> =
        meta::IsNothrowConvertibleV<InvokeResultTImpl<Args...>, Ret> || meta::IsVoidV<Ret>;

    // Internal implementation of IsNothrowInvocableRV
    template <class Ret, class... Args>
    inline const bool _IsNothrowInvocableRV =
        _IsNothrowInvocableRImpl<_IsNothrowInvocableV<Args...>, Ret, Args...>;

#else

#endif // ^^NEX_HAS_BUILTIN(__builtin_invoke)

// Wrapper to handle the return type of an invocation,
// allowing for void return types to be handled correctly.
template <class Ret, bool = meta::IsVoidV<Ret>>
struct InvokeVoidReturnWrapperImpl {
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr static Ret call(Args&&... args) {
        return _invokeImpl(NEX_FORWARD<Args>(args)...);
    }
};

// Specialization of InvokeVoidReturnWrapperImpl for void return types,
// which simply invokes the callable without returning a value.
template <class Ret>
struct InvokeVoidReturnWrapperImpl<Ret, true> {
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr static void call(Args&&... args) {
        _invokeImpl(NEX_FORWARD<Args>(args)...);
    }
};

// Internal implementation of invoking a callable with a specified return type,
// forwarding the provided arguments and handling void return types appropriately.
template <class Ret, class... Args>
NEX_HIDDEN_FROM_ABI constexpr Ret _invokeRImpl(Args&&... args) {
    return InvokeVoidReturnWrapperImpl<Ret>::call(NEX_FORWARD<Args>(args)...);
}

// ==================================================================================
// Public traits for checking invocability (IsInvocable)
// ==================================================================================

// Determine whether a callable can be invoked with the given arguments.
template <class Func, class... Args>
struct IsInvocable : meta::BoolConstant<_IsInvocableVImpl<Func, Args...> > {};

// Determine whether a callable can be invoked with the given arguments and return type.
template <class Ret, class Func, class... Args>
struct IsInvocableR : meta::BoolConstant<_IsInvocableRV<Ret, Func, Args...>> {};

// Determine whether a callable can be invoked with the given arguments.
template <class Func, class... Args>
inline constexpr bool IsInvocableV = _IsInvocableVImpl<Func, Args...>;

// Determine whether a callable can be invoked with the given arguments and return type.
template <class Ret, class Func, class... Args>
inline constexpr bool IsInvocableRV = _IsInvocableRV<Ret, Func, Args...>;

// =================================================================================
// Public traits for checking nothrow invocability (IsNothrowInvocable)
// =================================================================================

// Determine whether a callable can be invoked with the given arguments without throwing exceptions.
template <class Func, class... Args>
struct IsNothrowInvocable : meta::BoolConstant<_IsNothrowInvocableV<Func, Args...> > {};

// Determine whether a callable can be invoked with the given arguments and return type without throwing exceptions.
template <class Ret, class Func, class... Args>
struct IsNothrowInvocableR
    : meta::BoolConstant<_IsNothrowInvocableRV<Ret, Func, Args...>> {};

// Determine whether a callable can be invoked with the given arguments without throwing exceptions.
template <class Func, class... Args>
inline constexpr bool IsNothrowInvocableV = _IsNothrowInvocableV<Func, Args...>;

// Determine whether a callable can be invoked with the given arguments and return type without throwing exceptions.
template <class Ret, class Func, class... Args>
inline constexpr bool IsNothrowInvocableRV =
    _IsNothrowInvocableRV<Ret, Func, Args...>;

// =================================================================================
// Public traits for obtaining the result type of an invocation (InvokeResult)
// =================================================================================

// Determine the result type of invoking a callable with the given arguments.
template <class Func, class... Args>
struct InvokeResult : InvokeResultImpl<Func, Args...> {};

// Determine the result type of invoking a callable with the given arguments.
template <class Func, class... Args>
using InvokeResultT = InvokeResultTImpl<Func, Args...>;

// =================================================================================
// Public function for invoking a callable with the given arguments (invoke)
// =================================================================================

template <class Func, class... Args>
NEX_HIDDEN_FROM_ABI constexpr 
InvokeResultT<Func, Args...> invoke(Func&& fn, Args&&... args) 
    noexcept(IsNothrowInvocableV<Func, Args...>) {
    return _invokeImpl(NEX_FORWARD<Func>(fn), NEX_FORWARD<Args>(args)...);
}

NEX_NAMESPACE_END
