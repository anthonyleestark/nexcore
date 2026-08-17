/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/encoding.h"
#include "nex/core/text/string.h"
#include "nex/core/text/string_view.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(parser_helper)

/**
 * @note
 * Libraries like nlohmann::json and pugixml use std::string (which is UTF-8 encoded) for keys and string values.
 * In NexCore, we use String (which is UTF-16 encoded internally) and NString is the closest equivalent to std::string.
 * Thus, we need to convert between String and NString when interacting with these libraries.
 */

/**
 * @brief Converts NexCore's StringView to UTF-8 string with narrow character type (nchar).
 * @param value The StringView to convert.
 * @return The converted NString (UTF-8 encoded).
 * @note This function is useful when passing NexCore's StringView to libraries that expect std::string
 * or similar UTF-8 encoded strings.
 */
NEX_INTERNAL NString toUtf8Narrow(StringView value) {
    return value.asNarrowView();
}

/**
 * @brief Converts UTF-8 string with narrow character type (nchar) to NexCore's String type.
 * @param value The NStringView (UTF-8 encoded) to convert.
 * @return The converted String (UTF-16 encoded).
 * @note This function is useful when receiving UTF-8 encoded strings from libraries and converting them
 * to NexCore's String for internal use.
 */
NEX_INTERNAL String fromUtf8Narrow(NStringView value) noexcept {
    return String::fromNarrow(value);
}

NEX_SUBNAMESPACE_END(parser_helper)

NEX_NAMESPACE_END
