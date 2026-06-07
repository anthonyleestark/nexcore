/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <memory>

#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/error.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @note
 * The NEX_REPORT_RESULT_INVALID_ACCESS macro is specialized for the Result class to handle invalid access 
 * attempts when trying to retrieve a value from a Result that is in an error state, or trying to retrieve 
 * an error from a Result that is in a success state. 
 * In such cases, it will either throw an exception with the provided message if in a constexpr context, 
 * or it will call NEX_FATAL to crash the program with the message. 
 * This ensures that invalid access attempts are caught and handled appropriately, preventing undefined behavior 
 * and providing clear feedback about the nature of the error.
 * The use of a separate non-constexpr handler allows for more efficient error handling in runtime contexts 
 * while still providing safety in compile-time contexts.
 */
#if !defined(NEX_REPORT_RESULT_INVALID_ACCESS)
    NEX_HIDDEN_FROM_ABI inline 
    void __runtimeResultInvalidAccessHandler(cstring msg) { NEX_FATAL(msg); }
    #define NEX_REPORT_RESULT_INVALID_ACCESS(msg) \
        do { \
            if NEX_CONSTEVAL_CONTEXT { \
                throw(msg); \
            } else { \
                __runtimeResultInvalidAccessHandler(msg); \
            } \
        } while (0)
