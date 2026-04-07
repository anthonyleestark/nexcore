/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  adaptors.h
 * @brief Defines common adaptor container types used throughout the codebase, such as Stack, Queue, PriorityQueue, 
 *        and their associated type aliases.
 * 
 * @details
 * This header defines aliases for container adaptors that model constrained access strategies rather than
 * general-purpose traversal. It provides a shared naming layer for LIFO, FIFO, and priority-based processing
 * patterns used in NexSuite workflows.
 *
 * Adaptor containers are commonly used in command processing, staged execution, buffering, and scheduling
 * logic, where the retrieval policy is part of the business intent. Using these aliases makes that policy
 * explicit at the type level while keeping implementation code concise.
 *
 * Container selection guidance:
 * - Use Stack for depth-oriented processing, undo chains, and reverse-order consumption.
 * - Use Queue for arrival-order pipelines and producer-consumer style handoff.
 * - Use PriorityQueue when processing order is determined by priority rather than insertion time.
 *
 * These aliases keep code style consistent across modules and preserve the exact behavior of their Standard Library 
 * counterparts, without adding wrapper logic or runtime overhead.
 * 
 * @note
 * The adaptor container types defined in this file are implemented using the corresponding classes from the 
 * C++ Standard Library, and the type aliases provide a convenient way to use these containers with different 
 * types of elements.
 * 
 * @see https://en.cppreference.com/w/cpp/container for more information on C++ Standard Library containers.
 * @see https://en.cppreference.com/w/cpp/container/stack for more information on std::stack.
 * @see https://en.cppreference.com/w/cpp/container/queue for more information on std::queue.
 * @see https://en.cppreference.com/w/cpp/container/priority_queue for more information on std::priority_queue.
 */
 
#include <stack>
#include <queue>

#include "common/macros.h"
#include "common/types.h"

NEXSUITE_NAMESPACE_BEGIN

/**
 * @brief Stack (LIFO data structure) type aliases
 * @details
 * The Stack container is a Last-In-First-Out (LIFO) data structure that allows elements to be added and removed 
 * from the top of the stack, commonly used for storing and manipulating collections of data that require LIFO behavior.
 */
template <typename T, typename Container = NEXSUITE_STD deque<T>>
using Stack = NEXSUITE_STD stack<T, Container>;

/**
 * @brief Queue (FIFO data structure) type aliases
 * @details
 * The Queue container is a First-In-First-Out (FIFO) data structure that allows elements to be added at the back 
 * of the queue and removed from the front of the queue, commonly used for storing and manipulating collections 
 * of data that require FIFO behavior.
 */
template <typename T, typename Container = NEXSUITE_STD deque<T>>
using Queue = NEXSUITE_STD queue<T, Container>;

/**
 * @brief Priority Queue (heap-based data structure) type aliases
 * @details
 * The Priority Queue container is a heap-based data structure that allows elements to be added with a priority and 
 * removed based on their priority, commonly used for storing and manipulating collections of data that require 
 * priority-based behavior.
 */
template <typename T, typename Container = NEXSUITE_STD vector<T>>
using PriorityQueue = NEXSUITE_STD priority_queue<T, Container>;

NEXSUITE_NAMESPACE_END