/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/platform.h"
#include "nex/base/logging.h"

NEX_LAYER_NAMESPACE_BEGIN(logging)

#if NEX_PLATFORM_IS_WINDOWS
    // Windows error codes are typically represented as unsigned long integers
    using SystemErrorCode = ulong;
#else
    // On Unix-like systems, error codes are typically represented as integers
    using SystemErrorCode = int32;
#endif

/**
 * @class LastSystemError
 * @brief Represents the last system error code and message.
 * 
 * @details
 * The LastSystemError class encapsulates the last system error code and its corresponding message.
 * It provides static methods to retrieve the last error code and message, which can be useful for
 * logging and debugging purposes. The class is designed to be used internally by the logging system
 * and is not intended for direct instantiation.
 * The error code and message are typically obtained from the underlying operating system's error
 * reporting mechanisms.
 * The class is designed to be used in conjunction with the LogBuilder class, allowing for easy logging
 * of system errors in a structured manner.
 */
class NEX_INTERNAL LastSystemError {
public:
    // Default constructor
    LastSystemError() noexcept = default;

    // Get the last system error code
    NEX_INTERNAL SystemErrorCode getLastErrorCode() noexcept {
        return lastErrorCode_;
    }

    // Get the last system error message as a string
    NEX_INTERNAL LogString getLastErrorMessage() noexcept {
        return lastErrorMessage_;
    }

private:
    // Only the LogBuilder class can modify LastSystemError
    friend class LogBuilder;

    SystemErrorCode lastErrorCode_ = 0;     // Store the last error code
    LogString lastErrorMessage_ = {};       // Store the last error message
};

/**
 * @struct PendingLog
 * @brief Represents a pending log record to be submitted to the logging system.
 * 
 * @details
 * The PendingLog structure encapsulates the metadata, message buffer, and last system error information
 * associated with a log record that is ready to be submitted to the logging system.
 * It is used internally by the LogBuilder class to package log data before dispatching it to the logger.
 * The structure is designed to be efficient and lightweight, allowing for quick construction and submission
 * of log records without unnecessary overhead.
 */
struct NEX_INTERNAL PendingLog {
    LogMetadata         metadata;           // Metadata associated with the log record
    LogBuffer           buffer;             // Buffer containing the log message
    LastSystemError     lastSysError;       // Last system error code and message at the time of log creation
};

/**
 * @brief Submit a log data package to the logging system
 * @param log The pending log data package created by LogBuilder
 * @note 
 * This function is intended for internal use by the logging system and should not be called
 * directly by application code.
 * It is responsible for dispatching the log message to the appropriate log sinks based on
 * the provided metadata and message content.
 * The function takes ownership of the log data package, allowing for efficient transfer 
 * of log message data without unnecessary copying.
 * The logging system may perform additional processing, formatting, or filtering of the log message 
 * before it is output to the configured sinks.
 * The detailed implementation of this function will be provided by the higher-level logging system.
 */
NEX_INTERNAL void submit(PendingLog&& log) noexcept;

NEX_LAYER_NAMESPACE_END(logging)
