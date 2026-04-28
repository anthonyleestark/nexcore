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
#include "nex/base/result.h"
#include "nex/base/error.h"
#include "nex/core/text/regex.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @namespace   text
 * @brief       Common string manipulation utilities
 * 
 * This namespace provides a collection of utility functions for common string manipulation tasks, such as 
 * checking if a string is blank or numeric, splitting and joining strings, trimming whitespace, normalizing 
 * spaces, changing case, and extracting substrings. These functions are designed to work with the `String` 
 * and `StringView` types defined in the common library, and they provide convenient and efficient ways to 
 * perform common string operations that are frequently needed in various parts of the application.
 * 
 * @note
 * The actual implementation of these functions is provided in the corresponding source file (text.cpp), and they 
 * may involve iterating through the characters of the input strings and applying appropriate logic based on the 
 * specific operation being performed (e.g., checking character types, concatenating strings, etc.).
 */
namespace text {

    /**
     * @brief   Check if a string is blank (empty or contains only whitespace characters).
     * 
     * @details
     * The `isBlank` function takes a `StringView` as input and returns `true` if the string is empty or consists 
     * solely of whitespace characters (e.g., spaces, tabs, newlines). It iterates through the string and checks 
     * each character to determine if it is a whitespace character. If all characters are whitespace or the string 
     * is empty, the function returns `true`; otherwise, it returns `false`.
     * 
     * @param   str The input string to be checked.
     * @return  `true` if the string is blank (empty or only whitespace), `false` otherwise.
     */
    NEX_EXPORT NEX_NODISCARD
    bool isBlank(StringView str) noexcept;

    /**
     * @brief   Check if a string is numeric (contains only digit characters).
     * 
     * @details
     * The `isNumeric` function takes a `StringView` as input and returns `true` if the string consists entirely of 
     * digit characters (0-9). It iterates through the string and checks each character to determine if it is a 
     * digit. If all characters are digits and the string is not empty, the function returns `true`; otherwise, it 
     * returns `false`.
     * 
     * @param   str The input string to be checked.
     * @return  `true` if the string is numeric (only digit characters), `false` otherwise.
     */
    NEX_EXPORT NEX_NODISCARD
    bool isNumeric(StringView str) noexcept;

    /**
     * @brief   Check if a string is alphabetic (contains only alphabetic characters).
     * 
     * @details
     * The `isAlpha` function takes a `StringView` as input and returns `true` if the string consists entirely of 
     * alphabetic characters (e.g., A-Z, a-z). It iterates through the string and checks each character to determine 
     * if it is an alphabetic character. If all characters are alphabetic and the string is not empty, the function 
     * returns `true`; otherwise, it returns `false`.
     * 
     * @param   str The input string to be checked.
     * @return  `true` if the string is alphabetic (only alphabetic characters), `false` otherwise.
     */
    NEX_EXPORT NEX_NODISCARD
    bool isAlpha(StringView str) noexcept;

    /**
     * @brief   Split a string into a list of substrings based on a specified delimiter character.
     * 
     * @details
     * The `split` function takes a `StringView` as input and splits it into an `ArrayList` of `String` objects 
     * based on the specified delimiter character. It iterates through the input string, identifying segments 
     * separated by the delimiter, and adds each segment to the resulting list.
     * 
     * @param   str The input string to be split.
     * @param   delimiter The character used to split the string (default is space character).
     * @return  An ArrayList of String objects containing the substrings resulting from the split operation.
     */
    NEX_EXPORT NEX_NODISCARD
    ArrayList<String> split(StringView str, char16 delimiter = u' ');

    /**
     * @brief   Join a list of strings into a single string with a specified delimiter character between them.
     * 
     * @details
     * The `join` function takes an `ArrayList` of `String` objects and concatenates them into a single `String`, 
     * inserting the specified delimiter character between each pair of strings. It iterates through the list of 
     * strings, appending each string to the result and adding the delimiter character between them (except 
     * after the last string).
     * 
     * @param   strings The list of strings to be joined.
     * @param   delimiter The character used to separate the strings in the resulting string (default is space character).
     * @return  A single `String` containing all the input strings separated by the specified delimiter.
     */
    NEX_EXPORT NEX_NODISCARD
    String join(const ArrayList<String>& strings, char16 delimiter = u' ');

