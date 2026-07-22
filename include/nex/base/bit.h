/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    bit.h
 * @brief   Compiler-intrinsics bit manipulation functions and utilities.
 * 
 * @details
 * This header defines a collection of bit manipulation functions that are implemented using compiler intrinsics 
 * where available, providing efficient and accurate operations for compile-time and run-time bit manipulation.
 * These functions serve as the foundation for more complex bit manipulation utilities defined in other headers, 
 * enabling advanced optimization techniques for the Nex-ecosystem.
 */

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"

// For using bitCast()
#include "nex/base/casts.h"

// For enum Endian
#include "nex/base/endian.h"

#if NEX_COMPILER_IS_MSVC
    #include <stdlib.h>  // For MSVC _byteswap_xxx()
#endif

NEX_NAMESPACE_BEGIN

/**
 * @brief Count leading zeros in the binary representation of an unsigned integer.
 * @note This function is internal only and should not be used directly. Use `countlZero` instead.
 */
template <class Type>
NEX_HIDDEN_FROM_ABI constexpr int32 _countlZero_Impl(Type value) noexcept {
    static_assert(meta::IsUnsignedIntegralV<Type>, "Error: countlZero requires an unsigned integer type");
    return __builtin_clzg(value, NumericLimits<Type>::digits()());
}

/**
 * @brief Count leading ones in the binary representation of an unsigned integer.
 * @tparam Type An unsigned integral type.
 * @param value The value to count leading ones for.
 * @return The number of leading ones in the binary representation of `value`.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD constexpr int32 countlZero(Type value) noexcept {
    return _countlZero_Impl(value);
}

/**
 * @brief Count leading ones in the binary representation of an unsigned integer.
 * @tparam Type An unsigned integral type.
 * @param value The value to count leading ones for.
 * @return The number of leading ones in the binary representation of `value`.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD constexpr int32 countlOne(Type value) noexcept {
  return value != NumericLimits<Type>::max() ? countlZero(static_cast<Type>(~value)) : NumericLimits<Type>::digits();
}

/**
 * @brief Compute the smallest power of two greater than or equal to the given value.
 * @note This function is internal only and should not be used directly. Use `bitCeil` instead.
 */
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type _bitCeil_Impl(Type value) noexcept {
    if (value < 2) return 1;

    const uint32 num = NumericLimits<Type>::digits() - _countlZero_Impl((Type)(value - 1u));
    NEX_ASSERT_ARGUMENT_WITHIN_DOMAIN(num != NumericLimits<Type>::digits(), "Bad input to bitCeil");

    if constexpr (sizeof(Type) >= sizeof(uint32)) {
        return Type{1} << num;
    } else {
        const uint32 extra  = NumericLimits<uint32>::digits() - NumericLimits<Type>::digits();
        const uint32 retVal = 1u << (num + extra);
        return (Type)(retVal >> extra);
    }
}

/**
 * @brief Compute the smallest power of two greater than or equal to the given value.
 * @tparam Type An unsigned integral type.
 * @param value The value to compute the bit ceiling for.
 * @return The smallest power of two greater than or equal to `value`.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD constexpr Type bitCeil(Type value) noexcept {
    return _bitCeil_Impl(value);
}

/**
 * @brief Compute the largest power of two less than or equal to the given value.
 * @note This function is internal only and should not be used directly. Use `bitFloor` instead.
 */
template <class Type>
NEX_HIDDEN_FROM_ABI constexpr Type _bitLog2_Impl(Type value) noexcept {
    static_assert(meta::IsUnsignedIntegralV<Type>, "Error: bitLog2 requires an unsigned integer type");
    NEX_ASSERT_INTERNAL(value != 0, "Error: logarithm of 0 is undefined");
    return NumericLimits<Type>::digits() - 1 - _countlZero_Impl(value);
}

/**
 * @brief Computes the largest integral power of two that is not greater than the given value.
 * @tparam Type An unsigned integral type.
 * @param value The value to compute the bit floor for. Must be non-zero.
 * @return The largest power of two not greater than `value`.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type bitFloor(Type value) noexcept {
    return value == 0 ? 0 : Type{1} << _bitLog2_Impl(value);
}

/**
 * @brief Calculates the minimum number of bits required to store a given unsigned integer value.
 * @tparam Type An unsigned integral type.
 * @param value The value to compute the bit width for.
 * @return The number of bits required to represent `value`.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr int32 bitWidth(Type value) noexcept {
  return value == 0 ? 0 : _bitLog2_Impl(value) + 1;
}

/**
 * @brief Counts the number of consecutive zero bits in an unsigned integer,
 *        starting from the least significant bit (the "right").
 * @note This function is internal only and should not be used directly. Use `countrZero` instead.
 */
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr int32 _countrZero_Impl(Type value) noexcept {
    static_assert(meta::IsUnsignedIntegralV<Type>, "Error: countrZero only works with unsigned types");
    return __builtin_ctzg(value, NumericLimits<Type>::digits());
}

