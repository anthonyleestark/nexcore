/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/base/namespace.h"
#include "nex/base/logging.h"
#include "nex/base/assert_crash.h"
#include "src/base/logging_internal.h"
#include "src/infrastructure/logging/record.h"
#include "src/infrastructure/logging/queue.h"

#if NEX_PLATFORM_IS_WINDOWS
    #include "windows.h"
#elif NEX_PLATFORM_FAMILY_IS_POSIX
    #include <pthread.h>
    #include <unistd.h>
    #include <sys/syscall.h>
#endif

#if NEX_PLATFORM_FAMILY_IS_APPLE
    #include <mach/mach_time.h>
#endif

NEX_LAYER_NAMESPACE_BEGIN(logging)

// Retrieves the current high-resolution tick count.
TickCount getCurrentTickCount() noexcept {
#if NEX_PLATFORM_IS_WINDOWS
    return static_cast<TickCount>(::GetTickCount());
#elif NEX_PLATFORM_FAMILY_IS_APPLE
    return static_cast<TickCount>(mach_absolute_time());
#elif NEX_PLATFORM_FAMILY_IS_POSIX
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<TickCount>(ts.tv_sec) * 1000000 + static_cast<TickCount>(ts.tv_nsec) / 1000;
#else
    #error Unsupported platform for retrieving high-resolution tick count.
#endif
}

// Retrieves the current thread's ID.
ThreadId getCurrentThreadId() noexcept {
#if NEX_PLATFORM_IS_WINDOWS
    return static_cast<ThreadId>(::GetCurrentThreadId());
#elif NEX_PLATFORM_FAMILY_IS_APPLE
    uint64 tid;
    if (pthread_threadid_np(nullptr, &tid) == 0) {
        return static_cast<ThreadId>(tid);
    }
#elif NEX_PLATFORM_IS_LINUX
    return static_cast<ThreadId>(syscall(SYS_gettid));
#else
    #error Unsupported platform for retrieving thread ID.
#endif
}

// Retrieves the current process's ID.
ProcessId getCurrentProcessId() noexcept {
#if NEX_PLATFORM_IS_WINDOWS
    return static_cast<ProcessId>(::GetCurrentProcessId());
#elif NEX_PLATFORM_FAMILY_IS_POSIX
    return static_cast<ProcessId>(getpid());
#else
    #error Unsupported platform for retrieving process ID.
#endif
}

// Submits an accumulated log package to the core logging system.
void submit(PendingLog&& log) noexcept {
    // Create a LogRecord from the PendingLog payload
    LogRecord record = createLogRecord(NEX_FORWARD<PendingLog>(log));
    auto currentSeqNum = record.sequenceNumber.value();

    // Note: The actual logging queue instance should be managed by the logging backend.
    // TODO: For demonstration purposes, we will assume a global logging queue instance exists.
    extern LogQueue g_loggingQueue; // Assume this is defined elsewhere in the logging backend

    // Get the current size of the logging queue before enqueuing the new log record
    usize currentQueueSize = g_loggingQueue.size();

    // Enqueue the log record into the logging queue
    usize newQueueSize = g_loggingQueue.enqueue(NEX_MOVE(record));

    // Debug mode: Assert if the queue size has unexpectedly decreased after enqueuing
    NEX_ASSERT_MSG(newQueueSize >= currentQueueSize,
        "Error: Logging queue size decreased after enqueue operation."
        "This indicates a potential issue with the queue management or overflow policy.");

    // Update the last submitted sequence number
    lastSubmittedSequenceNumber.store(currentSeqNum, NEX_STD memory_order_release);
}

// Creates a log record from a pending log payload.
LogRecord createLogRecord(PendingLog&& payload) {
    LogRecord record;
    record.payload          = NEX_FORWARD<PendingLog>(payload);
    record.timestamp        = Timestamp::now();
    record.tickCount        = getCurrentTickCount();
    record.threadId         = getCurrentThreadId();
    record.processId        = getCurrentProcessId();
    record.sequenceNumber   = SequenceNumber::next();
    return record;
}

// Processes a log record after being dequeued from the logging queue.
Result<void> process(LogRecord&& record) noexcept {
    // Process the log record (e.g., format and route to sinks)
    // TODO: For demonstration purposes, we'll assume processing is always successful.
    return ok();
}

NEX_LAYER_NAMESPACE_END(logging)
