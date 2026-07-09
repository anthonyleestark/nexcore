/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/string.h"

NEX_NAMESPACE_BEGIN

/**
 * @enum LogLevel
 * @brief Enumeration of log levels for categorizing log messages by severity.
 * 
 * @details
 * Log levels are used to indicate the importance or severity of log messages.
 * This allows for filtering and controlling the verbosity of logging output.
 * Log levels are defined in increasing order of severity, with "Trace" being 
 * the most verbose and "Critical" being the most severe.
 * The "Off" level can be used to disable logging entirely.
 * Applications can configure their loggers to only output messages at or above 
 * a certain level, allowing for flexible logging based on the needs of the 
 * environment (e.g., development vs production).
 */
enum class LogLevel {
    Trace,      // Detailed information for diagnosing issues (most verbose)
    Debug,      // Information useful for debugging, but not needed in production
    Info,       // General informational messages about application operation
    Warn,       // Indications of potential issues or important events that are not errors
    Error,      // Indicates a failure in a specific operation, but the application can continue running
    Critical,   // Indicates a severe failure, potentially causing the application to terminate
    Off         // Disable logging (no log records will be output)
};

/**
 * @typedef  LogString/LogStringView
 * @brief    Type aliases represents string and string view types used for logging system.
 * 
 * @details
 * NexCore's logging system uses UTF-8 encoded strings for log messages to ensure compatibility and efficiency.
 * It is important to use the correct string types when working with the logging system to avoid encoding issues
 * and ensure proper handling of log messages.
 */

using LogString     = Utf8String;
using LogStringView = Utf8StringView;

/**
 * @struct  LogRecord
 * @brief   Represents a basic log record containing the log level, category, and message.
 * 
 * @details
 * The LogRecord structure encapsulates the essential information for a log entry, including its
 * severity level, the category of the log message, and the actual message content.
 * This structure is used internally by the logging system to represent log entries before
 * they are processed and output by log sinks.
 * 
 * @note
 * The LogRecord structure is designed to be lightweight and efficient, allowing for quick creation
 * and processing of log entries.
 */
struct LogRecord {
    /**
     * @brief Log level of the record
     * @details
     * The severity level of the log record, which can be used for filtering and formatting.
     * For example, a log record with LogLevel::Error would indicate an error condition that
     * should be addressed.
     * The log level can be used by log sinks to determine how to format the message (e.g., color coding)
     * or where to output it (e.g., only outputting warnings and above to a file).
     */
    LogLevel level;

    /**
     * @brief Log category
     * @details
     * The category of the log record, which can be used to group related log records together.
     * For example, log records related to networking could be categorized under "Network",
     * while records related to file I/O could be categorized under "FileIO".
     * This allows for more granular filtering and organization of log records based on
     * their source or context.
     */
    LogStringView category;

    /**
     * @brief Log message content
     * @details
     * The actual content of the log record, which can contain any information relevant to
     * the log entry, such as error details, diagnostic information, or general informational
     * messages about the application's operation.
     * The message is represented as a string view for efficient access without copying, allowing
     * for quick logging of messages without unnecessary overhead.
     */
    LogStringView message;
};

/**
 * @interface  LogSink
 * @brief      Interface for log sinks that handle log message output.
 * 
 * @details
 * Log sinks are responsible for writing log messages to their respective destinations,
 * such as files, consoles, or remote servers. Custom log sinks can be implemented
 * by inheriting from this interface and overriding the `log` method.
 * 
 * @note 
 * The Logger class may utilize multiple log sinks to output log messages to different destinations
 * simultaneously. For example, a logger could be configured to write to both a file and the console
 * by using multiple log sinks. Implementations of LogSink should ensure thread safety if they are
 * intended to be used in a multi-threaded environment.
 */
class NEX_HIDDEN_FROM_ABI LogSink {
public:
    /**
     * @brief Virtual destructor to allow proper cleanup of derived classes
     */
    virtual ~LogSink() = default;

    /**
     * @brief Write a log record to the sink
     * @details
     * This method is called by the Logger when a log message needs to be output.
     * The implementation of this method should handle the actual output of the log record 
     * to the sink's destination (e.g., writing to a file, outputting to the console,
     * sending to a remote server, etc.) based on the information/metadata contained
     * in the LogRecord parameter.
     */
    virtual void log(const LogRecord&) = 0;

