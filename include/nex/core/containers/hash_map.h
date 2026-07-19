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
 * @brief HashMap (hash table based key-value associative container)
 * 
 * @details
 * The HashMap container is an unordered associative container that maps keys to values. It provides average
 * constant-time complexity for insertions and lookups using hashing.
 * 
 * @see https://en.cppreference.com/w/cpp/container/unordered_map for more information on std::unordered_map.
 */
template <typename KeyType, typename ValueType, 
          typename Hasher = NEX_STD hash<KeyType>, 
          typename KeyEqual = NEX_STD equal_to<KeyType>,
          typename Allocator = NEX_STD allocator<NEX_STD pair<const KeyType, ValueType>>>
using HashMap = NEX_STD unordered_map<KeyType, ValueType, Hasher, KeyEqual, Allocator>;

NEX_NAMESPACE_END
