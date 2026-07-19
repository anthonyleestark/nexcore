/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <deque>

#include "nex/base/namespace.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Deque (double-ended queue)
 * 
 * @details 
 * Deque is an indexed sequence container that allows fast insertion and deletion at both its beginning and its end.
 * Unlike Vec, storage is not guaranteed to be contiguous, but it provides efficient access to both ends of the sequence,
 * making it suitable for use cases like queues or stacks where elements are frequently added and removed from both ends.
 * 
 * @see https://en.cppreference.com/w/cpp/container/deque for more information on std::deque.
 */
template <typename Type, typename Allocator = NEX_STD allocator<Type>>
using Deque = NEX_STD deque<Type, Allocator>;

NEX_NAMESPACE_END