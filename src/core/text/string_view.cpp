/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/core/text/string_view.h"
#include "nex/core/containers/array.h"
#include "nex/core/containers/vector.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// Internal helper functions and types for StringView class
// =================================================================================

/** 
 * @details
 *  For small substrings, a linear search is sufficient. For larger substrings, we build a lookup table for O(1) character checks.
 *  The lookup table is a bitset that can represent the presence of any UTF-16 code unit (0 to 65535) in the substring.
 *  The kLinearSearchThreshold is a heuristic threshold for deciding when to switch from linear search to lookup table.
 *  The kBitsPerWord is the number of bits in a uint64_t, which we use for our lookup table. Each bit represents the presence of a code unit.
 *  The CharLookupTable is an array of uint64_t, where each bit corresponds to a UTF-16 code unit.
 *  The buildCharLookupTable function constructs the lookup table from the characters in the input substring.
 *  The lookupContains function checks if a given character is present in the substring using the lookup table.
 *  These optimizations can significantly improve the performance of the contains() method for larger substrings, 
 *  while keeping the implementation straightforward for smaller substrings.
 *  The implementation of the contains() method will use these helper functions to efficiently determine 
 *  if a character is present in the substring, which is a common operation in string searching algorithms.
 */
NEX_ANONYMOUS_NAMESPACE_BEGIN

// Threshold for deciding when to use linear search vs. lookup table
constexpr StringView::size_type kLinearSearchThreshold = 16;

// Number of bits in a uint64_t, used for the lookup table
constexpr StringView::size_type kBitsPerWord = 64;

// Maximum number of UTF-16 code units (0 to 65535) that can be represented in the lookup table
constexpr StringView::size_type kCodeUnitCount = 1u << 16;

// Type alias for the character lookup table, which is an array of uint64_t 
// where each bit represents the presence of a UTF-16 code unit in the substring
using CharLookupTable = Array<uint64, kCodeUnitCount / kBitsPerWord>;

// Build a lookup table for the characters in the input substring
inline CharLookupTable buildCharLookupTable(StringView input) {
    CharLookupTable table{};
    for (StringView::size_type i = 0; i < input.size(); ++i) {
        const auto ch = static_cast<uint16>(input[i]);
        table[ch / kBitsPerWord] |= uint64{1} << (ch % kBitsPerWord);
    }
    return table;
}

// Check if the character is present in the substring using the lookup table
inline bool lookupContains(const CharLookupTable& table, StringView::value_type ch) {
    const auto codeUnit = static_cast<uint16>(ch);
    return (table[codeUnit / kBitsPerWord] & (uint64{1} << (codeUnit % kBitsPerWord))) != 0;
}

NEX_ANONYMOUS_NAMESPACE_END

// =================================================================================
// Implementation of StringView class methods
// =================================================================================

/**
 * @brief Find first occurrence of substring
 * 
 * @note This implementation uses the Knuth-Morris-Pratt (KMP) algorithm for efficient substring searching.
 *       The search is performed from the beginning of the view towards the end, which allows us to find 
 *       the first occurrence. For short substrings, a simple linear search could be more efficient, 
 *       but KMP is used here for generality and better performance on longer substrings.
 *       The KMP algorithm preprocesses the substring to create a longest prefix-suffix (LPS) array, 
 *       which allows it to skip unnecessary comparisons when a mismatch occurs.
 *       The KMP algorithm has a time complexity of O(n + m), where n is the length of the view 
 *       and m is the length of the substring, making it efficient for searching even in large views.
 *       The implementation assumes that the substring and the view are both UTF-16 encoded, 
 *       and it compares characters accordingly.
 * 
 * @param substring The substring to search for
 * @param pos The position in the view to start the search from (default is 0)
 * @return The index of the first occurrence of the substring, or npos if not found
 */
