/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file   init.h
 * @brief  Provides a wrapper for initializer lists, allowing for uniform handling of list initialization.
 *
 * @details
 * This header makes it easy for APIs to accept brace-initialized lists. It introduces the `InitList<T>` alias 
 * in the NexCore namespace and, when the standard library is unavailable, provides a compact, efficient replacement 
 * for `std::initializer_list`.
 *
 * The wrapper is header-only and allocation-free, and exposes `size()`, `begin()`, and `end()` so it works naturally 
 * with range-based for loops and common initializer-list idioms. When the standard library is present this header 
 * simply forwards to `<initializer_list>`.
 *
 * Usage:
 * - Prefer `InitList<T>` in function and constructor signatures to accept brace-initializers.
 * - Works with range-based for loops via the provided `begin()`/`end()` overloads.
 *
 * @note
 * The implementation is intentionally minimal, header-only, and allocation-free for efficiency.
 * It's designed to be used in both STL and no-STL builds, providing a consistent interface for
 * initializer lists across the Nex-ecosystem.
 */

#include "nex/base/compiler.h"
#include "nex/base/build.h"
#include "nex/base/namespace.h"
#include "nex/base/types.h"

#if defined(NEX_NO_STL)

NEX_STD_BEGIN

/**
 * @class initializer_list
 * @brief A lightweight wrapper for a list of elements that can be used to initialize containers or other objects.
 * 
 * @details
 * This class provides a convenient way to pass a list of values to functions or constructors that accept
 * initializer lists. It is particularly useful for APIs that want to support flexible argument counts without
 * requiring variadic templates. The initializer_list class is designed to be lightweight and efficient, allowing
 * for easy construction and iteration over the elements in the list.
 * 
 * @tparam ElementType The type of elements in the initializer list.
 * 
 * @note This class is intended to be used in conjunction with the NEX_STD namespace and is compatible with
 * standard containers and algorithms that accept initializer lists.
 */
template <typename ElementType>
class NEX_HIDDEN_FROM_ABI initializer_list {
public:
    using value_type = ElementType;
    using size_type = usize;
    using reference = const value_type&;
    using const_reference = const value_type&;
    using iterator = const value_type*;
    using const_iterator = const value_type*;

private:
    // Private constructor used by the compiler to create initializer_list objects
    NEX_HIDDEN_FROM_ABI constexpr initializer_list(const value_type* begin, size_type size) noexcept
        : begin_(begin), size_(size) {}

public:
    // Default constructor for an empty initializer_list
    NEX_HIDDEN_FROM_ABI constexpr initializer_list() noexcept 
        : begin_(nullptr), size_(0) {}

    // Return the number of elements in the initializer_list
    NEX_HIDDEN_FROM_ABI constexpr size_type size() const noexcept { return size_; }

    // Return a pointer to the first element in the initializer_list
    NEX_HIDDEN_FROM_ABI constexpr const_iterator begin() const noexcept { return begin_; }

    // Return a pointer to one past the last element in the initializer_list
    NEX_HIDDEN_FROM_ABI constexpr const_iterator end() const noexcept { return begin_ + size_; }

private:
    const value_type* begin_;
    size_type size_;
};

NEX_STD_END

#else
    #include <initializer_list>
#endif

NEX_NAMESPACE_BEGIN

/**
 * @brief A list of elements that can be used to initialize containers or other objects.
 */
template <typename ElementType>
using InitList = NEX_STD initializer_list<ElementType>;

// Overload of begin() for InitList
template <class ElementType>
inline NEX_HIDDEN_FROM_ABI constexpr 
auto begin(InitList<ElementType> ilist) noexcept -> decltype(ilist.begin()) {
    return ilist.begin();
}

// Overload of end() for InitList
template <class ElementType>
inline NEX_HIDDEN_FROM_ABI constexpr 
auto end(InitList<ElementType> ilist) noexcept -> decltype(ilist.end()) {
    return ilist.end();
}

NEX_NAMESPACE_END
