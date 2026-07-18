/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/base/casts.h"
#include "src/base/charconv.h"
#include "src/base/logging_internal.h"

#if NEX_PLATFORM_IS_WINDOWS
    #include <windows.h>
    #include <winerror.h>
#else
    #include <errno.h>
    #include <string.h>
#endif

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(logging)

// =================================================================================
// Nex-ecosystem logging system internal implementation
// =================================================================================

#if NEX_PLATFORM_FAMILY_IS_POSIX
// Referred from Chromium Embedded Framework (CEF) implementation

    #if NEX_LIBC_IS_GLIBC || defined(NEX_PLATFORM_NACL)
        #define NEX_USE_HISTORICAL_STRERRO_R 1
    #else
        #define NEX_USE_HISTORICAL_STRERRO_R 0
    #endif

    #if NEX_USE_HISTORICAL_STRERRO_R
        // glibc has two strerror_r functions: a historical GNU-specific one that
        // returns type char *, and a POSIX.1-2001 compliant one available since 2.3.4
        // that returns int. This wraps the GNU-specific one.
        static void NEX_MAYBE_UNUSED
        wrap_posix_strerror_r(char* (*strerror_r_ptr)(int, char*, size_t),
                              int err, char* buf, size_t len) {
            // GNU version.
            char* rc = (*strerror_r_ptr)(err, buf, len);
            if (rc != buf) {
                // glibc did not use buf and returned a static string instead. Copy it
                // into buf.
                buf[0] = '\0';
                strncat(buf, rc, len - 1);
            }
            // The GNU version never fails. Unknown errors get an "unknown error" message.
            // The result is always null terminated.
        }
    #endif  // ^^NEX_USE_HISTORICAL_STRERRO_R

    // Wrapper for strerror_r functions that implement the POSIX interface. POSIX
    // does not define the behaviour for some of the edge cases, so we wrap it to
    // guarantee that they are handled. This is compiled on all POSIX platforms, but
    // it will only be used on Linux if the POSIX strerror_r implementation is
    // being used (see below).
    static void NEX_MAYBE_UNUSED
    wrap_posix_strerror_r(int (*strerror_r_ptr)(int, char*, size_t),
                          int err, char* buf, size_t len) {
        int old_errno = errno;
        // Have to cast since otherwise we get an error if this is the GNU version
        // (but in such a scenario this function is never called). Sadly we can't use
        // C++-style casts because the appropriate one is reinterpret_cast but it's
        // considered illegal to reinterpret_cast a type to itself, so we get an
        // error in the opposite case.
        int result = (*strerror_r_ptr)(err, buf, len);
        if (result == 0) {
            // POSIX is vague about whether the string will be terminated, although
            // it indirectly implies that typically ERANGE will be returned, instead
            // of truncating the string. We play it safe by always terminating the
            // string explicitly.
            buf[len - 1] = '\0';
        } else {
            // Error. POSIX is vague about whether the return value is itself a system
            // error code or something else. On Linux currently it is -1 and errno is
            // set. On BSD-derived systems it is a system error and errno is unchanged.
            // We try and detect which case it is so as to put as much useful info as
            // we can into our message.
            int strerror_error;  // The error encountered in strerror
            int new_errno = errno;
            if (new_errno != old_errno) {
                // errno was changed, so probably the return value is just -1 or something
                // else that doesn't provide any info, and errno is the error.
                strerror_error = new_errno;
            } else {
                // Either the error from strerror_r was the same as the previous value, or
                // errno wasn't used. Assume the latter.
                strerror_error = result;
            }
            // snprintf truncates and always null-terminates.
            snprintf(buf, len, "Error %d while retrieving error %d", strerror_error, err);
        }
        errno = old_errno;
    }

    void safe_strerror_r(int err, char* buf, size_t len) {
        if (buf == NULL || len <= 0) {
            return;
        }
        // If using glibc (i.e., Linux), the compiler will automatically select the
        // appropriate overloaded function based on the function type of strerror_r.
        // The other one will be elided from the translation unit since both are
        // static.
        wrap_posix_strerror_r(&strerror_r, err, buf, len);
    }

    NString safe_strerror(int err) {
        const int buffer_size = 256;
        char buf[buffer_size];
        safe_strerror_r(err, buf, sizeof(buf));
        return NString(buf);
    }

#endif

// Get the last system error code from the operating system
SystemErrorCode getLastSystemErrorCode() noexcept {
#if NEX_PLATFORM_IS_WINDOWS
    return ::GetLastError();
#elif NEX_PLATFORM_FAMILY_IS_POSIX
    return errno;
#else
    #error Unsupported platform for retrieving system error code.
#endif
}

// Get the last system error message from the operating system
LogString getLastSystemErrorMessage(SystemErrorCode errorCode) noexcept {
#if NEX_PLATFORM_IS_WINDOWS
    LPSTR messageBuffer = nullptr;
    DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    usize size = FormatMessageA(
        flags,
        nullptr,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&messageBuffer),
        0,
        nullptr
    );

    LogString message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
#elif NEX_PLATFORM_FAMILY_IS_POSIX
    // TODO: May need to convert from narrow string to UTF-8 string
    return LogString(safe_strerror(errorCode));
#else
    #error Unsupported platform for retrieving system error message.
#endif
}

