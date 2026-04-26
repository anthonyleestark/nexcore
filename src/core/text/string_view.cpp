/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <array>
#include <cstdint>
#include <vector>

#include "nex/core/text/string_view.h"
#include "nex/core/text/string.h"
#include "nex/core/result.h"

NEX_NAMESPACE_BEGIN

/** 
 * @brief Internal helper functions for optimized character lookup in contains() method
 * 
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
namespace {

    // Threshold for deciding when to use linear search vs. lookup table
    constexpr StringView::size_type kLinearSearchThreshold = 16;

    // Number of bits in a uint64_t, used for the lookup table
    constexpr StringView::size_type kBitsPerWord = 64;

    // Maximum number of UTF-16 code units (0 to 65535) that can be represented in the lookup table
    constexpr StringView::size_type kCodeUnitCount = 1u << 16;

    // Type alias for the character lookup table, which is an array of uint64_t 
    // where each bit represents the presence of a UTF-16 code unit in the substring
    using CharLookupTable = NEX_STD array<NEX_STD uint64_t, kCodeUnitCount / kBitsPerWord>;

    // Build a lookup table for the characters in the input substring
    inline CharLookupTable buildCharLookupTable(StringView input) {
        CharLookupTable table{};
        for (StringView::size_type i = 0; i < input.size(); ++i) {
            const auto ch = static_cast<NEX_STD uint16_t>(input[i]);
            table[ch / kBitsPerWord] |= NEX_STD uint64_t{1} << (ch % kBitsPerWord);
        }
        return table;
    }

    // Check if the character is present in the substring using the lookup table
    inline bool lookupContains(const CharLookupTable& table, StringView::value_type ch) {
        const auto codeUnit = static_cast<NEX_STD uint16_t>(ch);
        return (table[codeUnit / kBitsPerWord] & (NEX_STD uint64_t{1} << (codeUnit % kBitsPerWord))) != 0;
    }

} // namespace

////// Construction ------------------------

// Construct from String (forward declaration needed)
StringView::StringView(const String& data)
    : data_(data.data()), size_(data.size()) {}

////// Conversion ------------------------

// Convert to String (creates a new String instance from the view)
String StringView::toString() const {
    return String(*this);
}

////// Operations -----------------------

// Copy substring to destination
String StringView::copy(size_type count, size_type pos /* = 0 */) const {
    return String(substr(pos, count));
}

////// Subview operations -----------------------

// Get substring view
StringView StringView::substr(size_type pos, size_type count /* = npos */) const {
    if (pos > size()) {
        NEX_ASSERT_MSG(false, "Position out of range");
        return StringView();
    }
    if (count == npos) {
        count = size() - pos;
    }
    size_type rcount = NEX_STD min(count, size() - pos);
    return StringView(data_ + pos, rcount);
}

// Get left part of the view
StringView StringView::left(size_type count) const noexcept {
    if (count >= size()) {
        return *this;
    }
    return StringView(data_, count);
}

// Get right part of the view
StringView StringView::right(size_type count) const noexcept {
    if (count >= size()) {
        return *this;
    }
    return StringView(data_ + size() - count, count);
}

// Get middle part of the view
StringView StringView::mid(size_type start, size_type count) const noexcept {
    if (start >= size()) {
        return StringView();
    }
    if (count == npos || start + count > size()) {
        count = size() - start;
    }
    return StringView(data_ + start, count);
}

////// Comparison -----------------------

// Compare substring with another view
int StringView::compare(size_type pos, size_type count, StringView other) const {
    return substr(pos, count).compare(other);
}

