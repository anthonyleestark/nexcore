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

NEX_NAMESPACE_END
