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

    static constexpr int8   i8min           = (-127_i8 - 1_i8);
    static constexpr int16  i16min          = (-32767_i16 - 1_i16);
    static constexpr int32  i32min          = (-2147483647_i32 - 1_i32);
    static constexpr int64  i64min          = (-9223372036854775807_i64 - 1_i64);

    static constexpr int8   i8max           = 127_i8;
    static constexpr int16  i16max          = 32767_i16;
    static constexpr int32  i32max          = 2147483647_i32;
    static constexpr int64  i64max          = 9223372036854775807_i64;

    static constexpr uint8  u8max           = static_cast<uint8>(~static_cast<uint8>(0));
    static constexpr uint16 u16max          = static_cast<uint16>(~static_cast<uint16>(0));
    static constexpr uint32 u32max          = ~static_cast<uint32>(0);
    static constexpr uint64 u64max          = ~static_cast<uint64>(0);

#if NEX_HAS_BUILTIN_INT128
    static constexpr int128  i128min        = (static_cast<int128>(-1) << 127_i128);
    static constexpr int128  i128max        = ~(static_cast<int128>(-1) << 127_i128);
    static constexpr uint128 u128max        = ~static_cast<uint128>(0);
#endif  // ^^NEX_HAS_BUILTIN_INT128

    static constexpr sshort sshmin          = i16min;  // no cast needed since sshort is int16
    static constexpr sshort sshmax          = i16max;  // no cast needed since sshort is int16
    static constexpr ushort ushmax          = u16max;  // no cast needed since ushort is uint16
    static constexpr sint simin             = i32min;  // no cast needed since sint is int32
    static constexpr sint simax             = i32max;  // no cast needed since sint is int32
    static constexpr uint uimax             = u32max;  // no cast needed since uint is uint32
    static constexpr slong slmin            = static_cast<slong>(sizeof(slong) == 4 ? i32min : i64min);
    static constexpr slong slmax            = static_cast<slong>(sizeof(slong) == 4 ? i32max : i64max);
    static constexpr ulong ulmax            = static_cast<ulong>(sizeof(ulong) == 4 ? u32max : u64max);
    static constexpr longlong llmin         = i64min;  // no cast needed since longlong is int64
    static constexpr longlong llmax         = i64max;  // no cast needed since longlong is int64
    static constexpr ulonglong ullmax       = u64max;  // no cast needed since ulonglong is uint64

#if NEX_BUILD_ENV_IS_64_BIT
    static constexpr sizetype sizemax       = static_cast<sizetype>(u64max);
    static constexpr ptrdiff ptrdiffmin     = static_cast<ptrdiff>(i64min);
    static constexpr ptrdiff ptrdiffmax     = static_cast<ptrdiff>(i64max);
#else  // Non-64-bit environment, assume 32-bit
    static constexpr sizetype sizemax       = static_cast<sizetype>(u32max);
    static constexpr ptrdiff ptrdiffmin     = static_cast<ptrdiff>(i32min);
    static constexpr ptrdiff ptrdiffmax     = static_cast<ptrdiff>(i32max);
#endif  // ^^sizemax

    // =================================================================================
    // Floating-point Limits (IEEE 754)
    // =================================================================================

#if NEX_HAS_BUILTIN_FLOAT16
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

    static constexpr float16 f16maxFinite           = f16max;
    static constexpr float16 f16lowestFinite        = f16lowest;
    // Number of radix digits in the mantissa, including the implicit hidden bit
    static constexpr float16 f16binaryDigits        = 11.0_f16;
    // Minimum negative integer exponent such that 2^(X-1) is a normalized float
    static constexpr float16 f16minExponent         = -14.0_f16;
    // Maximum positive integer exponent such that 2^(X-1) is a finite float
    static constexpr float16 f16maxExponent         = 15.0_f16;
    // The exponent bias value used in the IEEE 754 representation of float16
    static constexpr float16 f16exponentBias        = 15.0_f16;
    // Minimum decimal precision: number of decimal digits that can be round-tripped without loss
    static constexpr float16 f16decimalDigits       = 3.0_f16;
    // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
    static constexpr float16 f16maxDecimalDigits    = 5.0_f16;
    // Minimum negative integer exponent such that 10^X is a normalized float
    static constexpr float16 f16minDecimalExponent  = -4.0_f16;
    // Maximum positive integer exponent such that 10^X is a finite float
    static constexpr float16 f16maxDecimalExponent  = 4.0_f16;
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

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

#if NEX_HAS_BUILTIN(__builtin_inff)
    // Positive infinity for IEEE 754 single-precision binary32 (exponent all 1s, fraction all 0s)
    static constexpr float32 f32infinity            = __builtin_inff();
#else    // Compilers does not support __builtin_inff
    // Positive infinity for IEEE 754 single-precision binary32 (exponent all 1s, fraction all 0s)
    static constexpr float32 f32infinity            = __builtin_huge_valf();
#endif  // ^^float32 f32infinity

    // Quiet NaN for IEEE 754 single-precision binary32 (exponent all 1s, fraction non-zero with leading bit 1)
    static constexpr float32 f32quietNaN            = __builtin_nanf("");
    // Signaling NaN for IEEE 754 single-precision binary32 (exponent all 1s, fraction non-zero with leading bit 0)
    static constexpr float32 f32signalingNaN        = __builtin_nansf("");

    static constexpr float32 f32maxFinite           = f32max;
    static constexpr float32 f32lowestFinite        = f32lowest;
    // Number of radix digits in the mantissa, including the implicit hidden bit
    static constexpr float32 f32binaryDigits        = 24.0_f32;
    // Minimum negative integer exponent such that 2^(X-1) is a normalized float
    static constexpr float32 f32minExponent         = -126.0_f32;
    // Maximum positive integer exponent such that 2^(X-1) is a finite float
    static constexpr float32 f32maxExponent         = 127.0_f32;
    // The exponent bias value used in the IEEE 754 representation of float32
    static constexpr float32 f32exponentBias        = 127.0_f32;
    // Minimum decimal precision: number of decimal digits that can be round-tripped without loss
    static constexpr float32 f32decimalDigits       = 6.0_f32;
    // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
    static constexpr float32 f32maxDecimalDigits    = 9.0_f32;
    // Minimum negative integer exponent such that 10^X is a normalized float
    static constexpr float32 f32minDecimalExponent  = -37.0_f32;
    // Maximum positive integer exponent such that 10^X is a finite float
    static constexpr float32 f32maxDecimalExponent  = 38.0_f32;

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

#if NEX_HAS_BUILTIN(__builtin_inf)
    // Positive infinity for IEEE 754 double-precision binary64 (exponent all 1s, fraction all 0s)
    static constexpr float64 f64infinity            = __builtin_inf();
#else  // Compilers does not support __builtin_inf
    // Positive infinity for IEEE 754 double-precision binary64 (exponent all 1s, fraction all 0s)
    static constexpr float64 f64infinity            = __builtin_huge_val();
#endif  // ^^float64 f64infinity

    // Quiet NaN for IEEE 754 double-precision binary64 (exponent all 1s, fraction non-zero with leading bit 1)
    static constexpr float64 f64quietNaN            = __builtin_nan("");
    // Signaling NaN for IEEE 754 double-precision binary64 (exponent all 1s, fraction non-zero with leading bit 0)
    static constexpr float64 f64signalingNaN        = __builtin_nans("");

    static constexpr float64 f64maxFinite           = f64max;
    static constexpr float64 f64lowestFinite        = f64lowest;
    // Number of radix digits in the mantissa, including the implicit hidden bit
    static constexpr float64 f64binaryDigits        = 53.0_f64;
    // Minimum negative integer exponent such that 2^(X-1) is a normalized float
    static constexpr float64 f64minExponent         = -1022.0_f64;
    // Maximum positive integer exponent such that 2^(X-1) is a finite float
    static constexpr float64 f64maxExponent         = 1023.0_f64;
    // The exponent bias value used in the IEEE 754 representation of float64
    static constexpr float64 f64exponentBias        = 1023.0_f64;
    // Minimum decimal precision: number of decimal digits that can be round-tripped without loss
    static constexpr float64 f64decimalDigits       = 15.0_f64;
    // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
    static constexpr float64 f64maxDecimalDigits    = 17.0_f64;
    // Minimum negative integer exponent such that 10^X is a normalized float
    static constexpr float64 f64minDecimalExponent  = -307.0_f64;
    // Maximum positive integer exponent such that 10^X is a finite float
    static constexpr float64 f64maxDecimalExponent  = 308.0_f64;

