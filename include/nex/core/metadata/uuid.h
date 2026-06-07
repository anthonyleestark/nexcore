/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/string.h"
#include "nex/base/result.h"
#include "nex/base/wrappers.h"

NEX_NAMESPACE_BEGIN

/**
 * @class   Uuid
 * @brief   Universally Unique Identifier (UUID) class
 * 
 * The Uuid class represents a universally unique identifier (UUID), which is a 128-bit value 
 * used to uniquely identify objects, resources, or entities across different systems and contexts. 
 * The Uuid class provides methods for generating new UUIDs, parsing UUIDs from strings, converting 
 * UUIDs to strings, and comparing UUIDs.
 * 
 * @details
 * The Uuid class supports generating UUIDs using different versions, including version 4 (random) 
 * and version 7 (time-ordered). It also provides a method for creating a nil/invalid UUID, which has 
 * all bits set to zero. The class uses the Pimpl idiom to hide implementation details and allow for 
 * future changes without breaking ABI compatibility. The internal implementation of the Uuid class 
 * is based on the stduuid library, which provides efficient and standards-compliant UUID generation 
 * and parsing functionality.
 * 
 * @note
 * The Uuid class is designed to be used in a wide range of applications where unique identifiers are needed, 
 * such as for identifying resources, tracking entities, or generating unique keys. 
 * The class provides a simple and convenient interface for working with UUIDs, while leveraging the capabilities 
 * of the stduuid library for the underlying implementation. The Uuid class is also designed to be efficient 
 * and thread-safe, making it suitable for use in concurrent applications.
 * 
 * @see stduuid library for more details on the underlying implementation of UUID generation and parsing.
 * @see RuntimeId class for generating unique identifiers in a monotonic, process-local manner.
 */
class NEX_API Uuid {
public:
    ////// Construction -----------------------

    // Default constructor (create a nil/invalid UUID with all bits set to zero)
    explicit Uuid() noexcept;

    // Destructor
    NEX_DECLARE_DEFAULT_DTOR(Uuid);

    // Copy constructor and copy assignment operator
    Uuid(const Uuid& other);
    Uuid& operator=(const Uuid& other);

    // Default move semantics
    NEX_DECLARE_DEFAULT_MOVE(Uuid);

    ////// Factory methods for generating and creating Uuids -----------------------

    // Generate a new UUID (universally unique identifier; uses V4 random generation by default)
    static Uuid generate() noexcept;

    // Get a nil/invalid UUID (all bits set to zero)
    static Uuid nil() noexcept;

    // Generate a new version 4 (random) UUID
    static Uuid generateV4() noexcept;

    // Generate a new version 7 (time-ordered) UUID
    static Uuid generateV7() noexcept;

    // Parse a Uuid from a string representation
    // The string should be in the standard UUID format (e.g., "123e4567-e89b-12d3-a456-426614174000").
    // Returns a Result containing the parsed Uuid if successful, or an Error if the string is not a valid UUID.
    static Result<Uuid> fromString(StringView str) noexcept;

    ////// Accessors and validation -----------------------

    // Check if the Uuid is nil/invalid (all bits equal to zero)
    bool isNil() const noexcept;

    ////// Conversion methods -----------------------

    // Convert to string
    String toString() const;

    ////// Hashing support -----------------------

    // Get a hash value for the Uuid (enables use in hash-based containers)
    usize hash() const noexcept;

    ////// Comparison operators -----------------------

    // Equality operators
    bool operator==(const Uuid& other) const noexcept = default;

    // Inequality operator
    bool operator!=(const Uuid& other) const noexcept = default;

    // Strong ordering comparison operator (enables use in ordered containers and comparisons)
    NEX_STD strong_ordering operator<=>(const Uuid& other) const noexcept = default;

private:
    // Hidden implementation using the Pimpl idiom to avoid exposing internal details 
    // and to allow for future changes without breaking ABI compatibility
    struct Impl;
    UniquePtr<Impl> impl_;
};

NEX_NAMESPACE_END

// Hash specialization for Uuid 
// to enable use in hash-based containers like std::unordered_map

NEX_STD_BEGIN

template<>
struct hash<NEX_PREPEND_NAMESPACE(Uuid)> {
    size_t operator()(const NEX_PREPEND_NAMESPACE(Uuid)& uuid) const noexcept {
        return uuid.hash();
    }
};

NEX_STD_END
