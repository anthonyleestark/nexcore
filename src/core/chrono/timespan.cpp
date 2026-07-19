/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/chrono/timespan.h"
#include "src/core/chrono/string_helper.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

/**
 * @brief   Convert TimeSpan to string representation
 * @param   format  Format string. Supported formats:
 *                  - "default" or empty: "D days, H hours, M minutes, S seconds"
 *                  - "compact": "HH:MM:SS" (hours can exceed 24)
 *                  - "compact_ms": "HH:MM:SS.mmm" (with milliseconds)
 *                  - "iso": "PTnHnMnS" (ISO 8601 duration format)
 * @return  Formatted string representation
 */
String TimeSpan::toString(wcstring format) const {
    String result;
    if (!format) format = L"default";
    WString fmt(format);
    
    if (fmt == L"compact") {
        // Format: HH:MM:SS
        int64 totalH = totalHours();
        int32 m = minutes();
        int32 s = seconds();
        result = String::fromInt(totalH);
        if (totalH < 10) result = String(u"0") + result;
        result.append(u":");
        String minStr = String::fromInt(m);
        if (m < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr);
    }
    else if (fmt == L"compact_ms") {
        // Format: HH:MM:SS.mmm
        int64 totalH = totalHours();
        int32 m = minutes();
        int32 s = seconds();
        int32 ms = milliseconds();
        result = String::fromInt(totalH);
        if (totalH < 10) result = String(u"0") + result;
        result.append(u":");
        String minStr = String::fromInt(m);
        if (m < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr).append(u".");
        String msStr = String::fromInt(ms);
        if (ms < 10) result.append(u"00");
        else if (ms < 100) result.append(u"0");
        result.append(msStr);
    }
    else if (fmt == L"iso") {
        // Format: PTnHnMnS (ISO 8601 duration)
        result = String(u"PT");
        int64 totalH = totalHours();
        int32 m = minutes();
        int32 s = seconds();
        if (totalH != 0) {
            result.append(String::fromInt(totalH)).append(u"H");
        }
        if (m != 0) {
            result.append(String::fromInt(m)).append(u"M");
        }
        if (s != 0 || (totalH == 0 && m == 0)) {
            result.append(String::fromInt(s)).append(u"S");
        }
    }
    else {
        // Default format: "D days, H hours, M minutes, S seconds"
        int32 d = days();
        int32 h = hours();
        int32 m = minutes();
        int32 s = seconds();
        
        bool first = true;
        if (d != 0) {
            result.append(String::fromInt(d));
            result.append(d == 1 ? u" day" : u" days");
            first = false;
        }
        if (h != 0) {
            if (!first) result.append(u", ");
            result.append(String::fromInt(h));
            result.append(h == 1 ? u" hour" : u" hours");
            first = false;
        }
        if (m != 0) {
            if (!first) result.append(u", ");
            result.append(String::fromInt(m));
            result.append(m == 1 ? u" minute" : u" minutes");
            first = false;
        }
        if (s != 0 || first) {
            if (!first) result.append(u", ");
            result.append(String::fromInt(s));
            result.append(s == 1 ? u" second" : u" seconds");
        }
    }
    
    return result;
}

/**
 * @brief   Create TimeSpan from string representation
 * @param   str     String to parse. Supports formats:
 *                  - "D days, H hours, M minutes, S seconds"
 *                  - "HH:MM:SS" or "HH:MM:SS.mmm"
 *                  - "PTnHnMnS" (ISO 8601 duration)
 *                  - Integer seconds (e.g., "3600")
 * @return  Parsed TimeSpan, or empty TimeSpan on failure
 */
TimeSpan TimeSpan::fromString(const String& str) {
    String trimmed = string_helper::trim(str);
    if (trimmed.empty()) {
        return TimeSpan();
    }
    
    auto utf16Res = trimmed.toUtf16();
    if (!utf16Res.isOk()) return TimeSpan();
    const Utf16String& u16str = utf16Res.value();
    WString wstr(u16str.begin(), u16str.end());
    
    // Try ISO 8601 format: PTnHnMnS
    if (wstr.length() >= 2 && (wstr[0] == L'P' || wstr[0] == L'p') && 
        (wstr[1] == L'T' || wstr[1] == L't')) {
        try {
            NEX_STD wregex isoRegex(LR"(PT(?:(\d+)H)?(?:(\d+)M)?(?:(\d+)S)?)", NEX_STD regex::icase);
            NEX_STD wsmatch match;
            if (NEX_STD regex_match(wstr, match, isoRegex)) {
                int32 hours = match[1].matched ? NEX_STD stoi(match[1].str()) : 0;
                int32 minutes = match[2].matched ? NEX_STD stoi(match[2].str()) : 0;
                int32 seconds = match[3].matched ? NEX_STD stoi(match[3].str()) : 0;
                return TimeSpan(0, hours, minutes, seconds);
            }
        }
        catch (...) {
            // Fall through to other formats
        }
    }
    
    // Try compact format: HH:MM:SS or HH:MM:SS.mmm
    NEX_STD wregex compactRegex(LR"((\d+):(\d{1,2}):(\d{1,2})(?:\.(\d{1,3}))?)");
    NEX_STD wsmatch match;
    if (NEX_STD regex_match(wstr, match, compactRegex)) {
        try {
            int32 hours = NEX_STD stoi(match[1].str());
            int32 minutes = NEX_STD stoi(match[2].str());
            int32 seconds = NEX_STD stoi(match[3].str());
            int32 millisecs = match[4].matched ? NEX_STD stoi(match[4].str()) : 0;
            // Pad milliseconds to 3 digits if needed
            if (match[4].matched && match[4].str().length() == 1) millisecs *= 100;
            else if (match[4].matched && match[4].str().length() == 2) millisecs *= 10;
            return TimeSpan(0, hours, minutes, seconds, millisecs);
        }
        catch (...) {
            // Fall through
        }
    }
    
    // Try default format: "D days, H hours, M minutes, S seconds"
    try {
        int32 days = 0, hours = 0, minutes = 0, seconds = 0;
        NEX_STD wregex defaultRegex(LR"((\d+)\s*(?:day|days),?\s*(\d+)\s*(?:hour|hours),?\s*(\d+)\s*(?:minute|minutes),?\s*(\d+)\s*(?:second|seconds))", NEX_STD regex::icase);
        NEX_STD wsmatch match;
        if (NEX_STD regex_match(wstr, match, defaultRegex)) {
            days = NEX_STD stoi(match[1].str());
            hours = NEX_STD stoi(match[2].str());
            minutes = NEX_STD stoi(match[3].str());
            seconds = NEX_STD stoi(match[4].str());
            return TimeSpan(days, hours, minutes, seconds);
        }
    }
    catch (...) {
        // Fall through
    }
    
    // Try simple integer (seconds)
    try {
        int64 seconds = NEX_STD stoll(wstr);
        return TimeSpan(0, 0, 0, static_cast<int32>(seconds));
    }
    catch (...) {
        // Return empty TimeSpan on failure
    }
    
    return TimeSpan();
}

NEX_LAYER_NAMESPACE_END(chrono)
