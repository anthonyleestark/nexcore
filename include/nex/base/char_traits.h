/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/export.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/memory.h"
#include "nex/base/algorithm.h"

NEX_NAMESPACE_BEGIN

// Trait to check if a type is a character type.
template <class Type>
inline const bool IsCharTypeV = false;

// Specialization for nchar (native narrow character type).
template <>
inline const bool IsCharTypeV<nchar> = true;

#if NEX_HAS_BUILTIN_CHAR8_T
    // Specialization for char8 (UTF-8 code unit).
    template <>
    inline const bool IsCharTypeV<char8> = true;
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

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

// Specialization for nchar (native narrow character type).
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

// Concept to check if a type is a character type (nchar, char8, char16, char32, or wchar).
template <class Type>
concept Character = IsCharTypeV<Type>;  

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

/**
 * @struct CharTraits
 * @brief  Primary template for character traits. This template is specialized for different character types.
 * @tparam Type The character type (nchar, char8, char16, char, char32, or wchar).
 */
template <class Type>
struct CharTraits {
    static_assert(meta::AlwaysFalseV<Type>,
        "Error: CharTraits can not be instantiated with this character type");
};

/**
 * @struct CharTraits<nchar>
 * @brief  Traits specialization for native narrow character type (nchar).
 */
template <>
struct CharTraits<nchar> {
    using char_type = nchar;
    using int_type = unsigned int;

    static NEX_HIDDEN_FROM_ABI inline constexpr
    void assign(char_type& c1, const char_type& c2) noexcept { c1 = c2; }

    static NEX_HIDDEN_FROM_ABI inline constexpr
    bool eq(char_type c1, char_type c2) noexcept { return c1 == c2; }

    static NEX_HIDDEN_FROM_ABI inline constexpr
    bool lt(char_type c1, char_type c2) noexcept {
        return static_cast<uint8>(c1) < static_cast<uint8>(c2);
    }

    static NEX_HIDDEN_FROM_ABI constexpr
    int32 compare(const char_type* lhs, const char_type* rhs, usize count) noexcept {
        return NEX_CONSTEXPR_MEMCMP(lhs, rhs, count);
    }

    static NEX_HIDDEN_FROM_ABI constexpr usize length(const char_type* str) noexcept {
        return NEX_CONSTEXPR_STRLEN(str);
    }

    static NEX_HIDDEN_FROM_ABI constexpr
    const char_type* find(const char_type* str, char_type c, usize count) noexcept {
        return NEX_CONSTEXPR_MEMCHR(str, c, count);
    }

    static NEX_HIDDEN_FROM_ABI constexpr
    char_type* move(char_type* dest, const char_type* src, usize count) noexcept {
        return NEX_CONSTEXPR_MEMMOVE(dest, src, count);
    }

    static NEX_HIDDEN_FROM_ABI constexpr
    char_type* copy(char_type* dest, const char_type* src, usize count) noexcept {
        NEX_MEMORY_ASSERT_NON_OVERLAPPING_RANGES(!isPointerInRange(src, src + count, dest),
            "CharTraits<nchar>::copy: source and destination ranges overlap");
        return NEX_CONSTEXPR_MEMMOVE(dest, src, count);
    }

    static NEX_HIDDEN_FROM_ABI constexpr
    char_type* assign(char_type* str, usize count, char_type fillChar) noexcept {
        NEX_FILL_RANGE(str, count, fillChar);
        return str;
    }
};

/**
 * @struct CharTraitsBase
 * @brief  Base class for character traits, providing common type definitions and implementations.
 * @tparam CharType The character type (nchar, char8, char16, char32, or wchar).
 * @tparam IntType The integer type used for character representation (uint32, uint_least16, uint_least32, or wint).
 */
template <Character CharType, meta::Integral IntType>
struct NEX_INTERNAL CharTraitsBase {
    using char_type = CharType;
    using int_type = IntType;

    NEX_HIDDEN_FROM_ABI static inline constexpr
    void assign(char_type& lhs, const char_type& rhs) noexcept { lhs = rhs; }

    NEX_HIDDEN_FROM_ABI static inline constexpr
    bool eq(char_type lhs, char_type rhs) noexcept { return lhs == rhs; }

    NEX_HIDDEN_FROM_ABI static inline constexpr
    bool lt(char_type lhs, char_type rhs) noexcept { return lhs < rhs; }

    NEX_HIDDEN_FROM_ABI static constexpr
    char_type* move(char_type* dest, const char_type* src, usize count) noexcept {
        return NEX_CONSTEXPR_MEMMOVE(dest, src, count);
    }

