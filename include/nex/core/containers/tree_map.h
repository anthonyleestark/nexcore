/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <map>
#include <utility>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief TreeMap (key-value associative container)
 * 
 * @details
 * The TreeMap container is a key-value associative container that stores pairs of keys and values, commonly used
 * for storing and manipulating collections of data that have a key-value relationship.
 * 
 * @see https://en.cppreference.com/w/cpp/container/map for more information on std::map.
 */
template <typename KeyType, typename ValueType, 
          typename Comparator = NEX_STD less<KeyType>, 
          typename Allocator = NEX_STD allocator<NEX_STD pair<const KeyType, ValueType>>>
using TreeMap = NEX_STD map<KeyType, ValueType, Comparator, Allocator>;

NEX_NAMESPACE_END
