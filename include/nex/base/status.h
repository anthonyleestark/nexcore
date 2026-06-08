/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <memory>
#include <functional>

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/string.h"
#include "nex/base/error.h"

NEX_NAMESPACE_BEGIN

// Forward declaration of the Status class
class Status;

// Traits to identify if a type is a Status
template <typename Type> 
struct IsStatus : meta::FalseType {};

template <> 
struct IsStatus<Status> : meta::TrueType {};

template <typename Type> 
inline constexpr bool IsStatusV = IsStatus<Type>::value;

/**
 * @class   Status
 * @brief   Represents the result of an operation, indicating success or failure, 
 *          and providing error details if applicable.
 * 
 * @details
 * The `Status` class is designed to encapsulate the outcome of an operation, allowing functions 
 * to return a `Status` object that indicates success or failure. This approach provides a clear 
 * and consistent way to handle errors without relying on exceptions.
 * 
 * Example usage:
 * @code
 * Status doSomething() {
 *   // ... perform operation ...
 *   if (failure) {
 *       return Status::error(ErrorCode::InvalidArgument);
 *   }
 *   return Status::ok();
 * }
 * @endcode
 */
class NEX_API NEX_NODISCARD Status {
public:
    // Create a successful status
    static constexpr Status ok() noexcept {
        return Status();
    }

    // Create an error status with a specific error code
    static constexpr Status error(ErrorCode code) noexcept {
        return Status(Unexpected {{ code, errorCodeToString(code) }});
    }

    // Create an error status with a specific error code and message
    static constexpr Status error(ErrorCode code, StdString message) noexcept {
        return Status(Unexpected {{ code, message.c_str() }});
    }

    // Create an error status from an existing Error object
    static constexpr Status error(Error error) noexcept {
        return Status(Unexpected { NEX_MOVE(error) });
    }

    // Check if the status is successful
    constexpr bool isSuccess() const noexcept { return isSuccess_; }
    constexpr explicit operator bool() const noexcept { return isSuccess_; }

    // Get the error code (returns ErrorCode::Ok if status is successful)
    constexpr ErrorCode code() const noexcept {
        return isSuccess_ ? ErrorCode::Ok : storage_.error_.code;
    }

    // Get the error if the status is an error, otherwise returns a default "Ok" error
    constexpr Error error() const noexcept {
        if (isSuccess_) {
            return {
                .code = ErrorCode::Ok,    // No error code for successful status
                .message = "Ok"           // Message indicating successful status
            };
        }
        return storage_.error_;
    }

    // Execute a function if the status is successful, otherwise propagate the error
    template <typename Func>
    constexpr auto andThen(Func&& func) const noexcept -> decltype(func()) {
        using ReturnType = NEX_STD invoke_result_t<Func>;
        static_assert(IsStatusV<ReturnType>, "Error: andThen function must return a Status");
        if (isSuccess_) return NEX_STD invoke(NEX_FORWARD<Func>(func));
        return Status(Unexpected { storage_.error_ });
    }

    // Execute a function if the status is an error, otherwise propagate the success status
    template <typename Func>
    constexpr auto orElse(Func&& func) const noexcept -> decltype(func()) {
        using ReturnType = NEX_STD invoke_result_t<Func, const Error&>;
        static_assert(IsStatusV<ReturnType>, "Error: orElse function must return a Status");
        if (!isSuccess_) return NEX_STD invoke(NEX_FORWARD<Func>(func), storage_.error_);
        return Status::ok();
    }

private:
    // Represents an unexpected error
    struct Unexpected {
        Error error;
    };

    // Storage for either a successful status (no error) or an error status (with error details)
    union NEX_ALIGNAS(alignof(Error)) {
        monostate success_;     // Represents a successful status
        Error error_;           // Error information for failure cases
    } storage_;

    // Flag indicating whether the status is successful (true) or an error (false)
    bool isSuccess_ = true;

    // Constructs a successful status (default)
    constexpr Status() noexcept : isSuccess_(true) {
        NEX_STD construct_at(NEX_ADDRESS_OF(storage_.success_), monostate{});
    }

    // Constructs a Status object with an unexpected error
    constexpr Status(Unexpected unexpected) noexcept : isSuccess_(false) {
        NEX_STD construct_at(NEX_ADDRESS_OF(storage_.error_), NEX_MOVE(unexpected.error));
    }

    // Copy the contents of another Status object into this one
    constexpr void copyStatus(const Status& other) noexcept {
        isSuccess_ = other.isSuccess_;
        if (isSuccess_) {
            // Construct the dummy member for successful status
            NEX_STD construct_at(NEX_ADDRESS_OF(storage_.success_), monostate{});
        } else {
            // Copy the error information from the other Status object
            NEX_STD construct_at(NEX_ADDRESS_OF(storage_.error_), other.storage_.error_);
        }
    }

    // Move the contents of another Status object into this one
    constexpr void moveStatus(Status&& other) noexcept {
        isSuccess_ = other.isSuccess_;
        if (isSuccess_) {
            // Construct the dummy member for successful status
            NEX_STD construct_at(NEX_ADDRESS_OF(storage_.success_), monostate{});
        } else {
            // Move the error information from the other Status object
            NEX_STD construct_at(NEX_ADDRESS_OF(storage_.error_), NEX_MOVE(other.storage_.error_));
        }
    }

    // Destroy the existing error information if this Status holds an error
    constexpr void destroyExistingError() noexcept {
        if (!isSuccess_) {
            NEX_STD destroy_at(NEX_ADDRESS_OF(storage_.error_));
        }
    }

public:
    // Copy constructor for copying a Status object
    constexpr Status(const Status& other) noexcept {
        copyStatus(other);
    }

    // Copy assignment operator for copying a Status object
    constexpr Status& operator=(const Status& other) noexcept {
        if (this != &other) {
            destroyExistingError();
            copyStatus(other);
        }
        return *this;
    }

    // Constructor for moving a Status object
    constexpr Status(Status&& other) noexcept {
        moveStatus(NEX_MOVE(other));
    }

    // Move assignment operator for moving a Status object
    constexpr Status& operator=(Status&& other) noexcept {
        if (this != &other) {
            destroyExistingError();
            moveStatus(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor
    constexpr ~Status() {
        destroyExistingError();
    }
};

NEX_NAMESPACE_END
