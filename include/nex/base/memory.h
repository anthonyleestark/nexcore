/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <memory>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @note
 * The `construct_at` and `destroy_at` functions are part of the C++20 standard library and provide a way 
 * to construct and destroy objects in a specified memory location.
 * These functions are particularly useful in low-level programming scenarios where you need to manage 
 * object lifetimes manually, such as in custom memory allocators or when working with uninitialized storage.
 * NexCore provides these functions in the `nex` namespace to ensure compatibility and to provide a consistent 
 * interface, while allowing for potential future extensions or customizations specific to the NexCore framework.
 * 
 * @see https://en.cppreference.com/w/cpp/memory/construct_at
 * @see https://en.cppreference.com/w/cpp/memory/destroy_at
 */

using NEX_STD construct_at;
using NEX_STD destroy_at;

/**
 * @note
 * The `memcpy`, `memmove`, and `memset` functions are standard C library functions that provide low-level
 * memory manipulation capabilities.
 * NexCore provides these functions in the `nex` namespace to ensure compatibility and to provide a consistent
 * interface, while allowing for potential future extensions or customizations specific to the NexCore framework.
 * 
 * @see https://en.cppreference.com/w/cpp/string/byte/memcpy
 * @see https://en.cppreference.com/w/cpp/string/byte/memmove
 * @see https://en.cppreference.com/w/cpp/string/byte/memset
 */

using NEX_STD memcpy;
using NEX_STD memmove;
using NEX_STD memset;

// ======================================================================================
// Define macros for constructing and destroying objects in a specified memory location
// ======================================================================================

// Constructs an object of type `Type` at the specified memory location `ptr` using the provided arguments.
#define NEX_CONSTRUCT_AT(ptr, ...) \
    NEX_PREPEND_NAMESPACE(construct_at(ptr, __VA_ARGS__))

// Destroys the object of type `Type` at the specified memory location `ptr`.
#define NEX_DESTROY_AT(ptr) \
    NEX_PREPEND_NAMESPACE(destroy_at(ptr))

// ======================================================================================
// Define macros for low-level memory manipulation functions
// ======================================================================================

// Copies `count` bytes from the memory area `src` to the memory area `dest`.
#define NEX_MEMCPY(dest, src, count) \
    NEX_PREPEND_NAMESPACE(memcpy(dest, src, count))

// Moves `count` bytes from the memory area `src` to the memory area `dest`.
// The memory areas may overlap.
#define NEX_MEMMOVE(dest, src, count) \
    NEX_PREPEND_NAMESPACE(memmove(dest, src, count))

// Sets the first `count` bytes of the memory area pointed to by `dest` to the specified value `value`.
#define NEX_MEMSET(dest, value, count) \
    NEX_PREPEND_NAMESPACE(memset(dest, value, count))

NEX_NAMESPACE_END
