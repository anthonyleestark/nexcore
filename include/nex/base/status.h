/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <memory>

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/invoke.h"
#include "nex/base/error.h"
#include "nex/base/string.h"

NEX_NAMESPACE_BEGIN

// Forward declaration of the Status class
class Status;

// Determine if a type is a Status, returns false if the type is not a Status.
template <typename Type>
struct IsStatus : meta::FalseType {};

// Determine if a type is a Status, returns true if the type is a Status
template <>
struct IsStatus<Status> : meta::TrueType {};

// Variable template for easier usage of IsStatus trait
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
    using value_type = void;
    using error_type = Error;

    // Create a successful status
    static constexpr Status ok() noexcept {
        return Status(in_place);
    }

    // Create an error status from an unexpected error
    static constexpr Status error(const error_type& error) noexcept {
        return Status(unexpect, error);
    }

    // Create an error status from an unexpected error
    static constexpr Status error(error_type&& error) noexcept {
        return Status(unexpect, NEX_MOVE(error));
    }

    // Create an error status with a specific error code
    static constexpr Status error(ErrorCode code) noexcept {
        return Status(unexpect, error_type(code));
    }

    // Create an error status with a specific error code and message
    static constexpr Status error(ErrorCode code, StdStringView message) noexcept {
        return Status(unexpect, error_type(code, message.data()));
    }

    // Check if the status is successful
    constexpr bool isOk() const noexcept { return isOk_; }
    constexpr explicit operator bool() const noexcept { return isOk_; }

    // Get the error if the status is an error, otherwise returns a default "Ok" error
    constexpr error_type error() const noexcept {
        return isOk_ ? error_type(ErrorCode::Ok) : storage_.error_;
    }

    // Try to get the error value pointer (returns a nullptr if status is successful)
    constexpr const error_type* tryError() const noexcept {
        return isOk_ ? nullptr : &storage_.error_;
    }

    // Get the error code (returns ErrorCode::Ok if status is successful)
    constexpr ErrorCode code() const noexcept {
        return isOk_ ? ErrorCode::Ok : storage_.error_.code;
    }

    // Execute a function if the status is successful, otherwise propagate the error
    template <typename Func>
    constexpr auto andThen(Func&& func) & noexcept {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func>>;
        static_assert(IsStatusV<ReturnType>, "The result of the andThen callback must be a Status");
        return isOk() ? invoke(NEX_FORWARD<Func>(func)) : Status(unexpect, storage_.error_);
    }

    // Execute a function if the status is successful, otherwise propagate the error (const version)
    template <typename Func>
    constexpr auto andThen(Func&& func) const& noexcept {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func>>;
        static_assert(IsStatusV<ReturnType>, "The result of the andThen callback must be a Status");
        return isOk() ? invoke(NEX_FORWARD<Func>(func)) : Status(unexpect, storage_.error_ );
    }

    // Execute a function if the status is successful, otherwise propagate the error (rvalue version)
    template <typename Func>
    constexpr auto andThen(Func&& func) && noexcept {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func>>;
        static_assert(IsStatusV<ReturnType>, "The result of the andThen callback must be a Status");
        return isOk() ? invoke(NEX_FORWARD<Func>(func)) : Status(unexpect, NEX_MOVE(storage_.error_));
    }

    // Execute a function if the status is successful, otherwise propagate the error (const rvalue version)
    template <typename Func>
    constexpr auto andThen(Func&& func) const&& noexcept {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func>>;
        static_assert(IsStatusV<ReturnType>, "The result of the andThen callback must be a Status");
        return isOk() ? invoke(NEX_FORWARD<Func>(func)) : Status(unexpect, NEX_MOVE(storage_.error_));
    }

    // Execute a function if the status is an error, otherwise propagate the success status
    template <typename Func>
    constexpr auto orElse(Func&& func) & noexcept {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, const error_type&>>;
        static_assert(IsStatusV<ReturnType>, "The result of the orElse callback must be a Status");
        return isOk() ? Status::ok() : invoke(NEX_FORWARD<Func>(func), storage_.error_);
    }

    // Execute a function if the status is an error, otherwise propagate the success status (const version)
    template <typename Func>
    constexpr auto orElse(Func&& func) const& noexcept {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, const error_type&>>;
        static_assert(IsStatusV<ReturnType>, "The result of the orElse callback must be a Status");
        return isOk() ? Status::ok() : invoke(NEX_FORWARD<Func>(func), storage_.error_);
    }

    // Execute a function if the status is an error, otherwise propagate the success status (rvalue version)
    template <typename Func>
    constexpr auto orElse(Func&& func) && noexcept {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, const error_type&>>;
        static_assert(IsStatusV<ReturnType>, "The result of the orElse callback must be a Status");
        return isOk() ? Status::ok() : invoke(NEX_FORWARD<Func>(func), NEX_MOVE(storage_.error_));
    }

    // Execute a function if the status is an error, otherwise propagate the success status (const rvalue version)
    template <typename Func>
    constexpr auto orElse(Func&& func) const&& noexcept {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, const error_type&>>;
        static_assert(IsStatusV<ReturnType>, "The result of the orElse callback must be a Status");
        return isOk() ? Status::ok() : invoke(NEX_FORWARD<Func>(func), NEX_MOVE(storage_.error_));
    }

