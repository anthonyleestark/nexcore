/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  data_model.h
 * @brief Detects the target data model and fundamental type widths.
 *
 * @details
 * This header provides compile-time information about the target data model, including pointer width
 * and the relationship between the sizes of fundamental integer and pointer types.
 *
 * It exposes a consistent set of macros describing the target data model for use in platform-independent
 * code and conditional compilation.
 *
 * @note
 * The data model is not necessarily determined by the CPU architecture.
 * Different data models may be supported by the same processor architecture.
 */

#include "nex/base/platform.h"
#include "nex/base/architecture.h"
#include "nex/base/compiler.h"

// =================================================================================
// Data model detection based on pointer size and platform-specific macros.
// =================================================================================

// First, we prefer to detect the data model based on the pointer size if available.
// This ensures that the data model is correctly identified based on the actual pointer size,
// which is a reliable indicator of the target environment.
#if defined(__SIZEOF_POINTER__)
    #define NEX_SIZEOF_POINTER __SIZEOF_POINTER__

// 32-bit CPU (x86, ARM32...) usually implies a 32-bit data model.
#elif defined(NEX_ARCH_CPU_32_BITS)
    #define NEX_SIZEOF_POINTER 4

// 64-bit Windows usually implies a 64-bit data model.
#elif defined(NEX_PLATFORM_WINDOWS_64)
    #define NEX_SIZEOF_POINTER 8

// 32-bit Windows usually implies a 32-bit data model.
#elif defined(NEX_PLATFORM_WINDOWS_32)
    #define NEX_SIZEOF_POINTER 4

#else
    #error Unable to determine pointer size.
#endif

// Defines the data model based on the pointer size.
#if NEX_SIZEOF_POINTER == 8
    #define NEX_DATA_MODEL_64_BIT
    #define NEX_DATA_MODEL_IS_64_BIT 1
    #define NEX_DATA_MODEL_IS_32_BIT 0
#elif NEX_SIZEOF_POINTER == 4
    #define NEX_DATA_MODEL_32_BIT
    #define NEX_DATA_MODEL_IS_64_BIT 0
    #define NEX_DATA_MODEL_IS_32_BIT 1
#else
    #error Unsupported pointer size.
#endif

/**
 * @brief Detects the specific data model based on predefined macros.
 * 
 * @details
 * Linux x86-64 typically uses the LP64 data model, where long and pointers are 64-bit,
 * and int is 32-bit.
 * Windows x64 typically uses the LLP64 data model, where long long and pointers are 64-bit,
 * and int and long are 32-bit.
 * ILP64 and ILP32 are less common data models where int, long, and pointers are either
 * all 64-bit or all 32-bit, respectively.
 */
#if defined(__LP64__) || defined(_LP64)
    #define NEX_DATA_MODEL_LP64
#elif defined(__LLP64__) || defined(_LLP64)
    #define NEX_DATA_MODEL_LLP64
#elif defined(__ILP64__) || defined(_ILP64)
    #define NEX_DATA_MODEL_ILP64
#elif defined(__ILP32__) || defined(_ILP32)
    #define NEX_DATA_MODEL_ILP32
#endif  // ^^Specific data model detection

// Compilers such as IBM XL C/C++, IBM Open XL C/C++, IBM Enterprise Metal C
// may define the __64BIT__ macro to indicate a 64-bit compilation/addressing model.
#if defined(__64BIT__)
    #define NEX_ADDRESS_MODEL_64BIT
#endif  // ^^NEX_ADDRESS_MODEL_64BIT

// =================================================================================
// Detects the size of fundamental types
// =================================================================================

/**
 * @def NEX_ARCH_X86_ABI_LONG_DOUBLE_SIZE
 * @brief Detects the size of the long double type for x86 architectures.
 * 
 * @details
 * The size of the long double type varies depending on the specific x86 architecture.
 * For 32-bit x86 and x86_64 with ILP32, the size is typically 12 bytes.
 * For 64-bit x86_64, the size is typically 16 bytes.
 * This section defines the macro NEX_ARCH_X86_ABI_LONG_DOUBLE_SIZE accordingly.
 */
#if defined(NEX_ARCH_CPU_X86) || (defined(NEX_ARCH_CPU_X86_64) && defined(NEX_DATA_MODEL_ILP32))
    #define NEX_ARCH_X86_ABI_LONG_DOUBLE_SIZE 12
#elif defined(NEX_ARCH_CPU_X86_64)
    #define NEX_ARCH_X86_ABI_LONG_DOUBLE_SIZE 16
#endif  // ^^Size of long double for x86 architectures

// Determines the size of the long double type based on the compiler and architecture.
#if NEX_COMPILER_IS_MSVC
    // MSVC treats long double as 8-byte double.
    #define NEX_SIZEOF_LONG_DOUBLE 8
#elif defined(NEX_ARCH_X86_ABI_LONG_DOUBLE_SIZE)
    // Use the x86 ABI-specific long double size if available.
    #define NEX_SIZEOF_LONG_DOUBLE NEX_ARCH_X86_ABI_LONG_DOUBLE_SIZE
#elif defined(__SIZEOF_LONG_DOUBLE__)
    // GCC and Clang provide this exact size macro in bytes
    #define NEX_SIZEOF_LONG_DOUBLE __SIZEOF_LONG_DOUBLE__
