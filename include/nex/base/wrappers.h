/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  wrappers.h
 * @brief Defines common wrapper types used throughout the codebase, such as Optional, Smart Pointers, 
 *        Variant, InitList, Function, LockGuard and their associated type aliases.
 * 
 * @details
 * This header centralizes aliases for wrapper types used by Nex-ecosystem, including optional values, smart pointers,
 * variant types, initializer lists, type information, function wrappers, and mutex types. It establishes a uniform 
 * naming convention for wrapper-centric structures so interfaces can communicate behavior clearly and consistently.
 * Wrapper types are foundational in representing optional values, managing object lifetimes, encapsulating variant
 * types, managing type information, and synchronizing access to shared resources. Grouping these aliases in one 
 * place reduces repetitive template spelling and helps preserve readable API signatures across infrastructure, 
 * platform, and service layers.
 * 
 * @note
 * The wrapper types defined in this file are implemented using the corresponding classes from the C++ Standard Library,
 * and the type aliases provide a convenient way to use these wrapper types with different types of values, objects,
 * and functions.
 * 
 * @see https://en.cppreference.com/w/cpp/utility/optional for more information on std::optional.
 * @see https://en.cppreference.com/w/cpp/memory/shared_ptr for more information on std::shared_ptr.
 * @see https://en.cppreference.com/w/cpp/memory/unique_ptr for more information on std::unique_ptr.
 * @see https://en.cppreference.com/w/cpp/memory/weak_ptr for more information on std::weak_ptr.
 * @see https://en.cppreference.com/w/cpp/utility/pair for more information on std::pair.
 * @see https://en.cppreference.com/w/cpp/utility/tuple for more information on std::tuple.
 * @see https://en.cppreference.com/w/cpp/utility/any for more information on std::any.
 * @see https://en.cppreference.com/w/cpp/utility/variant for more information on std::variant.
 * @see https://en.cppreference.com/w/cpp/utility/expected for more information on std::expected.
 * @see https://en.cppreference.com/w/cpp/utility/initializer_list for more information on std::initializer_list.
 * @see https://en.cppreference.com/w/cpp/types/type_index for more information on std::type_index.
 * @see https://en.cppreference.com/w/cpp/types/type_info for more information on std::type_info.
 * @see https://en.cppreference.com/w/cpp/utility/functional for more information on std::function.
 * @see https://en.cppreference.com/w/cpp/utility/functional/reference_wrapper for more information on std::reference_wrapper.
 * @see https://en.cppreference.com/w/cpp/thread/lock_guard for more information on std::lock_guard.
 * @see https://en.cppreference.com/w/cpp/thread/unique_lock for more information on std::unique_lock.
 * @see https://en.cppreference.com/w/cpp/thread/shared_lock for more information on std::shared_lock.
 * @see https://en.cppreference.com/w/cpp/thread/scoped_lock for more information on std::scoped_lock.
 */

#include <optional>
#include <memory>
#include <utility>
#include <any>
#include <variant>
#include <initializer_list>
#include <typeindex>
#include <typeinfo>
#include <functional>
#include <shared_mutex>

#if NEX_HAS_CXX23
    #include <expected>
#endif // NEX_HAS_CXX23

#include "nex/base/namespace.h"
#include "nex/base/types.h"
#include "nex/base/string.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief A type-safe wrapper for values that may or may not exist.
 * @details 
 * Efficiently represents an optional value without dynamic allocation. 
 * Use this to avoid "magic values" (like null or -1) in API return types.
 */
template <typename T>
using Optional = NEX_STD optional<T>;

/**
 * @brief Shared ownership smart pointer.
 * @details 
 * Uses reference counting to manage an object's lifetime. The object is destroyed only 
 * when the last SharedPtr is destroyed or reset.
 */
template <typename T>
using SharedPtr = NEX_STD shared_ptr<T>;

/**
 * @brief Exclusive ownership smart pointer.
 * @details 
 * Manages a dynamically allocated object with sole responsibility. 
 * Cannot be copied, only moved. This is the preferred default for resource management.
 */
template <typename T>
using UniquePtr = NEX_STD unique_ptr<T>;

/**
 * @brief Non-owning reference to a SharedPtr-managed object.
 * @details 
 * Holds a weak reference that must be "locked" into a SharedPtr to access the data. 
 * Used to break circular dependencies or observe managed objects.
 */
template <typename T>
using WeakPtr = NEX_STD weak_ptr<T>;

/**
 * @brief Semantic alias for UniquePtr, emphasizing automatic lifecycle management.
 */
template <typename T>
using AutoPtr = NEX_STD unique_ptr<T>;

/**
 * @brief Semantic alias for WeakPtr, emphasizing a non-owning relationship.
 */
template <typename T>
using NonOwningPtr = NEX_STD weak_ptr<T>;

/**
 * @brief A raw pointer alias representing a "borrowed" reference.
 * @details 
 * Indicates that the pointer is used for observation only and the caller is NOT responsible for its deletion.
 * Use this for function parameters or return types where ownership is not transferred, and the pointer is 
 * guaranteed to be valid for the duration of its use.
 */
template <typename T>
using ObserverPtr = T*;

/**
 * @brief A wrapper for a pair of values.
 * @details 
 * Provides a simple structure to hold two related values together. 
 * Commonly used for returning multiple values from a function or representing key-value pairs in maps.
 */
template <typename T, typename U>
using Pair = NEX_STD pair<T, U>;

/**
 * @brief A wrapper for a fixed-size collection of heterogeneous values.
 * @details 
 * Similar to Pair but can hold any number of values of different types. 
 * Use this for grouping related data together without defining a custom struct.
 */
template <typename... Args>
using Tuple = NEX_STD tuple<Args...>;

