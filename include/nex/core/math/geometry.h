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

#include <iostream>
#include <numeric>
#include <utility>
#include <algorithm>
#include <stdexcept>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/traits.h"
#include "nex/base/assert_crash.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @namespace math_utils
 * @brief Utility functions for mathematical operations
 */
namespace math_utils {

    // For floating-point precision comparisons
    inline constexpr double kEpsilon = 1e-9;
    template<typename T>
    inline EnableIf<IsFloatingPointV<T>, bool>
    nearlyEqual(T a, T b, T epsilon = static_cast<T>(kEpsilon)) noexcept {
        return NEX_STD abs(a - b) <= epsilon * NEX_STD max(static_cast<T>(1), 
                NEX_STD max(NEX_STD abs(a), NEX_STD abs(b)));
    }
    template<typename T>
    inline EnableIf<IsFloatingPointV<T>, bool>
    equalsToZero(T a, T epsilon = static_cast<T>(kEpsilon)) noexcept {
        return nearlyEqual(a, static_cast<T>(0), epsilon);
    }

    // Fallback: exact equal for non-floating types
    template<typename T>
    inline EnableIf<!IsFloatingPointV<T>, bool>
    nearlyEqual(const T& a, const T& b) noexcept {
        return a == b;
    }
    template<typename T>
    inline EnableIf<!IsFloatingPointV<T>, bool> 
    equalsToZero(const T& a) noexcept {
        return a == 0;
    }
} // namespace math_utils 

/**
 * @struct Coordinate2DValues
 * @brief Template base structure for 2D coordinate-based values
 * 
 * This template struct provides a foundation for 2D coordinate structures with x and y components.
 * It supports template type conversion and provides comparison operators with floating-point
 * tolerance using math_utils::nearlyEqual.
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
        using math_utils::nearlyEqual;
        return nearlyEqual(x, other.x) &&
               nearlyEqual(y, other.y);
    }
    bool operator!=(const Coordinate2DValues& other) const noexcept {
        return !(*this == other);
    }

    // Lexicographical comparison operators
    bool operator<(const Coordinate2DValues& other) const noexcept {
        using math_utils::nearlyEqual;
        return (x < other.x) || (nearlyEqual(x, other.x) && y < other.y);
    }
    bool operator<=(const Coordinate2DValues& other) const noexcept {
        return (*this < other) || (*this == other);
    }
    bool operator>(const Coordinate2DValues& other) const noexcept {
        using math_utils::nearlyEqual;
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
 * This template struct provides a foundation for 2D size structures with width and height components.
 * It supports template type conversion and provides comparison operators with floating-point
 * tolerance using math_utils::nearlyEqual.
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
        using math_utils::nearlyEqual;
        return nearlyEqual(width, other.width) && nearlyEqual(height, other.height);
    }
    bool operator!=(const Size2DValues& other) const noexcept {
        return !(*this == other);
    }
    bool operator<(const Size2DValues& other) const noexcept {
        using math_utils::nearlyEqual;
        if (!nearlyEqual(width, other.width)) return width < other.width;
        return height < other.height;
    }
    bool operator<=(const Size2DValues& other) const noexcept {
        return (*this < other) || (*this == other);
    }
    bool operator>(const Size2DValues& other) const noexcept {
        using math_utils::nearlyEqual;
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
        using math_utils::equalsToZero;
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
 * This template struct provides a foundation for rectangle structures with x, y, width, and height components.
 * It supports template type conversion and provides comparison operators with floating-point
 * tolerance using math_utils::nearlyEqual.
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
        using math_utils::nearlyEqual;
        return nearlyEqual(x, other.x) &&
               nearlyEqual(y, other.y) &&
               nearlyEqual(width, other.width) &&
               nearlyEqual(height, other.height);
    }
    bool operator!=(const RectBase& other) const noexcept {
        return !(*this == other);
    }
    bool operator<(const RectBase& other) const noexcept {
        using math_utils::nearlyEqual;
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
        using math_utils::equalsToZero;
        return equalsToZero(x) && equalsToZero(y) &&
                equalsToZero(width) && equalsToZero(height);
    }
};

/**
 * @struct Edge2DValues
 * @brief Template base structure for edge-based values (left, top, right, bottom)
 * 
 * This template struct provides a foundation for edge-based structures with left, top, right, and bottom components.
 * It supports template type conversion and provides comparison operators with floating-point
 * tolerance using math_utils::nearlyEqual.
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
        using math_utils::nearlyEqual;
        return nearlyEqual(left, other.left) &&
               nearlyEqual(top, other.top) &&
               nearlyEqual(right, other.right) &&
               nearlyEqual(bottom, other.bottom);
    }
    bool operator!=(const Edge2DValues& other) const noexcept {
        return !(*this == other);
    }
    bool operator<(const Edge2DValues& other) const noexcept {
        using math_utils::nearlyEqual;
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
        using math_utils::equalsToZero;
        return equalsToZero(left) && equalsToZero(top) &&
                equalsToZero(right) && equalsToZero(bottom);
    }
};

/**
 * @struct Point
 * @brief Represents a point or 2D coordinate in 2D geometry
 * 
 * This struct represents a point in 2D space with x and y coordinates. It inherits from
 * Coordinate2DValues<float> and provides geometric operations for point calculations.
 * 
 * Point supports:
 * - Construction from x, y coordinates
 * - Scalar multiplication and division operations
 * - Geometric operations (distance, magnitude, dot product, cross product, angle calculation)
 * - Vector operations (normalization, midpoint calculation)
 * - Manhattan distance calculation
 * - Note: Addition and subtraction with other Points are intentionally disabled
 *   (use Vector2D for vector operations instead)
 */
struct Point : public Coordinate2DValues<float>
{
public:
    // Construction
    using Coordinate2DValues::Coordinate2DValues;

public:
    // Arithmetic Operators
    constexpr Point operator+(const Point& other) const noexcept = delete;
    constexpr Point operator-(const Point& other) const noexcept = delete;
    constexpr Point operator*(double scalar) const noexcept {
        return Point(x * scalar, y * scalar);
    }
    Point operator/(double scalar) const {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) return Point(x / scalar, y / scalar);
        NEX_ASSERT_MSG(false, "Division by zero");
    }

    // Compound Assignment Operators
    Point& operator+=(const Point& other) noexcept = delete;
    Point& operator-=(const Point& other) noexcept = delete;
    Point& operator*=(double scalar) noexcept {
        x *= scalar; y *= scalar;
        return *this;
    }
    Point& operator/=(double scalar) {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) { x /= scalar; y /= scalar; return *this; }
        NEX_ASSERT_MSG(false, "Division by zero");
    }

