/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  limits.h
 * @brief Custom, zero-dependency compile-time numerical limits and type traits.
 * 
 * @details 
 * Portable alternative to platform-specific system headers. Designed for freestanding, bare-metal, 
 * and multi-platform environments.Guaranteed zero runtime overhead via `constexpr` evaluations.
 */

#include "nex/base/platform.h"
#include "nex/base/compiler.h"
#include "nex/base/build.h"
#include "nex/base/attributes.h"
#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// A collection of compile-time constants for numerical limits
// =================================================================================

struct NumericLimitConstants {

    // =================================================================================
    // Integer Limits (Two's Complement)
    // =================================================================================

    /**
     * @note
     * The minimum value of a signed integer type is defined as -(2^(N-1)) where N is the number 
     * of bits in the type. This is because one bit is used for the sign, and the remaining bits 
     * represent the magnitude. 
     * The maximum value of a signed integer type is defined as 2^(N-1) - 1, which accounts for 
     * the fact that zero is included in the range of representable values.
     */

    static constexpr int8   i8min       = (-127_i8 - 1_i8);
    static constexpr int16  i16min      = (-32767_i16 - 1_i16);
    static constexpr int32  i32min      = (-2147483647_i32 - 1_i32);
    static constexpr int64  i64min      = (-9223372036854775807_i64 - 1_i64);

    static constexpr int8   i8max       = 127_i8;
    static constexpr int16  i16max      = 32767_i16;
    static constexpr int32  i32max      = 2147483647_i32;
    static constexpr int64  i64max      = 9223372036854775807_i64;

    static constexpr uint8  u8max       = static_cast<uint8>(~static_cast<uint8>(0));
    static constexpr uint16 u16max      = static_cast<uint16>(~static_cast<uint16>(0));
    static constexpr uint32 u32max      = ~static_cast<uint32>(0);
    static constexpr uint64 u64max      = ~static_cast<uint64>(0);

#if NEX_HAS_BUILTIN_INT128
    static constexpr int128  i128min    = (static_cast<int128>(-1) << 127_i128);
    static constexpr int128  i128max    = ~(static_cast<int128>(-1) << 127_i128);
    static constexpr uint128 u128max    = ~static_cast<uint128>(0);
#endif  // ^^NEX_HAS_BUILTIN_INT128

#if NEX_BUILD_ENV_IS_64_BIT
    static constexpr uint64 sizemax     = ~static_cast<uintptr>(0_u64);
#else  // Non-64-bit environment, assume 32-bit
    static constexpr uint32 sizemax     = 0xffffffff_u32;
#endif

    // =================================================================================
    // Floating-point Limits (IEEE 754)
    // =================================================================================

    // Minimum positive normal value for IEEE 754 half-precision binary16
    static constexpr float16 f16min                 = 6.10351562e-5_f16;
    // Maximum finite value for IEEE 754 half-precision binary16
    static constexpr float16 f16max                 = 65504.0_f16;
    // Minimum finite value for IEEE 754 half-precision binary16
    static constexpr float16 f16lowest              = -65504.0_f16;

    /**
     * @note 
     * Since float16 does not support infinity or NaN in the same way 
     * as float32/float64, we do not define those here.
     */
    static constexpr float16 f16epsilon             = 0.0009765625_f16;
    // Half of epsilon, maximum rounding error for float16
    static constexpr float16 f16roundError          = 0.00048828125_f16;
    // Minimum positive subnormal value for IEEE 754 half-precision binary16
    static constexpr float16 f16denormMin           = 5.9604644775390625e-8_f16;
    // Positive infinity for IEEE 754 half-precision binary16 (exponent all 1s, fraction all 0s)
    static constexpr float16 f16infinity            = static_cast<float16>(0x7c00_u16);
    // Quiet NaN for IEEE 754 half-precision binary16 (exponent all 1s, fraction non-zero with leading bit 1)
    static constexpr float16 f16quietNaN            = static_cast<float16>(0x7e00_u16);
    // Signaling NaN for IEEE 754 half-precision binary16 (exponent all 1s, fraction non-zero with leading bit 0)
    static constexpr float16 f16signalingNaN        = static_cast<float16>(0x7d00_u16);
    // Minimum positive subnormal value for IEEE 754 half-precision binary16
    static constexpr float16 f16minPositive         = f16denormMin;
    static constexpr float16 f16maxFinite           = f16max;
    static constexpr float16 f16lowestFinite        = f16lowest;
    // Maximum exponent value for normal numbers in float16
    static constexpr float16 f16maxExponent         = 15.0_f16;
    static constexpr float16 f16minExponent         = -14.0_f16;
    static constexpr float16 f16exponentBias        = 15.0_f16;     // Bias for the exponent in float16
    static constexpr float16 f16maxDigits           = 3.0_f16;
    static constexpr float16 f16minDigits           = 3.0_f16;
    static constexpr float16 f16maxDecimalDigits    = 5.0_f16;
    static constexpr float16 f16minDecimalDigits    = 5.0_f16;

