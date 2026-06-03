/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <sstream>
#include <iomanip>
#include <regex>
#include <algorithm>
#include <cwctype>

#include "nex/core/chrono/time.h"
#include "nex/base/string.h"

NEX_NAMESPACE_BEGIN

// ============================================================================
// Internal helper functions
// ============================================================================

NEX_ANONYMOUS_NAMESPACE_BEGIN

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

NEX_ANONYMOUS_NAMESPACE_END

// ============================================================================
// TimeSpan string conversion
// ============================================================================

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
    StdWString fmt(format);
    
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
    String trimmed = trim(str);
    if (trimmed.empty()) {
        return TimeSpan();
    }
    
    auto utf16Res = trimmed.toUtf16();
    if (!utf16Res.isOk()) return TimeSpan();
    const Utf16String& u16str = utf16Res.value();
    StdWString wstr(u16str.begin(), u16str.end());
    
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

// ============================================================================
// ClockTime string conversion
// ============================================================================

/**
 * @brief   Convert ClockTime to string representation
 * @param   format  Format string. Supported formats:
 *                  - "default" or empty: "HH:MM:SS"
 *                  - "with_ms": "HH:MM:SS.mmm" (with milliseconds)
 *                  - "12h": "HH:MM:SS AM/PM" (12-hour format)
 *                  - "12h_ms": "HH:MM:SS.mmm AM/PM" (12-hour with milliseconds)
 * @return  Formatted string representation
 */
