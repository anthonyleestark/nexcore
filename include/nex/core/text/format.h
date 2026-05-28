/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/core/text/string.h"
#include "nex/core/text/string_view.h"
#include "nex/base/result.h"

#include <fmt/format.h>

NEX_NAMESPACE_BEGIN

/**
 * @namespace format
 * @brief Provides string formatting utilities using the fmt library
 * 
 * The format namespace contains functions for formatting strings using the fmt library.
 * It provides a convenient interface for formatting strings with various types of arguments,
 * while handling UTF-16 to UTF-8 conversions as needed.
 */
NEX_SUBNAMESPACE_BEGIN(format)

/**
 * @brief Format a string
 * 
 * @details
 * This function takes a format string and a variable number of arguments, formats the string using 
 * the fmt library, and returns the formatted string. The format string is expected to be a UTF-16 
 * string (StringView), which will be converted to UTF-8 for formatting.
 * The formatted result will be converted back to UTF-16 and returned as a String.
 * 
 * @param   fmt The format string (internally UTF-16 encoded)
 * @tparam  Args Variadic template parameters for the arguments to be formatted
 * 
 * @return  The formatted string as a String (UTF-16 encoded)
 */
template <typename... Args>
String format(StringView fmt, Args&&... args) {
    // Convert StringView to UTF-8 string for formatting
    const auto utf8 = fmt.toString().toUtf8();
    if (utf8.isError()) {
        // If conversion fails, return an empty string or handle as needed
        return String();
    }

    // Use fmt library to format the string with the provided arguments
    try {
        const auto formattedUtf8 = fmt::format(utf8.value(), NEX_FORWARD<Args>(args)...);
        // Convert the formatted UTF-8 string back to our String type (UTF-16)
        const auto result = String::fromUtf8(formattedUtf8);
        return result.isOk() ? result.value() : String();
    }
    catch (const fmt::format_error& e) {
        // Handle formatting errors (e.g., invalid format string or arguments)
        // For now, we can return an empty string or log the error as needed
        return String();
    }
}

/**
 * @brief Format a string with UTF-8 format string
 * 
 * @details
 * This function takes a UTF-8 format string and a variable number of arguments, formats the string using 
 * the fmt library, and returns the formatted string as a UTF-16 encoded String. The format string is 
 * expected to be UTF-8 encoded, which is directly compatible with the fmt library.
 * This overload exists because for now there's is no direct way to implicitly convert a UTF-8 string literal 
 * to our StringView type without first converting it to UTF-16, which would be inefficient. In the future, 
 * we may want to add a user-defined literal for UTF-16 string literals to make this more seamless.
 * 
 * @param   utf8Fmt The format string (UTF-8 encoded)
 * @tparam  Args Variadic template parameters for the arguments to be formatted
 * 
 * @return  The formatted string as a String (UTF-16 encoded)
 */
template <typename... Args>
String format(const char* utf8Fmt, Args&&... args) {
    try {
        const auto formattedUtf8 = fmt::format(utf8Fmt, NEX_FORWARD<Args>(args)...);
        // Convert the formatted UTF-8 string back to our String type (UTF-16)
        const auto result = String::fromUtf8(formattedUtf8);
        return result.isOk() ? result.value() : String();
    }
    catch (const fmt::format_error& e) {
        // Handle formatting errors (e.g., invalid format string or arguments)
        // For now, we can return an empty string or log the error as needed
        return String();
    }
}

NEX_SUBNAMESPACE_END(format)

NEX_NAMESPACE_END