// Restore the last system error code in the operating system
void restoreLastSystemError(SystemErrorCode errorCode) noexcept {
#if NEX_PLATFORM_IS_WINDOWS
    ::SetLastError(errorCode);
#elif NEX_PLATFORM_FAMILY_IS_POSIX
    errno = errorCode;
#else
    #error Unsupported platform for restoring system error code.
#endif
}

// =================================================================================
// LogStream class implementation
// =================================================================================

// Append an integer value to the stream
LogStream& LogStream::operator<<(int64 value) noexcept {
    nchar buffer[32];
    usize length = charconv::formatInteger(buffer, sizeof(buffer), value);
    if (length > 0) {
        append(buffer, length);
    }
    return *this;
}

// Append an unsigned integer value to the stream
LogStream& LogStream::operator<<(uint64 value) noexcept {
    nchar buffer[32];
    usize length = charconv::formatUnsigned(buffer, sizeof(buffer), value);
    if (length > 0) {
        append(buffer, length);
    }
    return *this;
}

#if NEX_HAS_BUILTIN_INT128
// Append a 128-bit signed integer value to the stream
LogStream& LogStream::operator<<(int128 value) noexcept {
    nchar buffer[64];
    usize length = charconv::formatInteger(buffer, sizeof(buffer), value);
    if (length > 0) {
        append(buffer, length);
    }
    return *this;
}

// Append a 128-bit unsigned integer value to the stream
LogStream& LogStream::operator<<(uint128 value) noexcept {
    nchar buffer[64];
    usize length = charconv::formatUnsigned(buffer, sizeof(buffer), value);
    if (length > 0) {
        append(buffer, length);
    }
    return *this;
}
#endif

#if NEX_HAS_BUILTIN_FLOAT16
// Append a 16-bit floating-point value to the stream
LogStream& LogStream::operator<<(float16 value) noexcept {
    nchar buffer[32];
    usize length = charconv::formatFloating(buffer, sizeof(buffer), value);
    if (length > 0) {
        append(buffer, length);
    }
    return *this;
}
#endif

// Append a floating-point value to the stream
LogStream& LogStream::operator<<(float64 value) noexcept {
    nchar buffer[64];
    usize length = charconv::formatFloating(buffer, sizeof(buffer), value);
    if (length > 0) {
        append(buffer, length);
    }
    return *this;
}

// Append a 128-bit floating-point value to the stream
#if NEX_HAS_BUILTIN_FLOAT128
LogStream& LogStream::operator<<(float128 value) noexcept {
    nchar buffer[128];
    usize length = charconv::formatFloating(buffer, sizeof(buffer), value);
    if (length > 0) {
        append(buffer, length);
    }
    return *this;
}
#endif

// Append a boolean value to the stream
LogStream& LogStream::operator<<(bool value) noexcept {
    nchar buffer[6];
    usize length = charconv::formatBoolean(buffer, sizeof(buffer), value);
    if (length > 0) {
        append(buffer, length);
    }
    return *this;
}

// Append a character value to the stream
LogStream& LogStream::operator<<(nchar value) noexcept {
    append(&value, 1);
    return *this;
}

// Append a C-style string to the stream
LogStream& LogStream::operator<<(cstring str) noexcept {
    append(str, strlen(str));
    return *this;
}

// Append a string view to the stream
LogStream& LogStream::operator<<(NStringView view) noexcept {
    append(view.data(), view.length());
    return *this;
}

// Append a string data with specified pointer and length to the stream buffer
usize LogStream::append(cstring data, usize length) noexcept {
    if (!data) {
        // Use a placeholder to avoid undefined behavior
        data = "<null>";
        length = strlen(data);
    }
    auto nul = memchr(data, '\0', length);
    if (nul) {
        // Adjust the length to exclude the null terminator
        length = static_cast<cstring>(nul) - data;
    }
    if (length == 0) {
        return 0;
    }
    buffer_.append(data, length);
    return length;
}

// =================================================================================
// LogBuilder class implementation
// =================================================================================

// Construct a LogBuilder instance with specified metadata
LogBuilder::LogBuilder(LogLevel level, SourceLocation location, LogStringView category) noexcept
    : metadata_{level, category, location}, stream_{} {
    // Preserve the last system error code at the time of LogBuilder construction
    lastSysErrorCode_ = static_cast<uint64>(getLastSystemErrorCode());
    enabled_ = isEnabled(level, category);
}

// Finalize the log message and dispatch it to the logger
LogBuilder::~LogBuilder() noexcept {
    if (!enabled_) {
        // If logging is not enabled for this log builder, do nothing
        return;
    }

    if (stream_.empty()) {
        // If the log stream has no contents, skip submitting it to the logger
        return;
    }

    // Finalize the log package
    SystemErrorCode lastErrorCode = static_cast<SystemErrorCode>(lastSysErrorCode_);
    PendingLog pendingLog{NEX_MOVE(metadata_), NEX_MOVE(stream_.moveBuffer()), lastErrorCode};

    // Submit it to the logging system
    submit(NEX_MOVE(pendingLog));

    // Restore the last system error code to ensure
    // that logging does not interfere with the application's error state
    restoreLastSystemError(lastErrorCode);
}

NEX_SUBNAMESPACE_END(logging)

NEX_NAMESPACE_END
