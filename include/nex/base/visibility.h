/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  visibility.h
 * @brief Defines symbol visibility annotation macros.
 *
 * @details
 * This header provides a compiler-independent interface for controlling the visibility of symbols
 * across shared-library boundaries.
 *
 * It defines annotations for explicitly exporting symbols, hiding symbols from external consumers,
 * and controlling symbol visibility where supported by the active compiler and platform.
 *
 * Compiler-specific visibility mechanisms are abstracted behind a consistent set of NexCore macros.
 *
 * @note
 * Symbol visibility support and behavior vary across compilers, platforms, and binary formats.
 *
 * @see compiler_features.h
 * @see export.h
 */

#include "nex/base/compiler.h"
#include "nex/base/compiler_features.h"

/**
 * @def NEX_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
 * @brief Marks a symbol to be excluded from explicit template instantiation.
 *
 * @details
 * This macro expands to the appropriate compiler-specific attribute to exclude a symbol
 * from explicit template instantiation.
 * If the compiler does not support this attribute, it falls back to always inlining the symbol
 * to approximate the effect.
 */
#if NEX_HAS_ATTRIBUTE(exclude_from_explicit_instantiation)
    #define NEX_EXCLUDE_FROM_EXPLICIT_INSTANTIATION \
        NEX_GCC_OR_CLANG_ATTRIBUTE(__exclude_from_explicit_instantiation__)
#else
    // Try to approximate the effect of exclude_from_explicit_instantiation
    // (which is that entities are not assumed to be provided by explicit
    // template instantiations in the dylib) by always inlining those entities.
    #define NEX_EXCLUDE_FROM_EXPLICIT_INSTANTIATION NEX_ALWAYS_INLINE
#endif  // ^^NEX_EXCLUDE_FROM_EXPLICIT_INSTANTIATION

/**
 * @def NEX_VISIBILITY
 * @brief Annotates a symbol with a specific visibility for shared libraries.
 * 
 * @details
 * This macro can be used to control the visibility of symbols in shared libraries.
 * It expands to the appropriate compiler-specific attribute based on the detected compiler.
 */
#if NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_GCC_COMPATIBLE
    // On Clang and GCC, we can use the visibility attribute to control symbol export.
    #define NEX_VISIBILITY(vis) NEX_GCC_OR_CLANG_ATTRIBUTE(visibility(#vis))
#else
    // On MSVC, there is no direct equivalent to GCC's visibility attribute,
    // so we define it as a no-op.
    #define NEX_VISIBILITY(vis)
#endif  // ^^NEX_VISIBILITY

#if !defined(NEX_HIDDEN)
    // Marks a symbol as hidden from the public API using the visibility attribute.
    #define NEX_HIDDEN NEX_VISIBILITY(hidden)
#endif  // ^^NEX_HIDDEN

#if !defined(NEX_HIDDEN_FROM_ABI)
    // Marks a symbol as hidden from the public ABI,
    // which can help reduce symbol collisions and improve load times in shared libraries.
    #define NEX_HIDDEN_FROM_ABI \
        NEX_HIDDEN NEX_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#endif  // ^^NEX_HIDDEN_FROM_ABI

/**
 * @def NEX_INTERNAL
 * @brief Marks a symbol as internal and not recommended for public use.
 *
 * @details
 * This macro ensures that the annotated symbol is only used internally and is not recommended for use in public APIs.
 * It helps prevent accidental usage of internal symbols in public-facing code and can improve encapsulation
 * and maintainability.
 *
 * @note
 * This macro won't make the symbol completely inaccessible, but it signals that it is intended for internal use only.
 * Users can still access the symbol if necessary, but it is strongly discouraged and should be done with caution.
 */
#if !defined(NEX_INTERNAL)
    #define NEX_INTERNAL NEX_VISIBILITY(hidden)
#endif  // ^^NEX_INTERNAL
