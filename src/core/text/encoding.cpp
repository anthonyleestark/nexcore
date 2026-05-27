/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/base/casts.h"
#include "nex/core/text/encoding.h"

/**
 * @note
 * For now, we will implement the encoding conversion functions using platform-specific APIs on Windows 
 * and basic encoding conversion algorithms on other platforms.
 * In the future, we may want to consider using a more robust and cross-platform library for encoding conversion, 
 * such as ICU, to handle more complex cases and edge cases in encoding conversion, as well as to provide better 
 * performance and reliability across different platforms. However, for the current scope of this project, the 
 * platform-specific implementations should suffice for basic encoding conversion needs.
 */

#if NEX_PLATFORM_IS_WINDOWS
    #include <windows.h>
#endif

NEX_NAMESPACE_BEGIN

// ========================================================================================
// Implementation of character classification functions
// ========================================================================================

// Check if a character is a high surrogate character (used in UTF-16 encoding)
bool encoding::isHighSurrogate(char16 codeUnit) noexcept {
    return codeUnit >= 0xD800 && codeUnit <= 0xDBFF;
}

// Check if a character is a low surrogate character (used in UTF-16 encoding)
bool encoding::isLowSurrogate(char16 codeUnit) noexcept {
    return codeUnit >= 0xDC00 && codeUnit <= 0xDFFF;
}

// Check if a character is a Unicode whitespace character based on its code point
bool encoding::isUnicodeWhitespace(uint32 cp) noexcept {
    // 1. ASCII & Control characters (0009 - 000D, 0020)
    if (cp < 0x00A0) {
        return (cp == 0x0020) || (cp >= 0x0009 && cp <= 0x000D);
    }

    // 2. Latin-1 & Unicode Separators
    switch (cp) {
        case 0x00A0:    // Non-breaking space
        case 0x1680:    // Ogham space mark
        case 0x2028:    // Line separator
        case 0x2029:    // Paragraph separator
        case 0x202F:    // Narrow no-break space
        case 0x205F:    // Medium mathematical space
        case 0x3000:    // Ideographic space (Chinese/Japanese/Korean space)
        case 0xFEFF:    // Zero width no-break space (BOM; also treated as whitespace in some contexts)
            return true;
    }

    // 3. En Quad -> Hair Space (Continuous range from 2000 to 200A)
    if (cp >= 0x2000 && cp <= 0x200A) {
        return true;
    }

    return false;
}

// Check if a character is a valid Unicode code point
bool encoding::isValidCodePoint(char32 codePoint) noexcept {
    // Valid code points are in the range U+0000 to U+10FFFF, and must not be in the surrogate range (U+D800 to U+DFFF)
    return codePoint <= 0x10FFFF && !(codePoint >= 0xD800 && codePoint <= 0xDFFF);
}

// Check if a character is a whitespace character (e.g., space, tab, newline)
// This function checks if the given character is a whitespace character, which includes spaces, tabs, newlines, 
// and other characters that are considered whitespace in Unicode
bool encoding::isWhitespace(char16 ch) noexcept {
    // Because all current whitespace characters are < 0xFFFF,
    // we can directly check the character against the Unicode whitespace characters 
    // without needing to convert to code point or worry about surrogate pairs.
    return isUnicodeWhitespace(static_cast<uint32>(ch));
}

// Check if a character is a digit character (for any Unicode digit)
bool encoding::isUnicodeDigit(char16 ch) noexcept {
    // Check for ASCII digits
    if (isAsciiDigit(ch)) return true;

    // Check for other Unicode digit characters
    // This is a simplified check and may not cover all Unicode digit characters
    return (ch >= u'\u0660' && ch <= u'\u0669') || // Arabic-Indic digits
           (ch >= u'\u06F0' && ch <= u'\u06F9') || // Extended Arabic-Indic digits
           (ch >= u'\u0966' && ch <= u'\u096F') || // Devanagari digits
           (ch >= u'\uFF10' && ch <= u'\uFF19');   // Fullwidth digits
}

