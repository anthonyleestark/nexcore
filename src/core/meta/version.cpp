/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/base/linear.h"
#include "nex/core/text/string.h"
#include "nex/core/text/string_view.h"
#include "nex/core/text/encoding.h"
#include "nex/core/text/format.h"
#include "nex/core/text/text.h"
#include "nex/core/meta/version.h"

NEX_CORE_NAMESPACE_BEGIN

// =================================================================================
// Internal helper functions for Version class
// =================================================================================

namespace {

    // Check if a version number string has leading zeros (e.g., "01", "002")
    bool hasLeadingZero(StringView str) {
        return !str.empty() && str.front() == u'0' && str.size() > 1;
    }

    // Validate a version number string (e.g., "1", "2", "3") to ensure it is a valid unsigned integer
    bool isValidVersionNumber(StringView numberStr) {
        if (numberStr.empty()) return false;
        for (char16 ch : numberStr) {
            if (!encoding::isAsciiDigit(ch)) {
                return false; // Non-digit character found
            }
        }
        return true;
    }

    // Validate pre-release and build metadata identifiers (e.g., "alpha", "beta", "rc.1", "build.1234")
    bool doValidateIdentifier(StringView str, bool allowLeadingZeros) {
        // Check for empty string or empty segments (e.g., "rc..1")
        if (str.empty() || str.front() == u'.' || str.back() == u'.' || str.find(u"..") != StringView::npos) {
            return false;
        }

        // Split by '.' and check each segment:
        // Logic: segments must be [0-9A-Za-z-]. If numeric, no leading zero unless allowLeadingZeros is true
        usize start = 0;
        while (start < str.size()) {
            usize dotPos = str.find(u'.', start);
            if (dotPos == StringView::npos) dotPos = str.size();
            auto segment = str.substr(start, dotPos - start);
            // Check each character in the segment
            for (char16 ch : segment) {
                if (!encoding::isAsciiAlpha(ch) && !encoding::isAsciiDigit(ch) && ch != u'-') {
                    return false; // Invalid character in segment
                }
            }
            // Check for numeric segment with leading zero
            if (!allowLeadingZeros && !segment.empty() && encoding::isAsciiDigit(segment.front())) {
                bool isNumeric = true;
                for (char16 ch : segment) {
                    if (!encoding::isAsciiDigit(ch)) {
                        isNumeric = false;
                        break;
                    }
                }
                if (isNumeric && segment.size() > 1 && segment.front() == u'0') {
                    return false; // Leading zero in numeric segment
                }
            }
            start = dotPos + 1;
        }
        return true;
    }

    // Basic validation for version string format (e.g., "1.2.3", "1.2.3-alpha", "1.2.3+build")
    Result<Version> doValidateVersionString(StringView versionStr) {
        //// 0. Check for empty string
        if (versionStr.empty()) {
            return Result<Version>::error({
                ErrorCode::InvalidFormat, "Version string is empty"
            });
        }

        //// 1. Separate Build Metadata (+)
        String build, pre, core;
        usize plusPos = versionStr.find(u'+');
        if (plusPos != StringView::npos) {
            build = versionStr.substr(plusPos + 1).toString();
            versionStr = versionStr.substr(0, plusPos); // Remaining is core + pre-release
        }

        //// 2. Separate Pre-release (-)
        usize dashPos = versionStr.find(u'-');
        if (dashPos != StringView::npos) {
            pre = versionStr.substr(dashPos + 1).toString();
            core = versionStr.substr(0, dashPos).toString();
        } else {
            core = versionStr.toString();
        }

        //// 3. Validate Core (X.Y.Z)

        // Count dots and ensure exactly 3 segments
        int dots = 0;
        for (char c : core) {
            if (c == '.') dots++;
        }
        if (dots != 2) {
            return Result<Version>::error({
                ErrorCode::InvalidFormat, "Invalid core version format"
            });
        }

        // Split core into major, minor, patch and validate each
        auto coreParts = text::split(core, u'.');
        if (coreParts.size() != 3) {
            return Result<Version>::error({
                ErrorCode::InvalidFormat, "Core version must have three parts"
            });
        }
        for (const auto& part : coreParts) {
            if (!isValidVersionNumber(part)) {
                return Result<Version>::error({
                    ErrorCode::InvalidFormat, "Version number parts must be valid unsigned integers"
                });
            }
            if (hasLeadingZero(part)) {
                return Result<Version>::error({
                    ErrorCode::InvalidFormat, "Version number parts must not have leading zeros"
                });
            }
        }

        //// 4. Validate Pre-release and Build Metadata
        if (!pre.empty() && !doValidateIdentifier(pre, false)) {
            return Result<Version>::error({
                ErrorCode::InvalidFormat, "Invalid pre-release identifier format"
            });
        }
        if (!build.empty() && !doValidateIdentifier(build, true)) {
            return Result<Version>::error({
                ErrorCode::InvalidFormat, "Invalid build metadata identifier format"
            });
        }

        // If we reach here, the version string is valid, 
        // but we still need to parse the core version numbers
        Version version{};
        version.preRelease = pre;
        version.buildMetadata = build;
        version.major = static_cast<uint16>(NEX_STD stoi(coreParts[0].toUtf8().valueOr("0")));
        version.minor = static_cast<uint16>(NEX_STD stoi(coreParts[1].toUtf8().valueOr("0")));
        version.patch = static_cast<uint16>(NEX_STD stoi(coreParts[2].toUtf8().valueOr("0")));

        //// 5. Final validation of the version instance (e.g., check for valid ranges)

        // A version is considered valid if it has at least one non-zero component or non-empty metadata
        // A version like 0.0.0 is still considered valid if it has at least a valid pre-release or build metadata
        if (version.major == 0 && version.minor == 0 && version.patch == 0 
            && version.preRelease.empty() && version.buildMetadata.empty()) {
            return Result<Version>::error({
                ErrorCode::InvalidFormat, "Version is all zeros and has no metadata"
            });
        }
        // A version is considered valid if major, minor, and patch are within the allowed range
        // (0 to 65535 for uint16) and pre-release/build metadata are properly formatted if present
        if (version.major > 0xFFFF || version.minor > 0xFFFF || version.patch > 0xFFFF) {
            return Result<Version>::error({
                ErrorCode::InvalidFormat, "Version numbers exceed maximum allowed values"
            });
        }

        // If we reach here, the version is valid, so we return it as a successful result
        return Result<Version>::ok(version);
    }

} // namespace