// Geometric/Vector Math Operations
public:
    // Distance between points
    double distanceTo(const Point& other) const noexcept {
        double dx = x - other.x; double dy = y - other.y;
        return NEX_STD sqrt(dx * dx + dy * dy);
    }

    // length / magnitude (treats the point as a vector from the origin (0, 0))
    double magnitude() const noexcept {
        return NEX_STD sqrt(x * x + y * y);
    }

    // Dot products
    constexpr double dot(const Point& other) const noexcept {
        return x * other.x + y * other.y;
    }

    // Cross product (2D)
    // Used for determining orientation, area of parallelogram, etc.
    constexpr double cross(const Point& other) const noexcept {
        return x * other.y - y * other.x;
    }

    // Angle between two vectors
    double angleWith(const Point& other) const noexcept {
        double dotProd = this->dot(other);
        double magnitude1 = this->magnitude(); double magnitude2 = other.magnitude();
        using math_utils::equalsToZero;
        if (equalsToZero(magnitude1) || equalsToZero(magnitude2)) {
            NEX_ASSERT_MSG(false, "Cannot compute angle with zero-length vector");
        }
        double magnitudes = magnitude1 * magnitude2;
        return NEX_STD acos(NEX_STD clamp((dotProd / magnitudes), -1.0, 1.0)); // in radians
    }

// Utility Operations
public:
    // Normalization (unit vector)
    Point normalize() const noexcept {
        double magitude = this->magnitude();
        return (magitude == 0) ? Point(0, 0) : Point(x / magitude, y / magitude);
    }

    // Midpoint between two points
    Point midpoint(const Point& other) const noexcept {
        return Point((x + other.x) / 2, (y + other.y) / 2);
    }

    // Manhattan distance
    double manhattanDistanceTo(const Point& other) const noexcept {
        return NEX_STD abs(x - other.x) + NEX_STD abs(y - other.y);
    }
};

/**
 * @struct Vector2D
 * @brief Represents a 2D vector or line segment
 * 
 * This struct represents a 2D vector with x and y components. It inherits from
 * Coordinate2DValues<double> and provides comprehensive vector operations.
 * 
 * Vector2D supports:
 * - Construction from x, y components or from two Points (from, to)
 * - Arithmetic operations (addition, subtraction, scalar multiplication/division)
 * - Vector operations (length, squared length, normalization)
 * - Geometric operations (dot product, cross product, angle calculation)
 * - Zero vector checking
 * - Conversion to/from Point
 * - Commonly used as Offset2D, Velocity2D, and Accelaration2D type aliases
 */
struct Vector2D : public Coordinate2DValues<double>
{
public:
    // Construction
    using Coordinate2DValues::Coordinate2DValues;
    Vector2D(const Point& from, const Point& to) noexcept 
        : Vector2D(to.x - from.x, to.y - from.y) {}

public:
    // Access data
    Point getPoint() const noexcept {
        return Point(x, y);
    }
    void setPoint(const Point& point) noexcept {
        x = point.x; y = point.y;
    }

public:
    // Arithmetic Operators
    constexpr Vector2D operator+(const Vector2D& other) const noexcept {
        return Vector2D(x + other.x, y + other.y);
    }
    constexpr Vector2D operator-(const Vector2D& other) const noexcept {
        return Vector2D(x - other.x, y - other.y);
    }
    constexpr Vector2D operator*(double scalar) const noexcept {
        return Vector2D(x * scalar, y * scalar);
    }
    Vector2D operator/(double scalar) const {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) return Vector2D(x / scalar, y / scalar);
        NEX_ASSERT_MSG(false, "Division by zero");
    }

    // Compound Assignment Operators
    Vector2D& operator+=(const Vector2D& other) noexcept {
        if (this != &other) { x += other.x; y += other.y; }
        return *this;
    }
    Vector2D& operator-=(const Vector2D& other) noexcept {
        if (this != &other) { x -= other.x; y -= other.y; }
        return *this;
    }
    Vector2D& operator*=(double scalar) noexcept {
        x *= scalar; y *= scalar;
        return *this;
    }
    Vector2D& operator/=(double scalar) {
        using math_utils::equalsToZero;
        if (equalsToZero(scalar)) {
            NEX_ASSERT_MSG(false, "Division by zero");
        }
        x /= scalar; y /= scalar;
        return *this;
    }

public:
    // magnitude (length)
    double length() const noexcept {
        return NEX_STD sqrt(x * x + y * y);
    }

    // Squared length (no sqrt)
    constexpr double lengthSquared() const noexcept {
        return x * x + y * y;
    }

    // Normalized vector (unit length)
    Vector2D normalize() const {
        double len = length();
        using math_utils::equalsToZero;
        if (!equalsToZero(len)) return Vector2D(x / len, y / len);
        NEX_ASSERT_MSG(false, "Cannot normalize zero-length vector");
    }

    // Dot product
    constexpr double dot(const Vector2D& other) const noexcept {
        return x * other.x + y * other.y;
    }

    // Cross product (2D scalar cross)
    constexpr double cross(const Vector2D& other) const noexcept {
        return x * other.y - y * other.x;
    }

    // Angle between vectors (in radians)
    double angleTo(const Vector2D& other) const {
        double dotProd = this->dot(other);
        double length1 = this->length(); double length2 = other.length();
        using math_utils::equalsToZero;
        if (equalsToZero(length1) || equalsToZero(length2)) {
            NEX_ASSERT_MSG(false, "Cannot compute angle with zero-length vector");
        }
        double cosTheta = dotProd / (length1 * length2);
        return NEX_STD acos(NEX_STD clamp(cosTheta, -1.0, 1.0));
    }

    // Is zero
    bool isZero() const noexcept {
        using math_utils::equalsToZero;
        return equalsToZero(x) && equalsToZero(y);
    }
};

// Point & vector calculations
inline Point operator+(const Point& point, const Vector2D& vector) noexcept {
    return Point(point.x + vector.x, point.y + vector.y);
}
inline Point operator-(const Point& point, const Vector2D& vector) noexcept {
    return Point(point.x - vector.x, point.y - vector.y);
}
inline Vector2D operator-(const Point& a, const Point& b) noexcept {
    return Vector2D(a.x - b.x, a.y - b.y);
}

// Represents an 2D offset values.
// Commonly used for transforming and moving coordinates.
using Offset2D = Vector2D;

