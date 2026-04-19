/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/thread.h"
#include "nex/base/linear.h"
#include "nex/base/adaptors.h"
#include "nex/base/associative.h"
#include "nex/core/string.h"

NEX_NAMESPACE_BEGIN

// ======================================================================
// Thread Implementation
// ======================================================================

struct Thread::Impl {
    // Core Thread Data
    NEX_STD jthread         jthread;
    NEX_STD stop_source     stopSource;
    String                  name;
    ThreadPriority          priority = ThreadPriority::Normal;

    // Message System
    HashMap<TypeIndex, Function<void(Any)>>     messageHandlers;
    Queue<Any>                                  messageQueue;
    Mutex                                       queueMutex;
    ConditionVariable                           queueCV;

    // Exception handler
    Function<void(Any)>     exceptionHandler;

    // Flag
    AtomicBool              running{false};
};

////// Construction & Destruction ---------------------------------------------------

// Default constructor
// Creates an empty thread object that can be started later
Thread::Thread() noexcept 
    : impl_(NEX_STD make_unique<Impl>()) {}

// Constructor that starts the thread immediately with a basic task
Thread::Thread(BasicTask task) : impl_(NEX_STD make_unique<Impl>()) {
    start([taskCb = NEX_STD move(task)](NEX_STD stop_token) { 
        taskCb(); 
    });
}

// Constructor that starts the thread immediately with a stop-aware task
Thread::Thread(StoppableTask task) : impl_(NEX_STD make_unique<Impl>()) {
    start(NEX_STD move(task));
}

// Destructor
// Automatically requests stop and joins the thread (RAII) to ensure clean shutdown
Thread::~Thread() {
    requestStop();
    join();
}

// Default move semantics
NEX_DEFINE_DEFAULT_MOVE(Thread);

////// Starting a Thread ------------------------------------------------------------

// Starts the thread with the given basic task.
bool Thread::start(BasicTask task) {
    return start([t = NEX_STD move(task)](NEX_STD stop_token) { t(); });
}

// Starts the thread with the given stop-aware task.
bool Thread::start(StoppableTask task) {

    // If already running, do not start another thread
    if (isRunning()) return false;

    // Start the thread with a lambda that wraps the user task and handles exceptions
    impl_->jthread = NEX_STD jthread([this, task = NEX_STD move(task)](NEX_STD stop_token st) {
        impl_->running.store(true, NEX_STD memory_order_release);

        try {
            if (task) {
                task(st);                    // Pass stop_token to user task
            }
        } catch (...) {
            if (impl_->exceptionHandler) {
                impl_->exceptionHandler(NEX_STD current_exception());
            } else {
                spdlog::error("Uncaught exception in thread '{}'", impl_->name);
            }
        }

        // Clean up and reset running flag
        impl_->running.store(false, NEX_STD memory_order_release);
    });

    return true;
}

////// Thread Control ------------------------------------------------------------------

// Requests the thread to stop gracefully.
// The thread will finish its current work before exiting.
void Thread::requestStop() noexcept {
    impl_->stopSource.request_stop();
}

// Blocks until the thread finishes execution.
// If the thread is not joinable, this function does nothing.
void Thread::join() {
    if (impl_->jthread.joinable()) {
        impl_->jthread.join();
    }
}

// Detaches the thread from the calling thread.
// Use with caution as the thread may outlive the Thread object.
void Thread::detach() {
    if (impl_->jthread.joinable()) {
        impl_->jthread.detach();
    }
}

////// Status Queries -----------------------------------------------------------------

// Check whether the thread is currently executing
bool Thread::isRunning() const noexcept {
    return impl_->running.load(NEX_STD memory_order_acquire);
}

// Check whether the thread can be joined (i.e., it is joinable)
bool Thread::isJoinable() const noexcept {
    return impl_->jthread.joinable();
}

// Returns the native thread ID (if running), or a default-constructed ID if not running
Thread::Id Thread::getId() const noexcept {
    if (isRunning()) {
        return impl_->jthread.get_id();
    } else {
        return Id();
    }
}

// Checks if a stop has been requested. Can be used inside the task if needed.
bool Thread::stopRequested() const noexcept {
    return impl_->stopSource.stop_requested();
}

////// Thread Naming and Priority -----------------------------------------------------

// Set the name of the thread (for debugging purposes)
void Thread::setName(StringView name) {
    impl_->name = name;
    // Platform-specific code to set thread name can be added here if desired
}

// Get the name of the thread (if set). 
// Returns an empty string if no name was set.
StringView Thread::getName() const noexcept {
    return impl_->name;
}

// Set the priority of the thread.
// Note: Implementation is platform-dependent and may have limited effect.
void Thread::setPriority(ThreadPriority priority) {
    impl_->priority = priority;
    // Platform-specific code to set thread priority can be added here if desired
}

// Get the current priority of the thread.
ThreadPriority Thread::getPriority() const noexcept {
    return impl_->priority;
}

////// Static Utilities ---------------------------------------------------------------

// Returns the hardware concurrency (number of logical cores) available on the system
uint32 Thread::hardwareConcurrency() noexcept {
    return NEX_STD thread::hardware_concurrency();
}

// Returns the current thread's ID from anywhere
Thread::Id Thread::currentThreadId() noexcept {
    return NEX_STD this_thread::get_id();
}

NEX_NAMESPACE_END