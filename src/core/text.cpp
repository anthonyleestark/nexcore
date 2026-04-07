/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "common/text.h"
#include "common/encoding.h"

NEXSUITE_NAMESPACE_BEGIN

// ========================================================
// Implementation of text manipulation functions
// ========================================================

// Check if a string is blank (empty or contains only whitespace characters)
bool text::isBlank(StringView str) noexcept {
    if (str.empty()) {
        return true; // An empty string is considered blank
    }
    for (char16 ch : str) {
        if (!encoding::isWhitespace(ch)) {
            return false; // Found a non-whitespace character, so it's not blank
        }
    }
    return true; // All characters are whitespace or the string is empty
}

// Check if a string is numeric (contains only digit characters)
bool text::isNumeric(StringView str) noexcept {
    if (str.empty()) {
        return false; // An empty string is not considered numeric
    }
    for (char16 ch : str) {
        if (!encoding::isUnicodeDigit(ch)) {
            return false; // Found a non-digit character, so it's not numeric
        }
    }
    return true; // All characters are digits
}

// Check if a string is alphabetic (contains only alphabetic characters)
bool text::isAlpha(StringView str) noexcept {
    if (str.empty()) {
        return false; // An empty string is not considered alphabetic

    }
    for (char16 ch : str) {
        if (!encoding::isUnicodeAlpha(ch)) {
            return false; // Found a non-alphabetic character, so it's not alphabetic
        }
    }
    return true; // All characters are alphabetic
}

