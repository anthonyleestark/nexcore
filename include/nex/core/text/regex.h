/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/linear.h"
#include "nex/core/text/string.h"
#include "nex/core/text/string_view.h"
#include "nex/base/wrappers.h"

NEX_NAMESPACE_BEGIN

/**
 * @struct  RegexMatch
 * @brief   Represents the result of a regular expression match operation, including details about the match
 *          such as the success status, position, length, matched string, and capture groups.
 * 
 * @details
 * The `RegexMatch` struct is designed to encapsulate the result of a regular expression match operation, providing
 * comprehensive information about the match. A regex match operation typically involves searching a string for 
 * a pattern defined by a regular expression, and if a match is found, the `RegexMatch` struct can be used to store 
 * details about the match.
 */
struct RegexMatch {
    bool success = false;                   // Whether the regex match was successful or not
    usize position = 0;                     // Starting position of the match (UTF-16 code units)
    usize length = 0;                       // Length of the match (UTF-16 code units)
    String matched;                         // Full match (entire matched string)
    Vec<String> groups;                     // groups[0] = full match, groups[1..] = capture groups

    ////// Optional fields for more detailed match information -----------------------

    usize prefixLength = 0;                 // Length of the prefix before the match (from the start of the string)
    usize suffixLength = 0;                 // Length of the suffix after the match (to the end of the string)
    
    // Constructors
    RegexMatch() = default;
    explicit RegexMatch(bool ok) : success(ok) {}
};

/**
 * @enum    RegexOptions
 * @brief   Flags to specify options for regular expression matching.
 * 
 * @details
 * The `RegexOptions` enum defines various flags that can be used to modify the behavior of regular expression 
 * matching. These options can be combined using bitwise operations to enable multiple behaviors at once.
 * 
 * @note
 * The actual implementation of regular expression matching using these options is not provided in this header, and
 * would depend on the underlying regex library being used (e.g., std::regex, PCRE, etc.). The options defined here 
 * are common flags that are typically supported by regex libraries, but their availability and behavior may vary.
 */
enum class RegexOptions : uint32 {
    None            = 0,                    // No special options; use default regex behavior

    IgnoreCase      = 1 << 0,               // i  - ignore case when matching (case-insensitive)
    Multiline       = 1 << 1,               // m  - ^ and $ match the start/end of each line
    DotAll          = 1 << 2,               // s  - Dot (.) matches newline characters (\n, \r, ...)
    Unicode         = 1 << 3,               // u  - Enable full Unicode mode (important for UTF-16)
    CaseInsensitive = IgnoreCase,           // ignore case (alias for convenience)
    Optimize        = 1 << 4,               // Optimize for speed (if supported by the library)
    NoSubs          = 1 << 5,               // No capture groups (faster if groups are not needed)
    ECMAScript      = 1 << 6,               // Force ECMAScript syntax (similar to JavaScript regex)
};

// Enable bitwise OR operator for RegexOptions to allow combining flags
inline constexpr RegexOptions operator|(RegexOptions a, RegexOptions b) {
    return static_cast<RegexOptions>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

/**
 * @class   Regex
 * @brief   Represents a compiled regular expression pattern and provides methods for matching 
 *          and replacing text based on that pattern.
 * 
 * @details
 * The `Regex` class encapsulates a regular expression pattern and provides various methods for performing regex
 * operations such as searching, matching, and replacing text. The class is designed to be flexible and efficient, 
 * allowing users to specify regex options to modify the behavior of the matching operations.
 * 
 * @note
 * The actual implementation of the regex engine is not provided in this header, and would depend on the underlying 
 * regex library being used (e.g., CTRE, std::regex, PCRE, etc.). The methods defined in this class are intended to 
 * provide a high-level interface for regex operations, while the internal implementation would handle the specifics 
 * of compiling the regex pattern and performing the matching operations based on the specified options
 * 
 * @see RegexOptions enum for available options that can be used to modify the behavior of regex matching.
 * @see RegexMatch struct for details about the result of regex match operations, including success status, position,
 *      length, matched string, and capture groups.
 */
class NEX_EXPORT Regex {
public:
    // Constructor
    explicit Regex(StringView pattern, RegexOptions options = RegexOptions::None);

    // Destructor
    NEX_DECLARE_DEFAULT_DTOR(Regex);

    // Disable copy operations to prevent accidental copying of the regex object, 
    // which may be expensive to copy due to internal compiled state or resources.
    NEX_DISALLOW_COPY(Regex);

    // Enable move operations for efficient transfer of regex objects without copying internal resources.
    NEX_DECLARE_DEFAULT_MOVE(Regex);

    ////// Regular expression matching operations -----------------------

    // Search for the first match of the regex in the given text 
    // and return detailed match information
    RegexMatch search(StringView text) const;

    // Check if the regex fully matches the given text
    RegexMatch match(StringView text) const;

    // Quick check if the regex matches any part of the given text (returns true/false)
    bool contains(StringView text) const;

    // Quick check if the regex fully matches the given text (returns true/false)
    bool matches(StringView text) const;

    // Search for all matches of the regex in the given text and return a list of 
    // detailed match information for each match
    Vec<RegexMatch> searchAll(StringView text) const;

    ////// Replace operations -----------------------

    // Replace the first match of the regex in the given text with a replacement string
    String replace(StringView text, StringView replacement) const;

    // Replace the first match of the regex in the given text using a replacement function
    String replace(StringView text, const Function<String(const RegexMatch&)>& replacementFunc) const;

    ////// Accessors -----------------------

    // Get the original regex pattern as a string view
    StringView pattern() const noexcept;

    // Get the regex options used for this regex
    RegexOptions options() const noexcept;

    // Get the number of capture groups in the regex (not including the full match group)
    usize groupCount() const noexcept;

private:
    // Private implementation details (Pimpl idiom)
    struct Impl;
    UniquePtr<Impl> impl_;
};

NEX_NAMESPACE_END
