/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/base/casts.h"
#include "src/base/logging_internal.h"

#if NEX_PLATFORM_IS_WINDOWS
    #include <windows.h>
    #include <winerror.h>
#else
    #include <cerrno>
    #include <string.h>   // For strerror() without using namespace std
#endif

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(logging)

// Get the last system error code from the operating system
static SystemErrorCode getLastSystemErrorCode() noexcept {
#if NEX_PLATFORM_IS_WINDOWS
    return ::GetLastError();
#elif NEX_PLATFORM_FAMILY_IS_POSIX
    return errno;
#else
    #error Unsupported platform for retrieving system error code.
#endif
}

// Get the last system error message from the operating system
static LogString getLastSystemErrorMessage(SystemErrorCode errorCode) noexcept {
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
    return LogString(::strerror(errorCode));
#else
    #error Unsupported platform for retrieving system error message.
#endif
}

// Static instance to hold the last system error information at the time of log creation
static LastSystemError lastSysError;

// Construct a LogBuilder instance with specified metadata
LogBuilder::LogBuilder(LogLevel level, SourceLocation location, LogStringView category) noexcept
    : metadata_{level, category, location}, buffer_{} {
    // Preserve the last system error code and message at the time of LogBuilder construction
    lastSysError.lastErrorCode_ = getLastSystemErrorCode();
    lastSysError.lastErrorMessage_ = getLastSystemErrorMessage(lastSysError.lastErrorCode_);
}

// Finalize the log message and dispatch it to the logger
LogBuilder::~LogBuilder() noexcept {
    // Finalize the log package and submit it to the logging system
    PendingLog pendingLog{NEX_MOVE(metadata_), NEX_MOVE(buffer_), NEX_MOVE(lastSysError)};
    submit(NEX_MOVE(pendingLog));
}

NEX_SUBNAMESPACE_END(logging)

NEX_NAMESPACE_END
