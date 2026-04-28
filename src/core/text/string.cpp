/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <charconv>
#include <algorithm>

#include "nex/core/text/string.h"
#include "nex/core/text/string_view.h"
#include "nex/core/text/encoding.h"

NEX_CORE_NAMESPACE_BEGIN

////// Construction ------------------------

// Default constructor (empty string)
String::String() : buffer_() {}

// Construct from UTF-8 string view
String::String(Utf8StringView utf8) {
    buffer_ = fromUtf8(utf8).buffer_;
}

// Construct from UTF-8 string literal
String::String(const char* utf8) {
    buffer_ = fromUtf8(Utf8StringView(utf8)).buffer_;
}

// Construct from UTF-16 string view
String::String(Utf16StringView utf16) {
    buffer_.assign(utf16.data(), utf16.size());
}

// Construct from UTF-16 string literal
String::String(const char16* utf16) {
    buffer_ = fromUtf16(Utf16StringView(utf16)).buffer_;
}

// Constructor from StringView
// Because StringView is a non-owning view of a string, we need to copy the data into our own buffer 
// to ensure that the String instance owns its data and can manage its lifetime properly. 
// This constructor allows us to create a String from a StringView by copying the characters 
// from the view into the String's internal buffer.
// The StringView provides a data pointer and size, and its internal encoding is UTF-16 as well, 
// therefore, we can use them to directly assign the corresponding substring to the String's buffer.
String::String(StringView view) {
    buffer_.assign(view.data(), view.size());
}

////// Create string from numbers -----------------------

// Create string from a signed integer with specified base (default is 10)
String String::fromInt(int64 value, int base /* = 10 */) {
    char buffer[64];
    auto result = NEX_STD to_chars(buffer, buffer+64, value, base);
    if (result.ec == NEX_STD errc()) {
        // Successfully converted integer to string
        // Continue convert from ANSI to internal string representation
        return fromUtf16(encoding::ansiToUtf16(
            Utf8StringView(buffer, result.ptr - buffer)).valueOr(kEmpty));
    } else {
        // Error occurred during conversion (e.g., buffer too small)
        return String(kEmpty);
    }
}

// Create string from an unsigned integer with specified base (default is 10)
String String::fromUInt(uint64 value, int base /* = 10 */) {
    char buffer[64];
    auto result = NEX_STD to_chars(buffer, buffer+64, value, base);
    if (result.ec == NEX_STD errc()) {
        // Successfully converted integer to string
        // Continue convert from ANSI to internal string representation
        return fromUtf16(encoding::ansiToUtf16(
            Utf8StringView(buffer, result.ptr - buffer)).valueOr(kEmpty));
    } else {
        // Error occurred during conversion (e.g., buffer too small)
        return String(kEmpty);
    }
}

// Create string from a floating-point number
String String::fromFloat(double value, char format /* = 'g' */, int precision /* = 6 */) {
    // Determine the chars_format based on the given format character
    NEX_STD chars_format charsFormat = NEX_STD chars_format::general;
    switch (format) {
    case 'f':
    case 'F':
        // Fixed-point notation
        charsFormat = NEX_STD chars_format::fixed;
        break;
    case 'e':
    case 'E':
        // Scientific notation
        charsFormat = NEX_STD chars_format::scientific;
        break;
    case 'a':
    case 'A':
        // Hexadecimal notation
        charsFormat = NEX_STD chars_format::hex;
        break;
    case 'g':
    case 'G':
    default:
        // General format (default)
        charsFormat = NEX_STD chars_format::general;
        break;
    }

    // First try to convert using a small stack buffer
    char stackBuffer[128];
    const int safePrecision = precision < 0 ? 0 : precision;
    auto result = NEX_STD to_chars(stackBuffer, stackBuffer + sizeof(stackBuffer), value, charsFormat, safePrecision);
    if (result.ec == NEX_STD errc()) {
        // Successfully converted floating-point number to string
        // Continue convert from ANSI to internal string representation
        return fromUtf16(encoding::ansiToUtf16(
            Utf8StringView(stackBuffer, result.ptr - stackBuffer)).valueOr(kEmpty));
    }
    if (result.ec != NEX_STD errc::value_too_large) {
        // An error occurred during conversion (e.g., invalid format), 
        // but it's not due to buffer size, so return an empty string
        return String(kEmpty);
    }

    // If the stack buffer was too small, allocate a larger buffer on the heap
    // The required size can be estimated based on the precision and format, but to be safe, 
    // we can allocate a buffer that is large enough to hold the worst-case scenario for the given precision.
    Utf8String dynamicBuffer(static_cast<usize>(safePrecision) + 64, '\0');
    result = NEX_STD to_chars(dynamicBuffer.data(), dynamicBuffer.data() + dynamicBuffer.size()
                            , value, charsFormat, safePrecision);
    if (result.ec == NEX_STD errc()) {
        // Successfully converted floating-point number to string
        // Continue convert from ANSI to internal string representation
        return fromUtf16(encoding::ansiToUtf16(
            Utf8StringView(dynamicBuffer.data(), result.ptr - dynamicBuffer.data())).valueOr(kEmpty));
    }

    // An error occurred during conversion, return an empty string
    return String(kEmpty);
}

