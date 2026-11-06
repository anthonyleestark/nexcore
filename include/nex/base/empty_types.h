/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file   empty_types.h
 * @brief  Defines empty and tag types used for various utility purposes.
 */

#include "nex/base/attributes.h"
#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @struct monostate
 * @brief  Represents a "monostate" or "unit" type with no data.
 * @details
 * This type is used to represent a state with no meaningful data, often used in variant-like structures.
 * It is typically used as a placeholder type in template metaprogramming and variant types.
 */
struct monostate {};

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator==(monostate, monostate) noexcept { return true; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator!=(monostate, monostate) noexcept { return false; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator<(monostate, monostate) noexcept { return false; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator>(monostate, monostate) noexcept { return false; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator<=(monostate, monostate) noexcept { return true; }

NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
bool operator>=(monostate, monostate) noexcept { return true; }

/**
 * @struct in_place_tag
 * @brief  Tag type for in-place construction without additional data.
 * @note
 * In-place construction is typically used with containers and variant-like types to construct elements
 * directly in their storage.
 */
struct in_place_tag {
    explicit in_place_tag() = default;
};
inline constexpr in_place_tag in_place{};

/**
 * @struct in_place_type
 * @brief  Tag type for in-place construction with type information.
 */
template <class Type>
struct in_place_type : in_place_tag {
    explicit in_place_type() = default;
};

/**
 * @struct in_place_index
 * @brief  Tag type for in-place construction with index information.
 */
template <sizetype Index>
struct in_place_index : in_place_tag {
    explicit in_place_index() = default;
};

/**
 * @struct forward_args_tag
 * @brief  Tag type for forwarding arguments.
 * @note
 * This tag type is used to indicate that arguments should be forwarded as-is, typically in template functions.
 * It is commonly used in conjunction with perfect forwarding to preserve the value category of arguments.
 */
struct forward_args_tag {};
inline constexpr forward_args_tag forward_args{};

/**
 * @struct value_init_tag
 * @brief  Tag type for value initialization.
 * @note
 * Value initialization is typically used to initialize objects to their default values,
 * ensuring they are in a well-defined state.
 */
struct value_init_tag {};
inline constexpr value_init_tag value_init{};

/**
 * @struct allocator_arg_tag
 * @brief  Tag type for allocator argument.
 * @note
 * This tag type is used to indicate that an allocator argument is being provided, typically in container construction.
 * It is commonly used in conjunction with custom allocators to pass allocator-specific arguments.
 */
struct allocator_arg_tag {
    explicit allocator_arg_tag() = default;
};
inline constexpr allocator_arg_tag allocator_arg{};

/**
 * @struct unexpect_type
 * @brief  Represents an unexpected value or error state.
 */
struct unexpect_type {};
inline constexpr unexpect_type unexpect{};

NEX_NAMESPACE_END
