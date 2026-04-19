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
#include "nex/core/time.h"
#include "nex/core/string_view.h"

NEX_NAMESPACE_BEGIN

/**
 * @enum ThreadPriority
 * @brief Enumeration for thread priority levels.
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
 * @class Thread
 * @brief Modern, cross-platform thread wrapper using composition and task-based approach.
 *        Supports graceful shutdown, message queue, and seamless fallback between C++17 and C++20.
 * 
 * @note This class is move-only (non-copyable). It prefers composition over inheritance.
 */
class NEX_EXPORT Thread {
public:
    ////// Type Aliases ------------------------------------------------------------------

    // Thread ID type (system-specific)
    using Id = NEX_STD thread::id;

    // Native handle type (system-specific)
    using NativeHandle = NEX_STD thread::native_handle_type;
    
    // Stop token type for cooperative cancellation
    using StopToken = NEX_STD stop_token;

    // Task type
    template <typename R = void, typename... Args>
    using Task = Function<R(Args...)>;

    // Basic task type: a simple fire-and-forget task
    using BasicTask = Function<void()>;

    // A modern stop-aware task type
    using StoppableTask = Function<void(StopToken)>;

    // Generic task support
    template<typename F, typename... Args>
    using TaskResult = NEX_STD invoke_result_t<F, Args...>;

    // Exception handler type
    using ExceptionHandler = Function<void(NEX_STD exception_ptr)>;

    ////// Construction & Destruction ---------------------------------------------------

    // Default constructor 
    // Creates an empty thread object that can be started later
    Thread() noexcept;

    // Constructor that starts the thread immediately with a basic task
    explicit Thread(BasicTask task);

    // Constructor that starts the thread immediately with a stop-aware task
    explicit Thread(StoppableTask task);

    // Destructor
    // Automatically requests stop and joins the thread (RAII) to ensure clean shutdown
    ~Thread();

    // Move-only
    NEX_DISALLOW_COPY(Thread);
    NEX_DECLARE_DEFAULT_MOVE(Thread);

    ////// Starting a Thread ------------------------------------------------------------

    /**
     * @brief Starts the thread with the given basic task.
     * @param task The function to execute in the thread. Must be callable with no arguments.
     * @return true if the thread was successfully started, false otherwise (already running).
     */
    bool start(BasicTask task);

    /**
     * @brief Starts the thread with the given stop-aware task.
     * @param task The function to execute in the thread. Must be callable with a StopToken argument.
     * @return true if the thread was successfully started, false otherwise (already running).
     * 
     * @note If the thread is already running, this function will return false and do nothing.
     */
    bool start(StoppableTask task);

    /**
     * @brief Starts the thread with the given task.
     * @param task The function to execute in the thread. Can be any callable type (function, lambda, etc.).
     * @return true if the thread was successfully started, false otherwise (already running).
     * 
     * @note If the thread is already running, this function will return false and do nothing.
     */
    template <typename R, typename... Args>
    bool start(Task<R, Args...> task) {
        return start([taskFunc = NEX_STD move(task)]() mutable {
            taskFunc();
        });
    }

    /**
     * @brief Starts the thread with a generic callable and arguments.
     * @tparam F The type of the callable (function, lambda, etc.).
     * @tparam Args The types of the arguments to pass to the callable.
     * @param callable The function or callable object to execute in the thread.
     * @param args The arguments to pass to the callable when executed.
     * @return true if the thread was successfully started, false otherwise (already running).
     * 
     * @note If the thread is already running, this function will return false and do nothing.
     */
    template<typename F, typename... Args>
    bool start(F&& callable, Args&&... args) {
        return start(StoppableTask([func = NEX_STD forward<F>(callable), 
                                    tupleArgs = NEX_STD make_tuple(NEX_STD forward<Args>(args)...)] 
                                    (StopToken stopToken) mutable {
            NEX_STD apply([&](auto&&... unpacked) {
                func(NEX_STD forward<decltype(unpacked)>(unpacked)...);
            }, NEX_STD move(tupleArgs));
        }));
    }

