/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "common/macros.h"
#include "common/types.h"
#include "common/result.h"
#include "common/string_view.h"

NEXSUITE_NAMESPACE_BEGIN

/**
 * @class   KeyCombination
 * @brief   Key combination value object
 * 
 * Immutable representation of a keyboard shortcut (modifiers + key).
 * Provides type safety and utility methods.
 * 
 * @details
 * KeyCombination encapsulates a keyboard shortcut consisting of
 * modifier keys (e.g., Ctrl, Alt, Shift) and a virtual key code.
 * It ensures type safety by preventing mixing with raw integer types,
 * and provides utility methods for validation and string conversion.
 * 
 * @note An empty KeyCombination has no modifiers and a virtual key of 0.
 * @note Valid KeyCombinations must have a non-zero virtual key code.
 * 
 * @see Keyboard shortcut management
 */
class NEX_EXPORT KeyCombination {
public:
    // Supported modifiers as bit flags
    enum class Modifier : uint32 {
        None    = 0x0000,
        Alt     = 0x0001,
        Ctrl    = 0x0002,
        Shift   = 0x0004,
        Win     = 0x0008
    };

    // String format options for toString()
    enum class StringFormat {
        HumanReadable,
        Debug
    };

public:
    // Constants for supported modifiers
    static constexpr uint32 kModifierAlt = static_cast<uint32>(Modifier::Alt);
    static constexpr uint32 kModifierCtrl = static_cast<uint32>(Modifier::Ctrl);
    static constexpr uint32 kModifierShift = static_cast<uint32>(Modifier::Shift);
    static constexpr uint32 kModifierWin = static_cast<uint32>(Modifier::Win);
    static constexpr uint32 kSupportedModifierMask =
        kModifierAlt | kModifierCtrl | kModifierShift | kModifierWin;

public:
    // Create an empty key combination (no modifiers, virtual key = 0)
    static constexpr KeyCombination none() {
        return KeyCombination(0, 0);
    }

    // Constructor
    constexpr KeyCombination(uint32 modifiers, uint32 virtualKey)
        : modifiers_(modifiers), virtualKey_(virtualKey) {}

    // Constructor with Modifier enum
    constexpr KeyCombination(Modifier modifier, uint32 virtualKey)
        : modifiers_(static_cast<uint32>(modifier)), virtualKey_(virtualKey) {}

    // Copy constructor and assignment
    NEXSUITE_DEFAULT_COPY(KeyCombination);

    // Move constructor and assignment
    NEXSUITE_DEFAULT_MOVE(KeyCombination);

    ////// Accessors and utility methods ------------------------
    
    // Get modifiers
    constexpr uint32 modifiers() const { return modifiers_; }

    // Get virtual key
    constexpr uint32 virtualKey() const { return virtualKey_; }

    // Get normalized modifiers (unsupported bits removed)
    constexpr uint32 normalizedModifiers() const {
        return modifiers_ & kSupportedModifierMask;
    }

    // Return a normalized key combination
    constexpr KeyCombination normalized() const {
        return KeyCombination(normalizedModifiers(), virtualKey_);
    }

    ////// Modifier query methods ------------------------

    // Query modifiers
    constexpr bool hasModifier(Modifier modifier) const {
        return (normalizedModifiers() & static_cast<uint32>(modifier)) != 0;
    }

    // Check if any modifier is present
    constexpr bool hasAnyModifier() const {
        return normalizedModifiers() != 0;
    }

    // Check if the modifiers include Ctrl
    constexpr bool hasCtrl() const { return hasModifier(Modifier::Ctrl); }

    // Check if the modifiers include Alt
    constexpr bool hasAlt() const { return hasModifier(Modifier::Alt); }

    // Check if the modifiers include Shift
    constexpr bool hasShift() const { return hasModifier(Modifier::Shift); }

    // Check if the modifiers include Win
    constexpr bool hasWin() const { return hasModifier(Modifier::Win); }

    ////// Builder methods ------------------------

    // Builder methods to create modified key combinations
    constexpr KeyCombination withModifier(Modifier modifier, bool enabled) const {
        const uint32 modifierMask = static_cast<uint32>(modifier);
        const uint32 nextModifiers = enabled
            ? (normalizedModifiers() | modifierMask)
            : (normalizedModifiers() & ~modifierMask);
        return KeyCombination(nextModifiers, virtualKey_);
    }

    // Build with Ctrl modifier included/excluded
    constexpr KeyCombination withCtrl(bool enabled) const {
        return withModifier(Modifier::Ctrl, enabled);
    }

    // Build with Alt modifier included/excluded
    constexpr KeyCombination withAlt(bool enabled) const {
        return withModifier(Modifier::Alt, enabled);
    }

    // Build with Shift modifier included/excluded
    constexpr KeyCombination withShift(bool enabled) const {
        return withModifier(Modifier::Shift, enabled);
    }

    // Build with Win modifier included/excluded
    constexpr KeyCombination withWin(bool enabled) const {
        return withModifier(Modifier::Win, enabled);
    }

    // Build with a different virtual key
    constexpr KeyCombination withVirtualKey(uint32 virtualKey) const {
        return KeyCombination(normalizedModifiers(), virtualKey);
    }

    ////// Validation methods ------------------------
    
    // Check if key combination is empty
    constexpr bool isEmpty() const {
        return normalizedModifiers() == 0 && virtualKey_ == 0;
    }

    // Check if key combination is valid
    // A valid key combination must have a non-zero virtual key code
    // and can have any combination of modifiers (including none).
    constexpr bool isValid() const {
        return virtualKey_ != 0 && ((modifiers_ & ~kSupportedModifierMask) == 0);
    }

    ////// String conversion methods ------------------------

    // Convert to string
    String toString(StringFormat format = StringFormat::HumanReadable) const;

    // Parse from string (e.g. "Ctrl+Alt+K", "Shift+VK112")
    static Result<KeyCombination, ErrorCode> fromString(const String& text);
    static Result<KeyCombination, ErrorCode> fromString(StringView text);

    ////// Comparison operators ------------------------
    
    // Equality operator
    bool operator==(const KeyCombination& other) const {
        return modifiers_ == other.modifiers_ 
                && virtualKey_ == other.virtualKey_;
    }

    // Inequality operator
    bool operator!=(const KeyCombination& other) const {
        return !(*this == other);
    }
    
private:
    // Modifiers
    uint32 modifiers_ = 0;

    // Virtual key
    uint32 virtualKey_ = 0;
};

NEXSUITE_NAMESPACE_END

// Hash specialization for KeyCombination to allow usage in unordered containers
// Combines the modifiers and virtual key into a single hash value
namespace std {
    template <>
    struct hash<NEXSUITE_PREPEND_NAMESPACE(KeyCombination)> {
        constexpr size_t operator()(const NEXSUITE_PREPEND_NAMESPACE(KeyCombination)& value) const noexcept {
            return (static_cast<size_t>(value.modifiers()) << 32)
                ^ static_cast<size_t>(value.virtualKey());
        }
    };
} // namespace std

