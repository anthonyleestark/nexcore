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
 * @brief Severity levels for categorizing and filtering log records.
 * 
 * @details
 * Defines the runtime priority of a log entry in increasing order of severity. 
 * Framework configurations use these levels to filter out verbosity below a specified 
 * threshold (e.g., suppressing Debug output in production environments).
 */
enum class LogLevel {
    Trace,      // Ultra-verbose diagnostics for deep system tracing.
    Debug,      // Operational context useful for debugging; typically disabled in production.
    Info,       // High-level informational messages tracking normal application flow.
    Warn,       // Indications of abnormal or unexpected events that do not halt execution.
    Error,      // Runtime failures affecting specific operations where the application recovers.
    Critical,   // Severe, unrecoverable system failures that may precede termination.
    Off         // Sentinel value used to completely disable logging output.
};

/**
 * @typedef LogString/LogStringView
 * @brief   Type aliases representing the string types utilized across the logging system.
 * 
 * @details
 * Enforces UTF-8 encoding across all logging boundaries to guarantee consistent character presentation
 * and optimal serialization efficiency.
 */

using LogString     = Utf8String;
using LogStringView = Utf8StringView;

/**
 * @struct LogMetadata
 * @brief  Encapsulates the transient context associated with an individual log entry.
 * 
 * @details
 * Packs essential routing and categorization properties. This data is bundled alongside raw message payloads
 * before being evaluated and promoted to a formal LogRecord.
 */
struct LogMetadata {
    /**
     * @brief Severity priority of the event. Used for routing and console color-coding.
     * @details 
     * Used by the framework to drop or keep records based on runtime thresholds,
     * and by console sinks to apply level-specific color-coding (e.g., Warning vs. Error).
     */
    LogLevel level;

    /**
     * @brief Subsystem identifier or functional group (e.g., "Network", "Storage").
     * @details
     * Allows for granular routing control, letting developers filter or mute logs on a module-by-module basis
     * rather than globally.
     */
    LogStringView category;

    /**
     * @brief Source file name, line number, and function signature where the log originated.
     * @details
     * Provides detailed information about the origin of the log entry, including the file name, line number,
     * and function signature. This information is crucial for debugging and tracing the execution flow
     * of the application and across the framework.
     */
    SourceLocation location;
};

/**
 * @brief Inline capacity boundary for unformatted log messages.
 * 
 * @details
 * Specifies the maximum buffer size (in characters) allocated directly on the stack to eliminate
 * dynamic memory allocations for typical log lengths. Messages exceeding this threshold will
 * automatically fallback to heap allocation.
 */
inline constexpr usize DefaultLogBufferSize = 512;

/**
 * @typedef LogBuffer
 * @brief Stack-optimized buffer alias used for temporary log message storage.
 */
using LogBuffer = SmallBuffer<nchar, DefaultLogBufferSize>;

// Forward declaration of LogBuilder class
class LogBuilder;

/**
 * @class LogStream
 * @brief Provides the stream insertion interface for LogBuilder to accumulate log data.
 * 
 * @details
 * LogStream serves as a lightweight, stream-oriented adapter that bridges the LogBuilder interface with a LogBuffer.
 * By exposing standard C++ stream insertion operators (<<), it enables sequential appending of various data types
 * into the underlying buffer without exposing the storage layer directly.
 * 
 * Once data accumulation is complete, the stream provides structured access to its LogBuffer for subsequent
 * packaging and dispatch to the central logging system.
 */
class NEX_API LogStream {
public:
    /**
     * @brief Appends a signed 64-bit integer to the stream buffer.
     * @param value Integer value to append.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(int64 value) noexcept;

    /**
     * @brief Appends an unsigned 64-bit integer to the stream buffer.
     * @param value Integer value to append.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(uint64 value) noexcept;

#if NEX_HAS_BUILTIN_INT128
    /**
     * @brief Appends a 128-bit signed integer to the stream buffer.
     * @param value Integer value to append.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(int128 value) noexcept;

    /**
     * @brief Appends a 128-bit unsigned integer to the stream buffer.
     * @param value Integer value to append.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(uint128 value) noexcept;
#endif

#if NEX_HAS_BUILTIN_FLOAT16
    /**
     * @brief Appends a 16-bit floating-point value to the stream buffer.
     * @param value Floating-point value to append.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(float16 value) noexcept;
#endif

    /**
     * @brief Appends a 64-bit floating-point value to the stream buffer.
     * @param value Floating-point value to append.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(float64 value) noexcept;

#if NEX_HAS_BUILTIN_FLOAT128
    /**
     * @brief Appends a 128-bit floating-point value to the stream buffer.
     * @param value Floating-point value to append.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(float128 value) noexcept;
#endif

    /**
     * @brief Appends a boolean value ("true"/"false") to the stream buffer.
     * @param value Boolean value to append.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(bool value) noexcept;

    /**
     * @brief Appends a single native character to the stream buffer.
     * @param value Character to append.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(nchar value) noexcept;

    /**
     * @brief Appends a null-terminated C-style string to the stream buffer.
     * @param str Pointer to the character array.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(cstring str) noexcept;

    /**
     * @brief Appends a non-owning string view string to the stream buffer.
     * @param view String view tracking the character slice.
     * @return Reference to the stream instance for operational chaining.
     */
    LogStream& operator<<(NStringView view) noexcept;

    /**
     * @brief Provides direct mutable access to the internal storage buffer.
     */
    LogBuffer& buffer() noexcept { return buffer_; }

