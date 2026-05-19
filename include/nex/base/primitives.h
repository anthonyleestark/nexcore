/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  primitives.h
 * @brief Defines fundamental concurrency primitives such as mutexes, condition variables, and atomic types.
 * 
 * @details
 * This header defines fundamental concurrency primitives such as mutexes, condition variables, and atomic types.
 * These primitives are the building blocks for thread synchronization and communication in concurrent programming.
 * The primitives defined in this header are designed to be used in multi-threaded environments to ensure thread safety
 * and prevent data races. They provide mechanisms for mutual exclusion, thread synchronization, and atomic operations 
 * on shared data. It is important to use these primitives correctly to avoid issues such as deadlocks, race conditions, 
 * and other concurrency-related bugs.
 * 
 * @note
 * The primitive types defined in this file are mapped to standard C++ synchronization primitives.
 * These primitives are typically used in conjunction with higher-level abstractions (like lock guards, scoped locks, etc.)
 * to manage resource acquisition and release in a safe and exception-friendly manner.
 * 
 * @see https://en.cppreference.com/w/cpp/atomic/atomic for more information on std::atomic.
 * @see https://en.cppreference.com/w/cpp/thread/mutex for more information on std::mutex.
 * @see https://en.cppreference.com/w/cpp/thread/recursive_mutex for more information on std::recursive_mutex.
 * @see https://en.cppreference.com/w/cpp/thread/shared_mutex for more information on std::shared_mutex.
 * @see https://en.cppreference.com/w/cpp/thread/condition_variable for more information on std::condition_variable.
 * @see https://en.cppreference.com/w/cpp/thread/condition_variable_any for more information on std::condition_variable_any.
 */

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Template for thread-safe, atomic operations on a value.
 * @details 
 * Provides atomic access to a shared variable without the overhead of a mutex. 
 * It guarantees that operations are indivisible and prevents data races in multi-threaded environments.
 */
template <typename Type>
using Atomic = NEX_STD atomic<Type>;

/**
 * @brief A lock-free atomic boolean flag.
 * @details 
 * The simplest atomic type, guaranteed to be lock-free on all supported platforms. 
 * Ideal for building low-level synchronization primitives like spinlocks or simple "stop" signals.
 */
using AtomicFlag = NEX_STD atomic_flag;

/**
 * @brief Standard mutual exclusion primitive.
 * @details 
 * Basic synchronization object used to protect shared data from concurrent access. 
 * Only one thread can own the mutex at any given time.
 */
using Mutex = NEX_STD mutex;

/**
 * @brief Mutual exclusion primitive that can be locked multiple times by the same thread.
 * @details 
 * Allows a thread to acquire the same mutex multiple times without causing a deadlock. 
 * Useful for recursive function calls that require locking the same mutex.
 */
using TimedMutex = NEX_STD timed_mutex;

/**
 * @brief Shared mutex for Reader-Writer scenarios.
 * @details 
 * Supports two levels of access: 'shared' (multiple threads can read simultaneously) and 'exclusive' 
 * (only one thread can write). Optimized for workloads where reads are more frequent than writes.
 */
using SharedMutex = NEX_STD shared_mutex;

/**
 * @brief Shared timed mutex for Reader-Writer scenarios with timeout capabilities.
 * @details 
 * Similar to SharedMutex but allows threads to attempt to acquire locks with a timeout, preventing indefinite blocking. 
 * Ideal for scenarios where responsiveness is critical and waiting indefinitely is not acceptable.
 */
using SharedTimedMutex = NEX_STD shared_timed_mutex;

/**
 * @brief Mutex that can be locked multiple times by the same thread.
 * @details 
 * Prevents deadlocks when a thread calls a sequence of functions that each require locking the same mutex. 
 * Use sparingly as it often indicates a need for refactoring.
 */
using RecursiveMutex = NEX_STD recursive_mutex;

/**
 * @brief Mutex that can be locked multiple times by the same thread with timeout capabilities.
 * @details 
 * Similar to RecursiveMutex but allows threads to attempt to acquire locks with a timeout, 
 * preventing indefinite blocking. Useful for scenarios where a thread may need to wait for a resource 
 * but should not block indefinitely.
 */
using RecursiveTimedMutex = NEX_STD recursive_timed_mutex;

/**
 * @brief Condition variable for thread synchronization.
 * @details 
 * Allows threads to block until a particular condition is met. 
 * Used in conjunction with a mutex to protect shared data and avoid spurious wakeups.
 * @note Always use condition variables with a predicate to avoid missed notifications and spurious wakeups.
 */
using ConditionVariable = NEX_STD condition_variable;

/**
 * @brief Condition variable that can work with any lock type.
 * @details 
 * More flexible than ConditionVariable as it can be used with any basic-lockable type, 
 * such as UniqueLock<SharedMutex>. This allows for more complex synchronization patterns.
 * @note Always use condition variables with a predicate to avoid missed notifications and spurious wakeups.
 */
using ConditionVariableAny = NEX_STD condition_variable_any;

// =================================================================================
// Type aliases for atomic types with specific value types
// =================================================================================

using AtomicInt = Atomic<int32>;            // Atomic 32-bit signed integer
using AtomicUInt = Atomic<uint32>;          // Atomic 32-bit unsigned integer
using AtomicInt64 = Atomic<int64>;          // Atomic 64-bit signed integer
using AtomicUInt64 = Atomic<uint64>;        // Atomic 64-bit unsigned integer
using AtomicBool = Atomic<boolean>;         // Atomic boolean flag

NEX_NAMESPACE_END