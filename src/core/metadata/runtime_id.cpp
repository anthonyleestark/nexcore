/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/text/string.h"
#include "nex/core/metadata/runtime_id.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Convert the RuntimeId to a string representation.
 * @return A String representing the RuntimeId value.
 * @note The string representation is in decimal format.
 */
String RuntimeId::toString() const {
    return String::fromUInt(value_);
}

NEX_NAMESPACE_END
