/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/core/text/string.h"
#include "nex/core/chrono/chrono_base.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

/**
 * @class   DayOfWeek
 * @brief   Day of the week enumeration
 * 
 * Represents the seven days of the week in a strongly-typed enumeration.
 * The enumeration values correspond to the standard weekday encoding where
 * Sunday is 0 and Saturday is 6, matching the ISO 8601 standard for
 * weekday representation.
 * 
 * @details
 * The enumeration provides type-safe day-of-week values that can be used
 * with DateTime and other time-related operations. The underlying type is
 * uint8, ensuring compact storage while providing clear semantic meaning.
 * 
 * @note Sunday = 0, Monday = 1, Tuesday = 2, Wednesday = 3, Thursday = 4,
 *       Friday = 5, Saturday = 6
 * @see DateTime
 */
enum class DayOfWeek : uint8 { 
    Sunday, Monday, Tuesday, Wednesday, 
    Thursday, Friday, Saturday 
};

NEX_LAYER_NAMESPACE_END(chrono)
