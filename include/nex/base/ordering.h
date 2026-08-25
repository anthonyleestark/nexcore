/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/type_concepts.h"

NEX_NAMESPACE_BEGIN

/**
 * @enum OrderingResult
 * @brief Represents the result of a comparison operation.
 * 
 * @details
 * The `OrderingResult` enum class defines three possible outcomes of a comparison:
 *   - `Less`: Indicates that the first operand is less than the second operand.
 *   - `Equivalent`: Indicates that the first operand is equivalent to the second operand.
 *   - `Greater`: Indicates that the first operand is greater than the second operand.
 */
enum class OrderingResult : schar {
    Less        = -1,   // less than
    Equivalent  = 0,    // equivalent to / equal to
    Greater     = 1     // greater than
};

/**
 * @enum PartialOrderingResult
 * @brief Represents the result of a partial comparison operation.
 * 
 * @details
 * The `PartialOrderingResult` enum class extends the `OrderingResult` enum to include
 * an additional outcome for partial comparisons:
 *   - `Unordered`: Indicates that the operands are unordered or incomparable.
 */
enum class PartialOrderingResult : schar {
    Less        = static_cast<schar>(OrderingResult::Less),         // less than
    Equivalent  = static_cast<schar>(OrderingResult::Equivalent),   // equivalent to / equal to
    Greater     = static_cast<schar>(OrderingResult::Greater),      // greater than
    Unordered   = static_cast<schar>(-2)                            // unordered / incomparable
};

/**
 * @class CmpUnspecifiedParam
 * @brief Represents an unspecified parameter for comparison operations.
 * 
 * @details
 * The `CmpUnspecifiedParam` struct is used to represent an unspecified parameter in comparison operations.
 * It ensures that only a literal 0 can be used as the operand of a comparison with one of the ordering types.
 */
struct NEX_HIDDEN_FROM_ABI CmpUnspecifiedParam {
    // Only literal 0 is permitted as the comparison operand.
    // The alternative to the `NEX_CLANG_ENABLE_IF(...)` attribute would be to use the fact that a pointer
    // can be constructed from literal 0, but this conflicts with `-Wzero-as-null-pointer-constant`.
    template <class Type>
        requires meta::SameAs<Type, int32>
    NEX_HIDDEN_FROM_ABI consteval CmpUnspecifiedParam(Type zero) noexcept
        NEX_CLANG_ENABLE_IF(zero == 0,
            "Only literal 0 is allowed as the operand of a comparison with one of the ordering types")
    {
        (void)zero;
    }
};

/**
 * @class PartialOrdering
 * @brief Represents the result of a partial ordering comparison (3-way comparison)
 *        in accordance with C++20 standard requirements.
 *
 * @details
 * A partial ordering allows two values to be compared where a order relation might not exist.
 * The four possible comparison outcomes are:
 *    - `PartialOrdering::Less`
 *    - `PartialOrdering::Equivalent`
 *    - `PartialOrdering::Greater`
 *    - `PartialOrdering::Unordered` (indicates values are incomparable, e.g., NaN floating-point numbers)
 *
 * @note
 * Instances of this class are intended to be compared strictly against literal `0` using
 * relational operators (e.g., `res < 0`, `res == 0`), matching `std::partial_ordering` semantics.
 */
class PartialOrdering {
    // Constructs a PartialOrdering object with a specific internal result value.
    NEX_HIDDEN_FROM_ABI explicit constexpr PartialOrdering(PartialOrderingResult value) noexcept
        : value_(value) {}

public:
    static const PartialOrdering Less;
    static const PartialOrdering Equivalent;
    static const PartialOrdering Greater;
    static const PartialOrdering Unordered;

    // Compares two PartialOrdering instances for equality.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator==(PartialOrdering, PartialOrdering) noexcept = default;