    /**
     * @brief   Truncate a string to a specified maximum length, optionally adding an ellipsis character.
     * 
     * @details
     * The `truncate` function shortens the input string to the specified maximum length. If the string exceeds
     * the maximum length, it is truncated and an optional ellipsis character is appended to indicate truncation.
     * 
     * @param   str The input string to be truncated.
     * @param   maxLength The maximum length of the resulting string.
     * @param   ellipsisChar The character to indicate truncation (default is '…').
     * @return  A new `String` truncated to the specified length with an optional ellipsis character.
     */
    NEX_EXPORT NEX_NODISCARD
    String truncate(StringView str, usize maxLength, char16 ellipsisChar = u'…');

    /**
     * @brief   Trim characters from both sides of the string.
     * 
     * @details
     * The `trim` function removes all occurrences of the specified trim character from both the beginning and 
     * the end of the input string. It iterates through the string from both ends, skipping over any characters 
     * that match the trim character, until it encounters a different character or reaches the end of the string. 
     * The resulting string contains only the characters between the leading and trailing trim characters.
     * 
     * @param   str The input string to be trimmed.
     * @param   trimChar The character to be trimmed from both sides of the string (default is space character).
     * @return  A new `String` with leading and trailing trim characters removed.
     */
    NEX_EXPORT NEX_NODISCARD
    String trim(StringView str, char16 trimChar = u' ') noexcept;

    /**
     * @brief   Trim characters from the left side of the string.
     * 
     * @details
     * The `trimLeft` function removes all occurrences of the specified trim character from the beginning of 
     * the input string. It iterates through the string from the start, skipping over any characters that match 
     * the trim character, until it encounters a different character or reaches the end of the string. The resulting 
     * string contains only the characters after the leading trim characters.
     * 
     * @param   str The input string to be trimmed.
     * @param   trimChar The character to be trimmed from the left side of the string (default is space character).
     * @return  A new `String` with leading trim characters removed.
     */
    NEX_EXPORT NEX_NODISCARD
    String trimLeft(StringView str, char16 trimChar = u' ') noexcept;

    /**
     * @brief   Trim characters from the right side of the string.
     * 
     * @details
     * The `trimRight` function removes all occurrences of the specified trim character from the end of the input string. 
     * It iterates through the string from the end, skipping over any characters that match the trim character, until it 
     * encounters a different character or reaches the beginning of the string. The resulting string contains only the 
     * characters before the trailing trim characters.
     * 
     * @param   str The input string to be trimmed.
     * @param   trimChar The character to be trimmed from the right side of the string (default is space character).
     * @return  A new `String` with trailing trim characters removed.
     */
    NEX_EXPORT NEX_NODISCARD
    String trimRight(StringView str, char16 trimChar = u' ') noexcept;

    /**
     * @brief  Replace multiple consecutive whitespace characters in a string with a single space character.
     * 
     * @details
     * The `squashSpaces` function takes a `StringView` as input and returns a new `String` where all consecutive
     * whitespace characters are replaced with a single space character. It iterates through each character in the
     * input string, checks if it is a whitespace character, and if so, ensures that only a single space is added
     * to the resulting string for consecutive whitespace characters.
     * 
     * @param   str The input string in which to squash spaces.
     * @return  A new `String` with consecutive whitespace characters replaced by a single space.
     */
    NEX_EXPORT NEX_NODISCARD
    String squashSpaces(StringView str);

    /**
    * @brief   Normalize spaces in a string by trimming leading and trailing whitespace, replacing multiple 
    *          consecutive whitespace characters with a single space and other space-related characters such as
    *          tabs and newlines.
    * 
    * @details
    * The `normalizeSpaces` function takes a `StringView` as input and returns a new `String` where all leading and
    * trailing whitespace characters have been removed, and all consecutive whitespace characters (including spaces,
    * tabs, newlines, etc.) have been replaced with a single space character. It combines the functionality of trimming 
    * and squashing spaces to produce a clean, normalized string with consistent spacing.
    * 
    * @param   str The input string to be normalized.
    * @return  A new `String` with normalized spaces (trimmed and squashed).
    */
    NEX_EXPORT NEX_NODISCARD
    String normalizeSpaces(StringView str);

