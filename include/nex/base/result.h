/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/error.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @class Result
 * @brief Represents the result of an operation that can either succeed with a value or fail with an error.
 * 
 * @details
 * This template class provides a convenient way to represent the result of an operation, including whether 
 * it succeeded or failed, along with an optional value on success or an error on failure. It is designed 
 * to be used in functions that may fail and need to return detailed error information without using exceptions.
 * 
 * Result supports:
 * - Construction of successful results with a value.
 * - Construction of error results with an error.
 * - Checking if the result is successful or an error.
 * - Accessing the value or error, with safety checks that will crash if accessed incorrectly.
 * - A specialization for void return type, which only indicates success or failure without returning any value.
 * - Try-get methods that return pointers to the value or error, allowing for safe access without crashing.
 * 
 * @tparam ReturnType The type of the return value on success
 * @tparam ErrorType The type of the error on failure
 * 
 * @note This class is similar in concept to std::expected (C++23) or Result types in other languages.
 * 
 * Example usage:
 * ```
 * Result<int> divide(int a, int b) {
 *     if (b == 0) {
 *         return Result<int>::error({
 *             ErrorCode::InvalidArgument, "Division by zero is not allowed"
 *         });
 *     }
 *     return Result<int>::ok(a / b);
 * }
 * ```
 */
template<typename ReturnType, typename ErrorType = Error>
class NEX_API NEX_NODISCARD Result {
public:
    // Create a successful result
    static Result ok(ReturnType value) {
        Result result;
        result.isOk_ = true;
        ::new (&result.value_) ReturnType(NEX_MOVE(value));
        return result;
    }
    
    // Create an error result
    static Result error(ErrorType error) {
        Result result;
        result.isOk_ = false;
        ::new (&result.error_) ErrorType(NEX_MOVE(error));
        return result;
    }
    
    // Check if result is successful
    bool isOk() const { return isOk_; }
    explicit operator bool() const noexcept { return isOk_; }
    
    // Get the success value (crash if result is an error)
    ReturnType& value() {
        if (!isOk_) {
            NEX_FATAL(
                "Attempted to access value of error Result");
        }
        return value_;
    }
    
    // Get the success value (const, crash if result is an error)
    const ReturnType& value() const {
        if (!isOk_) {
            NEX_FATAL(
                "Attempted to access value of error Result");
        }
        return value_;
    }

    // Get the success value or a default
    ReturnType valueOr(ReturnType defaultValue) const {
        if (isOk_) {
            return value_;
        }
        return defaultValue;
    }
    
    // Get the error value (crash if result is successful)
    ErrorType& error() {
        if (isOk_) {
            NEX_FATAL(
                "Attempted to access error of success Result");
        }
        return error_;
    }
    
    // Get the error value (const, crash if result is successful)
    const ErrorType& error() const {
        if (isOk_) {
            NEX_FATAL(
                "Attempted to access error of success Result");
        }
        return error_;
    }

    // Try to get the success value pointer (returns a nullptr if result is an error)
    const ReturnType* tryValue() const noexcept {
        return isOk_ ? &value_ : nullptr;
    }

    // Try to get the error value pointer (returns a nullptr if result is successful)
    const ErrorType* tryError() const noexcept {
        return isOk_ ? nullptr : &error_;
    }

    // Destroy the Result and its contained value or error
    ~Result() {
        if (isOk_) {
            value_.~ReturnType();
        } else {
            error_.~ErrorType();
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

    // A flag to indicate whether the result is successful (true) or an error (false)
    bool isOk_ = false;

    // We use a union to store either the value or the error, but not both at the same time. 
    // This allows us to avoid the overhead of std::optional and manage the lifetime of 
    // the contained objects manually.
    union {
        ReturnType value_;
        ErrorType error_;
    };
};

/**
 * @class Result<void, ErrorType>
 * @brief Specialization of Result for operations that do not return a value, only success/failure status and error.
 * 
 * @details
 * This specialization of Result is designed for functions that do not return a value on success, but still need to 
 * indicate success or failure and provide error information. It provides the same interface for checking success 
 * and accessing errors, but does not store a value on success. This is useful for operations that are primarily 
 * about side effects and do not produce a meaningful return value.
 * 
 * @tparam ErrorType The type of the error on failure
 * 
 * @note This class is similar in concept to std::expected<void, ErrorType> (C++23) or Result types in other languages.
 * 
 * Example usage:
 * ```
 * Result<void, ErrorCode> performOperation() {
 *    if (someConditionFails) {
 *       return Result<void, ErrorCode>::error(ErrorCode::OperationFailed);
 *   }
 *   return Result<void, ErrorCode>::ok();
 * }
 * ```
 */
template<typename ErrorType>
class NEX_API NEX_NODISCARD Result<void, ErrorType> {
public:
    // Create a successful result
    static Result<void, ErrorType> ok() {
        Result<void, ErrorType> result;
        result.isOk_ = true;
        return result;
    }
    
    // Create an error result
    static Result<void, ErrorType> error(ErrorType error) {
        Result<void, ErrorType> result;
        result.error_ = error;
        result.isOk_ = false;
        return result;
    }
    
    // Check if result is successful
    bool isOk() const { return isOk_; }
    explicit operator bool() const noexcept { return isOk_; }
    
    // Get the error value (crash if result is successful)
    ErrorType& error() {
        if (isOk_) {
            NEX_FATAL(
                "Attempted to access error of success Result");
        }
        return error_;
    }
    
    // Get the error value (const, crash if result is successful)
    const ErrorType& error() const {
        if (isOk_) {
            NEX_FATAL(
                "Attempted to access error of success Result");
        }
        return error_;
    }

    // Try to get the error value pointer (returns a nullptr if result is successful)
    const ErrorType* tryError() const noexcept {
        return isOk_ ? nullptr : &error_;
    }

    // Destroy the Result and its contained error
    ~Result() {
        if (!isOk_) {
            error_.~ErrorType();
        }
    }

private:
    // Prevent public default construction; enforce factories
    Result() noexcept = default;

    // Disable copy semantics
    NEX_DISALLOW_COPY(Result);

    // Move constructor and assignment operator
    Result(Result&&) = default;
    Result& operator=(Result&&) = default;

    // A flag to indicate whether the result is successful (true) or an error (false)
    bool isOk_ = false;

    // We only need to store the error in this specialization since there is no success value. 
    // We do not use std::optional here either to avoid unnecessary overhead.
    ErrorType error_;
};

NEX_NAMESPACE_END
