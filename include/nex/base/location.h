/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    location.h
 * @brief   Define macros and utilities for capturing source code location information (file, line, function, etc.) 
 *          for debugging, logging, analysis, and error reporting purposes.
 * 
 * @details
 * This header defines macros and utilities for capturing source code location information, such as file name, line 
 * number, and function name. This information can be used for debugging, logging, analysis, and error reporting purposes
 * to provide more context about where in the source code an event occurred. The macros defined in this header are designed
 * to be easy to use and provide consistent source location information across different compilers and platforms,
 * making it easier to maintain and debug code in a cross-platform environment.
 */

#include "nex/base/compiler.h"
#include "nex/base/attributes.h"
#include "nex/base/namespace.h"

// ================================================================================================
// Compiler built-in attribute detection
// ================================================================================================

// Check for support of compiler built-ins for capturing source location information 
// (e.g., __builtin_FILE(), __builtin_LINE(), __builtin_FUNCTION()) and define a macro accordingly.
#if !defined(NEX_HAS_BUILTIN_SOURCE_LOCATION)
    #if NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_MSVC_COMPATIBLE
        #define NEX_HAS_BUILTIN_SOURCE_LOCATION 1
    #else  // Compiler does not support __builtin_FILE/LINE/FUNCTION()
        #define NEX_HAS_BUILTIN_SOURCE_LOCATION 0
    #endif  // NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE || NEX_COMPILER_MSVC_COMPATIBLE
#endif  // !defined(NEX_HAS_BUILTIN_SOURCE_LOCATION)

// User-configurable macro to enable/disable the use of compiler built-ins for capturing source location information.
#if !defined(NEX_USE_BUILTIN_SOURCE_LOCATION)
    #if NEX_HAS_BUILTIN_SOURCE_LOCATION
        #define NEX_USE_BUILTIN_SOURCE_LOCATION 1
    #else  // Compiler does not support built-in source location
        #define NEX_USE_BUILTIN_SOURCE_LOCATION 0
    #endif  // NEX_HAS_BUILTIN_SOURCE_LOCATION
#else
    /**
     * @note
     * If the user has already defined NEX_USE_BUILTIN_SOURCE_LOCATION, we use their definition, 
     * but we also check if it's set to 1 while the compiler does not support it, in which case 
     * we disable it to avoid compilation errors. 
     * This ensures that the code remains portable and compatible with different compilers, while still 
     * allowing the user to enable or disable the use of compiler built-ins for capturing source location 
     * information based on their preferences and the capabilities of the compiler being used. 
     * By providing this flexibility, we can ensure that the code can be compiled and run correctly across 
     * different platforms and compilers, while still providing the benefits of capturing source location 
     * information when supported by the compiler.
     */
    #if NEX_USE_BUILTIN_SOURCE_LOCATION && !NEX_HAS_BUILTIN_SOURCE_LOCATION
        #undef NEX_USE_BUILTIN_SOURCE_LOCATION
        #define NEX_USE_BUILTIN_SOURCE_LOCATION 0
    #endif  // NEX_USE_BUILTIN_SOURCE_LOCATION && !NEX_HAS_BUILTIN_SOURCE_LOCATION
#endif  // !defined(NEX_USE_BUILTIN_SOURCE_LOCATION)

// User-configurable macro to enable/disable the use of compiler built-ins for capturing detailed function signatures 
// (e.g., __PRETTY_FUNCTION__, __FUNCSIG__), which can provide more detailed information about the function, such as 
// parameter types and template arguments, but may be less readable than just the function name, so we allow the user 
// to choose whether to use the detailed function signature or just the function name for source location information.
#if !defined(NEX_USE_DETAILED_FUNCTION_SIGNATURE)
    #define NEX_USE_DETAILED_FUNCTION_SIGNATURE 1
#else
    /**
     * @note
     * If the user has already defined NEX_USE_DETAILED_FUNCTION_SIGNATURE, we use their definition, 
     * but we redefine it into a boolean value (1 or 0) to ensure consistent usage in the codebase. 
     * This allows the user to choose whether to use the detailed function signature or just the 
     * function name for source location information, while still ensuring that the code remains 
     * portable and compatible with different compilers. 
     */
    #undef NEX_USE_DETAILED_FUNCTION_SIGNATURE
    #define NEX_USE_DETAILED_FUNCTION_SIGNATURE 1
#endif  // !defined(NEX_USE_DETAILED_FUNCTION_SIGNATURE)

// ================================================================================================
// Source location utility class and macros
// ================================================================================================

NEX_NAMESPACE_BEGIN