    /**
     * @brief   Convert a string to lowercase.
     * 
     * @details
     * The `toLower` function takes a `StringView` as input and returns a new `String` where all uppercase characters 
     * have been converted to their lowercase equivalents. 
     * It iterates through each character in the input string, checks if it is an uppercase character, and if so, 
     * converts it to lowercase using the appropriate character conversion logic (e.g., based on Unicode case mapping).
     * 
     * @param   str The input string to be converted to lowercase.
     * @return  A new `String` with all characters converted to lowercase.
     */
    NEX_EXPORT NEX_NODISCARD
    String toLower(StringView str);

    /**
     * @brief   Convert a string to uppercase.
     * 
     * @details
     * The `toUpper` function takes a `StringView` as input and returns a new `String` where all lowercase characters 
     * have been converted to their uppercase equivalents. 
     * It iterates through each character in the input string, checks if it is a lowercase character, and if so, 
     * converts it to uppercase using the appropriate character conversion logic (e.g., based on Unicode case mapping).
     * 
     * @param   str The input string to be converted to uppercase.
     * @return  A new `String` with all characters converted to uppercase.
     */
    NEX_EXPORT NEX_NODISCARD
    String toUpper(StringView str);

    /**
     * @brief   Convert a string to title case (capitalize the first letter of each word).
     * 
     * @details
     * The `toTitleCase` function takes a `StringView` as input and returns a new `String` where the first letter 
     * of each word is capitalized. 
     * It iterates through each character in the input string, checks if it is the first character of a word, and if so, 
     * converts it to uppercase using the appropriate character conversion logic (e.g., based on Unicode case mapping). 
     * All other characters are converted to lowercase.
     * 
     * @param   str The input string to be converted to title case.
     * @return  A new `String` with the first letter of each word capitalized.
     */
    NEX_EXPORT NEX_NODISCARD
    String toTitleCase(StringView str);

    /**
     * @brief   Extract a substring from the left side of the string.
     * 
     * @details
     * The `left` function takes a `StringView` and a count of characters, and returns a new `String` containing 
     * the leftmost `count` characters of the input string. 
     * If the count exceeds the length of the string, the entire string is returned.
     * 
     * @param   str The input string from which to extract the left substring.
     * @param   count The number of characters to include in the left substring.
     * @return  A new `String` containing the leftmost `count` characters of the input string.
     */
    NEX_EXPORT NEX_NODISCARD
    String left(StringView str, usize count);

    /**
     * @brief   Extract a substring from the middle of the string.
     * 
     * @details
     * The `mid` function takes a `StringView`, a starting index, and a count of characters, and returns 
     * a new `String` containing the substring that starts at the specified index and includes the next 
     * `count` characters. 
     * If the starting index is out of bounds or if the count exceeds the remaining length of the string, 
     * an appropriate substring is returned based on the valid range.
     * 
     * @param   str The input string from which to extract the middle substring.
     * @param   start The starting index for the substring extraction.
     * @param   count The number of characters to include in the middle substring.
     * @return  A new `String` containing the specified middle substring of the input string.
     */
    NEX_EXPORT NEX_NODISCARD
    String mid(StringView str, usize start, usize count);

    /**
     * @brief   Extract a substring from the right side of the string.
     * 
     * @details
     * The `right` function takes a `StringView` and a count of characters, and returns a new `String` 
     * containing the rightmost `count` characters of the input string. 
     * If the count exceeds the length of the string, the entire string is returned.
     * 
     * @param   str The input string from which to extract the right substring.
     * @param   count The number of characters to include in the right substring.
     * @return  A new `String` containing the rightmost `count` characters of the input string.
     */
    NEX_EXPORT NEX_NODISCARD
    String right(StringView str, usize count);