    ////// Thread Control ------------------------------------------------------------------

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

    ////// Status Queries -----------------------------------------------------------------

    // Check whether the thread is currently executing
    bool isRunning() const noexcept;

    // Check whether the thread can be joined (i.e., it is joinable)
    bool isJoinable() const noexcept;

    // Returns the native thread ID (if running), or a default-constructed ID if not running
    Id getId() const noexcept;

    // Checks if a stop has been requested. Can be used inside the task if needed.
    bool stopRequested() const noexcept;

    ////// Thread Naming and Priority -----------------------------------------------------

    // Set a name for the thread (for debugging and profiling purposes). 
    // Note: This method should be called before start().
    void setName(StringView name);

    // Get the name of the thread (if set). 
    // Returns an empty string if no name was set.
    String getName() const;

    // Set the priority of the thread. 
    // Note: Implementation is platform-dependent and may have limited effect.
    void setPriority(ThreadPriority priority);

    // Get the current priority of the thread.
    ThreadPriority getPriority() const noexcept;

    ////// Thread Message Queue (High-level) ----------------------------------------------

    /**
     * @brief Posts a message to the thread's internal message queue.
     *        The message will be processed in the thread's context.
     * @tparam MessageType The type of the message to post. Can be any copyable or movable type.
     * @param message The message to post to the thread's message queue.
     * @return true if the message was queued successfully.
     */
    template<typename MessageType>
    bool postMessage(MessageType&& message);

    /**
     * @brief Posts multiple messages at once.
     *        This is a variadic template that allows posting multiple messages in a single call.
     * @tparam MessageTypes The types of the messages to post. Each can be any copyable or movable type.
     * @param messages The messages to post to the thread's message queue. Each message can be of 
     *        any copyable or movable type.
     * @return true if all messages were queued successfully.
     */
    template<typename... MessageTypes>
    bool postMessages(MessageTypes&&... messages);

    /**
     * @brief Sends a message to the thread's message queue.
     * @tparam MessageType The type of the message to send. Can be any copyable or movable type.
     * @param message The message to send to the thread's message queue.
     * @return true if the message was queued successfully.
     */
    template<typename MessageType>
    bool send(MessageType&& message) {
        // If the thread is not running, we cannot send messages to it
        if (!isRunning()) return false;

        // Lock the queue and add the message
        {
            NEX_STD lock_guard<NEX_STD mutex> lock(getMessageQueueMutex());
            getMessageQueue().emplace(NEX_STD forward<MessageType>(message));
        }

        // Notify one waiting thread that a new message is available
        getMessageQueueCV().notify_one();
        return true;
    }

    /**
     * @brief Registers a handler for a specific message type. 
     *        When a message of that type is received, the handler will be called with the message as an argument.
     * @tparam MessageType The type of the message to handle. Can be any copyable or movable type.
     * @tparam Handler The type of the handler function. Must be callable with a single argument of type MessageType.
     * @param handler The function to call when a message of the specified type is received. It should take 
     *        a single argument of type MessageType.
     * @note This allows you to set up a message handling system where different types of messages can be processed 
     *       by different handlers. The handler will be called in the thread's context when a message of the 
     *       corresponding type is received. You can register multiple handlers for different message types as needed.
     *       If a message is received for which no handler is registered, it will be ignored.
     */
    template<typename MessageType, typename Handler>
    void registerMessageHandler(Handler&& handler) {
        LockGuard<Mutex> lock(getMessageQueueMutex());
        getMessageHandlers()[TypeIndex(typeid(MessageType))] = 
            [h = NEX_STD forward<Handler>(handler)](Any msg) {
                h(NEX_STD any_cast<MessageType>(NEX_STD move(msg)));
            };
    }

    ////// Advanced Control ---------------------------------------------------------------
    
