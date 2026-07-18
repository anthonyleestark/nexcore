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
 * @brief Get the last system error code from the operating system
 * @return The last system error code as a SystemErrorCode type
 */
NEX_INTERNAL SystemErrorCode getLastSystemErrorCode() noexcept;

/**
 * @brief Get the last system error message from the operating system
 * @param errorCode The system error code for which to retrieve the message
 * @return A LogString containing the last system error message
 */
NEX_INTERNAL LogString getLastSystemErrorMessage(SystemErrorCode errorCode) noexcept;

/**
 * @brief Restore the last system error code in the operating system
 * @param errorCode The system error code to restore
 */
NEX_INTERNAL void restoreLastSystemError(SystemErrorCode errorCode) noexcept;

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
    SystemErrorCode     lastSysErrorCode;   // Last system error code at the time of log creation
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