    // Determines if the ordering represents `Equivalent` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator==(PartialOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ == PartialOrderingResult::Equivalent;
    }

    // Determines if the ordering represents `Less` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<(PartialOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ == PartialOrderingResult::Less;
    }

    // Determines if the ordering represents `Less` or `Equivalent` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<=(PartialOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ == PartialOrderingResult::Equivalent || ord.value_ == PartialOrderingResult::Less;
    }

    // Determines if the ordering represents `Greater` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>(PartialOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ == PartialOrderingResult::Greater;
    }

    // Determines if the ordering represents `Greater` or `Equivalent` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>=(PartialOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ == PartialOrderingResult::Equivalent || ord.value_ == PartialOrderingResult::Greater;
    }

    // Determines if `0` is less than the ordering result (equivalent to `ord > 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<(CmpUnspecifiedParam, PartialOrdering ord) noexcept {
        return PartialOrderingResult::Less == ord.value_;
    }
    // Determines if `0` is less than or equal to the ordering result (equivalent to `ord >= 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<=(CmpUnspecifiedParam, PartialOrdering ord) noexcept {
        return PartialOrderingResult::Equivalent == ord.value_ || PartialOrderingResult::Less == ord.value_;
    }
    // Determines if `0` is greater than the ordering result (equivalent to `ord < 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>(CmpUnspecifiedParam, PartialOrdering ord) noexcept {
        return PartialOrderingResult::Greater == ord.value_;
    }
    // Determines if `0` is greater than or equal to the ordering result (equivalent to `ord <= 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>=(CmpUnspecifiedParam, PartialOrdering ord) noexcept {
        return PartialOrderingResult::Equivalent == ord.value_ || PartialOrderingResult::Greater == ord.value_;
    }

    /**
     * @brief Performs a 3-way comparison between a PartialOrdering object and literal `0`.
     * @return The original PartialOrdering instance.
     */
    NEX_HIDDEN_FROM_ABI friend constexpr
    PartialOrdering operator<=>(PartialOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord;
    }

    /**
     * @brief Performs a 3-way comparison between literal `0` and a PartialOrdering object.
     * @return The inverted PartialOrdering instance.
     */
    NEX_HIDDEN_FROM_ABI friend constexpr
    PartialOrdering operator<=>(CmpUnspecifiedParam, PartialOrdering ord) noexcept {
        return ord < 0 ? PartialOrdering::Greater : (ord > 0 ? PartialOrdering::Less : ord);
    }

    /**
     * @brief Compares two values of potentially different types and returns a PartialOrdering result.
     * 
     * @tparam Type1 The type of the first value.
     * @tparam Type2 The type of the second value.
     * @param lhs The first value to compare.
     * @param rhs The second value to compare.
     * @return A PartialOrdering value indicating the relative order of the two values.
     * 
     * @note
     * This function uses the `<` operator to determine the ordering of the two values.
     * If the types are not comparable, it returns `PartialOrdering::Unordered`.
     */
    template <class Type1, class Type2>
    NEX_HIDDEN_FROM_ABI static constexpr PartialOrdering compare(Type1&& lhs, Type2&& rhs)
        noexcept(noexcept(lhs < rhs) && noexcept(rhs < lhs)) {
        if constexpr (LessThanComparable<Type1, Type2>) {
            return (lhs < rhs) ? PartialOrdering::Less
                               : ((rhs < lhs) ? PartialOrdering::Greater : PartialOrdering::Equivalent);
        } else {
            return PartialOrdering::Unordered;
        }
    }

private:
    PartialOrderingResult value_;
};

// Represents a result where the left operand is less than the right operand.
inline constexpr PartialOrdering PartialOrdering::Less{PartialOrderingResult::Less};
// Represents a result where the left operand is equivalent to the right operand.
inline constexpr PartialOrdering PartialOrdering::Equivalent{PartialOrderingResult::Equivalent};
// Represents a result where the left operand is greater than the right operand.
inline constexpr PartialOrdering PartialOrdering::Greater{PartialOrderingResult::Greater};
// Represents a result where the operands cannot be ordered (incomparable).
inline constexpr PartialOrdering PartialOrdering::Unordered{PartialOrderingResult::Unordered};

