/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <utility>

#include "nex/base/types.h"
#include "nex/base/error.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @class Result
 * @brief Template class to encapsulate operation results with success/failure status
 * 
 * This class provides a convenient way to represent the result of an operation,
 * including whether it succeeded or failed, along with an optional value or error code.
 * 
 * Result supports:
 * - Construction for success and failure cases
 * - Checking success/failure status
 * - Retrieving the value or error code
 * - Comparison operations
 * - Utility functions for common patterns
 * 
 * @tparam T The type of the value on success
 * @tparam E The type of the error on failure
 * 
 * @note This class is similar in concept to std::expected (C++23) or Result types in other languages.
 * 
 * Example usage:
 * ```
 * Result<int, Error> divide(int a, int b) {
 *     if (b == 0) {
 *         return Result<int, ErrorCode>::error(ErrorCode::InvalidArgument);
 *     }
 *     return Result<int, ErrorCode>::ok(a / b);
 * }
 * ```
 */
template<typename T, typename E = Error>
class NEX_EXPORT NEX_NODISCARD Result {
public:
    // Create a successful result
    static Result ok(T value) {
        Result result;
        result.isOk_ = true;
        new (&result.value_) T(NEX_STD move(value));
        return result;
    }
    
    // Create an error result
    static Result error(E error) {
        Result result;
        result.isOk_ = false;
        new (&result.error_) E(NEX_STD move(error));
        return result;
    }
    
    // Check if result is successful
    bool isOk() const { return isOk_; }
    explicit operator bool() const noexcept { return isOk_; }
    
    // Get the success value (crash if error)
    T& value() {
        if (!isOk_) {
            NEX_FATAL(
                "Attempted to access value of error Result");
        }
        return value_;
    }
    
    // Get the success value (const, crash if error)
    const T& value() const {
        if (!isOk_) {
            NEX_FATAL(
                "Attempted to access value of error Result");
        }
        return value_;
    }

    // Get the success value or a default
    T valueOr(T defaultValue) const {
        if (isOk_) {
            return value_;
        }
        return defaultValue;
    }
    
    // Get the error value (crash if success)
    E& error() {
        if (isOk_) {
            NEX_FATAL(
                "Attempted to access error of success Result");
        }
        return error_;
    }
    
    // Get the error value (const, crash if success)
    const E& error() const {
        if (isOk_) {
            NEX_FATAL(
                "Attempted to access error of success Result");
        }
        return error_;
    }

    // Try to get the success value pointer (returns a nullptr if error)
    const T* tryValue() const noexcept {
        return isOk_ ? &value_ : nullptr;
    }

    // Try to get the error value pointer (returns a nullptr if success)
    const E* tryError() const noexcept {
        return isOk_ ? nullptr : &error_;
    }

    // Properly destroy active union member
    ~Result() {
        if (isOk_) {
            value_.~T();
        } else {
            error_.~E();
        }
    }

private:
    // Prevent public default construction; enforce factories
    Result() noexcept = default;

    // Disable copy semantics
    NEX_DISALLOW_COPY(Result);

    // Move constructor and assignment
    Result(Result&&) = default;
    Result& operator=(Result&&) = default;

    // Success or error flag
    bool isOk_ = false;

    // We use a union to store either the value or the error,
    // along with a flag indicating which is active,
    // instead of std::variant or two std::optionals.
    // This provides a clearer memory layout, reduced overhead,
    // and prevents invalid states.
    union {
        T value_;
        E error_;
    };
};

/**
 * @class Result<void, E>
 * @brief Specialization of Result class for void type
 * 
 * This specialization of the Result class handles operations that do not return a value.
 * It encapsulates only the success/failure status and the error code.
 * 
 * @tparam E The type of the error on failure
 * 
 * @note This class is similar in concept to std::expected<void, E> (C++23) or Result types in other languages.
 * 
 * Example usage:
 * ```
 * Result<void, Error> performOperation() {
 *    if (someConditionFails) {
 *       return Result<void, Error>::error(ErrorCode::OperationFailed);
 *   }
 *   return Result<void, Error>::ok();
 * }
 * ```
 */
template<typename E>
class NEX_EXPORT NEX_NODISCARD Result<void, E> {
public:
    // Create a successful result
    static Result<void, E> ok() {
        Result<void, E> result;
        result.isOk_ = true;
        return result;
    }
    
    // Create an error result
    static Result<void, E> error(E error) {
        Result<void, E> result;
        result.error_ = error;
        result.isOk_ = false;
        return result;
    }
    
    // Check if result is successful
    bool isOk() const { return isOk_; }
    explicit operator bool() const noexcept { return isOk_; }
    
    // Get the error value (crash if success)
    E& error() {
        if (isOk_) {
            NEX_FATAL(
                "Attempted to access error of success Result");
        }
        return error_;
    }
    
    // Get the error value (const, crash if success)
    const E& error() const {
        if (isOk_) {
            NEX_FATAL(
                "Attempted to access error of success Result");
        }
        return error_;
    }

    // Try to get the error value pointer (returns a nullptr if success)
    const E* tryError() const noexcept {
        return isOk_ ? nullptr : &error_;
    }

private:
    // Prevent public default construction; enforce factories
    Result() noexcept = default;

    // Disable copy semantics
    NEX_DISALLOW_COPY(Result);

    // Move constructor and assignment operator
    Result(Result&&) = default;
    Result& operator=(Result&&) = default;

    // Success flag
    bool isOk_ = false;

    // We only need to store the error in this specialization
    // since there is no success value. We do not use std::optional
    // here either to avoid unnecessary overhead.
    E error_;
};

NEX_NAMESPACE_END