/**
 * @class SourceLocation
 * @brief A utility class for capturing source code location information (file, line, function, etc.) 
 *        for debugging, logging, analysis, and error reporting purposes.
 * 
 * @details
 * The SourceLocation class provides a convenient way to capture and store source code location information,
 * such as file name, line number, and function name. This information can be used for debugging, logging, 
 * analysis, and error reporting purposes to provide more context about where in the source code an event occurred.
 * 
 * @note
 * The SourceLocation class is designed to be lightweight and efficient, with minimal overhead, making it suitable 
 * for use in performance-critical code.
 */
class NEX_HIDDEN_FROM_ABI SourceLocation {
public:
    using FilePathType = const char*;
    using FunctionNameType = const char*;
    using LineNumberType = unsigned int;
    using ColumnNumberType = unsigned int;

    // Strip the path from a file path, returning only the file name, which can be used for various purposes 
    // such as improving the readability of error messages and logs by showing only the file name instead of 
    // the full file path.
    NEX_HIDDEN_FROM_ABI static constexpr 
    FilePathType stripFilePath(FilePathType filePath) {
        if (!filePath || *filePath == '\0') return "";

        FilePathType file = filePath;
        while (*filePath) {
            if (*filePath == '/' || *filePath == '\\') {
                file = filePath + 1;
            }
            ++filePath;
        }

        return file;
    }

    // Strip the absolute prefix from a file path, returning the relative path from the project root.
    // If the project root prefix is not found, it falls back to returning the original path.
    NEX_HIDDEN_FROM_ABI static constexpr 
    FilePathType stripToRelativePath(FilePathType filePath, FilePathType projectRoot) {
        if (!filePath || *filePath == '\0') return "";
        if (!projectRoot || *projectRoot == '\0') return filePath;

        FilePathType f = filePath;
        FilePathType r = projectRoot;

        // Scan through the projectRoot string to check if filePath starts with projectRoot
        while (*r) {
            // Normalize the path separators '/' and '\' to be compatible with both Windows and Linux/macOS
            char f_char = (*f == '\\') ? '/' : *f;
            char r_char = (*r == '\\') ? '/' : *r;

            if (f_char != r_char) {
                return filePath; // Root prefix does not match, return the original path
            }
            ++f;
            ++r;
        }

        // Skip any redundant separator character at the beginning of the relative path if present (e.g., '/' or '\')
        if (*f == '/' || *f == '\\') {
            ++f;
        }

        return f;
    }

