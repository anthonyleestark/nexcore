/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <memory>
#include <functional>

#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/error.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @note
 * The NEX_REPORT_RESULT_INVALID_ACCESS macro is specialized for the Result class to handle invalid access 
 * attempts when trying to retrieve a value from a Result that is in an error state, or trying to retrieve 
 * an error from a Result that is in a success state. 
 * In such cases, it will either throw an exception with the provided message if in a constexpr context, 
 * or it will call NEX_FATAL to crash the program with the message. 
 * This ensures that invalid access attempts are caught and handled appropriately, preventing undefined behavior 
 * and providing clear feedback about the nature of the error.
 * The use of a separate non-constexpr handler allows for more efficient error handling in runtime contexts 
 * while still providing safety in compile-time contexts.
 */
#if !defined(NEX_REPORT_RESULT_INVALID_ACCESS)
    NEX_HIDDEN_FROM_ABI inline 
    void __runtimeResultInvalidAccessHandler(cstring msg) { NEX_FATAL(msg); }
    #define NEX_REPORT_RESULT_INVALID_ACCESS(msg) \
        do { \
            if NEX_CONSTEVAL_CONTEXT { \
                throw(msg); \
            } else { \
                __runtimeResultInvalidAccessHandler(msg); \
            } \
        } while (0)
#endif  // !defined(NEX_REPORT_RESULT_INVALID_ACCESS)

/**
 * @brief Compile-time evaluator for Result value types.
 * @details
 * This trait is used to evaluate the suitability of a type as the value type in a Result. 
 * If the type does not meet these requirements, a static assertion will fail, providing 
 * a clear error message about the issue.
 */
template <typename ValueType>
struct NEX_INTERNAL ResultValueTypeEvaluator : meta::TrueType {
    static_assert(meta::IsVoidV<ValueType> || 
        meta::IsCopyConstructibleV<ValueType> || meta::IsMoveConstructibleV<ValueType>,
        "Error: Value type must be void or copy/move constructible");

    static_assert(!meta::IsSameV<ValueType, monostate>,     "Error: Value type cannot be monostate");
    static_assert(!meta::IsSameV<ValueType, in_place_tag>,  "Error: Value type cannot be in_place_tag");
    static_assert(!meta::IsFunctionV<ValueType>,            "Error: Value type cannot be a function type");
    static_assert(!meta::IsReferenceV<ValueType>,           "Error: Value type cannot be a reference type");
    static_assert(!meta::IsArrayV<ValueType>,               "Error: Value type cannot be an array type");
};

/**
 * @brief Compile-time evaluator for Result error types.
 * @details
 * This trait is used to evaluate the suitability of a type as the error type in a Result. 
 * If the type does not meet these requirements, a static assertion will fail, providing 
 * a clear error message about the issue.
 */
template <typename ErrorType>
struct NEX_INTERNAL ResultErrorTypeEvaluator : meta::TrueType {
    static_assert(meta::IsCopyConstructibleV<ErrorType> || meta::IsMoveConstructibleV<ErrorType>,
        "Error: Error type must be copy/move constructible");

    static_assert(!meta::IsVoidV<ErrorType>,                "Error: Error type cannot be void");
    static_assert(!meta::IsSameV<ErrorType, monostate>,     "Error: Error type cannot be monostate");
    static_assert(!meta::IsSameV<ErrorType, in_place_tag>,  "Error: Error type cannot be in_place_tag");
    static_assert(!meta::IsFunctionV<ErrorType>,            "Error: Error type cannot be a function type");
    static_assert(!meta::IsReferenceV<ErrorType>,           "Error: Error type cannot be a reference type");
    static_assert(!meta::IsArrayV<ErrorType>,               "Error: Error type cannot be an array type");
};

/**
 * @brief Internal tags for Result construction.
 * @details
 * These tags are used internally by the ResultStorage to differentiate between different construction scenarios,
 * such as in-place construction of the value or error, and not intended for public use.
 */
