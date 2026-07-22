/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/init.h"
#include "nex/base/invoke.h"
#include "nex/base/error.h"
#include "nex/base/assert_crash.h"
#include "nex/base/memory.h"

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
    void _resultInvalidAccessHandler(cstring msg) { NEX_FATAL(msg); }
    #define NEX_REPORT_RESULT_INVALID_ACCESS(msg) \
        do { \
            if NEX_CONSTEVAL_CONTEXT { \
                throw(msg); \
            } else { \
                _resultInvalidAccessHandler(msg); \
            } \
        } while (0)
#endif  // !defined(NEX_REPORT_RESULT_INVALID_ACCESS)

// Forward declaration of the Result class template
template <typename ValueType, typename ErrorType>
class Result;

// Determine if a type is a Result, returns false if the type is not a Result.
template <typename Type>
struct IsResult : meta::FalseType {};

// Determine if a type is a Result, returns true if the type is a Result
template <typename ValueType, typename ErrorType>
struct IsResult<Result<ValueType, ErrorType>> : meta::TrueType {};

// Variable template for easier usage of IsResult trait
template <typename Type>
inline constexpr bool IsResultV = IsResult<Type>::value;

/**
 * @brief Compile-time evaluator for Result value types.
 * @details
 * This trait is used to evaluate the suitability of a type as the value type in a Result. 
 * If the type does not meet these requirements, a static assertion will fail, providing 
 * a clear error message about the issue.
 */
template <typename ValueType>
struct NEX_INTERNAL ResultValueTypeEvaluator : meta::TrueType {
    static_assert(meta::IsVoidV<meta::RemoveCvT<ValueType>> || 
        meta::IsCopyConstructibleV<ValueType> || meta::IsMoveConstructibleV<ValueType>,
        "Error: Value type must be void or copy/move constructible");

    static_assert(!meta::IsReferenceV<ValueType>,   "Error: Value type cannot be a reference type");
    static_assert(!meta::IsFunctionV<ValueType>,    "Error: Value type cannot be a function type");
    static_assert(!meta::IsArrayV<ValueType>,       "Error: Value type cannot be an array type");
    static_assert(!meta::IsSameV<meta::RemoveCvT<ValueType>, monostate>,    "Error: Value type cannot be monostate");
    static_assert(!meta::IsSameV<meta::RemoveCvT<ValueType>, in_place_tag>, "Error: Value type cannot be in_place_tag");
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

    static_assert(!meta::IsReferenceV<ErrorType>,   "Error: Error type cannot be a reference type");
    static_assert(!meta::IsFunctionV<ErrorType>,    "Error: Error type cannot be a function type");
    static_assert(!meta::IsArrayV<ErrorType>,       "Error: Error type cannot be an array type");
    static_assert(!meta::IsVoidV<meta::RemoveCvT<ErrorType>>,               "Error: Error type cannot be void");
    static_assert(!meta::IsSameV<meta::RemoveCvT<ErrorType>, monostate>,    "Error: Error type cannot be monostate");
    static_assert(!meta::IsSameV<meta::RemoveCvT<ErrorType>, in_place_tag>, "Error: Error type cannot be in_place_tag");
};

/**
 * @brief Internal tags for Result construction.
 * @details
 * These tags are used internally by the ResultBase to differentiate between different construction scenarios,
 * such as in-place construction of the value or error, and not intended for public use.
 */
struct NEX_INTERNAL ResultValueInPlaceConstructionTag {};
struct NEX_INTERNAL ResultErrorInPlaceConstructionTag {};

/**
 * @brief Tag type for conditional [[no_unique_address]] optimization invocation.
 */
struct NEX_INTERNAL ConditionalNoUniqueAddressInvokeTag {};

/**
 * @brief Conditional wrapper for applying [[no_unique_address]] optimization based on a boolean condition.
 * @details
 * This class template implements an object with [[no_unique_address]] conditionally applied to it based on 
 * the value of NoUniqueAddr. 
 * A member of this class must always have [[no_unique_address]] applied to it. Otherwise, the `[[no_unique_address]]` 
 * in the `true` case would not have any effect. In the `false` case, the member is not `[[no_unique_address]]`, so 
 * nullifies the effects of the "outer" `[[no_unique_address]]` regarding data layout.
 */
template <bool NoUniqueAddr, typename Type>
struct NEX_INTERNAL ConditionalNoUniqueAddress;

template <class Type>
struct NEX_INTERNAL ConditionalNoUniqueAddress<true, Type> {
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ConditionalNoUniqueAddress(in_place_tag, Args&&... args)
        : value(NEX_FORWARD<Args>(args)...) {}

    template <class Func, class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ConditionalNoUniqueAddress(
        ConditionalNoUniqueAddressInvokeTag, Func&& func, Args&&... args)
        : value(invoke(NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...)) {}

    NEX_NO_UNIQUE_ADDRESS Type value;
};

template <class Type>
struct NEX_INTERNAL ConditionalNoUniqueAddress<false, Type> {
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ConditionalNoUniqueAddress(in_place_tag, Args&&... args)
        : value(NEX_FORWARD<Args>(args)...) {}

    template <class Func, class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ConditionalNoUniqueAddress(
        ConditionalNoUniqueAddressInvokeTag, Func&& func, Args&&... args)
        : value(invoke(NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...)) {}

    Type value;
};

/**
 * @brief Base class for Result.
 * @details
 * This class implements the internal storage mechanism for the Result class, managing the construction,
 * destruction, and access to the value and error types. It provides the necessary infrastructure for 
 * the Result class to function correctly, including handling the active member of the union and optimizing 
 * memory layout based on the characteristics of the value and error types.
 */
template <typename ValueType, typename ErrorType>
class NEX_INTERNAL ResultBase {
    static_assert(ResultValueTypeEvaluator<ValueType>::value);
    static_assert(ResultErrorTypeEvaluator<ErrorType>::value);

    // The StoredValueType type is determined based on whether the ValueType is void or not.
    // If ValueType is void, StoredValueType is set to monostate, which works as a dummy placeholder type.
    using StoredValueType = meta::ConditionalT<meta::IsVoidV<ValueType>, monostate, ValueType>;

    // The alignment of the storage is determined by the maximum alignment requirement 
    // of either the value type or the error type.
    static constexpr usize StorageAlignment = meta::MaxAlignOfV<StoredValueType, ErrorType>;

