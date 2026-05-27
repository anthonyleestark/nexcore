/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/traits.h"
#include "nex/base/wrappers.h"

NEX_NAMESPACE_BEGIN

/**
 * @struct PropertyPolicy
 * @brief  A struct that defines the policy for a property
 */
struct PropertyPolicy {
    bool hasValidator = false;
    bool hasCallback = false;
};

/**
 * @section Available policies
 * - `PropertyPolicy::Default`: A default policy with no validator or callback.
 * - `PropertyPolicy::WithValidator`: A policy that includes a validator function.
 * - `PropertyPolicy::WithCallback`: A policy that includes a callback function.
 * - `PropertyPolicy::WithValidatorAndCallback`: A policy that includes both a validator and a callback function.
 */
constexpr PropertyPolicy Default = {};
constexpr PropertyPolicy WithValidator = { .hasValidator = true };
constexpr PropertyPolicy WithCallback = { .hasCallback = true };
constexpr PropertyPolicy WithValidatorAndCallback = { .hasValidator = true, .hasCallback = true };

/**
 * @class Property
 * @brief  A class template that represents a property with a specific policy
 * 
 * @details
 * The `Property` class template allows you to define properties with customizable behavior based on the specified policy. 
 * The policy determines whether the property includes a validator function (to validate new values before setting them) 
 * and/or a callback function (to be called after the property value changes). 
 * 
 * @tparam T The type of the property value
 * @tparam Policy The policy that defines the behavior of the property
 * 
 * @par Example Usage:
 * @code
 * // Define a property with a validator and a callback
 * Property<int, WithValidatorAndCallback> myProperty;
 * myProperty.setValidator([](const int& newValue) {
 *     return newValue >= 0; // Only allow non-negative values
 * });
 * myProperty.setCallback([](const int& newValue) {
 *     std::cout << "Property value changed to: " << newValue << std::endl;
 * });
 * myProperty.set(10); // Valid value, callback will be called
 * myProperty.set(-5); // Invalid value, will be rejected by the validator
 * @endcode
 * 
 * @par Notes:
 * - The `set` method will only update the property value if the new value passes validation (if a validator 
 *   is defined) and will call the callback function (if defined) after updating the value.
 * - The `get` method allows you to retrieve the current value of the property.
 * - The class uses C++20 features such as concepts and conditional types to enable or disable functionality 
 *   based on the specified policy.
 */
template <typename T, PropertyPolicy Policy = Default>
class NEX_API Property {
public:
    // Type aliases for the property functions based on the policy
    using NullFunction = NEX_STD monostate;
    using ValidatorFuncType = Function<bool(const T&)>;
    using CallbackFuncType = Function<void(const T&)>;

    // Type aliases for the property's attributes
    using ValueType = T;
    using Validator = Conditional<Policy.hasValidator, ValidatorFuncType, NullFunction>;
    using Callback = Conditional<Policy.hasCallback, CallbackFuncType, NullFunction>;

private:
    // Internal attributes of the property
    ValueType value_{};
    NEX_NO_UNIQUE_ADDRESS Validator validator_;
    NEX_NO_UNIQUE_ADDRESS Callback callback_;

public:
    // Default constructor
    constexpr Property() noexcept(IsNothrowCopyConstructibleV<ValueType>) = default;

    // Constructors for initializing the property value
    constexpr explicit Property(const ValueType& v) : value_(v) {}
    constexpr explicit Property(ValueType&& v) noexcept(IsNothrowMoveConstructibleV<ValueType>)
        : value_(NEX_MOVE(v)) {}

    ////// Getters for the property value --------------------------------------------------

    // Returns a const reference to the property value
    NEX_NODISCARD const ValueType& get() const noexcept { return value_; }

    // Implicit conversion operator and dereference operator for convenient access to the property value
    NEX_NODISCARD operator const ValueType&() const noexcept { return value_; }

    // Returns a const reference to the property value (dereference operator)
    NEX_NODISCARD const ValueType& operator*() const noexcept { return value_; }

    // Returns a pointer to the property value (arrow operator)
    NEX_NODISCARD const ValueType* operator->() const noexcept 
    requires IsPointerV<ValueType> { 
        return value_; 
    }

    ////// Setters for the property value --------------------------------------------------

    // Sets the property value with validation and callback support based on the policy
    void set(const ValueType& newVal) {
        if constexpr (Policy.hasValidator) {
            if (validator_ && !validator_(newVal)) return;
        }

        // Store the old value before updating the property value
        ValueType oldVal = value_;
        value_ = newVal;

        if constexpr (Policy.hasCallback) {
            if (callback_) callback_(newVal);
        }
    }

    // Sets the property value with move semantics, validation, and callback support based on the policy
    void set(ValueType&& newVal) {
        if constexpr (Policy.hasValidator) {
            if (validator_ && !validator_(newVal)) return;
        }

        // Update the property value using move semantics
        value_ = NEX_MOVE(newVal);

        if constexpr (Policy.hasCallback) {
            if (callback_) callback_(value_);
        }
    }

    // Assignment operator for setting the property value
    Property& operator=(const ValueType& v) { set(v); return *this; }

    // Assignment operator for setting the property value with move semantics
    Property& operator=(ValueType&& v) { set(NEX_MOVE(v)); return *this; }

    ////// Configuration methods ---------------------------------------------------------------

    // Sets the validator function for the property (only if the policy includes a validator)
    template <typename ValidatorFunc>
    void setValidator(ValidatorFunc&& func)
    requires Policy.hasValidator && IsInvocableRV<bool, ValidatorFunc, const ValueType&> {
        validator_ = NEX_FORWARD(ValidatorFunc, func);
    }

    // Sets the callback function for the property (only if the policy includes a callback)
    template <typename CallbackFunc>
    void setCallback(CallbackFunc&& func)
    requires Policy.hasCallback && IsInvocableRV<void, CallbackFunc, const ValueType&> {
        callback_ = NEX_FORWARD(CallbackFunc, func);
    }
};

NEX_NAMESPACE_END
