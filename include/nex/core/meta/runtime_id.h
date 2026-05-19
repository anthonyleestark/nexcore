/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <functional>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/primitives.h"
#include "nex/core/text/string.h"

NEX_NAMESPACE_BEGIN

/**
 * @class   RuntimeId
 * @brief   Runtime unique identifiers for objects/entities.
 * 
 * The `RuntimeId` class provides a mechanism for generating and managing unique identifiers for objects 
 * at runtime. It is designed to be used in scenarios where objects need to be uniquely identified and tracked 
 * during their lifetime, such as in object management systems, event systems, or any context where unique 
 * object identity is required.
 * 
 * @details
 * The `RuntimeId` class generates unique identifiers in a monotonic, process-local manner, starting from 1
 * (since 0 is reserved as an invalid ID). Each time a new `RuntimeId` is generated, it increments an internal
 * counter to ensure that each ID is unique within the process. The class provides methods for checking
 * the validity of an ID, converting it to a string, and comparing IDs.
 * 
 * @note A RuntimeId with value 0 is considered invalid.
 * @note RuntimeIds generated at runtime are guaranteed to be unique within the process and monotonically 
 *       increasing. However, they are not globally unique across different runs or systems. For global 
 *       uniqueness, consider using UUIDs or similar mechanisms.
 * @note RuntimeIds can be used as keys in hash-based containers like std::unordered_map, and support 
 *       comparison operators for ordered containers.
 * @note RuntimeIds are immutable once created, ensuring consistent identity throughout their lifetime.
 * @note The RuntimeId class supports directly construction from uint64 values, but this should be used 
 *       with caution to avoid potential ID collisions or invalid IDs, as it bypasses the internal uniqueness 
 *       guarantees; only use this in special cases when you are certain the value is valid and unique.
 * 
 * @see Object management systems
 * @see Uuid class for globally unique identifiers
 */
class NEX_EXPORT RuntimeId {
public:
    // Constants
    static constexpr uint64 kInvalidId = 0;
    static constexpr uint64 kMaxId = UINT64_MAX;

    ////// Construction -----------------------

    // Default constructor (creates an invalid ID)
    constexpr RuntimeId() noexcept : value_(kInvalidId) {}

    // Explicit constructor from an uint64 value (use with caution)
    explicit constexpr RuntimeId(uint64 value) noexcept : value_(value) {}

    ////// Factory method for generating new RuntimeIds -----------------------
    
    // Generate a new runtime-unique Id (monotonic, process-local)
    static RuntimeId generate() {
        // Start from 1 because 0 is invalid
        static AtomicUInt64 counter{1};
        return RuntimeId(counter.fetch_add(1, NEX_STD memory_order_relaxed));
    }

    // Get an invalid RuntimeId
    static RuntimeId invalid() noexcept {
        return RuntimeId{/*kInvalidId*/};
    }

    // Create a RuntimeId from a uint64 value (use with caution, as it may not be unique)
    static RuntimeId fromUInt64(uint64 value) noexcept {
        return RuntimeId(value);
    }

    ////// Accessors and validation -----------------------
    
    // Get the underlying value of the RuntimeId
    constexpr uint64 get() const noexcept { return value_; }

    // Explicit conversion to uint64 (returns the underlying value of the RuntimeId)
    constexpr explicit operator uint64() const noexcept { return value_; }

    // Check if a RuntimeId is valid
    constexpr bool isValid() const noexcept { return value_ != kInvalidId; }
    
    // Check if a RuntimeId is invalid (equals to zero)
    constexpr bool isInvalid() const noexcept { return value_ == kInvalidId; }

    // Explicit conversion to bool (true if valid, false if invalid)
    constexpr explicit operator bool() const noexcept { return isValid(); }

    ////// Conversion -----------------------
    
    // Convert to string
    String toString() const {
        return String::fromUInt(value_);
    }

    ////// Comparison operators -----------------------

    // Equality operators
    constexpr bool operator==(const RuntimeId& other) const noexcept {
        return value_ == other.value_;
    }

    // Inequality operator
    constexpr bool operator!=(const RuntimeId& other) const noexcept {
        return value_ != other.value_;
    }
    
    // Strong ordering comparison operator (enables use in ordered containers and comparisons)
    constexpr NEX_STD strong_ordering operator<=>(const RuntimeId& other) const noexcept {
        return value_ <=> other.value_;
    }
    
private:
    // Internal value representing the ID (0 is invalid)
    uint64 value_;
};

NEX_NAMESPACE_END

// Implicit hash specialization for RuntimeId
// to allow usage in hash-based containers like std::unordered_map

NEX_STD_BEGIN

template<>
struct hash<NEX_PREPEND_NAMESPACE(RuntimeId)> {
    constexpr size_t operator()(const NEX_PREPEND_NAMESPACE(RuntimeId)& id) const noexcept {
        return NEX_STD hash<NEX_PREPEND_NAMESPACE(uint64)>{}(id.get());
    }
};

NEX_STD_END