#if (NEX_SIZEOF_LONG_DOUBLE == 8)
    // Minimum positive normal value for IEEE 754 extended precision
    static constexpr ldouble ldmin                  = 2.22507385850720138309e-308_ld;
    // Maximum finite value for IEEE 754 extended precision
    static constexpr ldouble ldmax                  = 1.79769313486231570815e+308_ld;
    // Minimum finite value for IEEE 754 extended precision
    static constexpr ldouble ldlowest               = -1.79769313486231570815e+308_ld;
    // Difference between 1.0 and the next representable value for IEEE 754 extended precision
    static constexpr ldouble ldepsilon              = 2.22044604925031308085e-16_ld;
    // Half of epsilon, maximum rounding error for extended precision
    static constexpr ldouble ldroundError           = 0.5_ld * ldepsilon;
    // Minimum positive subnormal value for IEEE 754 extended precision
    static constexpr ldouble lddenormMin            = 4.94065645841246544177e-324_ld;
#else
    // Minimum positive normal value for IEEE 754 extended precision
    static constexpr ldouble ldmin                  = 3.36210314311209350626267781732175260e-4932_ld;
    // Maximum finite value for IEEE 754 extended precision
    static constexpr ldouble ldmax                  = 1.18973149535723176508575932662800702e+4932_ld;
    // Minimum finite value for IEEE 754 extended precision
    static constexpr ldouble ldlowest               = -1.18973149535723176508575932662800702e+4932_ld;
    // Difference between 1.0 and the next representable value for IEEE 754 extended precision
    static constexpr ldouble ldepsilon              = 1.92592994438723585305597794258492732e-34_ld;
    // Half of epsilon, maximum rounding error for extended precision
    static constexpr ldouble ldroundError           = 0.5_ld * ldepsilon;
    // Minimum positive subnormal value for IEEE 754 extended precision
    static constexpr ldouble lddenormMin            = 6.47517511943802511092443895822764655e-4966_ld;
#endif  // ^^^^ldouble

#if NEX_HAS_BUILTIN(__builtin_infl)
    // Positive infinity for IEEE 754 extended precision
    static constexpr ldouble ldinfinity             = __builtin_infl();
#else  // Compilers does not support __builtin_infl
    // Positive infinity for IEEE 754 extended precision
    static constexpr ldouble ldinfinity             = __builtin_huge_vall();
#endif  // ^^ldouble ldinfinity

    // Quiet NaN for IEEE 754 extended precision
    static constexpr ldouble ldquietNaN             = __builtin_nanl("");
    // Signaling NaN for IEEE 754 extended precision
    static constexpr ldouble ldsignalingNaN         = __builtin_nansl("");

    static constexpr ldouble ldmaxFinite            = ldmax;
    static constexpr ldouble ldlowestFinite         = ldlowest;

#if (NEX_SIZEOF_LONG_DOUBLE == 8)
    // Number of radix digits in the mantissa, including the implicit hidden bit
    static constexpr ldouble ldbinaryDigits         = 53.0_ld;
    // Minimum negative integer exponent such that 2^(X-1) is a normalized float
    static constexpr ldouble ldminExponent          = -1022.0_ld;
    // Maximum positive integer exponent such that 2^(X-1) is a finite float
    static constexpr ldouble ldmaxExponent          = 1023.0_ld;
    // The exponent bias value used in the IEEE 754 representation of extended precision
    static constexpr ldouble ldexponentBias         = 1023.0_ld;
    // Minimum decimal precision: number of decimal digits that can be round-tripped without loss
    static constexpr ldouble lddecimalDigits        = 15.0_ld;
    // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
    static constexpr ldouble ldmaxDecimalDigits     = 17.0_ld;
    // Minimum negative integer exponent such that 10^X is a normalized float
    static constexpr ldouble ldminDecimalExponent   = -307.0_ld;
    // Maximum positive integer exponent such that 10^X is a finite float
    static constexpr ldouble ldmaxDecimalExponent   = 308.0_ld;
#else
    // Number of radix digits in the mantissa, including the implicit hidden bit
    static constexpr ldouble ldbinaryDigits         = 113.0_ld;
    // Minimum negative integer exponent such that 2^(X-1) is a normalized float
    static constexpr ldouble ldminExponent          = -16382.0_ld;
    // Maximum positive integer exponent such that 2^(X-1) is a finite float
    static constexpr ldouble ldmaxExponent          = 16383.0_ld;
    // The exponent bias value used in the IEEE 754 representation of extended precision
    static constexpr ldouble ldexponentBias         = 16383.0_ld;
    // Minimum decimal precision: number of decimal digits that can be round-tripped without loss
    static constexpr ldouble lddecimalDigits        = 33.0_ld;
    // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
    static constexpr ldouble ldmaxDecimalDigits     = 36.0_ld;
    // Minimum negative integer exponent such that 10^X is a normalized float
    static constexpr ldouble ldminDecimalExponent   = -4931.0_ld;
    // Maximum positive integer exponent such that 10^X is a finite float
    static constexpr ldouble ldmaxDecimalExponent   = 4932.0_ld;
#endif  // ^^^^ldouble

#if NEX_HAS_BUILTIN_FLOAT128
    // Minimum positive normal value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128min               = 3.36210314311209350626267781732175260e-4932q_f128;
    // Maximum finite value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128max               = 1.18973149535723176508575932662800702e+4932q_f128;
    // Minimum finite value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128lowest            = -1.18973149535723176508575932662800702e+4932q_f128;
    // Difference between 1.0 and the next representable value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128epsilon           = 1.92592994438723585305597794258492732e-34q_f128;
    // Half of epsilon, maximum rounding error for float128
    static constexpr float128 f128roundError        = 0.5q_f128 * f128epsilon;
    // Minimum positive subnormal value for IEEE 754 quadruple-precision binary128
    static constexpr float128 f128denormMin         = 6.47517511943802511092443895822764655e-4966q_f128;

    // Positive infinity for IEEE 754 quadruple-precision binary128 (exponent all 1s, fraction all 0s)
    static constexpr float128 f128infinity          = __builtin_huge_valq();
    // Quiet NaN for IEEE 754 quadruple-precision binary128 (exponent all 1s, fraction non-zero with leading bit 1)
    static constexpr float128 f128quietNaN          = __builtin_nanq("");
    // Signaling NaN for IEEE 754 quadruple-precision binary128 (exponent all 1s, fraction non-zero with leading bit 0)
    static constexpr float128 f128signalingNaN      = __builtin_nansq("");

    static constexpr float128 f128maxFinite         = f128max;
    static constexpr float128 f128lowestFinite      = f128lowest;
    static constexpr float128 f128maxExponent      = 16383.0q_f128;
    static constexpr float128 f128minExponent      = -16382.0q_f128;
    // Bias for the exponent in float128 (exponent is stored as an unsigned value with this bias)
    static constexpr float128 f128exponentBias     = 16383.0q_f128;
    static constexpr float128 f128maxDigits        = 33.0q_f128;
    static constexpr float128 f128minDigits        = 33.0q_f128;
    static constexpr float128 f128maxDecimalDigits = 36.0q_f128;
    static constexpr float128 f128minDecimalDigits = 36.0q_f128;
    
    // Number of radix digits in the mantissa, including the implicit hidden bit
    static constexpr float128 f128binaryDigits      = 113.0q_f128;
    // Minimum negative integer exponent such that 2^(X-1) is a normalized float
    static constexpr float128 f128minExponent       = -16382.0q_f128;
    // Maximum positive integer exponent such that 2^(X-1) is a finite float
    static constexpr float128 f128maxExponent       = 16383.0q_f128;
    // The exponent bias value used in the IEEE 754 representation of float64
    static constexpr float128 f128exponentBias      = 16383.0q_f128;
    // Minimum decimal precision: number of decimal digits that can be round-tripped without loss
    static constexpr float128 f128decimalDigits     = 33.0q_f128;
    // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
    static constexpr float128 f128maxDecimalDigits  = 36.0q_f128;
    // Minimum negative integer exponent such that 10^X is a normalized float
    static constexpr float128 f128minDecimalExponent = -4931.0q_f128;
    // Maximum positive integer exponent such that 10^X is a finite float
    static constexpr float128 f128maxDecimalExponent = 4932.0q_f128;
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

    // =================================================================================
    // UTF Character Code Unit Constants (C++20 encoding-aware)
    // =================================================================================

