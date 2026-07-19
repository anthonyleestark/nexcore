/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/platform.h"
#include "nex/base/logging.h"

NEX_LAYER_NAMESPACE_BEGIN(logging)

#if NEX_PLATFORM_IS_WINDOWS
    // Windows error codes are natively represented as unsigned long integers (DWORD)
    using SystemErrorCode = ulong;
#else
    // Unix-like systems natively represent error codes via integer types (errno)
    using SystemErrorCode = int32;
#endif

/**
 * @brief Retrieves the current thread's last recorded operating system error code.
 * @return The active system error code.
 */
NEX_INTERNAL SystemErrorCode getLastSystemErrorCode() noexcept;

/**
 * @brief Translates a given system error code into a human-readable string.
 * @param errorCode The operating system error code to resolve.
 * @return A `LogString` containing the localized system error message description.
 */
NEX_INTERNAL LogString getLastSystemErrorMessage(SystemErrorCode errorCode) noexcept;

/**
 * @brief Restores a previously captured error code into the operating system's active error state.
 * @param errorCode The system error code to re-apply to the current thread context.
 */
NEX_INTERNAL void restoreLastSystemError(SystemErrorCode errorCode) noexcept;

/**
 * @brief Evaluates whether a log entry should be processed based on its level and category
 * @param level The severity priority of the log message (e.g., Trace, Debug, Info, Warn, Error, Critical)
 * @param category Optional subsystem functional group identifier (e.g., "Network", "Storage")
 * @return true if the message meets active runtime thresholds, false if it should be discarded
 * @note The detailed implementation of this function will be provided by the higher-level logging system.
 */
NEX_INTERNAL bool isEnabled(LogLevel level, LogStringView category = {}) noexcept;

/**
 * @struct PendingLog
 * @brief Intermediate data package representing an unformatted log entry awaiting dispatch.
 * 
 * @details
 * Bundles the transient execution state—metadata, raw message segments, and system error diagnostics
 * into a single container. This allows `LogBuilder` to capture site-specific context rapidly[cite: 2] 
 * and hand it off to the engine without executing immediate formatting overhead.
 * 
 * @details
 * Encapsulates the metadata, message buffer, and last system error information associated with a log record
 * that is ready to be submitted to the logging system.
 * This allows `LogBuilder` to capture site-specific context rapidly and hand it off to the engine without
 * executing immediate formatting overhead.
 */
struct NEX_INTERNAL PendingLog {
    LogMetadata         metadata;           // Metadata associated with the log record
    LogBuffer           buffer;             // Buffer containing the log message
    SystemErrorCode     lastSysErrorCode;   // Last system error code at the time of log creation
};

/**
 * @brief Submits an accumulated log package to the core logging system.
 * @param log An rvalue reference to the pending log package instance, transferring buffer ownership.
 * @note 
 * Restricted to internal framework orchestration; this should not be called directly by application code.
 * The function takes ownership of the log package, allowing for efficient transfer of log message data
 * without unnecessary copying.
 * The central logging system consumes the package, performs additional processing, formatting, or filtering
 * before routing the structured record to configured / active log sinks.
 * The detailed implementation of this function will be provided by the higher-level logging system.
 */
NEX_INTERNAL void submit(PendingLog&& log) noexcept;

NEX_LAYER_NAMESPACE_END(logging)
