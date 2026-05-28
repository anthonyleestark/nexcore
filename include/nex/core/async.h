/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <exception>
#include <future>
#include <tuple>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/traits.h"
#include "nex/base/primitives.h"
#include "nex/base/wrappers.h"
#include "nex/core/executor.h"

NEX_NAMESPACE_BEGIN

// Forward declaration
template<typename T>
class Promise;

NEX_DETAIL_NAMESPACE_BEGIN

/**
 * @class FutureState
 * @brief Internal state for Future, shared between Promise and Future.
 * 
 * @tparam T The type of the value that the Future will hold.
 * 
 * @details
 * FutureState is an internal class that manages the state of a Future. It holds the value (or exception) 
 * that will be set by the Promise, and it provides synchronization mechanisms to allow the Future to wait 
 * for the value to become available. The FutureState class is designed to be thread-safe, allowing multiple 
 * threads to interact with the Future and Promise without race conditions. It uses a mutex and condition variable 
 * to manage access to the state and to allow the Future to block until the value is ready. 
 * The FutureState class is not intended to be used directly by users of the Future and Promise classes; 
 * it is an implementation detail that supports the functionality of those classes.
 */
template<typename T>
class NEX_INTERNAL FutureState {
public:
    // Set the value of the Future.
    // This will unblock any waiting threads.
    void setValue(T value) {
        {
            UniqueLock lock(mutex_);
            throwIfReady();
            value_.emplace(NEX_MOVE(value));
            ready_ = true;
        }
        condition_.notify_all();
    }

    // Set an exception for the Future.
    // This will unblock any waiting threads and cause them to throw the exception when they call get().
    void setException(NEX_STD exception_ptr exception) {
        {
            UniqueLock lock(mutex_);
            throwIfReady();
            exception_ = exception ? exception : makeException(NEX_STD future_errc::broken_promise);
            ready_ = true;
        }
        condition_.notify_all();
    }

    // Set a broken promise state, which indicates that the Promise was destroyed
    // without setting a value or an exception.
    void setBrokenPromise() noexcept {
        try {
            UniqueLock lock(mutex_);
            if (ready_) {
                return;
            }
            exception_ = makeException(NEX_STD future_errc::broken_promise);
            ready_ = true;
            condition_.notify_all();
        } catch (...) {
        }
    }

    // Get the value of the Future.
    // This will block until the value is available.
    T get() {
        UniqueLock lock(mutex_);
        condition_.wait(lock, [this]() { return ready_; });

        // Check if the value has already been consumed.
        // A Future can only be consumed once, and subsequent calls to get() will throw an exception.
        if (consumed_) {
            throwFutureError(NEX_STD future_errc::no_state);
        }
        consumed_ = true;

        // If an exception was set, rethrow it to the caller.
        if (exception_) {
            NEX_STD rethrow_exception(exception_);
        }

        // Return the value.
        // Since we are moving it out, we can reset the optional to release the memory.
        return NEX_MOVE(*value_);
    }

    // Check if the value is ready without blocking.
    bool isReady() const {
        LockGuard lock(mutex_);
        return ready_;
    }

private:
    // Helper function to create an exception pointer for a given future error code.
    static NEX_STD exception_ptr makeException(NEX_STD future_errc error) {
        return NEX_STD make_exception_ptr(
            NEX_STD future_error(error)
        );
    }

    // Helper function to throw a future_error with the given error code.
    static void throwFutureError(NEX_STD future_errc error) {
        throw NEX_STD future_error(error);
    }

    // Helper function to check if the Future is already ready (i.e., has a value or an exception set).
    // If it is ready, this function will throw a future_error with the promise_already_satisfied error code.
    void throwIfReady() const {
        if (ready_) {
            throwFutureError(NEX_STD future_errc::promise_already_satisfied);
        }
    }

    ////// Internal state variables for the Future --------------------------------------

    mutable Mutex mutex_;
    ConditionVariable condition_;
    Optional<T> value_;
    NEX_STD exception_ptr exception_;
    bool ready_ = false;
    bool consumed_ = false;
};

