/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <string>
#include <string_view>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/string.h"
#include "nex/base/result.h"
#include "nex/base/error.h"

NEX_NAMESPACE_BEGIN

/**
 * @namespace encoding
 * @brief     Encoding conversion utilities
 * 
 * This namespace provides utility functions for encoding conversion and character classification, particularly 
 * for UTF-16 encoding which is used internally in the `String` class. It includes functions to check for surrogate 
 * characters, whitespace characters, and to perform conversions between different encodings (e.g., UTF-8 to UTF-16 
 * and vice versa). These utilities are essential for handling Unicode strings correctly and efficiently in the NEX 
 * system, especially when dealing with internationalized text that may contain various types of characters and encodings.
 * 
 * @note
 * The actual implementation of these functions is provided in the corresponding source file (encoding.cpp),
 * and may involve platform-specific APIs or standard library utilities for encoding conversion.
 */
NEX_SUBNAMESPACE_BEGIN(encoding)

////// Character classification functions --------------------------------------------------

/**
 * @brief Check if a character is a high surrogate character (used in UTF-16 encoding)
 * 
 * @details
 * A high surrogate is a UTF-16 code unit in the range 0xD800 to 0xDBFF,
 * which is used as the first code unit in a surrogate pair to represent code points above U+FFFF.
 * 
 * @param codeUnit The UTF-16 code unit to check
 * @return `true` if the code unit is a high surrogate, `false` otherwise
 */
NEX_EXPORT NEX_NODISCARD
bool isHighSurrogate(char16 codeUnit) noexcept;

/**
 * @brief Check if a character is a low surrogate character (used in UTF-16 encoding)
 * 
 * @details
 * A low surrogate is a UTF-16 code unit in the range 0xDC00 to 0xDFFF,
 * which is used as the second code unit in a surrogate pair to represent code points above U+FFFF.
 * 
 * @param codeUnit The UTF-16 code unit to check
 * @return `true` if the code unit is a low surrogate, `false` otherwise
 */
NEX_EXPORT NEX_NODISCARD
bool isLowSurrogate(char16 codeUnit) noexcept;

/**
 * @brief Check if a character is a surrogate character (either high or low)
 * 
 * @details
 * A surrogate character is any UTF-16 code unit that falls within the ranges of high surrogates (0xD800 to 0xDBFF) 
 * or low surrogates (0xDC00 to 0xDFFF). This function checks if the given code unit is either a high surrogate 
 * or a low surrogate, which indicates that it is part of a surrogate pair used to represent code points above U+FFFF.
 * 
 * @param codeUnit The UTF-16 code unit to check
 * @return `true` if the code unit is a surrogate character, `false` otherwise
 */
NEX_EXPORT NEX_NODISCARD
bool isSurrogate(char16 codeUnit) noexcept {
    return isHighSurrogate(codeUnit) || isLowSurrogate(codeUnit);
}

/**
 * @brief Check if a character is a Unicode whitespace character based on its code point
 * @param codePoint The Unicode code point of the character to check
 * @return `true` if the character is a Unicode whitespace character, `false` otherwise
 * 
 * @note
 * This function checks if the given Unicode code point corresponds to a whitespace character in the Unicode standard. 
 * It can be used to determine if a character should be treated as a separator or ignored when processing strings, 
 * especially when dealing with internationalized text that may contain various types of whitespace characters beyond 
 * the basic ASCII set. 
 * The implementation may involve checking the code point against known ranges of Unicode whitespace characters or 
 * using Unicode properties to identify them.
 */
NEX_EXPORT NEX_NODISCARD
bool isUnicodeWhitespace(uint32 codePoint) noexcept;

/**
 * @brief Check if a UTF-32 code point is valid
 * 
 * @details
 * A valid UTF-32 code point must be in the range U+0000 to U+10FFFF, and must not be in the surrogate range 
 * (U+D800 to U+DFFF). This function checks if the given code point is a valid Unicode code point that can be 
 * represented in UTF-32 encoding.
 * 
 * @param codePoint The UTF-32 code point to check
 * @return true if the code point is valid, false otherwise
 */
NEX_EXPORT NEX_NODISCARD
bool isValidCodePoint(char32 codePoint) noexcept;

