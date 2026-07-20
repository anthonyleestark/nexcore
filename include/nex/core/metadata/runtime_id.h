/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <functional>

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"
#include "nex/base/primitives.h"
#include "nex/base/string.h"

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
class NEX_API RuntimeId {
public:
    // Underlying type for the RuntimeId
    using value_type = uint64;

    // Special constant values for RuntimeId
    static constexpr value_type invalidId = 0;
    static constexpr value_type maxId = NumericLimits<value_type>::max();

    /**
     * @brief Default constructor creates an invalid RuntimeId (value 0).
     * @note This constructor only allows for the default instantiation of objects with RuntimeId members
     * which will be invalid until explicitly set or generated, it should not be called directly in source code.
     */
    constexpr RuntimeId() noexcept = default;

    /**
     * @brief Explicit constructor from an unsigned integer value.
     * @note Use with caution, as the RuntimeId value may not be unique.
     */
    template<typename Type>
        requires (meta::IsIntegralV<Type> && meta::IsUnsignedIntegralV<Type>)
    explicit constexpr RuntimeId(Type value) noexcept : value_(static_cast<value_type>(value)) {}

    /**
     * @brief Generates the next runtime-unique identifier.
     * @return A new RuntimeId instance with a unique value.
     * @note The generated RuntimeId is guaranteed to be unique within the process and monotonically increasing.
     */
    static RuntimeId next() {
        // Starts from 1 because 0 is invalid
        static Atomic<value_type> counter{1};
        return RuntimeId(counter.fetch_add(1, NEX_STD memory_order_relaxed));
    }

    /**
     * @brief Gets an invalid RuntimeId.
     * @return A RuntimeId instance representing an invalid ID (value 0).
     */
    static RuntimeId invalid() noexcept {
        return RuntimeId{/*invalidId*/};
    }

    /**
     * @brief Creates a RuntimeId from a specific unsigned integer value.
     * @param value The unsigned integer value to create the RuntimeId from.
     * @return A RuntimeId instance with the specified value.
     * @note Use with caution, as the RuntimeId value may not be unique or valid.
     */
    template<typename Type>
        requires (meta::IsIntegralV<Type> && meta::IsUnsignedIntegralV<Type>)
    static RuntimeId fromNumber(Type value) noexcept {
        return RuntimeId(value);
    }

    /**
     * @brief Gets the underlying value of the RuntimeId.
     * @return The unsigned integer value representing the RuntimeId.
     */
    constexpr value_type value() const noexcept { return value_; }

    /**
     * @brief Converts the RuntimeId to the underlying value type.
     * @return The unsigned integer value representing the RuntimeId.
     */
    constexpr explicit operator value_type() const noexcept { return value_; }

    /**
     * @brief Checks if the RuntimeId is valid.
     * @return true if the RuntimeId is valid, false if it is invalid.
     */
    constexpr bool valid() const noexcept { return value_ != invalidId; }

    /**
     * @brief Explicit conversion to bool.
     * @return true if the RuntimeId is valid, false if it is invalid.
     */
    constexpr explicit operator bool() const noexcept { return valid(); }

    /**
     * @brief Converts the RuntimeId to a string representation.
     * @return A String representing the RuntimeId value.
     * @note The string representation is in decimal format.
     */
    String toString() const;

    /**
     * @brief Equality operator for comparing two RuntimeId instances.
     * @param other The other RuntimeId to compare with.
     * @return true if both RuntimeId instances have the same value, false otherwise.
     */
    constexpr bool operator==(const RuntimeId& other) const noexcept {
        return value_ == other.value_;
    }

    /**
     * @brief Inequality operator for comparing two RuntimeId instances.
     * @param other The other RuntimeId to compare with.
     * @return true if both RuntimeId instances have different values, false otherwise.
     */
    constexpr bool operator!=(const RuntimeId& other) const noexcept {
        return value_ != other.value_;
    }

    /**
     * @brief Strong ordering comparison operator for RuntimeId instances.
     * @param other The other RuntimeId to compare with.
     * @return A strong_ordering value indicating the relative order of the two RuntimeId instances.
     */
    constexpr NEX_STD strong_ordering operator<=>(const RuntimeId& other) const noexcept {
        return value_ <=> other.value_;
    }

private:
    // Internal value representing the ID (0 is invalid)
    value_type value_ = invalidId;
};

NEX_NAMESPACE_END

// Implicit hash specialization for RuntimeId
// to allow usage in hash-based containers like std::unordered_map

NEX_STD_BEGIN

template<>
struct hash<NEX_PREPEND_NAMESPACE(RuntimeId)> {
    constexpr size_t operator()(const NEX_PREPEND_NAMESPACE(RuntimeId)& id) const noexcept {
        return NEX_STD hash<NEX_PREPEND_NAMESPACE(RuntimeId::value_type)>{}(id.value());
    }
};

NEX_STD_END