    /**
     * @brief   Extract a substring that is between two specified delimiters.
     * 
     * @details
     * The `substringBetween` function takes a `StringView` and two delimiter strings (open and close), and returns 
     * a new `String` containing the substring that is found between the first occurrence of the open delimiter 
     * and the first occurrence of the close delimiter that follows it. If either delimiter is not found, or if the 
     * close delimiter does not follow the open delimiter, an empty string is returned.
     * 
     * @param   str The input string from which to extract the substring.
     * @param   open The string that marks the beginning of the substring to extract.
     * @param   close The string that marks the end of the substring to extract.
     * @return  A new `String` containing the substring between the specified delimiters, or an empty string if not found.
     */
    NEX_EXPORT NEX_NODISCARD
    String substringBetween(StringView str, StringView open, StringView close);

    /**
     * @brief   Check if a string contains any of the characters from a given set.
     * 
     * @details
     * The `containsAny` function takes a `StringView` and a set of characters, and returns `true` if
     * any of the characters from the set are found in the string, otherwise returns `false`.
     * 
     * @param   str The input string to check.
     * @param   chars The set of characters to look for in the string.
     * @return  `true` if any character from the set is found in the string, otherwise `false`.
     */
    NEX_EXPORT NEX_NODISCARD
    bool containsAny(StringView str, StringView chars);

    /**
     * @brief   Check if a string contains a specified regular expression pattern.
     * 
     * @details
     * The `containsRegex` function takes a `StringView` and a regular expression pattern, and returns `true` if
     * the string contains any substring that matches the pattern, otherwise returns `false`.
     * 
     * @param   str The input string to check for the regular expression pattern.
     * @param   pattern The regular expression pattern to search for in the string.
     * @param   options The regular expression options to use when matching the pattern.
     * @return  `true` if the string contains a substring that matches the regular expression pattern, 
     *          otherwise `false`.
     */
    NEX_EXPORT NEX_NODISCARD
    bool containsRegex(StringView str, StringView pattern, RegexOptions options = RegexOptions::None);

    /**
     * @brief   Search for a specified regular expression pattern in a string and return details of the match.
     * 
     * @details
     * The `regexSearch` function takes a `StringView` and a regular expression pattern, and returns 
     * a `RegexMatch` object containing the details of the first match found in the string. If no match 
     * is found, the `success` field of the returned `RegexMatch` will be `false`, and other fields may be 
     * default or empty.
     * 
     * @param   text The input string to search for the regular expression pattern.
     * @param   pattern The regular expression pattern to search for in the string.
     * @param   options The regular expression options to use when matching the pattern.
     * @return  A `RegexMatch` object containing the details of the first match found in the string, 
     *          or an unsuccessful match if not found.
     */
    RegexMatch regexSearch(StringView text, StringView pattern, RegexOptions options = RegexOptions::None);

    /**
     * @brief   Check if a string fully matches a specified regular expression pattern.
     * 
     * @details
     * The `matchesRegex` function takes a `StringView` and a regular expression pattern, and returns `true` if
     * the string fully matches the pattern, otherwise returns `false`. This means that the entire string must 
     * conform to the regular expression pattern for the function to return `true`.
     * It works similarly to `containsRegex`, but checking for a full match (^...$) instead of a partial match.
     * 
     * @param   str The input string to check against the regular expression pattern.
     * @param   pattern The regular expression pattern to match the string against.
     * @param   options The regular expression options to use when matching the pattern.
     * @return  A `RegexMatch` object containing the details of the match. If the match was not successful, 
     *          the `success` field will be `false` and other fields may be default or empty.
     */
    NEX_EXPORT NEX_NODISCARD
    RegexMatch regexMatch(StringView str, StringView pattern, RegexOptions options = RegexOptions::None);

    /**
     * @brief   Search for all occurrences of a specified regular expression pattern in a string.
     * 
     * @details
     * The `regexSearchAll` function takes a `StringView` and a regular expression pattern, and returns 
     * a list of `RegexMatch` objects containing the details of all matches found in the string. If no matches 
     * are found, the returned list will be empty.
     * 
     * @param   text The input string to search for the regular expression pattern.
     * @param   pattern The regular expression pattern to search for in the string.
     * @param   options The regular expression options to use when matching the pattern.
     * @return  A list of `RegexMatch` objects containing the details of all matches found in the string.
     */
    ArrayList<RegexMatch> regexSearchAll(StringView text, StringView pattern, 
                                         RegexOptions options = RegexOptions::None);