// Represents a velocity and an accelaration value in 2D geometry.
// Commonly used in animation, gesture processing and physics-like simulation.
using Velocity2D = Vector2D;
using Accelaration2D = Vector2D;

/**
 * @struct Size
 * @brief Represents a geometric size in 2D geometry
 * 
 * This struct represents a 2D size with width and height components. It inherits from
 * Size2DValues<double> and provides size manipulation operations.
 * 
 * Size supports:
 * - Construction from width, height values
 * - Arithmetic operations with other Size objects (addition, subtraction)
 * - Scalar operations (multiplication, division)
 * - Normalization (ensures positive width and height)
 * - Conversion to Vector2D
 * - Also available as Dimensions type alias
 */
struct Size : public Size2DValues<double>
{
public:
    // Construction
    using Size2DValues::Size2DValues;

public:
    // Arithmetic with Size
    constexpr Size operator+(const Size& other) const noexcept {
        return Size(width + other.width, height + other.height);
    }
    constexpr Size& operator+=(const Size& other) noexcept {
        width += other.width; height += other.height;
        return *this;
    }
    constexpr Size operator-(const Size& other) const noexcept {
        return Size(width - other.width, height - other.height);
    }
    constexpr Size& operator-=(const Size& other) noexcept {
        width -= other.width; height -= other.height;
        return *this;
    }

    // Scalar operations
    constexpr Size operator*(double scalar) const noexcept {
        return Size(width * scalar, height * scalar);
    }
    constexpr Size& operator*=(double scalar) noexcept {
        width *= scalar; height *= scalar;
        return *this;
    }
    Size operator/(double scalar) const {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) return Size(width / scalar, height / scalar);
        NEX_ASSERT_MSG(false, "Division by zero");
    }
    Size& operator/=(double scalar) {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) { width /= scalar; height /= scalar; return *this; }
        NEX_ASSERT_MSG(false, "Division by zero");
    }

public:
    // Normalize size (positive width and height)
    Size normalize() const noexcept {
        return Size(NEX_STD abs(width), NEX_STD abs(height));
    }
    Size& normalize() noexcept {
        width = NEX_STD abs(width); height = NEX_STD abs(height);
        return *this;
    }

    // Conversion to Vector2D
    constexpr Vector2D toVector() const noexcept {
        return Vector2D(width, height);
    }
};

// Represents dimensions of an object.
using Dimensions = Size;

/**
 * @struct Resolution
 * @brief Represents screen or display resolution dimensions
 * 
 * This struct represents resolution dimensions with width and height components. It inherits from
 * Size2DValues<double> and provides resolution-specific operations.
 * 
 * Resolution supports:
 * - Construction from width, height values
 * - Arithmetic operations with other Resolution objects (addition, subtraction)
 * - Scalar operations (multiplication, division)
 * - Normalization (ensures positive width and height)
 * - Aspect ratio calculation
 * - Simplified aspect ratio calculation (returns simplified fraction as pair)
 */
struct Resolution : public Size2DValues<double>
{
public:
    // Construction
    using Size2DValues::Size2DValues;

public:
    // Arithmetic with Resolution
    constexpr Resolution operator+(const Resolution& other) const noexcept {
        return Resolution(width + other.width, height + other.height);
    }
    constexpr Resolution& operator+=(const Resolution& other) noexcept {
        width += other.width; height += other.height;
        return *this;
    }
    constexpr Resolution operator-(const Resolution& other) const noexcept {
        return Resolution(width - other.width, height - other.height);
    }
    constexpr Resolution& operator-=(const Resolution& other) noexcept {
        width -= other.width; height -= other.height;
        return *this;
    }

    // Scalar operations
    constexpr Resolution operator*(double scalar) const noexcept {
        return Resolution(width * scalar, height * scalar);
    }
    constexpr Resolution& operator*=(double scalar) noexcept {
        width *= scalar; height *= scalar;
        return *this;
    }
    Resolution operator/(double scalar) const {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) return Resolution(width / scalar, height / scalar);
        NEX_ASSERT_MSG(false, "Division by zero");
    }
    Resolution& operator/=(double scalar) {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) { width /= scalar; height /= scalar; return *this; }
        NEX_ASSERT_MSG(false, "Division by zero");
    }

public:
    // Normalize resolution (positive width and height)
    Resolution normalize() const noexcept {
        return Resolution(NEX_STD abs(width), NEX_STD abs(height));
    }
    Resolution& normalize() noexcept {
        width = NEX_STD abs(width); height = NEX_STD abs(height);
        return *this;
    }

    // Calculate aspect ratio
    double aspectRatio() const noexcept {
        using math_utils::equalsToZero;
        if (equalsToZero(height)) return 0.0; // invalid result, not throwing exception here
        return static_cast<double>(width) / static_cast<double>(height);
    }
    NEX_STD pair<int, int> simplifiedAspectRatio() const {
        int w = static_cast<int>(NEX_STD round(width));
        int h = static_cast<int>(NEX_STD round(height));
        int gcd = NEX_STD gcd(w, h);
        return { w / gcd, h / gcd };
    }
};

/**
 * @struct Rect
 * @brief Represents a rectangle in 2D geometry
 * 
 * This struct represents a rectangle with position (x, y) and size (width, height). It inherits
 * from RectBase<double> and provides comprehensive rectangle operations. The rectangle can be
 * inverted (negative width or height), and operations handle both normal and inverted rectangles.
 * 
 * Rect supports:
 * - Construction from position and size, or from two corner points
 * - Factory methods (fromPositionSize, fromEdges)
 * - Access to corner points (topLeft, topRight, bottomLeft, bottomRight)
 * - Geometric properties (area, perimeter, center, diagonal, isSquare)
 * - Canonical operations for handling inverted rectangles
 * - Point containment checking
 * - Rectangle intersection and union operations
 * - Translation operations (offset, move)
 * - Transformation operations (flip, rotate, normalize, invert)
 * - Arithmetic operations with Point and Vector2D
 */
struct Rect : public RectBase<double>
{
public:
    // Construction
    using RectBase::RectBase;
    constexpr Rect(const Point& topLeft, const Point& bottomRight) 
        : Rect(topLeft.x, topLeft.y, (bottomRight.x - topLeft.x), (bottomRight.y - topLeft.y)) {}
    constexpr Rect(const Vector2D& position, const Size& size)
        : Rect(position.x, position.y, size.width, size.height) {}
        
public:
    // Create rectangle
    static constexpr Rect fromPositionSize(const Vector2D& position, const Size& size) {
        return Rect(position, size);
    }
    static constexpr Rect fromEdges(double left, double top, double right, double bottom) {
        return Rect(left, top, (right - left), (bottom - top));
    }
    static constexpr Rect fromEdges(const Point& topLeft, const Point& bottomRight) {
        return Rect(topLeft, bottomRight);
    }

public:
    // Access data
    Point topLeft() const noexcept { return Point(x, y); }
    Point topRight() const noexcept { return Point(x + width, y); }
    Point bottomLeft() const noexcept { return Point(x, y + height); }
    Point bottomRight() const noexcept { return Point(x + width, y + height); }

