/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <type_traits>

#include "nex/base/macros.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @typedef InvokeResult
 * @brief A type alias for the result type of invoking a callable with specific arguments.
 */
template<typename Fn, typename... Args>
using InvokeResult = NEX_STD invoke_result_t<Fn, Args...>;

NEX_NAMESPACE_END