    // The ResultStorage union is used to hold either the value or the error, but not both at the same time.
    union NEX_ALIGNAS(StorageAlignment) ResultStorage {
        // Conditional copy constructor that is only enabled if both StoredValueType and ErrorType 
        // are copy constructible and trivially copy constructible
        NEX_HIDDEN_FROM_ABI constexpr ResultStorage(const ResultStorage&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr ResultStorage(const ResultStorage&)
            requires(
                meta::IsCopyConstructibleV<StoredValueType> && meta::IsTriviallyCopyConstructibleV<StoredValueType> && 
                meta::IsCopyConstructibleV<ErrorType> && meta::IsTriviallyCopyConstructibleV<ErrorType>
            ) = default;

        // Conditional move constructor that is only enabled if both StoredValueType and ErrorType 
        // are move constructible and trivially move constructible
        NEX_HIDDEN_FROM_ABI constexpr ResultStorage(ResultStorage&&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr ResultStorage(ResultStorage&&)
            requires(
                meta::IsMoveConstructibleV<StoredValueType> && meta::IsTriviallyMoveConstructibleV<StoredValueType> && 
                meta::IsMoveConstructibleV<ErrorType> && meta::IsTriviallyMoveConstructibleV<ErrorType>
            ) = default;

        // Deleted assignment operators to prevent accidental copying/moving of the union, 
        // since the assignment should be managed by the Result class.
        NEX_HIDDEN_FROM_ABI constexpr ResultStorage& operator=(const ResultStorage&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr ResultStorage& operator=(ResultStorage&&) = delete;

        // Constructor for expected value
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit ResultStorage(in_place_tag, Args&&... args)
            : value_(NEX_FORWARD<Args>(args)...) {}

        // Constructor for unexpected error
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit ResultStorage(unexpect_type, Args&&... args)
            : error_(NEX_FORWARD<Args>(args)...) {}

        // Constructor for in-place construction of value using a function and arguments
        template <class Func, class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit 
        ResultStorage(ResultValueInPlaceConstructionTag, Func&& func, Args&&... args)
            : value_(invoke(NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...)) {}

        // Constructor for in-place construction of error using a function and arguments
        template <class Func, class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit 
        ResultStorage(ResultErrorInPlaceConstructionTag, Func&& func, Args&&... args)
            : error_(invoke(NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...)) {}

        // The destructor is trivial if both StoredValueType and ErrorType are trivially destructible
        NEX_HIDDEN_FROM_ABI constexpr ~ResultStorage()
            requires(
                meta::IsTriviallyDestructibleV<StoredValueType> && meta::IsTriviallyDestructibleV<ErrorType>
            ) = default;

        // Non-trivial destructor if either StoredValueType or ErrorType is not trivially destructible. 
        // The Result class will manage the destruction of the active member.
        NEX_HIDDEN_FROM_ABI constexpr ~ResultStorage() {}

        NEX_NO_UNIQUE_ADDRESS StoredValueType  value_;   // Success value
        NEX_NO_UNIQUE_ADDRESS ErrorType     error_;   // Error value
    };

    static constexpr bool PutFlagInTail = meta::_fitsInTailPadding<ResultStorage, bool>;
    static constexpr bool AllowReusingResultTailPadding = !PutFlagInTail;

    struct Repr {
        // Prevent default construction of the Representation
        NEX_HIDDEN_FROM_ABI constexpr explicit Repr() = delete;

        // Constructor for expected value
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit Repr(in_place_tag tag, Args&&... args)
            : storage_(in_place, tag, NEX_FORWARD<Args>(args)...), hasValue_(true) {}

        // Constructor for unexpected error
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit Repr(unexpect_type tag, Args&&... args)
            : storage_(in_place, tag, NEX_FORWARD<Args>(args)...), hasValue_(false) {}

        // Constructor for in-place construction of value using a function and arguments
        template <class Func, class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit 
        Repr(ResultValueInPlaceConstructionTag tag, Func&& func, Args&&... args)
            : storage_(in_place, tag, NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...), 
              hasValue_(true) {}

        // Constructor for in-place construction of error using a function and arguments
        template <class Func, class... Args>
        NEX_HIDDEN_FROM_ABI constexpr explicit 
        Repr(ResultErrorInPlaceConstructionTag tag, Func&& func, Args&&... args)
            : storage_(in_place, tag, NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...), 
              hasValue_(false) {}

        // Conditional copy constructor that is only enabled if both StoredValueType and ErrorType 
        // are copy constructible and trivially copy constructible
        NEX_HIDDEN_FROM_ABI constexpr Repr(const Repr&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr Repr(const Repr&)
            requires(
                meta::IsCopyConstructibleV<StoredValueType> && meta::IsTriviallyCopyConstructibleV<StoredValueType> && 
                meta::IsCopyConstructibleV<ErrorType> && meta::IsTriviallyCopyConstructibleV<ErrorType>
            ) = default;

        // Conditional move constructor that is only enabled if both StoredValueType and ErrorType 
        // are move constructible and trivially move constructible
        NEX_HIDDEN_FROM_ABI constexpr Repr(Repr&&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr Repr(Repr&&)
            requires(
                meta::IsMoveConstructibleV<StoredValueType> && meta::IsTriviallyMoveConstructibleV<StoredValueType> &&
                meta::IsMoveConstructibleV<ErrorType> && meta::IsTriviallyMoveConstructibleV<ErrorType>
            ) = default;

        // Deleted assignment operators to prevent accidental copying/moving of the representation, 
        // since the assignment should be managed by the Result class.
        NEX_HIDDEN_FROM_ABI constexpr Repr& operator=(const Repr&) = delete;
        NEX_HIDDEN_FROM_ABI constexpr Repr& operator=(Repr&&) = delete;

        // The destructor is trivial if both StoredValueType and ErrorType are trivially destructible
        NEX_HIDDEN_FROM_ABI constexpr ~Repr()
            requires(
                meta::IsTriviallyDestructibleV<StoredValueType> && meta::IsTriviallyDestructibleV<ErrorType>
            ) = default;

        // Non-trivial destructor if either StoredValueType or ErrorType is not trivially destructible
        NEX_HIDDEN_FROM_ABI constexpr ~Repr() {
            destroyStorageMember();
        }

        // Destroys the storage and ends the lifetime of the union
        NEX_HIDDEN_FROM_ABI constexpr void destroyStorage()
        requires(AllowReusingResultTailPadding &&
                (meta::IsTriviallyDestructibleV<StoredValueType> && meta::IsTriviallyDestructibleV<ErrorType>)) {
            // Note: Since the destructor of the union is trivial, this does nothing
            // except to end the lifetime of the union.
            NEX_DESTROY_AT(&storage_.value);
        }

        // Destroys the storage and ends the lifetime of the union
        NEX_HIDDEN_FROM_ABI constexpr void destroyStorage()
        requires(AllowReusingResultTailPadding &&
                (!meta::IsTriviallyDestructibleV<StoredValueType> || !meta::IsTriviallyDestructibleV<ErrorType>)) {
            destroyStorageMember();
            NEX_DESTROY_AT(&storage_.value);
        }

        // Constructs the value in the storage using perfect forwarding of arguments
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr 
        void constructStorage(in_place_tag, Args&&... args)
        requires(AllowReusingResultTailPadding) {
            NEX_CONSTRUCT_AT(&storage_.value, in_place, NEX_FORWARD<Args>(args)...);
            hasValue_ = true;
        }

        // Constructs the error in the storage using perfect forwarding of arguments
        template <class... Args>
        NEX_HIDDEN_FROM_ABI constexpr 
        void constructStorage(unexpect_type, Args&&... args)
        requires(AllowReusingResultTailPadding) {
            NEX_CONSTRUCT_AT(&storage_.value, unexpect, NEX_FORWARD<Args>(args)...);
            hasValue_ = false;
        }

    private:
        template <class, class>
        friend class ResultBase;

        // Destroys the currently active member of the storage (either value or error) 
        // if they are not trivially destructible
        NEX_HIDDEN_FROM_ABI constexpr 
        void destroyStorageMember()
        requires(!meta::IsTriviallyDestructibleV<StoredValueType> || !meta::IsTriviallyDestructibleV<ErrorType>) {
            if (hasValue_) {
                NEX_DESTROY_AT(NEX_ADDRESS_OF(storage_.value.value_));
            } else {
                NEX_DESTROY_AT(NEX_ADDRESS_OF(storage_.value.error_));
            }
        }

        // Creates a Storage instance from another storage, 
        // using the provided flag to determine whether to construct the value or error.
        template <class OtherStorage>
        NEX_HIDDEN_FROM_ABI static constexpr 
        Storage makeStorage(bool hasVal, OtherStorage&& other)
        requires(AllowReusingResultTailPadding) {
            if (hasVal) {
                return ResultStorage(in_place, NEX_FORWARD<OtherStorage>(other).value_);
            } else {
                return ResultStorage(unexpect, NEX_FORWARD<OtherStorage>(other).error_);
            }
        }

        // The actual storage for the value or error, using conditional no unique address optimization if possible
        NEX_NO_UNIQUE_ADDRESS ConditionalNoUniqueAddress<PutFlagInTail, ResultStorage> storage_;

        // Flag to indicate whether the storage currently holds a value (true) or an error (false)
        bool hasValue_ = false;
    };

    // Creates a Repr instance from another storage, 
    // using the provided flag to determine whether to construct the value or error.
    template <class OtherStorage>
    NEX_HIDDEN_FROM_ABI static constexpr 
    Repr makeRepr(bool hasVal, OtherStorage&& other)
    requires(PutFlagInTail) {
        if (hasVal) {
            return Repr(in_place, NEX_FORWARD<OtherStorage>(other).value_);
        } else {
            return Repr(unexpect, NEX_FORWARD<OtherStorage>(other).error_);
        }
    }

protected:
    // Constructor for ResultBase that takes perfect forwarding of arguments to construct the value or error
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit ResultBase(Args&&... args)
        : repr_(in_place, NEX_FORWARD<Args>(args)...) {}

    /**
     * @note
     * In case we copy/move construct from another `Result` we need to create our `Result` so that 
     * it either has a value or not, depending on the "hasValue_" flag of the other `Result`. 
     * To do this without falling back on `construct_at` we rely on guaranteed copy elision 
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
    ResultBase(bool hasVal, OtherStorageUnion&& other)
    requires(PutFlagInTail)
        : repr_(ConditionalNoUniqueAddressInvokeTag{},
            [&] {
                return makeRepr(hasVal, NEX_FORWARD<OtherStorageUnion>(other)); 
            }) {}

    // Destroy the representation
    NEX_HIDDEN_FROM_ABI constexpr void destroyImpl() {
        if constexpr (PutFlagInTail) {
            NEX_DESTROY_AT(&repr_.value);
        } else {
            repr_.value.destroyStorage();
        }
    }

    // Constructs the value or error in the representation using perfect forwarding of arguments
    template <class Tag, class... Args>
    NEX_HIDDEN_FROM_ABI constexpr void constructImpl(Tag tag, Args&&... args) {
        if constexpr (PutFlagInTail) {
            NEX_CONSTRUCT_AT(&repr_.value, tag, NEX_FORWARD<Args>(args)...);
        } else {
            repr_.value.constructStorage(tag, NEX_FORWARD<Args>(args)...);
        }
    }

    // Check if the representation currently holds a value
    NEX_HIDDEN_FROM_ABI constexpr bool hasValueImpl() const { return repr_.value.hasValue_; }

    // Access the storage union from the representation
    NEX_HIDDEN_FROM_ABI constexpr ResultStorage& storageImpl() { return repr_.value.storage_.value; }
    NEX_HIDDEN_FROM_ABI constexpr const ResultStorage& storageImpl() const { return repr_.value.storage_.value; }

    // Access the value from the storage union
    NEX_HIDDEN_FROM_ABI constexpr StoredValueType& valueImpl() { return repr_.value.storage_.value.value_; }
    NEX_HIDDEN_FROM_ABI constexpr const StoredValueType& valueImpl() const { return repr_.value.storage_.value.value_; }

    // Access the error from the storage union
    NEX_HIDDEN_FROM_ABI constexpr ErrorType& errorImpl() { return repr_.value.storage_.value.error_; }
    NEX_HIDDEN_FROM_ABI constexpr const ErrorType& errorImpl() const { return repr_.value.storage_.value.error_; }

private:
    // The internal representation of the ResultBase
    NEX_NO_UNIQUE_ADDRESS ConditionalNoUniqueAddress<AllowReusingResultTailPadding, Repr> repr_;
};

/**
 * @struct Ok
 * @brief Represents a successful result with a value.
 * @details
 * This struct is used internally by the Result class to represent a successful outcome of an operation,
 * encapsulating the value returned by the operation. It ensures that the value type meets the necessary
 * requirements for use in a Result, such as being copy or move constructible.
 * @tparam ValueType The type of the value to be encapsulated in the Ok result struct.
 */
template <class ValueType>
struct NEX_INTERNAL Ok {
    static_assert(ResultValueTypeEvaluator<ValueType>::value);
    using value_type = ValueType;
    value_type value;
};

/**
 * @struct Ok<void>
 * @brief Specialization of Ok result struct for void type.
 * @details
 * This specialization is used when the Result represents a successful outcome without any associated value.
 * It allows the Result class to indicate success without needing to store any data, simplifying the interface 
 * for operations that do not return a value.
 */
template <>
struct NEX_INTERNAL Ok<void> {
    using value_type = void;
};

/**
 * @brief Creates a successful result with the given value.
 * @tparam ValueType The type of the value to be encapsulated in the Ok result struct.
 * @param value The value to be encapsulated in the Ok result struct.
 * @return An Ok result struct containing the provided value.
 * @note This function is used to create a successful result that can be returned from a function,
 *       indicating that the operation was successful and providing the resulting value.
 */
template <class ValueType>
NEX_NODISCARD NEX_API constexpr auto ok(ValueType&& value) noexcept {
    using value_type = meta::RemoveCvrefT<ValueType>;
    return Ok<value_type>{NEX_FORWARD<ValueType>(value)};  // forward original type
}

/**
 * @brief Creates a successful result without any associated value (void specialization).
 * @return An Ok<void> indicating a successful outcome without any value.
 * @note This function is used to create a successful result for operations that do not return a value,
 *       allowing the Result class to indicate success without needing to store any data.
 */
template <class = void>
NEX_NODISCARD NEX_API constexpr auto ok() noexcept -> Ok<void> {
    return {};
}

/**
 * @brief Creates a successful result with the given value, allowing for perfect forwarding of arguments.
 * @tparam ValueType The type of the value to be encapsulated in the Ok result struct.
 * @tparam Args The types of the arguments to be forwarded to construct the value.
 * @param args The arguments to be forwarded to construct the value.
 * @return An Ok result struct containing the constructed value.
 * @note This function is used to create a successful result that can be returned from a function,
 *       indicating that the operation was successful and providing the resulting value.
 *       It allows for perfect forwarding of arguments to construct the value in place.
 */
template <class ValueType, class... Args>
    requires(!meta::IsVoidV<ValueType> && meta::ConstructibleFrom<ValueType, Args...>)
NEX_NODISCARD NEX_API constexpr auto makeOkResult(Args&&... args) noexcept -> Ok<ValueType> {
    return Ok<ValueType>{ValueType(NEX_FORWARD<Args>(args)...)};
}

/**
 * @struct Err
 * @brief Represents an error result with an error value.
 * @details
 * This struct is used internally by the Result class to represent a failed outcome of an operation,
 * encapsulating the error information. It ensures that the error type meets the necessary requirements
 * for use in a Result, such as being copy or move constructible.
 * @tparam ErrorType The type of the error to be encapsulated in the Err result struct.
 */
template <class ErrorType>
struct NEX_INTERNAL Err {
    static_assert(ResultErrorTypeEvaluator<ErrorType>::value);
    using error_type = ErrorType;
    error_type error;
};

/**
 * @brief Creates an error result with the given error value.
 * @tparam ErrorType The type of the error to be encapsulated in the Err result struct.
 * @param error The error value to be encapsulated in the Err result struct.
 * @return An Err result struct containing the provided error value.
 * @note This function is used to create an error result that can be returned from a function,
 *       indicating that the operation failed and providing the associated error information.
 */
template <class ErrorType>
NEX_NODISCARD NEX_API constexpr auto error(ErrorType&& error) noexcept {
    using error_type = meta::RemoveCvrefT<ErrorType>;
    return Err<error_type>{NEX_FORWARD<ErrorType>(error)};  // forward original type
}

/**
 * @brief Creates an error result with the given error value, allowing for perfect forwarding of arguments.
 * @tparam Args The types of the arguments to be forwarded to construct the error value.
 * @param args The arguments to be forwarded to construct the error value.
 * @return An Err result struct containing the constructed error value.
 * @note This function is used to create an error result that can be returned from a function,
 *       indicating that the operation failed and providing the associated error information.
 *       It allows for perfect forwarding of arguments to construct the error value in place.
 */
template <class... Args>
    requires(meta::ConstructibleFrom<Error, Args...>)
NEX_NODISCARD NEX_API constexpr auto error(Args&&... args) noexcept -> Err<Error> {
    return Err<Error>{Error(NEX_FORWARD<Args>(args)...)};
}

/**
 * @brief Creates an error result with the given error value, allowing for perfect forwarding of arguments.
 * @tparam ErrorType The type of the error to be encapsulated in the Err result struct.
 * @tparam Args The types of the arguments to be forwarded to construct the error value.
 * @param args The arguments to be forwarded to construct the error value.
 * @return An Err result struct containing the constructed error value.
 * @note This function is used to create an error result that can be returned from a function,
 *       indicating that the operation failed and providing the associated error information.
 *       It allows for perfect forwarding of arguments to construct the error value in place.
 */
template <class ErrorType, class... Args>
    requires(meta::ConstructibleFrom<ErrorType, Args...>)
NEX_NODISCARD NEX_API constexpr auto makeErrorResult(Args&&... args) noexcept -> Err<ErrorType> {
    return Err<ErrorType>{ErrorType(NEX_FORWARD<Args>(args)...)};
}

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
 *         return error(
 *             ErrorCode::InvalidArgument, "Division by zero is not allowed"
 *         );
 *     }
 *     return ok(a / b);
 * }
 * ```
 */
template<typename ValueType, typename ErrorType = Error>
class NEX_API NEX_NODISCARD Result : private ResultBase<ValueType, ErrorType> {
private:
    // Allow private access between Result classes with different template parameters, 
    // enabling conversions and assignments between them.
    template <class OtherValue, class OtherError>
    friend class Result;

    using base NEX_NODEBUG = ResultBase<ValueType, ErrorType>;

public:
    using value_type = ValueType;
    using error_type = ErrorType;

    template <class OtherValue>
    using rebind = Result<OtherValue, error_type>;

    // Type trait to determine if the Result is trivially relocatable,
    // which would be true if both the value and error types are trivially copyable.
    using TriviallyRelocatable NEX_NODEBUG = 
        meta::ConditionalT<
            meta::IsTriviallyCopyableV<value_type> && meta::IsTriviallyCopyableV<error_type>,
            Result, void>;

    // Type trait to determine if the Result is replaceable, 
    // which would be true if both the value and error types are replaceable.
    using Replaceable NEX_NODEBUG = 
        meta::ConditionalT<
            meta::IsReplaceableV<value_type> && meta::IsReplaceableV<error_type>,
            Result, void>;

private:
    // Default constructor creates a successful Result with a default-constructed value
    constexpr Result() noexcept(meta::IsNothrowDefaultConstructibleV<value_type>)
        requires meta::IsDefaultConstructibleV<value_type>
        : base(in_place) {}

    // Constructor for creating a successful Result with an in-place constructed value
    template <class Func, class... Args>
    constexpr explicit Result(
        ResultValueInPlaceConstructionTag tag, Func&& func, Args&&... args)
        : base(tag, NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...) {}

    // Constructor for creating an error Result with an in-place constructed error
    template <class Func, class... Args>
    constexpr explicit Result(
        ResultErrorInPlaceConstructionTag tag, Func&& func, Args&&... args)
        : base(tag, NEX_FORWARD<Func>(func), NEX_FORWARD<Args>(args)...) {}

public:
    // Check if the result is successful
    constexpr bool isOk() const noexcept { return this->hasValueImpl(); }

    // Check if the result has a value (also means it is successful)
    template <typename Type = value_type> 
    constexpr bool hasValue() const noexcept 
    requires (!meta::IsVoidV<Type>) { return this->hasValueImpl(); }

    // Explicit conversion to bool to check if the result is successful
    constexpr explicit operator bool() const noexcept { return this->hasValueImpl(); }

    // Get the success value (crash if result is an error)
    template <typename Type = value_type> 
    constexpr value_type& value() & noexcept
        requires (!meta::IsVoidV<value_type>) {
        static_assert(meta::IsCopyConstructibleV<error_type>, "Error: Error type has to be copy constructible");
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return NEX_ADDRESS_OF(this->valueImpl());
    }

    // Get the success value (read-only; crash if result is an error)
    template <typename Type = value_type> 
    constexpr const value_type& value() const& noexcept 
        requires (!meta::IsVoidV<value_type>) {
        static_assert(meta::IsCopyConstructibleV<error_type>, "Error: Error type has to be copy constructible");
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return NEX_ADDRESS_OF(this->valueImpl());
    }

    // Get the success value (move; crash if result is an error)
    template <typename Type = value_type> 
    constexpr value_type&& value() && noexcept 
        requires (!meta::IsVoidV<value_type>) {
        static_assert(meta::IsCopyConstructibleV<error_type> && 
            meta::IsConstructibleV<error_type, decltype(NEX_MOVE(error()))>,
            "Error: Error type has to be both copy constructible and constructible from decltype(NEX_MOVE(error()))");
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return NEX_MOVE(this->valueImpl());
    }

    // Get the success value (move; read-only; crash if result is an error)
    template <typename Type = value_type> 
    constexpr const value_type&& value() const&& noexcept 
        requires (!meta::IsVoidV<value_type>) {
        static_assert(meta::IsCopyConstructibleV<error_type> && 
            meta::IsConstructibleV<error_type, decltype(NEX_MOVE(error()))>,
            "Error: Error type has to be both copy constructible and constructible from decltype(NEX_MOVE(error()))");
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return NEX_MOVE(this->valueImpl());
    }

    // Access the success value through pointer semantics (read-only; crash if result is an error)
    template <typename Type = value_type> 
    constexpr const value_type* operator->() const noexcept 
        requires (!meta::IsVoidV<value_type>) {
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return NEX_ADDRESS_OF(this->valueImpl());
    }

    // Access the success value through pointer semantics (crash if result is an error)
    template <typename Type = value_type> 
    constexpr value_type* operator->() noexcept 
        requires (!meta::IsVoidV<value_type>) {
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return NEX_ADDRESS_OF(this->valueImpl());
    }

    // Get the success value through dereference semantics (read-only; crash if result is an error)
    template <typename Type = value_type> 
    constexpr const value_type& operator*() const& noexcept 
        requires (!meta::IsVoidV<value_type>) {
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return this->valueImpl();
    }

    // Get the success value through dereference semantics (crash if result is an error)
    template <typename Type = value_type> 
    constexpr value_type& operator*() & noexcept 
        requires (!meta::IsVoidV<value_type>) {
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return this->valueImpl();
    }

    // Get the success value through dereference semantics (move; read-only; crash if result is an error)
    template <typename Type = value_type> 
    constexpr const value_type&& operator*() const&& noexcept 
        requires (!meta::IsVoidV<value_type>) {
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return NEX_MOVE(this->valueImpl());
    }

    // Get the success value through dereference semantics (move; crash if result is an error)
    template <typename Type = value_type> 
    constexpr value_type&& operator*() && noexcept 
        requires (!meta::IsVoidV<value_type>) {
        if (!this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access value of error Result");
        }
        return NEX_MOVE(this->valueImpl());
    }

    // Get the success value or return a default if the result is an error
    template <typename DefaultValueType>
        requires (!meta::IsVoidV<value_type>
            && (meta::IsSameV<DefaultValueType, value_type> || meta::IsConvertibleV<DefaultValueType, value_type>))
    constexpr value_type valueOr(DefaultValueType&& defaultValue) const& noexcept {
        static_assert(meta::IsCopyConstructibleV<value_type>, "Error: Value type has to be copy constructible");
        return hasValue() ? value() : NEX_FORWARD<DefaultValueType>(defaultValue);
    }

    // Get the success value or return a default if the result is an error (move version)
    template <typename DefaultValueType>
        requires (!meta::IsVoidV<value_type>
            && (meta::IsSameV<DefaultValueType, value_type> || meta::IsConvertibleV<DefaultValueType, value_type>))
    constexpr value_type valueOr(DefaultValueType&& defaultValue) && noexcept {
        static_assert(meta::IsMoveConstructibleV<value_type>, "Error: Value type has to be move constructible");
        return hasValue() ? NEX_MOVE(value()) : NEX_FORWARD<DefaultValueType>(defaultValue);
    }

    // Get the error value (crash if result is successful)
    constexpr error_type& error() & noexcept {
        if (this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return NEX_ADDRESS_OF(this->errorImpl());
    }

    // Get the error value (read-only; crash if result is successful)
    constexpr const error_type& error() const& noexcept {
        if (this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return NEX_ADDRESS_OF(this->errorImpl());
    }

    // Get the error value (move; crash if result is successful)
    constexpr error_type&& error() && noexcept {
        if (this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return NEX_MOVE(this->errorImpl());
    }

    // Get the error value (move; read-only; crash if result is successful)
    constexpr const error_type&& error() const&& noexcept {
        if (this->hasValueImpl()) {
            NEX_REPORT_RESULT_INVALID_ACCESS(
                "Error: Attempted to access error of success Result");
        }
        return NEX_MOVE(this->errorImpl());
    }

    // Get the error value or return a default if the result is successful
    template <class DefaultErrorType = error_type>
        requires(meta::IsSameV<DefaultErrorType, error_type> || meta::IsConvertibleV<DefaultErrorType, error_type>)
    constexpr error_type errorOr(DefaultErrorType&& defaultError) const& {
        static_assert(meta::IsCopyConstructibleV<error_type>, "Error: Error type has to be copy constructible");
        return hasValue() ? NEX_FORWARD<DefaultErrorType>(defaultError) : error();
    }

    // Get the error value or return a default if the result is successful (move version)
    template <class DefaultErrorType = error_type>
        requires(meta::IsSameV<DefaultErrorType, error_type> || meta::IsConvertibleV<DefaultErrorType, error_type>)
    constexpr error_type errorOr(DefaultErrorType&& defaultError) && {
        static_assert(meta::IsMoveConstructibleV<error_type>, "Error: Error type has to be move constructible");
        return hasValue() ? NEX_FORWARD<DefaultErrorType>(defaultError) : NEX_MOVE(error());
    }

    // Try to get the success value pointer (returns a nullptr if result is an error)
    template <typename Type = value_type>
    constexpr const value_type* tryValue() const noexcept 
    requires (!meta::IsVoidV<Type>) {
        return this->hasValueImpl() ? NEX_ADDRESS_OF(this->valueImpl()) : nullptr;
    }

    // Try to get the error value pointer (returns a nullptr if result is successful)
    constexpr const error_type* tryError() const noexcept {
        return this->hasValueImpl() ? nullptr : NEX_ADDRESS_OF(this->errorImpl());
    }

    // Invoke a function with the success value if the result is successful, 
    // otherwise return an error result
    template <class Func>
        requires (meta::IsConstructibleV<error_type, error_type&>)
    constexpr auto andThen(Func&& func) & {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, value_type&>>;
        static_assert(IsResultV<ReturnType>, "The result of func(value()) must be a specialization of Result");
        static_assert(meta::IsSameV<typename ReturnType::error_type, error_type>,
            "The result of func(value()) must have the same error_type as this Result");
        if (isOk()) {
            if constexpr (meta::IsVoidV<value_type>) {
                return invoke(NEX_FORWARD<Func>(func));
            } else {
                return invoke(NEX_FORWARD<Func>(func), this->valueImpl());
            }
        } else {
            return ReturnType(unexpect, error());
        }
    }

    // Invoke a function with the success value if the result is successful, 
    // otherwise return an error result (const version)
    template <class Func>
        requires (meta::IsConstructibleV<error_type, const error_type&>)
    constexpr auto andThen(Func&& func) const& {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, const value_type&>>;
        static_assert(IsResultV<ReturnType>, "The result of func(value()) must be a specialization of Result");
        static_assert(meta::IsSameV<typename ReturnType::error_type, error_type>,
            "The result of func(value()) must have the same error_type as this Result");
        if (isOk()) {
            if constexpr (meta::IsVoidV<value_type>) {
                return invoke(NEX_FORWARD<Func>(func));
            } else {
                return invoke(NEX_FORWARD<Func>(func), this->valueImpl());
            }
        } else {
            return ReturnType(unexpect, error());
        }
    }

    // Invoke a function with the success value if the result is successful, 
    // otherwise return an error result (rvalue version)
    template <class Func>
        requires (meta::IsConstructibleV<error_type, error_type&&>)
    constexpr auto andThen(Func&& func) && {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, value_type&&>>;
        static_assert(IsResultV<ReturnType>, "The result of func(NEX_MOVE(value())) must be a specialization of Result");
        static_assert(meta::IsSameV<typename ReturnType::error_type, error_type>,
            "The result of func(NEX_MOVE(value())) must have the same error_type as this Result");
        if (isOk()) {
            if constexpr (meta::IsVoidV<value_type>) {
                return invoke(NEX_FORWARD<Func>(func));
            } else {
                return invoke(NEX_FORWARD<Func>(func), NEX_MOVE(this->valueImpl()));
            }
        } else {
            return ReturnType(unexpect, NEX_MOVE(error()));
        }
    }

    // Invoke a function with the success value if the result is successful, 
    // otherwise return an error result (const rvalue version)
    template <class Func>
        requires (meta::IsConstructibleV<error_type, const error_type&&>)
    constexpr auto andThen(Func&& func) const&& {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, const value_type&&>>;
        static_assert(IsResultV<ReturnType>, "The result of func(NEX_MOVE(value())) must be a specialization of Result");
        static_assert(meta::IsSameV<typename ReturnType::error_type, error_type>,
            "The result of func(NEX_MOVE(value())) must have the same error_type as this Result");
        if (isOk()) {
            if constexpr (meta::IsVoidV<value_type>) {
                return invoke(NEX_FORWARD<Func>(func));
            } else {
                return invoke(NEX_FORWARD<Func>(func), NEX_MOVE(this->valueImpl()));
            }
        } else {
            return ReturnType(unexpect, NEX_MOVE(error()));
        }
    }

    // Invoke a function with the error value if the result is an error, 
    // otherwise return a successful result
    template <class Func>
        requires (meta::IsConstructibleV<value_type, value_type&>)
    constexpr auto orElse(Func&& func) & {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, error_type&>>;
        static_assert(IsResultV<ReturnType>, "The result of func(error()) must be a specialization of Result");
        static_assert(meta::IsSameV<typename ReturnType::value_type, value_type>,
            "The result of func(error()) must have the same value_type as this expected");
        if (isOk()) {
            if constexpr (meta::IsVoidV<value_type>) {
                return ReturnType(in_place);
            } else {
                return ReturnType(in_place, this->valueImpl());
            }
        } else {
            return invoke(NEX_FORWARD<Func>(func), error());
        }
    }

    // Invoke a function with the error value if the result is an error, 
    // otherwise return a successful result (const version)
    template <class Func>
        requires (meta::IsConstructibleV<value_type, const value_type&>)
    constexpr auto orElse(Func&& func) const& {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, const error_type&>>;
        static_assert(IsResultV<ReturnType>, "The result of func(error()) must be a specialization of Result");
        static_assert(meta::IsSameV<typename ReturnType::value_type, value_type>,
            "The result of func(error()) must have the same value_type as this expected");
        if (isOk()) {
            if constexpr (meta::IsVoidV<value_type>) {
                return ReturnType(in_place);
            } else {
                return ReturnType(in_place, this->valueImpl());
            }
        } else {
            return invoke(NEX_FORWARD<Func>(func), error());
        }
    }

    // Invoke a function with the error value if the result is an error, 
    // otherwise return a successful result (rvalue version)
    template <class Func>
        requires (meta::IsConstructibleV<value_type, value_type&&>)
    constexpr auto orElse(Func&& func) && {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, error_type&&>>;
        static_assert(IsResultV<ReturnType>, "The result of func(NEX_MOVE(error())) must be a specialization of Result");
        static_assert(meta::IsSameV<typename ReturnType::value_type, value_type>,
            "The result of func(NEX_MOVE(error())) must have the same value_type as this Result");
        if (isOk()) {
            if constexpr (meta::IsVoidV<value_type>) {
                return ReturnType(in_place);
            } else {
                return ReturnType(in_place, NEX_MOVE(this->valueImpl()));
            }
        } else {
            return invoke(NEX_FORWARD<Func>(func), NEX_MOVE(error()));
        }
    }

    // Invoke a function with the error value if the result is an error, 
    // otherwise return a successful result (const rvalue version)
    template <class Func>
        requires (meta::IsConstructibleV<value_type, const value_type&&>)
    constexpr auto orElse(Func&& func) const&& {
        using ReturnType = meta::RemoveCvrefT<InvokeResultT<Func, const error_type&&>>;
        static_assert(IsResultV<ReturnType>, "The result of func(NEX_MOVE(error())) must be a specialization of Result");
        static_assert(meta::IsSameV<typename ReturnType::value_type, value_type>,
            "The result of func(NEX_MOVE(error())) must have the same value_type as this Result");
        if (isOk()) {
            if constexpr (meta::IsVoidV<value_type>) {
                return ReturnType(in_place);
            } else {
                return ReturnType(in_place, NEX_MOVE(this->valueImpl()));
            }
        } else {
            return invoke(NEX_FORWARD<Func>(func), NEX_MOVE(error()));
        }
    }

public:
    // Delete default copy constructor to prevent copying of Result objects by default
    constexpr Result(const Result&) = delete;

    // Default copy constructor if both ValueType and ErrorType are trivially copy constructible
    constexpr Result(const Result&)
        requires(meta::IsCopyConstructibleV<value_type> && meta::IsCopyConstructibleV<error_type> && 
                 meta::IsTriviallyCopyConstructibleV<value_type> && meta::IsTriviallyCopyConstructibleV<error_type>)
        = default;

    // Custom copy constructor to handle non-trivial copy construction of ValueType or ErrorType
    constexpr Result(const Result& other) 
        noexcept(meta::IsNothrowCopyConstructibleV<value_type> && meta::IsNothrowCopyConstructibleV<error_type>)
        requires(meta::IsCopyConstructibleV<value_type> && meta::IsCopyConstructibleV<error_type> &&
                !(meta::IsTriviallyCopyConstructibleV<value_type> && meta::IsTriviallyCopyConstructibleV<error_type>))
        : base(other.hasValue(), other.storageImpl()) {}

    // Allow copy construction from another Result with different value and error types,
    // as long as the value and error types are copy constructible and convertible to the current types
    template <typename OtherValue, typename OtherError>
        requires(meta::IsCopyConstructibleV<value_type> && meta::IsCopyConstructibleV<error_type>)
    constexpr explicit(
        !meta::IsConvertibleV<const OtherValue&, value_type> || !meta::IsConvertibleV<const OtherError&, error_type>)
    Result(const Result<OtherValue, OtherError>& other)
        noexcept(meta::IsNothrowConstructibleV<value_type, const OtherValue&> && 
                 meta::IsNothrowConstructibleV<error_type, const OtherError&>)
        : base(other.hasValue(), other.storageImpl()) {}

    // Default move constructor if both ValueType and ErrorType are trivially move constructible
    constexpr Result(Result&&)
        requires(meta::IsMoveConstructibleV<value_type> && meta::IsMoveConstructibleV<error_type> && 
                 meta::IsTriviallyMoveConstructibleV<value_type> && meta::IsTriviallyMoveConstructibleV<error_type>)
        = default;

    // Custom move constructor to handle non-trivial move construction of ValueType or ErrorType
    constexpr Result(Result&& other) 
        noexcept(meta::IsNothrowMoveConstructibleV<value_type> && meta::IsNothrowMoveConstructibleV<error_type>)
        requires(meta::IsMoveConstructibleV<value_type> && meta::IsMoveConstructibleV<error_type> &&
                !(meta::IsTriviallyMoveConstructibleV<value_type> && meta::IsTriviallyMoveConstructibleV<error_type>))
        : base(other.hasValue(), NEX_MOVE(other.storageImpl())) {}

    // Allow move construction from another Result with different value and error types,
    // as long as the value and error types are move constructible and convertible to the current types
    template <class OtherValue, class OtherError>
        requires(meta::IsMoveConstructibleV<value_type> && meta::IsMoveConstructibleV<error_type>)
    constexpr explicit(
        !meta::IsConvertibleV<const OtherValue&, value_type> || !meta::IsConvertibleV<const OtherError&, error_type>)
    Result(Result<OtherValue, OtherError>&& other) 
        noexcept(meta::IsNothrowConstructibleV<value_type, const OtherValue&> && 
                 meta::IsNothrowConstructibleV<error_type, const OtherError&>)
        : base(other.hasValue(), NEX_MOVE(other.storageImpl())) {}

    // Allow move construction and binding from an Ok result struct with a different value type,
    // as long as the value type is move constructible and convertible to the current value type
    template <class OtherValue>
        requires(!meta::IsVoidV<value_type> &&
            meta::IsMoveConstructibleV<value_type> && meta::IsMoveConstructibleV<error_type>)
    constexpr explicit(!meta::IsConvertibleV<const OtherValue&, value_type>)
    Result(Ok<OtherValue>&& okResult)
        noexcept(meta::IsNothrowConstructibleV<value_type, const OtherValue&>)
        : base(in_place, NEX_MOVE(okResult.value)) {}

    // Allow construction and binding from an Ok<void> result when the current value type is void,
    // as long as the error type is copy constructible
    template<class = void>
        requires(meta::IsVoidV<value_type> && meta::IsCopyConstructibleV<error_type>)
    constexpr Result(const Ok<void>&) noexcept : base(in_place) {}

    // Allow move construction and binding from an Err result struct with a different error type,
    // as long as the error type is move constructible and convertible to the current error type
    template <class OtherError>
        requires((meta::IsVoidV<value_type> || meta::IsMoveConstructibleV<value_type>) &&
            meta::IsMoveConstructibleV<error_type>)
    constexpr explicit(!meta::IsConvertibleV<const OtherError&, error_type>)
    Result(Err<OtherError>&& errResult)
        noexcept(meta::IsNothrowConstructibleV<error_type, const OtherError&>)
        : base(unexpect, NEX_MOVE(errResult.error)) {}

    // Default destructor
    constexpr ~Result() = default;

private:
    // Constructor for creating a successful Result 
    // with perfect forwarding of arguments to construct the expected value
    template <class... Args>
        requires meta::IsConstructibleV<value_type, Args...>
    constexpr explicit Result(in_place_tag, Args&&... args) 
        noexcept(meta::IsNothrowConstructibleV<value_type, Args...>)
        : base(in_place, NEX_FORWARD<Args>(args)...) {}

    // Constructor for creating a successful Result with an initializer list 
    // and perfect forwarding of additional arguments to construct the expected value
    template <class OtherValue, class... Args>
        requires meta::IsConstructibleV<value_type, InitList<OtherValue>&, Args... >
    NEX_HIDDEN_FROM_ABI constexpr explicit Result(in_place_tag, InitList<OtherValue> ilist, Args&&... args) 
        noexcept(meta::IsNothrowConstructibleV<value_type, InitList<OtherValue>&, Args...>)
        : base(in_place, ilist, NEX_FORWARD<Args>(args)...) {}

    // Constructor for creating an error Result 
    // with perfect forwarding of arguments to construct the unexpected error
    template <class... Args>
        requires meta::IsConstructibleV<error_type, Args...>
    constexpr explicit Result(unexpect_type, Args&&... args) 
        noexcept(meta::IsNothrowConstructibleV<error_type, Args...>)
        : base(unexpect, NEX_FORWARD<Args>(args)...) {}

    // Constructor for creating an error Result with an initializer list
    // and perfect forwarding of additional arguments to construct the unexpected error
    template <class OtherError, class... Args>
        requires meta::IsConstructibleV< error_type, InitList<OtherError>&, Args... >
    NEX_HIDDEN_FROM_ABI constexpr explicit Result(unexpect_type, InitList<OtherError> ilist, Args&&... args) noexcept(
        meta::IsNothrowConstructibleV<error_type, InitList<OtherError>&, Args...>) // strengthened
        : base(unexpect, ilist, NEX_FORWARD<Args>(args)...) {}

public:
    // Equality comparison operator for Result objects with the compatible value and error types
    template <class OtherValue, class OtherError>
    NEX_HIDDEN_FROM_ABI friend constexpr 
    bool operator==(const Result& lhs, const Result<OtherValue, OtherError>& rhs)
        requires (!meta::IsVoidV<OtherValue>)
            && requires {
                { *lhs == *rhs } -> meta::ConvertibleTo<bool>;
                { lhs.error() == rhs.error() } -> meta::ConvertibleTo<bool>;
            } 
    {
        if (lhs.hasValue() != rhs.hasValue()) {
            return false;
        } else {
            if (lhs.hasValue()) {
                return lhs.value() == rhs.value();
            } else {
                return lhs.error() == rhs.error();
            }
        }
    }

    // Equality comparison operator for a Result object with another type,
    // as long as the other type is not a Result and the value can be compared to the other type
    template <class OtherType>
    NEX_HIDDEN_FROM_ABI friend constexpr 
    bool operator==(const Result& lhs, const OtherType& rhs)
        requires (!IsResultV<OtherType>)
            && requires {
                { *lhs == rhs } -> meta::ConvertibleTo<bool>;
            }
    {
        return lhs.hasValue() && static_cast<bool>(lhs.value() == rhs);
    }

    // Equality comparison operator for void Result objects,
    // which only checks if both are errors and compares their error values
    template <class OtherValue, class OtherError>
    NEX_HIDDEN_FROM_ABI friend constexpr 
    bool operator==(const Result& lhs, const Result<OtherValue, OtherError>& rhs)
        requires (meta::IsVoidV<value_type> && meta::IsVoidV<OtherValue>)
            && requires {
                { lhs.error() == rhs.error() } -> meta::ConvertibleTo<bool>;
            }
    {
        if (lhs.isOk() != rhs.isOk()) {
            return false;
        } else {
            if (lhs.isOk()) {
                return false;
            } else {
                return static_cast<bool>(lhs.error() == rhs.error());
            }
        }
    }

    // Equality comparison operator for a void Result object with another type, 
    // as long as the other type is not a Result, and the error can be compared to the other type
    template <class OtherType>
    NEX_HIDDEN_FROM_ABI friend constexpr 
    bool operator==(const Result& lhs, const OtherType& rhs)
        requires (!IsResultV<OtherType> 
            && (meta::IsVoidV<value_type> && !meta::IsVoidV<OtherType>)
            && (meta::IsSameV<OtherType, error_type> || meta::IsConvertibleV<OtherType, error_type>))
            && requires {
                { lhs.error() == rhs } -> meta::ConvertibleTo<bool>;
            }
    {
        return !lhs.isOk() && static_cast<bool>(lhs.error() == rhs);
    }
};

NEX_NAMESPACE_END
