/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

// Using math for calculations
#ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
#endif

#include <cmath>

// Using PI constants
#ifndef M_PI
    #ifdef NEX_HAS_CXX20
        #if !defined(_NUMBERS_)
            #include <numbers>
        #endif
        constexpr double M_PI = NEX_STD numbers::pi;
        constexpr double M_PI_2 = NEX_STD numbers::pi / 2;
        constexpr double M_PI_4 = NEX_STD numbers::pi / 4;
    #else
        #define M_PI       3.14159265358979323846   // pi
        #define M_PI_2     1.57079632679489661923   // pi/2
        #define M_PI_4     0.785398163397448309616  // pi/4
    #endif
#endif

#include <numeric>
#include <utility>
#include <algorithm>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/traits.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @namespace math
 * @brief
 * Contains mathematical utilities, functions, and structures for performing various mathematical
 * operations and calculations, including geometry, vector math, and other mathematical concepts
 * that are commonly used in graphics, physics, and other computational domains.
 */
namespace math {

    // =============================================================================
    // For floating-point precision comparisons
    // =============================================================================

    // Epsilon value for floating-point comparisons
    inline constexpr double kEpsilon = 1e-9;

    // Nearly equal comparison for floating-point types with relative tolerance
    template<typename T>
    inline EnableIf<IsFloatingPointV<T>, bool>
    nearlyEqual(T a, T b, T epsilon = static_cast<T>(kEpsilon)) noexcept {
        return NEX_STD abs(a - b) <= epsilon * NEX_STD max(static_cast<T>(1), 
                NEX_STD max(NEX_STD abs(a), NEX_STD abs(b)));
    }

    // Nearly equal comparison to zero for floating-point types
    template<typename T>
    inline EnableIf<IsFloatingPointV<T>, bool>
    equalsToZero(T a, T epsilon = static_cast<T>(kEpsilon)) noexcept {
        return nearlyEqual(a, static_cast<T>(0), epsilon);
    }

    // =============================================================================
    // Fallback for non-floating-point types: exact equality
    // =============================================================================

    // For non-floating-point types, nearlyEqual is just equality
    template<typename T>
    inline EnableIf<!IsFloatingPointV<T>, bool>
    nearlyEqual(const T& a, const T& b) noexcept {
        return a == b;
    }

    // For non-floating-point types, equalsToZero is just equality to zero
    template<typename T>
    inline EnableIf<!IsFloatingPointV<T>, bool> 
    equalsToZero(const T& a) noexcept {
        return a == 0;
    }

    /**
     * @struct Coordinate2DValues
     * @brief Template base structure for 2D coordinate-based values
     * 
     * @details
     * This template struct provides a foundation for 2D coordinate structures with x and y components.
     * It supports template type conversion and provides comparison operators with floating-point
     * tolerance using nearlyEqual.
     * 
     * Coordinate2DValues supports:
     * - Construction from uniform value or separate x, y values
     * - Template type conversion between different numeric types
     * - Comparison operators (equality, less than, greater than, etc.) with floating-point tolerance
     * - Lexicographical ordering
     * - Stream output operators for NEX_STD ostream and NEX_STD wostream
     */
    template<typename T>
    struct Coordinate2DValues {
        T x = {}, y = {};
        
        // Constructions
        constexpr Coordinate2DValues() noexcept = default;
        constexpr explicit Coordinate2DValues(T uniform) noexcept : x(uniform), y(uniform) {}
        constexpr Coordinate2DValues(T xVal, T yVal) noexcept : x(xVal), y(yVal) {}

        // Template conversion
        template<typename U>
        constexpr explicit Coordinate2DValues(const Coordinate2DValues<U>& other)
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

        // Comparison operators
        bool operator==(const Coordinate2DValues& other) const noexcept {
            return nearlyEqual(x, other.x) && nearlyEqual(y, other.y);
        }
        bool operator!=(const Coordinate2DValues& other) const noexcept {
            return !(*this == other);
        }

        // Lexicographical comparison operators
        bool operator<(const Coordinate2DValues& other) const noexcept {
            return (x < other.x) || (nearlyEqual(x, other.x) && y < other.y);
        }
        bool operator<=(const Coordinate2DValues& other) const noexcept {
            return (*this < other) || (*this == other);
        }
        bool operator>(const Coordinate2DValues& other) const noexcept {
            return (x > other.x) || (nearlyEqual(x, other.x) && y > other.y);
        }
        bool operator>=(const Coordinate2DValues& other) const noexcept {
            return !(*this < other);
        }