#if NEX_HAS_BUILTIN_CHAR8_T
    static constexpr char8 c8min        = static_cast<char8>(0x00);
    static constexpr char8 c8max        = static_cast<char8>(0xff);
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

    static constexpr char16 c16min      = static_cast<char16>(0x0000);
    static constexpr char16 c16max      = static_cast<char16>(0xffff);
    static constexpr char32 c32min      = static_cast<char32>(0x00000000);
    static constexpr char32 c32max      = static_cast<char32>(0xffffffff);

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
#else  // Non-Windows ABIs
    /**
     * @note
     * Unix/Linux/macOS/Android ABIs define wchar_t/wint_t as 32-bit (usually signed or unsigned 
     * depending on the OS). To be absolutely safe for all distributions, we use bit shifting 
     * and static casting to compute the limits based on the actual size of wchar and wint, 
     * rather than assuming signedness.
     */
    static constexpr wchar wcharmin     = (static_cast<wchar>(-1) < 0) 
                                            ? static_cast<wchar>(-2147483647_u32 - 1) : static_cast<wchar>(0);
    static constexpr wchar wcharmax     = (static_cast<wchar>(-1) < 0) 
                                            ? static_cast<wchar>(2147483647_u32) : static_cast<wchar>(0xffffffff_u32);
    
    static constexpr wint wintmin       = (static_cast<wint>(-1) < 0) 
                                            ? static_cast<wint>(-2147483647_u32 - 1)  : static_cast<wint>(0);
    static constexpr wint wintmax       = (static_cast<wint>(-1) < 0) 
                                            ? static_cast<wint>(2147483647_u32) : static_cast<wint>(0xffffffff_u32);
#endif  // ^^wchar/wint limits

    // =============================================================================
    // Boolean Logical Constants
    // =============================================================================

    static constexpr boolean boolmin    = false;
    static constexpr boolean boolmax    = true;
};

// =================================================================================
// Macro definitions for fixed-width integer limits (C-style)
// =================================================================================

#define NEX_INT8_MIN                NEX_PREPEND_NAMESPACE(NumericLimitConstants::i8min)
#define NEX_INT16_MIN               NEX_PREPEND_NAMESPACE(NumericLimitConstants::i16min)
#define NEX_INT32_MIN               NEX_PREPEND_NAMESPACE(NumericLimitConstants::i32min)
#define NEX_INT64_MIN               NEX_PREPEND_NAMESPACE(NumericLimitConstants::i64min)
#define NEX_INT8_MAX                NEX_PREPEND_NAMESPACE(NumericLimitConstants::i8max)
#define NEX_INT16_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::i16max)
#define NEX_INT32_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::i32max)
#define NEX_INT64_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::i64max)
#define NEX_UINT8_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::u8max)
#define NEX_UINT16_MAX              NEX_PREPEND_NAMESPACE(NumericLimitConstants::u16max)
#define NEX_UINT32_MAX              NEX_PREPEND_NAMESPACE(NumericLimitConstants::u32max)
#define NEX_UINT64_MAX              NEX_PREPEND_NAMESPACE(NumericLimitConstants::u64max)

#if NEX_HAS_BUILTIN_INT128
    #define NEX_INT128_MIN          NEX_PREPEND_NAMESPACE(NumericLimitConstants::i128min)
    #define NEX_INT128_MAX          NEX_PREPEND_NAMESPACE(NumericLimitConstants::i128max)
    #define NEX_UINT128_MAX         NEX_PREPEND_NAMESPACE(NumericLimitConstants::u128max)
#endif  // ^^NEX_HAS_BUILTIN_INT128

#define NEX_INT_LEAST8_MIN          NEX_INT8_MIN
#define NEX_INT_LEAST16_MIN         NEX_INT16_MIN
#define NEX_INT_LEAST32_MIN         NEX_INT32_MIN
#define NEX_INT_LEAST64_MIN         NEX_INT64_MIN
#define NEX_INT_LEAST8_MAX          NEX_INT8_MAX
#define NEX_INT_LEAST16_MAX         NEX_INT16_MAX
#define NEX_INT_LEAST32_MAX         NEX_INT32_MAX
#define NEX_INT_LEAST64_MAX         NEX_INT64_MAX
#define NEX_UINT_LEAST8_MAX         NEX_UINT8_MAX
#define NEX_UINT_LEAST16_MAX        NEX_UINT16_MAX
#define NEX_UINT_LEAST32_MAX        NEX_UINT32_MAX
#define NEX_UINT_LEAST64_MAX        NEX_UINT64_MAX

#if NEX_HAS_BUILTIN_INT128
    #define NEX_INT_LEAST128_MIN    NEX_INT128_MIN
    #define NEX_INT_LEAST128_MAX    NEX_INT128_MAX
    #define NEX_UINT_LEAST128_MAX   NEX_UINT128_MAX
#endif  // ^^NEX_HAS_BUILTIN_INT128

#define NEX_INT_FAST8_MIN           NEX_INT8_MIN
#define NEX_INT_FAST16_MIN          NEX_INT16_MIN
#define NEX_INT_FAST32_MIN          NEX_INT32_MIN
#define NEX_INT_FAST64_MIN          NEX_INT64_MIN
#define NEX_INT_FAST8_MAX           NEX_INT8_MAX
#define NEX_INT_FAST16_MAX          NEX_INT16_MAX
#define NEX_INT_FAST32_MAX          NEX_INT32_MAX
#define NEX_INT_FAST64_MAX          NEX_INT64_MAX
#define NEX_UINT_FAST8_MAX          NEX_UINT8_MAX
#define NEX_UINT_FAST16_MAX         NEX_UINT16_MAX
#define NEX_UINT_FAST32_MAX         NEX_UINT32_MAX
#define NEX_UINT_FAST64_MAX         NEX_UINT64_MAX

#if NEX_HAS_BUILTIN_INT128
    #define NEX_INT_FAST128_MIN     NEX_INT128_MIN
    #define NEX_INT_FAST128_MAX     NEX_INT128_MAX
    #define NEX_UINT_FAST128_MAX    NEX_UINT128_MAX
#endif  // ^^NEX_HAS_BUILTIN_INT128

#if NEX_BUILD_ENV_IS_64_BIT
    #define NEX_INTPTR_MIN          NEX_INT64_MIN
    #define NEX_INTPTR_MAX          NEX_INT64_MAX
    #define NEX_UINTPTR_MAX         NEX_UINT64_MAX
#else  // Non-64-bit environment, assume 32-bit
    #define NEX_INTPTR_MIN          NEX_INT32_MIN
    #define NEX_INTPTR_MAX          NEX_INT32_MAX
    #define NEX_UINTPTR_MAX         NEX_UINT32_MAX
#endif  // ^^NEX_INTPTR_MIN/MAX / NEX_UINTPTR_MIN/MAX

#if NEX_HAS_BUILTIN_INT128
    #define NEX_INTMAX_MIN          NEX_INT128_MIN
    #define NEX_INTMAX_MAX          NEX_INT128_MAX
    #define NEX_UINTMAX_MAX         NEX_UINT128_MAX
#else  // Safe fallback to 64-bit integer limits
    #define NEX_INTMAX_MIN          NEX_INT64_MIN
    #define NEX_INTMAX_MAX          NEX_INT64_MAX
    #define NEX_UINTMAX_MAX         NEX_UINT64_MAX
#endif  // ^^NEX_HAS_BUILTIN_INT_128

// =================================================================================
// Macro definitions for common integer type limits (C-style)
// =================================================================================

#define NEX_SSHORT_MIN              NEX_PREPEND_NAMESPACE(NumericLimitConstants::sshmin)
#define NEX_SSHORT_MAX              NEX_PREPEND_NAMESPACE(NumericLimitConstants::sshmax)
#define NEX_USHORT_MAX              NEX_PREPEND_NAMESPACE(NumericLimitConstants::ushmax)

#define NEX_SINT_MIN                NEX_PREPEND_NAMESPACE(NumericLimitConstants::simin)
#define NEX_SINT_MAX                NEX_PREPEND_NAMESPACE(NumericLimitConstants::simax)
#define NEX_UINT_MAX                NEX_PREPEND_NAMESPACE(NumericLimitConstants::uimax)

#define NEX_SLONG_MIN               NEX_PREPEND_NAMESPACE(NumericLimitConstants::slmin)
#define NEX_SLONG_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::slmax)
#define NEX_ULONG_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::ulmax)

#define NEX_LONGLONG_MIN            NEX_PREPEND_NAMESPACE(NumericLimitConstants::llmin)
#define NEX_LONGLONG_MAX            NEX_PREPEND_NAMESPACE(NumericLimitConstants::llmax)
#define NEX_ULONGLONG_MAX           NEX_PREPEND_NAMESPACE(NumericLimitConstants::ullmax)

// =================================================================================
// Macro definitions for size and pointer difference limits (C-style)
// =================================================================================

#define NEX_SIZE_MAX                NEX_PREPEND_NAMESPACE(NumericLimitConstants::sizemax)

#define NEX_PTRDIFF_MIN             NEX_PREPEND_NAMESPACE(NumericLimitConstants::ptrdiffmin)
#define NEX_PTRDIFF_MAX             NEX_PREPEND_NAMESPACE(NumericLimitConstants::ptrdiffmax)

// =================================================================================
// Macro definitions for signal atomic type limits (C-style)
// =================================================================================

