/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <string.h>

#if !defined(NEX_NO_STL)
    #include <stdexcept>
#endif  // !defined(NEX_NO_STL)

#include "nex/base/macros.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

/**
 * @typedef ExceptionMessage
 * @brief   Type alias for exception message strings.
 * 
 * @details
 * Represents the type used for exception messages, which is a constant character pointer (nchar const*).
 * This type is used throughout the Nex-ecosystem to represent error messages associated with exceptions.
 */
using ExceptionMessage = nchar const*;

/**
 * @struct ExceptionData
 * @brief  Internal structure to hold exception data.
 * 
 * @details
 * This structure encapsulates the data associated with an exception, including the error message
 * and a flag indicating whether the message string should be freed in the destructor. It is used
 * internally by the Exception class to manage exception information.
 */
struct NEX_INTERNAL ExceptionData {
    ExceptionMessage what; // Pointer to the error message string
    bool doFree;           // Flag indicating whether the message string should be freed in the destructor
};

/**
 * @class  Exception
 * @brief  Base exception class for all exceptions across the Nex-ecosystem.
 * 
 * @details
 * The Exception class serves as the base class for all exceptions in the Nex-ecosystem, providing a consistent
 * interface for error handling. It encapsulates an error message and provides mechanisms for constructing
 * exceptions with descriptive messages, copying exceptions, and retrieving error information.
 * Derived classes represent specific types of exceptions, allowing for more granular error handling and reporting.
 * 
 * @note
 * The design orientation of Nex-ecosystem is non exception-based, and exceptions are used only for critical 
 * errors that cannot be handled gracefully. Users of the Nex-ecosystem are encouraged to use error codes 
 * and other non-exception-based error handling mechanisms where appropriate, and to reserve exceptions for 
 * truly exceptional circumstances.
 */
class NEX_API Exception {
public:
    // Constructs an Exception with a descriptive message (string literal or user-provided string)
    explicit Exception(ExceptionMessage const message) noexcept 
        : data_{message, false} {}

    // Constructs an Exception with a descriptive message (dynamically allocated string)
    Exception(ExceptionMessage const message, int32) noexcept {
        data_.what = strdup(message);
        data_.doFree = true;
    }

    // Copies the Exception, duplicating the message if necessary
    Exception(Exception const& other) noexcept {
        if (other.data_.doFree && other.data_.what) {
            data_.what = strdup(other.data_.what);
            data_.doFree = true;
        } else {
            data_.what = other.data_.what;
            data_.doFree = false;
        }
    }

    // Assigns the Exception, duplicating the message if necessary
    Exception& operator=(Exception const& other) noexcept {
        if (this != &other) {
            if (data_.doFree && data_.what) {
                free(const_cast<nchar*>(data_.what));
            }
            if (other.data_.doFree && other.data_.what) {
                data_.what = strdup(other.data_.what);
                data_.doFree = true;
            } else {
                data_.what = other.data_.what;
                data_.doFree = false;
            }
        }
        return *this;
     }

#if !defined(NEX_NO_STL)
    // Constructs an Exception from a standard C++ exception
    Exception(const NEX_STD exception& e) noexcept
        : data_{e.what(), false} {}

    // Assigns an Exception from a standard C++ exception
    Exception& operator=(const NEX_STD exception& e) noexcept {
        data_.what = e.what();
        data_.doFree = false; // Standard exceptions manage their own memory, so we don't free the message
        return *this;
    }
#endif  // !defined(NEX_NO_STL)

    // Default destructor, freeing the message if it was dynamically allocated
    virtual ~Exception() noexcept {
        if (data_.doFree && data_.what) {
            free(const_cast<nchar*>(data_.what));
        }
    }

    // Returns the error message associated with the exception
    virtual cstring what() const noexcept { 
        return data_.what ? data_.what : "Unknown exception"; 
    }

    // Returns the error message associated with the exception
    virtual cstring message() const noexcept { return what(); }

private:
    ExceptionData data_;
};

