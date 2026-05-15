/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <bitset>
#include <algorithm>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/wrappers.h"
#include "nex/core/text/string.h"
#include "nex/core/chrono/time.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @enum    RepeatFrequency
 * @brief   Frequency of repetition for a scheduled event
 * 
 * Defines the possible frequencies for repeating events such as alarms or reminders.
 * 
 * @note The default frequency is `None`, indicating no repetition.
 * @see RepeatPattern
 */
enum class RepeatFrequency {
    None,       // No repeat (default)
    Daily,      // Repeat every day
    Weekly,     // Repeat every week on the same day
    Monthly,    // Repeat every month on the same date
    Yearly      // Repeat every year on the same date
};

/**
 * @class   RepeatPattern
 * @brief   Repeat pattern value object
 * 
 * Immutable value object representing schedule repetition.
 * Provides configuration for repeat enabling, active days,
 * and snooze options.
 * 
 * @details
 * RepeatPattern encapsulates the configuration for repeating
 * scheduled events such as alarms or reminders. It allows
 * specification of which days of the week the event should
 * repeat on, whether snooze is allowed, and the snooze interval.
 * 
 * @note Days of the week are represented as a bitset, with
 *       bit 0 = Sunday, bit 1 = Monday, bit 2 = Tuesday, bit 3 = Wednesday,
 *       bit 4 = Thursday, bit 5 = Friday, bit 6 = Saturday.
 * @note Snooze interval is clamped between 1 minute and 1 hour.
 * 
 * @see Alarm, Reminder
 */
class NEX_EXPORT RepeatPattern {
private:
    // Constants
    static constexpr uint8 kDaysPerWeek = 7;                // Number of days in a week
    static constexpr uintmax kAllDaysMask = 0b01111111UL;   // All 7 days active (bits 0-6)
    static constexpr int32 kMinSnoozeSeconds = 60;          // Minimum: 1 minute
    static constexpr int32 kMaxSnoozeSeconds = 3600;        // Maximum: 1 hour
    static constexpr int32 kDefaultSnoozeSeconds = 600;     // Default: 10 minutes
    static constexpr int32 kMinInterval = 1;                // Minimum interval for repeating (e.g., 1 day, 1 week)
    static constexpr int32 kDefaultInterval = 1;            // Default interval for repeating
    static constexpr int32 kForeverCount = 0;               // Repeat forever

public:
    // Default constructor
    RepeatPattern() = default;

    // Constructor with repeat enabled
    explicit RepeatPattern(bool repeatEnabled)
        : enabled_(repeatEnabled),
        frequency_(deriveFrequency(repeatEnabled, NEX_STD bitset<7>(kAllDaysMask))),
        endDate_(NEX_STD nullopt) {}

    // Constructor with repeat enabled and active days
    RepeatPattern(bool repeatEnabled, const NEX_STD bitset<7>& activeDays)
        : enabled_(repeatEnabled),
        activeDays_(activeDays),
        frequency_(deriveFrequency(repeatEnabled, activeDays)),
        endDate_(NEX_STD nullopt) {}

    // Check if repeat is enabled
    bool isEnabled() const { return enabled_; }

    // Get active days
    const NEX_STD bitset<7>& activeDays() const { return activeDays_; }

    // Replace active days
    RepeatPattern withActiveDays(const NEX_STD bitset<7>& activeDays) const {
        RepeatPattern pattern = *this;
        pattern.activeDays_ = activeDays;
        if (pattern.enabled_ && isDayBasedFrequency(pattern.frequency_)) {
            pattern.frequency_ = deriveFrequency(true, activeDays);
        }
        return pattern;
    }

    // Check if repeat is active on a day
    bool isActiveOnDay(uint8 dayOfWeek) const { // 0=Sunday, 6=Saturday
        if (!enabled_ || dayOfWeek >= kDaysPerWeek) return false;
        return activeDays_[dayOfWeek];
    }

    // Set day active
    RepeatPattern withDayActive(uint8 dayOfWeek, bool active) const {
        RepeatPattern pattern = *this;
        if (dayOfWeek < kDaysPerWeek) {
            pattern.activeDays_[dayOfWeek] = active;
            if (pattern.enabled_ && isDayBasedFrequency(pattern.frequency_)) {
                pattern.frequency_ = deriveFrequency(true, pattern.activeDays_);
            }
        }
        return pattern;
    }

    // Check if snooze is allowed
    bool allowsSnooze() const { return allowsSnooze_; }

    // Get snooze interval
    int snoozeIntervalSeconds() const { return snoozeIntervalSeconds_; }
    
    // Set snooze options
    RepeatPattern withSnooze(bool allowed, int32 intervalSeconds) const {
        RepeatPattern pattern = *this;
        pattern.allowsSnooze_ = allowed;
        pattern.snoozeIntervalSeconds_ = clampSnoozeInterval(intervalSeconds);
        return pattern;
    }

    ////// Advanced configuration accessors -----------------------

    // Get label for the repeat pattern
    const String& label() const { return label_; }

    // Get interval for the repeat pattern
    int32 interval() const { return interval_; }

    // Get frequency for the repeat pattern
    RepeatFrequency frequency() const { return frequency_; }

    // Check if an end date is set
    bool hasEndDate() const { return endDate_.has_value(); }

    // Get the end date (returns nullopt if no end date is set)
    const Optional<DateTime::Date>& endDate() const { return endDate_; }

