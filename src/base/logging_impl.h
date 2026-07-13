/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/logging.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(logging)

/**
 * @brief Submit a log data package to the logging system
 * @param metadata The metadata associated with the log data
 * @param buffer The buffer containing the log message constructed by LogBuilder
 * @note 
 * This function is intended for internal use by the logging system and should not be called
 * directly by application code.
 * It is responsible for dispatching the log message to the appropriate log sinks based on
 * the provided metadata and message content.
 * The function takes ownership of the metadata and buffer, allowing for efficient transfer 
 * of log message data without unnecessary copying.
 * The logging system may perform additional processing, formatting, or filtering of the log message 
 * before it is output to the configured sinks.
 */
NEX_INTERNAL void submitLog(LogMetadata&& metadata, LogBuffer&& buffer) noexcept;

NEX_SUBNAMESPACE_END(logging)

NEX_NAMESPACE_END