/**
 * @class FutureState<void>
 * @brief Specialization of FutureState for void type.
 * 
 * @details
 * This specialization handles the case where the Future does not hold a value. It provides methods to
 * set the completion or an exception for the Future, and to check if the Future is ready without blocking. 
 * The implementation is similar to the general FutureState, but it does not need to manage a value, 
 * since the Future is of type void. Instead, it simply tracks whether the Future is ready and whether 
 * an exception has been set.
 */
template<>
class NEX_INTERNAL FutureState<void> {
public:
    // Set the value of the Future (for void, this just means marking it as ready).
    void setValue() {
        {
            UniqueLock lock(mutex_);
            throwIfReady();
            ready_ = true;
        }
        condition_.notify_all();
    }

    // Set an exception for the Future.
    // This will unblock any waiting threads and cause them to throw the exception when they call get().
    void setException(NEX_STD exception_ptr exception) {
        {
            UniqueLock lock(mutex_);
            throwIfReady();
            exception_ = exception ? exception : makeException(NEX_STD future_errc::broken_promise);
            ready_ = true;
        }
        condition_.notify_all();
    }

    // Set a broken promise state, which indicates that the Promise was destroyed
    // without setting a value or an exception.
    void setBrokenPromise() noexcept {
        try {
            UniqueLock lock(mutex_);
            if (ready_) {
                return;
            }
            exception_ = makeException(NEX_STD future_errc::broken_promise);
            ready_ = true;
            condition_.notify_all();
        } catch (...) {
            // Swallow any exceptions thrown while trying to set the broken promise state,
            // since we are already in an error state.
        }
    }

    // Get the value of the Future.
    // This will block until the value is available. For void, this just means waiting until it's ready or an exception is set.
    void get() {
        UniqueLock lock(mutex_);
        condition_.wait(lock, [this]() { return ready_; });

        // Check if the value has already been consumed.
        // A Future can only be consumed once, and subsequent calls to get() will throw an exception.
        if (consumed_) {
            throwFutureError(NEX_STD future_errc::no_state);
        }
        consumed_ = true;

        // If an exception was set, rethrow it to the caller.
        if (exception_) {
            NEX_STD rethrow_exception(exception_);
        }
    }

    // Check if the value is ready without blocking.
    bool isReady() const {
        LockGuard lock(mutex_);
        return ready_;
    }

private:
    // Helper function to create an exception pointer for a given future error code.
    static NEX_STD exception_ptr makeException(NEX_STD future_errc error) {
        return NEX_STD make_exception_ptr(
            NEX_STD future_error(error)
        );
    }

    // Helper function to throw a future_error with the given error code.
    static void throwFutureError(NEX_STD future_errc error) {
        throw NEX_STD future_error(error);
    }

    // Helper function to check if the Future is already ready (i.e., has an exception set or is marked as ready).
    // If it is ready, this function will throw a future_error with the promise_already_satisfied error code.
    void throwIfReady() const {
        if (ready_) {
            throwFutureError(NEX_STD future_errc::promise_already_satisfied);
        }
    }

    ////// Internal state variables for the Future --------------------------------------

    mutable Mutex mutex_;
    ConditionVariable condition_;
    NEX_STD exception_ptr exception_;
    bool ready_ = false;
    bool consumed_ = false;
};

/**
 * @struct PromiseLifetime
 * @brief Helper class to manage the lifetime of a Promise's state.
 * 
 * @details
 * This class is designed to ensure that if a Promise is destroyed without setting a value or an exception, 
 * the associated Future will be notified of a broken promise. It holds a shared pointer to the FutureState, 
 * and in its destructor, it checks if the state is still valid. If it is valid and the Future is not ready, 
 * it sets the broken promise state. This mechanism ensures that the Future will not wait indefinitely if 
 * the Promise is destroyed without fulfilling it, and it will receive a proper error instead. 
 * The Promise class will create an instance of this helper class to manage the lifetime of its state.
 * 
 * @tparam T The type of the value that the Promise is associated with.
 * 
 * @note 
 * The PromiseLifetime class is an implementation detail and is not intended to be used directly by users of 
 * the Future and Promise classes. It is used internally by the Promise class to manage the lifetime of the 
 * FutureState and to ensure proper error handling in case of a broken promise scenario.
 */
