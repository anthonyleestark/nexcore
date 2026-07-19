/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/assert_crash.h"
#include "nex/core/text/string.h"
#include "nex/core/chrono/chrono_base.h"
#include "nex/core/chrono/timespan.h"
#include "nex/core/chrono/date_time.h"

NEX_LAYER_NAMESPACE_BEGIN(chrono)

/**
 * @class   Timestamp
 * @brief   Represents an absolute instant on the system clock.
 *
 * @details
 * Timestamp stores a system-clock time point and is intended for event times,
 * persistence, ordering, and elapsed-time calculations. Unlike Date and
 * ClockTime, it has no calendar or time-of-day semantics of its own. Unix
 * epoch values can be read and created explicitly in seconds or milliseconds.
 * 
 * @see DateTime, TimeSpan
 */
class NEX_API Timestamp {
private:
    // Internal representation of the absolute instant.
    TimePoint timePoint_{};

public:
    // Construct an empty Timestamp at the Unix epoch.
    explicit Timestamp() = default;

    // Copy constructor and assignment.
    NEX_DEFAULT_COPY(Timestamp);

    // Move constructor and assignment.
    NEX_DEFAULT_MOVE(Timestamp);

    // Construct a Timestamp from a system-clock time point.
    explicit constexpr Timestamp(TimePoint timePoint) noexcept : timePoint_(timePoint) {}

    // Construct a Timestamp from a DateTime value.
    explicit Timestamp(const DateTime& dateTime) noexcept
        : timePoint_(dateTime.getTimePoint()) {}

    // Create a Timestamp for the current system-clock instant.
    static Timestamp now() noexcept {
        return Timestamp(NEX_STD chrono::system_clock::now());
    }

    // Create a Timestamp from seconds since the Unix epoch.
    static constexpr Timestamp fromSeconds(int64 seconds) noexcept {
        return Timestamp(TimePoint{ Seconds{ seconds } });
    }

    // Create a Timestamp from milliseconds since the Unix epoch.
    static constexpr Timestamp fromMilliseconds(int64 milliseconds) noexcept {
        return Timestamp(TimePoint{ Milliseconds{ milliseconds } });
    }

    // Get the underlying system-clock time point.
    constexpr TimePoint getTimePoint() const noexcept { return timePoint_; }

    // Convert this Timestamp to a DateTime representation.
    DateTime getDateTime() const noexcept { return DateTime(timePoint_); }

    // Get elapsed whole seconds since the Unix epoch.
    constexpr int64 secondsSinceEpoch() const noexcept {
        return NEX_STD chrono::duration_cast<Seconds>(timePoint_.time_since_epoch()).count();
    }

    // Get elapsed whole milliseconds since the Unix epoch.
    constexpr int64 millisecondsSinceEpoch() const noexcept {
        return NEX_STD chrono::duration_cast<Milliseconds>(timePoint_.time_since_epoch()).count();
    }

    // Check whether this Timestamp is the default Unix-epoch value.
    constexpr bool isEmpty() const noexcept { return timePoint_ == TimePoint{}; }

    // Return a Timestamp offset by a time span.
    Timestamp operator+(const TimeSpan& timeSpan) const noexcept {
        return Timestamp(timePoint_ + timeSpan.getDuration());
    }

    // Offset this Timestamp by a time span.
    Timestamp& operator+=(const TimeSpan& timeSpan) noexcept {
        timePoint_ += timeSpan.getDuration();
        return *this;
    }

    // Return a Timestamp offset by a number of seconds.
    Timestamp operator+(int32 seconds) const noexcept {
        return Timestamp(timePoint_ + Seconds{ seconds });
    }

    // Offset this Timestamp by a number of seconds.
    Timestamp& operator+=(int32 seconds) noexcept {
        timePoint_ += Seconds{ seconds };
        return *this;
    }

    // Return the elapsed time span between this Timestamp and another Timestamp.
    TimeSpan operator-(const Timestamp& other) const noexcept {
        return TimeSpan(timePoint_ - other.timePoint_);
    }

    // Return a Timestamp moved backward by a time span.
    Timestamp operator-(const TimeSpan& timeSpan) const noexcept {
        return Timestamp(timePoint_ - timeSpan.getDuration());
    }

    // Move this Timestamp backward by a time span.
    Timestamp& operator-=(const TimeSpan& timeSpan) noexcept {
        timePoint_ -= timeSpan.getDuration();
        return *this;
    }

    // Return a Timestamp moved backward by a number of seconds.
    Timestamp operator-(int32 seconds) const noexcept {
        return Timestamp(timePoint_ - Seconds{ seconds });
    }

