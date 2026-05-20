/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    architecture.h
 * @brief   CPU architecture detection and feature macros
 * 
 * @details
 * This header centralizes compile-time CPU architecture detection and exposes the result as a small set of macros.
 * It distinguishes specific processor targets such as ARM64, ARMEL, x86, and x86_64, broader families such as ARM
 * and x86, and basic architecture traits including bit-width and endianness.
 * The detection logic relies on compiler-provided predefined macros so the rest of the codebase can use a single,
 * consistent interface for architecture-specific compilation.
 * Unsupported targets fail fast at compile time.
 * 
 * @section arch_processors Processor Architecture Macros
 * The following macros identify specific processor architectures:
 *  - NEX_ARCH_CPU_ARM64: 64-bit ARM architecture
 *  - NEX_ARCH_CPU_ARMEL: 32-bit ARM (little-endian)
 *  - NEX_ARCH_CPU_X86: 32-bit x86
 *  - NEX_ARCH_CPU_X86_64: 64-bit x86 (AMD64/Intel 64)
 * 
 * @section arch_families Processor Family Macros
 * The following macros group related architectures:
 *  - NEX_ARCH_CPU_ARM_FAMILY: ARM64 or ARMEL
 *  - NEX_ARCH_CPU_X86_FAMILY: X86 or X86_64
 * 
 * @section arch_features Processor Feature Macros
 * The following macros describe processor characteristics:
 *  - NEX_ARCH_CPU_32_BITS: 32-bit address space
 *  - NEX_ARCH_CPU_64_BITS: 64-bit address space
 *  - NEX_ARCH_CPU_BIG_ENDIAN: Big-endian byte order
 *  - NEX_ARCH_CPU_LITTLE_ENDIAN: Little-endian byte order
 */

/**
 * @section arch_detection Processor Architecture Detection
 * 
 * The following preprocessor conditionals detect the target CPU architecture
 * based on compiler-defined macros.
 * 
 * @see https://msdn.microsoft.com/en-us/library/b0084kay.aspx MSVC predefined macros
 * @see http://www.agner.org/optimize/calling_conventions.pdf Calling conventions guide
 * @note With GCC, you can list all predefined macros: `echo | gcc -E -dM -`
 */
#if defined(_M_X64) || defined(__x86_64__)
    #define NEX_ARCH_CPU_X86_FAMILY 1
    #define NEX_ARCH_CPU_X86_64 1
    #define NEX_ARCH_CPU_64_BITS 1
    #define NEX_ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
    #define NEX_ARCH_CPU_X86_FAMILY 1
    #define NEX_ARCH_CPU_X86 1
    #define NEX_ARCH_CPU_32_BITS 1
    #define NEX_ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__ARMEL__)
    #define NEX_ARCH_CPU_ARM_FAMILY 1
    #define NEX_ARCH_CPU_ARMEL 1
    #define NEX_ARCH_CPU_32_BITS 1
    #define NEX_ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define NEX_ARCH_CPU_ARM_FAMILY 1
    #define NEX_ARCH_CPU_ARM64 1
    #define NEX_ARCH_CPU_64_BITS 1
    #define NEX_ARCH_CPU_LITTLE_ENDIAN 1
#else
    #error CPU architecture is not supported or not detected
#endif