    // Get repeat count (0 = forever)
    int32 count() const { return count_; }

    // Check if the repeat pattern repeats forever (count = 0)
    bool repeatsForever() const { return count_ == kForeverCount; }

    // Check if the repeat pattern repeats from completion time
    bool repeatFromCompletion() const { return repeatFromCompletion_; }

    ////// Advanced configuration modifiers -----------------------

    // Set label for the repeat pattern
    RepeatPattern withLabel(const String& label) const {
        RepeatPattern pattern = *this;
        pattern.label_ = label;
        return pattern;
    }

    // Set interval for the repeat pattern
    RepeatPattern withInterval(int32 interval) const {
        RepeatPattern pattern = *this;
        pattern.interval_ = normalizeInterval(interval);
        return pattern;
    }

    // Set frequency for the repeat pattern
    RepeatPattern withFrequency(RepeatFrequency frequency) const {
        RepeatPattern pattern = *this;
        pattern.frequency_ = frequency;
        pattern.enabled_ = (frequency != RepeatFrequency::None);
        if (frequency == RepeatFrequency::Daily) {
            pattern.activeDays_ = NEX_STD bitset<7>(kAllDaysMask);
        }
        return pattern;
    }

    // Set end date for the repeat pattern
    RepeatPattern withEndDate(const DateTime::Date& endDate) const {
        RepeatPattern pattern = *this;
        if (endDate.ok()) {
            pattern.endDate_ = endDate;
        } else {
            pattern.endDate_.reset();
        }
        return pattern;
    }

    // Remove end date from the repeat pattern
    RepeatPattern withoutEndDate() const {
        RepeatPattern pattern = *this;
        pattern.endDate_.reset();
        return pattern;
    }

    // Set repeat count for the repeat pattern
    RepeatPattern withCount(int32 count) const {
        RepeatPattern pattern = *this;
        pattern.count_ = normalizeCount(count);
        return pattern;
    }

    // Set whether to repeat from completion time
    RepeatPattern withRepeatFromCompletion(bool repeatFromCompletion) const {
        RepeatPattern pattern = *this;
        pattern.repeatFromCompletion_ = repeatFromCompletion;
        return pattern;
    }
    
    // Equality operator
    bool operator==(const RepeatPattern& other) const {
        return (enabled_ == other.enabled_ &&
                activeDays_ == other.activeDays_ &&
                allowsSnooze_ == other.allowsSnooze_ &&
                snoozeIntervalSeconds_ == other.snoozeIntervalSeconds_ &&
                label_ == other.label_ &&
                interval_ == other.interval_ &&
                frequency_ == other.frequency_ &&
                endDate_ == other.endDate_ &&
                count_ == other.count_ &&
                repeatFromCompletion_ == other.repeatFromCompletion_);
    }

    // Inequality operator
    bool operator!=(const RepeatPattern& other) const { return !(*this == other); }

private:
    ////// Basic attributes -----------------------

    bool enabled_ = false;                                  // Whether repeat is enabled
    NEX_STD bitset<7> activeDays_{ kAllDaysMask };          // Bit 0=Sunday, bit 6=Saturday
    bool allowsSnooze_ = false;                             // Whether snooze is allowed
    int32 snoozeIntervalSeconds_ = kDefaultSnoozeSeconds;   // Snooze interval in seconds

    ////// Advanced attributes -----------------------

    // Optional: "Morning Workout", "Take Medicine", etc. (for UI/debug)
    String label_{};

    // Repeat interval, e.g. 2 = every 2 days/weeks (default 1)
    int32 interval_ = kDefaultInterval;

    // Repeat frequency (daily, weekly, monthly, yearly)
    RepeatFrequency frequency_ = RepeatFrequency::None;

    // Optional end date for the repeat pattern
    Optional<DateTime::Date> endDate_ = NEX_STD nullopt;

    // Repeat count, e.g. 5 = repeat 5 times (0 = forever)
    int32 count_ = kForeverCount;

    // Whether to repeat from completion time (vs. original time)
    // Advanced: next occurrence after user completes/snoozes (vs fixed schedule)
    bool repeatFromCompletion_ = false;

    ////// Helper functions -----------------------

    // Clamp snooze interval to valid range
    static int32 clampSnoozeInterval(int32 seconds) {
        return NEX_STD clamp(seconds, kMinSnoozeSeconds, kMaxSnoozeSeconds);
    }

    // Normalize interval to valid range
    static int32 normalizeInterval(int32 interval) {
        return (interval < kMinInterval) ? kDefaultInterval : interval;
    }

    // Normalize count to valid range (0 = forever)
    static int32 normalizeCount(int32 count) {
        return (count < kForeverCount) ? kForeverCount : count;
    }

    // Check if the frequency is day-based (None, Daily, Weekly)
    static bool isDayBasedFrequency(RepeatFrequency frequency) {
        return (frequency == RepeatFrequency::None ||
                frequency == RepeatFrequency::Daily ||
                frequency == RepeatFrequency::Weekly);
    }

    // Derive frequency based on enabled state and active days
    static RepeatFrequency deriveFrequency(bool enabled, const NEX_STD bitset<7>& activeDays) {
        if (!enabled) {
            return RepeatFrequency::None;
        }

        return (activeDays == NEX_STD bitset<7>(kAllDaysMask))
            ? RepeatFrequency::Daily
            : RepeatFrequency::Weekly;
    }
};

NEX_CORE_NAMESPACE_END