#define NEX_SIG_ATOMIC_MIN          NEX_PREPEND_NAMESPACE(NumericLimitConstants::i32min)
#define NEX_SIG_ATOMIC_MAX          NEX_PREPEND_NAMESPACE(NumericLimitConstants::i32max)

// =================================================================================
// Macro definitions for UTF character limits (C-style)
// =================================================================================

#if NEX_HAS_BUILTIN_CHAR8_T
    #define NEX_CHAR8_MIN           NEX_PREPEND_NAMESPACE(NumericLimitConstants::c8min)
    #define NEX_CHAR8_MAX           NEX_PREPEND_NAMESPACE(NumericLimitConstants::c8max)
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

#define NEX_CHAR16_MIN              NEX_PREPEND_NAMESPACE(NumericLimitConstants::c16min)
#define NEX_CHAR32_MIN              NEX_PREPEND_NAMESPACE(NumericLimitConstants::c16min)
#define NEX_CHAR16_MAX              NEX_PREPEND_NAMESPACE(NumericLimitConstants::c32max)
#define NEX_CHAR32_MAX              NEX_PREPEND_NAMESPACE(NumericLimitConstants::c32max)

#define NEX_CODEPOINT_MIN           NEX_PREPEND_NAMESPACE(NumericLimitConstants::c32min)
#define NEX_CODEPOINT_MAX           NEX_PREPEND_NAMESPACE(NumericLimitConstants::c32max)

// =================================================================================
// Macro definitions for narrow/native & wide character limits (C-style)
// =================================================================================

#define NEX_NCHAR_MIN               NEX_PREPEND_NAMESPACE(NumericLimitConstants::ncharmin)
#define NEX_NCHAR_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::ncharmax)

#define NEX_WCHAR_MIN               NEX_PREPEND_NAMESPACE(NumericLimitConstants::wcharmin)
#define NEX_WCHAR_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::wcharmax)

#define NEX_SCHAR_MIN               NEX_PREPEND_NAMESPACE(NumericLimitConstants::i8min)
#define NEX_SCHAR_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::i8max)
#define NEX_UCHAR_MAX               NEX_PREPEND_NAMESPACE(NumericLimitConstants::u8max)

#define NEX_WINT_MIN                NEX_PREPEND_NAMESPACE(NumericLimitConstants::wintmin)
#define NEX_WINT_MAX                NEX_PREPEND_NAMESPACE(NumericLimitConstants::wintmax)

// =================================================================================
// Macro definitions for floating-point type limits (C-style)
// =================================================================================

#if NEX_HAS_BUILTIN_FLOAT16
    #define NEX_FLOAT16_MIN         NEX_PREPEND_NAMESPACE(NumericLimitConstants::f16min)
    #define NEX_FLOAT16_MAX         NEX_PREPEND_NAMESPACE(NumericLimitConstants::f16max)
    #define NEX_FLOAT16_LOWEST      NEX_PREPEND_NAMESPACE(NumericLimitConstants::f16lowest)
    #define NEX_FLOAT16_EPSILON     NEX_PREPEND_NAMESPACE(NumericLimitConstants::f16epsilon)
    #define NEX_FLOAT16_INFINITY    NEX_PREPEND_NAMESPACE(NumericLimitConstants::f16infinity)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

#define NEX_FLOAT32_MIN             NEX_PREPEND_NAMESPACE(NumericLimitConstants::f32min)
#define NEX_FLOAT32_MAX             NEX_PREPEND_NAMESPACE(NumericLimitConstants::f32max)
#define NEX_FLOAT32_LOWEST          NEX_PREPEND_NAMESPACE(NumericLimitConstants::f32lowest)
#define NEX_FLOAT32_EPSILON         NEX_PREPEND_NAMESPACE(NumericLimitConstants::f32epsilon)
#define NEX_FLOAT32_INFINITY        NEX_PREPEND_NAMESPACE(NumericLimitConstants::f32infinity)

#define NEX_FLOAT64_MIN             NEX_PREPEND_NAMESPACE(NumericLimitConstants::f64min)
#define NEX_FLOAT64_MAX             NEX_PREPEND_NAMESPACE(NumericLimitConstants::f64max)
#define NEX_FLOAT64_LOWEST          NEX_PREPEND_NAMESPACE(NumericLimitConstants::f64lowest)
#define NEX_FLOAT64_EPSILON         NEX_PREPEND_NAMESPACE(NumericLimitConstants::f64epsilon)
#define NEX_FLOAT64_INFINITY        NEX_PREPEND_NAMESPACE(NumericLimitConstants::f64infinity)

#define NEX_LDOUBLE_MIN             NEX_PREPEND_NAMESPACE(NumericLimitConstants::ldmin)
#define NEX_LDOUBLE_MAX             NEX_PREPEND_NAMESPACE(NumericLimitConstants::ldmax)
#define NEX_LDOUBLE_LOWEST          NEX_PREPEND_NAMESPACE(NumericLimitConstants::ldlowest)
#define NEX_LDOUBLE_EPSILON         NEX_PREPEND_NAMESPACE(NumericLimitConstants::ldepsilon)
#define NEX_LDOUBLE_INFINITY        NEX_PREPEND_NAMESPACE(NumericLimitConstants::ldinfinity)

#if NEX_HAS_BUILTIN_FLOAT128
    #define NEX_FLOAT128_MIN        NEX_PREPEND_NAMESPACE(NumericLimitConstants::f128min)
    #define NEX_FLOAT128_MAX        NEX_PREPEND_NAMESPACE(NumericLimitConstants::f128max)
    #define NEX_FLOAT128_LOWEST     NEX_PREPEND_NAMESPACE(NumericLimitConstants::f128lowest)
    #define NEX_FLOAT128_EPSILON    NEX_PREPEND_NAMESPACE(NumericLimitConstants::f128epsilon)
    #define NEX_FLOAT128_INFINITY   NEX_PREPEND_NAMESPACE(NumericLimitConstants::f128infinity)
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

// =================================================================================
// Base template for numeric limits (C++20 compile-time)
// =================================================================================

//  Enumeration for different IEEE rounding styles
enum class NumericRoundStyle : int32 {
    RoundIndeterminate      = -1,
    RoundTowardZero         = 0,
    RoundToNearest          = 1,
    RoundTowardInfinity     = 2,
    RoundTowardNegInfinity  = 3
};

template<typename Type>
struct NumericLimitsBase {
    static_assert((sizeof(Type) > 0), 
        "Error: NumericLimits is not specialized for this type");

    /**
     * @brief Core Type Identification
     * @note  False for all types by default; specializations will set to true for specific types.
     */

    // Indicates whether a meaningful specialization exists for the data type
    // It is false by default in the base struct and set to true for specific types (like int, float, etc.) 
    // to signal that the rest of the values are valid.
    static constexpr bool isSpecialized             = false;
    // Identifies if the type can represent both positive and negative values. 
    // For example, it is true for int and float, but false for uint32.
    static constexpr bool isSigned                  = false;
    // Specifies whether the type is an integer type (e.g., int, char, long). 
    // It is false for floating-point types like float or double.
    static constexpr bool isInteger                 = false;
    // Indicates if the type uses an exact representation without rounding errors. 
    // This is true for all integer types and false for floating-point types due to precision limits.
    static constexpr bool isExact                   = false;
    // Determines whether the type is a boolean type (bool).
    static constexpr bool isBoolean                 = false;

    /**
     * @brief Standards & Hardware Behavior
     * @note False or zero by default; specializations for floating-point types will set them 
     * according to IEC 60559 / IEEE 754 compliance.
     */

    // Specifies if the type adheres to the IEC 60559 / IEEE 754 standard for floating-point arithmetic. 
    // Most modern systems set this to true for standard float and double.
    static constexpr bool isIEC559                  = false;
    // Indicates whether an arithmetic operation that causes an error (like division by zero or overflow) 
    // will trap, causing a hardware signal or terminating the program.
    static constexpr bool traps                     = false;

    /**
     * @brief Floating-Point Special Values
     * @note False by default; specializations for floating-point types will set to true if the type
     * supports these special values according to the IEC 60559 / IEEE 754 standard.
     */

    // Identifies whether the type has a special representation for positive infinity (+∞). 
    // Generally true for IEEE 754 floating-point numbers.
    static constexpr bool hasInfinity               = false;
    // Determines if the type can represent a Quiet NaN (Not-a-Number). 
    // Quiet NaNs propagate through arithmetic operations without raising hardware exceptions.
    static constexpr bool hasQuietNaN               = false;
    // Determines if the type can represent a Signaling NaN. 
    // Unlike a Quiet NaN, using a Signaling NaN in an operation triggers an immediate hardware exception/trap.
    static constexpr bool hasSignalingNaN           = false;

    /**
     * @brief Subnormal / Denormalized Numbers
     * @note False by default; specializations for floating-point types will set to true if the type
     * supports subnormal or denormalized numbers according to the IEC 60559 / IEEE 754 standard.
     */

