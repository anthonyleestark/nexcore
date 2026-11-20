/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/visibility.h"
#include "nex/base/casts.h"
#include "nex/base/type_traits.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// Internal tag types for compressed pair construction
// =================================================================================

struct NEX_INTERNAL zero_then_va_args_tag {
    explicit zero_then_va_args_tag() = default;
};
struct NEX_INTERNAL one_then_va_args_tag {
    explicit one_then_va_args_tag() = default;
};

// =================================================================================
// Compressed pair implementation details
// =================================================================================

/**
 * @struct CompressedPair
 * @brief A pair of elements with potentially overlapping storage.
 * 
 * @details
 * This structure leverages the [[no_unique_address]] attribute to potentially overlap the storage
 * of its elements, reducing memory usage when one or both elements are empty types.
 * 
 * @tparam First The type of the first element.
 * @tparam Second The type of the second element.
 */
template <class First, class Second>
struct NEX_HIDDEN_FROM_ABI CompressedPair {
    using first_type = First;
    using second_type = Second;

    NEX_NO_UNIQUE_ADDRESS First first;        // The first element of the compressed pair
    NEX_NO_UNIQUE_ADDRESS Second second;      // The second element of the compressed pair

    // Default constructor
    NEX_HIDDEN_FROM_ABI constexpr CompressedPair()
        noexcept(
            IsNothrowDefaultConstructibleV<first_type> &&
            IsNothrowDefaultConstructibleV<second_type>
        ) requires (
            IsDefaultConstructibleV<first_type> &&
            IsDefaultConstructibleV<second_type>
        ) : first(), second() {}

    // Constructs the first element from nothing and forwards the remaining arguments to the second element
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit CompressedPair(zero_then_va_args_tag, Args&&... args)
        noexcept(
            IsNothrowDefaultConstructibleV<first_type> &&
            IsNothrowConstructibleV<second_type, Args...>
        ) requires (
            IsDefaultConstructibleV<first_type> &&
            IsConstructibleV<second_type, Args...>
        ) : first(), second(NEX_FORWARD<Args>(args)...) {}

    // Constructs the first element from the first argument and forwards the remaining arguments to the second element
    template <class Arg1, class... Args2>
    NEX_HIDDEN_FROM_ABI constexpr CompressedPair(one_then_va_args_tag, Arg1&& arg1, Args2&&... args2)
        noexcept(
            IsNothrowConstructibleV<first_type, Arg1> &&
            IsNothrowConstructibleV<second_type, Args2...>
        ) requires (
            IsConstructibleV<first_type, Arg1> &&
            IsConstructibleV<second_type, Args2...>
        ) : first(NEX_FORWARD<Arg1>(arg1)), second(NEX_FORWARD<Args2>(args2)...) {}

    // Constructs the first and second elements from the given arguments
    template <class Type1 = first_type, class Type2 = second_type>
    NEX_HIDDEN_FROM_ABI constexpr CompressedPair(Type1&& arg1, Type2&& arg2)
        noexcept(
            IsNothrowConstructibleV<first_type, Type1> &&
            IsNothrowConstructibleV<second_type, Type2>
        ) requires (
            IsConstructibleV<first_type, Type1> &&
            IsConstructibleV<second_type, Type2>
        ) : first(NEX_FORWARD<Type1>(arg1)), second(NEX_FORWARD<Type2>(arg2)) {}
};

NEX_NAMESPACE_END