struct NEX_INTERNAL ResultValueInPlaceConstructionTag {};
struct NEX_INTERNAL ResultErrorInPlaceConstructionTag {};

/**
 * @brief Helper template for conditional no unique address optimization in ResultStorage.
 * @details
 * This template is used to indicate when the "has value" flag can be stored in the tail padding of the storage union, 
 * allowing for the `[[no_unique_address]]` optimization to be applied to the representation of the ResultStorage. 
 * When this optimization is applicable, the `repr_` member of the ResultStorage can be marked as `[[no_unique_address]]`, 
 * which can potentially reduce the overall size of the ResultStorage by allowing the compiler to optimize away the 
 * storage for the "has value" flag when it is not needed. 
 * This template is used in the implementation of the ResultStorage to conditionally apply the `[[no_unique_address]]` 
 * attribute based on whether the "has value" flag can fit in the tail padding of the storage union.
 */

struct NEX_INTERNAL ConditionalNoUniqueAddressInvokeTag {};

// Primary template for ConditionalNoUniqueAddress
template <bool NoUniqueAddress, typename Type>
struct NEX_INTERNAL ConditionalNoUniqueAddress;

// Specialization for when NoUniqueAddress is true
template <class Type>
struct NEX_INTERNAL ConditionalNoUniqueAddress<true, Type> {
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ConditionalNoUniqueAddress(in_place_tag, Args&&... args)
        : value(NEX_FORWARD<Args>(args)...) {}

    template <class Func, class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ConditionalNoUniqueAddress(
        ConditionalNoUniqueAddressInvokeTag, Func&& func, Args&&... args)
        : value(NEX_STD invoke(NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...)) {}

    NEX_NO_UNIQUE_ADDRESS Type value;
};

// Specialization for when NoUniqueAddress is false
template <class Type>
struct NEX_INTERNAL ConditionalNoUniqueAddress<false, Type> {
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ConditionalNoUniqueAddress(in_place_tag, Args&&... args)
        : value(NEX_FORWARD<Args>(args)...) {}

    template <class Func, class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ConditionalNoUniqueAddress(
        ConditionalNoUniqueAddressInvokeTag, Func&& func, Args&&... args)
        : value(NEX_STD invoke(NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...)) {}

    Type value;
};

/**
 * @brief Storage class for Result.
 * @details
 * This class implements the internal storage mechanism for the Result class, managing the construction,
 * destruction, and access to the value and error types. It uses a union to store either the value or the error, 
 * and a boolean flag to indicate which one is currently active. 
 */
template <typename ValueType, typename ErrorType>
class NEX_INTERNAL ResultStorage {
    static_assert(ResultValueTypeEvaluator<ValueType>::value);
    static_assert(ResultErrorTypeEvaluator<ErrorType>::value);

    // The alignment of the storage is determined by the maximum alignment requirement 
    // of either the value type or the error type.
    static constexpr usize StorageAlignment = meta::MaxAlignOfV<ValueType, ErrorType>;