/**
 * @class  BadException
 * @brief  Exception for bad or unexpected errors.
 * 
 * @details
 * This exception is thrown when a bad or unexpected error occurs. It serves as a general-purpose
 * exception for situations that do not fit into other specific exception categories.
 */
class NEX_API BadException : public Exception {
public:
    // Constructs a BadException with a default message
    BadException() noexcept 
        : Exception("Bad exception") {}
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
class NEX_API InvalidArgument : public Exception {
public:
    // Constructs an InvalidArgument exception with a descriptive message (string literal or user-provided string)
    InvalidArgument(ExceptionMessage const message) noexcept
        : Exception(message) {}

    // Constructs an InvalidArgument exception with a descriptive message (dynamically allocated string)
    InvalidArgument(ExceptionMessage const message, int32) noexcept
        : Exception(message, 0) {}
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
class NEX_API RuntimeError : public Exception {
public:
    // Constructs a RuntimeError with a descriptive message (string literal or user-provided string)
    RuntimeError(ExceptionMessage const message) noexcept 
        : Exception(message) {}

    // Constructs a RuntimeError with a descriptive message (dynamically allocated string)
    RuntimeError(ExceptionMessage const message, int32) noexcept
        : Exception(message, 0) {}
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
class NEX_API LogicError : public Exception {
public:
    // Constructs a LogicError with a descriptive message (string literal or user-provided string)
    LogicError(ExceptionMessage const message) noexcept 
        : Exception(message) {}

    // Constructs a LogicError with a descriptive message (dynamically allocated string)
    LogicError(ExceptionMessage const message, int32) noexcept
        : Exception(message, 0) {}
};

/**
 * @class  DomainError
 * @brief  Exception for domain errors in mathematical functions.
 * 
 * @details
 * This exception is thrown when a mathematical function receives an argument outside its domain.
 * Examples of situations that may trigger this exception include taking the square root of a negative number
 * or calculating the logarithm of a non-positive number.
 */
class NEX_API DomainError : public LogicError {
public:
    // Constructs a DomainError with a descriptive message (string literal or user-provided string)
    DomainError(ExceptionMessage const message) noexcept
        : LogicError(message) {}

    // Constructs a DomainError with a descriptive message (dynamically allocated string)
    DomainError(ExceptionMessage const message, int32) noexcept
        : LogicError(message, 0) {}
};

/**
 * @class  LengthError
 * @brief  Exception for length errors in data structures or operations.
 * 
 * @details
 * This exception is thrown when an operation exceeds the maximum allowable length for a data structure
 * or when an operation is attempted on a data structure that is too long. Examples of situations that
 * may trigger this exception include attempting to resize a container beyond its maximum size or trying
 * to insert an element into a container that has reached its maximum capacity.
 */
class NEX_API LengthError : public LogicError {
public:
    // Constructs a LengthError with a descriptive message (string literal or user-provided string)
    LengthError(ExceptionMessage const message) noexcept
        : LogicError(message) {}

    // Constructs a LengthError with a descriptive message (dynamically allocated string)
    LengthError(ExceptionMessage const message, int32) noexcept
        : LogicError(message, 0) {}
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
class NEX_API OutOfRange : public Exception {
public:
    // Constructs an OutOfRange exception with a descriptive message (string literal or user-provided string)
    OutOfRange(ExceptionMessage const message) noexcept 
        : Exception(message) {}

    // Constructs an OutOfRange exception with a descriptive message (dynamically allocated string)
    OutOfRange(ExceptionMessage const message, int32) noexcept
        : Exception(message, 0) {}
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
class NEX_API DivisionByZero : public InvalidArgument {
public:
    // Constructs a DivisionByZero exception with a descriptive message
    DivisionByZero(ExceptionMessage const message = "Division by zero") noexcept
        : InvalidArgument(message) {}

    // Constructs a DivisionByZero exception with a descriptive message (dynamically allocated string)
    DivisionByZero(ExceptionMessage const message, int32) noexcept
        : InvalidArgument(message, 0) {}
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
class NEX_API NullPointer : public InvalidArgument {
public:
    // Constructs a NullPointer exception with a descriptive message
    NullPointer(ExceptionMessage const message = "Null pointer") noexcept
        : InvalidArgument(message) {}
    
