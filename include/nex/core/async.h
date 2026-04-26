/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <future>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/core/functional.h"
#include "nex/core/executor.h"

NEX_NAMESPACE_BEGIN

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

    Promise<ReturnType> promise;
    auto future = promise.getFuture();
    executor.execute(
        [p = NEX_STD move(promise),
         f = NEX_STD forward<Fn>(fn),
         ...a = NEX_STD forward<Args>(args)]() mutable {
            try {
                if constexpr (NEX_STD is_void_v<ReturnType>) {
                    NEX_STD invoke(f, NEX_STD move(a)...);
                    p.setValue();
                } else {
                    p.setValue(NEX_STD invoke(f, NEX_STD move(a)...));
                }
            } catch (...) {
                p.setException(NEX_STD current_exception());
            }
        }
    );

    return future;
}

NEX_NAMESPACE_END