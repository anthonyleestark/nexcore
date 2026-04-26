/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <thread>
#include <stop_token>
#include <future>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/primitive.h"
#include "nex/base/wrappers.h"
#include "nex/core/runtime_id.h"
#include "nex/core/string_view.h"
#include "nex/core/error.h"
#include "nex/core/result.h"

NEX_NAMESPACE_BEGIN

/**
 * @typedef ThreadId
 * @brief A type alias for the native thread ID type.
 * 
 * @details
 * This type represents the unique identifier for a thread as defined by the underlying operating system.
 * It is typically used for thread management and debugging purposes. The actual type of ThreadId may vary 
 * across different platforms, but it is designed to be compatible with the native thread ID type provided 
 * by the C++ standard library (std::thread::id).
 * The ThreadId can be obtained from a Thread object using the getId() method, and it can be compared with 
 * other ThreadId instances to check for thread identity.
 * 
 * @note
 * - ThreadId is not necessarily a numeric value and may not be directly usable for arithmetic operations. 
 *   It is primarily intended for comparison and identification of threads.
 * - A ThreadId can be reused by the system after a thread has finished execution, so it should not be stored 
 *   for long-term use without proper management.
 * - The default-constructed ThreadId represents a non-existent thread and can be used to check if a thread 
 *   is running (i.e., if getId() returns a non-default value).
 */
using ThreadId = NEX_STD thread::id;

/**
 * @typedef StopToken
 * @brief A type alias for the stop token used for cooperative cancellation of threads.
 * 
 * @details
 * This type represents a token that can be used to request a thread to stop its execution gracefully.
 * It is part of the C++20 standard library and is designed to facilitate cooperative cancellation of threads.
 * A StopToken can be passed to a thread's task function, allowing the thread to periodically check if a stop 
 * has been requested and exit accordingly.
 * The Thread class provides a requestStop() method that can be called to signal the thread to stop, and the 
 * thread's task can check for this signal using the stopRequested() method or by directly checking the StopToken.
 */
using StopToken = NEX_STD stop_token;

/**
 * @typedef StopSource
 * @brief A type alias for the stop source used to create stop tokens for cooperative cancellation of threads.
 * 
 * @details
 * This type represents a source that can be used to create StopToken instances for requesting threads to stop 
 * their execution gracefully. It is part of the C++20 standard library and works in conjunction with StopToken 
 * to facilitate cooperative cancellation of threads. A StopSource can be used to generate StopTokens that can be 
 * passed to thread tasks, allowing those tasks to check for stop requests and exit when requested.
 */
using StopSource = NEX_STD stop_source;

/**
 * @struct ThreadContext
 * @brief A struct that encapsulates the context of a thread, including its ID and stop token.
 * 
 * @details
 * This struct is designed to provide a convenient way to access thread-related information and control 
 * mechanisms within a thread's execution context. It contains the thread's unique identifier (ThreadId) 
 * and a StopToken that can be used to check for stop requests. The stopRequested() method allows the thread 
 * to easily determine if it should exit based on a stop request from another thread.
 */
struct ThreadContext {
    ThreadId threadId;
    StopToken stopToken;

    // Check if stop has been requested by the owning Thread.
    bool stopRequested() const noexcept {
        return stopToken.stop_requested();
    }
};

/**
 * @enum ThreadPriority
 * @brief Enumeration for thread priority levels.
 * 
 * @details
 * This enum defines various priority levels that can be assigned to threads. 
 * The actual effect of setting thread priority is platform-dependent and may not be supported on all systems. 
 * Use with caution and refer to your platform's documentation for details on how thread priorities are handled.
 * 
 * @note 
 * The priority levels are defined in a way that allows for easy comparison 
 * (e.g., Lowest < BelowNormal < Normal < AboveNormal < Highest).
 * Setting thread priority may require elevated permissions on some platforms.
 * 
 * @see Thread::setPriority(), Thread::getPriority()
 */
enum class ThreadPriority {
    Lowest,
    BelowNormal,
    Normal,
    AboveNormal,
    Highest
};

/**
 * @typedef ThreadNativeHandle
 * @brief A type alias for the native handle of a thread.
 * 
 * @details
 * This type represents the native handle of a thread, which is system-specific. It can be used to perform
 * low-level operations on the thread that are not covered by the standard C++ thread interface.
 */
using ThreadNativeHandle = NEX_STD thread::native_handle_type;

/**
 * @class Executor
 * @brief Abstract base class for creating custom executors.
 * 
 * @details
 * Executor provides a structured way to define the behavior of a thread. You can derive from this class and implement 
 * the execute() method to define how tasks are executed in the context of the thread.
 */
class Executor {
public:
    // Virtual destructor to allow proper cleanup of derived classes
    virtual ~Executor() = default;

    // Execute a task in the context of the thread
    virtual void execute(Function<void()> task) = 0;
};