// Check if a character is an alphabetic character (for any Unicode letter)
bool encoding::isUnicodeAlpha(char16 ch) noexcept {
    // Check for ASCII alphabetic characters
    if (isAsciiAlpha(ch)) return true;

    // Check for other Unicode alphabetic characters
    // This is a simplified check and may not cover all Unicode alphabetic characters
    if ((ch >= u'\u00C0' && ch <= u'\u00D6') ||   // Latin-1 uppercase letters
        (ch >= u'\u00D8' && ch <= u'\u00F6') ||   // Latin-1 letters
        (ch >= u'\u00F8' && ch <= u'\u00FF')) {   // Latin-1 lowercase letters
        return true;
    }

    // For languages with large character sets, such as Vietnamese, Chinese, Japanese, Korean, etc., 
    // we can use the OS-provided functions to check for alphabetic characters based on Unicode properties,
    // which will be more comprehensive and accurate than hardcoding specific ranges.
#if NEX_PLATFORM_IS_WINDOWS
    return iswalpha(static_cast<wint>(ch)) != 0;
#else
    // On non-Windows platforms, we can use the standard library function to check for alphabetic characters
    return NEX_STD iswalpha(static_cast<wint>(ch)) != 0;
#endif
}

// ========================================================================================
// Implementation of UTF-8 encoding utility functions
// ========================================================================================

// Check if a UTF-8 string contains invalid sequences
bool encoding::containsInvalidUtf8Sequences(Utf8StringView input) noexcept {
    const_char_ptr ptr = input.data();
    const_char_ptr end = ptr + input.size();

    while (ptr < end) {
        // Get the first byte to determine the length of the UTF-8 sequence
        uchar byte = static_cast<uchar>(*ptr);
        usize expectedLength = utf8SequenceLength(byte);

        if (expectedLength == 0 || ptr + expectedLength > end) {
            return true; // Invalid UTF-8 sequence found
        }

        // Validate continuation bytes
        for (usize i = 1; i < expectedLength; ++i) {
            if ((static_cast<uchar>(ptr[i]) & 0xC0) != 0x80) {
                return true; // Invalid continuation byte found
            }
        }

        ptr += expectedLength;
    }

    return false; // No invalid sequences found
}

// Check if a UTF-8 string is valid
bool encoding::isValidUtf8(Utf8StringView input) noexcept {
    return !containsInvalidUtf8Sequences(input);
}

// Check if a UTF-8 sequence is valid
bool encoding::isValidUtf8Sequence(Utf8StringView input) noexcept {
    if (input.empty()) {
        return false; // Empty input is not a valid UTF-8 sequence
    }

    // Get the first byte to determine the length of the UTF-8 sequence
    uchar firstByte = static_cast<uchar>(input[0]);
    usize expectedLength = utf8SequenceLength(firstByte);
    if (expectedLength == 0 || expectedLength > input.size()) {
        return false; // Invalid first byte or not enough bytes in the input
    }

    // Check continuation bytes
    for (usize i = 1; i < expectedLength; ++i) {
        if ((static_cast<uchar>(input[i]) & 0xC0) != 0x80) {
            return false; // Invalid continuation byte
        }
    }

    return true; // Valid UTF-8 sequence
}

