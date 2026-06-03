/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <memory>
#include <functional>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/string.h"
#include "nex/base/error.h"

NEX_NAMESPACE_BEGIN

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

    // Check if the status is OK (without error)
    constexpr bool isOk() const noexcept { return isOk_; }
    constexpr explicit operator bool() const noexcept { return isOk_; }

    // Get the error code (returns ErrorCode::Ok if status is successful)
    constexpr ErrorCode code() const noexcept {
        return isOk_ ? ErrorCode::Ok : storage_.error_.code;
    }

    // Get the error message (returns "Ok" if status is successful)
    constexpr Error error() const noexcept {
        if (isOk_) {
            return {
                .code = ErrorCode::Ok,    // No error code for successful/okay status
                .message = "Ok"           // Message indicating successful/okay status
            };
        }
        return storage_.error_;
    }

    // Execute a function if the status is OK, otherwise propagate the error
    template <typename Func>
    constexpr auto andThen(Func&& func) const noexcept -> decltype(func()) {
        if (isOk_) return NEX_STD invoke(NEX_FORWARD(Func, func));
        return Status(Unexpected { storage_.error_ });
    }

private:
    // Internal tag type for representing an unexpected error
    struct Unexpected {
        Error error;
    };

    // Storage for either a successful status (no error) or an error status (with error details)
    union NEX_ALIGNAS(alignof(Error)) {
        nchar dummy_;    // Dummy member to allow default construction of the union; not used for actual storage
        Error error_;    // Error information for failure cases; valid only if isOk_ is false
    } storage_;

    // Flag indicating whether the status is OK (true) or an error (false)
    bool isOk_ = true;

    // Constructs a successful status (default)
    constexpr Status() noexcept : isOk_(true) {
        NEX_STD construct_at(&storage_.dummy_);  // Construct the dummy member for successful status
    }

    // Constructs a Status object with an unexpected error
    constexpr Status(Unexpected unexpected) noexcept : isOk_(false) {
        NEX_STD construct_at(&storage_.error_, NEX_MOVE(unexpected.error));
    }

    // Copy the contents of another Status object into this one
    constexpr void copyStatus(const Status& other) noexcept {
        isOk_ = other.isOk_;
        if (isOk_) {
            // Construct the dummy member for successful status
            NEX_STD construct_at(&storage_.dummy_);
        } else {
            // Copy the error information from the other Status object
            NEX_STD construct_at(&storage_.error_, other.storage_.error_);
        }
    }

    // Move the contents of another Status object into this one
    constexpr void moveStatus(Status&& other) noexcept {
        isOk_ = other.isOk_;
        if (isOk_) {
            // Construct the dummy member for successful status
            NEX_STD construct_at(&storage_.dummy_);
        } else {
            // Move the error information from the other Status object
            NEX_STD construct_at(&storage_.error_, NEX_MOVE(other.storage_.error_));
        }
    }

    // Destroy the existing error information if this Status holds an error
    constexpr void destroy() noexcept {
        if (!isOk_) {
            NEX_STD destroy_at(&storage_.error_);
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
            destroy();
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
            destroy();
            moveStatus(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor
    constexpr ~Status() {
        destroy();
    }
};

NEX_NAMESPACE_END
