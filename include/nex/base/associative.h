/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  associative.h
 * @brief Defines common associative container types used throughout the codebase, 
 *        such as TreeMap, TreeSet, HashMap, HashSet, and their associated type aliases.
 * 
 * @details
 * This header centralizes aliases for associative containers used by Nex-ecosystem, including ordered/unordered maps, 
 * sets, and their multi-entry variants. It establishes a uniform naming convention to ensure that interface 
 * signatures communicate their behavior clearly and consistently across the codebase.
 *
 * These containers are foundational for configuration handling, indexing, and identity lookup. Centralizing these 
 * aliases reduces template verbosity and improves the readability of API signatures across all architectural layers.
 *
 * Selection Guidance:
 * - **TreeMap / TreeSet**: Use when deterministic key ordering or ordered iteration is required.
 * - **MultiMap / MultiSet**: Use when duplicate keys or values must be explicitly permitted.
 * - **HashMap / HashSet**: Use when O(1) average-case lookup is preferred over ordering.
 * - **HashMultiMap / HashMultiSet**: Use for non-unique associations where order is irrelevant.
 *
 * All aliases preserve Standard Library semantics; this header introduces no custom behavior.
 * 
 * @note
 * The associative container types defined in this file are implemented using the corresponding classes from the 
 * C++ Standard Library, and the type aliases provide a convenient way to use these containers with different 
 * types of keys, values, and elements.
 * 
 * @see https://en.cppreference.com/w/cpp/container for more information on C++ Standard Library containers.
 * @see https://en.cppreference.com/w/cpp/container/map for more information on std::map.
 * @see https://en.cppreference.com/w/cpp/container/set for more information on std::set.
 * @see https://en.cppreference.com/w/cpp/container/multimap for more information on std::multimap.
 * @see https://en.cppreference.com/w/cpp/container/multiset for more information on std::multiset.
 * @see https://en.cppreference.com/w/cpp/container/unordered_map for more information on std::unordered_map.
 * @see https://en.cppreference.com/w/cpp/container/unordered_set for more information on std::unordered_set.
 * @see https://en.cppreference.com/w/cpp/container/unordered_multimap for more information on std::unordered_multimap.
 * @see https://en.cppreference.com/w/cpp/container/unordered_multiset for more information on std::unordered_multiset.
 */

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief TreeMap (key-value associative container)
 * @details
 * The TreeMap container is a key-value associative container that stores pairs of keys and values, commonly used 
 * for storing and manipulating collections of data that have a key-value relationship.
 */
template <typename KeyType, typename ValueType, 
          typename Comparator = NEX_STD less<KeyType>, 
          typename Allocator = NEX_STD allocator<NEX_STD pair<const KeyType, ValueType>>>
using TreeMap = NEX_STD map<KeyType, ValueType, Comparator, Allocator>;

/**
 * @brief TreeSet (collection of unique elements)
 * @details
 * The TreeSet container is a collection of unique elements that does not allow duplicate values, commonly used 
 * for storing and manipulating collections of data that require uniqueness.
 */
template <typename KeyType, 
          typename Comparator = NEX_STD less<KeyType>, 
          typename Allocator = NEX_STD allocator<KeyType>>
using TreeSet = NEX_STD set<KeyType, Comparator, Allocator>;

/**
 * @brief MultiMap (key-value associative container that allows duplicate keys)
 * @details
 * The MultiMap container is a container that stores elements formed by a combination of a key value and a mapped value.
 * Unlike TreeMap, multiple elements can have equivalent keys. Elements are typically ordered by key.
 */
template <typename KeyType, typename ValueType, 
          typename Comparator = NEX_STD less<KeyType>, 
          typename Allocator = NEX_STD allocator<NEX_STD pair<const KeyType, ValueType>>>
using MultiMap = NEX_STD multimap<KeyType, ValueType, Comparator, Allocator>;

/**
 * @brief MultiSet (collection of unique elements that allows duplicate values)
 * @details
 * The MultiSet container is a container that stores elements in a specific order, where multiple elements can have 
 * equivalent values.
 */
template <typename KeyType, 
          typename Comparator = NEX_STD less<KeyType>, 
          typename Allocator = NEX_STD allocator<KeyType>>
using MultiSet = NEX_STD multiset<KeyType, Comparator, Allocator>;

/**
 * @brief HashMap (hash table based key-value associative container)
 * @details
 * The HashMap container is an unordered associative container that maps keys to values. It provides average 
 * constant-time complexity for insertions and lookups using hashing.
 */
template <typename KeyType, typename ValueType, 
          typename Hasher = NEX_STD hash<KeyType>, 
          typename KeyEqual = NEX_STD equal_to<KeyType>,
          typename Allocator = NEX_STD allocator<NEX_STD pair<const KeyType, ValueType>>>
using HashMap = NEX_STD unordered_map<KeyType, ValueType, Hasher, KeyEqual, Allocator>;

/**
 * @brief HashSet (hash table based collection of unique elements)
 * @details
 * The HashSet container is an unordered container that stores unique elements. Fast retrieval is achieved 
 * through hashing, and no particular order is guaranteed for the stored elements.
 */
template <typename KeyType, 
          typename Hasher = NEX_STD hash<KeyType>, 
          typename KeyEqual = NEX_STD equal_to<KeyType>, 
          typename Allocator = NEX_STD allocator<KeyType>>
using HashSet = NEX_STD unordered_set<KeyType, Hasher, KeyEqual, Allocator>;

/**
 * @brief HashMultiMap (hash table based key-value associative container that allows duplicate keys)
 * @details
 * The HashMultiMap is an unordered associative container that maps a single key to multiple values. 
 * Unlike a standard Map, it allows for duplicate keys and does not guarantee any specific element ordering.
 */
template <typename KeyType, typename ValueType, 
          typename Hasher = NEX_STD hash<KeyType>, 
          typename KeyEqual = NEX_STD equal_to<KeyType>, 
          typename Allocator = NEX_STD allocator<NEX_STD pair<const KeyType, ValueType>>>
using HashMultiMap = NEX_STD unordered_multimap<KeyType, ValueType, Hasher, KeyEqual, Allocator>;

/**
 * @brief HashMultiSet (hash table based collection of unique elements that allows duplicate values)
 * @details
 * The HashMultiSet container is a container that stores a collection of elements where multiple instances 
 * of the same value are permitted. Unlike MultiSet, elements are not stored in any particular order, but offer 
 * average constant-time complexity for operations.
 */
template <typename KeyType, 
          typename Hasher = NEX_STD hash<KeyType>, 
          typename KeyEqual = NEX_STD equal_to<KeyType>, 
          typename Allocator = NEX_STD allocator<KeyType>>
using HashMultiSet = NEX_STD unordered_multiset<KeyType, Hasher, KeyEqual, Allocator>;

NEX_NAMESPACE_END
