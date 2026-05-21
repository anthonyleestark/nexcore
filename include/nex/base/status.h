/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/error.h"

NEX_NAMESPACE_BEGIN

/**
 * @class   Status
 * @brief   Simple status representation for operation results
 * 
 * This class represents the status of an operation, indicating success or failure.
 * In case of failure, it holds an Error object with details.
 * 
 * Example usage:
 * ```
 * Status doSomething() {
 *   // ... perform operation ...
 *   if (failure) {
 *       return Status::error(ErrorCode::InvalidArgument);
 *   }
 *   return Status::ok();
 * }
 * ```
 */
class NEX_API NEX_NODISCARD Status {
public:
    // Create a successful status
    static Status ok() noexcept {
        return Status();
    }

    // Create an error status
    static Status error(ErrorCode code) noexcept {
        Status status;
        status.isOk_ = false;
        status.error_.code = code;
        status.error_.message = errorCodeToString(code);
        return status;
    }
    static Status error(Error error) noexcept {
        Status status;
        status.isOk_ = false;
        status.error_ = error;
        return status;
    }

    // Check if status is okay
    constexpr bool isOk() const noexcept { return isOk_; }
    constexpr explicit operator bool() const noexcept { return isOk_; }

    // Get error information
    ErrorCode code() const noexcept {
        if (isOk_) {
            return ErrorCode::Ok;
        }
        return error_.code;
    }
    const Error& error() const noexcept {
        if (isOk_) {
            static const Error success = { 
                ErrorCode::Ok,       // No error code for success
                "Ok"                 // Message indicating success
            };
            return success;
        }
        return error_;
    }

private:
    // Prevent public default construction; enforce factories
    Status() noexcept = default;
    
    // Ok status flag
    bool isOk_ = true;

    // Error status
    Error error_;
};

NEX_NAMESPACE_END