template<typename T>
struct NEX_INTERNAL PromiseLifetime {
    // Constructor that takes a shared pointer to the FutureState.
    explicit PromiseLifetime(SharedPtr<FutureState<T>> state)
        : state_(NEX_MOVE(state)) {
    }

    // Destructor that checks if the FutureState is still valid and sets the broken promise state if necessary.
    ~PromiseLifetime() {
        if (state_) {
            state_->setBrokenPromise();
        }
    }

    // The shared pointer to the FutureState that this PromiseLifetime manages.
    SharedPtr<FutureState<T>> state_;
};

/**
 * @class AsyncTask
 * @brief Represents an asynchronous task that will be executed and set the result in a Promise.
 *
 * @tparam ReturnType The type of the value that the task will produce.
 * @tparam Fn The type of the function to be executed asynchronously.
 * @tparam Args The types of the arguments to be passed to the function.
 */
template<typename ReturnType, typename Fn, typename... Args>
class NEX_INTERNAL AsyncTask {
public:
    // Constructor that takes a Promise, a function, and its arguments.
    AsyncTask(Promise<ReturnType> promise, Fn fn, Args... args)
        : promise_(NEX_MOVE(promise)),
          fn_(NEX_MOVE(fn)),
          args_(NEX_MOVE(args)...) {
    }

    // Run the task and set the result in the Promise.
    // This function will be called by the executor when the task is executed.
    void run() {
        try {
            // Invoke the function with the provided arguments and set the result in the Promise.
            if constexpr (NEX_STD is_void_v<ReturnType>) {
                NEX_STD apply(
                    [this](auto&... args) {
                        NEX_STD invoke(fn_, NEX_MOVE(args)...);
                    },
                    args_
                );
                promise_.setValue();
            } else {
                promise_.setValue(
                    NEX_STD apply(
                        [this](auto&... args) -> ReturnType {
                            return NEX_STD invoke(fn_, NEX_MOVE(args)...);
                        },
                        args_
                    )
                );
            }
        } catch (...) {
            promise_.setException(NEX_STD current_exception());
        }
    }

private:
    Promise<ReturnType> promise_;
    Fn fn_;
    NEX_STD tuple<Args...> args_;
};

NEX_DETAIL_NAMESPACE_END

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
class NEX_API Future {
public:
    // Default constructor creates an empty Future that is not associated with any state.
    Future() noexcept = default;

    // Default copy and move semantics
    NEX_DEFAULT_COPY_AND_MOVE(Future);

    // Get the value of the Future.
    // This will block until the value is available.
    T get() {
        if (!state_) {
            throw NEX_STD future_error(NEX_STD future_errc::no_state);
        }
        return state_->get();
    }

    // Check if the value is ready without blocking.
    bool isReady() const {
        return state_ && state_->isReady();
    }

private:
    // Allow Promise to access the private constructor that takes the shared state.
    friend class Promise<T>;

    // Private constructor used by Promise to create a Future associated with the given state.
    explicit Future(SharedPtr<NEX_DETAIL FutureState<T>> state)
        : state_(NEX_MOVE(state)) {
    }

    // Internal shared state of the Future, managed by the Promise.
    SharedPtr<NEX_DETAIL FutureState<T>> state_;
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
class NEX_API Promise {
public:
    // Default constructor creates a new Promise with an associated Future state.
    Promise()
        : state_(NEX_STD make_shared<NEX_DETAIL FutureState<T>>()),
          lifetime_(NEX_STD make_shared<NEX_DETAIL PromiseLifetime<T>>(state_)) {
    }

    // Default copy and move semantics
    NEX_DEFAULT_COPY_AND_MOVE(Promise);