private:
    // Internal storage types for the Status class
    using StoredSuccessType = monostate;
    using StoredErrorType   = error_type;

    // Storage for either a successful status (no error) or an error status (with error details)
    // The storage alignment is set to alignment of StoredErrorType by default, since the StoredSuccessType 
    // is only a dummy placeholder and does not require any specific alignment.
    union NEX_ALIGNAS(alignof(StoredErrorType)) {
        StoredSuccessType  success_;       // Dummy placeholder represents a successful status
        StoredErrorType    error_;         // Error information for failure cases
    } storage_;

    // Flag indicating whether the status is successful (true) or an error (false)
    bool isOk_ = true;

    // Deleted default implicit constructor to prevent uninitialized Status objects
    NEX_HIDDEN_FROM_ABI constexpr Status() = delete;

    // Constructs a successful status
    NEX_HIDDEN_FROM_ABI constexpr explicit 
    Status(in_place_tag) noexcept : isOk_(true) {
        NEX_STD construct_at(NEX_ADDRESS_OF(storage_.success_), StoredSuccessType{});
    }

    // Constructs a Status object with an unexpected error
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit 
    Status(unexpect_type, Args&&... args) noexcept : isOk_(false) {
        NEX_STD construct_at(NEX_ADDRESS_OF(storage_.error_), NEX_FORWARD<Args>(args)...);
    }

    // Copy the contents of another Status object into this one
    NEX_HIDDEN_FROM_ABI constexpr 
    void copyStatus(const Status& other) noexcept {
        isOk_ = other.isOk_;
        if (isOk_) {
            // Construct the dummy member for successful status
            NEX_STD construct_at(NEX_ADDRESS_OF(storage_.success_), StoredSuccessType{});
        } else {
            // Copy the error information from the other Status object
            NEX_STD construct_at(NEX_ADDRESS_OF(storage_.error_), other.storage_.error_);
        }
    }

    // Move the contents of another Status object into this one
    NEX_HIDDEN_FROM_ABI constexpr 
    void moveStatus(Status&& other) noexcept {
        isOk_ = other.isOk_;
        if (isOk_) {
            // Construct the dummy member for successful status
            NEX_STD construct_at(NEX_ADDRESS_OF(storage_.success_), StoredSuccessType{});
        } else {
            // Move the error information from the other Status object
            NEX_STD construct_at(NEX_ADDRESS_OF(storage_.error_), NEX_MOVE(other.storage_.error_));
        }
    }

    // Destroy the existing error information if this Status holds an error
    NEX_HIDDEN_FROM_ABI constexpr 
    void destroyExistingError() noexcept {
        if (!isOk_) {
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
