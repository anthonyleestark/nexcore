/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/types.h"
#include "nex/base/logging.h"
#include "nex/base/wrappers.h"  // For using SharedPtr & Atomic
#include "nex/core/thread.h"
#include "src/infrastructure/logging/record.h"
#include "src/infrastructure/logging/queue.h"

NEX_LAYER_NAMESPACE_BEGIN(logging)

/**
 * @class  LogWorker
 * @brief  Worker thread that processes log records from a LogQueue.
 * 
 * @details
 * The LogWorker class encapsulates a dedicated thread that continuously dequeues log records from a shared
 * LogQueue and processes them. It provides methods to start, stop, and join the worker thread, ensuring
 * that log records are handled efficiently in a multi-threaded environment.
 */
class NEX_INTERNAL LogWorker {
public:
    // Constructor that initializes the LogWorker with a shared pointer to a LogQueue
    explicit LogWorker(SharedPtr<LogQueue> logQueue) 
        : workerThread_(), logQueue_(NEX_MOVE(logQueue)), lastProcessedSequenceNumber_{} {}

    // Destructor that ensures the worker thread is properly stopped and joined
    ~LogWorker();

    // Starts the logging worker thread
    void start();

    // Flushes any remaining log records in the queue
    void flush();

    // Requests the logging worker thread to stop gracefully
    void requestStop();

    // Joins the logging worker thread, blocking until it finishes execution
    void join();

private:
    // Disable copy and move semantics
    NEX_DISALLOW_COPY_AND_MOVE(LogWorker);

    // The thread that runs the logging worker loop
    Thread workerThread_;

    // Shared pointer to the current logging queue
    SharedPtr<LogQueue> logQueue_;

    // The last processed sequence number to track the order of log records
    Atomic<SequenceNumber::value_type> lastProcessedSequenceNumber_;
};

NEX_LAYER_NAMESPACE_END(logging)
