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
 *  - NEX_ARCH_CPU_S390: 31-bit IBM System/390
 *  - NEX_ARCH_CPU_S390X: 64-bit IBM System/390
 *  - NEX_ARCH_CPU_PPC64: 64-bit PowerPC
 *  - NEX_ARCH_CPU_MIPS: 32-bit MIPS
 *  - NEX_ARCH_CPU_MIPS64: 64-bit MIPS
 *  - NEX_ARCH_CPU_MIPSEL: 32-bit MIPS (little-endian)
 * 
 * @section arch_families Processor Family Macros
 * The following macros group related architectures:
 *  - NEX_ARCH_CPU_ARM_FAMILY: ARM64 or ARMEL
 *  - NEX_ARCH_CPU_X86_FAMILY: X86 or X86_64
 *  - NEX_ARCH_CPU_S390_FAMILY: S390 or S390X
 *  - NEX_ARCH_CPU_PPC64_FAMILY: PPC64
 *  - NEX_ARCH_CPU_MIPS_FAMILY: MIPS, MIPS64, MIPSEL
 * 
 * @section arch_features Processor Feature Macros
 * The following macros describe processor characteristics:
 *  - NEX_ARCH_CPU_31_BITS: 31-bit address space
 *  - NEX_ARCH_CPU_32_BITS: 32-bit address space
 *  - NEX_ARCH_CPU_64_BITS: 64-bit address space
 *  - NEX_ARCH_CPU_BIG_ENDIAN: Big-endian byte order
 *  - NEX_ARCH_CPU_LITTLE_ENDIAN: Little-endian byte order
 */

/**
 * @section arch_detection Processor Architecture Detection
 * 
 * @details
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
#elif defined(__s390x__)
    #define NEX_ARCH_CPU_S390_FAMILY 1
    #define NEX_ARCH_CPU_S390X 1
    #define NEX_ARCH_CPU_64_BITS 1
    #define NEX_ARCH_CPU_BIG_ENDIAN 1
#elif defined(__s390__)
    #define NEX_ARCH_CPU_S390_FAMILY 1
    #define NEX_ARCH_CPU_S390 1
    #define NEX_ARCH_CPU_31_BITS 1
    #define NEX_ARCH_CPU_BIG_ENDIAN 1
#elif (defined(__PPC64__) || defined(__PPC__)) && defined(__BIG_ENDIAN__)
    #define NEX_ARCH_CPU_PPC64_FAMILY 1
    #define NEX_ARCH_CPU_PPC64 1
    #define NEX_ARCH_CPU_64_BITS 1
    #define NEX_ARCH_CPU_BIG_ENDIAN 1
#elif defined(__PPC64__)
    #define NEX_ARCH_CPU_PPC64_FAMILY 1
    #define NEX_ARCH_CPU_PPC64 1
    #define NEX_ARCH_CPU_64_BITS 1
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
#elif defined(__pnacl__) || defined(__asmjs__) || defined(__wasm__)
    #define NEX_ARCH_CPU_32_BITS 1
    #define NEX_ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__MIPSEL__)
    #if defined(__LP64__)
        #define NEX_ARCH_CPU_MIPS_FAMILY 1
        #define NEX_ARCH_CPU_MIPS64EL 1
        #define NEX_ARCH_CPU_64_BITS 1
        #define NEX_ARCH_CPU_LITTLE_ENDIAN 1
    #else
        #define NEX_ARCH_CPU_MIPS_FAMILY 1
        #define NEX_ARCH_CPU_MIPSEL 1
        #define NEX_ARCH_CPU_32_BITS 1
        #define NEX_ARCH_CPU_LITTLE_ENDIAN 1
    #endif
#elif defined(__MIPSEB__)
    #if defined(__LP64__)
        #define NEX_ARCH_CPU_MIPS_FAMILY 1
        #define NEX_ARCH_CPU_MIPS64 1
        #define NEX_ARCH_CPU_64_BITS 1
        #define NEX_ARCH_CPU_BIG_ENDIAN 1
    #else
        #define NEX_ARCH_CPU_MIPS_FAMILY 1
        #define NEX_ARCH_CPU_MIPS 1
        #define NEX_ARCH_CPU_32_BITS 1
        #define NEX_ARCH_CPU_BIG_ENDIAN 1
    #endif
#else
    #error CPU architecture is not supported or not detected
    #error Please add support for your architecture in include/nex/base/architecture.h
#endif  // ^^^ Architecture detection

/**
 * @section arch_endianness Endianness Detection
 * 
 * @details
 * The following preprocessor conditionals detect the endianness of the target CPU architecture.
 * The endianness is determined based on the architecture macros defined above.
 * If the endianness cannot be determined, a compilation error will be raised.
 * 
 * @see Compiler endian detection macros in include/nex/base/compiler.h
 */

#if defined(NEX_ARCH_CPU_LITTLE_ENDIAN) && NEX_ARCH_CPU_LITTLE_ENDIAN
    #define NEX_ARCH_CPU_IS_LITTLE_ENDIAN 1
    #define NEX_ARCH_CPU_IS_BIG_ENDIAN 0
#elif defined(NEX_ARCH_CPU_BIG_ENDIAN) && NEX_ARCH_CPU_BIG_ENDIAN
    #define NEX_ARCH_CPU_IS_BIG_ENDIAN 1
    #define NEX_ARCH_CPU_IS_LITTLE_ENDIAN 0
#else
    #error Unable to detect endianness of the target architecture.
#endif  // ^^^ Endianness detection
