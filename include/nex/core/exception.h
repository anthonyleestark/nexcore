/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <stdexcept>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/core/text/string.h"
#include "nex/core/text/string_view.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @class  Exception
 * @brief  Base exception class for all exceptions across the Nex-ecosystem.
 * 
 * @details
 * This class serves as the base for all exceptions in the Nex-ecosystem, providing a common interface 
 * and structure for error handling. It inherits from `std::exception` to integrate with standard C++ 
 * exception handling mechanisms.
 * 
 * @note
 * The design orientation of Nex-ecosystem is non exception-based, and exceptions are used only for critical 
 * errors that cannot be handled gracefully. Users of the Nex-ecosystem are encouraged to use error codes 
 * and other non-exception-based error handling mechanisms where appropriate, and to reserve exceptions for 
 * truly exceptional circumstances.
 */
class NEX_EXPORT Exception : public NEX_STD exception {
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

/**
 * @class  InvalidArgument
 * @brief  Exception for invalid arguments passed to functions or methods.
 * 
 * @details
 * This exception is thrown when a function or method receives an argument that is invalid or 
 * inappropriate for the operation being performed. It indicates that the caller has made a mistake 
 * in providing the input, and the function cannot proceed with the operation due to the invalid 
 * argument. Examples of situations that may trigger this exception include passing a null pointer 
 * where a valid object is expected, providing an out-of-range index, or supplying an argument that 
 * violates the expected format or constraints.
 */
class NEX_EXPORT InvalidArgument : public Exception {
public:
    // Constructor with a message
    InvalidArgument(const StringView& message) : Exception(message) {}

    // Destructor
    virtual ~InvalidArgument() noexcept = default;
};

/**
 * @class  RuntimeError
 * @brief  Exception for errors that occur during the execution of a program.
 * 
 * @details
 * This exception is thrown when an error occurs during the execution of a program that cannot be 
 * anticipated or prevented by the programmer. It indicates that something has gone wrong during 
 * the runtime of the program, and the program cannot continue to operate correctly. Examples of 
 * situations that may trigger this exception include division by zero, out-of-memory errors, 
 * or other unexpected conditions that arise during the execution of the program.
 */
class NEX_EXPORT RuntimeError : public Exception {
public:
    // Constructor with a message
    RuntimeError(const StringView& message) : Exception(message) {}

    // Destructor
    virtual ~RuntimeError() noexcept = default;
};

/**
 * @class  LogicError
 * @brief  Exception for logic errors in the program.
 * 
 * @details
 * This exception is thrown when a logic error occurs in the program, indicating that the program's logic 
 * is flawed. Examples of situations that may trigger this exception include violations of preconditions, 
 * postconditions, or invariants, as well as other errors that result from incorrect program logic.
 */
class NEX_EXPORT LogicError : public Exception {
public:
    // Constructor with a message
    LogicError(const StringView& message) : Exception(message) {}

    // Destructor
    virtual ~LogicError() noexcept = default;
};

/**
 * @class  OutOfRange
 * @brief  Exception for out-of-range errors.
 * 
 * @details
 * This exception is thrown when an operation attempts to access an element outside the valid range.
 * Examples of situations that may trigger this exception include accessing an array or container
 * with an invalid index, or performing arithmetic operations that result in values outside the
 * allowable range.
 */
class NEX_EXPORT OutOfRange : public Exception {
public:
    // Constructor with a message
    OutOfRange(const StringView& message) : Exception(message) {}

    // Destructor
    virtual ~OutOfRange() noexcept = default;
};

/**
 * @class  DivisionByZero
 * @brief  Exception for division by zero errors.
 * 
 * @details
 * This exception is thrown when an attempt is made to divide a number by zero, which is undefined 
 * in mathematics and results in an error in programming. It indicates that the program has attempted 
 * to perform an invalid arithmetic operation, and the program cannot continue to operate correctly 
 * due to this error.
 */
class NEX_EXPORT DivisionByZero : public InvalidArgument {
public:
    // Constructor with a message
    DivisionByZero(const StringView& context = u"Division by zero")
        : InvalidArgument(context) {}