    // Indicates whether the floating-point type supports subnormal (denormalized) numbers, 
    // which are numbers closer to zero than the smallest normal representation allows.
    static constexpr bool hasDenorm                 = false;
    // Indicates if a loss of precision is detected as a "denormalization loss" 
    // rather than an inexact result error.
    static constexpr bool hasDenormLoss             = false;
    // Determines whether a floating-point type detects "tinyness" (underflow) 
    // before or after rounding the result.
    static constexpr bool tinynessBefore            = false;

    /**
     * @brief Value Boundaries & Overflow Behavior
     * @note False by default; specializations for integer types will set them according to the type's limits.
     */

    // Specifies whether the set of values representable by the type is finite. 
    // Practically all built-in types are bounded (true), while an arbitrary-precision BigInt class 
    // would be unbounded (false).
    static constexpr bool isBounded                 = false;
    // Indicates if mathematically overflowing an operation rolls over to the opposite end of the spectrum 
    // (modulo arithmetic). This is traditionally true for unsigned integers (e.g., MAX_UINT + 1 == 0).
    static constexpr bool isModulo                  = false;

    /**
     * @brief Digital Radix & Precision
     * @note Zero by default; specializations for integer and floating-point types will set them according to 
     * the type's characteristics.
     */

    // The base of the representation system for the type. 
    // For integers and typical floating-point numbers, this is 2 (binary), but it could be 10 for decimal types.
    static constexpr int32 radix                    = 0;
    // The number of radix digits that the type can represent without losing precision. 
    // For integers, this is the number of non-sign bits. 
    // For floating-point numbers, it's the number of bits in the mantissa.
    static constexpr int32 digits                   = 0;
    // The number of base-10 (decimal) digits that can be represented by the type without alteration. 
    // For example, any 5-digit decimal number can fit perfectly into a type whose digits10 is >= 5.
    static constexpr int32 digits10                 = 0;
    // The number of base-10 digits required to uniquely identify every possible distinct value of this type. 
    // This is highly useful when serializing floating-point numbers to text and back without losing data.
    static constexpr int32 maxDigits10              = 0;

    /**
     * @brief Exponent Limits (Floating-Point Only)
     * @note Zero by default; specializations for floating-point types will set them according to 
     * the type's characteristics.
     */

    // The maximum positive integer power that the radix can be raised to such that the type can still represent 
    // a finite floating-point number without overflowing.
    static constexpr int32 maxExponent              = 0;
    // The maximum positive integer power of 10 that can be represented as a finite floating-point number.
    static constexpr int32 maxExponent10            = 0;
    // The minimum negative integer power that the radix can be raised to such that the type can still represent 
    // a normalized floating-point number.
    static constexpr int32 minExponent              = 0;
    // The minimum negative integer power of 10 that can be represented as a normalized floating-point number.
    static constexpr int32 minExponent10            = 0;

    /**
     * @brief Rounding Behavior
     * @note RoundTowardZero by default; specializations for floating-point types will set to 
     * the appropriate rounding style according to the IEC 60559 / IEEE 754 standard.
     */

    // Identifies the default rounding mode used by the type 
    // when a real number cannot fit into its precision constraints.
    static constexpr NumericRoundStyle roundStyle   = NumericRoundStyle::RoundTowardZero;
};

// =================================================================================
// Specializations of NumericLimitsBase for character and fixed-width integer types
// =================================================================================

template<typename Type>
struct IntegerLimitsBase : public NumericLimitsBase<Type> {
    static_assert(meta::IsIntegralV<Type>, 
        "Error: IntegerLimitsBase is only specialized for integral types");

    static constexpr bool isSpecialized     = true;
    static constexpr bool isInteger         = true;
    static constexpr bool isExact           = true;
    static constexpr bool isBounded         = true;
    static constexpr int32 radix            = 2;
};

template<typename Type>
struct SignedIntegerLimitsBase : public IntegerLimitsBase<Type> {
    static_assert(meta::IsSignedIntegralV<Type>, 
        "Error: SignedIntegerLimits is only specialized for signed integral types");

    static constexpr bool isSigned          = true;

    // For signed integers, the number of radix digits is typically the number of bits minus one for the sign bit.
    static constexpr int32 digits     = sizeof(Type) * 8 - 1;
    // The number of base-10 digits is calculated based on the number of binary digits and log10(2).
    static constexpr int32 digits10   = (digits * 30103) / 100000; // Approximation of digits * log10(2)
};

template<typename Type>
struct UnsignedIntegerLimitsBase : public IntegerLimitsBase<Type> {
    static_assert(meta::IsUnsignedIntegralV<Type>, 
        "Error: UnsignedIntegerLimits is only specialized for unsigned integral types");

    static constexpr bool isModulo          = true;

    // For unsigned integers, the number of radix digits is typically the number of bits since there is no sign bit.
    static constexpr int32 digits     = sizeof(Type) * 8;
    // The number of base-10 digits is calculated based on the number of binary digits and log10(2).
    static constexpr int32 digits10   = (digits * 30103) / 100000; // Approximation of digits * log10(2)
};

struct NCharLimitsBase 
    : public meta::ConditionalT<
        meta::IsSignedIntegralV<char>, 
        SignedIntegerLimitsBase<char>, 
        UnsignedIntegerLimitsBase<char>
    > {
    static constexpr bool isSigned          = (meta::IsSignedIntegralV<char>);
    static constexpr bool isModulo          = (!meta::IsSignedIntegralV<char>);
    static constexpr int32 digits           = sizeof(char) * 8 - (meta::IsSignedIntegralV<char>);
    static constexpr int32 digits10         = 2;  // Always 2 decimal digits, no matter the signedness
};

struct WCharLimitsBase 
    : public meta::ConditionalT<
        meta::IsSignedIntegralV<wchar_t>, 
        SignedIntegerLimitsBase<wchar_t>, 
        UnsignedIntegerLimitsBase<wchar_t>
    > {
    static constexpr bool isSigned          = (meta::IsSignedIntegralV<wchar_t>);
    static constexpr bool isModulo          = (!meta::IsSignedIntegralV<wchar_t>);
    static constexpr int32 digits           = sizeof(wchar_t) * 8 - (meta::IsSignedIntegralV<wchar_t>);
    static constexpr int32 digits10         = (digits * 30103) / 100000; // Approximation of digits * log10(2)
};

// =================================================================================
// Specializations of NumericLimitsBase for floating-point types
// =================================================================================

template<typename Type>
struct FloatingPointLimitsBase : public NumericLimitsBase<Type> {
    static_assert((meta::IsFloatingPointV<Type>), 
        "Error: FloatingPointLimits is only specialized for floating-point types");

    static constexpr bool isSpecialized             = true;
    static constexpr bool isSigned                  = true;
    static constexpr bool isIEC559                  = true;
    static constexpr bool hasInfinity               = true;
    static constexpr bool hasQuietNaN               = true;
    static constexpr bool hasSignalingNaN           = true;
    static constexpr bool hasDenorm                 = true;
    static constexpr bool isBounded                 = true;
    static constexpr int32 radix                    = 2;
    static constexpr NumericRoundStyle roundStyle   = NumericRoundStyle::RoundToNearest;
};

// =================================================================================
// Primary template for NumericLimits (C++20 compile-time)
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
class NumericLimits : public NumericLimitsBase<Type> {
    static_assert((meta::IsArithmeticV<Type>), 
        "Error: NumericLimits is only specialized for arithmetic types (integral and floating-point)");

    NEX_NODISCARD static constexpr Type (min)() noexcept        { return Type(0); }
    NEX_NODISCARD static constexpr Type (max)() noexcept        { return Type(0); }

    NEX_NODISCARD static constexpr Type lowest() noexcept       { return Type(0); }
    NEX_NODISCARD static constexpr Type epsilon() noexcept      { return Type(0); }
    NEX_NODISCARD static constexpr Type roundError() noexcept   { return Type(0); }
    NEX_NODISCARD static constexpr Type denormMin() noexcept    { return Type(0); }