/**
 * @class WeakOrdering
 * @brief Represents the result of a weak ordering comparison (3-way comparison)
 *        in accordance with C++20 standard requirements.
 *
 * @details
 * A weak ordering yields strict weak ordering semantics where equivalent values are not required
 * to be indistinguishable.
 * The three possible comparison outcomes are:
 *    - `WeakOrdering::Less`
 *    - `WeakOrdering::Equivalent`
 *    - `WeakOrdering::Greater`
 *
 * @note
 * Implements implicit conversion to `PartialOrdering`. Instances are intended to be compared strictly
 * against literal `0` using relational operators, matching `std::weak_ordering` semantics.
 */
class WeakOrdering {
    using value_type NEX_NODEBUG = schar;

    // Constructs a WeakOrdering object with a specific internal result value.
    NEX_HIDDEN_FROM_ABI explicit constexpr WeakOrdering(OrderingResult value) noexcept
        : value_(static_cast<value_type>(value)) {}

public:
    static const WeakOrdering Less;
    static const WeakOrdering Equivalent;
    static const WeakOrdering Greater;

    // Implicitly converts this WeakOrdering instance to a PartialOrdering instance.
    NEX_HIDDEN_FROM_ABI constexpr operator PartialOrdering() const noexcept {
        return value_ == 0 ? PartialOrdering::Equivalent
                           : (value_ < 0 ? PartialOrdering::Less : PartialOrdering::Greater);
    }

    // Compares two WeakOrdering instances for equality.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator==(WeakOrdering, WeakOrdering) noexcept = default;

    // Determines if the ordering represents `Equivalent` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator==(WeakOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ == 0;
    }

    // Determines if the ordering represents `Less` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<(WeakOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ < 0;
    }

    // Determines if the ordering represents `Less` or `Equivalent` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<=(WeakOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ <= 0;
    }

    // Determines if the ordering represents `Greater` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>(WeakOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ > 0;
    }

    // Determines if the ordering represents `Greater` or `Equivalent` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>=(WeakOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ >= 0;
    }

    // Determines if `0` is less than the ordering result (equivalent to `ord > 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<(CmpUnspecifiedParam, WeakOrdering ord) noexcept {
        return 0 < ord.value_;
    }

    // Determines if `0` is less than or equal to the ordering result (equivalent to `ord >= 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<=(CmpUnspecifiedParam, WeakOrdering ord) noexcept {
        return 0 <= ord.value_;
    }

    // Determines if `0` is greater than the ordering result (equivalent to `ord < 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>(CmpUnspecifiedParam, WeakOrdering ord) noexcept {
        return 0 > ord.value_;
    }

    // Determines if `0` is greater than or equal to the ordering result (equivalent to `ord <= 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>=(CmpUnspecifiedParam, WeakOrdering ord) noexcept {
        return 0 >= ord.value_;
    }

    /**
     * @brief Performs a 3-way comparison between a WeakOrdering object and literal `0`.
     * @return The original WeakOrdering instance.
     */
    NEX_HIDDEN_FROM_ABI friend constexpr
    WeakOrdering operator<=>(WeakOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord;
    }

    /**
     * @brief Performs a 3-way comparison between literal `0` and a WeakOrdering object.
     * @return The inverted WeakOrdering instance.
     */
    NEX_HIDDEN_FROM_ABI friend constexpr
    WeakOrdering operator<=>(CmpUnspecifiedParam, WeakOrdering ord) noexcept {
        return ord.value_ < 0 ? WeakOrdering::Greater : (ord.value_ > 0 ? WeakOrdering::Less : ord);
    }

    /**
     * @brief Compares two values of potentially different types and returns a WeakOrdering result.
     * 
     * @tparam Type1 The type of the first value.
     * @tparam Type2 The type of the second value.
     * @param lhs The first value to compare.
     * @param rhs The second value to compare.
     * @return A WeakOrdering value indicating the relative order of the two values.
     * 
     * @note
     * This function requires that the two types are less-than comparable.
     * It uses the `<` operator to determine the ordering.
     * The function is marked `noexcept` if the comparison operations are also `noexcept`.
     */
    template <class Type1, class Type2>
        requires (LessThanComparable<Type1, Type2>)
    NEX_HIDDEN_FROM_ABI static constexpr WeakOrdering compare(Type1&& lhs, Type2&& rhs)
        noexcept(noexcept(lhs < rhs) && noexcept(rhs < lhs)) {
        return (lhs < rhs) ? WeakOrdering::Less
                           : ((rhs < lhs) ? WeakOrdering::Greater : WeakOrdering::Equivalent);
    }