////// Searching -----------------------

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
StringView::size_type StringView::find(StringView substring, size_type pos) const {
    if (pos > size_) {
        return substring.size_ == 0 ? size_ : npos;
    }
    if (substring.size_ == 0) {
        return pos;
    }
    if (substring.size_ == 1) {
        return find(substring.data_[0], pos);
    }

    const size_type remaining = size_ - pos;
    if (substring.size_ > remaining) {
        return npos;
    }

    const size_type needleSize = substring.size_;
    NEX_STD vector<size_type> longestPrefixSuffix(needleSize, 0);
    for (size_type i = 1, prefixLength = 0; i < needleSize;) {
        if (substring.data_[i] == substring.data_[prefixLength]) {
            longestPrefixSuffix[i++] = ++prefixLength;
        } else if (prefixLength != 0) {
            prefixLength = longestPrefixSuffix[prefixLength - 1];
        } else {
            longestPrefixSuffix[i++] = 0;
        }
    }

    for (size_type haystackIndex = pos, needleIndex = 0; haystackIndex < size_;) {
        if (data_[haystackIndex] == substring.data_[needleIndex]) {
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
    if (pos >= size_) {
        return npos;
    }
    const char16* result = NEX_STD char_traits<char16>::find(data_ + pos, size_ - pos, ch);
    return result ? static_cast<size_type>(result - data_) : npos;
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
    if (substring.size_ > size_) {
        return npos;
    }
    if (substring.size_ == 0) {
        return pos < size_ ? pos : size_;
    }
    if (substring.size_ == 1) {
        return rfind(substring.data_[0], pos);
    }

    const size_type remaining = pos < size_ ? pos + 1 : size_;
    if (substring.size_ > remaining) {
        return npos;
    }

    for (size_type haystackIndex = remaining - substring.size_; haystackIndex != npos; --haystackIndex) {
        if (NEX_STD char_traits<char16>::compare(data_ + haystackIndex, substring.data_, substring.size_) == 0) {
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
    if (pos >= size_) {
        return npos;
    }
    for (size_type i = pos + 1; i-- > 0;) {
        if (data_[i] == ch) {
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
    if (pos >= size_ || input.size_ == 0) {
        return npos;
    }

    if (input.size_ < kLinearSearchThreshold) {
        for (size_type i = pos; i < size_; ++i) {
            if (NEX_STD char_traits<char16>::find(input.data_, input.size_, data_[i]) != nullptr) {
                return i;
            }
        }
        return npos;
    }

    const CharLookupTable inputChars = buildCharLookupTable(input);
    for (size_type i = pos; i < size_; ++i) {
        if (lookupContains(inputChars, data_[i])) {
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
    if (pos >= size_) {
        return npos;
    }
    if (input.size_ == 0) {
        return pos;
    }

    if (input.size_ < kLinearSearchThreshold) {
        for (size_type i = pos; i < size_; ++i) {
            if (NEX_STD char_traits<char16>::find(input.data_, input.size_, data_[i]) == nullptr) {
                return i;
            }
        }
        return npos;
    }

    const CharLookupTable inputChars = buildCharLookupTable(input);
    for (size_type i = pos; i < size_; ++i) {
        if (!lookupContains(inputChars, data_[i])) {
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
    if (size_ == 0 || input.size_ == 0) {
        return npos;
    }

    const size_type start = pos < size_ ? pos : size_ - 1;

    if (input.size_ < kLinearSearchThreshold) {
        for (size_type i = start + 1; i-- > 0;) {
            if (NEX_STD char_traits<char16>::find(input.data_, input.size_, data_[i]) != nullptr) {
                return i;
            }
        }
        return npos;
    }

    const CharLookupTable inputChars = buildCharLookupTable(input);
    for (size_type i = start + 1; i-- > 0;) {
        if (lookupContains(inputChars, data_[i])) {
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
    if (size_ == 0) {
        return npos;
    }

    const size_type start = pos < size_ ? pos : size_ - 1;
    if (input.size_ == 0) {
        return start;
    }

    if (input.size_ < kLinearSearchThreshold) {
        for (size_type i = start + 1; i-- > 0;) {
            if (NEX_STD char_traits<char16>::find(input.data_, input.size_, data_[i]) == nullptr) {
                return i;
            }
        }
        return npos;
    }

    const CharLookupTable inputChars = buildCharLookupTable(input);
    for (size_type i = start + 1; i-- > 0;) {
        if (!lookupContains(inputChars, data_[i])) {
            return i;
        }
    }

    return npos;
}

// Check if view starts with prefix
bool StringView::startsWith(StringView prefix) const {
    return size() >= prefix.size() &&
           NEX_STD char_traits<char16>::compare(data_, prefix.data_, prefix.size_) == 0;
}

// Check if view ends with suffix
bool StringView::endsWith(StringView suffix) const {
    return size() >= suffix.size() &&
           NEX_STD char_traits<char16>::compare(data_ + size() - suffix.size()
                                                    , suffix.data_, suffix.size_) == 0;
}

NEX_NAMESPACE_END