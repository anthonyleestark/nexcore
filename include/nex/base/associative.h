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
 * @brief TreeMap (key-value associative container) type aliases
 * @details
 * The TreeMap container is a key-value associative container that stores pairs of keys and values, commonly used 
 * for storing and manipulating collections of data that have a key-value relationship.
 */
template <typename Key, typename Value>
using TreeMap = NEX_STD map<Key, Value>;

/**
 * @brief TreeSet (collection of unique elements) type aliases
 * @details
 * The TreeSet container is a collection of unique elements that does not allow duplicate values, commonly used 
 * for storing and manipulating collections of data that require uniqueness.
 */
template <typename T>
using TreeSet = NEX_STD set<T>;

/**
 * @brief MultiMap (key-value associative container that allows duplicate keys) type aliases
 * @details
 * The MultiMap container is a container that stores elements formed by a combination of a key value and a mapped value.
 * Unlike TreeMap, multiple elements can have equivalent keys. Elements are typically ordered by key.
 */
template <typename Key, typename Value>
using MultiMap = NEX_STD multimap<Key, Value>;

/**
 * @brief MultiSet (collection of unique elements that allows duplicate values) type aliases
 * @details
 * The MultiSet container is a container that stores elements in a specific order, where multiple elements can have 
 * equivalent values.
 */
template <typename T>
using MultiSet = NEX_STD multiset<T>;

/**
 * @brief Hash Map (hash table based key-value associative container) type aliases
 * @details
 * The Hash Map container is an unordered associative container that maps keys to values. It provides average 
 * constant-time complexity for insertions and lookups using hashing.
 */
template <typename Key, typename Value, 
    typename Hasher = NEX_STD hash<Key>, 
    typename KeyEqual = NEX_STD equal_to<Key>,
    typename Allocator = NEX_STD allocator<NEX_STD pair<const Key, Value>>>
using HashMap = NEX_STD unordered_map<Key, Value, Hasher, KeyEqual, Allocator>;

/**
 * @brief Hash Set (hash table based collection of unique elements) type aliases
 * @details
 * The Hash Set container is an unordered container that stores unique elements. Fast retrieval is achieved 
 * through hashing, and no particular order is guaranteed for the stored elements.
 */
template <typename T, 
          typename Hasher = NEX_STD hash<T>, 
          typename KeyEqual = NEX_STD equal_to<T>, 
          typename Allocator = NEX_STD allocator<T>>
using HashSet = NEX_STD unordered_set<T, Hasher, KeyEqual, Allocator>;

/**
 * @brief Hash MultiMap (hash table based key-value associative container that allows duplicate keys) type aliases
 * @details
 * The Hash MultiMap is an unordered associative container that maps a single key to multiple values. 
 * Unlike a standard Map, it allows for duplicate keys and does not guarantee any specific element ordering.
 */
template <typename Key, typename Value, 
          typename Hasher = NEX_STD hash<Key>, 
          typename KeyEqual = NEX_STD equal_to<Key>, 
          typename Allocator = NEX_STD allocator<NEX_STD pair<const Key, Value>>>
using HashMultiMap = NEX_STD unordered_multimap<Key, Value, Hasher, KeyEqual, Allocator>;

/**
 * @brief Hash MultiSet (hash table based collection of unique elements that allows duplicate values) type aliases
 * @details
 * The Hash MultiSet container is a container that stores a collection of elements where multiple instances 
 * of the same value are permitted. Unlike MultiSet, elements are not stored in any particular order, but offer 
 * average constant-time complexity for operations.
 */
template <typename T, 
          typename Hasher = NEX_STD hash<T>, 
          typename KeyEqual = NEX_STD equal_to<T>, 
          typename Allocator = NEX_STD allocator<T>>
using HashMultiSet = NEX_STD unordered_multiset<T, Hasher, KeyEqual, Allocator>;

NEX_NAMESPACE_END