String ClockTime::toString(wcstring format) const {
    String result;
    if (!format) format = L"default";
    StdWString fmt(format);
    int32 h = hour();
    int32 m = minute();
    int32 s = second();
    int32 ms = millisecond();
    
    if (fmt == L"with_ms") {
        // Format: HH:MM:SS.mmm
        String hourStr = String::fromInt(h);
        if (h < 10) result = String(u"0");
        result.append(hourStr).append(u":");
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
    else if (fmt == L"12h") {
        // Format: HH:MM:SS AM/PM
        int32 hour12 = h % 12;
        if (hour12 == 0) hour12 = 12;
        const char16_t* ampm = (h < 12) ? u"AM" : u"PM";
        String hourStr = String::fromInt(hour12);
        if (hour12 < 10) result = String(u"0");
        result.append(hourStr).append(u":");
        String minStr = String::fromInt(m);
        if (m < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr).append(String(u" ")).append(String(ampm));
    }
    else if (fmt == L"12h_ms") {
        // Format: HH:MM:SS.mmm AM/PM
        int32 hour12 = h % 12;
        if (hour12 == 0) hour12 = 12;
        const char16_t* ampm = (h < 12) ? u"AM" : u"PM";
        String hourStr = String::fromInt(hour12);
        if (hour12 < 10) result = String(u"0");
        result.append(hourStr).append(u":");
        String minStr = String::fromInt(m);
        if (m < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr).append(u".");
        String msStr = String::fromInt(ms);
        if (ms < 10) result.append(u"00");
        else if (ms < 100) result.append(u"0");
        result.append(msStr).append(String(u" ")).append(String(ampm));
    }
    else {
        // Default format: HH:MM:SS
        String hourStr = String::fromInt(h);
        if (h < 10) result = String(u"0");
        result.append(hourStr).append(u":");
        String minStr = String::fromInt(m);
        if (m < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr);
    }
    
    return result;
}

/**
 * @brief   Create ClockTime from string representation
 * @param   str     String to parse. Supports formats:
 *                  - "HH:MM:SS" or "HH:MM:SS.mmm"
 *                  - "HH:MM:SS AM/PM" or "HH:MM:SS.mmm AM/PM" (12-hour format)
 *                  - "HHMM" (compact format, assumes 00 seconds)
 * @return  Parsed ClockTime, or empty ClockTime (00:00:00) on failure
 */
ClockTime ClockTime::fromString(const String& str) {
    String trimmed = trim(str);
    if (trimmed.empty()) {
        return ClockTime(0, 0, 0);
    }
    
    auto utf16Res = trimmed.toUtf16();
    if (!utf16Res.isOk()) return ClockTime(0, 0, 0);
    const Utf16String& u16str = utf16Res.value();
    StdWString wstr(u16str.begin(), u16str.end());
    
    // Try compact format: HHMM
    if (wstr.length() == 4 && NEX_STD all_of(wstr.begin(), wstr.end(), ::iswdigit)) {
        try {
            int32 hour = NEX_STD stoi(wstr.substr(0, 2));
            int32 minute = NEX_STD stoi(wstr.substr(2, 2));
            if (time::isValidHour(hour) && time::isValidMinute(minute)) {
                return ClockTime(hour, minute, 0);
            }
        }
        catch (...) {
            // Fall through
        }
    }
    
    // Try 12-hour format: HH:MM:SS AM/PM or HH:MM:SS.mmm AM/PM
    NEX_STD wregex regex12h(LR"((\d{1,2}):(\d{1,2}):(\d{1,2})(?:\.(\d{1,3}))?\s*(AM|PM))", NEX_STD regex::icase);
    NEX_STD wsmatch match;
    if (NEX_STD regex_match(wstr, match, regex12h)) {
        try {
            int32 hour = NEX_STD stoi(match[1].str());
            int32 minute = NEX_STD stoi(match[2].str());
            int32 second = NEX_STD stoi(match[3].str());
            int32 millisec = match[4].matched ? NEX_STD stoi(match[4].str()) : 0;
            // Pad milliseconds
            if (match[4].matched && match[4].str().length() == 1) millisec *= 100;
            else if (match[4].matched && match[4].str().length() == 2) millisec *= 10;
            
            StdWString ampm = match[5].str();
            NEX_STD transform(ampm.begin(), ampm.end(), ampm.begin(), ::towlower);
            if (ampm == L"pm" && hour != 12) hour += 12;
            else if (ampm == L"am" && hour == 12) hour = 0;
            
            if (time::isValidHour(hour) && time::isValidMinute(minute)
                && time::isValidSecond(second) && time::isValidMillisecs(millisec)) {
                return ClockTime(hour, minute, second, millisec);
            }
        }
        catch (...) {
            // Fall through
        }
    }
    
    // Try 24-hour format: HH:MM:SS or HH:MM:SS.mmm
    NEX_STD wregex regex24h(LR"((\d{1,2}):(\d{1,2}):(\d{1,2})(?:\.(\d{1,3}))?)");
    if (NEX_STD regex_match(wstr, match, regex24h)) {
        try {
            int32 hour = NEX_STD stoi(match[1].str());
            int32 minute = NEX_STD stoi(match[2].str());
            int32 second = NEX_STD stoi(match[3].str());
            int32 millisec = match[4].matched ? NEX_STD stoi(match[4].str()) : 0;
            // Pad milliseconds
            if (match[4].matched && match[4].str().length() == 1) millisec *= 100;
            else if (match[4].matched && match[4].str().length() == 2) millisec *= 10;
            
            if (time::isValidHour(hour) && time::isValidMinute(minute)
                && time::isValidSecond(second) && time::isValidMillisecs(millisec)) {
                return ClockTime(hour, minute, second, millisec);
            }
        }
        catch (...) {
            // Fall through
        }
    }
    
    // Return default (00:00:00) on failure
    return ClockTime(0, 0, 0);
}

// ============================================================================
// DateTime string conversion
// ============================================================================

/**
 * @brief   Convert DateTime to string representation
 * @param   format  Format string. Supported formats:
 *                  - "default" or empty: "YYYY-MM-DD HH:MM:SS"
 *                  - "with_ms": "YYYY-MM-DD HH:MM:SS.mmm" (with milliseconds)
 *                  - "iso": "YYYY-MM-DDTHH:MM:SS" (ISO 8601)
 *                  - "iso_ms": "YYYY-MM-DDTHH:MM:SS.mmm" (ISO 8601 with milliseconds)
 *                  - "us": "MM/DD/YYYY HH:MM:SS" (US date format)
 *                  - "us_12h": "MM/DD/YYYY HH:MM:SS AM/PM" (US format with 12-hour time)
 * @return  Formatted string representation
 */
String DateTime::toString(wcstring format) const {
    String result;
    if (!format) format = L"default";
    StdWString fmt(format);
    
    int32 y = year();
    uint32 m = month();
    uint32 d = day();
    int32 h = hour();
    int32 min = minute();
    int32 s = second();
    int32 ms = millisecond();
    
    if (fmt == L"with_ms") {
        // Format: YYYY-MM-DD HH:MM:SS.mmm
        result = String::fromInt(y).append(u"-");
        String monthStr = String::fromUInt(m);
        if (m < 10) result.append(u"0");
        result.append(monthStr).append(u"-");
        String dayStr = String::fromUInt(d);
        if (d < 10) result.append(u"0");
        result.append(dayStr).append(u" ");
        String hourStr = String::fromInt(h);
        if (h < 10) result.append(u"0");
        result.append(hourStr).append(u":");
        String minStr = String::fromInt(min);
        if (min < 10) result.append(u"0");
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
        // Format: YYYY-MM-DDTHH:MM:SS (ISO 8601)
        result = String::fromInt(y).append(u"-");
        String monthStr = String::fromUInt(m);
        if (m < 10) result.append(u"0");
        result.append(monthStr).append(u"-");
        String dayStr = String::fromUInt(d);
        if (d < 10) result.append(u"0");
        result.append(dayStr).append(u"T");
        String hourStr = String::fromInt(h);
        if (h < 10) result.append(u"0");
        result.append(hourStr).append(u":");
        String minStr = String::fromInt(min);
        if (min < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr);
    }
    else if (fmt == L"iso_ms") {
        // Format: YYYY-MM-DDTHH:MM:SS.mmm (ISO 8601 with milliseconds)
        result = String::fromInt(y).append(u"-");
        String monthStr = String::fromUInt(m);
        if (m < 10) result.append(u"0");
        result.append(monthStr).append(u"-");
        String dayStr = String::fromUInt(d);
        if (d < 10) result.append(u"0");
        result.append(dayStr).append(u"T");
        String hourStr = String::fromInt(h);
        if (h < 10) result.append(u"0");
        result.append(hourStr).append(u":");
        String minStr = String::fromInt(min);
        if (min < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr).append(u".");
        String msStr = String::fromInt(ms);
        if (ms < 10) result.append(u"00");
        else if (ms < 100) result.append(u"0");
        result.append(msStr);
    }
    else if (fmt == L"us") {
        // Format: MM/DD/YYYY HH:MM:SS (US date format)
        String monthStr = String::fromUInt(m);
        if (m < 10) result = String(u"0");
        result.append(monthStr).append(u"/");
        String dayStr = String::fromUInt(d);
        if (d < 10) result.append(u"0");
        result.append(dayStr).append(u"/");
        result.append(String::fromInt(y)).append(u" ");
        String hourStr = String::fromInt(h);
        if (h < 10) result.append(u"0");
        result.append(hourStr).append(u":");
        String minStr = String::fromInt(min);
        if (min < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr);
    }
    else if (fmt == L"us_12h") {
        // Format: MM/DD/YYYY HH:MM:SS AM/PM (US format with 12-hour time)
        int32 hour12 = h % 12;
        if (hour12 == 0) hour12 = 12;
        const char16_t* ampm = (h < 12) ? u"AM" : u"PM";
        String monthStr = String::fromUInt(m);
        if (m < 10) result = String(u"0");
        result.append(monthStr).append(u"/");
        String dayStr = String::fromUInt(d);
        if (d < 10) result.append(u"0");
        result.append(dayStr).append(u"/");
        result.append(String::fromInt(y)).append(u" ");
        String hourStr = String::fromInt(hour12);
        if (hour12 < 10) result.append(u"0");
        result.append(hourStr).append(u":");
        String minStr = String::fromInt(min);
        if (min < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr).append(String(u" ")).append(String(ampm));
    }
    else {
        // Default format: YYYY-MM-DD HH:MM:SS
        result = String::fromInt(y).append(u"-");
        String monthStr = String::fromUInt(m);
        if (m < 10) result.append(u"0");
        result.append(monthStr).append(u"-");
        String dayStr = String::fromUInt(d);
        if (d < 10) result.append(u"0");
        result.append(dayStr).append(u" ");
        String hourStr = String::fromInt(h);
        if (h < 10) result.append(u"0");
        result.append(hourStr).append(u":");
        String minStr = String::fromInt(min);
        if (min < 10) result.append(u"0");
        result.append(minStr).append(u":");
        String secStr = String::fromInt(s);
        if (s < 10) result.append(u"0");
        result.append(secStr);
    }
    
    return result;
}

/**
 * @brief   Create DateTime from string representation
 * @param   str     String to parse. Supports formats:
 *                  - "YYYY-MM-DD HH:MM:SS" or "YYYY-MM-DD HH:MM:SS.mmm"
 *                  - "YYYY-MM-DDTHH:MM:SS" or "YYYY-MM-DDTHH:MM:SS.mmm" (ISO 8601)
 *                  - "MM/DD/YYYY HH:MM:SS" or "MM/DD/YYYY HH:MM:SS.mmm" (US format)
 *                  - "MM/DD/YYYY HH:MM:SS AM/PM" (US format with 12-hour time)
 * @return  Parsed DateTime, or empty DateTime on failure
 */
DateTime DateTime::fromString(const String& str) {
    String trimmed = trim(str);
    if (trimmed.empty()) {
        return DateTime();
    }
    
    auto utf16Res = trimmed.toUtf16();
    if (!utf16Res.isOk()) return DateTime();
    const Utf16String& u16str = utf16Res.value();
    StdWString wstr(u16str.begin(), u16str.end());
    
    // Try US format with 12-hour: MM/DD/YYYY HH:MM:SS AM/PM
    NEX_STD wregex regexUS12h(LR"((\d{1,2})/(\d{1,2})/(\d{4})\s+(\d{1,2}):(\d{1,2}):(\d{1,2})(?:\.(\d{1,3}))?\s+(AM|PM))", NEX_STD regex::icase);
    NEX_STD wsmatch match;
    if (NEX_STD regex_match(wstr, match, regexUS12h)) {
        try {
            uint32 month = static_cast<uint32>(NEX_STD stoi(match[1].str()));
            uint32 day = static_cast<uint32>(NEX_STD stoi(match[2].str()));
            int32 year = NEX_STD stoi(match[3].str());
            int32 hour = NEX_STD stoi(match[4].str());
            int32 minute = NEX_STD stoi(match[5].str());
            int32 second = NEX_STD stoi(match[6].str());
            int32 millisec = match[7].matched ? NEX_STD stoi(match[7].str()) : 0;
            // Pad milliseconds
            if (match[7].matched && match[7].str().length() == 1) millisec *= 100;
            else if (match[7].matched && match[7].str().length() == 2) millisec *= 10;
            
            StdWString ampm = match[8].str();
            NEX_STD transform(ampm.begin(), ampm.end(), ampm.begin(), ::towlower);
            if (ampm == L"pm" && hour != 12) hour += 12;
            else if (ampm == L"am" && hour == 12) hour = 0;
            
            if (isValidDate(year, month, day) && isValidTime(hour, minute, second, millisec)) {
                return DateTime(year, month, day, hour, minute, second, millisec);
            }
        }
        catch (...) {
            // Fall through
        }
    }
    
    // Try US format: MM/DD/YYYY HH:MM:SS or MM/DD/YYYY HH:MM:SS.mmm
    NEX_STD wregex regexUS(LR"((\d{1,2})/(\d{1,2})/(\d{4})\s+(\d{1,2}):(\d{1,2}):(\d{1,2})(?:\.(\d{1,3}))?)");
    if (NEX_STD regex_match(wstr, match, regexUS)) {
        try {
            uint32 month = static_cast<uint32>(NEX_STD stoi(match[1].str()));
            uint32 day = static_cast<uint32>(NEX_STD stoi(match[2].str()));
            int32 year = NEX_STD stoi(match[3].str());
            int32 hour = NEX_STD stoi(match[4].str());
            int32 minute = NEX_STD stoi(match[5].str());
            int32 second = NEX_STD stoi(match[6].str());
            int32 millisec = match[7].matched ? NEX_STD stoi(match[7].str()) : 0;
            // Pad milliseconds
            if (match[7].matched && match[7].str().length() == 1) millisec *= 100;
            else if (match[7].matched && match[7].str().length() == 2) millisec *= 10;
            
            if (isValidDate(year, month, day) && isValidTime(hour, minute, second, millisec)) {
                return DateTime(year, month, day, hour, minute, second, millisec);
            }
        }
        catch (...) {
            // Fall through
        }
    }
    
    // Try ISO 8601 format: YYYY-MM-DDTHH:MM:SS or YYYY-MM-DDTHH:MM:SS.mmm
    NEX_STD wregex regexISO(LR"((\d{4})-(\d{1,2})-(\d{1,2})T(\d{1,2}):(\d{1,2}):(\d{1,2})(?:\.(\d{1,3}))?)");
    if (NEX_STD regex_match(wstr, match, regexISO)) {
        try {
            int32 year = NEX_STD stoi(match[1].str());
            uint32 month = static_cast<uint32>(NEX_STD stoi(match[2].str()));
            uint32 day = static_cast<uint32>(NEX_STD stoi(match[3].str()));
            int32 hour = NEX_STD stoi(match[4].str());
            int32 minute = NEX_STD stoi(match[5].str());
            int32 second = NEX_STD stoi(match[6].str());
            int32 millisec = match[7].matched ? NEX_STD stoi(match[7].str()) : 0;
            // Pad milliseconds
            if (match[7].matched && match[7].str().length() == 1) millisec *= 100;
            else if (match[7].matched && match[7].str().length() == 2) millisec *= 10;
            
            if (isValidDate(year, month, day) && isValidTime(hour, minute, second, millisec)) {
                return DateTime(year, month, day, hour, minute, second, millisec);
            }
        }
        catch (...) {
            // Fall through
        }
    }
    
    // Try default format: YYYY-MM-DD HH:MM:SS or YYYY-MM-DD HH:MM:SS.mmm
    NEX_STD wregex regexDefault(LR"((\d{4})-(\d{1,2})-(\d{1,2})\s+(\d{1,2}):(\d{1,2}):(\d{1,2})(?:\.(\d{1,3}))?)");
    if (NEX_STD regex_match(wstr, match, regexDefault)) {
        try {
            int32 year = NEX_STD stoi(match[1].str());
            uint32 month = static_cast<uint32>(NEX_STD stoi(match[2].str()));
            uint32 day = static_cast<uint32>(NEX_STD stoi(match[3].str()));
            int32 hour = NEX_STD stoi(match[4].str());
            int32 minute = NEX_STD stoi(match[5].str());
            int32 second = NEX_STD stoi(match[6].str());
            int32 millisec = match[7].matched ? NEX_STD stoi(match[7].str()) : 0;
            // Pad milliseconds
            if (match[7].matched && match[7].str().length() == 1) millisec *= 100;
            else if (match[7].matched && match[7].str().length() == 2) millisec *= 10;
            
            if (isValidDate(year, month, day) && isValidTime(hour, minute, second, millisec)) {
                return DateTime(year, month, day, hour, minute, second, millisec);
            }
        }
        catch (...) {
            // Fall through
        }
    }
    
    // Return empty DateTime on failure
    return DateTime();
}

NEX_NAMESPACE_END
