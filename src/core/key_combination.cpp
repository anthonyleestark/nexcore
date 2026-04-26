/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <algorithm>
#include <cctype>
#include <charconv>
#include <string>

#include "nex/core/key_combination.h"
#include "nex/core/text/string.h"

NEX_NAMESPACE_BEGIN

// Helper functions for parsing and formatting key combinations
namespace {

    // Trim leading and trailing ASCII whitespace from a string
    NEX_STD string trimAscii(NEX_STD string_view text) {
        usize start = 0;
        usize end = text.size();

        while (start < end && NEX_STD isspace(static_cast<unsigned char>(text[start])) != 0) {
            ++start;
        }
        while (end > start && NEX_STD isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
            --end;
        }

        return NEX_STD string(text.substr(start, end - start));
    }

    // Convert a string to uppercase ASCII
    NEX_STD string toUpperAscii(NEX_STD string_view text) {
        NEX_STD string upper(text);
        NEX_STD transform(upper.begin(), upper.end(), upper.begin(),
            [](unsigned char ch) {
                return static_cast<char>(NEX_STD toupper(ch));
            });
        return upper;
    }

    // Parse an unsigned integer from a string view, ensuring the entire string is consumed
    bool parseUnsignedValue(NEX_STD string_view text, uint32& value) {
        if (text.empty()) {
            return false;
        }

        const char* begin = text.data();
        const char* end = text.data() + text.size();
        auto parseResult = NEX_STD from_chars(begin, end, value);
        return parseResult.ec == NEX_STD errc() && parseResult.ptr == end;
    }

    // Parse function key tokens like "F1", "F2", ..., "F24" and convert them to virtual key codes
    bool parseFunctionKey(NEX_STD string_view token, uint32& value) {
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
    bool parseVirtualKeyToken(NEX_STD string_view token, uint32& value) {
        const NEX_STD string upper = toUpperAscii(token);

        if (upper.rfind("VK", 0) == 0) {
            return parseUnsignedValue(NEX_STD string_view(upper).substr(2), value);
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
            const unsigned char ch = static_cast<unsigned char>(upper[0]);
            if ((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')) {
                value = static_cast<uint32>(ch);
                return true;
            }
        }

        return false;
    }

    // Convert a virtual key code to a human-readable string representation
    NEX_STD string virtualKeyToHumanReadable(uint32 virtualKey) {
        if ((virtualKey >= 'A' && virtualKey <= 'Z') || (virtualKey >= '0' && virtualKey <= '9')) {
            return NEX_STD string(1, static_cast<char>(virtualKey));
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
    Result<KeyCombination, ErrorCode> parseFromUtf8String(NEX_STD string_view text) {
        const NEX_STD string trimmed = trimAscii(text);
        if (trimmed.empty()) {
            return Result<KeyCombination, ErrorCode>::ok(KeyCombination::none());
        }

        uint32 modifiers = 0;
        uint32 virtualKey = 0;

        usize start = 0;
        while (start <= trimmed.size()) {
            const usize plusPos = trimmed.find('+', start);
            const usize tokenEnd = (plusPos == NEX_STD string::npos) ? trimmed.size() : plusPos;
            const NEX_STD string token = trimAscii(
                NEX_STD string_view(trimmed).substr(start, tokenEnd - start));

            if (token.empty()) {
                return Result<KeyCombination, ErrorCode>::error(ErrorCode::InvalidArgument);
            }

            // Check if the token is a modifier or a virtual key
            const NEX_STD string upper = toUpperAscii(token);
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
                    return Result<KeyCombination, ErrorCode>::error(ErrorCode::InvalidArgument);
                }
                virtualKey = parsedKey;
            }

            if (plusPos == NEX_STD string::npos) {
                break;
            }
            start = plusPos + 1;
        }

        KeyCombination combination(modifiers, virtualKey);
        if (!combination.isValid()) {
            return Result<KeyCombination, ErrorCode>::error(ErrorCode::InvalidArgument);
        }

        return Result<KeyCombination, ErrorCode>::ok(combination.normalized());
    }

} // namespace

// Convert the key combination to a string representation based on the specified format
String KeyCombination::toString(StringFormat format /* = StringFormat::HumanReadable */) const {
    NEX_STD string text;
    const auto appendToken = [&text](NEX_STD string_view token) {
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
Result<KeyCombination, ErrorCode> KeyCombination::fromString(const String& text) {
    const auto utf8Result = text.toUtf8();
    if (!utf8Result.isOk()) {
        return Result<KeyCombination, ErrorCode>::error(ErrorCode::InvalidArgument);
    }
    return parseFromUtf8String(utf8Result.value());
}

// Parse a string view representation of a key combination and return the corresponding KeyCombination object
Result<KeyCombination, ErrorCode> KeyCombination::fromString(StringView text) {
    return fromString(text.toString());
}

NEX_NAMESPACE_END