/**
 * @brief Check if a character is a whitespace character (e.g., space, tab, newline)
 * @param ch The character to check
 * @return `true` if the character is a whitespace character, `false` otherwise
 * 
 * @note
 * This function checks if the given character is a whitespace character, which includes spaces, tabs, newlines, 
 * and other characters that are considered whitespace in Unicode. It can be used to determine if a character 
 * should be treated as a separator or ignored when processing strings, such as when trimming or normalizing spaces. 
 * The implementation may use standard library functions or custom logic to identify whitespace characters based 
 * on their Unicode properties.
 */
NEX_EXPORT NEX_NODISCARD
bool isWhitespace(char16 ch) noexcept;

/**
 * @brief Check if a character is an ASCII digit (0-9)
 * @param ch The character to check
 * @return `true` if the character is an ASCII digit, `false` otherwise
 */
NEX_EXPORT NEX_NODISCARD
constexpr bool isAsciiDigit(char16 ch) noexcept {
    return ch >= u'0' && ch <= u'9';
}

/**
 * @brief Check if a character is an ASCII alphabetic character (A-Z, a-z)
 * @param ch The character to check
 * @return `true` if the character is an ASCII alphabetic character, `false` otherwise
 */
NEX_EXPORT NEX_NODISCARD
constexpr bool isAsciiAlpha(char16 ch) noexcept {
    return (ch >= u'A' && ch <= u'Z') || (ch >= u'a' && ch <= u'z');
}

/**
 * @brief Check if a character is a digit character (for any Unicode digit)
 * @param ch The character to check
 * @return `true` if the character is a digit character, `false` otherwise
 */
NEX_EXPORT NEX_NODISCARD
bool isUnicodeDigit(char16 ch) noexcept;

/**
 * @brief Check if a character is an alphabetic character (for any Unicode letter)
 * @param ch The character to check
 * @return `true` if the character is an alphabetic character, `false` otherwise
 */
NEX_EXPORT NEX_NODISCARD
bool isUnicodeAlpha(char16 ch) noexcept;

////// UTF-8 encoding utility functions --------------------------------------------------

/**
 * @brief Check if a UTF-8 string contains invalid sequences
 *
 * This function checks if the given UTF-8 string contains any invalid sequences.
 * It returns true if any invalid sequences are found, or false if the string is valid.
 * 
 * @param input The UTF-8 string to check
 * @return true if the input string contains invalid UTF-8 sequences, false otherwise
 */
NEX_EXPORT NEX_NODISCARD 
bool containsInvalidUtf8Sequences(Utf8StringView input) noexcept;

/** 
 * @brief Check if a UTF-8 string is valid
 * 
 * A valid UTF-8 string must consist of valid UTF-8 sequences 
 * and must not contain any invalid byte patterns.
 * This function performs a comprehensive validation of the entire string,
 * ensuring that all sequences are well-formed and that there are no invalid bytes.
 * 
 * @param input The UTF-8 string to validate
 * @return true if the input string is a valid UTF-8 string, false otherwise
 */
NEX_EXPORT NEX_NODISCARD 
bool isValidUtf8(Utf8StringView input) noexcept;

/** 
 * @brief Check if a UTF-8 sequence is valid
 * 
 * A valid UTF-8 sequence must start with a valid leading byte, 
 * followed by the correct number of valid continuation bytes.
 * This function checks the validity of a single UTF-8 sequence starting at the given input position, 
 * and returns true if the sequence is well-formed according to UTF-8 encoding rules.
 * 
 * @param input The UTF-8 string containing the sequence to validate
 * @return true if the sequence at the start of the input is a valid UTF-8 sequence, false otherwise
 */
NEX_EXPORT NEX_NODISCARD 
bool isValidUtf8Sequence(Utf8StringView input) noexcept;

/** 
 * @brief Decode a UTF-8 code point
 * 
 * This function decodes a single UTF-8 code point from the input string,
 * updating the advance parameter to indicate the number of bytes consumed.
 * The function returns a Result containing the decoded Unicode code point (char32) if successful,
 * or an error if the input sequence is invalid or incomplete.
 * 
 * @param input The UTF-8 string containing the code point to decode
 * @param advance A reference to a variable that will be updated with the number of bytes consumed
 * @return A Result containing the decoded Unicode code point (char32) if successful, 
 *         or an error if the input sequence is invalid or incomplete
 */
NEX_EXPORT NEX_NODISCARD 
Result<char32> decodeUtf8CodePoint(Utf8StringView input, usize& advance);

