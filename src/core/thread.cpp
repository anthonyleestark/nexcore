/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/thread.h"
#include "nex/base/linear.h"
#include "nex/base/adaptors.h"
#include "nex/base/associative.h"
#include "nex/core/text/string.h"
#include "nex/core/text/encoding.h"

#if NEX_PLATFORM_IS_WINDOWS
    #include <windows.h>
#elif NEX_PLATFORM_IS_MAC || NEX_PLATFORM_IS_LINUX
    #include <pthread.h>
    #include <sched.h>
#endif

NEX_NAMESPACE_BEGIN

// ======================================================================
// Thread Implementation
// ======================================================================

struct Thread::Impl {
    // Core Thread Data
    NEX_STD jthread     jthread;
    StopSource          stopSource;

    // Metadata
    String              name;
    ThreadPriority      priority = ThreadPriority::Normal;
    RuntimeId           runtimeId;

    // Flag
    AtomicBool          running{false};

    // Method for applying thread name to the native thread handle
    void applyThreadName();

    // Method for applying thread priority to the native thread handle
    void applyThreadPriority();
};

////// Internal helper functions ------------------------------------------------------

// Apply the thread's name to the native thread handle (platform-specific implementation)
void Thread::Impl::applyThreadName() {
#if NEX_PLATFORM_IS_WINDOWS
    // Windows: Use SetThreadDescription for modern systems
    if (!name.empty()) {
        auto nameUtf16Res = name.toUtf16();
        if (!nameUtf16Res) {
            // Failed to convert name to UTF-16; log or handle error if needed
            return;
        }
        auto nameUtf16 = nameUtf16Res.value();
        HANDLE nativeHandle = jthread.native_handle();
        SetThreadDescription(nativeHandle, reinterpret_cast<LPCWSTR>(nameUtf16.c_str()));
    }
#elif NEX_PLATFORM_IS_MAC || NEX_PLATFORM_IS_LINUX
    // macOS/Linux: Use pthread_setname_np
    if (!name.empty()) {
        auto nameUtf8Res = encoding::utf16ToUtf8(name.view().toStdU16StringView());
        if (!nameUtf8Res) {
            // Failed to convert name to UTF-8; log or handle error if needed
            return;
        }
        auto nameUtf8 = nameUtf8Res.value();
        pthread_setname_np(nameUtf8.c_str());
    }
#endif
}

// Apply the thread priority to the native thread handle (platform-specific implementation)
void Thread::Impl::applyThreadPriority() {
#if NEX_PLATFORM_IS_WINDOWS

    // Map ThreadPriority to Windows thread priority levels
    HANDLE nativeHandle = jthread.native_handle();
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
    pthread_t nativeHandle = jthread.native_handle();
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

////// Construction & Destruction -----------------------------------------------------

// Default constructor
// Creates an empty thread object that can be started later
Thread::Thread() noexcept 
    : impl_(NEX_STD make_unique<Impl>()) {
    // Generate a unique runtime ID for this thread instance
    impl_->runtimeId = RuntimeId::generate();
}

// Destructor
// Automatically requests stop and joins the thread (RAII) to ensure clean shutdown
Thread::~Thread() {
    requestStop();
    join();
}

// Default move semantics
NEX_DEFINE_DEFAULT_MOVE(Thread);

////// Starting a Thread --------------------------------------------------------------

// Start the thread with a packaged task that wraps the callable and its arguments.
template<typename Fn, typename... Args>
bool Thread::startWithTask(Fn&& callable, Args&&... args) {
    // If the thread is already running, we cannot start it again
    if (isRunning()) return false;

    // Reset stop source to ensure a fresh token for the new thread, 
    // this is to prevent any previous stop requests from affecting the new thread,
    // because std::jthread may reuse the same thread object after it finishes
    impl_->stopSource = StopSource{};
    auto token = impl_->stopSource.get_token();

    // Mark the thread as running before starting to ensure getId() returns a valid ID inside the thread function
    impl_->running.store(true, NEX_STD memory_order_release);

    // Start the thread with a lambda that captures the callable and its arguments
    impl_->jthread = NEX_STD jthread(
        [func = NEX_STD forward<Fn>(callable), ...argList = NEX_STD forward<Args>(args)]
        (StopToken stopToken) mutable {

            // Apply the thread's attributes
            impl_->applyThreadName();
            impl_->applyThreadPriority();

            // Call the provided function with the stop token and arguments if it accepts a stop token, 
            // otherwise call it without the stop token
            if constexpr (NEX_STD is_invocable_v<Fn, StopToken, Args...>) {
                NEX_STD invoke(func, stopToken, NEX_STD move(argList)...);
            } else {
                NEX_STD invoke(func, NEX_STD move(argList)...);
            }

            // Mark the thread as not running when the function exits
            impl_->running.store(false, NEX_STD memory_order_release);
        }
    );

    return true;
}

// Explicit template instantations for basic task types
template bool Thread::startWithTask<Function<void()>>(Function<void()>&& callable);
template bool Thread::startWithTask<void(*)()>(void(*&& callable)());

// Explicit template instantiation for tasks that accept a StopToken
template bool Thread::startWithTask<Function<void(StopToken)>>(Function<void(StopToken)>&& callable);

////// Thread Control -----------------------------------------------------------------

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
ThreadId Thread::getId() const noexcept {
    if (isRunning()) {
        return impl_->jthread.get_id();
    } else {
        return ThreadId();
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
    if (isRunning()) {
        impl_->applyThreadName();
    }
}

// Get the name of the thread (if set). 
// Returns an empty string if no name was set.
StringView Thread::getName() const noexcept {
    return impl_->name;
}

// Set the priority of the thread.
void Thread::setPriority(ThreadPriority priority) {
    if (priority == impl_->priority) return; // No change needed
    impl_->priority = priority;
    if (isRunning()) {
        impl_->applyThreadPriority();
    }
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
ThreadId Thread::currentThreadId() noexcept {
    return NEX_STD this_thread::get_id();
}

NEX_NAMESPACE_END
