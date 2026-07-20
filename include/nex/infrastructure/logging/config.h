/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/types.h"
#include "nex/base/logging.h"

NEX_LAYER_NAMESPACE_BEGIN(logging)

/**
 * @enum LoggingMode
 * @brief Defines the operational mode of the logging system.
 */
enum class LoggingMode {
    Synchronous,    // Log records are processed immediately in the calling thread.
    Asynchronous    // Log records are queued and processed by a dedicated worker thread.
};

/**
 * @brief Minimum capacity for the logging queue.
 * @details
 * This constant defines the minimum number of log records that the logging queue can hold.
 * It is used to ensure that the queue has a reasonable size for efficient log management.
 */
inline constexpr usize MinLogQueueCapacity = 16;

/**
 * @brief Default capacity for the logging queue.
 * @details
 * This constant defines the default number of log records that the logging queue can hold.
 * It is used when no specific capacity is provided during the construction of the LogQueue.
 */
inline constexpr usize DefaultLogQueueCapacity = 1024;

/**
 * @brief Maximum capacity for the logging queue.
 * @details
 * This constant defines the maximum number of log records that the logging queue can hold.
 * It is used to prevent excessive memory usage and ensure that the logging system remains responsive.
 */
inline constexpr usize MaxLogQueueCapacity = 65536;

/**
 * @enum QueueOverflowPolicy
 * @brief Defines the behavior of the logging queue when it reaches its maximum capacity.
 */
enum class QueueOverflowPolicy {
    Block,          // Block the producer thread until space is available in the queue.
    DropNewest,     // Drop the newest log entry when the queue is full, keeping the oldest entries.
    DropOldest,     // Drop the oldest log entry when the queue is full, keeping the newest entries.
    Grow            // Allow the queue to grow dynamically beyond its initial capacity.
};

/**
 * @struct GlobalConfig
 * @brief Represents the global configuration settings for the logging system.
 */
struct GlobalConfig {
    // Operational mode of the logging system (synchronous or asynchronous).
    LoggingMode loggingMode                 = LoggingMode::Asynchronous;

    // Maximum number of log records the queue can hold before applying the overflow policy.
    usize logQueueCapacity                  = DefaultLogQueueCapacity;

    // Policy to apply when the log queue reaches its maximum capacity.
    QueueOverflowPolicy queueOverflowPolicy = QueueOverflowPolicy::DropOldest;

    // Automatically flush log records to the output sink after processing.
    bool autoFlush                          = true;
};

/**
 * @struct LoggerConfig
 * @brief Represents the configuration settings for an individual logger instance.
 */
struct LoggerConfig {
    // Minimum log level to process; messages below this level will be ignored.
    LogLevel minimumLevel = LogLevel::Trace;

    // Indicates whether the logger is enabled.
    bool enabled = true;

    // Indicates whether log messages should be propagated to parent loggers.
    bool propagate = true;
};

NEX_LAYER_NAMESPACE_END(logging)
