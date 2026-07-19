/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <sstream>
#include <iomanip>
#include <regex>
#include <algorithm>
#include <cwctype>

#include "nex/base/string.h"
#include "nex/core/text/string.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

NEX_SUBNAMESPACE_BEGIN(string_helper)

// Trim whitespace
String trim(const String& str) {
    if (str.empty()) return String();
    
    const auto res = str.toUtf16();
    if (!res.isOk()) return String();

    Utf16String u16str = res.value();
    size_t first = u16str.find_first_not_of(u" \t\n\r");
    if (first == Utf16String::npos) return String();

    size_t last = u16str.find_last_not_of(u" \t\n\r");
    return String(u16str.substr(first, (last - first + 1)));
}

// Convert to lowercase
String toLower(const String& str) {
    const auto res = str.toUtf16();
    if (!res.isOk()) return String();

    Utf16String u16str = res.value();
    NEX_STD transform(u16str.begin(), u16str.end(), u16str.begin(),
        [](char16_t c) { return NEX_STD towlower(c); });
    return String(u16str);
}

NEX_SUBNAMESPACE_END(string_helper)

NEX_LAYER_NAMESPACE_END(chrono)