    // Move this Timestamp backward by a number of seconds.
    Timestamp& operator-=(int32 seconds) noexcept {
        timePoint_ -= Seconds{ seconds };
        return *this;
    }

    // Compare two Timestamps for equality.
    constexpr bool operator==(const Timestamp& other) const noexcept {
        return timePoint_ == other.timePoint_;
    }

    // Compare two Timestamps for inequality.
    constexpr bool operator!=(const Timestamp& other) const noexcept {
        return timePoint_ != other.timePoint_;
    }

    // Check whether this Timestamp is earlier than another Timestamp.
    constexpr bool operator<(const Timestamp& other) const noexcept {
        return timePoint_ < other.timePoint_;
    }

    // Check whether this Timestamp is later than another Timestamp.
    constexpr bool operator>(const Timestamp& other) const noexcept {
        return timePoint_ > other.timePoint_;
    }

    // Check whether this Timestamp is no later than another Timestamp.
    constexpr bool operator<=(const Timestamp& other) const noexcept {
        return timePoint_ <= other.timePoint_;
    }

    // Check whether this Timestamp is no earlier than another Timestamp.
    constexpr bool operator>=(const Timestamp& other) const noexcept {
        return timePoint_ >= other.timePoint_;
    }

    // Advance this Timestamp by one second.
    Timestamp& operator++() noexcept { return increaseSeconds(1); }

    // Move this Timestamp back by one second.
    Timestamp& operator--() noexcept { return decreaseSeconds(1); }

    // Return the original Timestamp, then advance it by one second.
    Timestamp operator++(int32) noexcept { Timestamp original = *this; ++(*this); return original; }

    // Return the original Timestamp, then move it back by one second.
    Timestamp operator--(int32) noexcept { Timestamp original = *this; --(*this); return original; }

    // Check whether this Timestamp is later than the current system-clock instant.
    bool isInTheFuture() const noexcept {
        return timePoint_ > NEX_STD chrono::system_clock::now();
    }

    // Check whether this Timestamp is earlier than the current system-clock instant.
    bool isInThePast() const noexcept {
        return timePoint_ < NEX_STD chrono::system_clock::now();
    }

    // Advance this Timestamp by a number of days.
    Timestamp& increaseDays(int32 days) noexcept {
        timePoint_ += Days{ days };
        return *this;
    }

    // Move this Timestamp back by a number of days.
    Timestamp& decreaseDays(int32 days) noexcept {
        timePoint_ -= Days{ days };
        return *this;
    }

    // Advance this Timestamp by a number of hours.
    Timestamp& increaseHours(int32 hours) noexcept {
        timePoint_ += Hours{ hours };
        return *this;
    }

    // Move this Timestamp back by a number of hours.
    Timestamp& decreaseHours(int32 hours) noexcept {
        timePoint_ -= Hours{ hours };
        return *this;
    }

    // Advance this Timestamp by a number of minutes.
    Timestamp& increaseMinutes(int32 minutes) noexcept {
        timePoint_ += Minutes{ minutes };
        return *this;
    }

    // Move this Timestamp back by a number of minutes.
    Timestamp& decreaseMinutes(int32 minutes) noexcept {
        timePoint_ -= Minutes{ minutes };
        return *this;
    }

    // Advance this Timestamp by a number of seconds.
    Timestamp& increaseSeconds(int32 seconds) noexcept {
        timePoint_ += Seconds{ seconds };
        return *this;
    }

    // Move this Timestamp back by a number of seconds.
    Timestamp& decreaseSeconds(int32 seconds) noexcept {
        timePoint_ -= Seconds{ seconds };
        return *this;
    }

    // Advance this Timestamp by a number of milliseconds.
    Timestamp& increaseMilliseconds(int32 milliseconds) noexcept {
        timePoint_ += Milliseconds{ milliseconds };
        return *this;
    }

    // Move this Timestamp back by a number of milliseconds.
    Timestamp& decreaseMilliseconds(int32 milliseconds) noexcept {
        timePoint_ -= Milliseconds{ milliseconds };
        return *this;
    }

    /**
     * @brief   Convert Timestamp to a DateTime string representation.
     * @param   format  A DateTime format: default, with_ms, iso, iso_ms, us, or us_12h.
     * @return  The formatted Timestamp.
     */
    String toString(wcstring format = L"default") const;

    /**
     * @brief   Create a Timestamp from a DateTime string representation.
     * @param   str  A string accepted by DateTime::fromString.
     * @return  The parsed Timestamp, or the default Timestamp when parsing fails.
     */
    static Timestamp fromString(const String& str);
};


NEX_LAYER_NAMESPACE_END(chrono)