private:
    value_type value_;
};

// Represents a result where the left operand is less than the right operand.
inline constexpr WeakOrdering WeakOrdering::Less(OrderingResult::Less);
// Represents a result where the left operand is equivalent to the right operand.
inline constexpr WeakOrdering WeakOrdering::Equivalent(OrderingResult::Equivalent);
// Represents a result where the left operand is greater than the right operand.
inline constexpr WeakOrdering WeakOrdering::Greater(OrderingResult::Greater);

/**
 * @class StrongOrdering
 * @brief Represents the result of a strong ordering comparison (3-way comparison)
 *        in accordance with C++20 standard requirements.
 *
 * @details 
 * A strong ordering requires strict total ordering semantics where equivalent values are completely
 * indistinguishable.
 * The four static instances represent the comparison outcomes:
 *    - `StrongOrdering::Less`
 *    - `StrongOrdering::Equal`
 *    - `StrongOrdering::Equivalent` (alias for `Equal`)
 *    - `StrongOrdering::Greater`
 *
 * @note
 * Implements implicit conversions to both `PartialOrdering` and `WeakOrdering`. Instances are intended to be
 * compared strictly against literal `0` using relational operators, matching `std::strong_ordering` semantics.
 */
class StrongOrdering {
    using value_type NEX_NODEBUG = schar;

    // Constructs a StrongOrdering object with a specific internal result value.
    NEX_HIDDEN_FROM_ABI explicit constexpr StrongOrdering(OrderingResult value) noexcept
        : value_(static_cast<value_type>(value)) {}

public:
    static const StrongOrdering Less;
    static const StrongOrdering Equal;
    static const StrongOrdering Equivalent;
    static const StrongOrdering Greater;

    // Implicitly converts this StrongOrdering instance to a PartialOrdering instance.
    NEX_HIDDEN_FROM_ABI constexpr operator PartialOrdering() const noexcept {
        return value_ == 0 ? PartialOrdering::Equivalent
                           : (value_ < 0 ? PartialOrdering::Less : PartialOrdering::Greater);
    }

    // Implicitly converts this StrongOrdering instance to a WeakOrdering instance.
    NEX_HIDDEN_FROM_ABI constexpr operator WeakOrdering() const noexcept {
        return value_ == 0 ? WeakOrdering::Equivalent
                           : (value_ < 0 ? WeakOrdering::Less : WeakOrdering::Greater);
    }

    // Compares two StrongOrdering instances for equality.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator==(StrongOrdering, StrongOrdering) noexcept = default;