/** 
 * @brief Get the byte number of a UTF-8 code point
 * 
 * This function determines the number of bytes that make up 
 * a UTF-8 code point based on the value of the first byte.
 * The first byte of a UTF-8 sequence indicates the length of the sequence:
 *
 * - 0xxxxxxx: 1 byte (ASCII)
 * - 110xxxxx: 2 bytes
 * - 1110xxxx: 3 bytes
 * - 11110xxx: 4 bytes
 *
 * If the first byte does not match any valid UTF-8 leading byte pattern, 
 * the function returns 0 to indicate an invalid sequence.
 * This function is essential for correctly parsing UTF-8 encoded strings, 
 * as it allows the caller to determine how many bytes to read for each code point.
 * 
 * @param firstByte The first byte of the UTF-8 sequence
 * @return The number of bytes in the UTF-8 code point, or 0 if the first byte is invalid
 * 
 * @note This function does not perform full validation of the sequence; 
 *       it only checks the leading byte to determine the expected length.
 *       For comprehensive validation, use isValidSequence() or isValid() instead.
 */
NEX_EXPORT NEX_NODISCARD 
usize utf8SequenceLength(char8 firstByte);

/** 
 * @brief Encode a UTF-8 code point
 *
 * This function encodes a single Unicode code point (char32) into a UTF-8 sequence.
 * The encoded bytes are written to the provided output buffer.
 * The function returns the number of bytes written, or 0 if the code point is invalid.
 * 
 * @param cp The Unicode code point to encode
 * @param out A pointer to a buffer where the encoded UTF-8 bytes will be written
 * @return The number of bytes written to the output buffer, or 0 if the code point is invalid
 */
NEX_EXPORT NEX_NODISCARD 
usize encodeUtf8CodePoint(char32 cp, char8* out);

/**
 * @brief Count the number of Unicode code points in a UTF-8 string
 *
 * This function counts the number of Unicode code points in the given UTF-8 string.
 * It iterates through the string, decoding each code point and incrementing the count.
 * The function returns the total number of code points, or 0 if the string contains invalid sequences.
 * 
 * @param input The UTF-8 string to analyze
 * @return A Result containing the number of Unicode code points in the input string, 
 *         or an error if the string contains invalid sequences
 */
NEX_EXPORT NEX_NODISCARD
Result<usize> countUtf8CodePoints(Utf8StringView input) noexcept;

////// UTF-16 encoding utility functions --------------------------------------------------

/** 
 * @brief Decode a UTF-16 code point
 * 
 * This function decodes a single Unicode code point from the input UTF-16 string,
 * updating the advance parameter to indicate the number of code units consumed.
 * The function returns a Result containing the decoded Unicode code point (char32) if successful,
 * or an error if the input sequence is invalid or incomplete (e.g., unpaired surrogate).
 * 
 * @param input The UTF-16 string to decode from
 * @param advance A reference to a variable that will be updated with the number of code units consumed
 * @return A Result containing the decoded Unicode code point (char32) if successful, 
 *         or an error if the input sequence is invalid or incomplete (e.g., unpaired surrogate)
 */
NEX_EXPORT NEX_NODISCARD 
Result<char32> decodeUtf16CodePoint(Utf16StringView input, usize& advance);

/**
* @brief Encode a Unicode code point into UTF-16
* 
* This function encodes a single Unicode code point (char32) into a UTF-16 sequence.
* The encoded code units are written to the provided output buffer.
* The function returns the number of code units written, or 0 if the code point is invalid.
* 
* @param cp The Unicode code point to encode
* @param out A pointer to a buffer where the encoded UTF-16 code units will be written
* @return The number of code units written to the output buffer, or 0 if the code point is invalid
*/
NEX_EXPORT NEX_NODISCARD
usize encodeUtf16CodePoint(char32 cp, char16* out);

/**
 * @brief Count the number of Unicode code points in a UTF-16 string
 * 
 * This function counts the number of Unicode code points in the input UTF-16 string,
 * correctly handling surrogate pairs to ensure that each pair is counted as a single code point.
 * The function returns the total number of code points, which may be less than the number of code units
 * if surrogate pairs are present.
 * 
 * @param input The UTF-16 string to count code points in
 * @return A Result containing the number of Unicode code points in the input string, 
 *         or an error if the string contains invalid sequences (e.g., unpaired surrogates)
 */
