/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"
#include "nex/base/result.h"
#include "nex/base/primitives.h"
#include "nex/base/logging.h"
#include "src/base/logging_internal.h"
#include "nex/core/chrono/timestamp.h"

NEX_LAYER_NAMESPACE_BEGIN(logging)

// Using the Timestamp class from the core/chrono layer for log record timestamps.
using chrono::Timestamp;

/**
 * @typedef TickCount
 * @brief   Represents a high-resolution tick count used for performance measurements.
 * 
 * @details
 * This type is used to capture the number of ticks since an arbitrary point in time, typically used for
 * measuring elapsed time or performance metrics. It is a 64-bit unsigned integer to accommodate large
 * tick counts over extended periods.
 */
using TickCount = uint64;

/**
 * @brief Retrieves the current high-resolution tick count.
 * @return The current TickCount value.
 * @note The tick count is typically used for performance measurements and may not correspond to real-world time.
 */
NEX_NODISCARD NEX_INTERNAL
TickCount getCurrentTickCount() noexcept;

/**
 * @typedef ThreadId
 * @brief   Thread ID number used in log records.
 * 
 * @details
 * This type represents the unique identifier for a thread that generated a log record.
 * It is typically used for logging and debugging purposes to identify which thread produced a specific log entry.
 * 
 * @note
 * This ThreadId is different from the native thread ID provided by the Thread class.
 * It is a numeric representation used for logging purposes only.
 */
using ThreadId = uint64;

/**
 * @brief Retrieves the current thread's ID.
 * @return  The ThreadId of the current thread.
 */
NEX_NODISCARD NEX_INTERNAL
ThreadId getCurrentThreadId() noexcept;

/**
 * @typedef ProcessId
 * @brief   Process ID number used in log records.
 * 
 * @details
 * This type represents the unique identifier for the process that generated a log record.
 * It is typically used for logging and debugging purposes to identify which process produced a specific log entry.
 * 
 * @note
 * This ProcessId is a numeric representation used for logging purposes only.
 */
using ProcessId = uint64;

/**
 * @brief Retrieves the current process's ID.
 * @return The ProcessId of the current process.
 */
NEX_NODISCARD NEX_INTERNAL
ProcessId getCurrentProcessId() noexcept;

/**
 * @class   SequenceNumber
 * @brief   Monotonically increasing sequence number for log records.
 * 
 * @details
 * The SequenceNumber class provides a mechanism for generating and managing unique, monotonically increasing
 * sequence numbers for log records. Each time a new SequenceNumber is generated, it increments an internal
 * counter to ensure that each number is unique within the process. The class provides methods for checking
 * the validity of a sequence number, converting it to a uint64, and comparing sequence numbers.
 * 
 * @note
 * A SequenceNumber with value 0 is considered invalid.
 * SequenceNumbers generated at runtime are guaranteed to be unique within the process and monotonically
 * increasing. However, they are not globally unique across different runs or systems. For global uniqueness,
 * consider using UUIDs or similar mechanisms.
 * 
 * @see RuntimeId class for runtime unique identifiers, with similar functionality but for different use cases.
 */
class NEX_INTERNAL SequenceNumber {
public:
    // Underlying type for the sequence number
    using value_type = uint64;

    // Special constant representing the maximum value of a SequenceNumber (used for overflow checks).
    static constexpr value_type maxValue = NumericLimits<value_type>::max();

    // Default constructor initializes the SequenceNumber to an invalid state (value 0).
    constexpr SequenceNumber() noexcept = default;

    // Generates the next unique SequenceNumber.
    static SequenceNumber next() {
        // Start from 1 because 0 is invalid
        static Atomic<value_type> counter{1};
        return SequenceNumber(counter.fetch_add(1, NEX_STD memory_order_relaxed));
    }

    // Gets the underlying value of the SequenceNumber.
    constexpr value_type value() const noexcept { return value_; }

    // Converts the SequenceNumber to the underlying value type.
    constexpr explicit operator value_type() const noexcept { return value_; }

    // Checks if the SequenceNumber is valid.
    constexpr bool valid() const noexcept { return value_ != 0; }

