/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  linear.h
 * @brief Defines common linear container types used throughout the codebase, such as Array, ArrayList, Deque, 
 *        and their associated type aliases.
 * 
 * @details
 * This header centralizes aliases for sequence-oriented containers used by Nex-ecosystem, including fixed-size
 * arrays, dynamic arrays, and double-ended queues. The goal is to provide a single, stable vocabulary for
 * linear data structures so APIs and internal implementations remain readable and consistent across modules.
 *
 * The aliases intentionally mirror Standard Library semantics while reducing template verbosity in call sites.
 * This allows higher-level code to express intent first (for example, fixed-size storage versus growth-oriented
 * storage) without repeating long concrete type spellings.
 *
 * This file also includes common project-level aliases such as IntArray, StringArray, and StringList to cover
 * frequent patterns in configuration, parsing, and service orchestration code. These aliases are convenience
 * types only; no behavior is added or modified.
 *
 * Container selection guidance:
 * - Use Array when size is known at compile time and contiguous fixed storage is desired.
 * - Use ArrayList when random access and amortized growth are the primary needs.
 * - Use Deque when efficient insertion and removal are required at both front and back.
 *
 * Consolidating these aliases in one header helps keep container policy explicit, reduces duplication,
 * and simplifies future refactors if naming or container strategy evolves.
 * 
 * @note
 * The linear container types defined in this file are implemented using the corresponding classes from the 
 * C++ Standard Library, and the type aliases provide a convenient way to use these containers with different 
 * types of elements and sizes.
 * 
 * @see https://en.cppreference.com/w/cpp/container for more information on C++ Standard Library containers.
 * @see https://en.cppreference.com/w/cpp/container/array for more information on std::array.
 * @see https://en.cppreference.com/w/cpp/container/vector for more information on std::vector.
 * @see https://en.cppreference.com/w/cpp/container/deque for more information on std::deque.
 */

#include <vector>
#include <array>
#include <deque>

#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Array (fixed-size array) type aliases
 * @details A wrapper around a stack-allocated (usually) fixed-size array. 
 * Use this when the number of elements is known at compile-time to avoid dynamic allocation.
 */
template <typename T, usize Size>
using Array = NEX_STD array<T, Size>;

/**
 * @brief ArrayList (dynamic array) type aliases
 * @details A dynamic array that provides fast indexed access and efficient insertion/deletion at the end. 
 * Elements are stored contiguously in memory.
 */
template <typename T, typename Allocator = NEX_STD allocator<T>>
using ArrayList = NEX_STD vector<T, Allocator>;

/**
 * @brief Deque (double-ended queue) type aliases
 * @details An indexed sequence container that allows fast insertion and deletion at both its beginning and its end. 
 * Unlike ArrayList, storage is not guaranteed to be contiguous.
 */
template <typename T, typename Allocator = NEX_STD allocator<T>>
using Deque = NEX_STD deque<T, Allocator>;

/**
 * @section Common container type aliases
 * @details
 * These type aliases provide convenient names for commonly used container types with specific element types.
 * The type aliases can help to improve code readability and maintainability by providing consistent type names 
 * for commonly used container types with specific element types.
 */

/**
 * @brief Fixed-size numeric array aliases.
 * @details Provides convenient, fixed-width numeric types for static buffers. 
 * These aliases ensure type consistency for 32-bit and 64-bit integer or float arrays.
 */
template <usize Size>
using IntArray = Array<int32, Size>;

template <usize Size>
using UIntArray = Array<uint32, Size>;

template <usize Size>
using Int64Array = Array<int64, Size>;

template <usize Size>
using UInt64Array = Array<uint64, Size>;

template <usize Size>
using FloatArray = Array<float32, Size>;

template <usize Size>
using DoubleArray = Array<float64, Size>;

/**
 * @brief Fixed-size string array alias.
 * @details Provides a convenient alias for fixed-size arrays of String objects, commonly used for 
 * static configuration or small collections of text data where the size is known at compile time.
 */
template <usize Size>
using StringArray = Array<NEX_PREPEND_CORE_NAMESPACE(String), Size>;

/**
 * @brief Dynamic numeric and string list aliases.
 * @details Common aliases for ArrayList to simplify signatures in business logic.
 * These are the preferred containers for general-purpose dynamic collections.
 */
using IntList = ArrayList<int32>;
using UIntList = ArrayList<uint32>;

using Int64List = ArrayList<int64>;
using UInt64List = ArrayList<uint64>;

using FloatList = ArrayList<float32>;
using DoubleList = ArrayList<float64>;

using StringList = ArrayList<NEX_PREPEND_CORE_NAMESPACE(String)>;

NEX_NAMESPACE_END