// Decode a UTF-8 code point
Result<char32> encoding::decodeUtf8CodePoint(Utf8StringView input, usize& advance) {
    advance = 0;

    if (input.empty()) {
        return Result<char32>::error({ 
            ErrorCode::InvalidFormat, "Input string is empty" 
        });
    }

    // Get the first byte to determine the length of the UTF-8 sequence
    uchar b0 = static_cast<uchar>(input[0]);
    usize len = utf8SequenceLength(b0);

    if (len == 0 || len > input.size()) {
        return Result<char32>::error({ 
            ErrorCode::InvalidFormat, "Invalid lead byte" 
        });
    }
    
    char32 cp = 0;
    char32 min_cp = 0;

    if (len == 1) {
        cp = b0;
    } else {
        // First, validate the continuation bytes before decoding 
        // to prevent invalid memory access and ensure the sequence is well-formed.
        for (usize i = 1; i < len; ++i) {
            if ((static_cast<uchar>(input[i]) & 0xC0) != 0x80) {
                return Result<char32>::error({ 
                    ErrorCode::InvalidFormat, "Invalid continuation byte" 
                });
            }
        }

        // Decode the code point from the UTF-8 sequence based on the length determined by the first byte

        if (len == 2) {
            cp = ((b0 & 0x1F) << 6) | (static_cast<uchar>(input[1]) & 0x3F);
            min_cp = 0x80;
        } else if (len == 3) {
            cp = ((b0 & 0x0F) << 12) | ((static_cast<uchar>(input[1]) & 0x3F) << 6) 
                 | (static_cast<uchar>(input[2]) & 0x3F);
            min_cp = 0x800;
        } else if (len == 4) {
            cp = ((b0 & 0x07) << 18) | ((static_cast<uchar>(input[1]) & 0x3F) << 12) 
                 | ((static_cast<uchar>(input[2]) & 0x3F) << 6) | (static_cast<uchar>(input[3]) & 0x3F);
            min_cp = 0x10000;
        }
    }

    // --- IMPORTANT SECURITY CHECKS ---

    if (cp < min_cp) {
        return Result<char32>::error({ 
            ErrorCode::InvalidFormat, "Overlong encoding" 
        });
    }
    if (cp >= 0xD800 && cp <= 0xDFFF) {
        return Result<char32>::error({ 
            ErrorCode::InvalidFormat, "Surrogate range" 
        });
    }
    if (cp > 0x10FFFF) {
        return Result<char32>::error({ 
            ErrorCode::InvalidFormat, "Out of Unicode range" 
        });
    }

    // If we reach this point, the UTF-8 sequence is valid 
    // and we have successfully decoded the code point.
    advance = len;
    return Result<char32>::ok(cp);
}

// Get the byte number of a UTF-8 code point
usize encoding::utf8SequenceLength(char8 firstByte) {
    uchar byte = static_cast<uchar>(firstByte);
    if (byte <= 0x7F) {
        return 1; // ASCII character (1 byte)
    } else if ((byte & 0xE0) == 0xC0) {
        return 2; // 2-byte sequence
    } else if ((byte & 0xF0) == 0xE0) {
        return 3; // 3-byte sequence
    } else if ((byte & 0xF8) == 0xF0) {
        return 4; // 4-byte sequence
    }
    return 0; // Invalid first byte
}

// Encode a UTF-8 code point
usize encoding::encodeUtf8CodePoint(char32 cp, char8* out) {
    if (cp <= 0x7F) {
        out[0] = static_cast<char8>(cp);
        return 1; // ASCII character (1 byte)
    } else if (cp <= 0x7FF) {
        out[0] = static_cast<char8>(0xC0 | ((cp >> 6) & 0x1F));
        out[1] = static_cast<char8>(0x80 | (cp & 0x3F));
        return 2; // 2-byte sequence
    } else if (cp <= 0xFFFF) {
        out[0] = static_cast<char8>(0xE0 | ((cp >> 12) & 0x0F));
        out[1] = static_cast<char8>(0x80 | ((cp >> 6) & 0x3F));
        out[2] = static_cast<char8>(0x80 | (cp & 0x3F));
        return 3; // 3-byte sequence
    } else if (cp <= 0x10FFFF) {
        out[0] = static_cast<char8>(0xF0 | ((cp >> 18) & 0x07));
        out[1] = static_cast<char8>(0x80 | ((cp >> 12) & 0x3F));
        out[2] = static_cast<char8>(0x80 | ((cp >> 6) & 0x3F));
        out[3] = static_cast<char8>(0x80 | (cp & 0x3F));
        return 4; // 4-byte sequence
    }
    return 0; // Invalid code point
}