    // Minimum positive normal value for IEEE 754 single-precision binary32
    static constexpr float32 f32min                 = 1.175494350822287507969e-38_f32;
    // Maximum finite value for IEEE 754 single-precision binary32
    static constexpr float32 f32max                 = 3.402823466385288598117e+38_f32;
    // Minimum finite value for IEEE 754 single-precision binary32
    static constexpr float32 f32lowest              = -3.402823466385288598117e+38_f32;
    // Difference between 1.0 and the next representable value for IEEE 754 single-precision binary32
    static constexpr float32 f32epsilon             = 1.192092895507812500000e-7_f32;
    // Half of epsilon, maximum rounding error for float32
    static constexpr float32 f32roundError          = 0.5_f32 * f32epsilon;
    // Minimum positive subnormal value for IEEE 754 single-precision binary32
    static constexpr float32 f32denormMin           = 1.401298464324817070923e-45_f32;

    // Positive infinity for IEEE 754 single-precision binary32 (exponent all 1s, fraction all 0s)
    static constexpr float32 f32infinity             = __builtin_huge_valf();
    // Quiet NaN for IEEE 754 single-precision binary32 (exponent all 1s, fraction non-zero with leading bit 1)
    static constexpr float32 f32quietNaN             = __builtin_nanf("");
    // Signaling NaN for IEEE 754 single-precision binary32 (exponent all 1s, fraction non-zero with leading bit 0)
    static constexpr float32 f32signalingNaN         = __builtin_nansf("");

    // Minimum positive normal value for IEEE 754 single-precision binary32
    static constexpr float32 f32minPositive         = f32denormMin;
    static constexpr float32 f32maxFinite           = f32max;
    static constexpr float32 f32lowestFinite        = f32lowest;
    static constexpr float32 f32maxExponent         = 127.0_f32;
    static constexpr float32 f32minExponent         = -126.0_f32;
    static constexpr float32 f32exponentBias        = 127.0_f32;    // Bias for the exponent in float32
    static constexpr float32 f32maxDigits           = 6.0_f32;
    static constexpr float32 f32minDigits           = 6.0_f32;
    static constexpr float32 f32maxDecimalDigits    = 9.0_f32;
    static constexpr float32 f32minDecimalDigits    = 9.0_f32;

    // Minimum positive normal value for IEEE 754 double-precision binary64
    static constexpr float64 f64min                 = 2.225073858507201383090e-308_f64;
    // Maximum finite value for IEEE 754 double-precision binary64
    static constexpr float64 f64max                 = 1.797693134862315708145e+308_f64;
    // Minimum finite value for IEEE 754 double-precision binary64
    static constexpr float64 f64lowest              = -1.797693134862315708145e+308_f64;
    // Difference between 1.0 and the next representable value for IEEE 754 double-precision binary64
    static constexpr float64 f64epsilon             = 2.220446049250313080847e-16_f64;
    // Half of epsilon, maximum rounding error for float64
    static constexpr float64 f64roundError          = 0.5_f64 * f64epsilon;
    // Minimum positive subnormal value for IEEE 754 double-precision binary64
    static constexpr float64 f64denormMin           = 4.940656458412465441765e-324_f64;

    // Positive infinity for IEEE 754 double-precision binary64 (exponent all 1s, fraction all 0s)
    static constexpr float64 f64infinity            = __builtin_huge_val();
    // Quiet NaN for IEEE 754 double-precision binary64 (exponent all 1s, fraction non-zero with leading bit 1)
    static constexpr float64 f64quietNaN            = __builtin_nan("");
    // Signaling NaN for IEEE 754 double-precision binary64 (exponent all 1s, fraction non-zero with leading bit 0)
    static constexpr float64 f64signalingNaN        = __builtin_nans("");

