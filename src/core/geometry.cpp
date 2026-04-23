/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/geometry.h"
#include "nex/base/linear.h"

NEX_NAMESPACE_BEGIN

// Geometry2D implementation helper functions
namespace geometry2d
{
    // Rotation direction to angle radians
    double rotationToAngle(Rotation rotation) noexcept {
        double angleRadians = 0.0;
        switch (rotation) {
            case Rotation::Clockwise_90:         angleRadians = -M_PI / 2.0;        break;
            case Rotation::Clockwise_180:        angleRadians = -M_PI;                break;
            case Rotation::Clockwise_270:        angleRadians = -3 * M_PI / 2.0;    break;
            case Rotation::CounterClockwise_90:  angleRadians = M_PI / 2.0;            break;
            case Rotation::CounterClockwise_180: angleRadians = M_PI;                break;
            case Rotation::CounterClockwise_270: angleRadians = 3 * M_PI / 2.0;        break;
        }
        return angleRadians;
    }
    
    // Rotates a point around a pivot
    Vector2D rotatePointAround(const Vector2D& point, const Vector2D& pivot, double angle) noexcept {
        double sin = NEX_STD sin(angle);
        double cos = NEX_STD cos(angle);

        // Translate to origin
        Vector2D translated = point - pivot;

        // Rotate
        double newX = translated.x * cos - translated.y * sin;
        double newY = translated.x * sin + translated.y * cos;

        // Translate back
        return Vector2D(newX + pivot.x, newY + pivot.y);
    }
} // namespace geometry2d

// Rectangle rotation around center
Rect Rect::rotate(Rotation rotation) const noexcept {
    // Rotate around center
    double angleRadians = geometry2d::rotationToAngle(rotation);
    return rotateAround(center(), angleRadians);
}

// Rectangle rotation around a pivot (any point)
Rect Rect::rotateAround(const Vector2D& pivot, double angleRadians) const noexcept {
    // Get the raw top-left and bottom-right (without normalizing)
    double left = x, top = y;
    double right = x + width, bottom = y + height;

    // Get the 4 corners (respecting inversion)
    Array<Vector2D, 4> corners = {
        Vector2D(left,  top), Vector2D(right, top),
        Vector2D(right, bottom), Vector2D(left,  bottom)
    };

    // Rotate each corner around the pivot
    for (auto& point : corners)
        point = geometry2d::rotatePointAround(point, pivot, angleRadians);

    // Compute the new bounding box
    double minX = corners[0].x, maxX = corners[0].x;
    double minY = corners[0].y, maxY = corners[0].y;
    for (int i = 1; i < 4; ++i) {
        minX = NEX_STD min(minX, corners[i].x);
        maxX = NEX_STD max(maxX, corners[i].x);
        minY = NEX_STD min(minY, corners[i].y);
        maxY = NEX_STD max(maxY, corners[i].y);
    }

    // Preserve inverted state
    bool invertedX = (width < 0.0);
    bool invertedY = (height < 0.0);

    double newX = invertedX ? maxX : minX;
    double newY = invertedY ? maxY : minY;
    double newW = NEX_STD abs(maxX - minX);
    double newH = NEX_STD abs(maxY - minY);

    if (invertedX) newW = -newW;
    if (invertedY) newH = -newH;

    return Rect(newX, newY, newW, newH);
}

// Rectangle rotation around center
EdgeRect EdgeRect::rotate(Rotation rotation) const noexcept {
    // Rotate around center
    double angleRadians = geometry2d::rotationToAngle(rotation);
    return rotateAround(center(), angleRadians);
}

// Rectangle rotation around a pivot (any point)
EdgeRect EdgeRect::rotateAround(const Vector2D& pivot, double angleRadians) const noexcept {
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
    double minX = corners[0].x, maxX = corners[0].x;
    double minY = corners[0].y, maxY = corners[0].y;
    for (int i = 1; i < 4; ++i) {
        minX = NEX_STD min(minX, corners[i].x);
        maxX = NEX_STD max(maxX, corners[i].x);
        minY = NEX_STD min(minY, corners[i].y);
        maxY = NEX_STD max(maxY, corners[i].y);
    }

    // Preserve inversion
    double newLeft   = isInvertedX ? maxX : minX;
    double newRight  = isInvertedX ? minX : maxX;
    double newTop    = isInvertedY ? maxY : minY;
    double newBottom = isInvertedY ? minY : maxY;

    return EdgeRect(newLeft, newTop, newRight, newBottom);
}

NEX_NAMESPACE_END