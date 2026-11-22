/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  location.h
 * @brief Provides macros and utilities for capturing source location information (file, line, function, etc.)
 *        for debugging, logging, analysis, and error reporting purposes.
 * 
 * @details
 * This header defines macros and utilities for capturing source code location information, such as
 * file name, line number, and function name. This information can be used for debugging, logging,
 * analysis, and error reporting purposes to provide more context about where in the source code
 * an event occurred. The macros defined in this header are designed to be easy to use and provide
 * consistent source location information across different compilers and platforms, making it easier
 * to maintain and debug code in a cross-platform environment.
 */

#include "nex/base/compiler.h"
#include "nex/base/compiler_features.h"
#include "nex/base/attributes.h"
#include "nex/base/visibility.h"
#include "nex/base/namespace.h"

// ================================================================================================
// Configuration for source location utilities
// ================================================================================================

// User-configurable macro to enable/disable the use of compiler built-ins for capturing
// detailed function signatures (e.g., __PRETTY_FUNCTION__, __FUNCSIG__), which can provide
// more detailed information about the function, such as parameter types and template arguments,
// but may be less readable than just the function name, so we allow the user to choose whether
// to use the detailed function signature or just the function name for source location information.
#if !defined(NEX_USE_DETAILED_FUNCSIG)
    #define NEX_USE_DETAILED_FUNCSIG 0
#elif NEX_USE_DETAILED_FUNCSIG
    #undef NEX_USE_DETAILED_FUNCSIG
    #define NEX_USE_DETAILED_FUNCSIG 1
#endif  // ^^NEX_USE_DETAILED_FUNCSIG

// ================================================================================================
// Source location utility class and macros
// ================================================================================================

#if NEX_COMPILER_IS_CLANG_BASED && NEX_HAS_BUILTIN(__builtin_source_location)
    #define NEX_HAS_BUILTIN_SOURCE_LOCATION 1
#else  // Compiler does not support __builtin_source_location()
    #define NEX_HAS_BUILTIN_SOURCE_LOCATION 0
#endif  // ^^NEX_HAS_BUILTIN_SOURCE_LOCATION

#if NEX_HAS_BUILTIN_SOURCE_LOCATION
    #if defined(NEX_NO_STL)
        NEX_STD_BEGIN

        // Provide a minimal implementation of std::source_location for Clang
        // when the STL is not available.
        class source_location {
        public:
            /**
             * @note
             * The names source_location::__impl, _M_file_name, _M_function_name, _M_line, and _M_column
             * are hard-coded in the compiler and must not be changed here.
             */
            struct __impl {
                const char* _M_file_name;
                const char* _M_function_name;
                unsigned _M_line;
                unsigned _M_column;
            };
            using __bsl_ty NEX_NODEBUG = decltype(__builtin_source_location());

        private:
            const __impl* __ptr_ = nullptr;
        };

        NEX_STD_END
    #else
        #include <version>
        #if defined(__cpp_lib_source_location) && __cpp_lib_source_location >= 201907L
            #include <source_location>
            #define NEX_HAS_STD_SOURCE_LOCATION 1
        #else
            #define NEX_HAS_STD_SOURCE_LOCATION 0
        #endif
    #endif
#endif

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
#if NEX_HAS_BUILTIN_SOURCE_LOCATION
    /**
     * @note
     * The following type should match the Clang/GCC built-in source location's
     * internal representation. It is important to ensure that these types align
     * with the compiler's expectations to avoid undefined behavior.
     */
    using FilePathType      = const char*;
    using FunctionNameType  = const char*;
    using LineNumberType    = unsigned;
    using ColumnNumberType  = unsigned;
#else
    using FilePathType                      = const char*;
    using FunctionNameType                  = const char*;

    #if NEX_HAS_BUILTIN(__builtin_LINE)
        using LineNumberType NEX_NODEBUG    = decltype(__builtin_LINE());
    #else
        using LineNumberType                = int;
    #endif

    #if NEX_HAS_BUILTIN(__builtin_COLUMN)
        using ColumnNumberType NEX_NODEBUG  = decltype(__builtin_COLUMN());
    #else
        using ColumnNumberType              = int;
    #endif
#endif

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
 * @brief Gets the file path for source location
 * @note
 * Whether the returned string is a full path or a relative path depends entirely on how the
 * file path was supplied to the compiler during the build process (e.g., via CMake or CLI).
 * To enforce short file names across all build environments, consider using compiler flags
 * like `-fmacro-prefix-map` or `-ffile-prefix-map`.
 */
#if NEX_HAS_BUILTIN(__builtin_FILE)
    #define NEX_SOURCE_FILE_PATH __builtin_FILE()
#else  // Compiler does not support __builtin_FILE()
    #define NEX_SOURCE_FILE_PATH __FILE__
#endif  // ^^NEX_SOURCE_FILE_PATH