    /**
     * @brief Provides read-only access to the internal storage buffer.
     */
    const LogBuffer& buffer() const noexcept { return buffer_; }

    /**
     * @brief Extracts ownership of the underlying data by returning an rvalue reference.
     */
    LogBuffer&& moveBuffer() noexcept { return NEX_MOVE(buffer_); }

    /**
     * @brief Extracts ownership of the underlying data by returning a const rvalue reference.
     */
    const LogBuffer&& moveBuffer() const noexcept { return NEX_MOVE(buffer_); }

    /**
     * @brief Returns the total number of elements currently stored within the buffer.
     */
    constexpr usize size() const noexcept { return buffer_.size(); }

    /**
     * @brief Evaluates whether the stream buffer contains no active data.
     * @return true if the buffer is empty; false otherwise.
     */
    constexpr bool empty() const noexcept { return buffer_.empty(); }

private:
    /**
     * @brief Restricts object lifecycle management explicitly to the LogBuilder hierarchy.
     * @note  The LogStream should not be instantiated or managed independently outside of the LogBuilder context.
     */
    LogStream() = default;
    friend class LogBuilder;

    /**
     * @brief Internal staging buffer for raw unformatted log text fragments
     */
    LogBuffer buffer_ = {};

public:
    // Disallow copy semantics
    NEX_DISALLOW_COPY(LogStream);

    // Default move semantics are allowed for LogStream
    NEX_DEFAULT_MOVE(LogStream);

private:
    /**
     * @brief Appends raw string data with a specified pointer and length to the stream buffer.
     * @param data Pointer to the character data.
     * @param length Length of the string data.
     * @return The number of characters successfully appended.
     */
    usize append(cstring data, usize length) noexcept;
};

/**
 * @class LogBuilder
 * @brief Constructs a lightweight, unformatted log package and forwards it to the central logging engine.
 * 
 * @details
 * The LogBuilder provides a stream-like interface for efficiently gathering log data. It accumulates
 * metadata and raw message fragments into an unformatted buffer without executing immediate, expensive
 * string formatting operations.
 * 
 * Upon destruction, the builder hands this lightweight package off to the main logging system.
 * The central engine then processes the buffer, generates a fully detailed LogRecord, and routes it
 * to the active LogSinks.
 * 
 * @note This class is optimized for quick data accumulation to minimize the performance footprint at
 * the actual log-site.
 */
class NEX_API LogBuilder {
public:
    /**
     * @brief Constructs a LogBuilder instance with specified metadata.
     * @param level The severity priority of the message.
     * @param location The source location where the log originated.
     * @param category Optional sub-component or functional group identifier.
     */
    LogBuilder(LogLevel level,
               SourceLocation location,
               LogStringView category = {}) noexcept;

    /**
     * @brief Appends a value to the log message using stream insertion syntax.
     * @tparam Type The type of the value to append.
     * @param value The data value to insert.
     * @return Reference to the LogBuilder instance for operational chaining.
     */
    template <typename Type>
    LogBuilder& operator<<(const Type& value) noexcept {
        if (enabled_) {
            stream_ << value;
        }
        return *this;
    }

    /**
     * @brief Destructor finalizes the log package and forwards it to the central logging engine.
     */
    ~LogBuilder() noexcept;

private:
    // Disable copy and move semantics
    NEX_DISALLOW_COPY_AND_MOVE(LogBuilder);

    // Context properties associated with the entry
    LogMetadata metadata_;

    // Stream adapter constructing the unformatted log text
    LogStream stream_;

    // Flag indicating whether logging is enabled for this log builder
    // This flag is immutable and is captured at construction time
    bool enabled_;

    // Scoped system error code captured at the moment of creation,
    // using a type large enough to hold any platform-specific error codes
    uint64 lastSysErrorCode_;
};

NEX_SUBNAMESPACE_END(logging)

// ===========================================================================
// Nex-ecosystem logging system public macros
// ===========================================================================

/**
 * @def NEX_LOG
 * @brief Instantiates a scoped LogBuilder with a specified log level.
 * @param level The log severity (e.g., Trace, Debug, Info, Warn, Error, Critical)
 * 
 * Example usage:
 * @code
 * NEX_LOG(Info) << "Application started successfully.";
 * @endcode
 */
#define NEX_LOG(level) \
    NEX_PREPEND_LAYER_NAMESPACE(logging, LogBuilder)( \
        NEX_PREPEND_LAYER_NAMESPACE(logging, LogLevel)::level, \
        NEX_SOURCE_LOCATION, \
        NEX_PREPEND_LAYER_NAMESPACE(logging, LogStringView)())

/**
 * @def NEX_LOG_CATEGORY
 * @brief Instantiates a scoped LogBuilder with a specified log level and subsystem category.
 * @param level The log severity (e.g., Info, Warn, Error).
 * @param category A string literal identifying the subsystem (e.g., "Network", "Graphics", "FileIO").
 * 
 * Example usage:
 * @code
 * NEX_LOG_CATEGORY(Error, "Network") << "Connection timed out.";
 * @endcode
 */
#define NEX_LOG_CATEGORY(level, category) \
    NEX_PREPEND_LAYER_NAMESPACE(logging, LogBuilder)( \
        NEX_PREPEND_LAYER_NAMESPACE(logging, LogLevel)::level, \
        NEX_SOURCE_LOCATION, \
        NEX_PREPEND_LAYER_NAMESPACE(logging, LogStringView)(category))

NEX_NAMESPACE_END
