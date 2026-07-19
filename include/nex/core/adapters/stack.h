/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once
 
#include <stack>

#include "nex/base/namespace.h"
#include "nex/core/containers/deque.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Stack (LIFO data structure)
 * 
 * @details
 * The Stack container is a Last-In-First-Out (LIFO) data structure that allows elements to be added and removed
 * from the top of the stack, commonly used for storing and manipulating collections of data that require LIFO behavior.
 * 
 * @see https://en.cppreference.com/w/cpp/container/stack for more information on std::stack.
 */
template <typename Type, typename Container = Deque<Type>>
using Stack = NEX_STD stack<Type, Container>;

NEX_NAMESPACE_END