    // The Storage union is used to hold either the value or the error, but not both at the same time.
    union NEX_ALIGNAS(StorageAlignment) StorageUnion {
        // Conditional copy constructor that is only enabled if both ValueType and ErrorType 
        // are copy constructible and trivially copy constructible
        NEX_HIDDEN_FROM_ABI constexpr StorageUnion(const StorageUnion&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr StorageUnion(const StorageUnion&)
            requires(
                meta::IsCopyConstructibleV<ValueType> && meta::IsTriviallyCopyConstructibleV<ValueType> && 
                meta::IsCopyConstructibleV<ErrorType> && meta::IsTriviallyCopyConstructibleV<ErrorType>
            ) = default;

        // Conditional move constructor that is only enabled if both ValueType and ErrorType 
        // are move constructible and trivially move constructible
        NEX_HIDDEN_FROM_ABI constexpr StorageUnion(StorageUnion&&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr StorageUnion(StorageUnion&&)
            requires(
                meta::IsMoveConstructibleV<ValueType> && meta::IsTriviallyMoveConstructibleV<ValueType> && 
                meta::IsMoveConstructibleV<ErrorType> && meta::IsTriviallyMoveConstructibleV<ErrorType>
            ) = default;

        // Deleted assignment operators to prevent accidental copying/moving of the union, 
        // since the assignment should be managed by the Result class.
        NEX_HIDDEN_FROM_ABI constexpr StorageUnion& operator=(const StorageUnion&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr StorageUnion& operator=(StorageUnion&&) = delete;

        // Constructor for expected value
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit StorageUnion(in_place_tag, Args&&... args)
            : value_(NEX_FORWARD<Args>(args)...) {}

        // Constructor for unexpected error
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit StorageUnion(unexpected_type, Args&&... args)
            : error_(NEX_FORWARD<Args>(args)...) {}

        // Constructor for in-place construction of value using a function and arguments
        template <class Func, class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit 
        StorageUnion(ResultValueInPlaceConstructionTag, Func&& func, Args&&... args)
            : value_(NEX_STD invoke(NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...)) {}

        // Constructor for in-place construction of error using a function and arguments
        template <class Func, class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit 
        StorageUnion(ResultErrorInPlaceConstructionTag, Func&& func, Args&&... args)
            : error_(NEX_STD invoke(NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...)) {}

        // The destructor is trivial if both ValueType and ErrorType are trivially destructible
        NEX_HIDDEN_FROM_ABI constexpr ~StorageUnion()
            requires(meta::IsTriviallyDestructibleV<ValueType> && meta::IsTriviallyDestructibleV<ErrorType>) = default;

        // Non-trivial destructor if either ValueType or ErrorType is not trivially destructible. 
        // The Result class will manage the destruction of the active member.
        NEX_HIDDEN_FROM_ABI constexpr ~StorageUnion() {}

        NEX_NO_UNIQUE_ADDRESS ValueType value_;   // Success value
        NEX_NO_UNIQUE_ADDRESS ErrorType error_;   // Error value
    };

    static constexpr bool PutFlagInTail = meta::__fitsInTailPadding<StorageUnion, bool>;
    static constexpr bool AllowReusingResultTailPadding = !PutFlagInTail;

    struct Repr {
        // Prevent default construction of the Representation
        NEX_HIDDEN_FROM_ABI constexpr explicit Repr() = delete;

        // Constructor for expected value
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit Repr(in_place_tag tag, Args&&... args)
            : actualStorage_(in_place, tag, NEX_FORWARD<Args>(args)...), hasValue_(true) {}

        // Constructor for unexpected error
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit Repr(unexpected_type tag, Args&&... args)
            : actualStorage_(in_place, tag, NEX_FORWARD<Args>(args)...), hasValue_(false) {}

        // Constructor for in-place construction of value using a function and arguments
        template <class Func, class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit 
        Repr(ResultValueInPlaceConstructionTag tag, Func&& func, Args&&... args)
            : actualStorage_(in_place, tag, NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...), 
              hasValue_(true) {}

        // Constructor for in-place construction of error using a function and arguments
        template <class Func, class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit 
        Repr(ResultErrorInPlaceConstructionTag tag, Func&& func, Args&&... args)
            : actualStorage_(in_place, tag, NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...), 
              hasValue_(false) {}

        // Conditional copy constructor that is only enabled if both ValueType and ErrorType 
        // are copy constructible and trivially copy constructible
        NEX_HIDDEN_FROM_ABI constexpr Repr(const Repr&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr Repr(const Repr&)
            requires(
                meta::IsCopyConstructibleV<ValueType> && meta::IsTriviallyCopyConstructibleV<ValueType> && 
                meta::IsCopyConstructibleV<ErrorType> && meta::IsTriviallyCopyConstructibleV<ErrorType>
            ) = default;

        // Conditional move constructor that is only enabled if both ValueType and ErrorType 
        // are move constructible and trivially move constructible
        NEX_HIDDEN_FROM_ABI constexpr Repr(Repr&&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr Repr(Repr&&)
            requires(
                meta::IsMoveConstructibleV<ValueType> && meta::IsTriviallyMoveConstructibleV<ValueType> &&
                meta::IsMoveConstructibleV<ErrorType> && meta::IsTriviallyMoveConstructibleV<ErrorType>
            ) = default;

        // Deleted assignment operators to prevent accidental copying/moving of the representation, 
        // since the assignment should be managed by the Result class.
        NEX_HIDDEN_FROM_ABI constexpr Repr& operator=(const Repr&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr Repr& operator=(Repr&&) = delete;

        // The destructor is trivial if both ValueType and ErrorType are trivially destructible
        NEX_HIDDEN_FROM_ABI constexpr ~Repr()
            requires(meta::IsTriviallyDestructibleV<ValueType> && meta::IsTriviallyDestructibleV<ErrorType>) = default;

        // Non-trivial destructor if either ValueType or ErrorType is not trivially destructible
        NEX_HIDDEN_FROM_ABI constexpr ~Repr() {
            destroyStorageMember();
        }

        // Destroys the storage and ends the lifetime of the union
        NEX_HIDDEN_FROM_ABI constexpr void destroyStorage()
        requires(AllowReusingResultTailPadding &&
                (meta::IsTriviallyDestructibleV<ValueType> && meta::IsTriviallyDestructibleV<ErrorType>)) {
            // Note: Since the destructor of the union is trivial, this does nothing
            // except to end the lifetime of the union.
            NEX_STD destroy_at(&actualStorage_.value);
        }

        // Destroys the storage and ends the lifetime of the union
        NEX_HIDDEN_FROM_ABI constexpr void destroyStorage()
        requires(AllowReusingResultTailPadding &&
                (!meta::IsTriviallyDestructibleV<ValueType> || !meta::IsTriviallyDestructibleV<ErrorType>)) {
            destroyStorageMember();
            NEX_STD destroy_at(&actualStorage_.value);
        }

        // Constructs the value in the storage using perfect forwarding of arguments
        template <class... _Args>
        NEX_HIDDEN_FROM_ABI constexpr 
        void constructStorage(in_place_tag, _Args&&... __args)
        requires(AllowReusingResultTailPadding) {
            NEX_STD construct_at(&actualStorage_.value, in_place, NEX_FORWARD<_Args>(__args)...);
            hasValue_ = true;
        }

        // Constructs the error in the storage using perfect forwarding of arguments
        template <class... _Args>
        NEX_HIDDEN_FROM_ABI constexpr 
        void constructStorage(unexpected_type, _Args&&... __args)
        requires(AllowReusingResultTailPadding) {
            NEX_STD construct_at(&actualStorage_.value, unexpect, NEX_FORWARD<_Args>(__args)...);
            hasValue_ = false;
        }

    private:
        template <class, class>
        friend class ResultStorage;

        // Destroys the currently active member of the storage (either value or error) 
        // if they are not trivially destructible
        NEX_HIDDEN_FROM_ABI constexpr 
        void destroyStorageMember()
        requires(!meta::IsTriviallyDestructibleV<ValueType> || !meta::IsTriviallyDestructibleV<ErrorType>) {
            if (hasValue_) {
                NEX_STD destroy_at(NEX_ADDRESS_OF(actualStorage_.value.value_));
            } else {
                NEX_STD destroy_at(NEX_ADDRESS_OF(actualStorage_.value.error_));
            }
        }

        // Creates a StorageUnion instance from another storage, 
        // using the provided flag to determine whether to construct the value or error.
        template <class OtherStorageUnion>
        NEX_HIDDEN_FROM_ABI static constexpr 
        StorageUnion makeStorage(bool hasVal, OtherStorageUnion&& other)
        requires(AllowReusingResultTailPadding) {
            if (hasVal) {
                return StorageUnion(in_place, NEX_FORWARD<OtherStorageUnion>(other).value_);
            } else {
                return StorageUnion(unexpected, NEX_FORWARD<OtherStorageUnion>(other).error_);
            }
        }

        // The actual storage for the value or error, using conditional no unique address optimization if possible
        NEX_NO_UNIQUE_ADDRESS ConditionalNoUniqueAddress<PutFlagInTail, StorageUnion> actualStorage_;

        // Flag to indicate whether the storage currently holds a value (true) or an error (false)
        bool hasValue_ = false;
    };

    // Creates a Repr instance from another storage, 
    // using the provided flag to determine whether to construct the value or error.
    template <class OtherStorageUnion>
    NEX_HIDDEN_FROM_ABI static constexpr 
    Repr makeRepresentation(bool hasVal, OtherStorageUnion&& other)
    requires(PutFlagInTail) {
        if (hasVal) {
            return Repr(in_place, NEX_FORWARD<OtherStorageUnion>(other).value_);
        } else {
            return Repr(unexpected, NEX_FORWARD<OtherStorageUnion>(other).error_);
        }
    }

protected:
    // Constructor for ResultStorage that takes perfect forwarding of arguments to construct the value or error
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ResultStorage(Args&&... __args)
        : repr_(in_place, NEX_FORWARD<Args>(__args)...) {}

    /**
     * @note
     * In case we copy/move construct from another `Result` we need to create our `Result` so that 
     * it either has a value or not, depending on the "hasValue_" flag of the other `Result`. 
     * To do this without falling back on `std::construct_at` we rely on guaranteed copy elision 
     * using two helper functions `makeRepresentation` and `makeStorage`. There have to be two since
     * there are two data layouts with different members being `[[no_unique_address]]`. 
     * Compiler such as GCC (as of version 13) does not do guaranteed copy elision when initializing 
     * `[[no_unique_address]]` members. 
     * The two cases are:
     *
     * - `makeRepresentation`: This is used when the "has value" flag lives in the tail of the union. 
     *    In this case, the `repr_` member is _not_ `[[no_unique_address]]`.
     * - `makeStorage`: When the "hasValue_" flag does _not_ fit in the tail of the union, the `repr_` 
     *    member is `[[no_unique_address]]` and the union is not.
     *
     * This constructor "catches" the first case and leaves the second case to `StorageUnion`, 
     * its constructors and `makeStorage`.
     */
    template <class OtherStorageUnion>
    NEX_HIDDEN_FROM_ABI constexpr explicit 
    ResultStorage(bool hasVal, OtherStorageUnion&& other)
    requires(PutFlagInTail)
        : repr_(ConditionalNoUniqueAddressInvokeTag{},
                [&] { 
                    return makeRepresentation(hasVal, NEX_FORWARD<OtherStorageUnion>(other)); 
                }) {}

    // Destroy the representation
    NEX_HIDDEN_FROM_ABI constexpr void destroy() {
        if constexpr (PutFlagInTail) {
            NEX_STD destroy_at(&repr_.value);
        } else {
            repr_.value.destroyStorage();
        }
    }

    // Constructs the value or error in the representation using perfect forwarding of arguments
    template <class Tag, class... Args>
    NEX_HIDDEN_FROM_ABI constexpr void construct(Tag tag, Args&&... args) {
        if constexpr (PutFlagInTail) {
            NEX_STD construct_at(&repr_.value, tag, NEX_FORWARD<Args>(args)...);
        } else {
            repr_.value.constructStorage(tag, NEX_FORWARD<Args>(args)...);
        }
    }

    // Check if the representation currently holds a value
    NEX_HIDDEN_FROM_ABI constexpr bool hasValue() const { return repr_.value.hasValue_; }

    // Access the storage union from the representation
    NEX_HIDDEN_FROM_ABI constexpr StorageUnion& storage() { return repr_.value.actualStorage_.value; }
    NEX_HIDDEN_FROM_ABI constexpr const StorageUnion& storage() const { return repr_.value.actualStorage_.value; }

    // Access the value from the storage union
    NEX_HIDDEN_FROM_ABI constexpr ValueType& value() { return repr_.value.actualStorage_.value.value_; }
    NEX_HIDDEN_FROM_ABI constexpr const ValueType& value() const { return repr_.value.actualStorage_.value.value_; }

    // Access the error from the storage union
    NEX_HIDDEN_FROM_ABI constexpr ErrorType& error() { return repr_.value.actualStorage_.value.error_; }
    NEX_HIDDEN_FROM_ABI constexpr const ErrorType& error() const { return repr_.value.actualStorage_.value.error_; }

private:
    // The internal representation of the ResultStorage
    NEX_NO_UNIQUE_ADDRESS ConditionalNoUniqueAddress<AllowReusingResultTailPadding, Repr> repr_;
};

/**
 * @class Result
 * @brief Represents the result of an operation that can either succeed with a value or fail with an error.
 * 
 * @details
 * This template class provides a convenient way to represent the result of an operation, including whether 
 * it succeeded or failed, along with an optional value on success or an error on failure. It is designed 
 * to be used in functions that may fail and need to return detailed error information without using exceptions.
 * 
 * Result supports:
 * - Construction of successful results with a value.
 * - Construction of error results with an error.
 * - Checking if the result is successful or an error.
 * - Accessing the value or error, with safety checks that will crash if accessed incorrectly.
 * - A specialization for void return type, which only indicates success or failure without returning any value.
 * - Try-get methods that return pointers to the value or error, allowing for safe access without crashing.
 * 
 * @tparam ValueType The type of the return value on success
 * @tparam ErrorType The type of the error on failure
 * 
 * @note This class is similar in concept to std::expected (C++23) or Result types in other languages.
 * 
 * Example usage:
 * ```
 * Result<int32> divide(int32 a, int32 b) {
 *     if (b == 0) {
 *         return Result<int32>::error({
 *             ErrorCode::InvalidArgument, "Division by zero is not allowed"
 *         });
 *     }
 *     return Result<int32>::ok(a / b);
 * }
 * ```
 */
template<typename ValueType, typename ErrorType = Error>
class NEX_API NEX_NODISCARD Result {
public:
    using value_type = ValueType;
    using error_type = ErrorType;
    using expected_type = struct { value_type value; };
    using unexpected_type = struct { error_type error; };

public:
    // Create a successful result with an expected value
    template <typename AnyValue>
    static constexpr Result ok(AnyValue&& value) noexcept {
        static_assert(meta::IsSameV<AnyValue, value_type> || meta::IsConvertibleV<AnyValue, value_type>, 
            "Error: Provided value must be the same as or convertible to the Result's value type");
        return Result(expected_type { NEX_MOVE(value) });
    }

    // Create a successful result with perfect forwarding of arguments to construct the value
    template<typename... Args>
    static constexpr Result ok(Args&&... args) noexcept {
        return Result(expected_type { value_type(NEX_FORWARD<Args>(args)...) });
    }

    // Create an error result with an unexpected error
    template <typename AnyError>
    static constexpr Result error(AnyError&& error) noexcept {
        static_assert(meta::IsSameV<AnyError, error_type> || meta::IsConvertibleV<AnyError, error_type>, 
            "Error: Provided error must be the same as or convertible to the Result's error type");
        return Result(unexpected_type { NEX_MOVE(error) });
    }

    // Create an error result with perfect forwarding of arguments to construct the error
    template<typename... Args>
    static constexpr Result error(Args&&... args) noexcept {
        return Result(unexpected_type { error_type(NEX_FORWARD<Args>(args)...) });
    }

    // Check if the result is successful
    constexpr bool isSuccess() const noexcept { return hasValue_; }

    // Check if the result has a value (also means it is successful)
    constexpr bool hasValue() const noexcept { return hasValue_; }

    // Explicit conversion to bool to check if the result is successful
    constexpr explicit operator bool() const noexcept { return hasValue_; }

    // Get the success value (crash if result is an error)
    constexpr value_type& value() {
        if (!hasValue_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return storage_.value_;
    }

    // Get the success value (const, crash if result is an error)
    constexpr const value_type& value() const noexcept {
        if (!hasValue_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return storage_.value_;
    }

    // Get the success value or a default
    constexpr value_type valueOr(value_type defaultValue) const noexcept {
        if (hasValue_) {
            return storage_.value_;
        }
        return defaultValue;
    }

    // Get the error value (crash if result is successful)
    constexpr error_type& error() noexcept {
        if (hasValue_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return storage_.error_;
    }

    // Get the error value (const, crash if result is successful)
    constexpr const error_type& error() const noexcept {
        if (hasValue_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return storage_.error_;
    }

    // Try to get the success value pointer (returns a nullptr if result is an error)
    constexpr const value_type* tryValue() const noexcept {
        return hasValue_ ? &storage_.value_ : nullptr;
    }

    // Try to get the error value pointer (returns a nullptr if result is successful)
    constexpr const error_type* tryError() const noexcept {
        return hasValue_ ? nullptr : &storage_.error_;
    }

private:
    // Storage for either the success value or the error
    ResultStorage<value_type, error_type> storage_;

    // Construct a successful Result with an expected value
    constexpr Result(expected_type expected) noexcept {
        storage_.construct(in_place, NEX_MOVE(expected.value));
    }

    // Construct an error Result with an unexpected error
    constexpr Result(unexpected_type unexpected) noexcept {
        storage_.construct(in_place, NEX_MOVE(unexpected.error));
    }

    // Copy the contents of another Result object into this one
    constexpr void copyResult(const Result& other) noexcept {
        storage_.destroy();  // Destroy current contents if any
        if (other.hasValue()) {
            // Copy the success value from the other Result object
            storage_.construct(in_place, other.storage_.value_);
        } else {
            // Copy the error information from the other Result object
            storage_.construct(unexpected, other.storage_.error_);
        }
    }

    // Move the contents of another Result object into this one
    constexpr void moveResult(Result&& other) noexcept {
        storage_.destroy();  // Destroy current contents if any
        if (other.hasValue()) {
            // Move the success value from the other Result object
            storage_.construct(in_place, NEX_MOVE(other.storage_.value_));
        } else {
            // Move the error information from the other Result object
            storage_.construct(unexpected, NEX_MOVE(other.storage_.error_));
        }
    }

public:
    // Copy constructor for copying a Result object
    constexpr Result(const Result& other) noexcept {
        copyResult(other);
    }

    // Copy assignment operator for copying a Result object
    constexpr Result& operator=(const Result& other) noexcept {
        if (this != &other) {
            copyResult(other);
        }
        return *this;
    }

    // Constructor for moving a Result object
    constexpr Result(Result&& other) noexcept {
        moveResult(NEX_MOVE(other));
    }

    // Move assignment operator for moving a Result object
    constexpr Result& operator=(Result&& other) noexcept {
        if (this != &other) {
            moveResult(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor to clean up the contained value or error
    constexpr ~Result() {
        storage_.destroy();
    }
};

/**
 * @class Result<void, ErrorType>
 * @brief Specialization of Result for operations that do not return a value, only success/failure status and error.
 * 
 * @details
 * This specialization of Result is designed for functions that do not return a value on success, but still need to 
 * indicate success or failure and provide error information. It provides the same interface for checking success 
 * and accessing errors, but does not store a value on success. This is useful for operations that are primarily 
 * about side effects and do not produce a meaningful return value.
 * 
 * @tparam ErrorType The type of the error on failure
 * 
 * @note This class is similar in concept to std::expected<void, ErrorType> (C++23) or Result types in other languages.
 * 
 * Example usage:
 * ```
 * Result<void, ErrorCode> performOperation() {
 *    if (someConditionFails) {
 *       return Result<void, ErrorCode>::error(ErrorCode::OperationFailed);
 *   }
 *   return Result<void, ErrorCode>::ok();
 * }
 * ```
 */
template<typename ErrorType>
class NEX_API NEX_NODISCARD Result<void, ErrorType> {
public:
    // Create a successful result with no return value
    static constexpr Result ok() noexcept {
        return Result();
    }
    
    // Create an error result with an unexpected error
    static constexpr Result error(ErrorType error) noexcept {
        return Result(Unexpected { NEX_MOVE(error) });
    }

    // Create an error result with perfect forwarding of arguments to construct the error
    template<typename... Args>
    static constexpr Result error(Args&&... args) noexcept {
        return Result(Unexpected { ErrorType(NEX_FORWARD<Args>(args)...) });
    }

    // Check if result is successful
    constexpr bool isSuccess() const noexcept { return hasValue_; }

    // Check if result has a value (also means it is successful)
    constexpr bool hasValue() const noexcept { return hasValue_; }

    // Explicit conversion to bool to check if the result is successful
    constexpr explicit operator bool() const noexcept { return hasValue_; }

    // Get the error value (crash if result is successful)
    constexpr ErrorType& error() noexcept {
        if (hasValue_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return storage_.error_;
    }

    // Get the error value (const, crash if result is successful)
    constexpr const ErrorType& error() const noexcept {
        if (hasValue_) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return storage_.error_;
    }

    // Try to get the error value pointer (returns a nullptr if result is successful)
    constexpr const ErrorType* tryError() const noexcept {
        return hasValue_ ? nullptr : &storage_.error_;
    }

private:
    // Represents an unexpected error result
    struct Unexpected {
        ErrorType error;
    };

    // Storage for either a successful result (no value) or an error result (with error details)
    union NEX_ALIGNAS(alignof(ErrorType)) {
        nchar dummy_;       // Dummy member to allow default construction of the union; not used for actual storage
        ErrorType error_;   // Error information for failure cases; valid only if hasValue_ is false
    } storage_;

    // Flag indicating whether the result is successful (true) or an error (false)
    bool hasValue_ = false;

    // Construct a successful Result with an expected value
    constexpr Result() noexcept : hasValue_(true) {
        NEX_STD construct_at(&storage_.dummy_, nchar{});
    }

    // Construct an error Result with an unexpected error
    constexpr Result(Unexpected unexpected) noexcept : hasValue_(false) {
        NEX_STD construct_at(&storage_.error_, NEX_MOVE(unexpected.error));
    }

    // Copy the contents of another Result object into this one
    constexpr void copyResult(const Result& other) noexcept {
        hasValue_ = other.hasValue_;
        if (hasValue_) {
            // Construct the dummy member for successful result
            NEX_STD construct_at(&storage_.dummy_, nchar{});
        } else {
            // Copy the error information from the other Result object
            NEX_STD construct_at(&storage_.error_, other.storage_.error_);
        }
    }

    // Move the contents of another Result object into this one
    constexpr void moveResult(Result&& other) noexcept {
        hasValue_ = other.hasValue_;
        if (hasValue_) {
            // Construct the dummy member for successful result
            NEX_STD construct_at(&storage_.dummy_, nchar{});
        } else {
            // Move the error information from the other Result object
            NEX_STD construct_at(&storage_.error_, NEX_MOVE(other.storage_.error_));
        }
    }

    // Destroy the existing error information if this Result holds an error
    constexpr void destroy() noexcept {
        if (!hasValue_) {
            NEX_STD destroy_at(&storage_.error_);
        }
    }

public:
    // Copy constructor for copying a Result object
    constexpr Result(const Result& other) noexcept {
        copyResult(other);
    }

    // Copy assignment operator for copying a Result object
    constexpr Result& operator=(const Result& other) noexcept {
        if (this != &other) {
            destroy();
            copyResult(other);
        }
        return *this;
    }

    // Constructor for moving a Result object
    constexpr Result(Result&& other) noexcept {
        moveResult(NEX_MOVE(other));
    }

    // Move assignment operator for moving a Result object
    constexpr Result& operator=(Result&& other) noexcept {
        if (this != &other) {
            destroy();
            moveResult(NEX_MOVE(other));
        }
        return *this;
    }

    // Destructor
    constexpr ~Result() {
        destroy();
    }
};

NEX_NAMESPACE_END
