/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <stdio.h>

#include "nex/base/macros.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(charconv)

// Common bases for integer formatting
constexpr i32 BASE_BINARY               = 2;    // Binary base (base 2)
constexpr i32 BASE_OCTAL                = 8;    // Octal base (base 8)
constexpr i32 BASE_DECIMAL              = 10;   // Decimal base (base 10)
constexpr i32 BASE_HEXADECIMAL          = 16;   // Hexadecimal base (base 16)
constexpr i32 BASE_HEXATRIGESIMAL       = 36;   // Hexatrigesimal, maximum base (base 36, using digits and letters)

// Default precision for floating-point formatting
constexpr i32 DEFAULT_FLOAT_PRECISION   = 6;

NEX_DETAIL_NAMESPACE_BEGIN

// Clear a character buffer by setting the first character to null terminator
NEX_HIDDEN_FROM_ABI inline
void clearBuffer(ncptr buffer, usize bufferSize) noexcept {
    if (buffer != nullptr && bufferSize != 0) {
        buffer[0] = '\0';
    }
}

// Format a 64-bit unsigned integer value into a string representation (internal implementation)
NEX_HIDDEN_FROM_ABI inline
usize formatU64Impl(ncptr buffer, usize bufferSize, u64 value, i32 base, bool uppercase, bool negative) noexcept {
    if (buffer == nullptr || bufferSize == 0 || base < BASE_BINARY || base > BASE_HEXATRIGESIMAL) {
        clearBuffer(buffer, bufferSize);
        return 0;
    }

    constexpr nchar lowerDigits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    constexpr nchar upperDigits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    cstring const digits = uppercase ? upperDigits : lowerDigits;

    nchar reversed[64];
    usize digitCount = 0;
    do {
        reversed[digitCount++] = digits[value % static_cast<u64>(base)];
        value /= static_cast<u64>(base);
    } while (value != 0);

    const usize length = digitCount + (negative ? 1 : 0);
    if (bufferSize <= length) {
        clearBuffer(buffer, bufferSize);
        return 0;
    }

    usize position = 0;
    if (negative) {
        buffer[position++] = '-';
    }
    while (digitCount != 0) {
        buffer[position++] = reversed[--digitCount];
    }
    buffer[position] = '\0';
    return position;
}

#if NEX_HAS_BUILTIN_INT128
// Format a 128-bit unsigned integer value into a string representation (internal implementation)
NEX_HIDDEN_FROM_ABI inline
usize formatU128Impl(ncptr buffer, usize bufferSize, u128 value, i32 base, bool uppercase, bool negative) noexcept {
    if (buffer == nullptr || bufferSize == 0 || base < BASE_BINARY || base > BASE_HEXATRIGESIMAL) {
        clearBuffer(buffer, bufferSize);
        return 0;
    }

    constexpr nchar lowerDigits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    constexpr nchar upperDigits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    cstring const digits = uppercase ? upperDigits : lowerDigits;

    // A 128-bit value requires at most 128 binary digits, plus an optional sign.
    nchar reversed[128];
    usize digitCount = 0;
    do {
        reversed[digitCount++] = digits[value % static_cast<u128>(base)];
        value /= static_cast<u128>(base);
    } while (value != 0);

    const usize length = digitCount + (negative ? 1 : 0);
    if (bufferSize <= length) {
        clearBuffer(buffer, bufferSize);
        return 0;
    }

    usize position = 0;
    if (negative) {
        buffer[position++] = '-';
    }
    while (digitCount != 0) {
        buffer[position++] = reversed[--digitCount];
    }
    buffer[position] = '\0';
    return position;
}
#endif

NEX_DETAIL_NAMESPACE_END

/**
 * @brief Format an integer value into a string representation.
 * @param buffer The character buffer to write the formatted string into.
 * @param bufferSize The size of the character buffer.
 * @param value The integer value to format.
 * @param base The numerical base for formatting (default is 10).
 * @param uppercase Whether to use uppercase letters for bases greater than 10 (default is false).
 * @return The number of characters written to the buffer, excluding the null terminator, or 0 on failure.
 * @note The buffer is null-terminated on success. Invalid bases and insufficient buffer capacity produce an empty string.
 */