    // Minimum positive normal value for IEEE 754 double-precision binary64
    static constexpr float64 f64minPositive         = f64denormMin;
    static constexpr float64 f64maxFinite           = f64max;
    static constexpr float64 f64lowestFinite        = f64lowest;
    static constexpr float64 f64maxExponent         = 1023.0_f64;
    static constexpr float64 f64minExponent         = -1022.0_f64;
    static constexpr float64 f64exponentBias        = 1023.0_f64;   // Bias for the exponent in float64
    static constexpr float64 f64maxDigits           = 15.0_f64;
    static constexpr float64 f64minDigits           = 15.0_f64;
    static constexpr float64 f64maxDecimalDigits    = 17.0_f64;
    static constexpr float64 f64minDecimalDigits    = 17.0_f64;

#if NEX_HAS_BUILTIN_FLOAT128
    // Minimum positive normal value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128min              = 3.36210314311209350626267781732175260e-4932q_f128;
    // Maximum finite value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128max              = 1.18973149535723176508575932662800702e+4932q_f128;
    // Minimum finite value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128lowest           = -1.18973149535723176508575932662800702e+4932q_f128;
    // Difference between 1.0 and the next representable value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128epsilon          = 1.92592994438723585305597794258492732e-34q_f128;
    // Half of epsilon, maximum rounding error for float128
    static constexpr float128 f128roundError       = 0.5q_f128 * f128epsilon;
    // Minimum positive subnormal value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128denormMin        = 6.47517511943802511092443895822764655e-4966q_f128;

    // Positive infinity for IEEE 754 quadruple-precision binary128 (exponent all 1s, fraction all 0s)
    static constexpr float128 f128infinity         = __builtin_huge_valq();
    // Quiet NaN for IEEE 754 quadruple-precision binary128 (exponent all 1s, fraction non-zero with leading bit 1)
    static constexpr float128 f128quietNaN         = __builtin_nanq("");
    // Signaling NaN for IEEE 754 quadruple-precision binary128 (exponent all 1s, fraction non-zero with leading bit 0)
    static constexpr float128 f128signalingNaN     = __builtin_nansq("");

    // Minimum positive normal value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128minPositive      = f128denormMin;
    static constexpr float128 f128maxFinite        = f128max;
    static constexpr float128 f128lowestFinite     = f128lowest;
    static constexpr float128 f128maxExponent      = 16383.0q_f128;
    static constexpr float128 f128minExponent      = -16382.0q_f128;
    static constexpr float128 f128exponentBias     = 16383.0q_f128;  // Bias for the exponent in float128
    static constexpr float128 f128maxDigits        = 33.0q_f128;
    static constexpr float128 f128minDigits        = 33.0q_f128;
    static constexpr float128 f128maxDecimalDigits = 36.0q_f128;
    static constexpr float128 f128minDecimalDigits = 36.0q_f128;
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

    // =================================================================================
    // UTF Character Code Unit Constants (C++20 encoding-aware)
    // =================================================================================

    static constexpr char8 c8min        = static_cast<char8>(0x00);
    static constexpr char8 c8max        = static_cast<char8>(0xFF);
    static constexpr char16 c16min      = static_cast<char16>(0x0000);
    static constexpr char16 c16max      = static_cast<char16>(0xFFFF);
    static constexpr char32 c32min      = static_cast<char32>(0x00000000);
    static constexpr char32 c32max      = static_cast<char32>(0xFFFFFFFF);

    // =============================================================================
    // Native & Wide Character Constants (Platform/ABI Dependent)
    // =============================================================================

