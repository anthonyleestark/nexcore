/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <stdexcept>

#include "common/macros.h"
#include "common/types.h"
#include "common/string.h"
#include "common/string_view.h"

NEXSUITE_NAMESPACE_BEGIN

// Base exception class for all NexSuite exceptions
class NEX_EXPORT Exception : public NEXSUITE_STD exception {
private:
    // The message of the exception
    String message_;

public:
    // Constructor with a message
    Exception(const StringView& message) : message_(message) {}

    // Destructor
    virtual ~Exception() noexcept = default;

    // Get the message of the exception
    virtual const StringView& message() const noexcept { return message_.view(); }
};

// Exception for invalid arguments
class NEX_EXPORT InvalidArgument : public Exception {
public:
    // Constructor with a message
    InvalidArgument(const StringView& message) : Exception(message) {}

    // Destructor
    virtual ~InvalidArgument() noexcept = default;
};

// Exception for runtime errors
class NEX_EXPORT RuntimeError : public Exception {
public:
    // Constructor with a message
    RuntimeError(const StringView& message) : Exception(message) {}

    // Destructor
    virtual ~RuntimeError() noexcept = default;
};

// Exception for logic errors
class NEX_EXPORT LogicError : public Exception {
public:
    // Constructor with a message
    LogicError(const StringView& message) : Exception(message) {}

    // Destructor
    virtual ~LogicError() noexcept = default;
};

// Exception for out of range errors
class NEX_EXPORT OutOfRange : public Exception {
public:
    // Constructor with a message
    OutOfRange(const StringView& message) : Exception(message) {}

    // Destructor
    virtual ~OutOfRange() noexcept = default;
};

// Exception for division by zero
class NEX_EXPORT DivisionByZero : public InvalidArgument {
public:
    // Constructor with a message
    DivisionByZero(const StringView& context = u"Division by zero")
        : InvalidArgument(context) {}

    // Destructor
    virtual ~DivisionByZero() noexcept = default;
};

// Exception for null pointer errors
class NEX_EXPORT NullPointer : public InvalidArgument {
public:
    // Constructor with a message
    NullPointer(const StringView& context = u"Null pointer")
        : InvalidArgument(context) {}

    // Destructor
    virtual ~NullPointer() noexcept = default;
};

// Exception for overflow errors
class NEX_EXPORT Overflow : public RuntimeError {
public:
    // Constructor with a message
    Overflow(const StringView& message = u"Overflow error")
        : RuntimeError(message) {}

    // Destructor
    virtual ~Overflow() noexcept = default;
};

// Exception for underflow errors
class NEX_EXPORT Underflow : public RuntimeError {
public:
    // Constructor with a message
    Underflow(const StringView& message = u"Underflow error")
        : RuntimeError(message) {}

    // Destructor
    virtual ~Underflow() noexcept = default;
};

// Exception for not implemented errors
class NEX_EXPORT NotImplemented : public LogicError {
public:
    // Constructor with a message
    NotImplemented(const StringView& feature = u"Feature not implemented")
        : LogicError(feature) {}

    // Destructor
    virtual ~NotImplemented() noexcept = default;
};

// Exception for operation not supported
class NEX_EXPORT NotSupported : public LogicError {
public:
    // Constructor with a message
    NotSupported(const StringView& operation = u"Operation not supported")
        : LogicError(operation) {}

    // Destructor
    virtual ~NotSupported() noexcept = default;
};

// Exception for invalid state
class NEX_EXPORT InvalidState : public LogicError {
public:
    // Constructor with a message
    InvalidState(const StringView& message = u"Invalid state")
        : LogicError(message) {}

    // Destructor
    virtual ~InvalidState() noexcept = default;
};

// Exception for format errors (used in string formatting)
class NEX_EXPORT FormatError : public InvalidArgument {
public:
    // Constructor with a message
    FormatError(const StringView& message = u"Format error")
        : InvalidArgument(message) {}

    // Destructor
    virtual ~FormatError() noexcept = default;
};

NEXSUITE_NAMESPACE_END