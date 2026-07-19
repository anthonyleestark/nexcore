/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/chrono/clock_time.h"
#include "src/core/chrono/string_helper.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

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
    WString fmt(format);
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
    String trimmed = string_helper::trim(str);
    if (trimmed.empty()) {
        return ClockTime(0, 0, 0);
    }
    
    auto utf16Res = trimmed.toUtf16();
    if (!utf16Res.isOk()) return ClockTime(0, 0, 0);
    const Utf16String& u16str = utf16Res.value();
    WString wstr(u16str.begin(), u16str.end());
    
    // Try compact format: HHMM
    if (wstr.length() == 4 && NEX_STD all_of(wstr.begin(), wstr.end(), ::iswdigit)) {
        try {
            int32 hour = NEX_STD stoi(wstr.substr(0, 2));
            int32 minute = NEX_STD stoi(wstr.substr(2, 2));
            if (isValidHour(hour) && isValidMinute(minute)) {
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
            
            WString ampm = match[5].str();
            NEX_STD transform(ampm.begin(), ampm.end(), ampm.begin(), ::towlower);
            if (ampm == L"pm" && hour != 12) hour += 12;
            else if (ampm == L"am" && hour == 12) hour = 0;
            
            if (isValidHour(hour) && isValidMinute(minute)
                && isValidSecond(second) && isValidMillisecs(millisec)) {
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
            
            if (isValidHour(hour) && isValidMinute(minute)
                && isValidSecond(second) && isValidMillisecs(millisec)) {
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

NEX_LAYER_NAMESPACE_END(chrono)