NEX_NODISCARD NEX_INTERNAL inline
usize formatInteger(ncptr buffer, usize bufferSize, i64 value, i32 base = BASE_DECIMAL, bool uppercase = false) noexcept {
    const bool negative = value < 0;
    // Negating the minimum signed value would overflow, so convert its magnitude
    // by first moving it one step toward zero.
    const u64 magnitude = negative
        ? static_cast<u64>(-(value + 1)) + 1
        : static_cast<u64>(value);
    return NEX_DETAIL formatU64Impl(buffer, bufferSize, magnitude, base, uppercase, negative);
}

/**
 * @brief Format an unsigned integer value into a string representation.
 * @param buffer The character buffer to write the formatted string into.
 * @param bufferSize The size of the character buffer.
 * @param value The unsigned integer value to format.
 * @param base The numerical base for formatting (default is BASE_DECIMAL).
 * @param uppercase Whether to use uppercase letters for bases greater than 10 (default is false).
 * @return The number of characters written to the buffer, excluding the null terminator, or 0 on failure.
 * @note The buffer is null-terminated on success. Invalid bases and insufficient buffer capacity produce an empty string.
 */
NEX_NODISCARD NEX_INTERNAL inline
usize formatUnsigned(ncptr buffer, usize bufferSize, u64 value, i32 base = BASE_DECIMAL, bool uppercase = false) noexcept {
    return NEX_DETAIL formatU64Impl(buffer, bufferSize, value, base, uppercase, false);
}

#if NEX_HAS_BUILTIN_INT128
/**
 * @brief Format a 128-bit signed integer value into a string representation.
 * @param buffer The character buffer to write the formatted string into.
 * @param bufferSize The size of the character buffer.
 * @param value The integer value to format.
 * @param base The numerical base for formatting (default is BASE_DECIMAL).
 * @param uppercase Whether to use uppercase letters for bases greater than 10 (default is false).
 * @return The number of characters written to the buffer, excluding the null terminator, or 0 on failure.
 * @note The buffer is null-terminated on success. Invalid bases and insufficient buffer capacity produce an empty string.
 */
NEX_NODISCARD NEX_INTERNAL inline
usize formatInteger(ncptr buffer, usize bufferSize, i128 value, i32 base = BASE_DECIMAL, bool uppercase = false) noexcept {
    const bool negative = value < 0;
    const u128 magnitude = negative
        ? static_cast<u128>(-(value + 1)) + 1
        : static_cast<u128>(value);
    return NEX_DETAIL formatU128Impl(buffer, bufferSize, magnitude, base, uppercase, negative);
}

/**
 * @brief Format a 128-bit unsigned integer value into a string representation.
 * @param buffer The character buffer to write the formatted string into.
 * @param bufferSize The size of the character buffer.
 * @param value The unsigned integer value to format.
 * @param base The numerical base for formatting (default is BASE_DECIMAL).
 * @param uppercase Whether to use uppercase letters for bases greater than 10 (default is false).
 * @return The number of characters written to the buffer, excluding the null terminator, or 0 on failure.
 * @note The buffer is null-terminated on success. Invalid bases and insufficient buffer capacity produce an empty string.
 */
NEX_NODISCARD NEX_INTERNAL inline
usize formatUnsigned(ncptr buffer, usize bufferSize, u128 value, i32 base = BASE_DECIMAL, bool uppercase = false) noexcept {
    return NEX_DETAIL formatU128Impl(buffer, bufferSize, value, base, uppercase, false);
}
#endif

/**
 * @brief Format a floating-point value into a string representation.
 * @param buffer The character buffer to write the formatted string into.
 * @param bufferSize The size of the character buffer.
 * @param value The floating-point value to format.
 * @param precision The number of digits after the decimal point (default is 6).
 * @return The number of characters written to the buffer, excluding the null terminator, or 0 on failure.
 * @note The output uses fixed-point notation and is null-terminated on success.
 */
