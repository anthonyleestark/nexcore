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
 * @brief HashMultiSet (hash table based collection of unique elements that allows duplicate values)
 * 
 * @details
 * The HashMultiSet container is a container that stores a collection of elements where multiple instances
 * of the same value are permitted. Unlike MultiSet, elements are not stored in any particular order, but offer
 * average constant-time complexity for operations.
 * 
 * @see https://en.cppreference.com/w/cpp/container/unordered_multiset for more information on std::unordered_multiset.
 */
template <typename KeyType, 
          typename Hasher = NEX_STD hash<KeyType>, 
          typename KeyEqual = NEX_STD equal_to<KeyType>, 
          typename Allocator = NEX_STD allocator<KeyType>>
using HashMultiSet = NEX_STD unordered_multiset<KeyType, Hasher, KeyEqual, Allocator>;

NEX_NAMESPACE_END
