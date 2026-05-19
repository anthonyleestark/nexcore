/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  coordinators.h
 * @brief Defines synchronization primitives such as semaphores, latches, and barriers for thread coordination.
 * 
 * @details
 * This header encapsulates high-level thread synchronization and orchestration mechanisms introduced in modern 
 * C++ (C++20 and later). Unlike low-level Primitives (such as Mutex or ConditionVariable) which focus on micro-level 
 * resource protection and mutual exclusion, Coordinators operate at the macro-level. They are architectural components 
 * designed to synchronize execution flow, manage lifecycle phases, and orchestrate progress across a collective 
 * group of concurrent threads.
 * By abstracting away the complex, error-prone boilerplate of manually stitching together mutexes and condition variables, 
 * coordinators provide safe, out-of-the-box structural patterns for parallel algorithms.
 * 
 * @see https://en.cppreference.com/w/cpp/thread/counting_semaphore for more information on std::counting_semaphore.
 * @see https://en.cppreference.com/w/cpp/thread/binary_semaphore for more information on std::binary_semaphore.
 * @see https://en.cppreference.com/w/cpp/thread/latch for more information on std::latch.
 * @see https://en.cppreference.com/w/cpp/thread/barrier for more information on std::barrier.
 */

#include <semaphore>
#include <latch>
#include <barrier>

#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Counting semaphore for controlling access to a resource pool.
 * @details 
 * Maintains a count of available resources and blocks threads when the count reaches zero. 
 * Ideal for managing a fixed number of identical resources (like a connection pool).
 */
template <isize LeastMaxValue = 1024>
using CountingSemaphore = NEX_STD counting_semaphore<LeastMaxValue>;

/**
 * @brief Binary semaphore for simple signaling between threads.
 * @details
 * A special case of counting semaphore that can only be in two states: "available" (count = 1) 
 * or "unavailable" (count = 0). Useful for simple synchronization scenarios where one thread needs 
 * to signal another to proceed.
 */
using BinarySemaphore = NEX_STD binary_semaphore;

/**
 * @brief Latch for one-time synchronization.
 * @details 
 * A latch allows threads to wait until a certain number of events have occurred. 
 * Once the count reaches zero, all waiting threads are released and the latch cannot be reused.
 */
using Latch = NEX_STD latch;

/**
 * @brief Barrier for reusable thread synchronization.
 * @details 
 * A barrier allows a group of threads to wait for each other at a certain point in the code. 
 * Once all threads have reached the barrier, they are all released and the barrier can be reused.
 */
template <typename Func = void(*)()>
using Barrier = NEX_STD barrier<Func>;

NEX_NAMESPACE_END
