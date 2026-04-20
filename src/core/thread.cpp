/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/thread.h"
#include "nex/base/linear.h"
#include "nex/base/adaptors.h"
#include "nex/base/associative.h"
#include "nex/core/string.h"
#include "nex/core/encoding.h"

#if NEX_PLATFORM_IS_WINDOWS
    #include <windows.h>
#elif NEX_PLATFORM_IS_MAC || NEX_PLATFORM_IS_LINUX
    #include <pthread.h>
    #include <sched.h>
#endif

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
    impl_->jthread = NEX_STD jthread([this, task = NEX_STD move(task)](StopToken stopToken) {
        impl_->running.store(true, NEX_STD memory_order_release);

        try {
            if (task) {
                task(stopToken);    // Pass stop_token to user task
            }
        } catch (...) {
            if (impl_->exceptionHandler) {
                impl_->exceptionHandler(NEX_STD current_exception());
            } else {
                // TODO: Log the exception using your logging system. 
                // For example:
                // spdlog::error("Uncaught exception in thread '{}'", impl_->name);
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
                workerThread->run();    // run() will check for stopRequested() internally
            }

            // Perform cleanup after stopping
            if (!workerThread->cleanup()) {
                NEX_ASSERT_MSG(false, "Thread worker cleanup failed");
                return;
            }
        } catch (...) {
            // If an exception escapes from the worker, call the exception handler if set
            if (impl_->exceptionHandler) {
                impl_->exceptionHandler(NEX_STD current_exception());
            }

            // Ensure cleanup is called even if an exception occurs
            if (!workerThread->cleanup()) {
                // TODO: Assert or log or handle cleanup failure if needed
                return;
            }
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
RuntimeId Thread::getRuntimeId() const noexcept {
    return impl_->runtimeId;
}

// Checks if a stop has been requested. Can be used inside the task if needed.
bool Thread::stopRequested() const noexcept {
    return impl_->stopSource.stop_requested();
}

////// Thread Naming and Priority -----------------------------------------------------

// Set the name of the thread (for debugging purposes)
void Thread::setName(StringView name) {
    impl_->name = name.toString();

#if NEX_PLATFORM_IS_WINDOWS
    // Windows: Use SetThreadDescription for modern systems
    if (isRunning()) {
        auto nameUtf16Res = impl_->name.toUtf16();
        if (!nameUtf16Res) {
            // Failed to convert name to UTF-16; log or handle error if needed
            return;
        }
        auto nameUtf16 = nameUtf16Res.value();
        HANDLE nativeHandle = impl_->jthread.native_handle();
        SetThreadDescription(nativeHandle, reinterpret_cast<LPCWSTR>(nameUtf16.c_str()));
    }
#elif NEX_PLATFORM_IS_MAC || NEX_PLATFORM_IS_LINUX
    // macOS/Linux: Use pthread_setname_np
    if (isRunning()) {
        auto nameUtf8Res = encoding::utf16ToUtf8(name);
        if (!nameUtf8Res) {
            // Failed to convert name to UTF-8; log or handle error if needed
            return;
        }
        auto nameUtf8 = nameUtf8Res.value();
        pthread_setname_np(nameUtf8.c_str());
    }
#endif
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

    if (!isRunning()) {
        // Thread is not running; priority will be applied when the thread starts.
        return;
    }

#if NEX_PLATFORM_IS_WINDOWS

    // Map ThreadPriority to Windows thread priority levels
    HANDLE nativeHandle = impl_->jthread.native_handle();
    int winPriority;

    switch (priority) {
        case ThreadPriority::Lowest:
            winPriority = THREAD_PRIORITY_LOWEST;
            break;
        case ThreadPriority::BelowNormal:
            winPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case ThreadPriority::Normal:
            winPriority = THREAD_PRIORITY_NORMAL;
            break;
        case ThreadPriority::AboveNormal:
            winPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case ThreadPriority::Highest:
            winPriority = THREAD_PRIORITY_HIGHEST;
            break;
    }

    if (!SetThreadPriority(nativeHandle, winPriority)) {
        // Failed to set thread priority; log or handle error if needed
    }

#elif NEX_PLATFORM_IS_MAC || NEX_PLATFORM_IS_LINUX

    int policy;
    struct sched_param param;

    // Map ThreadPriority to platform-specific priority levels
    pthread_t nativeHandle = impl_->jthread.native_handle();
    if (pthread_getschedparam(nativeHandle, &policy, &param) != 0) {
        // Failed to get current scheduling parameters; log or handle error if needed
        return;
    }

    switch (priority) {
        case ThreadPriority::Lowest:
            param.sched_priority = sched_get_priority_min(policy);
            break;
        case ThreadPriority::BelowNormal:
            param.sched_priority = sched_get_priority_min(policy) + 1;
            break;
        case ThreadPriority::Normal:
            param.sched_priority = (sched_get_priority_min(policy) + sched_get_priority_max(policy)) / 2;
            break;
        case ThreadPriority::AboveNormal:
            param.sched_priority = sched_get_priority_max(policy) - 1;
            break;
        case ThreadPriority::Highest:
            param.sched_priority = sched_get_priority_max(policy);
            break;
    }

    if (pthread_setschedparam(nativeHandle, policy, &param) != 0) {
        // Failed to set thread priority; log or handle error if needed
    }

#endif
}

// Get the current priority of the thread.
ThreadPriority Thread::getPriority() const noexcept {
    return impl_->priority;
}

////// Exception Handling -------------------------------------------------------------

// Set a handler to be called when an exception escapes from the task.
// If not set, exception will be logged and thread will terminate.
void Thread::setExceptionHandler(ExceptionHandler handler) {
    if (handler) {
        impl_->exceptionHandler = [h = NEX_STD move(handler)](Any any) {
            // Try to extract exception_ptr from Any.
            // Adjust this based on how your `nex::Any` works (std::any-like? custom?).
            try {
                auto eptr = any_cast<NEX_STD exception_ptr>(any);  // or whatever your Any supports
                h(eptr);
            } catch (...) {
                // Optional: ignore or handle type mismatch
            }
        };
    } else {
        // Invalid handler means we should clear the existing handler
        impl_->exceptionHandler = nullptr;
    }
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
