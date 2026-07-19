/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <queue>

#include "nex/base/namespace.h"
#include "nex/core/containers/deque.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Queue (FIFO data structure)
 * 
 * @details
 * The Queue container is a First-In-First-Out (FIFO) data structure that allows elements to be added at the back
 * of the queue and removed from the front of the queue, commonly used for storing and manipulating collections
 * of data that require FIFO behavior.
 * 
 * @see https://en.cppreference.com/w/cpp/container/queue for more information on std::queue.
 */
template <typename Type, typename Container = Deque<Type>>
using Queue = NEX_STD queue<Type, Container>;

NEX_NAMESPACE_END
