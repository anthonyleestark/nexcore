/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/chrono/date.h"
#include "src/core/chrono/string_helper.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

// Return a Date offset by the whole-day portion of a time span.
Date Date::operator+(const TimeSpan& timeSpan) const noexcept {
    Date result = *this;
    result += timeSpan;
    return result;
}

// Offset this Date by the whole-day portion of a time span.
Date& Date::operator+=(const TimeSpan& timeSpan) noexcept {
    requireValid();
    date_ = CalendarDate{getSysDays() + NEX_STD chrono::floor<Days>(timeSpan.getDuration())};
    return *this;
}

// Return a Date offset by a number of calendar days.
Date Date::operator+(int32 days) const noexcept {
    Date result = *this;
    result += days;
    return result;
}

// Offset this Date by a number of calendar days.
Date& Date::operator+=(int32 days) noexcept {
    return increaseDays(days);
}

// Return the elapsed time span between this Date and another Date.
TimeSpan Date::operator-(const Date& other) const noexcept {
    requireValid();
    other.requireValid();
    return TimeSpan(getSysDays() - other.getSysDays());
}

// Return a Date moved backward by the whole-day portion of a time span.
Date Date::operator-(const TimeSpan& timeSpan) const noexcept {
    Date result = *this;
    result -= timeSpan;
    return result;
}

// Move this Date backward by the whole-day portion of a time span.
Date& Date::operator-=(const TimeSpan& timeSpan) noexcept {
    requireValid();
    date_ = CalendarDate{getSysDays() + NEX_STD chrono::floor<Days>(-timeSpan.getDuration())};
    return *this;
}

// Return a Date moved backward by a number of calendar days.
Date Date::operator-(int32 days) const noexcept {
    Date result = *this;
    result -= days;
    return result;
}

// Move this Date backward by a number of calendar days.
Date& Date::operator-=(int32 days) noexcept {
    return decreaseDays(days);
}

// Check if this Date represents the current system date.
bool Date::isToday() const noexcept {
    return ok() && getSysDays() == NEX_STD chrono::floor<Days>(NEX_STD chrono::system_clock::now());
}

// Set the year of this Date, preserving the month and day if valid.
void Date::setYear(int32 year) noexcept {
    requireValid();
    NEX_ASSERT(isValidDate(year, month(), day()));
    assign(year, month(), day());
}

// Set the month of this Date, preserving the year and day if valid.
void Date::setMonth(uint32 month) noexcept {
    requireValid();
    NEX_ASSERT(isValidDate(year(), month, day()));
    assign(year(), month, day());
}

// Set the day of this Date, preserving the year and month if valid.
void Date::setDay(uint32 day) noexcept {
    requireValid();
    NEX_ASSERT(isValidDate(year(), month(), day));
    assign(year(), month(), day);
}

// Increase the year of this Date by a specified number of years, preserving the month and day if valid.
Date& Date::increaseDays(int32 days) noexcept {
    requireValid();
    date_ = CalendarDate{getSysDays() + Days{days}};
    return *this;
}

// Decrease the year of this Date by a specified number of years, preserving the month and day if valid.
Date& Date::decreaseDays(int32 days) noexcept {
    requireValid();
    date_ = CalendarDate{getSysDays() - Days{days}};
    return *this;
}

// Increase the month of this Date by a specified number of months, preserving the year and day if valid.
Date& Date::increaseMonths(int32 months) noexcept {
    requireValid();
    const auto targetMonth = date_.year() / date_.month() + Months{months};
    const int32 targetYear = static_cast<int32>(targetMonth.year());
    const uint32 targetMonthNumber = static_cast<uint32>(targetMonth.month());
    const uint32 targetDay = NEX_STD min(day(), daysInMonth(targetYear, targetMonthNumber));
    NEX_ASSERT(isValidDate(targetYear, targetMonthNumber, targetDay));
    assign(targetYear, targetMonthNumber, targetDay);
    return *this;
}

