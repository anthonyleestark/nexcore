/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <unordered_set>
#include <utility>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief HashSet (hash table based collection of unique elements)
 * 
 * @details
 * The HashSet container is an unordered container that stores unique elements. Fast retrieval is achieved
 * through hashing, and no particular order is guaranteed for the stored elements.
 * 
 * @see https://en.cppreference.com/w/cpp/container/unordered_set for more information on std::unordered_set.
 */
template <typename KeyType, 
          typename Hasher = NEX_STD hash<KeyType>, 
          typename KeyEqual = NEX_STD equal_to<KeyType>, 
          typename Allocator = NEX_STD allocator<KeyType>>
using HashSet = NEX_STD unordered_set<KeyType, Hasher, KeyEqual, Allocator>;

NEX_NAMESPACE_END