/** @brief Gets the line number for source location */
#if NEX_HAS_BUILTIN(__builtin_LINE)
    #define NEX_SOURCE_LINE_NUMBER  \
        static_cast<NEX_PREPEND_NAMESPACE(SourceLocation::LineNumberType)>(__builtin_LINE())
#else  // Compiler does not support __builtin_LINE()
    #define NEX_SOURCE_LINE_NUMBER  \
        static_cast<NEX_PREPEND_NAMESPACE(SourceLocation::LineNumberType)>(__LINE__)
#endif  // ^^NEX_SOURCE_LINE_NUMBER

/**
 * @brief Gets the column number for source location
 * @note
 * The column number might be different between compilers, especially if you are using Tab ('\t') characters
 * in your source code, as different compilers may interpret the width of a Tab character differently.
 */
#if NEX_HAS_BUILTIN(__builtin_COLUMN)
    #define NEX_SOURCE_COLUMN_NUMBER  \
        static_cast<NEX_PREPEND_NAMESPACE(SourceLocation::ColumnNumberType)>(__builtin_COLUMN())
#else  // Compiler does not support __builtin_COLUMN()
    #define NEX_SOURCE_COLUMN_NUMBER  \
        static_cast<NEX_PREPEND_NAMESPACE(SourceLocation::ColumnNumberType)>(0)
#endif  // ^^NEX_SOURCE_COLUMN_NUMBER

/** @brief Gets the function name for source location */
#if NEX_USE_DETAILED_FUNCSIG
    #if NEX_HAS_BUILTIN(__builtin_FUNCSIG)
        /**
         * @note
         * Prefer __builtin_FUNCSIG() when available because it provides the full function signature,
         * including parameter types, calling convention, and other compiler-specific details.
         * This is particularly useful for MSVC and Clang in MSVC compatibility mode.
         */
        #define NEX_SOURCE_FUNCTION_NAME __builtin_FUNCSIG()
    #elif NEX_COMPILER_IS_MSVC
        /**
         * @note
         * MSVC provides __FUNCSIG__ as a predefined macro containing the full function signature.
         * Use it as a fallback when __builtin_FUNCSIG() is unavailable.
         */
        #define NEX_SOURCE_FUNCTION_NAME __FUNCSIG__
    #elif NEX_COMPILER_IS_CLANG_BASED || NEX_COMPILER_GCC_COMPATIBLE
        /**
         * @note
         * Clang and GCC provide __PRETTY_FUNCTION__ as a predefined identifier containing
         * the function signature, including parameter types and template information.
         * Prefer it over __builtin_FUNCTION(), which only provides the function name.
         */
        #define NEX_SOURCE_FUNCTION_NAME __PRETTY_FUNCTION__
    #else
        /**
         * @note
         * Use the standard __func__ identifier as a portable fallback.
         * Unlike compiler-specific alternatives, it provides only the function name,
         * without parameter types or other signature details.
         * @see https://en.cppreference.com/w/c/language/function_definition#func
         */
        #define NEX_SOURCE_FUNCTION_NAME __func__
    #endif
#else  // User prefers short function names
    #if NEX_HAS_BUILTIN(__builtin_FUNCTION)
        /**
         * @note
         * Prefer __builtin_FUNCTION() when available because it directly provides the function name
         * without requiring additional processing to strip the signature.
         */
        #define NEX_SOURCE_FUNCTION_NAME __builtin_FUNCTION()
    #else
        /**
         * @note
         * Use the standard __func__ identifier as a portable fallback.
         * It provides the function name without the full signature.
         * @see https://en.cppreference.com/w/c/language/function_definition#func
         */
        #define NEX_SOURCE_FUNCTION_NAME __func__
    #endif
#endif  // ^^NEX_SOURCE_FUNCTION_NAME

// ================================================================================================
// Source location class implementation
// ================================================================================================

#if NEX_HAS_BUILTIN_SOURCE_LOCATION && defined(NEX_NO_STL)

private:
    // Here we use our own source_location's implementation struct.
    using Impl NEX_NODEBUG = NEX_STD source_location::__impl;
    const Impl* __ptr_ = nullptr;

    using BuiltinSourceLocationPtr NEX_NODEBUG = decltype(__builtin_source_location());

public:
    /**
     * @brief Captures the current source location.
     * @note
     * The defaulted `ptr` argument is necessary here, so it ensures that the builtin is evaluated
     * within the context of the caller. An explicit value should never be provided, as it would defeat
     * the purpose of capturing the caller's source location.
     */
    static consteval SourceLocation current(
        BuiltinSourceLocationPtr ptr = __builtin_source_location()
    ) noexcept {
        SourceLocation loc;
        // The cast ensures that the pointer is of the correct type for the implementation struct.
        loc.__ptr_ = static_cast<const Impl*>(ptr);
        return loc;
    }

    // Default constructor for SourceLocation,
    // which initializes the source location to an empty state (no file, no function, line 0, column 0).
    NEX_HIDDEN_FROM_ABI constexpr SourceLocation() noexcept = default;

    // Returns the line number for the source location
    NEX_HIDDEN_FROM_ABI constexpr LineNumberType line() const noexcept {
        return __ptr_ != nullptr ? __ptr_->_M_line : 0;
    }

    // Returns the column number for the source location
    NEX_HIDDEN_FROM_ABI constexpr ColumnNumberType column() const noexcept {
        return __ptr_ != nullptr ? __ptr_->_M_column : 0;
    }

    // Returns the file name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FilePathType fileName() const noexcept {
        return __ptr_ != nullptr ? __ptr_->_M_file_name : "";
    }

