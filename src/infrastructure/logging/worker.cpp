/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/text/format.h"
#include "nex/core/chrono/chrono_base.h"
#include "src/infrastructure/logging/worker.h"

NEX_LAYER_NAMESPACE_BEGIN(logging)

// Destructor that ensures the worker thread is properly stopped and joined
LogWorker::~LogWorker() {
    requestStop();
    join();
    logQueue_.reset();
}

// Starts the logging worker thread
void LogWorker::start() {
    NEX_ASSERT_MSG(!workerThread_.isRunning(), "Error: LogWorker thread is already running.");
    NEX_ASSERT_MSG(logQueue_ != nullptr, "Error: LogWorker has been initialized with an invalid LogQueue.");

    // Starts the thread
    workerThread_.start([this]() {
        LogRecord record;
        do {
            if (logQueue_->dequeue(record)) {
                // Process the log record
                auto currentSeqNum = record.sequenceNumber.value();
                auto result = process(NEX_MOVE(record));
                if (!result) {
                    // Handle the error (e.g., log it, assert, etc.)
                    String msg = format::format("Error: Failed to process log record: {}", result.error().message);
                    NEX_ASSERT_MSG(false, msg.toUtf8().valueOr("Unknown error"));
                } else {
                    // Update the last processed sequence number
                    lastProcessedSequenceNumber_.store(currentSeqNum, NEX_STD memory_order_release);
                }
            } else {
                // If the queue is empty, we can sleep for a short duration to avoid busy waiting
                NEX_STD this_thread::sleep_for(chrono::Milliseconds(10));
            }
        } while (!workerThread_.stopRequested());
    });
}

// Flushes any remaining log records in the queue
void LogWorker::flush() {
    NEX_ASSERT_MSG(logQueue_ != nullptr, "Error: LogWorker has been initialized with an invalid LogQueue.");
}

// Requests the logging worker thread to stop gracefully
void LogWorker::requestStop() {
    workerThread_.requestStop();
}

// Joins the logging worker thread, blocking until it finishes execution
void LogWorker::join() {
    workerThread_.join();
}

NEX_LAYER_NAMESPACE_END(logging)