NEX_EXPORT NEX_NODISCARD
Result<usize> countUtf16CodePoints(Utf16StringView input) noexcept;

////// UTF-32 encoding utility functions --------------------------------------------------

/**
 * @brief Check if a UTF-32 string is valid
 * 
 * @details
 * A valid UTF-32 string must consist of valid UTF-32 code points, and must not contain any invalid code points 
 * (such as surrogates or out-of-range values). This function performs a comprehensive validation of the entire 
 * string, ensuring that all code points are well-formed according to Unicode rules.
 * 
 * @param input The UTF-32 string to validate
 * @return true if the input string is a valid UTF-32 string, false otherwise
 */
NEX_EXPORT NEX_NODISCARD
bool isValidUtf32Sequence(Utf32StringView input) noexcept;

/**
 * @brief Count the number of Unicode code points in a UTF-32 string
 * 
 * @details
 * This function counts the number of Unicode code points in the given UTF-32 string.
 * Since each char32 represents a single Unicode code point, this function simply returns the length of the string,
 * but it also performs validation to ensure that all code points are valid.
 * The function returns the total number of code points, or 0 if the string contains invalid code points.
 * 
 * @param input The UTF-32 string to analyze
 * @return A Result containing the number of Unicode code points in the input string, 
 *         or an error if the string contains invalid code points (e.g., surrogates or out-of-range values)
 */
NEX_EXPORT NEX_NODISCARD
Result<usize> countUtf32CodePoints(Utf32StringView input) noexcept;

////// Encoding conversion functions --------------------------------------------------

/**
 * @brief Convert ANSI string to UTF-16 string
 * @param ansi The input ANSI string to convert
 * @return Result containing the converted UTF-16 string if successful, or an error if the conversion fails
 */
NEX_EXPORT NEX_NODISCARD 
Result<Utf16String> ansiToUtf16(Utf8StringView ansi);

/**
 * @brief Convert UTF-16 string to ANSI string
 * @param utf16 The input UTF-16 string to convert
 * @return Result containing the converted ANSI string if successful, 
 *         or an error if the conversion fails
 */
NEX_EXPORT NEX_NODISCARD 
Result<Utf8String> utf16ToAnsi(Utf16StringView utf16);

/**
 * @brief Convert local string to UTF-16 string
 * @param local The input local string to convert
 * @return Result containing the converted UTF-16 string if successful, 
 *         or an error if the conversion fails
 */
NEX_EXPORT NEX_NODISCARD 
Result<Utf16String> localToUtf16(Utf8StringView local);

/**
 * @brief Convert UTF-16 string to local string
 * @param utf16 The input UTF-16 string to convert
 * @return Result containing the converted local string if successful, or an error if the conversion fails
 */
NEX_EXPORT NEX_NODISCARD 
Result<Utf8String> utf16ToLocal(Utf16StringView utf16);

/**
 * @brief Convert UTF-8 string to UTF-16 string
 * @param utf8 The input UTF-8 string to convert
 * @return Result containing the converted UTF-16 string if successful, or an error if the conversion fails
 */
NEX_EXPORT NEX_NODISCARD 
Result<Utf16String> utf8ToUtf16(Utf8StringView utf8);

/**
 * @brief Convert UTF-16 string to UTF-8 string
 * @param utf16 The input UTF-16 string to convert
 * @return Result containing the converted UTF-8 string if successful, or an error if the conversion fails
 */
NEX_EXPORT NEX_NODISCARD 
Result<Utf8String> utf16ToUtf8(Utf16StringView utf16);

/**
 * @brief Convert UTF-32 string to UTF-16 string
 * @param utf32 The input UTF-32 string to convert
 * @return Result containing the converted UTF-16 string if successful, or an error if the conversion fails
 */
NEX_EXPORT NEX_NODISCARD
Result<Utf16String> utf32ToUtf16(Utf32StringView utf32);

/**
 * @brief Convert UTF-16 string to UTF-32 string
 * @param utf16 The input UTF-16 string to convert
 * @return Result containing the converted UTF-32 string if successful, or an error if the conversion fails
 */
NEX_EXPORT NEX_NODISCARD
Result<Utf32String> utf16ToUtf32(Utf16StringView utf16);

NEX_SUBNAMESPACE_END(encoding)

NEX_NAMESPACE_END
