/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @concept GrowthPolicy
 * @brief   A concept that defines the requirements for a growth policy.
 *
 * @details
 * A growth policy is a strategy for determining how to increase the capacity of a dynamic buffer
 * when it needs to grow. The policy must provide a static method `nextCapacity` that takes the
 * current capacity, the required capacity, and the maximum allowed capacity, and returns the next
 * capacity to use.
 * 
 * @tparam Type The type of the growth policy.
 * 
 * Constraints:
 * 
 * @pre current > 0
 * @pre required > current
 * @pre required <= maximum
 * 
 * @post result >= required
 * @post result <= maximum
 * @post result > current
 */
template <typename Type>
concept GrowthPolicy =
    requires(usize current, usize required, usize maximum) {
        { Type::grow(current, required, maximum) } noexcept -> meta::SameAs<usize>;
    };

/**
 * @struct DoubleGrowth
 * @brief  A growth policy that doubles the capacity of a buffer when resizing.
 */
struct DoubleGrowth {
    NEX_HIDDEN_FROM_ABI static constexpr
    usize grow(usize current, usize required, usize maximum) noexcept {
        NEX_ASSERT(current > 0 && required > current && required <= maximum);
        usize newCapacity = current;
        do {
            newCapacity *= 2;
            if (newCapacity > maximum / 2) {
                newCapacity = maximum;
                break;
            }
        } while (newCapacity < required);
        return newCapacity;
    }
};

/**
 * @struct OnePointFiveGrowth
 * @brief  A growth policy that increases the capacity of a buffer by 50% (x1.5).
 */
struct OnePointFiveGrowth {
    NEX_HIDDEN_FROM_ABI static constexpr
    usize grow(usize current, usize required, usize maximum) noexcept {
        NEX_ASSERT(current > 0 && required > current && required <= maximum);
        usize newCapacity = current;
        do {
            // Increase by 50% (x1.5), but at least 1
            usize increase = meta::_maxOf(newCapacity / 2, static_cast<usize>(1));
            newCapacity = newCapacity + increase;
            if (newCapacity > maximum * 2 / 3) {
                newCapacity = maximum;
                break;
            }
        } while (newCapacity < required);
        return newCapacity;
    }
};

/**
 * @struct GoldenRatioGrowth
 * @brief  A growth policy that increases the capacity of a buffer by the golden ratio.
 */
struct GoldenRatioGrowth {
    NEX_HIDDEN_FROM_ABI static constexpr
    usize grow(usize current, usize required, usize maximum) noexcept {
        NEX_ASSERT(current > 0 && required > current && required <= maximum);
        usize newCapacity = current;
        do {
            newCapacity = newCapacity * 13 / 8;  // Increase by golden ratio (approximately 1.625)
            if (newCapacity > maximum * 8 / 13) {
                newCapacity = maximum;
                break;
            }
        } while (newCapacity < required);
        return newCapacity;
    }
};

/**
 * @struct LinearGrowth
 * @brief  A growth policy that increases the capacity of a buffer by a fixed amount (linear growth).
 * @tparam Increment The fixed amount to increase the capacity by (must be > 0).
 */
template <usize Increment = 1>
    requires (Increment > 0)
struct LinearGrowth {
    NEX_HIDDEN_FROM_ABI static constexpr
    usize grow(usize current, usize required, usize maximum) noexcept {
        NEX_ASSERT(current > 0 && required > current && required <= maximum);
        usize newCapacity = current;
        do {
            newCapacity += Increment;
            if (newCapacity > maximum - Increment) {
                newCapacity = maximum;
                break;
            }
        } while (newCapacity < required);
        return newCapacity;
    }
};

/**
 * @struct PowerOfTwoGrowth
 * @brief  A growth policy that increases the capacity of a buffer to the next power of two.
 */
struct PowerOfTwoGrowth {
    NEX_HIDDEN_FROM_ABI static constexpr
    usize grow(usize current, usize required, usize maximum) noexcept {
        NEX_ASSERT(current > 0 && required > current && required <= maximum);
        usize newCapacity = current;
        do {
            newCapacity <<= 1; // Multiply by 2
            if (newCapacity > maximum / 2) {
                newCapacity = maximum;
                break;
            }
        } while (newCapacity < required);
        return newCapacity;
    }
};

/**
 * @struct ExactGrowth
 * @brief  A growth policy that increases the capacity of a buffer to exactly the required size.
 */
struct ExactGrowth {
    NEX_HIDDEN_FROM_ABI static constexpr
    usize grow(usize current, usize required, usize maximum) noexcept {
        NEX_ASSERT(current > 0 && required > current && required <= maximum);
        if (required > maximum) {
            required = maximum;
        }
        return required;
    }
};

NEX_NAMESPACE_END
