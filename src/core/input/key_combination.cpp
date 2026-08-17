/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <algorithm>
#include <cctype>
#include <charconv>

#include "nex/base/string.h"
#include "nex/core/text/string.h"
#include "nex/core/input/key_combination.h"

NEX_NAMESPACE_BEGIN

// ============================================================================
// Internal helper functions for parsing and formatting key combinations
// ============================================================================

NEX_ANONYMOUS_NAMESPACE_BEGIN

// Trim leading and trailing ASCII whitespace from a string
NString trimAscii(NStringView text) {
    usize start = 0;
    usize end = text.size();

    while (start < end && NEX_STD isspace(static_cast<uchar>(text[start])) != 0) {
        ++start;
    }
    while (end > start && NEX_STD isspace(static_cast<uchar>(text[end - 1])) != 0) {
        --end;
    }

    return NString(text.subview(start, end - start));
}

// Convert a string to uppercase ASCII
NString toUpperAscii(NStringView text) {
    NString upper(text);
    NEX_STD transform(upper.begin(), upper.end(), upper.begin(),
        [](uchar ch) {
            return static_cast<nchar>(NEX_STD toupper(ch));
        });
    return upper;
}

// Parse an unsigned integer from a string view, ensuring the entire string is consumed
bool parseUnsignedValue(NStringView text, uint32& value) {
    if (text.empty()) {
        return false;
    }

    cstring begin = text.data();
    cstring end = text.data() + text.size();
    auto parseResult = NEX_STD from_chars(begin, end, value);
    return parseResult.ec == NEX_STD errc() && parseResult.ptr == end;
}

// Parse function key tokens like "F1", "F2", ..., "F24" and convert them to virtual key codes
bool parseFunctionKey(NStringView token, uint32& value) {
    if (token.size() < 2 || token[0] != 'F') {
        return false;
    }

    uint32 number = 0;
    if (!parseUnsignedValue(token.subview(1), number) || number == 0 || number > 24) {
        return false;
    }

    value = 0x70 + (number - 1);  // VK_F1..VK_F24
    return true;
}

// Parse a virtual key token and convert it to a virtual key code
bool parseVirtualKeyToken(NStringView token, uint32& value) {
    const NStringView upper = toUpperAscii(token).asView();

    if (upper.rfind("VK", 0) == 0) {
        return parseUnsignedValue(upper.subview(2), value);
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
NString virtualKeyToHumanReadable(uint32 virtualKey) {
    if ((virtualKey >= 'A' && virtualKey <= 'Z') || (virtualKey >= '0' && virtualKey <= '9')) {
        return NString(1, static_cast<nchar>(virtualKey));
    }

    if (virtualKey >= 0x70 && virtualKey <= 0x87) {
        return NString("F") + NEX_STD to_string((virtualKey - 0x70) + 1).c_str();
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
        return NString("VK") + NEX_STD to_string(virtualKey).c_str();
    }
}

// Parse from a UTF-8 string view and return a KeyCombination result
Result<NEX_PREPEND_SUBNAMESPACE(input, KeyCombination)> 
parseFromUtf8String(Utf8StringView text) {
    using NEX_PREPEND_SUBNAMESPACE(input, KeyCombination);

    const NStringView trimmed = trimAscii(encoding::utf8ToNarrow(text)).asView();
    if (trimmed.empty()) {
        return ok(KeyCombination::none());
    }

    uint32 modifiers = 0;
    uint32 virtualKey = 0;

    usize start = 0;
    while (start <= trimmed.size()) {
        const usize plusPos = trimmed.find('+', start);
        const usize tokenEnd = (plusPos == NString::npos) ? trimmed.size() : plusPos;
        const NString token = trimAscii(trimmed.subview(start, tokenEnd - start));

        if (token.empty()) {
            return error(
                ErrorCode::InvalidArgument, "Empty token in key combination string"
            );
        }

        // Check if the token is a modifier or a virtual key
        const NString upper = toUpperAscii(token.asView());
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
            if (!parseVirtualKeyToken(token.asView(), parsedKey)) {
                return error(
                    ErrorCode::InvalidArgument, "Invalid virtual key token"
                );
            }
            virtualKey = parsedKey;
        }

        if (plusPos == NString::npos) {
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
    NString text;
    const auto appendToken = [&text](NStringView token) {
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
            appendToken((NString("VK") + NEX_STD to_string(virtualKey_).c_str()).asView());
        } else {
            appendToken(virtualKeyToHumanReadable(virtualKey_).asView());
        }
    }

    return String::fromUtf8(reinterpret_cast<u8cstring>(text.data()));
}

// Parse a string representation of a key combination and return the corresponding KeyCombination object
Result<KeyCombination> KeyCombination::fromString(const String& text) {
    const auto utf8Result = text.toUtf8();
    if (!utf8Result.isOk()) {
        return error(
            ErrorCode::InvalidArgument, "Invalid string: Failed to convert to UTF-8"
        );
    }
    return parseFromUtf8String(utf8Result->asView());
}

// Parse a string view representation of a key combination and return the corresponding KeyCombination object
Result<KeyCombination> KeyCombination::fromString(StringView text) {
    return fromString(text);
}

NEX_SUBNAMESPACE_END(input)

NEX_NAMESPACE_END