        // Stream Operators
        friend NEX_STD ostream& operator<<(NEX_STD ostream& os, const Coordinate2DValues& coord) {
            return os << "(" << coord.x << ", " << coord.y << ")";
        }
        friend NEX_STD wostream& operator<<(NEX_STD wostream& wos, const Coordinate2DValues& coord) {
            return wos << L"(" << coord.x << L", " << coord.y << L")";
        }
    };

    /**
     * @struct Size2DValues
     * @brief Template base structure for 2D size-based values
     * 
     * @details
     * This template struct provides a foundation for 2D size structures with width and height components.
     * It supports template type conversion and provides comparison operators with floating-point
     * tolerance using nearlyEqual.
     * 
     * Size2DValues supports:
     * - Construction from uniform value or separate width, height values
     * - Template type conversion between different numeric types
     * - Comparison operators (equality, less than, greater than, etc.) with floating-point tolerance
     * - Area calculation
     * - Zero and validity checking
     * - Stream output operators for NEX_STD ostream and NEX_STD wostream
     */
    template<typename T>
    struct Size2DValues {
        T width = {}, height = {};

        // Constructors
        constexpr Size2DValues() noexcept = default;
        constexpr Size2DValues(T w, T h) noexcept : width(w), height(h) {}
        constexpr explicit Size2DValues(T uniform) noexcept : width(uniform), height(uniform) {}

        // Template conversion
        template<typename U>
        constexpr explicit Size2DValues(const Size2DValues<U>& other)
            : width(static_cast<T>(other.width)), height(static_cast<T>(other.height)) {}

        // Comparison operators
        bool operator==(const Size2DValues& other) const noexcept {
            return nearlyEqual(width, other.width) && nearlyEqual(height, other.height);
        }
        bool operator!=(const Size2DValues& other) const noexcept {
            return !(*this == other);
        }
        bool operator<(const Size2DValues& other) const noexcept {
            if (!nearlyEqual(width, other.width)) return width < other.width;
            return height < other.height;
        }
        bool operator<=(const Size2DValues& other) const noexcept {
            return (*this < other) || (*this == other);
        }
        bool operator>(const Size2DValues& other) const noexcept {
            return (width > other.width) || (nearlyEqual(width, other.width) && height > other.height);
        }
        bool operator>=(const Size2DValues& other) const noexcept {
            return !(*this < other);
        }

        // Stream Operators
        friend NEX_STD ostream& operator<<(NEX_STD ostream& os, const Size2DValues& size) {
            return os << "(" << size.width << ", " << size.height << ")";
        }
        friend NEX_STD wostream& operator<<(NEX_STD wostream& wos, const Size2DValues& size) {
            return wos << L"(" << size.width << L", " << size.height << L")";
        }
        
        // Area
        double area() const noexcept {
            return NEX_STD abs(width * height);
        }
        
        // Is zero
        bool isZero() const noexcept {
            return equalsToZero(width) && equalsToZero(height);
        }

        // Is valid
        constexpr bool isValid() const noexcept {
            return width > T{} && height > T{};
        }
    };

    /**
     * @enum Rotation
     * @brief Enumeration for rotation directions
     * 
     * @details
     * This enum class defines standard rotation directions for 2D geometry operations.
     * It provides both clockwise and counter-clockwise rotations at 90, 180, and 270 degree increments.
     * 
     * Rotation values:
     * - Clockwise_90, Clockwise_180, Clockwise_270
     * - CounterClockwise_90, CounterClockwise_180, CounterClockwise_270
     */
    enum class Rotation {
        Clockwise_90, Clockwise_180, Clockwise_270,
        CounterClockwise_90, CounterClockwise_180, CounterClockwise_270,
    };

    /**
     * @struct RectBase
     * @brief Template base structure for rectangle-based values
     * 
     * @details
     * This template struct provides a foundation for rectangle structures with x, y, width, and height components.
     * It supports template type conversion and provides comparison operators with floating-point
     * tolerance using nearlyEqual.
     * 
     * RectBase supports:
     * - Construction from x, y, width, height values
     * - Template type conversion between different numeric types
     * - Comparison operators (equality, less than, greater than, etc.) with floating-point tolerance
     * - Empty operation to reset all values to zero
     * - Zero checking
     * - Stream output operators for NEX_STD ostream and NEX_STD wostream
     */
    template<typename T>
    struct RectBase {
        T x = {}, y = {}, width = {}, height = {};

        // Constructors
        constexpr RectBase() noexcept = default;
        constexpr RectBase(T xVal, T yVal, T wVal, T hVal) noexcept : 
                            x(xVal), y(yVal), width(wVal), height(hVal) {}