    // Destructor
    virtual ~DivisionByZero() noexcept = default;
};

/**
 * @class  NullPointer
 * @brief  Exception for null pointer errors.
 * 
 * @details
 * This exception is thrown when an attempt is made to dereference a null pointer, which is invalid 
 * in programming. It indicates that the program has attempted to access memory through a pointer 
 * that is not initialized or has been set to null, and the program cannot continue to operate 
 * correctly due to this error.
 */
class NEX_EXPORT NullPointer : public InvalidArgument {
public:
    // Constructor with a message
    NullPointer(const StringView& context = u"Null pointer")
        : InvalidArgument(context) {}

    // Destructor
    virtual ~NullPointer() noexcept = default;
};

/**
 * @class  Overflow
 * @brief  Exception for overflow errors.
 * 
 * @details
 * This exception is thrown when an arithmetic operation results in a value that exceeds the maximum
 * representable value for a given data type. It indicates that the program has attempted to perform
 * an operation that produces a result outside the allowable range, and the program cannot continue
 * to operate correctly due to this error.
 */
class NEX_EXPORT Overflow : public RuntimeError {
public:
    // Constructor with a message
    Overflow(const StringView& message = u"Overflow error")
        : RuntimeError(message) {}

    // Destructor
    virtual ~Overflow() noexcept = default;
};

/**
 * @class  Underflow
 * @brief  Exception for underflow errors.
 * 
 * @details
 * This exception is thrown when an arithmetic operation results in a value that is below the minimum
 * representable value for a given data type. It indicates that the program has attempted to perform
 * an operation that produces a result outside the allowable range, and the program cannot continue
 * to operate correctly due to this error.
 */
class NEX_EXPORT Underflow : public RuntimeError {
public:
    // Constructor with a message
    Underflow(const StringView& message = u"Underflow error")
        : RuntimeError(message) {}

    // Destructor
    virtual ~Underflow() noexcept = default;
};

/**
 * @class  NotImplemented
 * @brief  Exception for not implemented errors.
 * 
 * @details
 * This exception is thrown when a feature or functionality is not implemented. It indicates that
 * the program has attempted to use a feature that is not yet available, and the program cannot
 * continue to operate correctly due to this error.
 */
class NEX_EXPORT NotImplemented : public LogicError {
public:
    // Constructor with a message
    NotImplemented(const StringView& feature = u"Feature not implemented")
        : LogicError(feature) {}

    // Destructor
    virtual ~NotImplemented() noexcept = default;
};

/**
 * @class  NotSupported
 * @brief  Exception for not supported errors.
 * 
 * @details
 * This exception is thrown when an operation or feature is not supported. It indicates that the
 * program has attempted to use an operation or feature that is not supported in the current context,
 * and the program cannot continue to operate correctly due to this error.
 */
class NEX_EXPORT NotSupported : public LogicError {
public:
    // Constructor with a message
    NotSupported(const StringView& operation = u"Operation not supported")
        : LogicError(operation) {}

    // Destructor
    virtual ~NotSupported() noexcept = default;
};

/**
 * @class  InvalidState
 * @brief  Exception for invalid state errors.
 * 
 * @details
 * This exception is thrown when an operation is attempted in an invalid state. It indicates that
 * the program has encountered a state that is not allowed or expected, and the program cannot
 * continue to operate correctly due to this error.
 */
class NEX_EXPORT InvalidState : public LogicError {
public:
    // Constructor with a message
    InvalidState(const StringView& message = u"Invalid state")
        : LogicError(message) {}

    // Destructor
    virtual ~InvalidState() noexcept = default;
};

/**
 * @class  FormatError
 * @brief  Exception for format errors.
 * 
 * @details
 * This exception is thrown when a format error occurs, such as when parsing or formatting data. It
 * indicates that the program has encountered data that does not conform to the expected format, and
 * the program cannot continue to operate correctly due to this error.
 */
class NEX_EXPORT FormatError : public InvalidArgument {
public:
    // Constructor with a message
    FormatError(const StringView& message = u"Format error")
        : InvalidArgument(message) {}

    // Destructor
    virtual ~FormatError() noexcept = default;
};

NEX_CORE_NAMESPACE_END