/**
 * @brief Type-erased container for any CopyConstructible type.
 * @details 
 * A flexible container that can hold a single value of any type at runtime. 
 * Use this for high-level polymorphism or plugin systems where the set of possible types 
 * is not known at compile time.
 */
using Any = NEX_STD any;

/**
 * @brief Type-safe union for a fixed set of alternative types.
 * @details 
 * A modern, safer alternative to 'union'. It ensures that only one of the specified types 
 * is active at a time and provides compile-time type checking for access.
 */
template <typename... Args>
using Variant = NEX_STD variant<Args...>;

/**
 * @brief A wrapper for expected values or errors.
 * @details 
 * Represents a value that may either contain a valid result (T) or an error (E). 
 * Use this for functions that can fail, providing a clear way to handle success and error cases without exceptions.
 * @note This is a C++23 feature, so ensure your compiler supports it or use Result instead.
 * @see Result class in nex/base/result.h for a similar pattern that does not require C++23 support.
 */
#if NEX_HAS_CXX23
    template <typename T, typename E>
    using Expected = NEX_STD expected<T, E>;
#endif // NEX_HAS_CXX23

/**
 * @brief A wrapper for initializer lists, allowing for uniform handling of list initialization.
 * @details 
 * Provides a convenient way to pass a list of values to functions or constructors that accept 
 * initializer lists. This is particularly useful for APIs that want to support flexible argument 
 * counts without requiring variadic templates.
 */
template <typename Ep>
using InitList = NEX_STD initializer_list<Ep>;

/**
 * @brief A wrapper for type_info to allow usage in associative containers.
 * @details 
 * Provides a copyable and comparable representation of a type. 
 * Commonly used as a key in Maps or HashMaps to associate data with specific types.
 */
using TypeIndex = NEX_STD type_index;

/**
 * @brief Metadata representing a type at runtime.
 * @details 
 * Provides access to type names and comparison logic. 
 * Use this primarily through the 'typeid' operator for low-level RTTI operations.
 */
using TypeInfo = NEX_STD type_info;

/**
 * @brief A general-purpose polymorphic function wrapper.
 * @details 
 * Can store, copy, and invoke any callable target—such as functions, lambda expressions, or bind expressions. 
 * Use this for callbacks or event-handling systems where the specific callable type is erased.
 */
template <typename T>
using Function = NEX_STD function<T>;

/**
 * @brief A wrapper that makes references "assignable" and "copyable".
 * @details 
 * Enables storing references in standard containers (like Vec) which otherwise require elements 
 * to be erasable/assignable. It acts as a non-nullable pointer with reference semantics.
 */
template <typename T>
using Reference = NEX_STD reference_wrapper<T>;

/**
 * @brief A wrapper for constant references, ensuring read-only access.
 * @details 
 * Similar to Reference, but holds a 'const T&'. 
 * Useful for storing collections of read-only observers without the overhead of pointers.
 */
template <typename T>
using ConstReference = NEX_STD reference_wrapper<const T>;

/**
 * @brief Strict RAII wrapper for a single mutex.
 * @details 
 * Automatically locks the mutex on construction and unlocks on destruction. 
 * It is non-copyable and has the least overhead among lock wrappers.
 */
template <typename T>
using LockGuard = NEX_STD lock_guard<T>;

/**
 * @brief Flexible RAII wrapper with manual control.
 * @details 
 * Unlike LockGuard, it supports deferred locking, timed attempts, and manual unlocking 
 * before the object goes out of scope. Required for use with Condition Variables.
 */
template <typename T>
using UniqueLock = NEX_STD unique_lock<T>;

/**
 * @brief RAII wrapper for shared ownership (Reading).
 * @details 
 * Used in conjunction with SharedMutex to acquire shared (read) access. 
 * Multiple SharedLocks can coexist for the same SharedMutex.
 */
template <typename T>
using SharedLock = NEX_STD shared_lock<T>;

/**
 * @brief Deadlock-avoiding wrapper for multiple mutexes.
 * @details 
 * Uses a deadlock-avoidance algorithm to lock multiple mutexes simultaneously. 
 * This is the modern replacement for std::lock when managing multiple resources.
 */
template <typename... Args>
using ScopedLock = NEX_STD scoped_lock<Args...>;

/**
 * @brief Common wrapper type aliases
 * @details
 * These type aliases provide convenient names for commonly used wrapper types with specific types of values, 
 * objects, and functions. The type aliases can help to improve code readability and maintainability by providing 
 * consistent type names for commonly used wrapper types with specific types of values, objects, and functions.
 */

/**
 * @brief Optional integer and numeric aliases.
 * @details 
 * Represents values that may or may not be present, effectively replacing nullable pointers 
 * or "magic number" error codes (e.g., -1).
 */
using MaybeInt = Optional<int32>;
using MaybeUInt = Optional<uint32>;

using MaybeInt64 = Optional<int64>;
using MaybeUInt64 = Optional<uint64>;

using MaybeFloat = Optional<float32>;
using MaybeDouble = Optional<float64>;

/**
 * @brief Tri-state boolean representation.
 * @details Useful for configurations where a setting can be true, false, or "not set / default".
 */
using MaybeBool = Optional<bool>;

/**
 * @brief Optional string alias.
 * @details 
 * Represents a string value that may or may not be present, allowing for efficient handling of optional text data 
 * without resorting to empty strings or null pointers.
 */
using MaybeString = Optional<NEX_PREPEND_CORE_NAMESPACE(String)>;

/**
 * @brief Semantic alias for search operations.
 * @details 
 * A value of 'nullopt' explicitly indicates that a search failed to find a match, providing better clarity 
 * than returning an out-of-bounds index.
 */
using SearchResult = Optional<usize>;

NEX_NAMESPACE_END