// Count the number of Unicode code points in a UTF-8 string
Result<usize> encoding::countUtf8CodePoints(Utf8StringView input) noexcept {
    usize count = 0;
    const_char_ptr ptr = input.data();
    const_char_ptr end = ptr + input.size();

    while (ptr < end) {
        // Get the first byte to determine the length of the UTF-8 sequence,
        // and validate the sequence before counting to ensure we don't count invalid sequences.
        uchar byte = static_cast<uchar>(*ptr);
        usize expectedLength = utf8SequenceLength(byte);
        if (expectedLength == 0 || ptr + expectedLength > end) {
            return Result<usize>::error({ 
                ErrorCode::InvalidFormat, "Invalid UTF-8 sequence" 
            });
        }
        ptr += expectedLength;
        ++count;
    }

    // Return the number of code points in the input string (0 if the string is empty)
    return Result<usize>::ok(count);
}

// ========================================================================================
// Implementation of UTF-16 encoding utility functions
// ========================================================================================

// Decode a UTF-16 code point
Result<char32> encoding::decodeUtf16CodePoint(Utf16StringView input, usize& advance) {
    advance = 0;

    if (input.empty()) {
        return Result<char32>::error({ 
            ErrorCode::InvalidFormat, "Input string is empty" 
        });
    }

    char16 firstUnit = input[0];
    if (isHighSurrogate(firstUnit)) {
        if (input.size() < 2) {
            return Result<char32>::error({ 
                ErrorCode::InvalidFormat, "Unpaired high surrogate" 
            });
        }
        char16 secondUnit = input[1];
        if (!isLowSurrogate(secondUnit)) {
            return Result<char32>::error({ 
                ErrorCode::InvalidFormat, "Expected low surrogate after high surrogate" 
            });
        }
        char32 cp = ((static_cast<char32>(firstUnit - 0xD800) << 10) | (secondUnit - 0xDC00)) + 0x10000;
        advance = 2;
        return Result<char32>::ok(cp);
    } else if (isLowSurrogate(firstUnit)) {
        return Result<char32>::error({ 
            ErrorCode::InvalidFormat, "Unpaired low surrogate" 
        });
    } else {
        advance = 1;
        return Result<char32>::ok(static_cast<char32>(firstUnit));
    }
}

// Encode a UTF-16 code point
usize encoding::encodeUtf16CodePoint(char32 cp, char16* out) {
    if (cp <= 0xFFFF) {
        if (cp >= 0xD800 && cp <= 0xDFFF) {
            return 0; // Invalid code point (surrogate range)
        }
        out[0] = static_cast<char16>(cp);
        return 1; // BMP character (1 code unit)
    } else if (cp <= 0x10FFFF) {
        cp -= 0x10000;
        out[0] = static_cast<char16>((cp >> 10) + 0xD800); // High surrogate
        out[1] = static_cast<char16>((cp & 0x3FF) + 0xDC00); // Low surrogate
        return 2; // Supplementary character (2 code units)
    }
    return 0; // Invalid code point
}

// Count the number of Unicode code points in a UTF-16 string
Result<usize> encoding::countUtf16CodePoints(Utf16StringView input) noexcept {
    usize count = 0;
    usize i = 0;

    while (i < input.size()) {
        char16 codeUnit = input[i];
        if (isHighSurrogate(codeUnit)) {
            if (i + 1 < input.size() && isLowSurrogate(input[i + 1])) {
                ++count; // Valid surrogate pair counts as one code point
                i += 2; // Skip both high and low surrogate
            } else {
                // Invalid surrogate pair (high surrogate not followed by low surrogate)
                return Result<usize>::error({ 
                    ErrorCode::InvalidFormat, "Invalid surrogate pair" 
                });
            }
        } else if (isLowSurrogate(codeUnit)) {
            // Invalid surrogate pair (low surrogate without preceding high surrogate)
            return Result<usize>::error({ 
                ErrorCode::InvalidFormat, "Unpaired low surrogate" 
            });
        } else {
            ++count; // BMP character counts as one code point
            ++i; // Move to the next code unit
        }
    }

    // Successfully counted the number of code points in the input string
    return Result<usize>::ok(count);
}

// ========================================================================================
// Implementation of UTF-32 encoding utility functions
// ========================================================================================