#else
    #error Unable to determine size of long double.
#endif  // ^^NEX_SIZEOF_LONG_DOUBLE

// Determines the size of the long type based on the compiler and platform.
#if defined(__SIZEOF_LONG__)
    // Compiler explicitly reports the size of long (GCC, Clang, MinGW, ICC).
    #define NEX_SIZEOF_LONG __SIZEOF_LONG__
#elif NEX_PLATFORM_IS_WINDOWS
    // Both 32-bit (ILP32) and 64-bit (LLP64) Windows use 4-byte long.
    #define NEX_SIZEOF_LONG 4
#elif defined(NEX_DATA_MODEL_LP64) || (NEX_PLATFORM_IS_AIX && defined(NEX_ADDRESS_MODEL_64BIT))
    // 64-bit Unix, Linux, macOS, and AIX use 8-byte long.
    #define NEX_SIZEOF_LONG 8
#elif defined(NEX_DATA_MODEL_ILP32)
    // Standard 32-bit ILP32 targets use 4-byte long.
    #define NEX_SIZEOF_LONG 4
#else
    #error Unable to determine size of long.
#endif  // ^^NEX_SIZEOF_LONG

// Determines the number of bits in a char.
#if defined(__CHAR_BIT__)
    // Compiler explicitly reports the number of bits in a char (GCC, Clang, MinGW, ICC).
    #define NEX_CHAR_BIT __CHAR_BIT__
#elif defined(_TMS320C2000) || defined(__TMS320C2000__) || defined(__TMS320C55X__)
    // Texas Instruments C2000 / C55x DSPs have 16-bit chars.
    #define NEX_CHAR_BIT 16
#elif defined(__DSP56K__) || defined(__DSP563XX__)
    // Motorola/Freescale DSP56k has 24-bit chars.
    #define NEX_CHAR_BIT 24
#elif defined(__SHARC__) || defined(_TMS320C3X) || defined(_TMS320C4X)
    // Analog Devices SHARC and TI C3x/C4x DSPs have 32-bit chars.
    #define NEX_CHAR_BIT 32
#elif NEX_PLATFORM_IS_WINDOWS || NEX_PLATFORM_FAMILY_IS_UNIX || NEX_PLATFORM_FAMILY_IS_APPLE || \
      NEX_PLATFORM_IS_LINUX || NEX_PLATFORM_IS_AIX
    // Windows, Apple, and all POSIX-compliant operating systems strictly mandate 8-bit chars.
    #define NEX_CHAR_BIT 8
#else
    #error Unable to determine number of bits in a char.
#endif  // ^^NEX_CHAR_BIT

// Determines the size of the wchar type based on the platform and compiler.
#if defined(__SIZEOF_WCHAR_T__)
    // Compiler explicitly reports size of wchar (GCC, Clang, MinGW, ICC).
    #define NEX_SIZEOF_WCHAR __SIZEOF_WCHAR_T__
#elif defined(__WCHAR_WIDTH__)
    // Compiler reports bit-width (C23 / newer GCC & Clang).
    #define NEX_SIZEOF_WCHAR (__WCHAR_WIDTH__ / NEX_CHAR_BIT)
#elif NEX_PLATFORM_FAMILY_IS_POSIX && NEX_COMPILER_GCC_COMPATIBLE && defined(__WCHAR_MAX__)
    // GCC on POSIX reports the maximum value of wchar.
    #if (__WCHAR_MAX__ == 0x7fffffff || __WCHAR_MAX__ == 0xffffffff)
        #define NEX_SIZEOF_WCHAR 4
    #elif (__WCHAR_MAX__ == 0x7fff || __WCHAR_MAX__ == 0xffff)
        #define NEX_SIZEOF_WCHAR 2
    #else
        #error Unsupported maximum value of wchar.
    #endif
#elif NEX_PLATFORM_IS_WINDOWS
    // The size of wchar is 2 bytes on Windows (MSVC and Windows ABI).
    #define NEX_SIZEOF_WCHAR 2
#elif NEX_PLATFORM_IS_AIX
    #if defined(NEX_ADDRESS_MODEL_64BIT)
        // The size of wchar is 4 bytes on 64-bit mode on AIX.
        #define NEX_SIZEOF_WCHAR 4
    #else
        // The size of wchar is 2 byte on 32-bit mode on AIX.
        #define NEX_SIZEOF_WCHAR 2
    #endif
#elif NEX_PLATFORM_FAMILY_IS_UNIX || NEX_PLATFORM_FAMILY_IS_APPLE || NEX_PLATFORM_IS_LINUX
    // The size of wchar is 4 bytes on most Unix-like systems (Standard LP64/ILP32 Unix/POSIX ABI).
    // e.g., older IBM XL, Solaris Studio
    #define NEX_SIZEOF_WCHAR 4
#else
    #error Unable to determine size of wchar.
#endif  // ^^NEX_SIZEOF_WCHAR

// Determines if wchar is considered "short" (2 bytes) or not (4 bytes).
#if (NEX_SIZEOF_WCHAR == 2)
    #define NEX_SHORT_WCHAR 1
#elif (NEX_SIZEOF_WCHAR == 4)
    #define NEX_SHORT_WCHAR 0
#else
    #error Unsupported size of wchar.
#endif  // ^^NEX_SHORT_WCHAR
