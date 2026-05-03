/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  linked.h
 * @brief Defines common linked container types used throughout the codebase, such as LinkedList, ForwardList, 
 *        and their associated type aliases.
 * 
 * @details
 * This header defines aliases for node-based linked containers used in Nex-ecosystem. It provides a consistent
 * naming layer for doubly linked and singly linked list types so modules can communicate traversal and
 * mutation characteristics directly through their type choices.
 *
 * Linked containers are useful when insertion and erasure at arbitrary positions are more important than
 * contiguous storage or constant-time random access. By exposing these aliases centrally, the codebase can
 * use linked structures where they are appropriate without mixing naming styles across services and domains.
 *
 * Container selection guidance:
 * - Use LinkedList when bidirectional traversal and stable iterators are needed.
 * - Use ForwardList when memory footprint and forward-only traversal are sufficient.
 *
 * The aliases in this file do not add behavior; they preserve the exact semantics and complexity guarantees
 * of the corresponding Standard Library containers while improving readability and maintainability.
 * 
 * @note
 * The linked container types defined in this file are implemented using the corresponding classes from the 
 * C++ Standard Library, and the type aliases provide a convenient way to use these containers with different 
 * types of elements.
 * 
 * @see https://en.cppreference.com/w/cpp/container for more information on C++ Standard Library containers.
 * @see https://en.cppreference.com/w/cpp/container/list for more information on std::list.
 * @see https://en.cppreference.com/w/cpp/container/forward_list for more information on std::forward_list.
 */

#include <list>
#include <forward_list>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief LinkedList (Doubly linked list)
 * @details 
 * A non-contiguous, node-based container that supports constant-time insertion and removal from any position 
 * once an iterator is obtained. Use this when frequent reordering or middle-insertions are required without 
 * invalidating pointers to other elements.
 */
template <typename T>
using LinkedList = NEX_STD list<T>;

/**
 * @brief ForwardList (Singly linked list)
 * @details 
 * A space-optimized, one-way linked list. It provides efficient insertion and deletion but only supports 
 * forward traversal. This container has a lower memory overhead per element compared to a doubly linked List 
 * as it only stores a single next-pointer per node.
 */
template <typename T>
using ForwardList = NEX_STD forward_list<T>;

NEX_NAMESPACE_END