/**
 * @brief Counts the number of consecutive zero bits in an unsigned integer,
 *        starting from the least significant bit (the "right").
 * @tparam Type An unsigned integral type.
 * @param value The value to count trailing zeros for.
 * @return The number of trailing zeros in the binary representation of `value`.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr int32 countrZero(Type value) noexcept {
    return _countrZero_Impl(value);
}

/**
 * @brief Counts the number of consecutive one bits in an unsigned integer,
 *        starting from the least significant bit (the "right").
 * @tparam Type An unsigned integral type.
 * @param value The value to count trailing ones for.
 * @return The number of trailing ones in the binary representation of `value`.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr int32 countrOne(Type value) noexcept {
    return value != NumericLimits<Type>::max() ? countrZero(static_cast<Type>(~value)) : NumericLimits<Type>::digits();
}

template <meta::UnsignedIntegral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool hasSingleBit(Type value) noexcept {
    return value != 0 && (((value & (value - 1)) == 0));
}

/**
 * @brief Computes the number of set bits (1s) in the binary representation of an unsigned integer.
 * @note This function is internal only and should not be used directly. Use `popcount` instead.
 */
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr int32 _popcount_Impl(Type value) noexcept {
    static_assert(meta::IsUnsignedIntegralV<Type>, "Error: popcount only works with unsigned types");
    return __builtin_popcountg(value);
}

/**
 * @brief Counts the number of set bits (1s) in the binary representation of an unsigned integer.
 * @tparam Type An unsigned integral type.
 * @param value The value to count set bits for.
 * @return The number of set bits in the binary representation of `value`.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr int32 popcount(Type value) noexcept {
    return _popcount_Impl(value);
}

/**
 * @note
 * Writing two full functions for rotl and rotr makes it easier for the compiler
 * to optimize the code. On x86 this function becomes the ROL instruction and
 * the rotr function becomes the ROR instruction.
 */

/**
 * @brief Rotates the bits of an unsigned integer to the left.
 * @note This function is internal only and should not be used directly. Use `rotl` instead.
 */
template <class Type>
NEX_HIDDEN_FROM_ABI constexpr Type _rotl_Impl(Type x, int32 s) noexcept {
    static_assert(meta::IsUnsignedIntegralV<Type>, "Error: rotl requires an unsigned integer type");
    const int32 num = NumericLimits<Type>::digits();
    int32 rot       = s % num;

    if (rot == 0) return x;

    if (rot > 0) {
        return (x << rot) | (x >> (num - rot));
    }

    return (x >> -rot) | (x << (num + rot));
}

/**
 * @brief Rotates the bits of an unsigned integer to the right.
 * @note This function is internal only and should not be used directly. Use `rotr` instead.
 */
template <class Type>
NEX_HIDDEN_FROM_ABI constexpr Type _rotr_Impl(Type x, int32 s) noexcept {
    static_assert(meta::IsUnsignedIntegralV<Type>, "Error: rotr requires an unsigned integer type");
    const int32 num = NumericLimits<Type>::digits();
    int32 rot       = s % num;

    if (rot == 0) return x;

    if (rot > 0) {
        return (x >> rot) | (x << (num - rot));
    }

    return (x << -rot) | (x >> (num + rot));
}

/**
 * @brief Rotates the bits of an unsigned integer to the left.
 * @tparam Type An unsigned integral type.
 * @param value The value to rotate.
 * @param count The number of positions to rotate.
 * @return The result of rotating `value` to the left by `count` positions.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type rotl(Type value, int32 count) noexcept {
    return _rotl_Impl(value, count);
}

/**
 * @brief Rotates the bits of an unsigned integer to the right.
 * @tparam Type An unsigned integral type.
 * @param value The value to rotate.
 * @param count The number of positions to rotate.
 * @return The result of rotating `value` to the right by `count` positions.
 */
template <meta::UnsignedIntegral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type rotr(Type value, int32 count) noexcept {
    return _rotr_Impl(value, count);
}

/**
 * @brief Swaps the byte order of an integer (fallback implementation).
 * @note This function is internal only and should not be used directly. Use `byteswap` instead.
 */