/**
 * @struct Runnable
 * @brief An interface for defining runnable tasks that can be executed by a thread.
 * 
 * @details
 * This struct defines a simple interface for tasks that can be run in a thread. The run() method takes a StopToken as 
 * an argument, allowing the task to check for stop requests and exit gracefully when requested. 
 * You can implement this interface in your own classes to define custom behavior for thread tasks.
 */
struct Runnable {
    // Virtual destructor to allow proper cleanup of derived classes
    virtual ~Runnable() = default;

    // Run a task with the given stop token for cooperative cancellation
    virtual void run(StopToken) = 0;
};

/**
 * @class Thread
 * @brief Modern, high-level, cross-platform thread abstraction with rich task support,
 *        graceful shutdown, typed messaging, exception safety, and async capabilities.
 * 
 * @details
 * The Thread class provides a comprehensive interface for creating and managing threads in C++.
 * It supports starting threads with various types of tasks, including basic fire-and-forget tasks,
 * stop-aware tasks that can respond to cancellation requests, and generic callables with arguments.
 * The class also includes features for thread control (joining, detaching), status queries (running, 
 * joinable, stop requested), and exception handling.
 * Additionally, it offers a simple message queue system for inter-thread communication, allowing you to
 * send messages to a thread and register handlers for specific message types. Thread naming and priority
 * are also supported for better debugging and performance tuning.
 * 
 * @note
 * Always ensure that threads are properly joined or detached to avoid resource leaks.
 * When using stop-aware tasks, make sure to periodically check for stop requests to allow for graceful shutdown.
 * The behavior of thread priorities is platform-dependent and may not have the desired effect on all systems.
 * Exception handling within threads is crucial to prevent crashes; consider setting an exception handler 
 * if your tasks may throw.
 * 
 * @see ThreadPriority enum for available priority levels.
 */
class NEX_EXPORT Thread {
public:
    ////// Construction & Destruction ---------------------------------------------------

    // Default constructor 
    // Creates an empty thread object that can be started later
    Thread() noexcept;

    // Destructor
    // Automatically requests stop and joins the thread (RAII) to ensure clean shutdown
    ~Thread();

    // Move-only
    NEX_DISALLOW_COPY(Thread);
    NEX_DECLARE_DEFAULT_MOVE(Thread);

    ////// Starting a Thread ------------------------------------------------------------

    /**
     * @brief Starts the thread with a generic callable and arguments.
     * @tparam Fn The type of the callable (function, lambda, etc.).
     * @tparam Args The types of the arguments to pass to the callable.
     * @param callable The function or callable object to execute in the thread.
     * @param args The arguments to pass to the callable when executed.
     * @return true if the thread was successfully started, false otherwise (already running).
     * 
     * @note If the thread is already running, this function will return false and do nothing.
     */
    template<typename Fn, typename... Args>
    bool start(Fn&& callable, Args&&... args) {
        return startWithTask(NEX_STD forward<Fn>(callable), NEX_STD forward<Args>(args)...);
    }

    ////// Thread Control ---------------------------------------------------------------

    /**
     * @brief Requests the thread to stop gracefully.
     *        The thread will finish its current work before exiting.
     * @note
     * This does not forcefully terminate the thread. The thread's task should periodically check for 
     * stop requests using stopRequested() and exit when requested.
     */
    void requestStop() noexcept;

    /**
     * @brief Blocks until the thread finishes execution.
     *        If the thread is not joinable, this function does nothing.
     * @note
     * Always ensure that the thread is joinable before calling join() to avoid undefined behavior. 
     * If the thread has already been joined or detached, this function will do nothing.
     */
    void join();

    /**
     * @brief Detaches the thread from the calling thread.
     * @warning Use with caution as the thread may outlive the Thread object.
     * @note
     * Once detached, the thread will continue to run independently. The Thread object will no longer 
     * have control over it, and you cannot join or request stop on a detached thread. Ensure that any 
     * resources used by the thread are properly managed to avoid leaks or dangling references.
     */
    void detach();

    ////// Status Queries ---------------------------------------------------------------

    // Check whether the thread is currently executing
    bool isRunning() const noexcept;

    // Check whether the thread can be joined (i.e., it is joinable)
    bool isJoinable() const noexcept;

    // Returns the native thread ID (if running), or a default-constructed ID if not running
    ThreadId getId() const noexcept;

    /**
     * @brief Returns the RuntimeId associated with this thread.
     * @return The RuntimeId of the thread.
     * @note 
     * The RuntimeId is a unique identifier for the thread that can be used for tracking and debugging purposes.
     * This is different from the native thread ID and is generated internally by the Thread class.
     * The thread doesn't need to be running to have a valid RuntimeId, as it is assigned when the thread is 
     * constructed or started and remains constant for the lifetime of the Thread object. 
     * Unlike the native thread ID, which can be reused by the system, the RuntimeId is ensured to be unique 
     * across all threads in the process.
     */
    RuntimeId getRuntimeId() const noexcept;