StringView::size_type StringView::find(self other, size_type pos) const {
    if (pos > size()) {
        return other.size() == 0 ? size() : npos;
    }
    if (other.size() == 0) {
        return pos;
    }
    if (other.size() == 1) {
        return find(other[0], pos);
    }

    const size_type remaining = size() - pos;
    if (other.size() > remaining) {
        return npos;
    }

    const size_type needleSize = other.size();
    Vec<size_type> longestPrefixSuffix(needleSize, 0);
    for (size_type i = 1, prefixLength = 0; i < needleSize;) {
        if (other[i] == other[prefixLength]) {
            longestPrefixSuffix[i++] = ++prefixLength;
        } else if (prefixLength != 0) {
            prefixLength = longestPrefixSuffix[prefixLength - 1];
        } else {
            longestPrefixSuffix[i++] = 0;
        }
    }

    for (size_type haystackIndex = pos, needleIndex = 0; haystackIndex < size();) {
        if (operator[](haystackIndex) == other[needleIndex]) {
            ++haystackIndex;
            ++needleIndex;
            if (needleIndex == needleSize) {
                return haystackIndex - needleSize;
            }
        } else if (needleIndex != 0) {
            needleIndex = longestPrefixSuffix[needleIndex - 1];
        } else {
            ++haystackIndex;
        }
    }

    return npos;
}

/**
 * @brief Find first occurrence of character
 * 
 * @note This implementation uses std::char_traits<char16>::find for efficient character searching, 
 *       which is optimized for the character type and can take advantage of platform-specific optimizations.
 *       The search is performed from the beginning of the view towards the end, which allows us to find
 *       the first occurrence. The implementation assumes that the view is UTF-16 encoded, and it compares
 *       characters accordingly. If the character is found, the index of its first occurrence is returned;
 *       otherwise, npos is returned to indicate that the character was not found in the view.
 *. 
 * @param ch The character to search for
 * @param pos The position in the view to start the search from (default is 0)
 * @return The index of the first occurrence of the character, or npos if not found
 */
StringView::size_type StringView::find(value_type ch, size_type pos) const {
    if (pos >= size()) {
        return npos;
    }
    const char16* result = traits_type::find(data() + pos, size() - pos, ch);
    return result ? static_cast<size_type>(result - data()) : npos;
}

/**
 * @brief Find last occurrence of substring
 * 
 * @note This implementation is not optimized for long substrings, but it is simple and works correctly.
 *       For long substrings, a more efficient algorithm like Boyer-Moore could be implemented if needed.
 *       The search is performed from the end of the view towards the beginning, which allows us to find the last occurrence.
 * 
 * @param substring The substring to search for
 * @param pos The position in the view to start the search from (default is npos)
 * @return The index of the last occurrence of the substring, or npos if not found
 */
StringView::size_type StringView::rfind(StringView substring, size_type pos) const {
    if (substring.size() > size()) {
        return npos;
    }
    if (substring.size() == 0) {
        return pos < size() ? pos : size();
    }
    if (substring.size() == 1) {
        return rfind(substring[0], pos);
    }

    const size_type remaining = pos < size() ? pos + 1 : size();
    if (substring.size() > remaining) {
        return npos;
    }

    for (size_type haystackIndex = remaining - substring.size(); haystackIndex != npos; --haystackIndex) {
        if (traits_type::compare(data() + haystackIndex, substring.data(), substring.size()) == 0) {
            return haystackIndex;
        }
    }

    return npos;
}

/**
 * @brief Find last occurrence of character
 * 
 * @note This implementation uses std::char_traits<char16>::find for efficient character searching,
 *       which is optimized for the character type and can take advantage of platform-specific optimizations.
 *       The search is performed from the end of the view towards the beginning, which allows us to find
 *       the last occurrence. The implementation assumes that the view is UTF-16 encoded, and it compares
 *       characters accordingly. If the character is found, the index of its last occurrence is returned;
 *       otherwise, npos is returned to indicate that the character was not found in the view.
 * 
 * @param ch The character to search for
 * @param pos The position in the view to start the search from (default is npos)
 * @return The index of the last occurrence of the character, or npos if not found
 */
StringView::size_type StringView::rfind(value_type ch, size_type pos) const {
    if (pos >= size()) {
        return npos;
    }
    for (size_type i = pos + 1; i-- > 0;) {
        if (operator[](i) == ch) {
            return i;
        }
    }
    return npos;
}

/**
 * @brief Find first occurrence of any character in the set
 * 
 * @note This implementation uses a linear search for small sets and a lookup table for larger sets,
 *       which provides a balance between simplicity and performance. The search is performed from
 *       the beginning of the view towards the end, which allows us to find the first occurrence.
 * 
 * @param input The set of characters to search for
 * @param pos The position in the view to start the search from (default is 0)
 * @return The index of the first occurrence of any character in the set, or npos if not found
 */
