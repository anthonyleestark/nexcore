/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

// Trait to check if a type is a character type.
template <class Type>
inline const bool IsCharTypeV = false;

// Specialization for nchar (narrow/native character type).
template <>
inline const bool IsCharTypeV<nchar> = true;

#if NEX_HAS_BUILTIN_CHAR8_T
    // Specialization for char8 (UTF-8 code unit).
    template <>
    inline const bool IsCharTypeV<char8> = true;
#endif

// Specialization for char16 (UTF-16 code unit).
template <>
inline const bool IsCharTypeV<char16> = true;

// Specialization for char32 (UTF-32 code unit).
template <>
inline const bool IsCharTypeV<char32> = true;

// Specialization for wchar (wide character type).
template <>
inline const bool IsCharTypeV<wchar> = true;

// Trait to check if a type is a character type.
template <class Type>
struct IsCharType : meta::BoolConstant<IsCharTypeV<Type>> {};

// Traits to check if a type is a single-byte character type (nchar or char8).
template <class Type>
inline const bool IsSingleByteCharTypeV = false;

// Specialization for nchar (narrow/native character type).
template <>
inline const bool IsSingleByteCharTypeV<nchar> = true;

#if NEX_HAS_BUILTIN_CHAR8_T
    // Specialization for char8 (UTF-8 code unit).
    template <>
    inline const bool IsSingleByteCharTypeV<char8> = true;
#endif

// Trait to check if a type is a single-byte character type (nchar or char8).
template <class Type>
struct IsSingleByteCharType : meta::BoolConstant<IsSingleByteCharTypeV<Type>> {};

/**
 * @brief Computes the length of a null-terminated string at compile time.
 * @tparam Type The character type of the string (nchar or char8).
 * @param str The null-terminated string.
 * @return The length of the string, excluding the null terminator.
 */
template <class Type>
inline NEX_HIDDEN_FROM_ABI constexpr usize constexprStrLen(const Type* str) noexcept {
    static_assert(IsSingleByteCharTypeV<Type>,
        "Error: constexprStrLen only works with single-byte character types (nchar or char8)");

    // GCC currently doesn't support __builtin_strlen for heap-allocated memory during constant evaluation.
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70816
    if NEX_CONSTEVAL_CONTEXT {
#if NEX_HAS_BUILTIN(__builtin_strlen)
        if constexpr (meta::IsSameV<Type, nchar>)
            return __builtin_strlen(str);
#else
        usize i = 0;
        for (; str[i] != '\0'; ++i)
        ;
        return i;
#endif
    }

#if NEX_HAS_BUILTIN(__builtin_strlen)
    return __builtin_strlen(reinterpret_cast<cstring>(str));
#else
    usize i = 0;
    for (; str[i] != '\0'; ++i)
    ;
    return i;
#endif
}

// Computes the length of a null-terminated string at compile time.
#define NEX_CONSTEXPR_STRLEN(str) \
    NEX_PREPEND_NAMESPACE(constexprStrLen)(str)

// CharTraits primary template
template <class Type>
struct CharTraits {
    static_assert(IsCharTypeV<Type>,
        "Error: CharTraits can only be instantiated with character types");
};

NEX_NAMESPACE_END