        // Template conversion
        template<typename U>
        constexpr explicit RectBase(const RectBase<U>& other)
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)),
            width(static_cast<T>(other.width)), height(static_cast<T>(other.height)) {}
            
        // Comparison operators
        bool operator==(const RectBase& other) const noexcept {
            return nearlyEqual(x, other.x) &&
                nearlyEqual(y, other.y) &&
                nearlyEqual(width, other.width) &&
                nearlyEqual(height, other.height);
        }
        bool operator!=(const RectBase& other) const noexcept {
            return !(*this == other);
        }
        bool operator<(const RectBase& other) const noexcept {
            if (!nearlyEqual(x, other.x)) return x < other.x;
            if (!nearlyEqual(y, other.y)) return y < other.y;
            if (!nearlyEqual(width, other.width)) return width < other.width;
            return height < other.height;
        }
        bool operator<=(const RectBase& other) const noexcept {
            return (*this < other || *this == other);
        }
        bool operator>(const RectBase& other) const noexcept {
            return !(*this <= other);
        }
        bool operator>=(const RectBase& other) const noexcept {
            return !(*this < other);
        }

        // Stream Operators
        friend NEX_STD ostream& operator<<(NEX_STD ostream& os, const RectBase& rect) {
            return os << "(" << rect.x << ", " << rect.y << ", " << rect.width << ", " << rect.height << ")";
        }
        friend NEX_STD wostream& operator<<(NEX_STD wostream& wos, const RectBase& rect) {
            return wos << L"(" << rect.x << L", " << rect.y << L", " << rect.width << L", " << rect.height << L")";
        }

        // Empty the rect
        void empty() noexcept {
            x = y = width = height = T{};
        }

        // Is zero
        bool isZero() const noexcept {
            return equalsToZero(x) && equalsToZero(y) &&
                    equalsToZero(width) && equalsToZero(height);
        }
    };

    /**
     * @struct Edge2DValues
     * @brief Template base structure for edge-based values (left, top, right, bottom)
     * 
     * @details
     * This template struct provides a foundation for edge-based structures with left, top, right, and bottom components.
     * It supports template type conversion and provides comparison operators with floating-point
     * tolerance using nearlyEqual.
     * 
     * Edge2DValues supports:
     * - Construction from uniform value or separate left, top, right, bottom values
     * - Template type conversion between different numeric types
     * - Comparison operators (equality, less than, greater than, etc.) with floating-point tolerance
     * - Empty operation to reset all values to zero
     * - Zero checking
     */
    template<typename T>
    struct Edge2DValues {
        T left = {}, top = {}, right = {}, bottom = {};

        // Constructions
        constexpr Edge2DValues() noexcept = default;
        constexpr explicit Edge2DValues(T uniform) noexcept
            : left(uniform), top(uniform), right(uniform), bottom(uniform) {}
        constexpr Edge2DValues(T leftVal, T topVal, T rightVal, T bottomVal) noexcept
            : left(leftVal), top(topVal), right(rightVal), bottom(bottomVal) {}

        // Template conversion
        template<typename U>
        constexpr explicit Edge2DValues(const Edge2DValues<U>& other)
            : left(static_cast<T>(other.left)), top(static_cast<T>(other.top)),
                right(static_cast<T>(other.right)), bottom(static_cast<T>(other.bottom)) {}

        // Comparison operators
        bool operator==(const Edge2DValues& other) const noexcept {
            return nearlyEqual(left, other.left) &&
                nearlyEqual(top, other.top) &&
                nearlyEqual(right, other.right) &&
                nearlyEqual(bottom, other.bottom);
        }
        bool operator!=(const Edge2DValues& other) const noexcept {
            return !(*this == other);
        }
        bool operator<(const Edge2DValues& other) const noexcept {
            if (!nearlyEqual(left, other.left)) return left < other.left;
            if (!nearlyEqual(top, other.top)) return top < other.top;
            if (!nearlyEqual(right, other.right)) return right < other.right;
            return bottom < other.bottom;
        }
        bool operator<=(const Edge2DValues& other) const noexcept {
            return (*this < other || *this == other);
        }
        bool operator>(const Edge2DValues& other) const noexcept {
            return !(*this <= other);
        }
        bool operator>=(const Edge2DValues& other) const noexcept {
            return !(*this < other);
        }

        // Empty the edge values
        void empty() noexcept {
            left = top = right = bottom = T{};
        }

        // Is zero
        bool isZero() const noexcept {
            return equalsToZero(left) && equalsToZero(top) &&
                    equalsToZero(right) && equalsToZero(bottom);
        }
    };

} // namespace math

NEX_CORE_NAMESPACE_END
