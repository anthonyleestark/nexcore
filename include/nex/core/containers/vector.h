/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <vector>

#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Vec (dynamic array)
 * 
 * @details 
 * Vec is a dynamic array that provides fast indexed access and efficient insertion/deletion at the end.
 * Elements are stored contiguously in memory, making it ideal for cases where the size may change over time
 * and random access is required.
 * 
 * @see https://en.cppreference.com/w/cpp/container/vector for more information on std::vector.
 */
template <typename Type, typename Allocator = NEX_STD allocator<Type>>
using Vec = NEX_STD vector<Type, Allocator>;

NEX_NAMESPACE_END