    // Checks if a stop has been requested. Can be used inside the task if needed.
    bool stopRequested() const noexcept;

    ////// Thread Naming and Priority ---------------------------------------------------

    // Set a name for the thread (for debugging and profiling purposes).
    void setName(StringView name);

    // Get the name of the thread (if set).
    // Returns an empty string if no name was set.
    StringView getName() const noexcept;

    // Set the priority of the thread. 
    // Note: Implementation is platform-dependent and may have limited effect.
    void setPriority(ThreadPriority priority);

    // Get the current priority of the thread.
    ThreadPriority getPriority() const noexcept;

    ////// Static Utilities -------------------------------------------------------------

    // Returns the hardware concurrency (number of logical cores) available on the system
    static uint32 hardwareConcurrency() noexcept;

    // Returns the current thread's ID from anywhere
    static ThreadId currentThreadId() noexcept;

    /**
     * @brief Create a named thread. The thread will run the provided task.
     * @tparam Fn The type of the callable (function, lambda, etc.).
     * @tparam Args The types of the arguments to pass to the callable.
     * @param name The name of the thread.
     * @param callable The function or callable object to execute.
     * @param args The arguments to pass to the callable when executed.
     * @return A Thread object representing the newly created thread.
     */
    template<typename Fn, typename... Args>
    static Thread create(StringView name, Fn&& callable, Args&&... args) {
        Thread t;
        t.setName(name);
        t.start(NEX_STD forward<Fn>(callable), NEX_STD forward<Args>(args)...);
        return t;
    }

private:
    // Internal implementation details
    struct Impl;
    UniquePtr<Impl> impl_;

    /**
     * @brief Start the thread with a packaged task that wraps the callable and its arguments.
     * @tparam Fn The type of the callable (function, lambda, etc.).
     * @tparam Args The types of the arguments to pass to the callable.
     * @param callable The function or callable object to execute in the thread.
     * @param args The arguments to pass to the callable when executed.
     * @return true if the thread was successfully started, false otherwise (already running).
     * @note If the thread is already running, this function will return false and do nothing.
     */
    template<typename Fn, typename... Args>
    bool startWithTask(Fn&& callable, Args&&... args);
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
    Thread thread_;
    Queue<Function<void()>> queue_;
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
};

/**
 * @class Future
 * @brief Represents a value that will be available at some point in the future.
 * 
 * @details
 * Future is a template class that encapsulates the result of an asynchronous operation. It provides a way 
 * to retrieve the value once it is ready, and to check if the value is available without blocking. 
 * A Future can be obtained from a Promise, which is used to set the value or an exception that occurred 
 * during the asynchronous operation. The Future class provides methods to get the value, check if it is ready, 
 * and handle exceptions. It is designed to work seamlessly with the Thread and ThreadPool classes for managing 
 * asynchronous tasks and their results.
 * 
 * @tparam T The type of the value that will be available.
 */
template<typename T>
class NEX_EXPORT Future {
public:
    // Get the value of the Future. 
    // This will block until the value is available.
    T get();

    // Check if the value is ready without blocking.
    bool isReady() const;
};

/**
 * @class Promise
 * @brief Used to set a value or an exception for a Future.
 * 
 * @details
 * Promise is a template class that allows you to set the value or an exception for a Future. It is typically 
 * used in conjunction with asynchronous operations, where the Promise is used to signal the completion of a task 
 * and provide the result to any waiting Futures. The Promise class provides methods to set the value, set an 
 * exception, and retrieve the associated Future. It is designed to work seamlessly with the Thread and ThreadPool 
 * classes for managing asynchronous tasks and their results.
 * 
 * @tparam T The type of the value that will be set for the Future.
 */
template<typename T>
class NEX_EXPORT Promise {
public:
    // Set the value of the Promise, which will make it available to the associated Future.
    void setValue(T value);

    // Set an exception for the Promise, which will be propagated to the associated Future.
    void setException(NEX_STD exception_ptr e);

    // Get the Future associated with this Promise.
    Future<T> getFuture();
};

/**
 * @typedef InvokeResult
 * @brief A type alias for the result type of invoking a callable with specific arguments.
 */
template<typename Fn, typename... Args>
using InvokeResult = NEX_STD invoke_result_t<Fn, Args...>;

/**
 * @brief Asynchronously executes a callable on the given executor and returns a Future for its result.
 * @tparam Fn The type of the callable (function, lambda, etc.).
 * @tparam Args The types of the arguments to pass to the callable.
 * @param executor The executor on which to run the task.
 * @param fn The function or callable object to execute.
 * @param args The arguments to pass to the callable when executed.
 * @return A Future representing the result of the asynchronous operation.
 */
template<typename Fn, typename... Args>
auto async(Executor& executor, Fn&& fn, Args&&... args) -> Future<InvokeResult<Fn, Args...>>;

NEX_NAMESPACE_END
