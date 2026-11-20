/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  preprocessor.h
 * @brief Provides general-purpose preprocessor utility macros.
 *
 * @details
 * This header defines reusable preprocessor utilities for macro expansion, stringification,
 * token concatenation, unique identifier generation, and variadic argument handling.
 *
 * These utilities are intended to simplify macro definitions and reduce repetitive
 * preprocessor operations throughout NexCore.
 *
 * @note
 * This header contains general-purpose preprocessor utilities and does not define
 * compiler-specific feature detection.
 */

// =================================================================================
// Define macros for compilation and preprocessor utilities
// =================================================================================

#define NEX_STRINGIFY_IMPL(x)       #x
#define NEX_CONCAT_IMPL(a, b)       a##b
#define NEX_EXPAND(x)               x

// Stringify a macro argument
#define NEX_STRINGIFY(x) \
    NEX_STRINGIFY_IMPL(x)

// Concatenate two macro arguments into a single token
#define NEX_CONCAT(a, b) \
    NEX_CONCAT_IMPL(a, b)

// Concatenate three macro arguments into a single token
#define NEX_CONCAT3(a, b, c) \
    NEX_CONCAT(NEX_CONCAT(a, b), c)

// Concatenate four macro arguments into a single token
#define NEX_CONCAT4(a, b, c, d) \
    NEX_CONCAT(NEX_CONCAT3(a, b, c), d)

#define NEX_MAKE_UNIQUE_NAME_IMPL(prefix, line) NEX_CONCAT(prefix, line)

// Generates a unique name by concatenating a prefix with the current line number
#define NEX_MAKE_UNIQUE_NAME(prefix) \
    NEX_MAKE_UNIQUE_NAME_IMPL(prefix, __LINE__)

#define NEX_VA_ARG_COUNT_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10, N, ...) N

// Counts the number of arguments passed to a variadic macro
#define NEX_VA_ARG_COUNT(...) \
    NEX_EXPAND(NEX_VA_ARG_COUNT_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))

// =================================================================================
// Defines macros for working with types
// =================================================================================

/**
 * @def NEX_ALIGNOF(type)
 * @brief Returns the alignment requirement of the specified types in bytes.
 * @return The alignment requirement of the type in bytes.
 */
#define NEX_ALIGNOF(...) alignof(__VA_ARGS__)