    // Get the function name from a function signature, which can be used for various purposes such as improving the 
    // readability of error messages and logs by showing only the function name instead of the full function signature.
    NEX_HIDDEN_FROM_ABI static constexpr 
    FunctionNameType stripFunctionSignature(FunctionNameType funcSignature) {
        if (!funcSignature || *funcSignature == '\0') return "";

        // Step 1: Find the position of the first '(' character in the function signature
        FunctionNameType end = funcSignature;
        while (*end && *end != '(') {
            ++end;
        }

        // If there is no '(', return the entire original string
        if (*end == '\0') return funcSignature;

        // Step 2: Move backward from the '(' to skip any whitespace (if any) and find the end of the function name
        FunctionNameType p = end - 1;
        while (p > funcSignature && *p == ' ') {
            --p;
        }

        // Step 3: Continue moving backward until the first whitespace separating the return type / calling convention
        // (But do not stop if encountering ':' or '>' as it could be a namespace or template)
        while (p > funcSignature) {
            if (*p == ' ' && *(p + 1) != '*' && *(p + 1) != '&') {
                // For MSVC: Skip calling conventions like __cdecl, __thiscall, __stdcall
                if (*(p + 1) == '_' && *(p + 2) == '_') {
                    --p;
                    continue;
                }
                // Found the whitespace separating "return type/calling convention" and "function name"
                return p + 1;
            }
            --p;
        }

        return funcSignature;
    }

// ================================================================================================
// Source location metadata macros 
// ================================================================================================

/**
 * @brief Get the file path for source location
 * @note
 * Whether the returned string is a full path or a relative path depends entirely on how the 
 * file path was supplied to the compiler during the build process (e.g., via CMake or CLI). 
 * To enforce short file names across all build environments, consider using compiler flags 
 * like `-fmacro-prefix-map` or `-ffile-prefix-map`.
 */
#if NEX_USE_BUILTIN_SOURCE_LOCATION
    #define NEX_SOURCE_FILE_PATH __builtin_FILE()
#else  // Compiler does not support __builtin_FILE()
    #define NEX_SOURCE_FILE_PATH __FILE__
#endif  // NEX_USE_BUILTIN_SOURCE_LOCATION

/** @brief Get the line number for source location */
#if NEX_USE_BUILTIN_SOURCE_LOCATION
    #define NEX_SOURCE_LINE_NUMBER  \
        static_cast<NEX_PREPEND_NAMESPACE(SourceLocation::LineNumberType)>(__builtin_LINE())
#else  // Compiler does not support __builtin_LINE()
    #define NEX_SOURCE_LINE_NUMBER  \
        static_cast<NEX_PREPEND_NAMESPACE(SourceLocation::LineNumberType)>(__LINE__)
#endif  // NEX_USE_BUILTIN_SOURCE_LOCATION

/**
 * @brief Get the column number for source location
 * @note
 * The column number might be difference between compilers, especially if you are using Tab ('\t') characters 
 * in your source code, as different compilers may interpret the width of a Tab character differently.
 */
#if NEX_USE_BUILTIN_SOURCE_LOCATION
    #define NEX_SOURCE_COLUMN_NUMBER  \
        static_cast<NEX_PREPEND_NAMESPACE(SourceLocation::ColumnNumberType)>(__builtin_COLUMN())
#else  // Compiler does not support __builtin_COLUMN()
    #define NEX_SOURCE_COLUMN_NUMBER  \
        static_cast<NEX_PREPEND_NAMESPACE(SourceLocation::ColumnNumberType)>(0)
#endif  // NEX_USE_BUILTIN_SOURCE_LOCATION

/** @brief Get the function name for source location */
#if NEX_USE_BUILTIN_SOURCE_LOCATION
    #if NEX_USE_DETAILED_FUNCTION_SIGNATURE
        #if NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE
            #define NEX_SOURCE_FUNCTION_NAME __builtin_FUNCTION()
        #elif NEX_COMPILER_IS_MSVC
            #define NEX_SOURCE_FUNCTION_NAME __builtin_FUNCSIG()
        #elif NEX_COMPILER_MSVC_COMPATIBLE
            /**
             * @note
             * Clang on MSVC supports __builtin_FUNCTION() and provides more detailed function signatures 
             * than __builtin_FUNCSIG(), so we use __builtin_FUNCTION() for better readability and consistency 
             * with other compilers and across platforms.
             */
            #define NEX_SOURCE_FUNCTION_NAME __builtin_FUNCTION()
        #endif
    #else  // User prefers short function names, so we strip the function signature to get only the function name
        #if NEX_COMPILER_IS_MSVC
            /**
             * @note
             * The corresponding built-in for short-signature on MSVC is __builtin_FUNCTION(), 
             * but for safety and consistency with other compilers, we use __builtin_FUNCSIG() instead 
             * and then strip the function signature to get only the function name.
             */
            #define NEX_SOURCE_FUNCTION_NAME  \
                NEX_PREPEND_NAMESPACE(SourceLocation::stripFunctionSignature)(__builtin_FUNCSIG())
        #else
            #define NEX_SOURCE_FUNCTION_NAME  \
                NEX_PREPEND_NAMESPACE(SourceLocation::stripFunctionSignature)(__builtin_FUNCTION())
        #endif
    #endif  // !NEX_USE_DETAILED_FUNCTION_SIGNATURE
#else  // Compiler does not support __builtin_FUNCTION()
    #if NEX_USE_DETAILED_FUNCTION_SIGNATURE
        #if NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE
            #define NEX_SOURCE_FUNCTION_NAME __PRETTY_FUNCTION__
        #elif NEX_COMPILER_IS_MSVC
            #define NEX_SOURCE_FUNCTION_NAME __FUNCSIG__
        #elif NEX_COMPILER_MSVC_COMPATIBLE
            /**
             * @note
             * Clang on MSVC supports __PRETTY_FUNCTION__ and provides more detailed function signatures 
             * than __FUNCSIG__, so we use __PRETTY_FUNCTION__ for better readability and consistency with 
             * other compilers and across platforms.
             */
            #define NEX_SOURCE_FUNCTION_NAME __PRETTY_FUNCTION__
        #else
            // See: https://en.cppreference.com/w/c/language/function_definition#func
            #define NEX_SOURCE_FUNCTION_NAME __func__
        #endif
    #else  // User prefers short function names
        // See also: https://en.cppreference.com/w/c/language/function_definition#func
        #define NEX_SOURCE_FUNCTION_NAME __func__
    #endif
#endif  // NEX_USE_BUILTIN_SOURCE_LOCATION

// ================================================================================================
// Source location class implementation
// ================================================================================================

#if NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE

private:
    /**
     * @note
     * The names SourceLocation::__impl, _M_file_name, _M_function_name, _M_line, and _M_column
     * are hard-coded in the compiler and must not be changed here.
     */
    struct __impl {
        FilePathType        _M_file_name;
        FunctionNameType    _M_function_name;
        LineNumberType      _M_line;
        ColumnNumberType    _M_column;
    };

    // Pointer to the implementation struct that holds the source location information
    const __impl* __ptr_ = nullptr;

