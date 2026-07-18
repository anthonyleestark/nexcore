/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/location.h"
#include "nex/base/buffer.h"
#include "nex/base/string.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(logging)

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
 * @struct  LogMetadata
 * @brief   Represents metadata associated with a log record.
 * 
 * @details
 * The LogMetadata structure encapsulates the essential information for a log entry, including its
 * severity level, the category of the log message, and the source location where the log entry was generated.
 * This structure is used internally by the logging system to represent log entries before they are processed 
 * and output by the higher-level logger.
 */
struct LogMetadata {
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
     * @brief Location information for the log record
     * @details
     * The source location of the log record, which can be used to identify where in the code
     * the log entry was generated. This can be useful for debugging and tracing issues in the
     * application, as it provides context about the origin of the log message.
     * The location information includes the file name, line number, and function name where
     * the log record was created.
     */
    SourceLocation location;
};

/**
 * @brief Log buffer used for constructing log message by LogBuilder
 * @details
 * The LogBuffer is a small, inline-optimized dynamic buffer that is used by the LogBuilder class
 * to construct log messages. It is designed to hold log messages efficiently, using inline storage
 * for small messages and dynamically allocating memory for larger messages.
 * @note
 * The default buffer size is set to 512 bytes, which is typically sufficient for most log messages.
 * This size indicates the maximum inline storage capacity for log messages before dynamic allocation is required.
 * If a log message exceeds this size, dynamic memory allocation will be used to accommodate the message.
 */

inline constexpr usize DefaultLogBufferSize = 512;
using LogBuffer = SmallBuffer<nchar, DefaultLogBufferSize>;

// Forward declaration of LogBuilder class
class LogBuilder;

/**
 * @class LogStream
 * @brief Log stream used for constructing log message by LogBuilder
 * @details
 * The LogStream is a lightweight stream-like class that provides an interface for constructing
 * log messages in a convenient manner. It allows for appending various types of data to the log message
 * using the stream operator (<<), similar to standard C++ streams.
 * The LogStream internally uses a LogBuffer to store the constructed log message, and it provides
 * access to the underlying buffer for further processing or submission to the logging system.
 */
class NEX_API LogStream {
public:
    /**
     * @brief Append a signed integer value to the stream
     * @param value The signed integer value to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(int64 value) noexcept;

    /**
     * @brief Append an unsigned integer value to the stream
     * @param value The unsigned integer value to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(uint64 value) noexcept;

#if NEX_HAS_BUILTIN_INT128
    /**
     * @brief Append a 128-bit signed integer value to the stream
     * @param value The 128-bit signed integer value to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(int128 value) noexcept;

    /**
     * @brief Append a 128-bit unsigned integer value to the stream
     * @param value The 128-bit unsigned integer value to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(uint128 value) noexcept;
#endif

#if NEX_HAS_BUILTIN_FLOAT16
    /**
     * @brief Append a 16-bit floating-point value to the stream
     * @param value The 16-bit floating-point value to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(float16 value) noexcept;
#endif

    /**
     * @brief Append a floating-point value to the stream
     * @param value The floating-point value to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(float64 value) noexcept;

#if NEX_HAS_BUILTIN_FLOAT128
    /**
     * @brief Append a 128-bit floating-point value to the stream
     * @param value The 128-bit floating-point value to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(float128 value) noexcept;
#endif

    /**
     * @brief Append a boolean value to the stream
     * @param value The boolean value to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(bool value) noexcept;

    /**
     * @brief Append a character value to the stream
     * @param value The character value to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(nchar value) noexcept;

    /**
     * @brief Append a C-style string to the stream
     * @param str The C-style string to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(cstring str) noexcept;

    /**
     * @brief Append a string view to the stream
     * @param view The string view to append
     * @return A reference to the LogStream instance for chaining
     */
    LogStream& operator<<(NStringView view) noexcept;

    /**
     * @brief Get the underlying log buffer
     * @return A reference to the underlying LogBuffer instance
     */
    LogBuffer& buffer() noexcept { return buffer_; }

    /**
     * @brief Get the underlying log buffer (const version)
     * @return A const reference to the underlying LogBuffer instance
     */
    const LogBuffer& buffer() const noexcept { return buffer_; }

    /**
     * @brief Get the underlying log buffer as an r-value reference (for moving)
     * @return An r-value reference to the underlying LogBuffer instance
     */
    LogBuffer&& moveBuffer() noexcept { return NEX_MOVE(buffer_); }

    /**
     * @brief Get the underlying log buffer as an r-value reference (const version, for moving)
     * @return A const r-value reference to the underlying LogBuffer instance
     */
    const LogBuffer&& moveBuffer() const noexcept { return NEX_MOVE(buffer_); }