// Check if a UTF-32 string is valid
bool encoding::isValidUtf32Sequence(Utf32StringView input) noexcept {
    for (char32 cp : input) {
        if (!isValidCodePoint(cp)) {
            return false; // Found an invalid code point
        }
    }
    return true; // No invalid code points found
}

// Count the number of Unicode code points in a UTF-32 string
Result<usize> encoding::countUtf32CodePoints(Utf32StringView input) noexcept {
    if (input.empty()) {
        return Result<usize>::ok(0); // Empty string has 0 code points
    } 
    for (char32 cp : input) {
        if (!isValidCodePoint(cp)) {
            return Result<usize>::error({
                ErrorCode::InvalidFormat, "Invalid UTF-32 code point found"
            });
        }
    }

    // If we reach this point, all code points in the input string are valid, 
    // and we can simply return the length of the string as the number of code points.
    return Result<usize>::ok(input.size());
}

// ========================================================================================
// Internal helper functions for encoding conversion
// ========================================================================================

#if NEX_PLATFORM_IS_WINDOWS
    // Helper function to safely cast size_t to int for Windows API calls, with overflow check
    int32 safeStaticCastInt(usize size) {
        if (size > static_cast<usize>((NEX_STD numeric_limits<int32>::max)())) return -1;
        return static_cast<int32>(size);
    }
#endif

// ========================================================================================
// Implementation of encoding conversion functions
// ========================================================================================

// Convert ANSI string to UTF-16 string
Result<Utf16String> encoding::ansiToUtf16(Utf8StringView ansi) {
#if NEX_PLATFORM_IS_WINDOWS
    // Windows-specific implementation using MultiByteToWideChar

    int32 len = safeStaticCastInt(ansi.size());
    if (len < 0) {
        // Input size is too large to fit in an int, return an error result
        return Result<Utf16String>::error({
                ErrorCode::InvalidFormat, "Input string is too large" 
            });
    }
    int32 requiredSize = MultiByteToWideChar(CP_ACP, 0, ansi.data(), len, nullptr, 0);
    if (requiredSize <= 0) {
        // Failed to convert ANSI to UTF-16, return an error result
        return Result<Utf16String>::error({
                ErrorCode::OperationFailed, "Failed to convert ANSI to UTF-16" 
            });
    }
    Utf16String utf16(requiredSize, '\0');
    MultiByteToWideChar(CP_ACP, 0, ansi.data(), len, 
                        reinterpret_cast<wchar*>(utf16.data()), 
                        requiredSize);

    // Successfully converted ANSI to UTF-16, return the result
    return Result<Utf16String>::ok(NEX_MOVE(utf16));

#else
    // On non-Windows platforms, we can assume ANSI is UTF-8 and convert to UTF-16
    return utf8ToUtf16(ansi);
#endif
}

// Convert UTF-16 string to ANSI string
Result<Utf8String> encoding::utf16ToAnsi(Utf16StringView utf16) {
#if NEX_PLATFORM_IS_WINDOWS
    // Windows-specific implementation using WideCharToMultiByte

    int32 len = safeStaticCastInt(utf16.size());
    if (len < 0) {
        // Input size is too large to fit in an int, return an error result
        return Result<Utf8String>::error({
                ErrorCode::InvalidFormat, "Input string is too large" 
            });
    }
    int32 requiredSize = WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<LPCWCH>(utf16.data()), 
                            len, nullptr, 0, nullptr, nullptr);
    if (requiredSize <= 0) {
        // Failed to convert UTF-16 to ANSI, return an error result
        return Result<Utf8String>::error({
                ErrorCode::OperationFailed, "Failed to convert UTF-16 to ANSI" 
            });
    }

    BOOL usedDefaultChar = FALSE;
    Utf8String ansi(requiredSize, '\0');
    WideCharToMultiByte(CP_ACP, 0, 
                        reinterpret_cast<LPCWCH>(utf16.data()), 
                        len, 
                        ansi.data(), 
                        requiredSize, 
                        nullptr, 
                        &usedDefaultChar);
    if (usedDefaultChar) {
        // The conversion used a default character, which means some characters could not be represented in ANSI
        return Result<Utf8String>::error({
                ErrorCode::OperationFailed, "Some characters could not be represented in ANSI encoding" 
            });
    }

    // Successfully converted UTF-16 to ANSI, return the result
    return Result<Utf8String>::ok(NEX_MOVE(ansi));

