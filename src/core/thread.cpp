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
// ThreadWorker Implementation
// ======================================================================

// Check if a stop has been requested. 
// Can be used inside the task if needed.
bool ThreadWorker::stopRequested() const noexcept {
    return owner_ ? owner_->stopRequested() : true;
}

// ======================================================================
// Thread Implementation
// ======================================================================

struct Thread::Impl {
    // Core Thread Data
    NEX_STD jthread                             jthread;
    NEX_STD stop_source                         stopSource;
    String                                      name;
    ThreadPriority                              priority = ThreadPriority::Normal;
    RuntimeId                                   runtimeId;

    // Worker thread
    UniquePtr<ThreadWorker>                     worker;

    // Message System
    HashMap<TypeIndex, Function<void(Any)>>     messageHandlers;
    Queue<Any>                                  messageQueue;
    Mutex                                       queueMutex;
    ConditionVariable                           queueCV;

    // Exception handler
    Function<void(Any)>                         exceptionHandler;

    // Flag
    AtomicBool                                  running{false};
};

////// Internal Helpers ------------------------------------------------------------------

// Get the thread's internal message queue (for processing messages in the thread's context)
Queue<Any>& Thread::getMessageQueue() const {
    return impl_->messageQueue;
}

// Get the thread's internal message queue mutex (for synchronizing access to the message queue)
Mutex& Thread::getMessageQueueMutex() const {
    return impl_->queueMutex;
}

// Get the thread's internal condition variable (for waiting on messages)
ConditionVariable& Thread::getMessageQueueCV() const {
    return impl_->queueCV;
}

// Get the thread's internal message handlers map (for dispatching messages to handlers)
HashMap<TypeIndex, Function<void(Any)>>& Thread::getMessageHandlers() const {
    return impl_->messageHandlers;
}

////// Construction & Destruction ---------------------------------------------------

// Default constructor
// Creates an empty thread object that can be started later
Thread::Thread() noexcept 
    : impl_(NEX_STD make_unique<Impl>()) {
    // Generate a unique runtime ID for this thread instance
    impl_->runtimeId = RuntimeId::generate();
}

// Constructor that starts the thread immediately with a basic task
Thread::Thread(BasicTask task) : impl_(NEX_STD make_unique<Impl>()) {
    // Generate a unique runtime ID for this thread instance
    impl_->runtimeId = RuntimeId::generate();

    // Start the thread
    start([taskCb = NEX_STD move(task)](NEX_STD stop_token) { 
        taskCb(); 
    });
}

// Constructor that starts the thread immediately with a stop-aware task
Thread::Thread(StoppableTask task) : impl_(NEX_STD make_unique<Impl>()) {
    // Generate a unique runtime ID for this thread instance
    impl_->runtimeId = RuntimeId::generate();

    // Start the thread
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
    // Generate a unique runtime ID for this thread instance if it doesn't have one
    if (impl_->runtimeId.isInvalid()) {
        impl_->runtimeId = RuntimeId::generate();
    }
    
    // Start the thread
    return start([t = NEX_STD move(task)](NEX_STD stop_token) { t(); });
}

// Starts the thread with the given stop-aware task.
bool Thread::start(StoppableTask task) {

    // If already running, do not start another thread
    if (isRunning()) return false;

    // Generate a unique runtime ID for this thread instance if it doesn't have one
    if (impl_->runtimeId.isInvalid()) {
        impl_->runtimeId = RuntimeId::generate();
    }

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

// Starts the thread with a custom ThreadWorker implementation
bool Thread::start(UniquePtr<ThreadWorker> worker) {
    // If already running or the worker is invalid, do not start
    if (isRunning() || !worker) return false;

    // Generate a unique runtime ID for this thread instance if it doesn't have one
    if (impl_->runtimeId.isInvalid()) {
        impl_->runtimeId = RuntimeId::generate();
    }

    // Bind the worker to the thread and inject the owner pointer
    impl_->worker = NEX_STD move(worker);
    impl_->worker->owner_ = this;

    // Start the thread
    return start([this](StopToken stopToken) {
        auto* workerThread = impl_->worker.get();
        if (!workerThread) return;

        try {
            // Initialize the worker. 
            // If initialization fails, we should not run the thread.
            if (!workerThread->init()) {
                // TODO: Assert or log initialization failure if needed
                return;
            }

            // Main thread loop
            while (!stopRequested() && !stopToken.stop_requested()) {
                workerThread->run();                   // run() will check for stopRequested() internally
            }

            // Perform cleanup after stopping
            workerThread->cleanup();
        } catch (...) {
            // If an exception escapes from the worker, call the exception handler if set
            if (impl_->exceptionHandler) {
                impl_->exceptionHandler(NEX_STD current_exception());
            }

            // Ensure cleanup is called even if an exception occurs
            workerThread->cleanup();
        }
    });
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

// Returns the RuntimeId associated with this thread.
Thread::RuntimeId Thread::getRuntimeId() const noexcept {
    return impl_->runtimeId;
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