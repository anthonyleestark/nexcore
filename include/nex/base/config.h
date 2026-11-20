/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  config.h
 * @brief Provides the central configuration interface for NexCore.
 *
 * @details
 * This header serves as the umbrella header for NexCore configuration and compile-time environment information.
 *
 * It brings together the relevant configuration headers responsible for detecting the target platform,
 * CPU architecture, data model, compiler, language standard, ABI, and build configuration.
 *
 * It also provides access to the common annotation and configuration macros used throughout NexCore.
 *
 * @note
 * Individual configuration headers may be included directly when only a specific set of configuration facilities
 * is required.
 *
 * @see platform.h
 * @see architecture.h
 * @see compiler.h
 * @see abi.h
 * @see language.h
 * @see compiler_features.h
 * @see libc.h
 * @see build.h
 */

#include "nex/base/platform.h"
#include "nex/base/architecture.h"
#include "nex/base/compiler.h"
#include "nex/base/abi.h"
#include "nex/base/language.h"
#include "nex/base/compiler_features.h"
#include "nex/base/libc.h"
#include "nex/base/build.h"

