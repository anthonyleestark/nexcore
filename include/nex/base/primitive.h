/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  primitives.h
 * @brief Defines core synchronization and hardware-level primitive types, such as Atomics, Mutexes, 
 *        Condition Variables, Semaphores, and their associated type aliases.
 * 
 * @details
 * This header centralizes aliases for fundamental building blocks used by Nex-ecosystem for concurrency control, 
 * thread synchronization, and atomic memory operations. Unlike wrappers that manage lifetimes, primitives provide 
 * the underlying mechanisms for mutual exclusion, signaling, and lock-free programming. It establishes a uniform 
 * naming convention for these low-level structures so that core infrastructure components can interact with hardware 
 * and OS scheduling features consistently. 
 * Primitives are foundational for protecting critical sections, coordinating task execution between threads, 
 * and ensuring memory consistency across multi-core systems. Grouping these aliases in one place ensures that 
 * threading policies remain cohesive across infrastructure, platform, and service layers.
 * 
 * @note
 * The primitive types defined in this file are mapped to standard C++ synchronization primitives. These types 
 * represent the most basic units of concurrency and should typically be managed by RAII wrappers (like lock_guard) 
 * at the application level to ensure exception safety.
 * 
 * @see https://en.cppreference.com/w/cpp/atomic/atomic for more information on std::atomic.
 * @see https://en.cppreference.com/w/cpp/thread/mutex for more information on std::mutex.
 * @see https://en.cppreference.com/w/cpp/thread/recursive_mutex for more information on std::recursive_mutex.
 * @see https://en.cppreference.com/w/cpp/thread/shared_mutex for more information on std::shared_mutex.
 * @see https://en.cppreference.com/w/cpp/thread/condition_variable for more information on std::condition_variable.
 * @see https://en.cppreference.com/w/cpp/thread/counting_semaphore for more information on std::counting_semaphore.
 * @see https://en.cppreference.com/w/cpp/thread/binary_semaphore for more information on std::binary_semaphore.
 * @see https://en.cppreference.com/w/cpp/thread/latch for more information on std::latch.
 * @see https://en.cppreference.com/w/cpp/thread/barrier for more information on std::barrier.
 */

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <semaphore>
#include <latch>
#include <barrier>

#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Template for thread-safe, atomic operations on a value.
 * @details 
 * Provides atomic access to a shared variable without the overhead 
 * of a mutex. It guarantees that operations are indivisible and prevents 
 * data races in multi-threaded environments.
 */
template <typename T>
using Atomic = NEX_STD atomic<T>;

/**
 * @brief A lock-free atomic boolean flag.
 * @details 
 * The simplest atomic type, guaranteed to be lock-free on all 
 * supported platforms. Ideal for building low-level synchronization 
 * primitives like spinlocks or simple "stop" signals.
 */
using AtomicFlag = NEX_STD atomic_flag;

/**
 * @brief Standard mutual exclusion primitive.
 * @details 
 * Basic synchronization object used to protect shared data from 
 * concurrent access. Only one thread can own the mutex at any given time.
 */
using Mutex = NEX_STD mutex;

/**
 * @brief Shared mutex for Reader-Writer scenarios.
 * @details 
 * Supports two levels of access: 'shared' (multiple threads can read 
 * simultaneously) and 'exclusive' (only one thread can write). Optimized for 
 * workloads where reads are more frequent than writes.
 */
using SharedMutex = NEX_STD shared_mutex;

/**
 * @brief Mutex that can be locked multiple times by the same thread.
 * @details 
 * Prevents deadlocks when a thread calls a sequence of functions 
 * that each require locking the same mutex. Use sparingly as it often 
 * indicates a need for refactoring.
 */
using RecursiveMutex = NEX_STD recursive_mutex;

/**
 * @brief Condition variable for thread synchronization.
 * @details 
 * Allows threads to block until a particular condition is met. 
 * Used in conjunction with a mutex to protect shared data and avoid spurious wakeups.
 * @note Always use condition variables with a predicate to avoid missed notifications and spurious wakeups.
 */
using ConditionVariable = NEX_STD condition_variable;

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
using Barrier = NEX_STD barrier;

/**
 * @brief Common primitive type aliases
 * @details
 * These type aliases provide convenient names for commonly used primitive types with specific types of values, 
 * objects, and functions. The type aliases can help to improve code readability and maintainability by providing 
 * consistent type names for commonly used primitive types with specific types of values, objects, and functions.
 */

/**
 * @brief Thread-safe integer aliases for concurrent counters and flags.
 * @details Provides atomic primitives for common integer types. These ensure 
 * that modifications (like increments or exchanges) are safe across multiple 
 * threads without requiring a Mutex.
 */
using AtomicInt = Atomic<int32>;
using AtomicUInt = Atomic<uint32>;

using AtomicInt64 = Atomic<int64>;
using AtomicUInt64 = Atomic<uint64>;

using AtomicBool = Atomic<bool>;

NEX_NAMESPACE_END