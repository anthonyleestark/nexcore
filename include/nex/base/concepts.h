/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <concepts>
#include <iterator>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/traits.h"

NEX_NAMESPACE_BEGIN

/**
 * @typedef  DecayCopyable
 * @brief    A concept that checks if a type is both copy constructible and move constructible.
 * 
 * @details
 * This concept is used to ensure that a type can be safely copied and moved, which is important
 * for types that will be stored in certain containers or passed by value. It requires that the type
 * can be copy constructed and move constructed, which means it can be duplicated and transferred
 * without issues.
 */
template <typename T>
concept DecayCopyable = 
    NEX_STD is_copy_constructible_v<T> && 
    NEX_STD is_move_constructible_v<T>;

/**
 * @typedef  RegularValue
 * @brief    A concept that checks if a type is a regular value type, meaning it is copyable, 
 *           equality comparable, and default initializable.
 * 
 * @details
 * This concept is used to constrain templates to only accept types that behave like regular values,
 * which means they can be copied, compared for equality, and default constructed. This is useful for
 * types that are intended to be used as values in containers or passed by value in functions.
 */
template <typename T>
concept RegularValue = 
    NEX_STD copyable<T> && 
    NEX_STD equality_comparable<T> && 
    NEX_STD default_initializable<T>;

/**
 * @typedef  CallableReturns
 * @brief    A concept that checks if a callable type F can be invoked with arguments Args... 
 *           and returns a type R.
 * 
 * @details
 * This concept is used to constrain templates to only accept callables that return a specific type.
 * It ensures that the callable can be invoked with the given arguments and that the result of the 
 * invocation is exactly the specified return type R.
 */
template <typename F, typename R, typename... Args>
concept CallableReturns = 
    NEX_STD invocable<F, Args...> && 
    NEX_STD same_as<InvokeResult<F, Args...>, R>;

/**
 * @typedef  Predicate
 * @brief    A concept that checks if a callable type F can be invoked with arguments Args... 
 *           and returns a type that is convertible to bool.
 * 
 * @details
 * This concept is used to constrain templates to only accept callables that can be used as predicates,
 * meaning they can be invoked with the given arguments and their return type can be converted to bool.
 * This is useful for functions that take predicates, such as algorithms that filter or search based 
 * on a condition.
 */
template <typename F, typename... Args>
concept Predicate = 
    NEX_STD invocable<F, Args...> && 
    NEX_STD convertible_to<InvokeResult<F, Args...>, bool>;

/**
 * @typedef  Invocable
 * @brief    A concept that checks if a type F can be invoked with arguments Args...
 * 
 * @details
 * This concept is used to constrain templates to only accept types that can be invoked as callables
 * with the specified arguments. It ensures that the type can be called with the given arguments,
 * regardless of the return type. This is a more general concept than CallableReturns or Predicate,
 * as it does not place any requirements on the return type of the invocation.
 */
template <typename F, typename... Args>
concept Invocable = requires(F&& f, Args&&... args) {
    typename InvokeResult<F, Args...>;
};

/**
 * @typedef  Iterable
 * @brief    A concept that checks if a type T is iterable, meaning it has begin and end functions 
 *           that return iterators.
 * 
 * @details
 * This concept is used to constrain templates to only accept types that can be iterated over using
 * range-based for loops or standard algorithms. It requires that the type has a begin function that
 * returns an input or output iterator, and an end function that returns a sentinel for that iterator.
 */
template <typename T>
concept Iterable = requires(T& t) {
    { NEX_STD begin(t) } -> NEX_STD input_or_output_iterator;
    { NEX_STD end(t) } -> NEX_STD sentinel_for<decltype(NEX_STD begin(t))>;
};

/**
 * @typedef  Indexable
 * @brief    A concept that checks if a type T is indexable, meaning it is iterable and supports 
 *           the subscript operator.
 * 
 * @details
 * This concept is used to constrain templates to only accept types that can be indexed using 
 * the subscript operator. It requires that the type is iterable and that the subscript operator 
 * is valid for the type. This is useful for types like arrays, vectors, and other container types 
 * that provide random access to their elements.
 */
template <typename T>
concept Indexable = Iterable<T> && requires(T& t) {
    t[0];
};

/**
 * @typedef  PointerLike
 * @brief    A concept that checks if a type T behaves like a pointer, meaning it supports dereferencing 
 *           and the arrow operator.
 * 
 * @details
 * This concept is used to constrain templates to only accept types that can be treated like pointers.
 * It requires that the type supports dereferencing with the unary operator* and that it has an 
 * operator-> that returns a pointer to the element type. This is useful for smart pointers, iterators, 
 * and other types that provide pointer-like semantics.
 */
template <typename T>
concept PointerLike = requires(T t) {
    { *t } -> NEX_STD same_as<typename NEX_STD remove_reference_t<T>::element_type&>;
    { t.operator->() } -> NEX_STD same_as<typename NEX_STD remove_reference_t<T>::element_type*>;
};

NEX_NAMESPACE_END