////// Create string from specified encoding -----------------------

// Create string from UTF-8 string
String String::fromUtf8(Utf8StringView utf8) {
    Result<Utf16String> result = encoding::utf8ToUtf16(utf8);
    if (result.isOk()) {
        // Successfully converted UTF-8 to UTF-16, 
        // create String from UTF-16 result
        return fromUtf16(result.value());
    } else {
        // Handle conversion error (e.g., invalid UTF-8 sequence)
        NEX_ASSERT_MSG(false, "Invalid UTF-8 sequence");
        return String(kEmpty);
    }
}

// Create string from UTF-16 string
String String::fromUtf16(Utf16StringView utf16) {
    String result;
    result.buffer_.assign(utf16.data(), utf16.size());
    return result;
}

////// Convert to specified encoding (instance methods) -----------------------

// Convert to UTF-8 string
Result<Utf8String> String::toUtf8() const {
    return encoding::utf16ToUtf8(buffer_);
}

// Convert to UTF-16 string
Result<Utf16String> String::toUtf16() const {
    // Since the internal representation is already UTF-16, we can directly return it as a result
    return Result<Utf16String>::ok(buffer_);
}

////// Concatenation operator -----------------------

// Concatenate with a StringView
String String::operator+(StringView view) const {
    String result(this->buffer_);
    result.append(view);
    return result;
}

// Concatenate with a StringView (in-place)
String& String::operator+=(StringView view) {
    return append(view);
}

////// String manipulation & modification -----------------------

// Append a StringView
String& String::append(StringView view) {
    buffer_.append(view.data(), view.size());
    return *this;
}

////// Extract a part of the string -----------------------

// Extract a substring from the string
String String::substr(size_type pos, size_type count) const {
    if (pos > buffer_.size()) return String();
    if (count == npos || pos + count > buffer_.size()) count = buffer_.size() - pos;
    return String(buffer_.substr(pos, count));
}

////// Searching -----------------------

// Find the first match of a substring
String::size_type String::find(StringView substring, size_type pos = 0) const {
    if (pos > buffer_.size()) return npos;
    size_type index = buffer_.find(substring.data(), pos, substring.size());
    return index != Utf16String::npos ? index : npos;
}

// Find the first match of a character
String::size_type String::find(value_type ch, size_type pos = 0) const {
    if (pos > buffer_.size()) return npos;
    size_type index = buffer_.find(ch, pos);
    return index != Utf16String::npos ? index : npos;
}

// Find the last match of a substring
String::size_type String::rfind(StringView substring, size_type pos = npos) const {
    if (pos > buffer_.size()) return npos;
    size_type index = buffer_.rfind(substring.data(), pos, substring.size());
    return index != Utf16String::npos ? index : npos;
}

// Find the last match of a character
String::size_type String::rfind(value_type ch, size_type pos = npos) const {
    if (pos > buffer_.size()) return npos;
    size_type index = buffer_.rfind(ch, pos);
    return index != Utf16String::npos ? index : npos;
}

// Check if the string contains a substring
bool String::contains(StringView substring) const {
    return find(substring) != npos;
}

// Check if the string contains a character
bool String::contains(value_type ch) const {
    return find(ch) != npos;
}

////// Comparison -----------------------

// Compare two string (case-sensitive)
// Returns:
// - -1 if this string is less than the other string
// - 0 if this string is equal to the other string
// - 1 if this string is greater than the other string
int String::compare(StringView other) const noexcept {
    size_type minLength = NEX_STD min(buffer_.size(), other.size());

    // If either string is empty, the result depends on their sizes
    if (minLength == 0) { 
        if (size() < other.size()) return -1;
        if (size() > other.size()) return 1;
        return 0;
    }

    int cmp = buffer_.compare(0, minLength, other.data(), minLength);
    if (cmp != 0) 
        return cmp; // If the common prefix is different, return the comparison result

    // If the common prefix is the same, 
    // compare lengths to determine order and return the difference in lengths

    // This string is shorter than the other string
    if (buffer_.size() < other.size()) {
        return -1; // It is considered less than the other string
    }

    // This string is longer than the other string
    if (buffer_.size() > other.size()) {
        return 1; // It is considered greater than the other string
    }

    // Strings are equal
    return 0; 
}

NEX_CORE_NAMESPACE_END
