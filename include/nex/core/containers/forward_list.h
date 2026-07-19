/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <list>
#include <forward_list>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief ForwardList (Singly linked list)
 * 
 * @details 
 * A space-optimized, one-way linked list. It provides efficient insertion and deletion but only supports 
 * forward traversal. This container has a lower memory overhead per element compared to a doubly linked List 
 * as it only stores a single next-pointer per node.
 * 
 * @see https://en.cppreference.com/w/cpp/container/forward_list for more information on std::forward_list.
 */
template <typename Type, typename Allocator = NEX_STD allocator<Type>>
using ForwardList = NEX_STD forward_list<Type, Allocator>;

NEX_NAMESPACE_END