    // Determines if the ordering represents `Equal` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator==(StrongOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ == 0;
    }

    // Determines if the ordering represents `Less` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<(StrongOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ < 0;
    }

    // Determines if the ordering represents `Less` or `Equal` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<=(StrongOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ <= 0;
    }

    // Determines if the ordering represents `Greater` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>(StrongOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ > 0;
    }

    // Determines if the ordering represents `Greater` or `Equal` when compared to `0`.
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>=(StrongOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord.value_ >= 0;
    }

    // Determines if `0` is less than the ordering result (equivalent to `ord > 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<(CmpUnspecifiedParam, StrongOrdering ord) noexcept {
        return 0 < ord.value_;
    }

    // Determines if `0` is less than or equal to the ordering result (equivalent to `ord >= 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator<=(CmpUnspecifiedParam, StrongOrdering ord) noexcept {
        return 0 <= ord.value_;
    }

    // Determines if `0` is greater than the ordering result (equivalent to `ord < 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>(CmpUnspecifiedParam, StrongOrdering ord) noexcept {
        return 0 > ord.value_;
    }

    // Determines if `0` is greater than or equal to the ordering result (equivalent to `ord <= 0`).
    NEX_HIDDEN_FROM_ABI friend constexpr bool operator>=(CmpUnspecifiedParam, StrongOrdering ord) noexcept {
        return 0 >= ord.value_;
    }

    /**
     * @brief Performs a 3-way comparison between a StrongOrdering object and literal `0`.
     * @return The original StrongOrdering instance.
     */
    NEX_HIDDEN_FROM_ABI friend constexpr
    StrongOrdering operator<=>(StrongOrdering ord, CmpUnspecifiedParam) noexcept {
        return ord;
    }

    /**
     * @brief Performs a 3-way comparison between literal `0` and a StrongOrdering object.
     * @return The inverted StrongOrdering instance.
     */
    NEX_HIDDEN_FROM_ABI friend constexpr
    StrongOrdering operator<=>(CmpUnspecifiedParam, StrongOrdering ord) noexcept {
        return ord.value_ < 0 ? StrongOrdering::Greater : (ord.value_ > 0 ? StrongOrdering::Less : ord);
    }

    /**
     * @brief Compares two values of potentially different types and returns a StrongOrdering result.
     * 
     * @tparam Type1 The type of the first value.
     * @tparam Type2 The type of the second value.
     * @param lhs The first value to compare.
     * @param rhs The second value to compare.
     * @return A StrongOrdering value indicating the relative order of the two values.
     * 
     * @note
     * This function requires that the two types are less-than comparable.
     * It uses the `<` operator to determine the ordering.
     * The function is marked `noexcept` if the comparison operations are also `noexcept`.
     */
    template <class Type1, class Type2>
        requires (LessThanComparable<Type1, Type2>)
    NEX_HIDDEN_FROM_ABI static constexpr StrongOrdering compare(Type1&& lhs, Type2&& rhs)
        noexcept(noexcept(lhs < rhs) && noexcept(rhs < lhs)) {
        return (lhs < rhs) ? StrongOrdering::Less
                           : ((rhs < lhs) ? StrongOrdering::Greater : StrongOrdering::Equal);
    }

private:
    value_type value_;
};

// Represents a result where the left operand is less than the right operand.
inline constexpr StrongOrdering StrongOrdering::Less(OrderingResult::Less);
// Represents a result where the left operand is equal to the right operand.
inline constexpr StrongOrdering StrongOrdering::Equal(OrderingResult::Equivalent);
// Represents a result where the left operand is equivalent to the right operand (alias of Equal).
inline constexpr StrongOrdering StrongOrdering::Equivalent(OrderingResult::Equivalent);
// Represents a result where the left operand is greater than the right operand.
inline constexpr StrongOrdering StrongOrdering::Greater(OrderingResult::Greater);

// Determines if a type is one of the comparison category types (PartialOrdering, WeakOrdering, StrongOrdering).
template <class Type>
concept ComparisonCategory =
    meta::SameAs<meta::RemoveCvrefT<Type>, PartialOrdering> ||
    meta::SameAs<meta::RemoveCvrefT<Type>, WeakOrdering> ||
    meta::SameAs<meta::RemoveCvrefT<Type>, StrongOrdering>;

NEX_NAMESPACE_END