    /**
     * @brief   Split a string into a list of substrings based on a specified regular expression pattern.
     * 
     * @details
     * The `regexSplit` function takes a `StringView`, a regular expression pattern, and returns a list 
     * of `String` objects resulting from splitting the input string at each occurrence of the pattern. 
     * The regular expression pattern is used to identify the delimiters for splitting the string, 
     * and the resulting substrings are collected into a list. If the pattern is not found in the string, 
     * the entire string is returned as a single element in the list.
     *
     * @param   text The input string to be split.
     * @param   pattern The regular expression pattern used to split the string.
     * @param   options The regular expression options to use when matching the pattern.
     * @return  A list of `String` objects resulting from splitting the input string by the regular expression pattern.
     */
    ArrayList<String> regexSplit(StringView text, StringView pattern, RegexOptions options = RegexOptions::None);

    /**
     * @brief   Replace all occurrences of a specified regular expression pattern in a string 
     *          with a replacement string.
     * 
     * @details
     * The `regexReplace` function takes a `StringView`, a regular expression pattern, and a replacement string, 
     * and returns a new `String` where all occurrences of the pattern in the input string have been replaced 
     * with the replacement string. If no matches are found, the original string is returned unchanged.
     * 
     * @param   text The input string in which to replace occurrences of the regular expression pattern.
     * @param   pattern The regular expression pattern to search for in the string.
     * @param   replacement The string to replace each match of the pattern with.
     * @param   options The regular expression options to use when matching the pattern.
     * @return  A new `String` with all occurrences of the regular expression pattern replaced by the 
     *          replacement string.
     */
    String regexReplace(StringView text, StringView pattern, StringView replacement,
                        RegexOptions options = RegexOptions::None);

    /**
     * @brief   Count the number of occurrences of a specified character in a string.
     * 
     * @details
     * The `countOccurrences` function takes a `StringView` and a character, and returns the number of times
     * the specified character appears in the string.
     * 
     * @param   str The input string in which to count occurrences.
     * @param   character The character to count in the string.
     * @return  The number of occurrences of the specified character in the string.
     */
    NEX_EXPORT NEX_NODISCARD
    usize countOccurrences(StringView str, char16 character) noexcept;

    /**
     * @brief   Count the number of occurrences of a specified substring in a string.
     * 
     * @details
     * The `countOccurrences` function takes a `StringView` and a substring, and returns the number of times
     * the specified substring appears in the string.
     * 
     * @param   str The input string in which to count occurrences.
     * @param   substring The substring to count in the string.
     * @return  The number of occurrences of the specified substring in the string.
     */
    NEX_EXPORT NEX_NODISCARD
    usize countOccurrences(StringView str, StringView substring) noexcept;

    /**
    * @brief   Remove all occurrences of a specified character from a string.
    * 
    * @details
    * The `erase` function takes a `StringView` and a character to remove, and returns a new `String` with 
    * all occurrences of the specified character removed. 
    * It iterates through each character in the input string, checks if it matches the character to remove, 
    * and if not, appends it to the result string.
    * 
    * @param   str The input string from which to remove characters.
    * @param   charToRemove The character to be removed from the string.
    * @return  A new `String` with all occurrences of the specified character removed.
    */
    NEX_EXPORT NEX_NODISCARD
    String erase(StringView str, char16 charToRemove);

    /**
     * @brief   Remove all occurrences of a specified substring from a string.
     * 
     * @details
     * The `erase` function takes a `StringView` and a substring to remove, and returns a new `String` with 
     * all occurrences of the specified substring removed. 
     * It iterates through the input string, searching for occurrences of the target substring, and constructs 
     * the result string by skipping over any matches.
     * 
     * @param   str The input string from which to remove substrings.
     * @param   substring The substring to be removed from the string.
     * @return  A new `String` with all occurrences of the specified substring removed.
     */
    NEX_EXPORT NEX_NODISCARD
    String erase(StringView str, StringView substring);

