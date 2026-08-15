/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"
#include "nex/base/growth.h"
#include "nex/base/assert_crash.h"
#include "nex/base/char_traits.h"
#include "nex/base/span.h"

NEX_NAMESPACE_BEGIN

/**
 * @class  BasicStringView
 * @brief  A non-owning view of a contiguous sequence of characters.
 *
 * @details
 * BasicStringView provides a lightweight, non-owning reference to a sequence of characters. It does not manage the
 * lifetime of the underlying data and is intended for read-only access. It can be used to efficiently pass around
 * substrings or character sequences without copying.
 *
 * @tparam CharType The character type (e.g., nchar, wchar, char8, char16, char32).
 */
template <Character CharType>
class NEX_HIDDEN_FROM_ABI BasicStringView {
public:
    using value_type = CharType;
    using size_type = usize;
    using difference_type = isize;

    using char_type = CharType;
    using traits_type = CharTraits<CharType>;

    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    using self = BasicStringView<char_type>;
    using span_type = Span<const char_type>;

    using const_iterator = const_pointer;
    using iterator = const_iterator;

    // Represents an invalid position in the string view.
    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    const_pointer data_ = nullptr;  // Pointer to the character data.
    size_type size_ = 0;            // Number of characters in the view.

public:
    // Default constructor creates an empty string view.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringView() noexcept = default;

    // Default copy constructor and copy assignment operator.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringView(const BasicStringView&) noexcept = default;
    NEX_HIDDEN_FROM_ABI constexpr BasicStringView& operator=(const BasicStringView&) noexcept = default;

    // Constructor that takes a pointer to character data and a size.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringView(const_pointer data, size_type size) noexcept
        : data_(data), size_(size) {}

    // Constructor that takes a null-terminated string and computes its length.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringView(const_pointer str) noexcept
        : data_(str), size_(traits_type::length(str)) {}

    // Constructor that takes a Span of characters.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringView(span_type span) noexcept
        : data_(span.data()), size_(span.size()) {}

    // Default destructor.
    NEX_HIDDEN_FROM_ABI constexpr ~BasicStringView() noexcept = default;

    // Returns a read-only pointer to the string view's content.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_pointer data() const noexcept { return data_; }

    // Returns the number of characters in the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr size_type size() const noexcept { return size_; }