    /**
     * @note
     * The limits for nchar depend on the platform and ABI.
     * On most platforms, char is signed and has a range of -128 to 127. 
     * However, on some platforms (especially embedded or older ones), char may be unsigned 
     * with a range of 0 to 255.
     * To ensure our constants are correct regardless of the platform's signedness for char, 
     * we use a conditional expression that checks the signedness of char at compile time,
     * and computes the limits accordingly.
     */
    static constexpr nchar ncharmin     = (static_cast<nchar>(-1) < 0) 
                                            ? static_cast<nchar>(-128) : static_cast<nchar>(0);
    static constexpr nchar ncharmax     = (static_cast<nchar>(-1) < 0) 
                                            ? static_cast<nchar>(127)  : static_cast<nchar>(255);

#if NEX_PLATFORM_IS_WINDOWS
    // Windows ABI defines wchar_t/wint_t as 16-bit unsigned shorts
    static constexpr wchar wcharmin     = static_cast<wchar>(0x0000_u16);
    static constexpr wchar wcharmax     = static_cast<wchar>(0xffff_u16);
    static constexpr wint wintmin       = static_cast<wint>(0x0000_u16);
    static constexpr wint wintmax       = static_cast<wint>(0xffff_u16);
#else
    /**
     * @note
     * Unix/Linux/macOS/Android ABIs define wchar_t/wint_t as 32-bit (usually signed or unsigned 
     * depending on the OS). To be absolutely safe for all distributions, we use bit shifting 
     * and static casting to compute the limits based on the actual size of wchar and wint, 
     * rather than assuming signedness.
     */
    static constexpr wchar wcharmin     = (static_cast<wchar>(-1) < 0) 
                                            ? static_cast<wchar>(-2147483647 - 1) : static_cast<wchar>(0);
    static constexpr wchar wcharmax     = (static_cast<wchar>(-1) < 0) 
                                            ? static_cast<wchar>(2147483647) : static_cast<wchar>(0xffffffff_u32);
    
    static constexpr wint wintmin       = (static_cast<wint>(-1) < 0) 
                                            ? static_cast<wint>(-2147483647 - 1)  : static_cast<wint>(0);
    static constexpr wint wintmax       = (static_cast<wint>(-1) < 0) 
                                            ? static_cast<wint>(2147483647) : static_cast<wint>(0xffffffff_u32);
#endif

    // =============================================================================
    // Boolean Logical Constants
    // =============================================================================

    static constexpr boolean boolmin    = false;
    static constexpr boolean boolmax    = true;
};

// =================================================================================
// Macro definitions for fixed-width integer limits (C-style)
// =================================================================================

#define NEX_INT8_MIN            NEX_PREPEND_NAMESPACE(NumericLimitConstants::i8min)
#define NEX_INT16_MIN           NEX_PREPEND_NAMESPACE(NumericLimitConstants::i16min)
#define NEX_INT32_MIN           NEX_PREPEND_NAMESPACE(NumericLimitConstants::i32min)
#define NEX_INT64_MIN           NEX_PREPEND_NAMESPACE(NumericLimitConstants::i64min)
#define NEX_INT8_MAX            NEX_PREPEND_NAMESPACE(NumericLimitConstants::i8max)
#define NEX_INT16_MAX           NEX_PREPEND_NAMESPACE(NumericLimitConstants::i16max)
#define NEX_INT32_MAX           NEX_PREPEND_NAMESPACE(NumericLimitConstants::i32max)
#define NEX_INT64_MAX           NEX_PREPEND_NAMESPACE(NumericLimitConstants::i64max)
#define NEX_UINT8_MAX           NEX_PREPEND_NAMESPACE(NumericLimitConstants::u8max)
#define NEX_UINT16_MAX          NEX_PREPEND_NAMESPACE(NumericLimitConstants::u16max)
#define NEX_UINT32_MAX          NEX_PREPEND_NAMESPACE(NumericLimitConstants::u32max)
#define NEX_UINT64_MAX          NEX_PREPEND_NAMESPACE(NumericLimitConstants::u64max)

#define NEX_INT_LEAST8_MIN      NEX_INT8_MIN
#define NEX_INT_LEAST16_MIN     NEX_INT16_MIN
#define NEX_INT_LEAST32_MIN     NEX_INT32_MIN
#define NEX_INT_LEAST64_MIN     NEX_INT64_MIN
#define NEX_INT_LEAST8_MAX      NEX_INT8_MAX
#define NEX_INT_LEAST16_MAX     NEX_INT16_MAX
#define NEX_INT_LEAST32_MAX     NEX_INT32_MAX
#define NEX_INT_LEAST64_MAX     NEX_INT64_MAX
#define NEX_UINT_LEAST8_MAX     NEX_UINT8_MAX
#define NEX_UINT_LEAST16_MAX    NEX_UINT16_MAX
#define NEX_UINT_LEAST32_MAX    NEX_UINT32_MAX
#define NEX_UINT_LEAST64_MAX    NEX_UINT64_MAX