    /**
     * @brief Get the size of the underlying log buffer
     * @return The size of the underlying log buffer
     */
    constexpr usize size() const noexcept { return buffer_.size(); }

    /**
     * @brief Check if the underlying log buffer is empty
     * @return True if the underlying log buffer is empty, false otherwise
     */
    constexpr bool empty() const noexcept { return buffer_.empty(); }

private:
    // Private constructor to prevent direct instantiation
    LogStream() = default;

    // Only the LogBuilder class can create instances of LogStream
    friend class LogBuilder;

    // Internal buffer to hold the log stream data
    LogBuffer buffer_ = {};

public:
    // Disallow copy semantics
    NEX_DISALLOW_COPY(LogStream);

    // Default move semantics are allowed for LogStream
    NEX_DEFAULT_MOVE(LogStream);

private:
    /**
     * @brief Append a string data with specified pointer and length to the stream buffer
     * @param data Pointer to the string data
     * @param length Length of the string data
     * @return The number of characters (or bytes) appended to the stream buffer
     */
    usize append(cstring data, usize length) noexcept;
};

/**
 * @class LogBuilder
 * @brief Helper class for building log messages with a stream-like interface.
 * 
 * @details
 * The LogBuilder class provides a convenient way to construct log messages using a stream-like syntax.
 * It allows for appending multiple pieces of information to a log message before dispatching it to the logger.
 * The LogBuilder is typically used in conjunction with the Logger class, allowing for easy logging of messages
 * with various severity levels and categories.
 */
class NEX_API LogBuilder {
public:
    /**
     * @brief Construct a LogBuilder instance with specified metadata
     * @param level The log level of the message
     * @param location The source location of the log message
     * @param category An optional category for the log message
     */
    LogBuilder(LogLevel level,
               SourceLocation location,
               LogStringView category = {}) noexcept;

    /**
     * @brief Append a value to the log message using stream-like syntax
     * @tparam Type The type of the value to append
     * @param value The value to append to the log message
     * @return A reference to the LogBuilder instance for chaining
     */
    template <typename Type>
    LogBuilder& operator<<(const Type& value) noexcept {
        if (enabled_) {
            stream_ << value;
        }
        return *this;
    }

    /**
     * @brief Finalize the log message and dispatch it to the logger
     * @details
     * The destructor of the LogBuilder finalizes the log message and dispatches it to the logger.
     * This ensures that the log message is sent to the logger when the LogBuilder goes out of scope, 
     * allowing for convenient logging syntax using the stream operator.
     */
    ~LogBuilder() noexcept;

private:
    // Disable copy and move semantics
    NEX_DISALLOW_COPY_AND_MOVE(LogBuilder);

    // Metadata associated with the log record
    LogMetadata metadata_;

    // Stream for constructing the log message buffer
    LogStream stream_;

    // Flag indicating whether logging is enabled for this log builder
    bool enabled_;

    // Last system error code at the time of log creation,
    // using a type large enough to hold platform-specific error codes
    uint64 lastSysErrorCode_;
};

NEX_SUBNAMESPACE_END(logging)

// ===========================================================================
// Nex-ecosystem logging system public macros
// ===========================================================================

/**
 * @brief Macro for logging messages with a specified log level
 * @param level The log level of the message (e.g., Trace, Debug, Info, Warn, Error, Critical)
 * @details
 * This macro creates a LogBuilder instance with the specified log level and source location.
 * It allows for convenient logging syntax using the stream operator to append messages.
 * Example usage:
 *     NEX_LOG(Info) << "This is an informational message.";
 */
#define NEX_LOG(level) \
    NEX_PREPEND_LAYER_NAMESPACE(logging, LogBuilder)( \
        NEX_PREPEND_LAYER_NAMESPACE(logging, LogLevel)::level, \
        NEX_SOURCE_LOCATION, \
        NEX_PREPEND_LAYER_NAMESPACE(logging, LogStringView)())

/**
 * @brief Macro for logging messages with a specified log level and category
 * @param level The log level of the message (e.g., Trace, Debug, Info, Warn, Error, Critical)
 * @param category The category of the log message (e.g., "Network", "FileIO")
 * @details
 * This macro creates a LogBuilder instance with the specified log level, source location, and category.
 * It allows for convenient logging syntax using the stream operator to append messages.
 * Example usage:
 *     NEX_LOG_CATEGORY(Info, "Network") << "This is a network-related informational message.";
 */
#define NEX_LOG_CATEGORY(level, category) \
    NEX_PREPEND_LAYER_NAMESPACE(logging, LogBuilder)( \
        NEX_PREPEND_LAYER_NAMESPACE(logging, LogLevel)::level, \
        NEX_SOURCE_LOCATION, \
        NEX_PREPEND_LAYER_NAMESPACE(logging, LogStringView)(category))

NEX_NAMESPACE_END
