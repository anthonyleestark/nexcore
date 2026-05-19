/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  linear.h
 * @brief Defines common linear container types used throughout the codebase, such as Array, Vec, Deque, 
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
 * Container selection guidance:
 * - Use Array when size is known at compile time and contiguous fixed storage is desired.
 * - Use Vec when random access and amortized growth are the primary needs.
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

NEX_NAMESPACE_BEGIN

/**
 * @brief Array (fixed-size array)
 * @details 
 * Array is a wrapper around a stack-allocated (usually) fixed-size array. 
 * Use this when the number of elements is known at compile-time to avoid dynamic allocation.
 */
template <typename Type, usize Size>
using Array = NEX_STD array<Type, Size>;

/**
 * @brief Vec (dynamic array)
 * @details 
 * Vec is a dynamic array that provides fast indexed access and efficient insertion/deletion at the end. 
 * Elements are stored contiguously in memory, making it ideal for cases where the size may change over time 
 * and random access is required.
 */
template <typename Type, typename Allocator = NEX_STD allocator<Type>>
using Vec = NEX_STD vector<Type, Allocator>;

/**
 * @brief Deque (double-ended queue)
 * @details 
 * Deque is an indexed sequence container that allows fast insertion and deletion at both its beginning and its end. 
 * Unlike Vec, storage is not guaranteed to be contiguous, but it provides efficient access to both ends of the sequence, 
 * making it suitable for use cases like queues or stacks where elements are frequently added and removed from both ends.
 */
template <typename Type, typename Allocator = NEX_STD allocator<Type>>
using Deque = NEX_STD deque<Type, Allocator>;

NEX_NAMESPACE_END