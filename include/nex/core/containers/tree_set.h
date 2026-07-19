/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <set>
#include <utility>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief TreeSet (collection of unique elements)
 * 
 * @details
 * The TreeSet container is a collection of unique elements that does not allow duplicate values, commonly used
 * for storing and manipulating collections of data that require uniqueness.
 * 
 * @see https://en.cppreference.com/w/cpp/container/set for more information on std::set.
 */
template <typename KeyType, 
          typename Comparator = NEX_STD less<KeyType>, 
          typename Allocator = NEX_STD allocator<KeyType>>
using TreeSet = NEX_STD set<KeyType, Comparator, Allocator>;

NEX_NAMESPACE_END