#endif  // !defined(NEX_REPORT_RESULT_INVALID_ACCESS)


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
 * Result<int32> divide(int32 a, int32 b) {
 *     if (b == 0) {
 *         return Result<int32>::error({
 *             ErrorCode::InvalidArgument, "Division by zero is not allowed"
 *         });
 *     }
 *     return Result<int32>::ok(a / b);
 * }
 * ```
 */
template<typename ReturnType, typename ErrorType = Error>
class NEX_API NEX_NODISCARD Result {
public:
    // Create a successful result with an expected value
    static constexpr Result ok(ReturnType value) noexcept {
        return Result(Expected { NEX_MOVE(value) });
    }

    // Create a successful result with perfect forwarding of arguments to construct the value
    template<typename... Args>
    static constexpr Result ok(Args&&... args) noexcept {
        return Result(Expected { ReturnType(NEX_FORWARD<Args>(args)...) });
    }

    // Create an error result with an unexpected error
    static constexpr Result error(ErrorType error) noexcept {
        return Result(Unexpected { NEX_MOVE(error) });
    }

    // Create an error result with perfect forwarding of arguments to construct the error
    template<typename... Args>
    static constexpr Result error(Args&&... args) noexcept {
        return Result(Unexpected { ErrorType(NEX_FORWARD<Args>(args)...) });
    }

    // Check if the result is successful
    constexpr bool isOk() const noexcept { return isOk_; }
    constexpr explicit operator bool() const noexcept { return isOk_; }

    // Get the success value (crash if result is an error)
    constexpr ReturnType& value() {
        if (!isOk_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return storage_.value_;
    }

    // Get the success value (const, crash if result is an error)
    constexpr const ReturnType& value() const noexcept {
        if (!isOk_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return storage_.value_;
    }

    // Get the success value or a default
    constexpr ReturnType valueOr(ReturnType defaultValue) const noexcept {
        if (isOk_) {
            return storage_.value_;
        }
        return defaultValue;
    }

    // Get the error value (crash if result is successful)
    constexpr ErrorType& error() noexcept {
        if (isOk_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return storage_.error_;
    }

    // Get the error value (const, crash if result is successful)
    constexpr const ErrorType& error() const noexcept {
        if (isOk_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return storage_.error_;
    }

    // Try to get the success value pointer (returns a nullptr if result is an error)
    constexpr const ReturnType* tryValue() const noexcept {
        return isOk_ ? &storage_.value_ : nullptr;
    }

    // Try to get the error value pointer (returns a nullptr if result is successful)
    constexpr const ErrorType* tryError() const noexcept {
        return isOk_ ? nullptr : &storage_.error_;
    }

private:
    // Represents a successful result with a value
    struct Expected {
        ReturnType value;
    };

    // Represents an unexpected error result
    struct Unexpected {
        ErrorType error;
    };

    // Storage for either the success value or the error, using a union to avoid unnecessary overhead.
    // Aligned to the maximum alignment requirement of either ReturnType or ErrorType to ensure proper storage.
    static constexpr usize StorageAlignment = meta::MaxAlignOfV<ReturnType, ErrorType>;
    union NEX_ALIGNAS(StorageAlignment) {
        ReturnType value_;
        ErrorType error_;
    } storage_;

    // Flag indicating whether the result is successful (true) or an error (false)
    bool isOk_ = false;

    // Construct a successful Result with an expected value
    constexpr Result(Expected expected) noexcept : isOk_(true) {
        NEX_STD construct_at(&storage_.value_, NEX_MOVE(expected.value));
    }

    // Construct an error Result with an unexpected error
    constexpr Result(Unexpected unexpected) noexcept : isOk_(false) {
        NEX_STD construct_at(&storage_.error_, NEX_MOVE(unexpected.error));
    }

    // Copy the contents of another Result object into this one
    constexpr void copyStatus(const Result& other) noexcept {
        isOk_ = other.isOk_;
        if (isOk_) {
            // Copy the success value from the other Result object
            NEX_STD construct_at(&storage_.value_, other.value_);
        } else {
            // Copy the error information from the other Result object
            NEX_STD construct_at(&storage_.error_, other.error_);
        }
    }

    // Move the contents of another Result object into this one
    constexpr void moveStatus(Result&& other) noexcept {
        isOk_ = other.isOk_;
        if (isOk_) {
            // Move the success value from the other Result object
            NEX_STD construct_at(&storage_.value_, NEX_MOVE(other.value_));
        } else {
            // Move the error information from the other Result object
            NEX_STD construct_at(&storage_.error_, NEX_MOVE(other.error_));
        }
    }

    // Destroy the existing value or error if this Result holds one
    constexpr void destroy() noexcept {
        if (isOk_) {
            NEX_STD destroy_at(&storage_.value_);
        } else {
            NEX_STD destroy_at(&storage_.error_);
        }
    }

public:
    // Copy constructor for copying a Result object
    constexpr Result(const Result& other) noexcept {
        copyStatus(other);
    }

    // Copy assignment operator for copying a Result object
    constexpr Result& operator=(const Result& other) noexcept {
        if (this != &other) {
            destroy();
            copyStatus(other);
        }
        return *this;
    }

    // Constructor for moving a Result object
    constexpr Result(Result&& other) noexcept {
        moveStatus(NEX_MOVE(other));
    }

    // Move assignment operator for moving a Result object
    constexpr Result& operator=(Result&& other) noexcept {
        if (this != &other) {
            destroy();
            moveStatus(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor to clean up the contained value or error
    constexpr ~Result() {
        destroy();
    }
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
    // Create a successful result with no return value
    static constexpr Result ok() noexcept {
        return Result();
    }
    
    // Create an error result with an unexpected error
    static constexpr Result error(ErrorType error) noexcept {
        return Result(Unexpected { NEX_MOVE(error) });
    }

    // Create an error result with perfect forwarding of arguments to construct the error
    template<typename... Args>
    static constexpr Result error(Args&&... args) noexcept {
        return Result(Unexpected { ErrorType(NEX_FORWARD<Args>(args)...) });
    }

    // Check if result is successful
    constexpr bool isOk() const noexcept { return isOk_; }
    constexpr explicit operator bool() const noexcept { return isOk_; }

    // Get the error value (crash if result is successful)
    constexpr ErrorType& error() noexcept {
        if (isOk_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return storage_.error_;
    }

    // Get the error value (const, crash if result is successful)
    constexpr const ErrorType& error() const noexcept {
        if (isOk_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return storage_.error_;
    }

    // Try to get the error value pointer (returns a nullptr if result is successful)
    constexpr const ErrorType* tryError() const noexcept {
        return isOk_ ? nullptr : &storage_.error_;
    }

private:
    // Represents an unexpected error result
    struct Unexpected {
        ErrorType error;
    };

    // Storage for either a successful result (no value) or an error result (with error details)
    union NEX_ALIGNAS(alignof(ErrorType)) {
        nchar dummy_;       // Dummy member to allow default construction of the union; not used for actual storage
        ErrorType error_;   // Error information for failure cases; valid only if isOk_ is false
    } storage_;

    // Flag indicating whether the result is successful (true) or an error (false)
    bool isOk_ = false;

    // Construct a successful Result with an expected value
    constexpr Result() noexcept : isOk_(true) {
        NEX_STD construct_at(&storage_.dummy_, nchar{});
    }

    // Construct an error Result with an unexpected error
    constexpr Result(Unexpected unexpected) noexcept : isOk_(false) {
        NEX_STD construct_at(&storage_.error_, NEX_MOVE(unexpected.error));
    }

    // Copy the contents of another Result object into this one
    constexpr void copyResult(const Result& other) noexcept {
        isOk_ = other.isOk_;
        if (isOk_) {
            // Construct the dummy member for successful result
            NEX_STD construct_at(&storage_.dummy_, nchar{});
        } else {
            // Copy the error information from the other Result object
            NEX_STD construct_at(&storage_.error_, other.storage_.error_);
        }
    }

    // Move the contents of another Result object into this one
    constexpr void moveResult(Result&& other) noexcept {
        isOk_ = other.isOk_;
        if (isOk_) {
            // Construct the dummy member for successful result
            NEX_STD construct_at(&storage_.dummy_, nchar{});
        } else {
            // Move the error information from the other Result object
            NEX_STD construct_at(&storage_.error_, NEX_MOVE(other.storage_.error_));
        }
    }

    // Destroy the existing error information if this Result holds an error
    constexpr void destroy() noexcept {
        if (!isOk_) {
            NEX_STD destroy_at(&storage_.error_);
        }
    }

public:
    // Copy constructor for copying a Result object
    constexpr Result(const Result& other) noexcept {
        copyResult(other);
    }

    // Copy assignment operator for copying a Result object
    constexpr Result& operator=(const Result& other) noexcept {
        if (this != &other) {
            destroy();
            copyResult(other);
        }
        return *this;
    }

    // Constructor for moving a Result object
    constexpr Result(Result&& other) noexcept {
        moveResult(NEX_MOVE(other));
    }

    // Move assignment operator for moving a Result object
    constexpr Result& operator=(Result&& other) noexcept {
        if (this != &other) {
            destroy();
            moveResult(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor
    constexpr ~Result() {
        destroy();
    }
};

NEX_NAMESPACE_END
