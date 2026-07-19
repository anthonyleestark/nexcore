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
 * @brief MultiSet (collection of unique elements that allows duplicate values)
 * 
 * @details
 * The MultiSet container is a container that stores elements in a specific order, where multiple elements can have
 * equivalent values.
 * 
 * @see https://en.cppreference.com/w/cpp/container/multiset for more information on std::multiset.
 */
template <typename KeyType, 
          typename Comparator = NEX_STD less<KeyType>, 
          typename Allocator = NEX_STD allocator<KeyType>>
using MultiSet = NEX_STD multiset<KeyType, Comparator, Allocator>;

NEX_NAMESPACE_END