StringView::size_type StringView::findFirstOf(StringView input, size_type pos) const {
    if (pos >= size() || input.size() == 0) {
        return npos;
    }

    if (input.size() < kLinearSearchThreshold) {
        for (size_type i = pos; i < size(); ++i) {
            if (traits_type::find(input.data(), input.size(), operator[](i)) != nullptr) {
                return i;
            }
        }
        return npos;
    }

    const CharLookupTable inputChars = buildCharLookupTable(input);
    for (size_type i = pos; i < size(); ++i) {
        if (lookupContains(inputChars, operator[](i))) {
            return i;
        }
    }

    return npos;
}

/**
 * @brief Find first occurrence of any character not in the set
 * 
 * @note This implementation uses a linear search for small sets and a lookup table for larger sets,
 *       which provides a balance between simplicity and performance. The search is performed from
 *       the beginning of the view towards the end, which allows us to find the first occurrence.
 * 
 * @param input The set of characters to search for
 * @param pos The position in the view to start the search from (default is 0)
 * @return The index of the first occurrence of any character not in the set, or npos if not found
 */
StringView::size_type StringView::findFirstNotOf(StringView input, size_type pos) const {
    if (pos >= size()) {
        return npos;
    }
    if (input.size() == 0) {
        return pos;
    }

    if (input.size() < kLinearSearchThreshold) {
        for (size_type i = pos; i < size(); ++i) {
            if (traits_type::find(input.data(), input.size(), operator[](i)) == nullptr) {
                return i;
            }
        }
        return npos;
    }

    const CharLookupTable inputChars = buildCharLookupTable(input);
    for (size_type i = pos; i < size(); ++i) {
        if (!lookupContains(inputChars, operator[](i))) {
            return i;
        }
    }

    return npos;
}

/**
 * @brief Find last occurrence of any character in the set
 * 
 * @note This implementation uses a linear search for small sets and a lookup table for larger sets,
 *       which provides a balance between simplicity and performance. The search is performed from
 *       the end of the view towards the beginning, which allows us to find the last occurrence.
 * 
 * @param input The set of characters to search for
 * @param pos The position in the view to start the search from (default is npos)
 * @return The index of the last occurrence of any character in the set, or npos if not found
 */
StringView::size_type StringView::findLastOf(StringView input, size_type pos) const {
    if (size() == 0 || input.size() == 0) {
        return npos;
    }

    const size_type start = pos < size() ? pos : size() - 1;

    if (input.size() < kLinearSearchThreshold) {
        for (size_type i = start + 1; i-- > 0;) {
            if (traits_type::find(input.data(), input.size(), operator[](i)) != nullptr) {
                return i;
            }
        }
        return npos;
    }

    const CharLookupTable inputChars = buildCharLookupTable(input);
    for (size_type i = start + 1; i-- > 0;) {
        if (lookupContains(inputChars, operator[](i))) {
            return i;
        }
    }

    return npos;
}

/**
 * @brief Find last occurrence of any character not in the set
 * 
 * @note This implementation uses a linear search for small sets and a lookup table for larger sets,
 *       which provides a balance between simplicity and performance. The search is performed from
 *       the end of the view towards the beginning, which allows us to find the last occurrence.
 * 
 * @param input The set of characters to search for
 * @param pos The position in the view to start the search from (default is npos)
 * @return The index of the last occurrence of any character not in the set, or npos if not found
 */
StringView::size_type StringView::findLastNotOf(StringView input, size_type pos) const {
    if (size() == 0) {
        return npos;
    }

    const size_type start = pos < size() ? pos : size() - 1;
    if (input.size() == 0) {
        return start;
    }

    if (input.size() < kLinearSearchThreshold) {
        for (size_type i = start + 1; i-- > 0;) {
            if (traits_type::find(input.data(), input.size(), operator[](i)) == nullptr) {
                return i;
            }
        }
        return npos;
    }

    const CharLookupTable inputChars = buildCharLookupTable(input);
    for (size_type i = start + 1; i-- > 0;) {
        if (!lookupContains(inputChars, operator[](i))) {
            return i;
        }
    }

    return npos;
}

NEX_NAMESPACE_END