// Split a string into a list of substrings based on a specified delimiter character
ArrayList<String> text::split(StringView str, char16 delimiter /* = u' ' */) {
    ArrayList<String> result;
    usize start = 0;
    usize end = str.find(delimiter);

    // Iterate through the string and split it based on the delimiter
    while (end != StringView::npos) {
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    // Add the last segment after the final delimiter
    if (start < str.size()) {
        result.emplace_back(str.substr(start));
    }

    return result;
}

// Join a list of strings into a single string with a specified delimiter character between them
String text::join(const ArrayList<String>& strings, char16 delimiter /* = u' ' */) {
    if (strings.empty()) {
        return String();
    }

    // Start with the first string and append the rest with the delimiter in between
    String result = strings[0];
    for (usize i = 1; i < strings.size(); ++i) {
        result += delimiter;
        result += strings[i];
    }

    return result;
}

// Truncate a string to a specified maximum length, optionally adding an ellipsis character
String text::truncate(StringView str, usize maxLength, char16 ellipsis /* = u'…' */) {
    if (str.size() <= maxLength) {
        return String(str); // No truncation needed
    }
    if (maxLength == 0) {
        return String(); // Return an empty string if maxLength is zero
    }
    if (maxLength == 1) {
        return String().append(ellipsis); // Return just the ellipsis if maxLength is one
    }
    // Truncate the string and add the ellipsis at the end
    return str.substr(0, maxLength - 1).toString() + ellipsis;
}

// Trim characters from both sides of the string
String text::trim(StringView str, char16 trimChar /* = u' ' */) noexcept {
    usize start = 0;
    usize end = str.size();

    // Trim from the left
    while (start < end && str[start] == trimChar) {
        ++start;
    }
    // Trim from the right
    while (end > start && str[end - 1] == trimChar) {
        --end;
    }

    // Return the trimmed substring as a new String
    return str.substr(start, end - start).toString();
}

// Trim characters from the left side of the string
String text::trimLeft(StringView str, char16 trimChar /* = u' ' */) noexcept {
    usize start = 0;
    usize end = str.size();

    // Trim from the left
    while (start < end && str[start] == trimChar) {
        ++start;
    }

    // Return the left-trimmed substring as a new String
    return str.substr(start, end - start).toString();
}

// Trim characters from the right side of the string
String text::trimRight(StringView str, char16 trimChar /* = u' ' */) noexcept {
    usize start = 0;
    usize end = str.size();

    // Trim from the right
    while (end > start && str[end - 1] == trimChar) {
        --end;
    }

    // Return the right-trimmed substring as a new String
    return str.substr(start, end - start).toString();
}

// Replace multiple consecutive whitespace characters in a string with a single space character
String text::squashSpaces(StringView str) {
    String result;
    result.reserve(str.size());

    bool inWhitespace = false; // Flag to track if we are currently in a sequence of whitespace characters

    for (char16 ch : str) {
        if (encoding::isWhitespace(ch)) {
            if (!inWhitespace) {
                result += u' ';         // Add a single space for the first whitespace character
                inWhitespace = true;    // We are now in a sequence of whitespace characters
            }
        } else {
            result += ch;           // Add non-whitespace characters unchanged
            inWhitespace = false;   // We are no longer in a sequence of whitespace characters
        }
    }

    return result;
}

// Normalize spaces in a string by trimming leading and trailing whitespace, replacing multiple consecutive 
// whitespace characters with a single space and other space-related characters such as tabs and newlines
String text::normalizeSpaces(StringView str) {
    // First, trim leading and trailing whitespace
    String trimmed = trim(str);

    // Then, replace multiple consecutive whitespace characters with a single space
    return squashSpaces(trimmed);
}

// Convert a string to lowercase
String text::toLower(StringView str) {
    String result;
    result.reserve(str.size());

    for (char16 ch : str) {
        // Convert uppercase ASCII letters to lowercase
        if (ch >= u'A' && ch <= u'Z') {
            result += (ch - u'A' + u'a');
        } else {
            result += ch; // Non-uppercase characters are added unchanged
        }
    }

    return result;
}

// Convert a string to uppercase
String text::toUpper(StringView str) {
    String result;
    result.reserve(str.size());

    for (char16 ch : str) {
        // Convert lowercase ASCII letters to uppercase
        if (ch >= u'a' && ch <= u'z') {
            result += (ch - u'a' + u'A');
        } else {
            result += ch; // Non-lowercase characters are added unchanged
        }
    }

    return result;
}

// Convert a string to title case (capitalize the first letter of each word)
String text::toTitleCase(StringView str) {
    String result;
    result.reserve(str.size());

    bool newWord = true;            // Flag to indicate the start of a new word

    for (char16 ch : str) {
        if (encoding::isWhitespace(ch)) {
            newWord = true;         // Next character will be the start of a new word
            result += ch;           // Add whitespace characters unchanged
        } else {
            if (newWord && ch >= u'a' && ch <= u'z') {
                result += (ch - u'a' + u'A');                   // Capitalize the first letter of the word
            } else if (!newWord && ch >= u'A' && ch <= u'Z') {
                result += (ch - u'A' + u'a');                   // Convert other letters to lowercase
            } else {
                result += ch;                                   // Non-alphabetic characters are added unchanged
            }
            newWord = false;                                    // We are now inside a word
        }
    }

    return result;
}

// Extract a substring from the left side of the string
String text::left(StringView str, usize count) {
    if (count >= str.size()) {
        return String(str); // Return the entire string if count exceeds its length
    }
    return str.substr(0, count).toString();
}

// Extract a substring from the middle of the string
String text::mid(StringView str, usize start, usize count) {
    if (start >= str.size()) {
        return String(); // Return an empty string if start index is out of bounds
    }
    if (count == StringView::npos || start + count > str.size()) {
        count = str.size() - start; // Adjust count to get the remaining substring if it exceeds the length
    }

    return str.substr(start, count).toString();
}

// Extract a substring from the right side of the string
String text::right(StringView str, usize count) {
    if (count >= str.size()) {
        return String(str); // Return the entire string if count exceeds its length
    }
    return str.substr(str.size() - count, count).toString();
}

// Extract a substring that is between two specified delimiters
String text::substringBetween(StringView str, StringView open, StringView close) {
    usize start = str.find(open);
    if (start == StringView::npos) {
        return String(); // Return an empty string if the opening delimiter is not found
    }
    start += open.size(); // Move past the opening delimiter

    usize end = str.find(close, start);
    if (end == StringView::npos) {
        return String(); // Return an empty string if the closing delimiter is not found
    }

    return str.substr(start, end - start).toString();
}

// Check if a string contains any of the characters from a given set
bool text::containsAny(StringView str, StringView characters) {
    for (char16 ch : str) {
        if (characters.find(ch) != StringView::npos) {
            return true; // Found a character from the set in the string
        }
    }
    return false; // No characters from the set were found in the string
}

// Check if a string contains a specified regular expression pattern
bool text::containsRegex(StringView str, StringView pattern, RegexOptions options /* = RegexOptions::None */) {
    // TODO: Implement later using a simple regex engine such as CTRE or a custom implementation
    return false;
}

// Search for a specified regular expression pattern in a string and return details of the match
RegexMatch text::regexSearch(StringView text, StringView pattern, RegexOptions options /* = RegexOptions::None */) {
    // TODO: Implement later using a simple regex engine such as CTRE or a custom implementation
    return RegexMatch();
}

// Check if a string fully matches a specified regular expression pattern
RegexMatch text::regexMatch(StringView str, StringView pattern, RegexOptions options /* = RegexOptions::None */) {
    // TODO: Implement later using a simple regex engine such as CTRE or a custom implementation
    return RegexMatch();
}

// Search for all occurrences of a specified regular expression pattern in a string.
ArrayList<RegexMatch> text::regexSearchAll(StringView text, StringView pattern, 
                                            RegexOptions options /* = RegexOptions::None */) {
    // TODO: Implement later using a simple regex engine such as CTRE or a custom implementation
    return ArrayList<RegexMatch>();
}

// Replace all occurrences of a specified regular expression pattern in a string with a replacement string.
String text::regexReplace(StringView text, StringView pattern, StringView replacement,
                          RegexOptions options /* = RegexOptions::None */) {
    // TODO: Implement later using a simple regex engine such as CTRE or a custom implementation
    return String();
}

// Count the number of occurrences of a specified character in a string
usize text::countOccurrences(StringView str, char16 character) {
    usize count = 0;
    for (char16 ch : str) {
        if (ch == character) {
            ++count; // Increment count for each occurrence of the character
        }
    }
    return count;
}

// Count the number of occurrences of a specified substring in a string
usize text::countOccurrences(StringView str, StringView substring) {
    if (substring.empty()) {
        return 0; // Return 0 if the substring is empty to avoid infinite counting
    }
    usize count = 0;
    usize start = 0;
    usize end = str.find(substring, start);
    while (end != StringView::npos) {
        ++count;
        start = end + substring.size();
        end = str.find(substring, start);
    }
    return count;
}

// Remove all occurrences of a specified character from a string
String text::erase(StringView str, char16 character) {
    String result;
    result.reserve(str.size());

    // Iterate through the string and build a new string 
    // without the specified character
    for (char16 ch : str) {
        if (ch != character) {
            result += ch;
        }
    }

    return result;
}

// Remove all occurrences of a specified substring from a string
String text::erase(StringView str, StringView substring) {
    String result;
    result.reserve(str.size());

    usize start = 0;
    usize end = str.find(substring);

    // Iterate through the string and build a new string
    // without the specified substring
    while (end != StringView::npos) {
        result += str.substr(start, end - start);   // Add the part before the substring
        start = end + substring.size();             // Move past the substring
        end = str.find(substring, start);           // Find the next occurrence
    }

    result += str.substr(start); // Add the remaining part of the string after the last substring

    return result;
}

// Replace all occurrences of a specified character in a string with another character
String text::replace(StringView str, char16 target, char16 replacement) {
    String result;
    result.reserve(str.size());

    // Iterate through the string and replace the target character
    for (char16 ch : str) {
        if (ch == target) {
            result += replacement; // Replace target with replacement
        } else {
            result += ch;          // Add other characters unchanged
        }
    }

    return result;
}

// Replace all occurrences of a specified substring in a string with another substring
String text::replace(StringView str, StringView target, StringView replacement) {
    String result;
    result.reserve(str.size());

    usize start = 0;
    usize end = str.find(target);

    // Iterate through the string and replace the target substring
    while (end != StringView::npos) {
        result += str.substr(start, end - start);   // Add the part before the target
        result += replacement;                      // Add the replacement substring
        start = end + target.size();                // Move past the target substring
        end = str.find(target, start);              // Find the next occurrence of the target
    }

    result += str.substr(start); // Add the remaining part of the string after the last target

    return result;
}

// Pad a string on the left side with a specified character until it reaches a specified total length
String text::leftPad(StringView str, usize totalLength, char16 paddingChar /* = u' ' */) noexcept {
    if (str.size() >= totalLength) {
        return String(str); // Return the original string if it's already long enough
    }
    String result;
    result.reserve(totalLength);

    // Calculate how many padding characters are needed
    usize paddingCount = totalLength - str.size();

    // Add the padding characters to the left
    for (usize i = 0; i < paddingCount; ++i) {
        result += paddingChar;
    }

    result += str; // Add the original string after the padding
    return result;
}

// Pad a string on the right side with a specified character until it reaches a specified total length
String text::rightPad(StringView str, usize totalLength, char16 paddingChar /* = u' ' */) noexcept {
    if (str.size() >= totalLength) {
        return String(str); // Return the original string if it's already long enough
    }
    String result;
    result.reserve(totalLength);

    result += str; // Add the original string first

    // Calculate how many padding characters are needed
    usize paddingCount = totalLength - str.size();

    // Add the padding characters to the right
    for (usize i = 0; i < paddingCount; ++i) {
        result += paddingChar;
    }

    return result;
}

// Obscure sensitive information in a string by replacing characters with a specified mask character
String text::mask(StringView str, usize start, usize end, char16 maskChar /* = u'*' */) noexcept {
    if (start >= str.size() || end > str.size() || start >= end) {
        return String(str); // Return the original string if the indices are invalid
    }
    String result;
    result.reserve(str.size());

    // Build the masked string
    for (usize i = 0; i < str.size(); ++i) {
        if (i >= start && i < end) {
            result += maskChar;   // Replace characters in the specified range with the mask character
        } else {
            result += str[i];     // Add other characters unchanged
        }
    }

    return result;
}

NEXSUITE_NAMESPACE_END