    // Constructs a NullPointer exception with a descriptive message (dynamically allocated string)
    NullPointer(ExceptionMessage const message, int32) noexcept
        : InvalidArgument(message, 0) {}
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
class NEX_API Overflow : public RuntimeError {
public:
    // Constructs an Overflow exception with a descriptive message
    Overflow(ExceptionMessage const message = "Overflow error") noexcept
        : RuntimeError(message) {}
    
    // Constructs an Overflow exception with a descriptive message (dynamically allocated string)
    Overflow(ExceptionMessage const message, int32) noexcept
        : RuntimeError(message, 0) {}
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
class NEX_API Underflow : public RuntimeError {
public:
    // Constructs an Underflow exception with a descriptive message
    Underflow(ExceptionMessage const message = "Underflow error") noexcept
        : RuntimeError(message) {}
    
    // Constructs an Underflow exception with a descriptive message (dynamically allocated string)
    Underflow(ExceptionMessage const message, int32) noexcept
        : RuntimeError(message, 0) {}
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
class NEX_API NotImplemented : public LogicError {
public:
    // Constructs a NotImplemented exception with a descriptive message
    NotImplemented(ExceptionMessage const message = "Feature not implemented") noexcept
        : LogicError(message) {}

    // Constructs a NotImplemented exception with a descriptive message (dynamically allocated string)
    NotImplemented(ExceptionMessage const message, int32) noexcept
        : LogicError(message, 0) {}
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
class NEX_API NotSupported : public LogicError {
public:
    // Constructs a NotSupported exception with a descriptive message
    NotSupported(ExceptionMessage const message = "Operation not supported") noexcept
        : LogicError(message) {}
    
    // Constructs a NotSupported exception with a descriptive message (dynamically allocated string)
    NotSupported(ExceptionMessage const message, int32) noexcept
        : LogicError(message, 0) {}
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
class NEX_API InvalidState : public LogicError {
public:
    // Constructs an InvalidState exception with a descriptive message
    InvalidState(ExceptionMessage const message = "Invalid state") noexcept
        : LogicError(message) {}
    
    // Constructs an InvalidState exception with a descriptive message (dynamically allocated string)
    InvalidState(ExceptionMessage const message, int32) noexcept
        : LogicError(message, 0) {}
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
class NEX_API FormatError : public InvalidArgument {
public:
    // Constructs a FormatError exception with a descriptive message
    FormatError(ExceptionMessage const message = "Format error") noexcept
        : InvalidArgument(message) {}
    
    // Constructs a FormatError exception with a descriptive message (dynamically allocated string)
    FormatError(ExceptionMessage const message, int32) noexcept
        : InvalidArgument(message, 0) {}
};

/**
 * @class  BadAlloc
 * @brief  Exception for bad memory allocation.
 *
 * @details
 * This exception is thrown when a memory allocation fails. It indicates that the program was unable
 * to allocate the requested memory, and the program cannot continue to operate correctly due to this error.
 */
class NEX_API BadAlloc : public RuntimeError {
public:
    // Constructs a BadAlloc exception
    BadAlloc() noexcept
        : RuntimeError("Bad allocation") {}

private:
    friend class BadArrayNewLength;

    // Constructs a BadAlloc exception with a descriptive message
    BadAlloc(ExceptionMessage const message) noexcept
        : RuntimeError(message) {}
};

/**
 * @class  BadArrayNewLength
 * @brief  Exception for bad array new length.
 *
 * @details
 * This exception is thrown when an attempt is made to allocate an array with a length that is invalid
 * or exceeds the maximum allowable size. It indicates that the program has attempted to create an array
 * with an invalid length, and the program cannot continue to operate correctly due to this error.
 */
class NEX_API BadArrayNewLength : public BadAlloc {
public:
    // Constructs a BadArrayNewLength exception
    BadArrayNewLength() noexcept
        : BadAlloc("Bad array new length") {}
};

NEX_NAMESPACE_END
