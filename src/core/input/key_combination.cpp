/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <algorithm>
#include <cctype>
#include <charconv>

#include "nex/base/string.h"
#include "nex/core/input/key_combination.h"
#include "nex/core/text/string.h"

NEX_NAMESPACE_BEGIN

// ============================================================================
// Internal helper functions for parsing and formatting key combinations
// ============================================================================

NEX_ANONYMOUS_NAMESPACE_BEGIN

// Trim leading and trailing ASCII whitespace from a string
Utf8String trimAscii(Utf8StringView text) {
    usize start = 0;
    usize end = text.size();

    while (start < end && NEX_STD isspace(static_cast<uchar>(text[start])) != 0) {
        ++start;
    }
    while (end > start && NEX_STD isspace(static_cast<uchar>(text[end - 1])) != 0) {
        --end;
    }

    return Utf8String(text.substr(start, end - start));
}

// Convert a string to uppercase ASCII
Utf8String toUpperAscii(Utf8StringView text) {
    Utf8String upper(text);
    NEX_STD transform(upper.begin(), upper.end(), upper.begin(),
        [](uchar ch) {
            return static_cast<nchar>(NEX_STD toupper(ch));
        });
    return upper;
}

// Parse an unsigned integer from a string view, ensuring the entire string is consumed
bool parseUnsignedValue(Utf8StringView text, uint32& value) {
    if (text.empty()) {
        return false;
    }

    cstring begin = text.data();
    cstring end = text.data() + text.size();
    auto parseResult = NEX_STD from_chars(begin, end, value);
    return parseResult.ec == NEX_STD errc() && parseResult.ptr == end;
}

// Parse function key tokens like "F1", "F2", ..., "F24" and convert them to virtual key codes
bool parseFunctionKey(Utf8StringView token, uint32& value) {
    if (token.size() < 2 || token[0] != 'F') {
        return false;
    }

    uint32 number = 0;
    if (!parseUnsignedValue(token.substr(1), number) || number == 0 || number > 24) {
        return false;
    }

    value = 0x70 + (number - 1);  // VK_F1..VK_F24
    return true;
}

// Parse a virtual key token and convert it to a virtual key code
bool parseVirtualKeyToken(Utf8StringView token, uint32& value) {
    const Utf8String upper = toUpperAscii(token);

    if (upper.rfind("VK", 0) == 0) {
        return parseUnsignedValue(Utf8StringView(upper).substr(2), value);
    }

    // Handle function keys
    if (parseFunctionKey(upper, value)) {
        return true;
    }

    // Handle common named keys
    if (upper == "TAB") { value = 0x09; return true; }
    if (upper == "ENTER" || upper == "RETURN") { value = 0x0D; return true; }
    if (upper == "ESC" || upper == "ESCAPE") { value = 0x1B; return true; }
    if (upper == "SPACE") { value = 0x20; return true; }
    if (upper == "DELETE" || upper == "DEL") { value = 0x2E; return true; }
    if (upper == "BACKSPACE" || upper == "BKSP") { value = 0x08; return true; }

    if (upper.size() == 1) {
        const uchar ch = static_cast<uchar>(upper[0]);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
            value = static_cast<uint32>(ch);
            return true;
        }
    }

    return false;
}

// Convert a virtual key code to a human-readable string representation
Utf8String virtualKeyToHumanReadable(uint32 virtualKey) {
    if ((virtualKey >= 'A' && virtualKey <= 'Z') || (virtualKey >= '0' && virtualKey <= '9')) {
        return Utf8String(1, static_cast<nchar>(virtualKey));
    }

    if (virtualKey >= 0x70 && virtualKey <= 0x87) {
        return "F" + NEX_STD to_string((virtualKey - 0x70) + 1);
    }

    // Handle common named keys
    switch (virtualKey) {
    case 0x09: return "Tab";
    case 0x0D: return "Enter";
    case 0x1B: return "Esc";
    case 0x20: return "Space";
    case 0x2E: return "Delete";
    case 0x08: return "Backspace";
    default:
        return "VK" + NEX_STD to_string(virtualKey);
    }
}

