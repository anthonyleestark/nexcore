/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <compare>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/core/text/string.h"
#include "nex/core/text/string_view.h"
#include "nex/base/result.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @struct Version
 * @brief  A struct that represents version information for the library or application
 * 
 * @details
 * The `Version` struct encapsulates version information, including major, minor, and patch version 
 * numbers, as well as optional pre-release and build metadata. 
 * It provides comparison operators for easy version comparison, factory methods for creating `Version` 
 * instances from strings or packed integer representations, and utility methods for checking version 
 * validity and pre-release status.
 * 
 * @par Example Usage:
 * @code
 * Version v1(1, 0, 0); // Major version 1, minor version 0, patch version 0
 * Version v2 = Version::fromString("1.2.3"); // Create from string
 * Version v3 = Version::fromPacked(0x000100020003); // Create from packed integer
 * if (v1 < v2) {
 *     std::cout << "v1 is older than v2" << std::endl;
 * }
 * std::cout << "v2: " << v2.toString() << std::endl; // Output: v2: 1.2.3
 * @endcode
 * 
 * @see Semantic Versioning (https://semver.org/) for more details on versioning conventions.
 */
struct NEX_EXPORT Version {
    uint16 major = 0;           // Major version number (e.g., 1 in "1.2.3")
    uint16 minor = 0;           // Minor version number (e.g., 2 in "1.2.3")
    uint16 patch = 0;           // Patch version number (e.g., 3 in "1.2.3")
    String preRelease;          // Optional pre-release identifier (e.g., "alpha", "beta", "rc.1")
    String buildMetadata;       // Optional build metadata (e.g., "build.1234", "exp.sha.5114f85")

    ////// Constructors --------------------------------------------------

    // Default constructor
    Version() = default;

    // Construct a version info with individual version components
    Version(uint16 major, uint16 minor, uint16 patch) noexcept
        : major(major), minor(minor), patch(patch) {}

    ////// Comparison operators ------------------------------------------

    // Equality operator
    constexpr bool operator==(const Version& other) const noexcept {
        return major == other.major && minor == other.minor && patch == other.patch;
    }

    // Inequality operator
    constexpr bool operator!=(const Version& other) const noexcept {
        return !(*this == other);
    }

    // Less-than operator for version comparison
    constexpr bool operator<(const Version& other) const noexcept {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }

    // Greater-than operator for version comparison
    constexpr bool operator>(const Version& other) const noexcept {
        return other < *this;
    }

    // Less-than or equal operator for version comparison
    constexpr bool operator<=(const Version& other) const noexcept {
        return !(other < *this);
    }

    // Greater-than or equal operator for version comparison
    constexpr bool operator>=(const Version& other) const noexcept {
        return !(*this < other);
    }

    // Strongly-ordered comparison operator for version comparison
    constexpr NEX_STD strong_ordering operator<=>(const Version& other) const noexcept {
        if (auto cmp = major <=> other.major; cmp != 0) return cmp;
        if (auto cmp = minor <=> other.minor; cmp != 0) return cmp;
        return patch <=> other.patch;
    }

    ////// Factory method --------------------------------------------------

    // Create a Version instance from a string in the format "major.minor.patch"
    static Result<Version> fromString(StringView versionStr);

    // Create a Version instance from a packed 64-bit integer representation
    static Result<Version> fromPacked(uint64 packedVersion);

    ////// String representation --------------------------------------------------

    // Validate a string representation of a version, returning a boolean result
    static bool isValidVersionString(StringView versionStr) noexcept;

    // Validate a string representation of a version, returning a Result containing an Error if invalid
    static Result<void, Error> validateVersionString(StringView versionStr) noexcept;

    // Returns a string representation of the version in the format "major.minor.patch"
    String toString() const;

    // Returns a full string representation of the version, 
    // including pre-release or build metadata if applicable
    String toFullString() const;

    ////// Conversion methods --------------------------------------------------

    // Converts to packed 64-bit integer representation (e.g., for efficient storage or comparison)
    constexpr uint64 toPacked() const noexcept {
        return (static_cast<uint64>(major) << 32) | (static_cast<uint64>(minor) << 16) | patch;
    }

    ////// Utility methods --------------------------------------------------

    // Checks if the version is valid (as defined by semantic versioning rules)
    bool isValid() const noexcept;

    // Checks if the version is in the zero phase (major version is 0, indicating initial development)
    constexpr bool isInZeroPhase() const noexcept {
        return major == 0;
    }

    // Checks if the version is a pre-release version (has a non-empty pre-release identifier)
    constexpr bool isPreRelease() const noexcept {
        return !preRelease.empty();
    }
};

NEX_CORE_NAMESPACE_END