    // Modify data
    void setTopLeft(const Point& topLeft) noexcept { x = topLeft.x; y = topLeft.y; }
    void setTopLeft(double xVal, double yVal) noexcept { x = xVal; y = yVal; }
    void setBottomRight(const Point& bottomRight) noexcept { width = bottomRight.x - x; height = bottomRight.y - y; }
    void setBottomRight(double xVal, double yVal) noexcept { width = xVal - x; height = yVal - y; }

public:
    // Return canonical values for inverted rectangles
    constexpr double canonicalWidth() const noexcept {
        return NEX_STD abs(width);
    }
    constexpr double canonicalHeight() const noexcept {
        return NEX_STD abs(height);
    }
    constexpr Rect canonicalRect() const noexcept {
        return Rect(x, y, canonicalWidth(), canonicalHeight());
    }

    // Base conversion
    constexpr const RectBase<double>& asRectBase() const noexcept {
        return static_cast<const RectBase<double>&>(*this);
    }

public:
    // Comparison operators
    bool operator==(const Rect& other) const noexcept {
        return (this->canonicalRect().asRectBase() == other.canonicalRect().asRectBase());
    }
    bool operator!=(const Rect& other) const noexcept {
        return !(*this == other);
    }
    bool operator<(const Rect& other) const noexcept {
        return (this->canonicalRect().asRectBase() < other.canonicalRect().asRectBase());
    }
    bool operator<=(const Rect& other) const noexcept {
        return (*this < other || *this == other);
    }
    bool operator>(const Rect& other) const noexcept {
        return !(*this <= other);
    }
    bool operator>=(const Rect& other) const noexcept {
        return !(*this < other);
    }

    // Arithmetic Operators
    Rect operator+(const Point& offset) const noexcept {
        return Rect(x + offset.x, y + offset.y, width, height);
    }
    Rect& operator+=(const Point& offset) noexcept {
        x += offset.x; y += offset.y;
        return *this;
    }
    Rect operator-(const Point& offset) const noexcept {
        return Rect(x - offset.x, y - offset.y, width, height);
    }
    Rect& operator-=(const Point& offset) noexcept {
        x -= offset.x; y -= offset.y;
        return *this;
    }
    Rect operator+(const Vector2D& offset) const noexcept {
        return Rect(x + offset.x, y + offset.y, width, height);
    }
    Rect& operator+=(const Vector2D& offset) noexcept {
        x += offset.x; y += offset.y;
        return *this;
    }

public:
    // Size of rectangle
    Size size() const noexcept {
        return Size(width, height);
    }

    // Position of rectangle
    constexpr Vector2D position() const noexcept {
        return Vector2D(x, y);
    }

    // Is a square
    bool isSquare() const noexcept {
        using math_utils::nearlyEqual;
        return nearlyEqual(width, height);
    }

    // Area of the rectangle
    double area() const noexcept {
        return canonicalWidth() * canonicalHeight();
    }

    // Perimeter of the rectangle
    double perimeter() const noexcept {
        return (canonicalWidth() * 2.0 + canonicalHeight() * 2.0);
    }

    // Center position of the rectangle
    constexpr Vector2D center() const noexcept {
        return Vector2D(x + canonicalWidth() / 2.0, y + canonicalHeight() / 2.0);
    }

    // Diagonal of the rectangle
    double diagonal() const noexcept {
        return NEX_STD sqrt(canonicalWidth() * canonicalWidth() 
                                + canonicalHeight() * canonicalHeight());
    }

    // For inverted rectangles
    constexpr bool isInverted() const noexcept {
        return (width < 0.0 || height < 0.0);
    }
    Rect& normalize() noexcept {
        width = canonicalWidth(); height = canonicalHeight();
        return *this;
    }

    // Invert the rectangle
    Rect& invert() noexcept {
        width = (width < 0.0) ? NEX_STD abs(width) : (0.0 - width);
        height = (height < 0.0) ? NEX_STD abs(height) : (0.0 - height);
        return *this;
    }

    // Contains a point
    constexpr bool contains(double ptX, double ptY) const noexcept {
        return ptX >= x && ptX <= (x + canonicalWidth()) 
            && ptY >= y && ptY <= (y + canonicalHeight());
    }
    constexpr bool contains(const Point& point) const noexcept {
        return contains(point.x, point.y);
    }

    // Intersects with another rectangle
    constexpr bool intersects(const Rect& other) const noexcept {
        Rect canonThis = this->canonicalRect(), canonOther = other.canonicalRect();
        return !((canonThis.x + canonThis.width) < canonOther.x || canonThis.x > (canonOther.x + canonOther.width) || 
                (canonThis.y + canonThis.height) < canonOther.y || canonThis.y > (canonOther.y + canonOther.height));
    }

    // Intersection rectangle (returns an empty Rect if no intersection)
    Rect intersection(const Rect& other) const noexcept {
        if (!intersects(other)) return Rect();
        Rect canonThis = this->canonicalRect(), canonOther = other.canonicalRect();
        return Rect::fromEdges(NEX_STD max(canonThis.x, canonOther.x), NEX_STD max(canonThis.y, canonOther.y),
            NEX_STD min((canonThis.x + canonThis.width), (canonOther.x + canonOther.width)), 
            NEX_STD min((canonThis.y + canonThis.height), (canonOther.y + canonOther.height)));
    }

    // Union rectangle
    Rect unite(const Rect& other) const noexcept {
        Rect canonThis = this->canonicalRect(), canonOther = other.canonicalRect();
        return Rect::fromEdges(NEX_STD min(canonThis.x, canonOther.x), NEX_STD min(canonThis.y, canonOther.y),
            NEX_STD max((canonThis.x + canonThis.width), (canonOther.x + canonOther.width)), 
            NEX_STD max((canonThis.y + canonThis.height), (canonOther.y + canonOther.height)));
    }

    // Move/Offset the rectangle
    Rect& offset(double deltaX, double deltaY = 0) noexcept {
        x += deltaX; y += deltaY;
        return *this;
    }
    Rect& offset(const Point& point) noexcept {
        return offset(point.x, point.y);
    }
    Rect& offset(const Vector2D& vector) noexcept {
        return offset(vector.x, vector.y);
    }

