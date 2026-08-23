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
// Utilities for integral conversion and type manipulation
// =================================================================================

NEX_HIDDEN_FROM_ABI inline constexpr int32 convertToIntegral(int32 value) noexcept { return value; }

NEX_HIDDEN_FROM_ABI inline constexpr uint32 convertToIntegral(uint32 value) noexcept { return value; }

NEX_HIDDEN_FROM_ABI inline constexpr int64 convertToIntegral(int64 value) noexcept { return value; }

NEX_HIDDEN_FROM_ABI inline constexpr uint64 convertToIntegral(uint64 value) noexcept { return value; }

NEX_HIDDEN_FROM_ABI inline constexpr slong convertToIntegral(slong value) noexcept { return value; }

NEX_HIDDEN_FROM_ABI inline constexpr ulong convertToIntegral(ulong value) noexcept { return value; }

NEX_HIDDEN_FROM_ABI inline constexpr longlong convertToIntegral(longlong value) noexcept { return value; }

NEX_HIDDEN_FROM_ABI inline constexpr ulonglong convertToIntegral(ulonglong value) noexcept { return value; }

#if NEX_HAS_BUILTIN_INT128
    NEX_HIDDEN_FROM_ABI inline constexpr int128 convertToIntegral(int128 value) noexcept { return value; }

    NEX_HIDDEN_FROM_ABI inline constexpr uint128 convertToIntegral(uint128 value) noexcept { return value; }
#endif  // ^^NEX_HAS_BUILTIN_INT128

template <meta::FloatingPoint Floating>
NEX_HIDDEN_FROM_ABI inline constexpr longlong convertToIntegral(Floating value) {
    return static_cast<longlong>(value);
}

template <class Type, bool = meta::IsEnumV<Type>>
struct NEX_HIDDEN_FROM_ABI SFINAEUnderlyingType {
    using type = meta::UnderlyingTypeT<Type>;
    using promoted_type = decltype(((type)1) + 0);
};

template <class Type>
struct SFINAEUnderlyingType<Type, false> {};

template <class Type>
inline NEX_HIDDEN_FROM_ABI constexpr
typename SFINAEUnderlyingType<Type>::promoted_type convertToIntegral(Type value) {
    return value;
}

NEX_NAMESPACE_END