    NEX_NODISCARD static constexpr Type infinity() noexcept     { return Type(0); }
    NEX_NODISCARD static constexpr Type quietNaN() noexcept     { return Type(0); }
    NEX_NODISCARD static constexpr Type signalingNaN() noexcept { return Type(0); }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept       { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept        { return 0; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept        { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return 0; }
};

// =================================================================================
// Specializations of NumericLimits for top-level const/volatile qualifiers
// =================================================================================

template<typename Type>
class NumericLimits<const Type> : public NumericLimits<Type> {};

template<typename Type>
class NumericLimits<volatile Type> : public NumericLimits<Type> {};

template<typename Type>
class NumericLimits<const volatile Type> : public NumericLimits<Type> {};

// =================================================================================
// Specializations of NumericLimits for boolean type
// =================================================================================

template<>
class NumericLimits<bool> : public IntegerLimitsBase<bool> {
public:
    NEX_NODISCARD static constexpr bool (min)() noexcept { return NumericLimitConstants::boolmin; }
    NEX_NODISCARD static constexpr bool (max)() noexcept { return NumericLimitConstants::boolmax; }

    NEX_NODISCARD static constexpr bool lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr bool epsilon() noexcept      { return false; }
    NEX_NODISCARD static constexpr bool roundError() noexcept   { return false; }
    NEX_NODISCARD static constexpr bool denormMin() noexcept    { return false; }

    NEX_NODISCARD static constexpr bool infinity() noexcept     { return false; }
    NEX_NODISCARD static constexpr bool quietNaN() noexcept     { return false; }
    NEX_NODISCARD static constexpr bool signalingNaN() noexcept { return false; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept       { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }

    static constexpr bool isSpecialized     = true;
    static constexpr bool isInteger         = true;
    static constexpr bool isExact           = true;
    static constexpr bool isBoolean         = true;
    static constexpr bool isBounded         = true;
    static constexpr int32 radix            = 2;
    static constexpr int32 digits           = 1;
};

// =================================================================================
// Specializations of NumericLimits for character types
// =================================================================================

template<>
class NumericLimits<nchar> : public NCharLimitsBase {
public:
    NEX_NODISCARD static constexpr nchar (min)() noexcept { return NumericLimitConstants::ncharmin; }
    NEX_NODISCARD static constexpr nchar (max)() noexcept { return NumericLimitConstants::ncharmax; }

    NEX_NODISCARD static constexpr nchar lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr nchar epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr nchar roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr nchar denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr nchar infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr nchar quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr nchar signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept       { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<schar> : public SignedIntegerLimitsBase<schar> {
public:
    NEX_NODISCARD static constexpr schar (min)() noexcept { return NumericLimitConstants::i8min; }
    NEX_NODISCARD static constexpr schar (max)() noexcept { return NumericLimitConstants::i8max; }

    NEX_NODISCARD static constexpr schar lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr schar epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr schar roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr schar denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr schar infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr schar quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr schar signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<uchar> : public UnsignedIntegerLimitsBase<uchar> {
public:
    NEX_NODISCARD static constexpr uchar (min)() noexcept { return 0; }
    NEX_NODISCARD static constexpr uchar (max)() noexcept { return NumericLimitConstants::u8max; }

    NEX_NODISCARD static constexpr uchar lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr uchar epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr uchar roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr uchar denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr uchar infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr uchar quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr uchar signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

#if NEX_HAS_BUILTIN_CHAR8_T
    template<>
    class NumericLimits<char8> : public UnsignedIntegerLimitsBase<char8> {
    public:
        NEX_NODISCARD static constexpr char8 (min)() noexcept { return NumericLimitConstants::c8min; }
        NEX_NODISCARD static constexpr char8 (max)() noexcept { return NumericLimitConstants::c8max; }

        NEX_NODISCARD static constexpr char8 lowest() noexcept       { return (min)(); }
        NEX_NODISCARD static constexpr char8 epsilon() noexcept      { return 0; }
        NEX_NODISCARD static constexpr char8 roundError() noexcept   { return 0; }
        NEX_NODISCARD static constexpr char8 denormMin() noexcept    { return 0; }

        NEX_NODISCARD static constexpr char8 infinity() noexcept     { return 0; }
        NEX_NODISCARD static constexpr char8 quietNaN() noexcept     { return 0; }
        NEX_NODISCARD static constexpr char8 signalingNaN() noexcept { return 0; }

        NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
        NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
        NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

        NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
        NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
        NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
        NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
    };
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

template<>
class NumericLimits<char16> : public UnsignedIntegerLimitsBase<char16> {
public:
    NEX_NODISCARD static constexpr char16 (min)() noexcept { return NumericLimitConstants::c16min; }
    NEX_NODISCARD static constexpr char16 (max)() noexcept { return NumericLimitConstants::c16max; }

    NEX_NODISCARD static constexpr char16 lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr char16 epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr char16 roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr char16 denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr char16 infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr char16 quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr char16 signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<char32> : public UnsignedIntegerLimitsBase<char32> {
public:
    NEX_NODISCARD static constexpr char32 (min)() noexcept { return NumericLimitConstants::c32min; }
    NEX_NODISCARD static constexpr char32 (max)() noexcept { return NumericLimitConstants::c32max; }

    NEX_NODISCARD static constexpr char32 lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr char32 epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr char32 roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr char32 denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr char32 infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr char32 quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr char32 signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<wchar> : public WCharLimitsBase {
public:
    NEX_NODISCARD static constexpr wchar (min)() noexcept { return NumericLimitConstants::wcharmin; }
    NEX_NODISCARD static constexpr wchar (max)() noexcept { return NumericLimitConstants::wcharmax; }

    NEX_NODISCARD static constexpr wchar lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr wchar epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr wchar roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr wchar denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr wchar infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr wchar quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr wchar signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

// =================================================================================
// Specializations of NumericLimits for fixed-width integer types
// =================================================================================

template<>
class NumericLimits<int16> : public SignedIntegerLimitsBase<int16> {
public:
    NEX_NODISCARD static constexpr int16 (min)() noexcept { return NumericLimitConstants::i16min; }
    NEX_NODISCARD static constexpr int16 (max)() noexcept { return NumericLimitConstants::i16max; }

    NEX_NODISCARD static constexpr int16 lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr int16 epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr int16 roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr int16 denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr int16 infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr int16 quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr int16 signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<uint16> : public UnsignedIntegerLimitsBase<uint16> {
public:
    NEX_NODISCARD static constexpr uint16 (min)() noexcept { return 0; }
    NEX_NODISCARD static constexpr uint16 (max)() noexcept { return NumericLimitConstants::u16max; }

    NEX_NODISCARD static constexpr uint16 lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr uint16 epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr uint16 roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr uint16 denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr uint16 infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr uint16 quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr uint16 signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<int32> : public SignedIntegerLimitsBase<int32> {
public:
    NEX_NODISCARD static constexpr int32 (min)() noexcept { return NumericLimitConstants::i32min; }
    NEX_NODISCARD static constexpr int32 (max)() noexcept { return NumericLimitConstants::i32max; }

    NEX_NODISCARD static constexpr int32 lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr int32 epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr int32 roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr int32 denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr int32 infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr int32 quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr int32 signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<uint32> : public UnsignedIntegerLimitsBase<uint32> {
public:
    NEX_NODISCARD static constexpr uint32 (min)() noexcept { return 0; }
    NEX_NODISCARD static constexpr uint32 (max)() noexcept { return NumericLimitConstants::u32max; }

    NEX_NODISCARD static constexpr uint32 lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr uint32 epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr uint32 roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr uint32 denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr uint32 infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr uint32 quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr uint32 signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<slong> : public SignedIntegerLimitsBase<slong> {
public:
    NEX_NODISCARD static constexpr slong (min)() noexcept { return NumericLimitConstants::slmin; }
    NEX_NODISCARD static constexpr slong (max)() noexcept { return NumericLimitConstants::slmax; }

    NEX_NODISCARD static constexpr slong lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr slong epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr slong roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr slong denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr slong infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr slong quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr slong signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<ulong> : public UnsignedIntegerLimitsBase<ulong> {
public:
    NEX_NODISCARD static constexpr ulong (min)() noexcept { return 0; }
    NEX_NODISCARD static constexpr ulong (max)() noexcept { return NumericLimitConstants::ulmax; }

    NEX_NODISCARD static constexpr ulong lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr ulong epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr ulong roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr ulong denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr ulong infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr ulong quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr ulong signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<int64> : public SignedIntegerLimitsBase<int64> {
public:
    NEX_NODISCARD static constexpr int64 (min)() noexcept { return NumericLimitConstants::i64min; }
    NEX_NODISCARD static constexpr int64 (max)() noexcept { return NumericLimitConstants::i64max; }

    NEX_NODISCARD static constexpr int64 lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr int64 epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr int64 roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr int64 denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr int64 infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr int64 quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr int64 signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

template<>
class NumericLimits<uint64> : public UnsignedIntegerLimitsBase<uint64> {
public:
    NEX_NODISCARD static constexpr uint64 (min)() noexcept { return 0; }
    NEX_NODISCARD static constexpr uint64 (max)() noexcept { return NumericLimitConstants::u64max; }

    NEX_NODISCARD static constexpr uint64 lowest() noexcept       { return (min)(); }
    NEX_NODISCARD static constexpr uint64 epsilon() noexcept      { return 0; }
    NEX_NODISCARD static constexpr uint64 roundError() noexcept   { return 0; }
    NEX_NODISCARD static constexpr uint64 denormMin() noexcept    { return 0; }

    NEX_NODISCARD static constexpr uint64 infinity() noexcept     { return 0; }
    NEX_NODISCARD static constexpr uint64 quietNaN() noexcept     { return 0; }
    NEX_NODISCARD static constexpr uint64 signalingNaN() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
};

#if NEX_HAS_BUILTIN_INT128
    template<>
    class NumericLimits<int128> : public SignedIntegerLimitsBase<int128> {
    public:
        NEX_NODISCARD static constexpr int128 (min)() noexcept { return NumericLimitConstants::i128min; }
        NEX_NODISCARD static constexpr int128 (max)() noexcept { return NumericLimitConstants::i128max; }

        NEX_NODISCARD static constexpr int128 lowest() noexcept       { return (min)(); }
        NEX_NODISCARD static constexpr int128 epsilon() noexcept      { return 0; }
        NEX_NODISCARD static constexpr int128 roundError() noexcept   { return 0; }
        NEX_NODISCARD static constexpr int128 denormMin() noexcept    { return 0; }

        NEX_NODISCARD static constexpr int128 infinity() noexcept     { return 0; }
        NEX_NODISCARD static constexpr int128 quietNaN() noexcept     { return 0; }
        NEX_NODISCARD static constexpr int128 signalingNaN() noexcept { return 0; }

        NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
        NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
        NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

        NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
        NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
        NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
        NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
    };

    template<>
    class NumericLimits<uint128> : public UnsignedIntegerLimitsBase<uint128> {
    public:
        NEX_NODISCARD static constexpr uint128 (min)() noexcept { return 0; }
        NEX_NODISCARD static constexpr uint128 (max)() noexcept { return NumericLimitConstants::u128max; }

        NEX_NODISCARD static constexpr uint128 lowest() noexcept       { return (min)(); }
        NEX_NODISCARD static constexpr uint128 epsilon() noexcept      { return 0; }
        NEX_NODISCARD static constexpr uint128 roundError() noexcept   { return 0; }
        NEX_NODISCARD static constexpr uint128 denormMin() noexcept    { return 0; }

        NEX_NODISCARD static constexpr uint128 infinity() noexcept     { return 0; }
        NEX_NODISCARD static constexpr uint128 quietNaN() noexcept     { return 0; }
        NEX_NODISCARD static constexpr uint128 signalingNaN() noexcept { return 0; }

        NEX_NODISCARD static constexpr int32 exponentBias() noexcept { return 0; }
        NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return 0; }
        NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return 0; }

        NEX_NODISCARD static constexpr int32 minDigits() noexcept    { return digits; }
        NEX_NODISCARD static constexpr int32 maxDigits() noexcept    { return digits; }
        NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
        NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }
    };
#endif  // ^^NEX_HAS_BUILTIN_INT128

// =================================================================================
// Specializations of NumericLimits for floating-point types
// =================================================================================

#if NEX_HAS_BUILTIN_FLOAT16
    template<>
    class NumericLimits<float16> : public FloatingPointLimitsBase<float16> {
    public:
        NEX_NODISCARD static constexpr float16 (min)() noexcept { return NumericLimitConstants::f16min; }
        NEX_NODISCARD static constexpr float16 (max)() noexcept { return NumericLimitConstants::f16max; }

        NEX_NODISCARD static constexpr float16 lowest() noexcept       { return NumericLimitConstants::f16lowest; }
        NEX_NODISCARD static constexpr float16 epsilon() noexcept      { return NumericLimitConstants::f16epsilon; }
        NEX_NODISCARD static constexpr float16 roundError() noexcept   { return NumericLimitConstants::f16roundError; }
        NEX_NODISCARD static constexpr float16 denormMin() noexcept    { return NumericLimitConstants::f16denormMin; }

        NEX_NODISCARD static constexpr float16 infinity() noexcept     { return NumericLimitConstants::f16infinity; }
        NEX_NODISCARD static constexpr float16 quietNaN() noexcept     { return NumericLimitConstants::f16quietNaN; }
        NEX_NODISCARD static constexpr float16 signalingNaN() noexcept { return NumericLimitConstants::f16signalingNaN; }

        NEX_NODISCARD static constexpr int32 exponentBias() noexcept { 
            return static_cast<int32>(NumericLimitConstants::f16exponentBias); 
        }
        NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return minExponent10; }
        NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return maxExponent10; }

        NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
        NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
        NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
        NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }

        // Number of radix digits in the mantissa, including the implicit hidden bit
        static constexpr int32 digits           = static_cast<int32>(NumericLimitConstants::f16binaryDigits);
        // Minimum decimal precision: number of decimal digits that can be round-tripped without alteration.
        static constexpr int32 digits10         = static_cast<int32>(NumericLimitConstants::f16decimalDigits);
        // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
        static constexpr int32 maxDigits10      = static_cast<int32>(NumericLimitConstants::f16maxDecimalDigits);

        // The maximum positive integer power that the radix can be raised to such that the type can still represent 
        // a finite floating-point number without overflowing.
        static constexpr int32 maxExponent      = static_cast<int32>(NumericLimitConstants::f16maxExponent);
        // The maximum positive integer power of 10 that can be represented as a finite floating-point number.
        static constexpr int32 maxExponent10    = static_cast<int32>(NumericLimitConstants::f16maxDecimalExponent);
        // The minimum negative integer power that the radix can be raised to such that the type can still represent 
        // a normalized floating-point number.
        static constexpr int32 minExponent      = static_cast<int32>(NumericLimitConstants::f16minExponent);
        // The minimum negative integer power of 10 that can be represented as a normalized floating-point number.
        static constexpr int32 minExponent10    = static_cast<int32>(NumericLimitConstants::f16minDecimalExponent);
    };
#endif  // ^^NEX_HAS_BUILTIN_FLOAT16

template<>
class NumericLimits<float32> : public FloatingPointLimitsBase<float32> {
public:
    NEX_NODISCARD static constexpr float32 (min)() noexcept { return NumericLimitConstants::f32min; }
    NEX_NODISCARD static constexpr float32 (max)() noexcept { return NumericLimitConstants::f32max; }

    NEX_NODISCARD static constexpr float32 lowest() noexcept       { return NumericLimitConstants::f32lowest; }
    NEX_NODISCARD static constexpr float32 epsilon() noexcept      { return NumericLimitConstants::f32epsilon; }
    NEX_NODISCARD static constexpr float32 roundError() noexcept   { return NumericLimitConstants::f32roundError; }
    NEX_NODISCARD static constexpr float32 denormMin() noexcept    { return NumericLimitConstants::f32denormMin; }

    NEX_NODISCARD static constexpr float32 infinity() noexcept     { return NumericLimitConstants::f32infinity; }
    NEX_NODISCARD static constexpr float32 quietNaN() noexcept     { return NumericLimitConstants::f32quietNaN; }
    NEX_NODISCARD static constexpr float32 signalingNaN() noexcept { return NumericLimitConstants::f32signalingNaN; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { 
        return static_cast<int32>(NumericLimitConstants::f32exponentBias); 
    }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return minExponent10; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return maxExponent10; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }

    // Number of radix digits in the mantissa, including the implicit hidden bit
    static constexpr int32 digits           = static_cast<int32>(NumericLimitConstants::f32binaryDigits);
    // Minimum decimal precision: number of decimal digits that can be round-tripped without alteration.
    static constexpr int32 digits10         = static_cast<int32>(NumericLimitConstants::f32decimalDigits);
    // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
    static constexpr int32 maxDigits10      = static_cast<int32>(NumericLimitConstants::f32maxDecimalDigits);

    // The maximum positive integer power that the radix can be raised to such that the type can still represent 
    // a finite floating-point number without overflowing.
    static constexpr int32 maxExponent      = static_cast<int32>(NumericLimitConstants::f32maxExponent);
    // The maximum positive integer power of 10 that can be represented as a finite floating-point number.
    static constexpr int32 maxExponent10    = static_cast<int32>(NumericLimitConstants::f32maxDecimalExponent);
    // The minimum negative integer power that the radix can be raised to such that the type can still represent 
    // a normalized floating-point number.
    static constexpr int32 minExponent      = static_cast<int32>(NumericLimitConstants::f32minExponent);
    // The minimum negative integer power of 10 that can be represented as a normalized floating-point number.
    static constexpr int32 minExponent10    = static_cast<int32>(NumericLimitConstants::f32minDecimalExponent);
};

template<>
class NumericLimits<float64> : public FloatingPointLimitsBase<float64> {
public:
    NEX_NODISCARD static constexpr float64 (min)() noexcept { return NumericLimitConstants::f64min; }
    NEX_NODISCARD static constexpr float64 (max)() noexcept { return NumericLimitConstants::f64max; }

    NEX_NODISCARD static constexpr float64 lowest() noexcept       { return NumericLimitConstants::f64lowest; }
    NEX_NODISCARD static constexpr float64 epsilon() noexcept      { return NumericLimitConstants::f64epsilon; }
    NEX_NODISCARD static constexpr float64 roundError() noexcept   { return NumericLimitConstants::f64roundError; }
    NEX_NODISCARD static constexpr float64 denormMin() noexcept    { return NumericLimitConstants::f64denormMin; }

    NEX_NODISCARD static constexpr float64 infinity() noexcept     { return NumericLimitConstants::f64infinity; }
    NEX_NODISCARD static constexpr float64 quietNaN() noexcept     { return NumericLimitConstants::f64quietNaN; }
    NEX_NODISCARD static constexpr float64 signalingNaN() noexcept { return NumericLimitConstants::f64signalingNaN; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { 
        return static_cast<int32>(NumericLimitConstants::f64exponentBias); 
    }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return minExponent10; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return maxExponent10; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }

    // Number of radix digits in the mantissa, including the implicit hidden bit
    static constexpr int32 digits           = static_cast<int32>(NumericLimitConstants::f64binaryDigits);
    // Minimum decimal precision: number of decimal digits that can be round-tripped without alteration.
    static constexpr int32 digits10         = static_cast<int32>(NumericLimitConstants::f64decimalDigits);
    // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
    static constexpr int32 maxDigits10      = static_cast<int32>(NumericLimitConstants::f64maxDecimalDigits);

    // The maximum positive integer power that the radix can be raised to such that the type can still represent 
    // a finite floating-point number without overflowing.
    static constexpr int32 maxExponent      = static_cast<int32>(NumericLimitConstants::f64maxExponent);
    // The maximum positive integer power of 10 that can be represented as a finite floating-point number.
    static constexpr int32 maxExponent10    = static_cast<int32>(NumericLimitConstants::f64maxDecimalExponent);
    // The minimum negative integer power that the radix can be raised to such that the type can still represent 
    // a normalized floating-point number.
    static constexpr int32 minExponent      = static_cast<int32>(NumericLimitConstants::f64minExponent);
    // The minimum negative integer power of 10 that can be represented as a normalized floating-point number.
    static constexpr int32 minExponent10    = static_cast<int32>(NumericLimitConstants::f64minDecimalExponent);
};

template<>
class NumericLimits<ldouble> : public FloatingPointLimitsBase<ldouble> {
public:
    NEX_NODISCARD static constexpr ldouble (min)() noexcept { return NumericLimitConstants::ldmin; }
    NEX_NODISCARD static constexpr ldouble (max)() noexcept { return NumericLimitConstants::ldmax; }

    NEX_NODISCARD static constexpr ldouble lowest() noexcept       { return NumericLimitConstants::ldlowest; }
    NEX_NODISCARD static constexpr ldouble epsilon() noexcept      { return NumericLimitConstants::ldepsilon; }
    NEX_NODISCARD static constexpr ldouble roundError() noexcept   { return NumericLimitConstants::ldroundError; }
    NEX_NODISCARD static constexpr ldouble denormMin() noexcept    { return NumericLimitConstants::lddenormMin; }

    NEX_NODISCARD static constexpr ldouble infinity() noexcept     { return NumericLimitConstants::ldinfinity; }
    NEX_NODISCARD static constexpr ldouble quietNaN() noexcept     { return NumericLimitConstants::ldquietNaN; }
    NEX_NODISCARD static constexpr ldouble signalingNaN() noexcept { return NumericLimitConstants::ldsignalingNaN; }

    NEX_NODISCARD static constexpr int32 exponentBias() noexcept { 
        return static_cast<int32>(NumericLimitConstants::ldexponentBias); 
    }
    NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return minExponent10; }
    NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return maxExponent10; }

    NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
    NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
    NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }

    // Number of radix digits in the mantissa, including the implicit hidden bit
    static constexpr int32 digits           = static_cast<int32>(NumericLimitConstants::ldbinaryDigits);
    // Minimum decimal precision: number of decimal digits that can be round-tripped without alteration.
    static constexpr int32 digits10         = static_cast<int32>(NumericLimitConstants::lddecimalDigits);
    // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
    static constexpr int32 maxDigits10      = static_cast<int32>(NumericLimitConstants::ldmaxDecimalDigits);

    // The maximum positive integer power that the radix can be raised to such that the type can still represent 
    // a finite floating-point number without overflowing.
    static constexpr int32 maxExponent      = static_cast<int32>(NumericLimitConstants::ldmaxExponent);
    // The maximum positive integer power of 10 that can be represented as a finite floating-point number.
    static constexpr int32 maxExponent10    = static_cast<int32>(NumericLimitConstants::ldmaxDecimalExponent);
    // The minimum negative integer power that the radix can be raised to such that the type can still represent 
    // a normalized floating-point number.
    static constexpr int32 minExponent      = static_cast<int32>(NumericLimitConstants::ldminExponent);
    // The minimum negative integer power of 10 that can be represented as a normalized floating-point number.
    static constexpr int32 minExponent10    = static_cast<int32>(NumericLimitConstants::ldminDecimalExponent);
};

#if NEX_HAS_BUILTIN_FLOAT128
    template<>
    class NumericLimits<float128> : public FloatingPointLimitsBase<float128> {
    public:
        NEX_NODISCARD static constexpr float128 (min)() noexcept { return NumericLimitConstants::f128min; }
        NEX_NODISCARD static constexpr float128 (max)() noexcept { return NumericLimitConstants::f128max; }

        NEX_NODISCARD static constexpr float128 lowest() noexcept       { return NumericLimitConstants::f128lowest; }
        NEX_NODISCARD static constexpr float128 epsilon() noexcept      { return NumericLimitConstants::f128epsilon; }
        NEX_NODISCARD static constexpr float128 roundError() noexcept   { return NumericLimitConstants::f128roundError; }
        NEX_NODISCARD static constexpr float128 denormMin() noexcept    { return NumericLimitConstants::f128denormMin; }

        NEX_NODISCARD static constexpr float128 infinity() noexcept     { return NumericLimitConstants::f128infinity; }
        NEX_NODISCARD static constexpr float128 quietNaN() noexcept     { return NumericLimitConstants::f128quietNaN; }
        NEX_NODISCARD static constexpr float128 signalingNaN() noexcept { return NumericLimitConstants::f128signalingNaN; }

        NEX_NODISCARD static constexpr int32 exponentBias() noexcept { 
            return static_cast<int32>(NumericLimitConstants::f128exponentBias); 
        }
        NEX_NODISCARD static constexpr int32 minDecimalExponent() noexcept { return minExponent10; }
        NEX_NODISCARD static constexpr int32 maxDecimalExponent() noexcept { return maxExponent10; }

        NEX_NODISCARD static constexpr int32 minDigits() noexcept     { return digits; }
        NEX_NODISCARD static constexpr int32 maxDigits() noexcept     { return digits; }
        NEX_NODISCARD static constexpr int32 minDecimalDigits() noexcept { return digits10; }
        NEX_NODISCARD static constexpr int32 maxDecimalDigits() noexcept { return maxDigits10; }

        // Number of radix digits in the mantissa, including the implicit hidden bit
        static constexpr int32 digits           = static_cast<int32>(NumericLimitConstants::f128binaryDigits);
        // Minimum decimal precision: number of decimal digits that can be round-tripped without alteration.
        static constexpr int32 digits10         = static_cast<int32>(NumericLimitConstants::f128decimalDigits);
        // Maximum decimal precision: number of decimal digits required to uniquely identify a distinct value
        static constexpr int32 maxDigits10      = static_cast<int32>(NumericLimitConstants::f128maxDecimalDigits);

        // The maximum positive integer power that the radix can be raised to such that the type can still represent 
        // a finite floating-point number without overflowing.
        static constexpr int32 maxExponent      = static_cast<int32>(NumericLimitConstants::f128maxExponent);
        // The maximum positive integer power of 10 that can be represented as a finite floating-point number.
        static constexpr int32 maxExponent10    = static_cast<int32>(NumericLimitConstants::f128maxDecimalExponent);
        // The minimum negative integer power that the radix can be raised to such that the type can still represent 
        // a normalized floating-point number.
        static constexpr int32 minExponent      = static_cast<int32>(NumericLimitConstants::f128minExponent);
        // The minimum negative integer power of 10 that can be represented as a normalized floating-point number.
        static constexpr int32 minExponent10    = static_cast<int32>(NumericLimitConstants::f128minDecimalExponent);
    };
#endif  // ^^NEX_HAS_BUILTIN_FLOAT128

NEX_NAMESPACE_END