    // Returns the maximum number of characters the string view can hold.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI static constexpr size_type maxSize() noexcept {
        return NumericLimits<size_type>::max() / sizeof(value_type);
    }

    // Returns the number of characters in the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr size_type length() const noexcept { return size_; }

    // Determines whether the string view is empty.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool empty() const noexcept { return size_ == 0; }

    // Returns a const iterator to the beginning of the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_iterator begin() const noexcept { return cbegin(); }

    // Returns a const iterator to the end of the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_iterator end() const noexcept { return cend(); }

    // Returns a const iterator to the beginning of the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_iterator cbegin() const noexcept {
        return const_iterator(data_);
    }

    // Returns a const iterator to the end of the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_iterator cend() const noexcept {
        return const_iterator(data_ + size_);
    }

    // Returns the character at position without bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference operator[](size_type position) const {
        return data_[position];
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference at(size_type position) const {
        NEX_VERIFY_MSG(position < size_, "BasicStringView::at: position out of range");
        return data_[position];
    }

    // Returns the first character in the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference front() const {
        NEX_VERIFY_MSG(!empty(), "BasicStringView::front: string view is empty");
        return data_[0];
    }

    // Returns the last character in the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference back() const {
        NEX_VERIFY_MSG(!empty(), "BasicStringView::back: string view is empty");
        return data_[size_ - 1];
    }

    // Returns a subview of the string view starting at position with an optional count.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr self subview(size_type position, size_type count = npos) const {
        NEX_VERIFY_MSG(position <= size_, "BasicStringView::subview: position out of range");
        if (count == npos || count > size_ - position) {
            count = size_ - position;
        }
        return self(data_ + position, count);
    }

    // Returns a subview of the first count characters of the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr self first(size_type count) const {
        NEX_VERIFY_MSG(count <= size_, "BasicStringView::first: count out of range");
        return self(data_, count);
    }

    // Returns a subview of the last count characters of the string view.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr self last(size_type count) const {
        NEX_VERIFY_MSG(count <= size_, "BasicStringView::last: count out of range");
        return self(data_ + size_ - count, count);
    }

    // Removes the first count characters from the view.
    NEX_HIDDEN_FROM_ABI constexpr void removePrefix(size_type count) noexcept {
        if (count > size()) { count = size(); }
        *this = subview(count);
    }

    // Removes the last count characters from the view.
    NEX_HIDDEN_FROM_ABI constexpr void removeSuffix(size_type count) noexcept {
        if (count > size()) { count = size(); }
        *this = first(size() - count);
    }

    // Returns the index of the first occurrence of the specified character, or npos if not found.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr usize find(char_type ch, size_type position = 0) const {
        if (position >= size_) {
            return npos;
        }
        for (size_type i = position; i < size_; ++i) {
            if (traits_type::eq(data_[i], ch)) {
                return i;
            }
        }
        return npos;
    }

    // Returns the index of the first occurrence of the specified substring, or npos if not found.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr usize find(self other, size_type position = 0) const {
        if (position >= size_ || other.size() > size_ - position) {
            return npos;
        }
        if (other.empty()) {
            return position;
        }
        for (size_type i = position; i <= size_ - other.size(); ++i) {
            if (traits_type::compare(data_ + i, other.data(), other.size()) == 0) {
                return i;
            }
        }
        return npos;
    }

    // Returns the index of the last occurrence of the specified character, or npos if not found.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr usize rfind(char_type ch, size_type position = npos) const {
        if (empty()) {
            return npos;
        }
        if (position >= size_) {
            position = size_ - 1;
        }
        for (size_type i = position + 1; i-- > 0;) {
            if (traits_type::eq(data_[i], ch)) {
                return i;
            }
        }
        return npos;
    }

    // Returns the index of the last occurrence of the specified substring, or npos if not found.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr usize rfind(self other, size_type position = npos) const {
        if (other.size() > size_) {
            return npos;
        }
        if (position > size_ - other.size()) {
            position = size_ - other.size();
        }
        for (size_type i = position + 1; i-- > 0;) {
            if (traits_type::compare(data_ + i, other.data(), other.size()) == 0) {
                return i;
            }
        }
        return npos;
    }

    // Determines whether the string view contains the specified character. 
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool contains(char_type ch) const noexcept {
        return find(ch) != npos;
    }

    // Determines whether the string view contains the specified substring.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool contains(self other) const noexcept {
        return find(other) != npos;
    }

    // Determines whether the string view starts with the specified substring.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool startsWith(self other) const noexcept {
        if (other.empty()) return true;
        return size_ >= other.size() && traits_type::compare(data_, other.data(), other.size()) == 0;
    }

    // Determines whether the string view starts with the specified character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool startsWith(char_type ch) const noexcept {
        return !empty() && traits_type::eq(data_[0], ch);
    }

    // Determines whether the string view ends with the specified substring.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool endsWith(self other) const noexcept {
        if (other.empty()) return true;
        return size_ >= other.size() &&
            traits_type::compare(data_ + size_ - other.size(), other.data(), other.size()) == 0;
    }

    // Determines whether the string view ends with the specified character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool endsWith(char_type ch) const noexcept {
        return !empty() && traits_type::eq(data_[size_ - 1], ch);
    }

    // Compares this string view with another string view lexicographically.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr int32 compare(self other) const noexcept {
        size_type minSize = (size_ < other.size()) ? size_ : other.size();
        int32 result = traits_type::compare(data_, other.data(), minSize);
        if (result != 0) {
            return result;
        }
        if (size_ < other.size()) {
            return -1;
        }
        if (size_ > other.size()) {
            return 1;
        }
        return 0;
    }

    // Compares this string view with another string view for equality.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator==(self other) const noexcept {
        return size_ == other.size() && compare(other) == 0;
    }

    // Compares this string view with another string view for inequality.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator!=(self other) const noexcept {
        return !(*this == other);
    }

    // Compares this string view with another string view for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator<(self other) const noexcept {
        return compare(other) < 0;
    }

    // Compares this string view with another string view for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator<=(self other) const noexcept {
        return compare(other) <= 0;
    }

    // Compares this string view with another string view for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator>(self other) const noexcept {
        return compare(other) > 0;
    }

    // Compares this string view with another string view for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator>=(self other) const noexcept {
        return compare(other) >= 0;
    }

    // Returns a Span representing the string view's content.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr span_type asSpan() const noexcept {
        return span_type(data_, size_);
    }
};

// =================================================================================
// Defines common type aliases for BasicStringView with different character types.
// =================================================================================

/**
 * @typedef NStringView
 * @brief   A non-owning view over a native narrow character sequence (`nchar`).
 */
using NStringView = BasicStringView<nchar>;

/**
 * @typedef WStringView
 * @brief   A non-owning view over a wide character sequence (`wchar`).
 */
using WStringView = BasicStringView<wchar>;

/**
 * @typedef AnsiStringView
 * @brief   A non-owning view over an ANSI character sequence (`nchar`).
 */
using AnsiStringView = BasicStringView<nchar>;

#if NEX_HAS_BUILTIN_CHAR8_T
    /**
     * @typedef Utf8StringView
     * @brief   A non-owning view over a UTF-8 character sequence (`char8`).
     */
    using Utf8StringView = BasicStringView<char8>;
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

/**
 * @typedef Utf16StringView
 * @brief   A non-owning view over a UTF-16 character sequence (`char16`).
 */
using Utf16StringView = BasicStringView<char16>;

/**
 * @typedef Utf32StringView
 * @brief   A non-owning view over a UTF-32 character sequence (`char32`).
 */
using Utf32StringView = BasicStringView<char32>;

#if NEX_PLATFORM_IS_WINDOWS
    /**
     * @typedef LocalStringView
     * @brief   A non-owning view over a local character sequence.
     * @details
     * The LocalStringView type is defined as BasicStringView<wchar> on Windows platforms,
     * as Windows uses wide characters with UTF-16 encoding for local strings.
     */
    using LocalStringView = BasicStringView<wchar>;
#elif NEX_PLATFORM_FAMILY_IS_POSIX
    /**
     * @typedef LocalStringView
     * @brief   A non-owning view over a local character sequence.
     * @details
     * The LocalStringView type is defined as BasicStringView<char8> on POSIX platforms (macOS/Linux),
     * as POSIX uses narrow characters with UTF-8 encoding for local strings.
     */
    using LocalStringView = BasicStringView<char8>;
#endif

NEX_NAMESPACE_END