// Decrease the month of this Date by a specified number of months, preserving the year and day if valid.
Date& Date::decreaseMonths(int32 months) noexcept {
    return increaseMonths(-months);
}

// Increase the year of this Date by a specified number of years, preserving the month and day if valid.
Date& Date::increaseYears(int32 years) noexcept {
    requireValid();
    const int32 targetYear = year() + years;
    const uint32 targetDay = NEX_STD min(day(), daysInMonth(targetYear, month()));
    NEX_ASSERT(isValidDate(targetYear, month(), targetDay));
    assign(targetYear, month(), targetDay);
    return *this;
}

// Decrease the year of this Date by a specified number of years, preserving the month and day if valid.
Date& Date::decreaseYears(int32 years) noexcept {
    return increaseYears(-years);
}

// Convert this Date to a string representation in the specified format.
String Date::toString(wcstring format) const {
    if (!ok()) return String();

    const WString fmt(format ? format : L"default");
    if (fmt == L"us") {
        return string_helper::padded(month(), 2).append(u"/")
            .append(string_helper::padded(day(), 2)).append(u"/")
            .append(string_helper::paddedYear(year()));
    }
    if (fmt == L"compact") {
        return string_helper::paddedYear(year())
            .append(string_helper::padded(month(), 2))
            .append(string_helper::padded(day(), 2));
    }
    return string_helper::paddedYear(year()).append(u"-")
        .append(string_helper::padded(month(), 2)).append(u"-")
        .append(string_helper::padded(day(), 2));
}

// Create a Date from a string representation in various formats.
Date Date::fromString(const String& str) {
    const String trimmed = string_helper::trim(str);
    if (trimmed.empty()) return Date();

    const auto utf16Result = trimmed.toUtf16();
    if (!utf16Result.isOk()) return Date();
    const Utf16String& utf16 = utf16Result.value();
    const WString text(utf16.begin(), utf16.end());
    NEX_STD wsmatch match;

    const NEX_STD wregex isoPattern(LR"((\d{4})[-/](\d{1,2})[-/](\d{1,2}))");
    if (NEX_STD regex_match(text, match, isoPattern)) {
        try {
            const int32 parsedYear = NEX_STD stoi(match[1].str());
            const uint32 parsedMonth = static_cast<uint32>(NEX_STD stoul(match[2].str()));
            const uint32 parsedDay = static_cast<uint32>(NEX_STD stoul(match[3].str()));
            if (isValidDate(parsedYear, parsedMonth, parsedDay)) {
                return Date(parsedYear, parsedMonth, parsedDay);
            }
        } catch (...) {
        }
    }

    const NEX_STD wregex usPattern(LR"((\d{1,2})/(\d{1,2})/(\d{4}))");
    if (NEX_STD regex_match(text, match, usPattern)) {
        try {
            const uint32 parsedMonth = static_cast<uint32>(NEX_STD stoul(match[1].str()));
            const uint32 parsedDay = static_cast<uint32>(NEX_STD stoul(match[2].str()));
            const int32 parsedYear = NEX_STD stoi(match[3].str());
            if (isValidDate(parsedYear, parsedMonth, parsedDay)) {
                return Date(parsedYear, parsedMonth, parsedDay);
            }
        } catch (...) {
        }
    }

    const NEX_STD wregex compactPattern(LR"((\d{4})(\d{2})(\d{2}))");
    if (NEX_STD regex_match(text, match, compactPattern)) {
        try {
            const int32 parsedYear = NEX_STD stoi(match[1].str());
            const uint32 parsedMonth = static_cast<uint32>(NEX_STD stoul(match[2].str()));
            const uint32 parsedDay = static_cast<uint32>(NEX_STD stoul(match[3].str()));
            if (isValidDate(parsedYear, parsedMonth, parsedDay)) {
                return Date(parsedYear, parsedMonth, parsedDay);
            }
        } catch (...) {
        }
    }

    return Date();
}

NEX_LAYER_NAMESPACE_END(chrono)