NEX_NODISCARD NEX_INTERNAL inline
usize formatFloating(ncptr buffer, usize bufferSize, f64 value, i32 precision = DEFAULT_FLOAT_PRECISION) noexcept {
    if (buffer == nullptr || bufferSize == 0) {
        NEX_DETAIL clearBuffer(buffer, bufferSize);
        return 0;
    }

    const i32 safePrecision = precision < 0 ? 0 : precision;
    const i32 length = snprintf(buffer, bufferSize, "%.*f", safePrecision, value);
    if (length < 0 || static_cast<usize>(length) >= bufferSize) {
        NEX_DETAIL clearBuffer(buffer, bufferSize);
        return 0;
    }
    return static_cast<usize>(length);
}

#if NEX_HAS_BUILTIN_FLOAT16
/**
 * @brief Format a 16-bit floating-point value into a string representation.
 * @param buffer The character buffer to write the formatted string into.
 * @param bufferSize The size of the character buffer.
 * @param value The floating-point value to format.
 * @param precision The number of digits after the decimal point (default is 6).
 * @return The number of characters written to the buffer, excluding the null terminator, or 0 on failure.
 * @note The buffer is null-terminated on success.
 */
NEX_NODISCARD NEX_INTERNAL inline
usize formatFloating(ncptr buffer, usize bufferSize, f16 value, i32 precision = DEFAULT_FLOAT_PRECISION) noexcept {
    // Every float16 value is represented exactly by float64.
    return formatFloating(buffer, bufferSize, static_cast<f64>(value), precision);
}
#endif

#if NEX_HAS_BUILTIN_FLOAT128
/**
 * @brief Format a 128-bit floating-point value into a string representation.
 * @param buffer The character buffer to write the formatted string into.
 * @param bufferSize The size of the character buffer.
 * @param value The floating-point value to format.
 * @param precision The number of digits after the decimal point (default is 6).
 * @return The number of characters written to the buffer, excluding the null terminator, or 0 on failure.
 * @note The buffer is null-terminated on success. Precision is limited by the platform's long double format.
 */
NEX_NODISCARD NEX_INTERNAL inline
usize formatFloating(ncptr buffer, usize bufferSize, f128 value, i32 precision = DEFAULT_FLOAT_PRECISION) noexcept {
    if (buffer == nullptr || bufferSize == 0) {
        NEX_DETAIL clearBuffer(buffer, bufferSize);
        return 0;
    }

    const i32 safePrecision = precision < 0 ? 0 : precision;
    const i32 length = snprintf(buffer, bufferSize, "%.*Lf", safePrecision, static_cast<ldouble>(value));
    if (length < 0 || static_cast<usize>(length) >= bufferSize) {
        NEX_DETAIL clearBuffer(buffer, bufferSize);
        return 0;
    }
    return static_cast<usize>(length);
}
#endif

/**
 * @brief Format a boolean value into a string representation.
 * @param buffer The character buffer to write the formatted string into.
 * @param bufferSize The size of the character buffer.
 * @param value The boolean value to format.
 * @return The number of characters written to the buffer, excluding the null terminator, or 0 on failure.
 * @note The buffer is null-terminated on success.
 */
NEX_NODISCARD NEX_INTERNAL inline
usize formatBoolean(ncptr buffer, usize bufferSize, bool value) noexcept {
    if (buffer == nullptr || bufferSize == 0) {
        NEX_DETAIL clearBuffer(buffer, bufferSize);
        return 0;
    }

    constexpr nchar trueValue[] = "true";
    constexpr nchar falseValue[] = "false";
    cstring const text = value ? trueValue : falseValue;
    const usize length = value ? 4 : 5;

    if (bufferSize <= length) {
        NEX_DETAIL clearBuffer(buffer, bufferSize);
        return 0;
    }

    for (usize index = 0; index < length; ++index) {
        buffer[index] = text[index];
    }
    buffer[length] = '\0';
    return length;
}

NEX_SUBNAMESPACE_END(charconv)

NEX_NAMESPACE_END