#if NEX_USE_DETAILED_FUNCSIG
    // Returns the full function signature for the source location
    NEX_HIDDEN_FROM_ABI constexpr FunctionNameType functionName() const noexcept {
        return __ptr_ != nullptr ? __ptr_->_M_function_name : "";
    }
#else  // User prefers short function names, so we strip the function signature to get only the function name
    // Returns the function name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FunctionNameType functionName() const noexcept {
        return __ptr_ != nullptr
            ? stripFunctionSignature(__ptr_->_M_function_name)
            : "";
    }
#endif  // !NEX_USE_DETAILED_FUNCSIG

#elif NEX_HAS_BUILTIN_SOURCE_LOCATION && NEX_HAS_STD_SOURCE_LOCATION

private:
    // Here we use the C++ standard library's source_location as the underlying representation.
    using Repr NEX_NODEBUG = NEX_STD source_location;
    Repr location_;

    // Private constructor that initializes the SourceLocation with a given source_location object.
    explicit constexpr SourceLocation(Repr loc) noexcept
        : location_(loc) {}

public:
    /**
     * @brief Captures the current source location.
     * @note
     * The defaulted `loc` argument is necessary here, so it ensures that the builtin is evaluated
     * within the context of the caller. An explicit value should never be provided, as it would defeat
     * the purpose of capturing the caller's source location.
     */
    static consteval SourceLocation current(Repr loc = Repr::current()) noexcept {
        return SourceLocation(loc);
    }

    // Default constructor for SourceLocation,
    // which initializes the source location to an empty state (no file, no function, line 0, column 0).
    NEX_HIDDEN_FROM_ABI constexpr SourceLocation() noexcept = default;

    // Returns the line number for the source location
    NEX_HIDDEN_FROM_ABI constexpr LineNumberType line() const noexcept {
        return static_cast<LineNumberType>(location_.line());
    }

    // Returns the column number for the source location
    NEX_HIDDEN_FROM_ABI constexpr ColumnNumberType column() const noexcept {
        return static_cast<ColumnNumberType>(location_.column());
    }

    // Returns the file name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FilePathType fileName() const noexcept {
        return location_.file_name();
    }

#if NEX_USE_DETAILED_FUNCSIG
    // Returns the full function signature for the source location
    NEX_HIDDEN_FROM_ABI constexpr FunctionNameType functionName() const noexcept {
        return location_.function_name();
    }
#else  // User prefers short function names, so we strip the function signature to get only the function name
    // Returns the function name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FunctionNameType functionName() const noexcept {
        return stripFunctionSignature(location_.function_name());
    }
#endif  // !NEX_USE_DETAILED_FUNCSIG

#else  // ^^Non-clang/GCC compilers may not support __builtin_source_location() and related built-ins

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
    /**
     * @brief Captures the current source location.
     * @note
     * The defaulted `ptr` arguments are necessary here, so that the builtins are evaluated
     * within the context of the caller. Explicit values should never be provided, as it would defeat
     * the purpose of capturing the caller's source location.
     */
    static consteval SourceLocation current(
        FilePathType fileName         = NEX_SOURCE_FILE_PATH,
        FunctionNameType functionName = NEX_SOURCE_FUNCTION_NAME,
        LineNumberType lineNumber     = NEX_SOURCE_LINE_NUMBER,
        ColumnNumberType columnNumber = NEX_SOURCE_COLUMN_NUMBER
    ) noexcept {
        return {
            fileName,
            functionName,   // Detailed func. signature or short func. name based on user preference
            lineNumber,     // Already casted by the macro definition
            columnNumber    // Already casted by the macro definition
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

    // Returns the line number for the source location
    NEX_HIDDEN_FROM_ABI constexpr LineNumberType line() const noexcept {
        return line_;
    }

    // Returns the column number for the source location
    NEX_HIDDEN_FROM_ABI constexpr ColumnNumberType column() const noexcept {
        return column_;
    }

    // Returns the file name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FilePathType fileName() const noexcept {
        return file_name_;
    }

    // Returns the function name for the source location
    NEX_HIDDEN_FROM_ABI constexpr FunctionNameType functionName() const noexcept {
        return function_name_;
    }

#endif

};

// Define macro for capturing the current source location, which can be used for logging, error reporting,
// debugging, and other purposes where information about the source location is valuable.
#define NEX_SOURCE_LOCATION \
    NEX_PREPEND_NAMESPACE(SourceLocation::current())

NEX_NAMESPACE_END
