/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/wrappers.h"
#include "nex/core/thread.h"

NEX_NAMESPACE_BEGIN

/**
 * @class Executor
 * @brief Abstract base class for creating custom executors.
 * 
 * @details
 * Executor provides a structured way to define the behavior of a thread. You can derive from this class 
 * and implement the execute() method to define how tasks are executed in the context of the thread.
 */
class Executor {
public:
    // Virtual destructor to allow proper cleanup of derived classes
    virtual ~Executor() = default;

    // Execute a task in the context of the thread
    virtual void execute(Function<void()> task) = 0;
};

/**
 * @class InlineExecutor
 * @brief An executor that runs tasks immediately in the calling thread.
 * 
 * @details
 * InlineExecutor is a simple implementation of the Executor interface that executes tasks immediately in 
 * the calling thread. This can be useful for testing or when you want to execute tasks synchronously without 
 * the overhead of thread management. 
 * 
 * @note
 * Using InlineExecutor will block the calling thread until the task is completed, so it should be used with caution 
 * in performance-critical or UI code.
 */
class InlineExecutor : public Executor {
public:
    void execute(Function<void()> task) override {
        task();
    }
};

/**
 * @class ThreadExecutor
 * @brief Manages a dedicated background thread for sequential task execution.
 * 
 * @details
 * ThreadExecutor is a basic implementation of the Executor interface that manages a single thread and 
 * a queue of tasks. It allows you to start a thread that continuously processes tasks from the queue until stopped.
 * You can submit tasks to the executor using the execute() method, and the thread will execute them in the order 
 * they were received. The executor can be stopped gracefully, allowing the thread to finish processing any 
 * remaining tasks before exiting.
 * 
 * @note
 * Tasks are processed in FIFO order. 
 * Ensure stop() is called to prevent resource leaks and allow pending tasks to complete.
 * 
 * @see Executor for the base class interface.
 * @see Thread for the underlying thread management.
 */
class NEX_EXPORT ThreadExecutor : public Executor {
public:
    // Constructor and destructor
    ThreadExecutor();
    ~ThreadExecutor();

    // Start the executor's thread and begin processing tasks from the queue.
    void start();
    
    // Request the executor to stop processing tasks and exit the thread gracefully.
    void stop();

    // Execute a task in the context of the thread. 
    // Tasks are added to a queue and processed sequentially.
    void execute(Function<void()> task) override;

private:
    // Hidden implementation details using the Pimpl idiom
    struct Impl;
    UniquePtr<Impl> impl_;
};

/**
 * @class ThreadPool
 * @brief A fixed-size pool of threads for concurrent task processing.
 * 
 * @details
 * ThreadPool manages a pool of threads that can execute tasks concurrently. You can specify the number of threads
 * in the pool, and the pool will distribute tasks among a set of available worker threads to maximize CPU utilization. 
 * Tasks are submitted to the pool using the execute() method, and the pool will handle the scheduling and execution 
 * of those tasks across the threads. Ideal for compute-intensive or parallelizable workloads.
 * 
 * @see Executor for the base class interface.
 * @see Thread for the underlying thread management.
 */
class NEX_EXPORT ThreadPool : public Executor {
public:
    // Constructor that initializes the thread pool with a specified number of threads.
    explicit ThreadPool(uint32 threadCount = Thread::hardwareConcurrency());

    // Start all threads in the pool and begin processing tasks from the shared queue.
    void start();

    // Request all threads in the pool to stop processing tasks and exit gracefully.
    void stop();

    // Execute a task in the context of the thread pool.
    // Tasks are added to a shared queue and processed by available threads.
    void execute(Function<void()> task) override;

    // Get the number of threads in the pool.
    uint32 threadCount() const noexcept;

private:
    // Hidden implementation details using the Pimpl idiom
    struct Impl;
    UniquePtr<Impl> impl_;
};

NEX_NAMESPACE_END