    // Set new size (preserves whether it's inverted or not)
    Rect& setSize(const Size& newSize) noexcept {
        width = newSize.width; height = newSize.height;
        return *this;
    }

    // Flip horizontally: mirror across vertical axis
    Rect& flipHorizontally() noexcept {
        x += width; width = (width < 0.0) ? NEX_STD abs(width) : (0.0 - width);
        return *this;
    }

    // Flip vertically: mirror across horizontal axis
    Rect& flipVertically() noexcept {
        y += height; height = (height < 0.0) ? NEX_STD abs(height) : (0.0 - height);
        return *this;
    }

    // Rotation around center
    // This method only works like width/height swapping
    Rect rotate(Rotation rotation) const noexcept;

    // Rotatition around center with an arbitary angle
    // This returns a new axis-aligned bounding rectangle,
    // which often has different width and height compared to the original rectangle
    Rect rotate(double angleRadians) const noexcept {
        return rotateAround(center(), angleRadians);
    }

    // Rotation around a pivot (any point)
    // This returns a new axis-aligned bounding rectangle,
    // which often has different width and height compared to the original rectangle
    Rect rotateAround(const Vector2D& pivot, double angleRadians) const noexcept;
};

/**
 * @struct EdgeRect
 * @brief Represents an edge-based rectangle in 2D geometry
 * 
 * This struct represents a rectangle using edge coordinates (left, top, right, bottom) instead of
 * position and size. It inherits from Edge2DValues<double> and provides rectangle operations
 * similar to Rect but using edge-based representation. The rectangle can be inverted (right < left
 * or bottom < top), and operations handle both normal and inverted rectangles.
 * 
 * EdgeRect supports:
 * - Construction from edges, position and size, or from two corner points
 * - Factory methods (fromPositionSize, fromEdges)
 * - Access to corner points (topLeft, topRight, bottomLeft, bottomRight)
 * - Geometric properties (width, height, area, perimeter, center, diagonal, isSquare)
 * - Canonical operations for handling inverted rectangles
 * - Point containment checking
 * - Rectangle intersection and union operations
 * - Translation operations (offset, move)
 * - Transformation operations (flip, rotate, normalize, invert)
 * - Arithmetic operations with Point and Vector2D
 */
struct EdgeRect : public Edge2DValues<double>
{
public:
    // Construction
    using Edge2DValues::Edge2DValues;
    constexpr EdgeRect(const Point& topLeft, const Point& bottomRight) 
        : Edge2DValues(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y) {}
    constexpr EdgeRect(const Vector2D& position, const Size& size)
        : Edge2DValues(position.x, position.y, (position.x + size.width), (position.y + size.height)) {}

public:
    // Create rectangle
    static constexpr EdgeRect fromPositionSize(const Vector2D& position, const Size& size) {
        return EdgeRect(position, size);
    }
    static constexpr EdgeRect fromEdges(double left, double top, double right, double bottom) {
        return EdgeRect(left, top, right, bottom);
    }
    static constexpr EdgeRect fromEdges(const Point& topLeft, const Point& bottomRight) {
        return EdgeRect(topLeft, bottomRight);
    }

public:
    // Access data
    Point topLeft() const noexcept { return Point(left, top); }
    Point topRight() const noexcept { return Point(right, top); }
    Point bottomLeft() const noexcept { return Point(left, bottom); }
    Point bottomRight() const noexcept { return Point(right, bottom); }

    // Modify data
    void setTopLeft(const Point& topLeft) noexcept { left = topLeft.x; top = topLeft.y; }
    void setTopLeft(double x, double y) noexcept { left = x; top = y; }
    void setBottomRight(const Point& bottomRight) noexcept { right = bottomRight.x; bottom = bottomRight.y; }
    void setBottomRight(double x, double y) noexcept { right = x; bottom = y; }

public:
    // Return canonical edge values for inverted rectangles
    constexpr double canonicalLeft() const noexcept {
        return NEX_STD min(left, right);
    }
    constexpr double canonicalRight() const noexcept {
        return NEX_STD max(left, right);
    }
    constexpr double canonicalTop() const noexcept {
        return NEX_STD min(top, bottom);
    }
    constexpr double canonicalBottom() const noexcept {
        return NEX_STD max(top, bottom);
    }
    constexpr EdgeRect canonicalRect() const noexcept {
        return EdgeRect(canonicalLeft(), canonicalTop(), canonicalRight(), canonicalBottom());
    }

    // Base conversion
    constexpr const Edge2DValues<double>& asEdge2DValues() const noexcept {
        return static_cast<const Edge2DValues<double>&>(*this);
    }

public:
    // Comparison operators
    bool operator==(const EdgeRect& other) const noexcept {
        return (this->canonicalRect().asEdge2DValues() == other.canonicalRect().asEdge2DValues());
    }
    bool operator!=(const EdgeRect& other) const noexcept {
        return !(*this == other);
    }
    bool operator<(const EdgeRect& other) const noexcept {
        return (this->canonicalRect().asEdge2DValues() < other.canonicalRect().asEdge2DValues());
    }
    bool operator<=(const EdgeRect& other) const noexcept {
        return (*this < other || *this == other);
    }
    bool operator>(const EdgeRect& other) const noexcept {
        return !(*this <= other);
    }
    bool operator>=(const EdgeRect& other) const noexcept {
        return !(*this < other);
    }

    // Arithmetic Operators
    EdgeRect operator+(const Point& offset) const noexcept {
        return EdgeRect(left + offset.x, top + offset.y, right + offset.x, bottom + offset.y);
    }
    EdgeRect& operator+=(const Point& offset) noexcept {
        left += offset.x; top += offset.y; right += offset.x; bottom += offset.y;
        return *this;
    }
    EdgeRect operator-(const Point& offset) const noexcept {
        return EdgeRect(left - offset.x, top - offset.y, right - offset.x, bottom - offset.y);
    }
    EdgeRect& operator-=(const Point& offset) noexcept {
        left -= offset.x; top -= offset.y;
        right -= offset.x; bottom -= offset.y;
        return *this;
    }
    EdgeRect operator+(const Vector2D& offset) const noexcept {
        return EdgeRect(left + offset.x, top + offset.y, right + offset.x, bottom + offset.y);
    }
    EdgeRect& operator+=(const Vector2D& offset) noexcept {
        left += offset.x; top += offset.y; right += offset.x; bottom += offset.y;
        return *this;
    }

