/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/chrono/date_time.h"
#include "src/core/chrono/string_helper.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

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
    WString fmt(format);
    
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
    String trimmed = string_helper::trim(str);
    if (trimmed.empty()) {
        return DateTime();
    }
    
    auto utf16Res = trimmed.toUtf16();
    if (!utf16Res.isOk()) return DateTime();
    const Utf16String& u16str = utf16Res.value();
    WString wstr(u16str.begin(), u16str.end());
    
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
            
            WString ampm = match[8].str();
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

NEX_LAYER_NAMESPACE_END(chrono)