    // Explicit conversion to bool.
    constexpr explicit operator bool() const noexcept { return valid(); }

    // Equality operator for comparing two SequenceNumber instances.
    constexpr bool operator==(const SequenceNumber& other) const noexcept {
        return value_ == other.value_;
    }

    // Inequality operator for comparing two SequenceNumber instances.
    constexpr bool operator!=(const SequenceNumber& other) const noexcept {
        return value_ != other.value_;
    }
    
    // Strong ordering comparison operator for SequenceNumber instances.
    constexpr NEX_STD strong_ordering operator<=>(const SequenceNumber& other) const noexcept {
        return value_ <=> other.value_;
    }

private:
    // Creates a SequenceNumber from a specific unsigned integer value.
    template<typename Type>
        requires (meta::IsIntegralV<Type> && meta::IsUnsignedIntegralV<Type>)
    explicit constexpr SequenceNumber(Type value) noexcept : value_(static_cast<value_type>(value)) {}

    // Internal value representing the sequence number (0 is invalid)
    value_type value_ = 0;
};

/**
 * @struct  LogRecord
 * @brief   Represents a fully detailed log record used by the logging system.
 * 
 * @details
 * This struct encapsulates the information associated with a log record, including the payload,
 * thread ID, process ID, and sequence number. It is used by the logging system to manage and process
 * log entries generated by different threads and processes.
 */
struct NEX_INTERNAL LogRecord {
    /**
     * @brief The payload of the log record, containing metadata and the raw log message buffer.
     * @details
     * The payload is transferred to the logging system by the submit() function and is directly consumed
     * by the logging backend without any further modifications. It will then be processed and formatted
     * by the logging system when the log record is actually written to the output sink.
     */
    PendingLog          payload;

    /**
     * @brief The timestamp of when the log record was created.
     * @note
     * The timestamp is captured at the moment the logging system receives the log package from the base/logging layer
     * right after the submit() function is called and represents the absolute time at which the log entry was generated.
     */
    Timestamp           timestamp;

    /**
     * @brief The tick count of when the log record was created.
     * @note
     * The tick count is captured at the moment the logging system receives the log package from the base/logging layer
     * right after the submit() function is called and represents the high-resolution tick count at which the log entry
     * was generated.
     */
    TickCount           tickCount;

    /**
     * @brief The thread ID of the thread that generated the log record.
     * @details
     * This field is used to identify which thread produced a specific log entry. It is typically used for
     * logging and debugging purposes to track the source of log messages in multi-threaded applications.
     */
    ThreadId            threadId;

    /**
     * @brief The process ID of the process that generated the log record.
     * @details
     * This field is used to identify which process produced a specific log entry. It is typically used for
     * logging and debugging purposes to track the source of log messages in multi-process applications.
     */
    ProcessId           processId;

    /**
     * @brief The sequence number assigned to the log record.
     * @details
     * This field is a monotonically increasing number that is assigned to each log record. It is used to
     * track the order of log entries and can be useful for debugging and analyzing the flow of log messages.
     */
    SequenceNumber      sequenceNumber;
};

/**
 * @brief Creates a LogRecord from a PendingLog payload.
 * @param payload An rvalue reference to the PendingLog instance containing the log metadata and message buffer.
 * @return LogRecord instance.
 * @note 
 * This function is intended for internal use by the logging system and should not be called directly by 
 * application code.
 */
NEX_NODISCARD NEX_INTERNAL
LogRecord createLogRecord(PendingLog&& payload);

// Counter for the last submitted sequence number.
static Atomic<SequenceNumber::value_type> lastSubmittedSequenceNumber = 0;

/**
 * @brief Process a LogRecord after being dequeued from the logging queue.
 * @param record An rvalue reference to the LogRecord instance to be processed.
 * @return Result<void> indicating success or failure of the processing operation.
 * @note
 * This function is intended for internal use by the logging system and should not be called directly by
 * application code. It is responsible for formatting the log record and routing it to the appropriate sinks
 */
NEX_INTERNAL Result<void> process(LogRecord&& record) noexcept;

NEX_LAYER_NAMESPACE_END(logging)