#else
    // On non-Windows platforms, we can assume ANSI is UTF-8 and convert from UTF-16 to UTF-8
    return utf16ToUtf8(utf16);
#endif
}

// Convert local string to UTF-16 string
Result<Utf16String> encoding::localToUtf16(Utf8StringView local) {
#if NEX_PLATFORM_IS_WINDOWS
    // On Windows, local encoding is typically ANSI, so we can reuse the ANSI conversion
    return ansiToUtf16(local);
#else
    // On non-Windows platforms, we can assume local encoding is UTF-8 and convert to UTF-16
    return utf8ToUtf16(local);
#endif
}

// Convert UTF-16 string to local string
Result<Utf8String> encoding::utf16ToLocal(Utf16StringView utf16) {
#if NEX_PLATFORM_IS_WINDOWS
    // On Windows, local encoding is typically ANSI, so we can reuse the ANSI conversion
    return utf16ToAnsi(utf16);
#else
    // On non-Windows platforms, we can assume local encoding is UTF-8 and convert to UTF-8
    return utf16ToUtf8(utf16);
#endif
}

// Convert UTF-8 string to UTF-16 string
Result<Utf16String> encoding::utf8ToUtf16(Utf8StringView utf8) {
#if NEX_PLATFORM_IS_WINDOWS
    // Windows-specific implementation using MultiByteToWideChar

    int32 len = safeStaticCastInt(utf8.size());
    if (len < 0) {
        // Input size is too large to fit in an int, return an error result
        return Result<Utf16String>::error({
                ErrorCode::InvalidFormat, "Input string is too large" 
            });
    }
    int32 requiredSize = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), len, nullptr, 0);
    if (requiredSize <= 0) {
        // Failed to convert UTF-8 to UTF-16, return an error result
        return Result<Utf16String>::error({
                ErrorCode::OperationFailed, "Failed to convert UTF-8 to UTF-16" 
            });
    }
    Utf16String utf16(requiredSize, '\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), len, 
                        reinterpret_cast<wchar*>(utf16.data()), 
                        requiredSize);

    // Successfully converted UTF-8 to UTF-16, return the result
    return Result<Utf16String>::ok(NEX_MOVE(utf16));

#else
    // On non-Windows platforms, we can assume UTF-8 is already in the correct encoding and convert to UTF-16

    Utf16String result;

    // Reserve enough space to avoid multiple reallocations (worst case: all ASCII characters)
    result.reserve(utf8.size());
    
    const_char_ptr ptr = utf8.data();
    const_char_ptr end = ptr + utf8.size();
    
    while (ptr < end) {
        usize advance = 0;
        auto decodeRes = decodeUtf8CodePoint(Utf8StringView(ptr, end - ptr), advance);
        
        if (!decodeRes.isOk()) {
            // Failed to decode a UTF-8 code point, return an error result
            return Result<Utf16String>::error(decodeRes.error());
        }
        
        char16 utf16Buf[2];
        usize u16Len = encodeUtf16CodePoint(decodeRes.value(), utf16Buf);
        result.append(utf16Buf, u16Len);
        
        ptr += advance;
    }

    // Successfully converted UTF-8 to UTF-16, return the result
    return Result<Utf16String>::ok(NEX_MOVE(result));

#endif
}