    /**
     * @brief Flush the sink's output (if applicable)
     * @details
     * This method can be called to ensure that any buffered log messages are flushed
     * to the sink's destination.
     * This is particularly important for sinks that buffer output (e.g., file sinks) to ensure
     * that log messages are not lost in the event of a crash or unexpected termination.
     * It can also be used to ensure that log messages are output in a timely manner, especially
     * in scenarios where the application may be idle for extended periods.
     */
    virtual void flush() {}
};

/**
 * @interface  Logger
 * @brief      Interface for a logger that manages log sinks and handles log message dispatching.
 */
class NEX_HIDDEN_FROM_ABI Logger {
public:
    /**
     * @brief Virtual destructor to allow proper cleanup of derived classes
     */
    virtual ~Logger() = default;

    /**
     * @brief Output a log record
     * @details
     * This method is used to log a record with a specific severity level.
     * The log record will be processed by the logger's configured sinks, which will determine
     * how to output the record based on its level and other metadata.
     */
    virtual void log(const LogRecord&) = 0;

    /**
     * @brief Flush all log sinks to ensure that all buffered log records are output
     * @details
     * This method can be called to ensure that any buffered log records are flushed to their
     * respective sinks.
     * This is particularly important for sinks that buffer output (e.g., file sinks) to ensure
     * that log records are not lost in the event of a crash or unexpected termination.
     * It can also be used to ensure that log records are output in a timely manner, especially
     * in scenarios where the application may be idle for extended periods.
     */
    virtual void flush() = 0;

    /**
     * @brief Get the current logging level of the logger
     * @details
     * This method returns the current logging level of the logger, which determines the minimum
     * severity level of log messages that will be processed and output by the logger.
     */
    virtual LogLevel level() const noexcept = 0;

    /**
     * @brief Set the logging level of the logger
     * @details
     * This method sets the logging level of the logger, which determines the minimum severity level
     * of log messages that will be processed and output by the logger. Log messages with a severity
     * level below the set level will be ignored.
     */
    virtual void setLevel(LogLevel) noexcept = 0;
};

/**
 * @brief Get the default logger instance
 * @details
 * This function returns a pointer to the default logger instance, which can be used for logging
 * throughout the application. The default logger is typically configured with a set of log sinks
 * and a logging level, allowing for consistent logging behavior.
 */
Logger* defaultLogger();

/**
 * @brief Set the default logger instance
 * @details
 * This function sets the default logger instance, allowing for customization of the logging behavior
 * throughout the application. The provided logger will be used as the default logger for all logging
 * operations.
 */
void setDefaultLogger(Logger*);

/**
 * @interface  LogDispatcher
 * @brief      Interface for log dispatchers that manages log sinks and dispatches log records to them.
 * 
 * @details
 * The LogDispatcher class is responsible for managing a collection of log sinks and dispatching
 * log records to them. It provides methods for adding and removing log sinks, as well as for
 * dispatching log records to all registered sinks. The dispatcher ensures that log records are
 * sent to all sinks in a thread-safe manner, allowing for concurrent logging from multiple threads.
 */
class NEX_HIDDEN_FROM_ABI LogDispatcher {
public:
    /**
     * @brief Virtual destructor to allow proper cleanup of derived classes
     */
    virtual ~LogDispatcher() = default;

    /**
     * @brief Dispatch a log record to all registered sinks
     * @details
     * This method is used to dispatch a log record to all registered sinks, allowing them to process
     * the log record. The dispatcher is responsible for ensuring that log records are sent to
     * all sinks in a thread-safe manner, and that any necessary formatting or filtering is applied
     * based on the log level and other metadata.
     */
    virtual void dispatch(const LogRecord&) = 0;

    /**
     * @brief Flush all log sinks to ensure that all buffered log records are output
     * @details
     * This method can be called to ensure that any buffered log records are flushed to their
     * respective sinks. This is particularly important for sinks that buffer output (e.g., file sinks)
     * to ensure that log records are not lost in the event of a crash or unexpected termination.
     * It can also be used to ensure that log records are output in a timely manner, especially in
     * scenarios where the application may be idle for extended periods.
     */
    virtual void flush() = 0;
};

NEX_NAMESPACE_END