    NEX_HIDDEN_FROM_ABI static constexpr
    char_type* copy(char_type* dest, const char_type* src, usize count) noexcept {
        NEX_MEMORY_ASSERT_NON_OVERLAPPING_RANGES(!isPointerInRange(src, src + count, dest),
            "CharTraitsBase::copy: source and destination ranges overlap");
        return NEX_CONSTEXPR_MEMMOVE(dest, src, count);
    }

    NEX_HIDDEN_FROM_ABI static constexpr
    char_type* assign(char_type* str, usize count, char_type fillChar) noexcept {
        NEX_FILL_RANGE(str, count, fillChar);
        return str;
    }
};

/**
 * @struct CharTraits<char8>
 * @brief  Traits specialization for UTF-8 character type (char8).
 */
#if NEX_HAS_BUILTIN_CHAR8_T
    template <>
    struct CharTraits<char8> : CharTraitsBase<char8, uint32> {
        static NEX_HIDDEN_FROM_ABI constexpr
        int32 compare(const char_type* lhs, const char_type* rhs, usize count) noexcept {
            return NEX_CONSTEXPR_MEMCMP(lhs, rhs, count);
        }

        static NEX_HIDDEN_FROM_ABI constexpr usize length(const char_type* str) noexcept {
            return NEX_CONSTEXPR_STRLEN(str);
        }

        static NEX_HIDDEN_FROM_ABI constexpr
        const char_type* find(const char_type* str, char_type c, usize count) noexcept {
            return NEX_CONSTEXPR_MEMCHR(str, c, count);
        }
    };
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

/**
 * @struct CharTraits<char16>
 * @brief  Traits specialization for UTF-16 character type (char16).
 */
template <>
struct CharTraits<char16> : CharTraitsBase<char16, uint_least16> {
    static NEX_HIDDEN_FROM_ABI constexpr
    int32 compare(const char_type* lhs, const char_type* rhs, usize count) noexcept {
        for (; count; --count, ++lhs, ++rhs) {
            if (lt(*lhs, *rhs))
                return -1;
            if (lt(*rhs, *lhs))
                return 1;
        }
        return 0;
    }

    static NEX_HIDDEN_FROM_ABI constexpr usize length(const char_type* str) noexcept {
        usize len = 0;
        for (; !eq(str[len], char_type(0)); ++len)
            ;
        return len;
    }

    static NEX_HIDDEN_FROM_ABI constexpr
    const char_type* find(const char_type* str, char_type c, usize count) noexcept {
        for (; count; --count) {
            if (eq(*str, c))
                return str;
            ++str;
        }
        return nullptr;
    }
};

/**
 * @struct CharTraits<char32>
 * @brief  Traits specialization for UTF-32 character type (char32).
 */
template <>
struct CharTraits<char32> : CharTraitsBase<char32, uint_least32> {
    static NEX_HIDDEN_FROM_ABI constexpr
    int32 compare(const char_type* lhs, const char_type* rhs, usize count) noexcept {
        for (; count; --count, ++lhs, ++rhs) {
            if (lt(*lhs, *rhs))
                return -1;
            if (lt(*rhs, *lhs))
                return 1;
        }
        return 0;
    }

    static NEX_HIDDEN_FROM_ABI constexpr usize length(const char_type* str) noexcept {
        usize len = 0;
        for (; !eq(str[len], char_type(0)); ++len)
            ;
        return len;
    }

    static NEX_HIDDEN_FROM_ABI constexpr
    const char_type* find(const char_type* str, char_type c, usize count) noexcept {
        for (; count; --count) {
            if (eq(*str, c))
                return str;
            ++str;
        }
        return nullptr;
    }
};

/**
 * @struct CharTraits<wchar>
 * @brief  Traits specialization for wide character type (wchar).
 */
template <>
struct CharTraits<wchar> : CharTraitsBase<wchar, wint> {
    static NEX_HIDDEN_FROM_ABI constexpr
    int32 compare(const char_type* lhs, const char_type* rhs, usize count) noexcept {
        for (; count; --count, ++lhs, ++rhs) {
            if (lt(*lhs, *rhs))
                return -1;
            if (lt(*rhs, *lhs))
                return 1;
        }
        return 0;
    }

    static NEX_HIDDEN_FROM_ABI constexpr usize length(const char_type* str) noexcept {
        usize len = 0;
        for (; !eq(str[len], char_type(0)); ++len)
            ;
        return len;
    }

    static NEX_HIDDEN_FROM_ABI constexpr
    const char_type* find(const char_type* str, char_type c, usize count) noexcept {
        for (; count; --count) {
            if (eq(*str, c))
                return str;
            ++str;
        }
        return nullptr;
    }
};

NEX_NAMESPACE_END
