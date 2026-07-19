/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/chrono/timestamp.h"
#include "src/core/chrono/string_helper.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

/**
 * @brief   Convert Timestamp to a DateTime string representation.
 * @param   format  A DateTime format: default, with_ms, iso, iso_ms, us, or us_12h.
 * @return  The formatted Timestamp.
 */
String Timestamp::toString(wcstring format) const {
    return getDateTime().toString(format);
}

/**
 * @brief   Create a Timestamp from a DateTime string representation.
 * @param   str  A string accepted by DateTime::fromString.
 * @return  The parsed Timestamp, or the default Timestamp when parsing fails.
 */
Timestamp Timestamp::fromString(const String& str) {
    return Timestamp(DateTime::fromString(str));
}

NEX_LAYER_NAMESPACE_END(chrono)
