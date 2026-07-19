/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/containers/array.h"
#include "nex/core/math/geometry.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// Geometry2D implementation helper functions
// =================================================================================

NEX_SUBNAMESPACE_BEGIN(geometry2d)

// Rotation direction to angle radians
float64 rotationToAngle(math::Rotation rotation) noexcept {
    float64 angleRadians = 0.0;
    switch (rotation) {
        case math::Rotation::Clockwise_90:         angleRadians = -M_PI / 2.0;          break;
        case math::Rotation::Clockwise_180:        angleRadians = -M_PI;                break;
        case math::Rotation::Clockwise_270:        angleRadians = -3 * M_PI / 2.0;      break;
        case math::Rotation::CounterClockwise_90:  angleRadians = M_PI / 2.0;           break;
        case math::Rotation::CounterClockwise_180: angleRadians = M_PI;                 break;
        case math::Rotation::CounterClockwise_270: angleRadians = 3 * M_PI / 2.0;       break;
    }
    return angleRadians;
}

// Rotates a point around a pivot
Vector2D rotatePointAround(const Vector2D& point, const Vector2D& pivot, float64 angle) noexcept {
    float64 sin = NEX_STD sin(angle);
    float64 cos = NEX_STD cos(angle);

    // Translate to origin
    Vector2D translated = point - pivot;

    // Rotate
    float64 newX = translated.x * cos - translated.y * sin;
    float64 newY = translated.x * sin + translated.y * cos;

    // Translate back
    return Vector2D(newX + pivot.x, newY + pivot.y);
}

NEX_SUBNAMESPACE_END(geometry2d)

// =================================================================================
// Implementation of Rect class methods
// =================================================================================

// Rectangle rotation around center
Rect Rect::rotate(math::Rotation rotation) const noexcept {
    // Rotate around center
    float64 angleRadians = geometry2d::rotationToAngle(rotation);
    return rotateAround(center(), angleRadians);
}

// Rectangle rotation around a pivot (any point)
Rect Rect::rotateAround(const Vector2D& pivot, float64 angleRadians) const noexcept {
    // Get the raw top-left and bottom-right (without normalizing)
    float64 left = x, top = y;
    float64 right = x + width, bottom = y + height;

    // Get the 4 corners (respecting inversion)
    Array<Vector2D, 4> corners = {
        Vector2D(left,  top), Vector2D(right, top),
        Vector2D(right, bottom), Vector2D(left,  bottom)
    };

    // Rotate each corner around the pivot
    for (auto& point : corners)
        point = geometry2d::rotatePointAround(point, pivot, angleRadians);

    // Compute the new bounding box
    float64 minX = corners[0].x, maxX = corners[0].x;
    float64 minY = corners[0].y, maxY = corners[0].y;
    for (int32 i = 1; i < 4; ++i) {
        minX = NEX_STD min(minX, corners[i].x);
        maxX = NEX_STD max(maxX, corners[i].x);
        minY = NEX_STD min(minY, corners[i].y);
        maxY = NEX_STD max(maxY, corners[i].y);
    }

    // Preserve inverted state
    bool invertedX = (width < 0.0);
    bool invertedY = (height < 0.0);

    float64 newX = invertedX ? maxX : minX;
    float64 newY = invertedY ? maxY : minY;
    float64 newW = NEX_STD abs(maxX - minX);
    float64 newH = NEX_STD abs(maxY - minY);

    if (invertedX) newW = -newW;
    if (invertedY) newH = -newH;

    return Rect(newX, newY, newW, newH);
}

// =================================================================================
// Implementation of EdgeRect class methods
// =================================================================================

// Rectangle rotation around center
EdgeRect EdgeRect::rotate(math::Rotation rotation) const noexcept {
    // Rotate around center
    float64 angleRadians = geometry2d::rotationToAngle(rotation);
    return rotateAround(center(), angleRadians);
}

// Rectangle rotation around a pivot (any point)
EdgeRect EdgeRect::rotateAround(const Vector2D& pivot, float64 angleRadians) const noexcept {
    // Pre-check inverted state
    bool isInvertedX = (left > right);
    bool isInvertedY = (top > bottom);

    // Get the 4 corners (respecting inversion)
    Array<Vector2D, 4> corners = {
        Vector2D(left,  top), Vector2D(right, top),
        Vector2D(right, bottom), Vector2D(left,  bottom)
    };

    // Rotate each corner around the pivot
    for (auto& point : corners)
        point = geometry2d::rotatePointAround(point, pivot, angleRadians);

    // Compute the new bounding box
    float64 minX = corners[0].x, maxX = corners[0].x;
    float64 minY = corners[0].y, maxY = corners[0].y;
    for (int32 i = 1; i < 4; ++i) {
        minX = NEX_STD min(minX, corners[i].x);
        maxX = NEX_STD max(maxX, corners[i].x);
        minY = NEX_STD min(minY, corners[i].y);
        maxY = NEX_STD max(maxY, corners[i].y);
    }

    // Preserve inversion
    float64 newLeft   = isInvertedX ? maxX : minX;
    float64 newRight  = isInvertedX ? minX : maxX;
    float64 newTop    = isInvertedY ? maxY : minY;
    float64 newBottom = isInvertedY ? minY : maxY;

    return EdgeRect(newLeft, newTop, newRight, newBottom);
}

NEX_NAMESPACE_END