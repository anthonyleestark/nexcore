/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/executor.h"
#include "nex/base/primitive.h"
#include "nex/base/adaptors.h"
#include "nex/base/linear.h"

NEX_NAMESPACE_BEGIN

// ======================================================================
// ThreadExecutor Implementations
// ======================================================================

// Implementation details of ThreadExecutor
struct ThreadExecutor::Impl {
    Thread                  thread_;        // The thread that will execute tasks
    Queue<Function<void()>> taskQueue_;     // Queue of tasks to be executed
    Mutex                   queueMutex_;    // Mutex to protect access to the task queue
};

// Default constructor
ThreadExecutor::ThreadExecutor()
    : impl_(NEX_STD make_unique<Impl>()) {
};

// Destructor
ThreadExecutor::~ThreadExecutor() {
    stop(); // Ensure the thread is stopped to prevent resource leaks
}

// Start the executor's thread and begin processing tasks from the queue.
void ThreadExecutor::start() {
    // Start a thread that continuously processes tasks from the queue until stopped
    impl_->thread_.start([this]() {
        while (true) {
            // Check for stop request
            if (impl_->thread_.stopRequested()) {
                break; // Exit the loop to stop the thread
            }
            // Process tasks from the queue
            Function<void()> task;
            {
                LockGuard lock(impl_->queueMutex_);     // Ensure thread-safe access to the queue
                if (!impl_->taskQueue_.empty()) {
                    task = NEX_STD move(impl_->taskQueue_.front());
                    impl_->taskQueue_.pop();
                }
            }
            if (task) {
                task(); // Execute the task
            } else {
                // No tasks available, yield to prevent busy-waiting
                NEX_STD this_thread::yield();
            }
        }
    });
}

// Request the executor to stop processing tasks and exit the thread gracefully.
void ThreadExecutor::stop() {
    impl_->thread_.requestStop();      // Signal the thread to stop
    impl_->thread_.join();             // Wait for the thread to finish execution
}

// Execute a task in the context of the thread. 
// Tasks are added to a queue and processed sequentially.
void ThreadExecutor::execute(Function<void()> task) {
    LockGuard lock(impl_->queueMutex_);             // Ensure thread-safe access to the queue
    impl_->taskQueue_.push(NEX_STD move(task));     // Add the task to the queue
}

// ======================================================================
// ThreadPool Implementations
// ======================================================================

// Implementation details of ThreadPool
struct ThreadPool::Impl {
    ArrayList<Thread>       threads_;       // The pool of worker threads
    Queue<Function<void()>> taskQueue_;     // Queue of tasks to be executed
    Mutex                   queueMutex_;    // Mutex to protect access to the task queue
};

// Constructor that initializes the thread pool with a specified number of threads.
ThreadPool::ThreadPool(uint32 threadCount /*= Thread::hardwareConcurrency()*/)
    : impl_(NEX_STD make_unique<Impl>()) {
    impl_->threads_.reserve(threadCount);   // Reserve space for the specified number of threads
}

// Start all threads in the pool and begin processing tasks from the shared queue.
void ThreadPool::start() {
    for (size_t i = 0; i < impl_->threads_.capacity(); ++i) {
        impl_->threads_.emplace_back();     // Create a new Thread object for each worker thread
        impl_->threads_.back().start([this]() {
            while (true) {
                // Check for stop request
                if (impl_->threads_.back().stopRequested()) {
                    break; // Exit the loop to stop the thread
                }
                // Process tasks from the queue
                Function<void()> task;
                {
                    LockGuard lock(impl_->queueMutex_);     // Ensure thread-safe access to the queue
                    if (!impl_->taskQueue_.empty()) {
                        task = NEX_STD move(impl_->taskQueue_.front());
                        impl_->taskQueue_.pop();
                    }
                }
                if (task) {
                    task(); // Execute the task
                } else {
                    // No tasks available, yield to prevent busy-waiting
                    NEX_STD this_thread::yield();
                }
            }
        });
    }
}

// Request all threads in the pool to stop processing tasks and exit gracefully.
void ThreadPool::stop() {
    for (auto& thread : impl_->threads_) {
        thread.requestStop();   // Signal each thread to stop
    }
    for (auto& thread : impl_->threads_) {
        thread.join();          // Wait for each thread to finish execution
    }
}

// Execute a task in the context of the thread pool.
// Tasks are added to a shared queue and processed by available threads.
void ThreadPool::execute(Function<void()> task) {
    LockGuard lock(impl_->queueMutex_);             // Ensure thread-safe access to the queue
    impl_->taskQueue_.push(NEX_STD move(task));     // Add the task to the queue
}

// Get the number of threads in the pool.
uint32 ThreadPool::threadCount() const noexcept {
    return static_cast<uint32>(impl_->threads_.size());
}

NEX_NAMESPACE_END