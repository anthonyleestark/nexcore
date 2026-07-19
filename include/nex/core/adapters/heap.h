/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <queue>

#include "nex/base/namespace.h"
#include "nex/core/containers/vector.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Heap (heap-based data structure)
 * @details
 * The Heap container is a heap-based data structure that allows elements to be added with a priority and
 * removed based on their priority, commonly used for storing and manipulating collections of data that require
 * priority-based behavior.
 * 
 * @see https://en.cppreference.com/w/cpp/container/priority_queue for more information on std::priority_queue.
 */
template <typename Type, typename Container = Vec<Type>>
using Heap = NEX_STD priority_queue<Type, Container>;

NEX_NAMESPACE_END