    // Stream Operators
    friend NEX_STD ostream& operator<<(NEX_STD ostream& os, const EdgeRect& rect) {
        return os << "EdgeRect(left=" << rect.left << ", top=" << rect.top 
                    << ", right=" << rect.right << ", bottom=" << rect.bottom << ")";
    }
    friend NEX_STD wostream& operator<<(NEX_STD wostream& wos, const EdgeRect& rect) {
        return wos << L"EdgeRect(left=" << rect.left << L", top=" << rect.top 
                    << L", right=" << rect.right << L", bottom=" << rect.bottom << L")";
    }

public:
    // Width and height of the rectangle
    double width() const noexcept {
        return NEX_STD abs(right - left);
    }
    double height() const noexcept {
        return NEX_STD abs(bottom - top);
    }

    // Size of rectangle
    Size size() const noexcept {
        return Size(width(), height());
    }

    // Position of rectangle
    constexpr Vector2D position() const noexcept {
        return Vector2D(canonicalLeft(), canonicalTop());
    }

    // Is a square
    bool isSquare() const noexcept {
        using math_utils::nearlyEqual;
        return nearlyEqual(width(), height());
    }

    // Area of the rectangle
    double area() const noexcept {
        return width() * height();
    }

    // Perimeter of the rectangle
    double perimeter() const noexcept {
        return (width() * 2.0 + height() * 2.0);
    }

    // Center position of the rectangle
    constexpr Vector2D center() const noexcept {
        return Vector2D((canonicalLeft() + canonicalRight()) / 2.0, (canonicalTop() + canonicalBottom()) / 2.0);
    }

    // Diagonal of the rectangle
    double diagonal() const noexcept {
        return NEX_STD sqrt(width() * width() + height() * height());
    }

    // For inverted rectangles
    constexpr bool isInverted() const noexcept {
        return (right < left || bottom < top);
    }
    EdgeRect& normalize() noexcept {
        if (right < left) NEX_STD swap(right, left);
        if (bottom < top) NEX_STD swap(bottom, top);
        return *this;
    }

    // Invert the rectangle
    EdgeRect& invert() noexcept {
        return this->flipHorizontally().flipVertically();
    }

    // Contains a point
    constexpr bool contains(double x, double y) const noexcept {
        return x >= canonicalLeft() && x <= canonicalRight() && y >= canonicalTop() && y <= canonicalBottom();
    }
    constexpr bool contains(const Point& point) const noexcept {
        return contains(point.x, point.y);
    }

    // Intersects with another rectangle
    constexpr bool intersects(const EdgeRect& other) const noexcept {
        EdgeRect canonThis = this->canonicalRect(), canonOther = other.canonicalRect();
        return !(canonThis.right < canonOther.left || canonThis.left > canonOther.right || 
                canonThis.bottom < canonOther.top || canonThis.top > canonOther.bottom);
    }

    // Intersection rectangle (returns an empty EdgeRect if no intersection)
    EdgeRect intersection(const EdgeRect& other) const noexcept {
        if (!intersects(other)) return EdgeRect();
        EdgeRect canonThis = this->canonicalRect(), canonOther = other.canonicalRect();
        return EdgeRect(NEX_STD max(canonThis.left, canonOther.left), NEX_STD max(canonThis.top, canonOther.top),
            NEX_STD min(canonThis.right, canonOther.right), NEX_STD min(canonThis.bottom, canonOther.bottom));
    }

    // Union rectangle
    EdgeRect unite(const EdgeRect& other) const noexcept {
        EdgeRect canonThis = this->canonicalRect(), canonOther = other.canonicalRect();
        return EdgeRect(NEX_STD min(canonThis.left, canonOther.left), NEX_STD min(canonThis.top, canonOther.top),
            NEX_STD max(canonThis.right, canonOther.right), NEX_STD max(canonThis.bottom, canonOther.bottom));
    }

    // Move/Offset the rectangle
    EdgeRect& offset(double deltaX, double deltaY = 0) noexcept {
        left += deltaX; right += deltaX; top += deltaY; bottom += deltaY;
        return *this;
    }
    EdgeRect& offset(const Point& point) noexcept {
        return offset(point.x, point.y);
    }
    EdgeRect& offset(const Vector2D& vector) noexcept {
        return offset(vector.x, vector.y);
    }

    // Set new size (preserves whether it's inverted or not)
    EdgeRect& setSize(const Size& newSize) noexcept {
        if (right >= left) { right = left + newSize.width; }
        else { right = left - newSize.width; }
        if (bottom >= top) { bottom = top + newSize.height; }
        else { bottom = top - newSize.height; }
        return *this;
    }

    // Flip horizontally: mirror across vertical axis
    EdgeRect& flipHorizontally() noexcept {
        NEX_STD swap(left, right);
        return *this;
    }

    // Flip vertically: mirror across horizontal axis
    EdgeRect& flipVertically() noexcept {
        NEX_STD swap(top, bottom);
        return *this;
    }

    // Rotation around center
    // This method only works like width/height swapping
    EdgeRect rotate(Rotation rotation) const noexcept;

    // Rotatition around center with an arbitary angle
    // This returns a new axis-aligned bounding rectangle,
    // which often has different width and height compared to the original rectangle
    EdgeRect rotate(double angleRadians) const noexcept {
        return rotateAround(center(), angleRadians);
    }

    // Rotation around a pivot (any point)
    // This returns a new axis-aligned bounding rectangle,
    // which often has different width and height compared to the original rectangle
    EdgeRect rotateAround(const Vector2D& pivot, double angleRadians) const noexcept;
};

/**
 * @struct Margin
 * @brief Represents UI margin (space outside an element's border)
 * 
 * This struct represents margin values with left, top, right, and bottom components. It inherits
 * from Edge2DValues<double> and is commonly used in UI layout calculations to define spacing
 * outside an element's border.
 * 
 * Margin supports:
 * - Construction from left, top, right, bottom values
 * - Factory method from EdgeRect
 * - Arithmetic operations with other Margin objects (addition, subtraction)
 * - Scalar operations (multiplication, division)
 * - Total horizontal and vertical margin calculation
 * - Centering operations (centerHorizontally, centerVertically)
 * - Application to EdgeRect (expands rectangle by margin)
 * - Size manipulation (shrinkSize, expandSize)
 * - Conversion to EdgeRect
 */
