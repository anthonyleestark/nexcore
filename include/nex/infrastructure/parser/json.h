/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/wrappers.h"
#include "nex/core/text/string.h"
#include "nex/core/text/string_view.h"
#include "nex/base/result.h"
#include "nex/base/error.h"

NEX_INFRA_NAMESPACE_BEGIN

// Access common core types
NEX_IMPORT_TYPE_FROM_LAYER(core, String);
NEX_IMPORT_TYPE_FROM_LAYER(core, StringView);

/**
 * @class   Json
 * @brief   A safe, high-performance JSON adapter for NexCore.
 *
 * @details
 * The `Json` class provides a thin abstraction layer over `nlohmann::json`. It employs the Pimpl idiom 
 * to minimize compile-time dependencies while offering a non-throwing, error-aware API for configuration 
 * and data parsing.
 *
 * Key Functionality
 * - Safe Retrieval: Type-specific getters with default values and `tryGet` variants to avoid exceptions.
 * - Path Navigation: Access nested data using dot-notation (e.g., "server.port") or lightweight 
 *   `NodeHandle` objects for read-only traversal.
 * - Array Support: Specialized index-based access through `tryGet*At` methods.
 * - Serialization: Convert to UTF-8 strings with optional pretty-printing.
 *
 * @note 
 * Key paths use dot-separation. Use `tryGet*At` for array indices rather than embedding brackets in path strings.
 * 
 * @warning 
 * `NodeHandle` references are only valid as long as the underlying adapter remains unmodified.
 */
class NEX_EXPORT Json {
public:
    // Constructor
    Json();

    // Destructor
    NEX_DECLARE_DEFAULT_DTOR(Json);

    // Non-copyable
    NEX_DISALLOW_COPY(Json);

    // Default move semantics
    NEX_DEFAULT_MOVE(Json);

    ////// Load/Parse JSON from external sources -----------------------

    // Load JSON from file
    Result<void> loadFromFile(StringView filePath);

    // Load JSON from string
    Result<void> loadFromString(StringView jsonString);

    ////// Accessors -----------------------

    // Get string value by key path (e.g., "application.name")
    String getString(StringView keyPath, StringView defaultValue = StringView()) const;

    // Get integer value by key path
    int32 getInt(StringView keyPath, int32 defaultValue = 0) const;

    // Get unsigned integer value by key path
    uint32 getUInt(StringView keyPath, uint32 defaultValue = 0) const;

    // Get boolean value by key path
    bool getBool(StringView keyPath, bool defaultValue = false) const;

    // Get double value by key path
    float64 getDouble(StringView keyPath, float64 defaultValue = 0.0) const;

    ////// Try-getters (return false if key not found or type mismatch) -----------------------

    // Try to get string value by key path
    bool tryGetString(StringView keyPath, String& value) const;

    // Try to get integer value by key path
    bool tryGetInt(StringView keyPath, int32& value) const;

    // Try to get unsigned integer value by key path
    bool tryGetUInt(StringView keyPath, uint32& value) const;

    // Try to get boolean value by key path
    bool tryGetBool(StringView keyPath, bool& value) const;

    // Try to get double value by key path
    bool tryGetDouble(StringView keyPath, float64& value) const;

    ////// Validation ------------------------

    // Check if JSON is valid/loaded
    bool isValid() const noexcept;

    // Check if key path exists
    bool hasKey(StringView keyPath) const;

    // Check if key path is an object
    bool isObject(StringView keyPath) const;

    // Check if key path is an array
    bool isArray(StringView keyPath) const;

    ////// Array accessors -----------------------

    // Get size of array at key path
    usize arraySize(StringView keyPath) const;

    // Try to get string value at array index by key path (e.g., "users[0].name")
    bool tryGetStringAt(StringView keyPath, usize index, String& value) const;

    // Try to get integer value at array index by key path
    bool tryGetIntAt(StringView keyPath, usize index, int32& value) const;

    ////// String conversion -----------------------

    // Convert JSON to string (pretty-printed if specified)
    String toString(bool pretty = false) const;

private:
    // Internal implementation
    struct Impl;
    UniquePtr<Impl> impl_;

private:
    // Internal node handle (opaque, implementation-specific)
    struct NodeHandle;

    // Navigate nested JSON by key path and return internal node handle.
    // The handle is only valid while the adapter remains unchanged.
    const NodeHandle* navigatePath(StringView path) const;
};

NEX_INFRA_NAMESPACE_END
