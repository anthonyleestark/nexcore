/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <array>

#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Array (fixed-size array)
 * 
 * @details 
 * Array is a wrapper around a stack-allocated (usually) fixed-size array.
 * Use this when the number of elements is known at compile-time to avoid dynamic allocation.
 * 
 * @see https://en.cppreference.com/w/cpp/container/array for more information on std::array.
 */
template <typename Type, usize Size>
using Array = NEX_STD array<Type, Size>;

NEX_NAMESPACE_END