#define NEX_INT_FAST8_MIN       NEX_INT8_MIN
#define NEX_INT_FAST16_MIN      NEX_INT16_MIN
#define NEX_INT_FAST32_MIN      NEX_INT32_MIN
#define NEX_INT_FAST64_MIN      NEX_INT64_MIN
#define NEX_INT_FAST8_MAX       NEX_INT8_MAX
#define NEX_INT_FAST16_MAX      NEX_INT16_MAX
#define NEX_INT_FAST32_MAX      NEX_INT32_MAX
#define NEX_INT_FAST64_MAX      NEX_INT64_MAX
#define NEX_UINT_FAST8_MAX      NEX_UINT8_MAX
#define NEX_UINT_FAST16_MAX     NEX_UINT16_MAX
#define NEX_UINT_FAST32_MAX     NEX_UINT32_MAX
#define NEX_UINT_FAST64_MAX     NEX_UINT64_MAX

#if NEX_BUILD_ENV_IS_64_BIT
    #define NEX_INTPTR_MIN      NEX_INT64_MIN
    #define NEX_INTPTR_MAX      NEX_INT64_MAX
    #define NEX_UINTPTR_MAX     NEX_UINT64_MAX
#else  // Non-64-bit environment, assume 32-bit
    #define NEX_INTPTR_MIN      NEX_INT32_MIN
    #define NEX_INTPTR_MAX      NEX_INT32_MAX
    #define NEX_UINTPTR_MAX     NEX_UINT32_MAX
#endif

#define NEX_INTMAX_MIN          NEX_INT64_MIN
#define NEX_INTMAX_MAX          NEX_INT64_MAX
#define NEX_UINTMAX_MAX         NEX_UINT64_MAX

// =================================================================================
// Macro definitions for pointer difference and size limits (C-style)
// =================================================================================

#define NEX_PTRDIFF_MIN         NEX_INTPTR_MIN
#define NEX_PTRDIFF_MAX         NEX_INTPTR_MAX

#define NEX_SIZE_MAX            NEX_PREPEND_NAMESPACE(NumericLimitConstants::sizemax)

// =================================================================================
// Macro definitions for signal atomic type limits (C-style)
// =================================================================================

#define NEX_SIG_ATOMIC_MIN      NEX_PREPEND_NAMESPACE(NumericLimitConstants::i32min)
#define NEX_SIG_ATOMIC_MAX      NEX_PREPEND_NAMESPACE(NumericLimitConstants::i32max)

// =================================================================================
// Macro definitions for wide character limits (C-style)
// =================================================================================

#define NEX_WCHAR_MIN           NEX_PREPEND_NAMESPACE(NumericLimitConstants::wcharmin)
#define NEX_WCHAR_MAX           NEX_PREPEND_NAMESPACE(NumericLimitConstants::wcharmax)

#define NEX_WINT_MIN            NEX_PREPEND_NAMESPACE(NumericLimitConstants::wintmin)
#define NEX_WINT_MAX            NEX_PREPEND_NAMESPACE(NumericLimitConstants::wintmax)

// =================================================================================
// Base template for numeric limits (C++-style)
// =================================================================================

template<typename Type>
struct NumericLimitsBase {
    static_assert(sizeof(Type) == 0, "NumericLimits is not specialized for this type");

    // Indicates whether limits are defined for this type
    static constexpr bool isSpecialized     = false;
    // Indicates whether the type is signed (only meaningful for integer types)
    static constexpr bool isSigned          = false;
    // Indicates whether the type is an integer type 
    // (true for integral types, false for floating-point types)
    static constexpr bool isInteger         = false;
    // Indicates whether the type is an exact type 
    // (true for integer types, false for floating-point types)
    static constexpr bool isExact           = false;
    // Indicates whether the type supports infinity 
    // (only meaningful for floating-point types)
    static constexpr bool hasInfinity       = false;
    // Indicates whether the type supports quiet NaN 
    // (only meaningful for floating-point types)
    static constexpr bool hasQuietNaN       = false;
    // Indicates whether the type supports signaling NaN 
    // (only meaningful for floating-point types)
    static constexpr bool hasSignalingNaN   = false;
    // Indicates whether the type supports denormalized numbers 
    // (only meaningful for floating-point types)
    static constexpr bool hasDenorm         = false;
    // Indicates whether loss of precision is detected when denormalized numbers are used 
    // (only meaningful for floating-point types)
    static constexpr bool hasDenormLoss     = false;
    // Indicates whether the type has finite bounds (true for all built-in types)
    static constexpr bool isBounded         = false;
    // Indicates whether the type is a modulo type 
    // (true for unsigned integer types, false for signed integer and floating-point types)
    static constexpr bool isModulo          = false;
};