    // Helper function to cast a pointer to the implementation struct, which is used 
    // for accessing the source location information
    static consteval const __impl* cast_bsl_ptr(auto ptr) noexcept {
        union {
            decltype(ptr) src;
            const __impl* dst;
        } u{.src = ptr};
        return u.dst;
    }

public:
    // The defaulted `ptr` argument ensures that the builtin is evaluated within the context of the caller. 
    // The exact type of `ptr` is deferred and deduced at the caller side. An explicit value should never be provided.
    static consteval SourceLocation current(auto ptr = __builtin_source_location()) noexcept {
        SourceLocation loc;
        loc.__ptr_ = cast_bsl_ptr(ptr);
        return loc;
    }

    // Default constructor for SourceLocation, 
    // which initializes the source location to an empty state (no file, no function, line 0, column 0).
    NEX_HIDDEN_FROM_ABI constexpr SourceLocation() noexcept = default;

    // Get the line number for the source location
    NEX_HIDDEN_FROM_ABI constexpr LineNumberType line() const noexcept {
        return __ptr_ != nullptr ? __ptr_->_M_line : 0;
    }

    // Get the column number for the source location
    NEX_HIDDEN_FROM_ABI constexpr ColumnNumberType column() const noexcept {
        return __ptr_ != nullptr ? __ptr_->_M_column : 0;
    }

    // Get the file name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FilePathType fileName() const noexcept {
        return __ptr_ != nullptr ? __ptr_->_M_file_name : "";
    }

#if NEX_USE_DETAILED_FUNCTION_SIGNATURE
    // Get the full function signature for the source location
    NEX_HIDDEN_FROM_ABI constexpr FunctionNameType functionName() const noexcept {
        return __ptr_ != nullptr ? __ptr_->_M_function_name : "";
    }
#else  // User prefers short function names, so we strip the function signature to get only the function name
    // Get the function name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FunctionNameType functionName() const noexcept {
        return __ptr_ != nullptr
            ? stripFunctionSignature(__ptr_->_M_function_name)
            : "";
    }
#endif  // !NEX_USE_DETAILED_FUNCTION_SIGNATURE

#else  // ^^NEX_COMPILER_IS_CLANG || NEX_COMPILER_GCC_COMPATIBLE

private:
    /**
     * @note
     * For non-Clang/GCC compilers, we store the source location information directly in member variables 
     * instead of using a pointer to an implementation struct.
     * There's also no strict requirement for the names of these member variables, but we use a similar 
     * naming convention to maintain consistency with the Clang/GCC implementation.
     */
    FilePathType        file_name_;
    FunctionNameType    function_name_;
    LineNumberType      line_;
    ColumnNumberType    column_;

public:
    // Create a SourceLocation object representing the current source location using compiler built-ins
    static consteval SourceLocation current() noexcept {
        return {
            NEX_SOURCE_FILE_PATH,
            NEX_SOURCE_FUNCTION_NAME,   // Detailed function signature or short function name based on user preference
            NEX_SOURCE_LINE_NUMBER,     // Already casted by the macro definition
            NEX_SOURCE_COLUMN_NUMBER    // Already casted by the macro definition
        };
    }

    // Default constructor for SourceLocation, 
    // which initializes the source location to an empty state (no file, no function, line 0, column 0).
    NEX_HIDDEN_FROM_ABI constexpr SourceLocation() noexcept
        : file_name_(""), function_name_(""), line_(0), column_(0) {}

    // Explicit constructor for SourceLocation, which allows the user to create a SourceLocation object with
    // specific file name, function name, line number, and column number
    NEX_HIDDEN_FROM_ABI constexpr 
    SourceLocation(FilePathType fileName, FunctionNameType functionName, 
                   LineNumberType lineNumber, ColumnNumberType columnNumber) noexcept
        : file_name_(fileName), function_name_(functionName), 
          line_(lineNumber), column_(columnNumber) {}

    // Get the line number for the source location
    NEX_HIDDEN_FROM_ABI constexpr LineNumberType line() const noexcept {
        return line_;
    }

    // Get the column number for the source location
    NEX_HIDDEN_FROM_ABI constexpr ColumnNumberType column() const noexcept {
        return column_;
    }

    // Get the file name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FilePathType fileName() const noexcept {
        return file_name_;
    }

    // Get the function name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FunctionNameType functionName() const noexcept {
        return function_name_;
    }

#endif  // ^^Non-clang/GCC compilers may not support __builtin_source_location() and related built-ins

};

// Define macro for capturing the current source location, which can be used for logging, error reporting, 
// debugging, and other purposes where information about the source location is valuable.
#define NEX_SOURCE_LOCATION \
    NEX_PREPEND_NAMESPACE(SourceLocation::current())

NEX_NAMESPACE_END