// Convert UTF-16 string to UTF-8 string
Result<Utf8String> encoding::utf16ToUtf8(Utf16StringView utf16) {
#if NEX_PLATFORM_IS_WINDOWS
    // Windows-specific implementation using WideCharToMultiByte
    int32 len = safeStaticCastInt(utf16.size());
    if (len < 0) {
        // Input size is too large to fit in an int, return an error result
        return Result<Utf8String>::error({
                ErrorCode::InvalidFormat, "Input string is too large" 
            });
    }
    int32 requiredSize = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<LPCWCH>(utf16.data()), 
                            len, nullptr, 0, nullptr, nullptr);
    if (requiredSize <= 0) {
        // Failed to convert UTF-16 to UTF-8, return an error result
        return Result<Utf8String>::error({
                ErrorCode::OperationFailed, "Failed to convert UTF-16 to UTF-8" 
            });
    }

    Utf8String utf8(requiredSize, '\0');
    WideCharToMultiByte(CP_UTF8, 0, 
                        reinterpret_cast<LPCWCH>(utf16.data()), 
                        len, 
                        utf8.data(), 
                        requiredSize, 
                        nullptr, 
                        nullptr);

    // Successfully converted UTF-16 to UTF-8, return the result
    return Result<Utf8String>::ok(NEX_MOVE(utf8));
#else
    // On non-Windows platforms, we can assume UTF-16 is already in the correct encoding and convert to UTF-8
    
    Utf8String result;

    // Reserve enough space to avoid multiple reallocations (worst case: all characters are 4-byte code points)
    result.reserve(utf16.size() * 3 / 2); 

    usize i = 0;
    while (i < utf16.size()) {
        usize advance = 0;
        auto decodeRes = decodeUtf16CodePoint(utf16.substr(i), advance);

        if (!decodeRes.isOk()) { 
            // Failed to decode a UTF-16 code point, return an error result
            return Result<Utf8String>::error(decodeRes.error());
        }

        char8 u8buf[4];
        usize u8len = encodeUtf8CodePoint(decodeRes.value(), u8buf);
        if (u8len == 0) {
            // Failed to encode a UTF-8 code point, return an error result
            return Result<Utf8String>::error({ 
                ErrorCode::InvalidFormat, "Failed to encode UTF-8" 
            });
        }

        result.append(reinterpret_cast<const_char_ptr>(u8buf), u8len);
        i += advance;
    }

    // Successfully converted UTF-16 to UTF-8, return the result
    return Result<Utf8String>::ok(NEX_MOVE(result));

#endif
}

// Convert UTF-32 string to UTF-16 string
Result<Utf16String> encoding::utf32ToUtf16(Utf32StringView utf32) {
    Utf16String result;

    // Reserve enough space to avoid multiple reallocations, 
    // worst case: all characters are 4-byte code points, which would require 2 UTF-16 code units each
    result.reserve(utf32.size() * 2);

    for (char32 cp : utf32) {
        if (!isValidCodePoint(cp)) {
            // Found an invalid code point, return an error result
            return Result<Utf16String>::error({ 
                ErrorCode::InvalidFormat, "Invalid Unicode code point in input" 
            });
        }

        char16 utf16Buf[2];
        usize u16Len = encodeUtf16CodePoint(cp, utf16Buf);
        if (u16Len == 0) {
            // Failed to encode a UTF-16 code point, return an error result
            return Result<Utf16String>::error({ 
                ErrorCode::InvalidFormat, "Failed to encode UTF-16" 
            });
        }

        result.append(utf16Buf, u16Len);
    }

    // Successfully converted UTF-32 to UTF-16, return the result
    return Result<Utf16String>::ok(NEX_MOVE(result));
}

// Convert UTF-16 string to UTF-32 string
Result<Utf32String> encoding::utf16ToUtf32(Utf16StringView utf16) {
    Utf32String result;
    result.reserve(utf16.size()); // Reserve enough space to avoid multiple reallocations

    usize i = 0;
    while (i < utf16.size()) {
        usize advance = 0;
        auto decodeRes = decodeUtf16CodePoint(utf16.substr(i), advance);

        if (!decodeRes.isOk()) { 
            // Failed to decode a UTF-16 code point, return an error result
            return Result<Utf32String>::error(decodeRes.error());
        }

        result.push_back(decodeRes.value());
        i += advance;
    }

    // Successfully converted UTF-16 to UTF-32, return the result
    return Result<Utf32String>::ok(NEX_MOVE(result));
}

NEX_NAMESPACE_END