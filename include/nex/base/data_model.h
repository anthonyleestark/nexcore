/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  data_model.h
 * @brief Detects the target data model and fundamental type widths.
 *
 * @details
 * This header provides compile-time information about the target data model, including pointer width
 * and the relationship between the sizes of fundamental integer and pointer types.
 *
 * It exposes a consistent set of macros describing the target data model for use in platform-independent
 * code and conditional compilation.
 *
 * @note
 * The data model is not necessarily determined by the CPU architecture.
 * Different data models may be supported by the same processor architecture.
 */