    /**
     * @brief   Replace all occurrences of a specified character in a string with another character.
     * 
     * @details
     * The `replace` function takes a `StringView`, a target character to replace, and a replacement character, 
     * and returns a new `String` where all occurrences of the target character have been replaced with the 
     * replacement character. 
     * It iterates through each character in the input string, checks if it matches the target character, and 
     * if so, appends the replacement character to the result string; otherwise, it appends the original character.
     * 
     * @param   str The input string in which to replace characters.
     * @param   target The character to be replaced in the string.
     * @param   replacement The character to replace the target character with.
     * @return  A new `String` with all occurrences of the target character replaced by the replacement character.
     */
    NEX_EXPORT NEX_NODISCARD
    String replace(StringView str, char16 target, char16 replacement);

    /**
     * @brief   Replace all occurrences of a specified substring in a string with another substring.
     * 
     * @details
     * The `replace` function takes a `StringView`, a target substring to replace, and a replacement substring, 
     * and returns a new `String` where all occurrences of the target substring have been replaced with the 
     * replacement substring. 
     * It iterates through the input string, searching for occurrences of the target substring, and constructs 
     * the result string by replacing matches with the replacement substring.
     * 
     * @param   str The input string in which to replace substrings.
     * @param   target The substring to be replaced in the string.
     * @param   replacement The substring to replace the target substring with.
     * @return  A new `String` with all occurrences of the target substring replaced by the replacement substring.
     */
    NEX_EXPORT NEX_NODISCARD
    String replace(StringView str, StringView target, StringView replacement);

    /**
     * @brief   Pad a string on the left side with a specified character until it reaches a total length.
     * 
     * @details
     * The `leftPad` function takes a `StringView`, a total desired length, and an optional padding character, 
     * and returns a new `String` that is padded on the left side with the specified character until it reaches 
     * the total length. If the input string is already equal to or longer than the total length, it is returned 
     * unchanged.
     * 
     * @param   str The input string to be padded.
     * @param   totalLength The desired total length of the resulting string after padding.
     * @param   paddingChar The character to use for padding (default is space character).
     * @return  A new `String` padded on the left side to reach the specified total length.
     */
    NEX_EXPORT NEX_NODISCARD
    String leftPad(StringView str, usize totalLength, char16 paddingChar = u' ') noexcept;

    /**
     * @brief   Pad a string on the right side with a specified character until it reaches a total length.
     * 
     * @details
     * The `rightPad` function takes a `StringView`, a total desired length, and an optional padding character, 
     * and returns a new `String` that is padded on the right side with the specified character until it reaches 
     * the total length. If the input string is already equal to or longer than the total length, it is returned 
     * unchanged.
     * 
     * @param   str The input string to be padded.
     * @param   totalLength The desired total length of the resulting string after padding.
     * @param   paddingChar The character to use for padding (default is space character).
     * @return  A new `String` padded on the right side to reach the specified total length.
     */
    NEX_EXPORT NEX_NODISCARD
    String rightPad(StringView str, usize totalLength, char16 paddingChar = u' ') noexcept;

    /**
     * @brief  Obscure sensitive information in a string by replacing characters with a specified mask character.
     * 
     * @details
     * The `mask` function takes a `StringView`, a starting index, an ending index, and an optional mask character, 
     * and returns a new `String` where the characters between the specified indices are replaced with the mask character.
     * The function ensures that the start and end indices are within the bounds of the string and that the start index 
     * is less than the end index. If the indices are invalid, the original string is returned unchanged. 
     * This function can be used to obscure sensitive information in a string, such as masking a portion of an email address, 
     * password, or a credit card number.
     * 
     * @param   str The input string to be masked.
     * @param   start The starting index of the portion to be masked.
     * @param   end The ending index of the portion to be masked.
     * @param   maskChar The character to use for masking (default is '*').
     * @return  A new `String` with the specified portion masked.
     */
    NEX_EXPORT NEX_NODISCARD
    String mask(StringView str, usize start, usize end, char16 maskChar = u'*') noexcept;

} // namespace text

NEX_CORE_NAMESPACE_END