struct Margin : public Edge2DValues<double>
{
public:
    // Construction
    using Edge2DValues::Edge2DValues;

public:
    // Create margin
    static constexpr Margin fromEdgeRect(const EdgeRect& rect) {
        return Margin(rect.left, rect.top, rect.right, rect.bottom);
    }

public:
    // Arithmetic Operators
    Margin operator+(const Margin& other) const noexcept {
        return Margin(left + other.left, top + other.top, right + other.right, bottom + other.bottom);
    }
    Margin& operator+=(const Margin& other) noexcept {
        left += other.left; top += other.top; right += other.right; bottom += other.bottom;
        return *this;
    }
    Margin operator-(const Margin& other) const noexcept {
        return Margin(left - other.left, top - other.top, right - other.right, bottom - other.bottom);
    }
    Margin& operator-=(const Margin& other) noexcept {
        left -= other.left; top -= other.top; right -= other.right; bottom -= other.bottom;
        return *this;
    }

    // Scalar operators
    constexpr Margin operator*(double scalar) const noexcept {
        return Margin(left * scalar, top * scalar, right * scalar, bottom * scalar);
    }
    Margin operator/(double scalar) const {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) return Margin(left / scalar, top / scalar, right / scalar, bottom / scalar);
        NEX_ASSERT_MSG(false, "Division by zero");
    }
    Margin& operator*=(double scalar) noexcept {
        left *= scalar; top *= scalar; right *= scalar; bottom *= scalar;
        return *this;
    }
    Margin& operator/=(double scalar) {
        using math_utils::equalsToZero;
        if (equalsToZero(scalar)) { 
            NEX_ASSERT_MSG(false, "Division by zero");
        }
        left /= scalar; top /= scalar; right /= scalar; bottom /= scalar; 
        return *this; 
    }

    // Stream Operators
    friend NEX_STD ostream& operator<<(NEX_STD ostream& os, const Margin& margin) {
        return os << "Margin(left=" << margin.left << ", top=" << margin.top 
                    << ", right=" << margin.right << ", bottom=" << margin.bottom << ")";
    }
    friend NEX_STD wostream& operator<<(NEX_STD wostream& wos, const Margin& margin) {
        return wos << L"Margin(left=" << margin.left << L", top=" << margin.top 
                    << L", right=" << margin.right << L", bottom=" << margin.bottom << L")";
    }

public:
    // Total horizontal/vertical
    double totalHorizontal() const noexcept {
        return left + right;
    }
    double totalVertical() const noexcept {
        return top + bottom;
    }

    // Center horizontally/vertically
    Margin& centerHorizontally() noexcept {
        double totalHMargin = totalHorizontal();
        left = right = totalHMargin / 2;
        return *this;
    }
    Margin& centerVertically() noexcept {
        double totalVMargin = totalVertical();
        top = bottom = totalVMargin / 2;
        return *this;
    }

    // Stretch an EdgeRect by margin (used to calculate bounding box)
    EdgeRect& applyTo(EdgeRect& rect) const noexcept {
        if (rect.left > rect.right) { rect.left += left; rect.right -= right; }
        else { rect.left -= left; rect.right += right; }
        if (rect.top > rect.bottom) { rect.top += top; rect.bottom -= bottom; }
        else { rect.top -= top; rect.bottom += bottom; }
        return rect;
    }

    // Shrink or expand a size by margin
    Size& shrinkSize(Size& size) const noexcept {
        size.width -= totalHorizontal(); size.height -= totalVertical();
        return size;
    }
    Size& expandSize(Size& size) const noexcept {
        size.width += totalHorizontal(); size.height += totalVertical();
        return size;
    }

    // Conversion
    EdgeRect toEdgeRect() const noexcept {
        return EdgeRect(left, top, right, bottom);
    }
};

/**
 * @struct Padding
 * @brief Represents UI padding (space inside an element's border)
 * 
 * This struct represents padding values with left, top, right, and bottom components. It inherits
 * from Edge2DValues<double> and is commonly used in UI layout calculations to define spacing
 * inside an element's border.
 * 
 * Padding supports:
 * - Construction from left, top, right, bottom values
 * - Factory method from EdgeRect
 * - Arithmetic operations with other Padding objects (addition, subtraction)
 * - Scalar operations (multiplication, division)
 * - Total horizontal and vertical padding calculation
 * - Centering operations (centerHorizontally, centerVertically)
 * - Application to EdgeRect (shrinks rectangle by padding)
 * - Size manipulation (shrinkSize, expandSize)
 * - Conversion to EdgeRect
 */
struct Padding : public Edge2DValues<double>
{
public:
    // Construction
    using Edge2DValues::Edge2DValues;

public:
    // Create padding
    static constexpr Padding fromEdgeRect(const EdgeRect& rect) {
        return Padding(rect.left, rect.top, rect.right, rect.bottom);
    }

public:
    // Arithmetic Operators
    Padding operator+(const Padding& other) const noexcept {
        return Padding(left + other.left, top + other.top, right + other.right, bottom + other.bottom);
    }
    Padding& operator+=(const Padding& other) noexcept {
        left += other.left; top += other.top; right += other.right; bottom += other.bottom;
        return *this;
    }
    Padding operator-(const Padding& other) const noexcept {
        return Padding(left - other.left, top - other.top, right - other.right, bottom - other.bottom);
    }
    Padding& operator-=(const Padding& other) noexcept {
        left -= other.left; top -= other.top; right -= other.right; bottom -= other.bottom;
        return *this;
    }

    // Scalar operators
    constexpr Padding operator*(double scalar) const noexcept {
        return Padding(left * scalar, top * scalar, right * scalar, bottom * scalar);
    }
    Padding operator/(double scalar) const {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) return Padding(left / scalar, top / scalar, right / scalar, bottom / scalar);
        NEX_ASSERT_MSG(false, "Division by zero");
    }
    Padding& operator*=(double scalar) noexcept {
        left *= scalar; top *= scalar; right *= scalar; bottom *= scalar;
        return *this;
    }
    Padding& operator/=(double scalar) {
        using math_utils::equalsToZero;
        if (equalsToZero(scalar)) { 
            NEX_ASSERT_MSG(false, "Division by zero");
        }
        left /= scalar; top /= scalar; right /= scalar; bottom /= scalar; 
        return *this; 
    }

    // Stream Operators
    friend NEX_STD ostream& operator<<(NEX_STD ostream& os, const Padding& padding) {
        return os << "Padding(left=" << padding.left << ", top=" << padding.top 
                    << ", right=" << padding.right << ", bottom=" << padding.bottom << ")";
    }
    friend NEX_STD wostream& operator<<(NEX_STD wostream& wos, const Padding& padding) {
        return wos << L"Padding(left=" << padding.left << L", top=" << padding.top 
                    << L", right=" << padding.right << L", bottom=" << padding.bottom << L")";
    }