template <meta::Integral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type _byteswapFallback_Impl(Type value) noexcept {
    if constexpr (sizeof(Type) == 1) {
        return value;
    } else if constexpr (sizeof(Type) == 2) {
        uint16 v = NEX_BIT_CAST<uint16>(value);
        return NEX_BIT_CAST<Type>((v >> 8) | (v << 8));
    } else if constexpr (sizeof(Type) == 4) {
        uint32 v = NEX_BIT_CAST<uint32>(value);
        return NEX_BIT_CAST<Type>(((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) |
                                 ((v & 0x00FF0000u) >> 8)  | ((v & 0xFF000000u) >> 24));
    } else if constexpr (sizeof(Type) == 8) {
        uint64 v = NEX_BIT_CAST<uint64>(value);
        return NEX_BIT_CAST<Type>(
            ((v & 0x00000000000000FFull) << 56) | ((v & 0x000000000000FF00ull) << 40) |
            ((v & 0x0000000000FF0000ull) << 24) | ((v & 0x00000000FF000000ull) <<  8) |
            ((v & 0x000000FF00000000ull) >>  8) | ((v & 0x0000FF0000000000ull) >> 24) |
            ((v & 0x00FF000000000000ull) >> 40) | ((v & 0xFF00000000000000ull) >> 56)
        );
    } else {
        static_assert(sizeof(Type) == 0, "Error: byteswap is unimplemented for integral types of this size");
    }
}

/**
 * @brief Swaps the byte order of an integer.
 * @tparam Type An integral type (either signed or unsigned).
 * @param value The value to swap the byte order for.
 * @return The value with its byte order reversed.
 */
template <meta::Integral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type byteswap(Type value) noexcept {
    using UType = meta::MakeUnsignedT<Type>;
    if constexpr (sizeof(Type) == 1) {
        return value;
    } else if constexpr (sizeof(Type) == 2) {
#if NEX_HAS_BUILTIN(__builtin_bswap16)
        return NEX_BIT_CAST<Type>(__builtin_bswap16(NEX_BIT_CAST<UType>(value)));
#elif NEX_COMPILER_IS_MSVC
        return NEX_BIT_CAST<Type>(_byteswap_ushort(NEX_BIT_CAST<UType>(value)));
#else
        return _byteswapFallback_Impl(value);
#endif  // ^^^for uint16
    } else if constexpr (sizeof(Type) == 4) {
#if NEX_HAS_BUILTIN(__builtin_bswap32)
        return NEX_BIT_CAST<Type>(__builtin_bswap32(NEX_BIT_CAST<UType>(value)));
#elif NEX_COMPILER_IS_MSVC
        return NEX_BIT_CAST<Type>(_byteswap_ulong(NEX_BIT_CAST<UType>(value)));
#else
        return _byteswapFallback_Impl(value);
#endif  // ^^^for uint32
    } else if constexpr (sizeof(Type) == 8) {
#if NEX_HAS_BUILTIN(__builtin_bswap64)
        return NEX_BIT_CAST<Type>(__builtin_bswap64(NEX_BIT_CAST<UType>(value)));
#elif NEX_COMPILER_IS_MSVC
        return NEX_BIT_CAST<Type>(_byteswap_uint64(NEX_BIT_CAST<UType>(value)));
#else
        return _byteswapFallback_Impl(value);
#endif  // ^^^for uint64
#if NEX_HAS_BUILTIN_INT128
    } else if constexpr (sizeof(Type) == 16) {
    #if NEX_HAS_BUILTIN(__builtin_bswap128)
        return NEX_BIT_CAST<Type>(__builtin_bswap128(NEX_BIT_CAST<UType>(value)));
    #else
        UType v = NEX_BIT_CAST<UType>(value);
        const uint64 high = static_cast<uint64>(v >> 64);
        const uint64 low  = static_cast<uint64>(v);
        return NEX_BIT_CAST<Type>(
            (static_cast<UType>(byteswap(low)) << 64) | static_cast<UType>(byteswap(high))
        );
    #endif // ^^^NEX_HAS_BUILTIN(__builtin_bswap128)
#endif   // ^^^NEX_HAS_BUILTIN_INT128
    } else {
        static_assert(sizeof(Type) == 0, "Error: byteswap is unimplemented for integral types of this size");
    }
}

/**
 * @brief Converts an integer from host byte order to big-endian byte order.
 * @tparam Type An integral type (either signed or unsigned).
 * @param value The value to convert.
 * @return The value in big-endian byte order.
 */
template <meta::Integral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type hostToBigEndian(Type value) noexcept {
    if constexpr (isBigEndian()) {
        return value;
    } else {
        return byteswap(value);
    }
}

/**
 * @brief Converts an integer from host byte order to little-endian byte order.
 * @tparam Type An integral type (either signed or unsigned).
 * @param value The value to convert.
 * @return The value in little-endian byte order.
 */
template <meta::Integral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type hostToLittleEndian(Type value) noexcept {
    if constexpr (isLittleEndian()) {
        return value;
    } else {
        return byteswap(value);
    }
}

/**
 * @brief Converts an integer from big-endian byte order to host byte order.
 * @tparam Type An integral type (either signed or unsigned).
 * @param value The value to convert.
 * @return The value in host byte order.
 */
template <meta::Integral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type bigEndianToHost(Type value) noexcept {
    return hostToBigEndian(value);
}

/**
 * @brief Converts an integer from little-endian byte order to host byte order.
 * @tparam Type An integral type (either signed or unsigned).
 * @param value The value to convert.
 * @return The value in host byte order.
 */
template <meta::Integral Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type littleEndianToHost(Type value) noexcept {
    return hostToLittleEndian(value);
}

NEX_NAMESPACE_END
