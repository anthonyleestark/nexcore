/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <list>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief LinkedList (Doubly linked list)
 * 
 * @details 
 * A non-contiguous, node-based container that supports constant-time insertion and removal from any position 
 * once an iterator is obtained. Use this when frequent reordering or middle-insertions are required without 
 * invalidating pointers to other elements.
 * 
 * @see https://en.cppreference.com/w/cpp/container/list for more information on std::list.
 */
template <typename Type, typename Allocator = NEX_STD allocator<Type>>
using LinkedList = NEX_STD list<Type, Allocator>;

NEX_NAMESPACE_END