public:
    // Total horizontal/vertical
    double totalHorizontal() const noexcept {
        return left + right;
    }
    double totalVertical() const noexcept {
        return top + bottom;
    }

    // Center horizontally/vertically
    Padding& centerHorizontally() noexcept {
        double totalHPadding = totalHorizontal();
        left = right = totalHPadding / 2;
        return *this;
    }
    Padding& centerVertically() noexcept {
        double totalVPadding = totalVertical();
        top = bottom = totalVPadding / 2;
        return *this;
    }

    // Shrink an EdgeRect by padding (used to calculate bounding box)
    EdgeRect& applyTo(EdgeRect& rect) const noexcept {
        if (rect.left < rect.right) { rect.left += left; rect.right -= right; }
        else { rect.left -= left; rect.right += right; }
        if (rect.top < rect.bottom) { rect.top += top; rect.bottom -= bottom; }
        else { rect.top -= top; rect.bottom += bottom; }
        return rect;
    }

    // Shrink or expand a size by padding
    Size& shrinkSize(Size& size) const noexcept {
        size.width -= totalHorizontal(); size.height -= totalVertical();
        return size;
    }
    Size& expandSize(Size& size) const noexcept {
        size.width += totalHorizontal(); size.height += totalVertical();
        return size;
    }

    // Conversion
    EdgeRect toEdgeRect() const noexcept {
        return EdgeRect(left, top, right, bottom);
    }
};

/**
 * @struct Thickness
 * @brief Represents UI border thickness (width of an element's border)
 * 
 * This struct represents border thickness values with left, top, right, and bottom components.
 * It inherits from Edge2DValues<double> and is commonly used in UI layout calculations to define
 * the width of an element's border on each side.
 * 
 * Thickness supports:
 * - Construction from left, top, right, bottom values
 * - Factory methods from EdgeRect, Margin, and Padding
 * - Arithmetic operations with other Thickness objects (addition, subtraction)
 * - Scalar operations (multiplication, division)
 * - Total horizontal and vertical thickness calculation
 * - Centering operations (centerHorizontally, centerVertically)
 * - Application to EdgeRect (adjusts rectangle by thickness)
 * - Size manipulation (shrinkSize, expandSize)
 * - Conversion to EdgeRect
 */
struct Thickness : public Edge2DValues<double>
{
public:
    // Construction
    using Edge2DValues::Edge2DValues;

public:
    // Create thickness
    static constexpr Thickness fromEdgeRect(const EdgeRect& rect) {
        return Thickness(rect.left, rect.top, rect.right, rect.bottom);
    }
    static constexpr Thickness fromMargin(const Margin& margin) {
        return Thickness(margin.left, margin.top, margin.right, margin.bottom);
    }
    static constexpr Thickness fromPadding(const Padding& padding) {
        return Thickness(padding.left, padding.top, padding.right, padding.bottom);
    }

public:
    // Arithmetic Operators
    Thickness operator+(const Thickness& other) const noexcept {
        return Thickness(left + other.left, top + other.top, right + other.right, bottom + other.bottom);
    }
    Thickness& operator+=(const Thickness& other) noexcept {
        left += other.left; top += other.top; right += other.right; bottom += other.bottom;
        return *this;
    }
    Thickness operator-(const Thickness& other) const noexcept {
        return Thickness(left - other.left, top - other.top, right - other.right, bottom - other.bottom);
    }
    Thickness& operator-=(const Thickness& other) noexcept {
        left -= other.left; top -= other.top; right -= other.right; bottom -= other.bottom;
        return *this;
    }

    // Scalar operators
    constexpr Thickness operator*(double scalar) const noexcept {
        return Thickness(left * scalar, top * scalar, right * scalar, bottom * scalar);
    }
    Thickness operator/(double scalar) const {
        using math_utils::equalsToZero;
        if (!equalsToZero(scalar)) return Thickness(left / scalar, top / scalar, right / scalar, bottom / scalar);
        NEX_ASSERT_MSG(false, "Division by zero");
    }
    Thickness& operator*=(double scalar) noexcept {
        left *= scalar; top *= scalar; right *= scalar; bottom *= scalar;
        return *this;
    }
    Thickness& operator/=(double scalar) {
        using math_utils::equalsToZero;
        if (equalsToZero(scalar)) { 
            NEX_ASSERT_MSG(false, "Division by zero");
        }
        left /= scalar; top /= scalar; right /= scalar; bottom /= scalar; 
        return *this; 
    }

    // Stream Operators
    friend NEX_STD ostream& operator<<(NEX_STD ostream& os, const Thickness& thickness) {
        return os << "Thickness(left=" << thickness.left << ", top=" << thickness.top 
                    << ", right=" << thickness.right << ", bottom=" << thickness.bottom << ")";
    }
    friend NEX_STD wostream& operator<<(NEX_STD wostream& wos, const Thickness& thickness) {
        return wos << L"Thickness(left=" << thickness.left << L", top=" << thickness.top 
                    << L", right=" << thickness.right << L", bottom=" << thickness.bottom << L")";
    }

public:
    // Total horizontal/vertical
    double totalHorizontal() const noexcept {
        return left + right;
    }
    double totalVertical() const noexcept {
        return top + bottom;
    }

    // Center horizontally/vertically
    Thickness& centerHorizontally() noexcept {
        double totalHThickness = totalHorizontal();
        left = right = totalHThickness / 2;
        return *this;
    }
    Thickness& centerVertically() noexcept {
        double totalVThickness = totalVertical();
        top = bottom = totalVThickness / 2;
        return *this;
    }

    // Stretch an EdgeRect by border thickness (used to calculate bounding box)
    EdgeRect& applyTo(EdgeRect& rect) const noexcept {
        if (rect.left > rect.right) { rect.left += left; rect.right -= right; }
        else { rect.left -= left; rect.right += right; }
        if (rect.top > rect.bottom) { rect.top += top; rect.bottom -= bottom; }
        else { rect.top -= top; rect.bottom += bottom; }
        return rect;
    }

    // Shrink or expand a size by border thickness
    Size& shrinkSize(Size& size) const noexcept {
        size.width -= totalHorizontal(); size.height -= totalVertical();
        return size;
    }
    Size& expandSize(Size& size) const noexcept {
        size.width += totalHorizontal(); size.height += totalVertical();
        return size;
    }

    // Conversion
    EdgeRect toEdgeRect() const noexcept {
        return EdgeRect(left, top, right, bottom);
    }
};

NEX_CORE_NAMESPACE_END