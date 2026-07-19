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
 * @brief MultiMap (key-value associative container that allows duplicate keys)
 * @details
 * The MultiMap container is a container that stores elements formed by a combination of a key value and a mapped value.
 * Unlike TreeMap, multiple elements can have equivalent keys. Elements are typically ordered by key.
 * @see https://en.cppreference.com/w/cpp/container/multimap for more information on std::multimap.
 */
template <typename KeyType, typename ValueType, 
          typename Comparator = NEX_STD less<KeyType>, 
          typename Allocator = NEX_STD allocator<NEX_STD pair<const KeyType, ValueType>>>
using MultiMap = NEX_STD multimap<KeyType, ValueType, Comparator, Allocator>;

NEX_NAMESPACE_END