// =================================================================================
// Implementation of Version class methods
// =================================================================================

// Create a Version instance from a string in the format "major.minor.patch"
Result<Version> Version::fromString(StringView versionStr) {
    auto validationResult = doValidateVersionString(versionStr);
    if (validationResult.isOk()) {
        return Result<Version>::ok(validationResult.value());
    } else {
        return Result<Version>::error(validationResult.error());
    }
}

// Create a Version instance from a packed 64-bit integer representation
Result<Version> Version::fromPacked(uint64 packedVersion) {
    // Validate that the packed version does not exceed the maximum allowed values for major, minor, and patch
    if ((packedVersion >> 32) > 0xFFFF 
        || ((packedVersion >> 16) & 0xFFFF) > 0xFFFF 
        || (packedVersion & 0xFFFF) > 0xFFFF) {
        return Result<Version>::error({
            ErrorCode::InvalidFormat, "Packed version exceeds maximum allowed values"
        });
    }

    // Extract major, minor, and patch versions from the packed integer
    Version version;
    version.major = static_cast<uint16>((packedVersion >> 32) & 0xFFFF);
    version.minor = static_cast<uint16>((packedVersion >> 16) & 0xFFFF);
    version.patch = static_cast<uint16>(packedVersion & 0xFFFF);
    return Result<Version>::ok(version);
}

// Validate a string representation of a version, returning a boolean result
bool Version::isValidVersionString(StringView versionStr) noexcept {
    return validateVersionString(versionStr).isOk();
}

// Validate a string representation of a version, returning a Result containing an Error if invalid
Result<void> Version::validateVersionString(StringView versionStr) noexcept {
    auto result = validateVersionString(versionStr);
    if (result.isOk()) {
        return Result<void>::ok();
    } else {
        return Result<void>::error(result.error());
    }
}

// Returns a string representation of the version in the format "major.minor.patch"
String Version::toString() const {
    return format::format(u"{}.{}.{}", major, minor, patch);
}

// Returns a full string representation of the version, including pre-release or build metadata if applicable
String Version::toFullString() const {
    String versionStr = toString();
    if (!preRelease.empty()) {
        versionStr.append(u"-").append(preRelease);
    }
    if (!buildMetadata.empty()) {
        versionStr.append(u"+").append(buildMetadata);
    }
    return versionStr;
}

// Checks if the version is valid (as defined by semantic versioning rules)
bool Version::isValid() const {
    // A version is considered valid if it has at least one non-zero component or non-empty metadata
    // A version like 0.0.0 is still considered valid if it has at least a valid pre-release or build metadata
    if (major == 0 && minor == 0 && patch == 0 && preRelease.empty() && buildMetadata.empty()) {
        return false; // Version is all zeros and has no metadata, considered invalid
    }
    // A version is considered valid if major, minor, and patch are within the allowed range
    // (0 to 65535 for uint16) and pre-release/build metadata are properly formatted if present
    if (major > 0xFFFF || minor > 0xFFFF || patch > 0xFFFF) {
        return false; // Version numbers exceed maximum allowed values
    }
    if (!preRelease.empty() && !doValidateIdentifier(preRelease, false)) {
        return false; // Invalid pre-release identifier
    }
    // Build metadata allows leading zeros, so we pass 'true' to allow them
    if (!buildMetadata.empty() && !doValidateIdentifier(buildMetadata, true)) {
        return false; // Invalid build metadata identifier
    }
    return true;
}

NEX_CORE_NAMESPACE_END