// =================================================================================
// Specializations of NumericLimitsBase for built-in types (C++-style)
// =================================================================================

template<typename Type>
struct IntegerLimitsBase : public NumericLimitsBase<Type> {
    static_assert(sizeof(Type) == 0, "IntegerLimits is not specialized for this type");

    // Indicates whether limits are defined for this type
    static constexpr bool isSpecialized     = true;
    // Indicates whether the type is signed 
    // (true for signed integer types, false for unsigned integer types)
    static constexpr bool isSigned          = (Type(-1) < Type(0));
    // Indicates whether the type is an integer type
    static constexpr bool isInteger         = true;
    // Indicates whether the type is an exact type
    static constexpr bool isExact           = true;
    // Indicates whether the type has finite bounds
    static constexpr bool isBounded         = true;
    // Indicates whether the type is a modulo type 
    // (true for unsigned integer types, false for signed integer types)
    static constexpr bool isModulo          = !isSigned;
};

// =================================================================================
// Specializations of NumericLimitsBase for floating-point types (C++-style)
// =================================================================================

template<typename Type>
struct FloatingPointLimitsBase : public NumericLimitsBase<Type> {
    static_assert(sizeof(Type) == 0, "FloatingPointLimits is not specialized for this type");

    // Indicates whether limits are defined for this type
    static constexpr bool isSpecialized     = true;
    // Indicates whether the type is signed 
    // (all floating-point types are signed)
    static constexpr bool isSigned          = true;
    // Indicates whether the type is an integer type 
    // (false for all floating-point types)
    static constexpr bool isInteger         = false;
    // Indicates whether the type is an exact type 
    // (false for all floating-point types)
    static constexpr bool isExact           = false;
    // Indicates whether the type supports infinity 
    // (true for all floating-point types)
    static constexpr bool hasInfinity       = true;
    // Indicates whether the type supports quiet NaN 
    // (true for all floating-point types)
    static constexpr bool hasQuietNaN       = true;
    // Indicates whether the type supports signaling NaN 
    // (true for all floating-point types)
    static constexpr bool hasSignalingNaN   = true;
    // Indicates whether the type supports denormalized numbers 
    // (true for all floating-point types)
    static constexpr bool hasDenorm         = true;
    // Indicates whether loss of precision is detected when denormalized numbers are used 
    // (true for all floating-point types that support denormals)
    static constexpr bool hasDenormLoss     = true;
    // Indicates whether the type has finite bounds 
    // (true for all built-in types, including floating-point)
    static constexpr bool isBounded         = true;
};

// =================================================================================
// Primary template for NumericLimits (C++-style)
// =================================================================================

/**
 * @note
 * Undefine min/max macros if defined (to avoid conflicts with NumericLimits::min/max)
 * This is necessary because some platforms (notably Windows) define min and max as macros, 
 * which can interfere with our constexpr functions and constants. By undefining them here, 
 * we ensure that our code can use min and max as identifiers without issues.
 */

#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

template<typename Type>
struct NumericLimits : public NumericLimitsBase<Type> {
    static_assert(sizeof(Type) == 0, "NumericLimits is not specialized for this type");

    NEX_NODISCARD static constexpr Type min() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type max() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type lowest() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type epsilon() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type roundError() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type denormMin() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type infinity() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type quietNaN() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type signalingNaN() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type minPositive() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type maxFinite() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type lowestFinite() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type maxExponent() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type minExponent() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type exponentBias() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type maxDigits() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type minDigits() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type maxDecimalDigits() noexcept { return Type(0); }
    NEX_NODISCARD static constexpr Type minDecimalDigits() noexcept { return Type(0); }
};

NEX_NAMESPACE_END