// Parse from a UTF-8 string view and return a KeyCombination result
Result<NEX_PREPEND_SUBNAMESPACE(input, KeyCombination)> 
parseFromUtf8String(Utf8StringView text) {
    using NEX_PREPEND_SUBNAMESPACE(input, KeyCombination);

    const Utf8String trimmed = trimAscii(text);
    if (trimmed.empty()) {
        return ok(KeyCombination::none());
    }

    uint32 modifiers = 0;
    uint32 virtualKey = 0;

    usize start = 0;
    while (start <= trimmed.size()) {
        const usize plusPos = trimmed.find('+', start);
        const usize tokenEnd = (plusPos == Utf8String::npos) ? trimmed.size() : plusPos;
        const Utf8String token = trimAscii(
            Utf8StringView(trimmed).substr(start, tokenEnd - start));

        if (token.empty()) {
            return error(
                ErrorCode::InvalidArgument, "Empty token in key combination string"
            );
        }

        // Check if the token is a modifier or a virtual key
        const Utf8String upper = toUpperAscii(token);
        if (upper == "CTRL" || upper == "CONTROL") {
            modifiers |= KeyCombination::kModifierCtrl;
        } else if (upper == "ALT") {
            modifiers |= KeyCombination::kModifierAlt;
        } else if (upper == "SHIFT") {
            modifiers |= KeyCombination::kModifierShift;
        } else if (upper == "WIN" || upper == "META" || upper == "SUPER") {
            modifiers |= KeyCombination::kModifierWin;
        } else {
            uint32 parsedKey = 0;
            if (!parseVirtualKeyToken(token, parsedKey)) {
                return error(
                    ErrorCode::InvalidArgument, "Invalid virtual key token"
                );
            }
            virtualKey = parsedKey;
        }

        if (plusPos == Utf8String::npos) {
            break;
        }
        start = plusPos + 1;
    }

    KeyCombination combination(modifiers, virtualKey);
    if (!combination.isValid()) {
        return error(
            ErrorCode::InvalidArgument, "Invalid key combination"
        );
    }

    return ok(combination.normalized());
}

NEX_ANONYMOUS_NAMESPACE_END

// ============================================================================
// Implementation of KeyCombination methods
// ============================================================================

NEX_SUBNAMESPACE_BEGIN(input)

// Convert the key combination to a string representation based on the specified format
String KeyCombination::toString(StringFormat format /* = StringFormat::HumanReadable */) const {
    Utf8String text;
    const auto appendToken = [&text](Utf8StringView token) {
        if (!text.empty()) {
            text += '+';
        }
        text.append(token.data(), token.size());
    };

    // Append modifiers in a consistent order
    const uint32 normalizedMask = normalizedModifiers();
    if ((normalizedMask & kModifierCtrl) != 0) { appendToken("Ctrl"); }
    if ((normalizedMask & kModifierAlt) != 0) { appendToken("Alt"); }
    if ((normalizedMask & kModifierShift) != 0) { appendToken("Shift"); }
    if ((normalizedMask & kModifierWin) != 0) { appendToken("Win"); }

    if (virtualKey_ != 0) {
        if (format == StringFormat::Debug) {
            appendToken("VK" + NEX_STD to_string(virtualKey_));
        } else {
            appendToken(virtualKeyToHumanReadable(virtualKey_));
        }
    }

    return String::fromUtf8(text);
}

// Parse a string representation of a key combination and return the corresponding KeyCombination object
Result<KeyCombination> KeyCombination::fromString(const String& text) {
    const auto utf8Result = text.toUtf8();
    if (!utf8Result.isOk()) {
        return error(
            ErrorCode::InvalidArgument, "Invalid string: Failed to convert to UTF-8"
        );
    }
    return parseFromUtf8String(utf8Result.value());
}

// Parse a string view representation of a key combination and return the corresponding KeyCombination object
Result<KeyCombination> KeyCombination::fromString(StringView text) {
    return fromString(text.toString());
}

NEX_SUBNAMESPACE_END(input)

NEX_NAMESPACE_END
