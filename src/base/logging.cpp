/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/base/casts.h"
#include "src/base/logging_impl.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(logging)

// Construct a LogBuilder instance with specified metadata
LogBuilder::LogBuilder(LogLevel level, SourceLocation location, LogStringView category) noexcept
    : metadata_{level, category, location} {
}

LogBuilder::~LogBuilder() noexcept {
    // Finalize the log message and submit it to the logging system
    submitLog(NEX_MOVE(metadata_), NEX_MOVE(buffer_));
}

NEX_SUBNAMESPACE_END(logging)

NEX_NAMESPACE_END