    // Set the value of the Promise, which will make it available to the associated Future.
    void setValue(T value) {
        ensureState();
        state_->setValue(NEX_MOVE(value));
        lifetime_.reset();
    }

    // Set an exception for the Promise, which will be propagated to the associated Future.
    void setException(NEX_STD exception_ptr e) {
        ensureState();
        state_->setException(e);
        lifetime_.reset();
    }

    // Get the Future associated with this Promise.
    Future<T> getFuture() {
        ensureState();
        return Future<T>(state_);
    }

private:
    // Helper function to ensure that the Promise is still valid (i.e., it has an associated state).
    // If the state is null, this means the Promise has been moved from or is otherwise invalid,
    // and we should throw an exception.
    void ensureState() const {
        if (!state_) {
            throw NEX_STD future_error(NEX_STD future_errc::no_state);
        }
    }

    // Internal shared state of the Future, managed by this Promise.
    // This is shared with the Future to allow communication of the value or exception.
    SharedPtr<NEX_DETAIL FutureState<T>> state_;

    // Lifetime manager for the Promise's state.
    // This ensures that if the Promise is destroyed without setting a value or an exception,
    // the associated Future will be notified appropriately.
    SharedPtr<NEX_DETAIL PromiseLifetime<T>> lifetime_;
};

/**
 * @class Promise<void>
 * @brief Specialization of Promise for void type.
 *
 * @details
 * This specialization handles the case where the Future does not return a value.
 * It provides methods to set the completion or an exception for the associated Future.
 */
template<>
class NEX_API Promise<void> {
public:
    // Default constructor creates a new Promise with an associated Future state.
    Promise()
        : state_(NEX_STD make_shared<NEX_DETAIL FutureState<void>>()),
          lifetime_(NEX_STD make_shared<NEX_DETAIL PromiseLifetime<void>>(state_)) {
    }

    // Default copy and move semantics
    NEX_DEFAULT_COPY_AND_MOVE(Promise);

    // Set the value of the Promise, which will make it available to the associated Future.
    void setValue() {
        ensureState();
        state_->setValue();
        lifetime_.reset();
    }

    // Set an exception for the Promise, which will be propagated to the associated Future.
    void setException(NEX_STD exception_ptr e) {
        ensureState();
        state_->setException(e);
        lifetime_.reset();
    }

    // Get the Future associated with this Promise.
    Future<void> getFuture() {
        ensureState();
        return Future<void>(state_);
    }

private:
    // Helper function to ensure that the Promise is still valid (i.e., it has an associated state).
    // If the state is null, this means the Promise has been moved from or is otherwise invalid,
    // and we should throw an exception.
    void ensureState() const {
        if (!state_) {
            throw NEX_STD future_error(NEX_STD future_errc::no_state);
        }
    }

    // Internal shared state of the Future, managed by this Promise.
    // This is shared with the Future to allow communication of the completion or exception.
    SharedPtr<NEX_DETAIL FutureState<void>> state_;

    // Lifetime manager for the Promise's state.
    // This ensures that if the Promise is destroyed without setting a value or an exception,
    // the associated Future will be notified appropriately.
    SharedPtr<NEX_DETAIL PromiseLifetime<void>> lifetime_;
};

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
auto async(Executor& executor, Fn&& fn, Args&&... args)
    -> Future<InvokeResult<Fn, Args...>>
{
    using ReturnType = InvokeResult<Fn, Args...>;
    using Task = NEX_DETAIL AsyncTask<
        ReturnType,
        Decay<Fn>,
        Decay<Args>...
    >;

    Promise<ReturnType> promise;
    auto future = promise.getFuture();
    auto taskPromise = promise;
    auto task = NEX_STD make_shared<Task>(
        NEX_MOVE(taskPromise),
        NEX_FORWARD<Fn>(fn),
        NEX_FORWARD<Args>(args)...
    );

    try {
        executor.execute([task = NEX_MOVE(task)]() mutable {
            task->run();
        });
    } catch (...) {
        promise.setException(NEX_STD current_exception());
    }

    return future;
}

NEX_NAMESPACE_END
