/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <unordered_map>
#include <utility>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief HashMultiMap (hash table based key-value associative container that allows duplicate keys)
 * 
 * @details
 * The HashMultiMap is an unordered associative container that maps a single key to multiple values.
 * Unlike a standard Map, it allows for duplicate keys and does not guarantee any specific element ordering.
 * 
 * @see https://en.cppreference.com/w/cpp/container/unordered_multimap for more information on std::unordered_multimap.
 */
template <typename KeyType, typename ValueType, 
          typename Hasher = NEX_STD hash<KeyType>, 
          typename KeyEqual = NEX_STD equal_to<KeyType>, 
          typename Allocator = NEX_STD allocator<NEX_STD pair<const KeyType, ValueType>>>
using HashMultiMap = NEX_STD unordered_multimap<KeyType, ValueType, Hasher, KeyEqual, Allocator>;

NEX_NAMESPACE_END