    /**
     * @brief Execute a task asynchronously and return a future with result.
     *        This runs on this thread, not on a thread pool.
     * @tparam F The type of the callable (function, lambda, etc.).
     * @tparam Args The types of the arguments to pass to the callable.
      * @param f The function or callable object to execute.
     * @param args The arguments to pass to the callable when executed.
     * @return A future that will hold the result of the callable once it has finished executing
     */
    template<typename F, typename... Args>
    auto async(F&& f, Args&&... args) 
        -> NEX_STD future<TaskResult<F, Args...>> 
    {
        using ResultType = TaskResult<F, Args...>;
        auto promise = NEX_STD make_shared<NEX_STD promise<ResultType>>();
        auto future = promise->get_future();

        auto task = [p = NEX_STD move(promise), 
                    func = NEX_STD forward<F>(f), 
                    tup = NEX_STD make_tuple(NEX_STD forward<Args>(args)...)]() mutable {
            try {
                if constexpr (NEX_STD is_void_v<ResultType>) {
                    NEX_STD apply(NEX_STD move(func), NEX_STD move(tup));
                    p->set_value();
                } else {
                    auto result = NEX_STD apply(NEX_STD move(func), NEX_STD move(tup));
                    p->set_value(NEX_STD move(result));
                }
            } catch (...) {
                p->set_exception(NEX_STD current_exception());
            }
        };

        // Post task as message or run directly if possible
        send(NEX_STD function<void()>(NEX_STD move(task)));

        return future;
    }

    /**
     * @brief Wait for a specific condition with timeout inside the thread.
     *        This can be used for waiting on a condition variable or any predicate that becomes true 
     *        when the thread should proceed.
     * @tparam Predicate The type of the predicate function.
     * @param pred The predicate function to evaluate.
     * @param timeout The maximum duration to wait for the predicate to become true.
     * @return true if the predicate became true within the timeout, false otherwise.
     */
    template<typename Predicate>
    bool waitFor(Predicate&& pred, time::Milliseconds timeout = time::Milliseconds::max());
    
    ////// Exception Handling -------------------------------------------------------------

    /**
     * @brief Set a handler to be called when an exception escapes from the task.
     *        If not set, exception will be logged and thread will terminate.
     * @param handler The function to call when an exception is caught. 
     *        It receives the exception as a std::exception_ptr.
     * @note This allows you to handle exceptions gracefully without crashing the entire application. 
     *       You can log the exception, attempt recovery, or perform any necessary cleanup. If no handler 
     *       is set, the default behavior is to log the exception and terminate the thread.
     */
    void setExceptionHandler(ExceptionHandler handler);

    ////// Static Utilities ---------------------------------------------------------------

    // Returns the hardware concurrency (number of logical cores) available on the system
    static uint32 hardwareConcurrency() noexcept;

    // Returns the current thread's ID from anywhere
    static Id currentThreadId() noexcept;

    /**
     * @brief Create a named thread. The thread will run the provided task.
     * @tparam F The type of the callable (function, lambda, etc.).
     * @tparam Args The types of the arguments to pass to the callable.
     * @param name The name of the thread.
     * @param f The function or callable object to execute.
     * @param args The arguments to pass to the callable when executed.
     * @return A Thread object representing the newly created thread.
     */
    template<typename F, typename... Args>
    static Thread create(StringView name, F&& f, Args&&... args) {
        Thread t;
        t.setName(name);
        t.start(NEX_STD forward<F>(f), NEX_STD forward<Args>(args)...);
        return t;
    }

private:
    // Internal implementation details
    struct Impl;
    UniquePtr<Impl> impl_;

    ////// Internal Helpers ------------------------------------------------------------------

    // Get the thread's internal message queue (for processing messages in the thread's context)
    Queue<Any> getMessageQueue() const;

    // Get the thread's internal message queue mutex (for synchronizing access to the message queue)
    Mutex& getMessageQueueMutex() const;

    // Get the thread's internal condition variable (for waiting on messages)
    ConditionVariable& getMessageQueueCV() const;

    // Get the thread's internal message handlers map (for dispatching messages to handlers)
    HashMap<TypeIndex, Function<void(Any)>>& getMessageHandlers() const;
};

NEX_NAMESPACE_END
