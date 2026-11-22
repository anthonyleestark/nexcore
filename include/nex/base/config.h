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
 * @see platform.h for target platform detection and configuration aspects
 * @see architecture.h for CPU architecture detection and configuration aspects
 * @see compiler.h for compiler detection and configuration aspects
 * @see abi.h for ABI detection and configuration aspects
 * @see language.h for language standard detection and configuration aspects
 * @see runtime.h for C runtime library detection and configuration aspects
 * @see compiler_features.h for compiler feature detection
 * @see libc.h for C library detection and configuration aspects
 * @see build.h for build configuration aspects
 */

#include "nex/base/platform.h"
#include "nex/base/architecture.h"
#include "nex/base/compiler.h"
#include "nex/base/abi.h"
#include "nex/base/language.h"
#include "nex/base/runtime.h"
#include "nex/base/compiler_features.h"
#include "nex/base/libc.h"
#include "nex/base/build.h"

// ^^end of NexCore central configuration interface
