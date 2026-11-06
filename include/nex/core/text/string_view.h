/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/fwd.h"
#include "nex/base/iterator.h"
#include "nex/base/reverse_iterator.h"
#include "nex/base/assert_crash.h"
#include "nex/base/string_view.h"

NEX_NAMESPACE_BEGIN

/**
 * @class StringView
 * @brief A non-owning view of a UTF-8 encoded string for NexCore core layer.
 * 
 * @details
 * StringView provides a lightweight, non-owning reference to a UTF-8 encoded string.
 * It does not manage the lifetime of the underlying data and is intended for read-only access.
 * It can be used to efficiently pass around substrings or character sequences without copying.
 * 
 * @note
 * StringView is designed to work with UTF-8 encoded strings, making it suitable for
 * applications that require Unicode support while maintaining compatibility with existing APIs.
 * 
 * @see String class for an owning UTF-8 string implementation.
 * @see BasicStringView for a more general non-owning view of character sequences.
 */
class NEX_API StringView {
public:
    using self = StringView;
    using base_view = BasicStringView<char8>;
    using narrow_view = BasicStringView<nchar>;

    using value_type = base_view::value_type;
    using size_type = base_view::size_type;
    using difference_type = base_view::difference_type;

    using char_type = base_view::char_type;
    using traits_type = base_view::traits_type;

    using pointer = base_view::pointer;
    using const_pointer = base_view::const_pointer;
    using reference = base_view::reference;
    using const_reference = base_view::const_reference;

    using span_type = base_view::span_type;

    using const_iterator = const_pointer;
    using iterator = const_iterator;
    using reverse_iterator = ReverseIterator<const_iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;

    // Representing "not found" for find operations or "until the end" for substring operations
    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    base_view view_;  // Underlying BasicStringView for UTF-8 data

public:
    // Default constructor creates an empty StringView
    constexpr StringView() noexcept = default;

    // Default copy constructor and copy assignment operator
    constexpr StringView(const StringView& other) = default;
    constexpr StringView& operator=(const StringView& other) = default;

    // Constructor that takes an Utf8StringView
    constexpr StringView(base_view view) noexcept
        : view_(view.data(), view.size()) {}
    
    // Constructor that takes a pointer to an UTF-8 string data and a size
    constexpr StringView(const_pointer data, size_type size) noexcept
        : view_(data, size) {}

    // Constructor that takes a null-terminated UTF-8 string
    constexpr StringView(const_pointer data) noexcept
        : view_(data) {}

    // Constructor that takes a Span of UTF-8 characters
    constexpr StringView(span_type span) noexcept
        : view_(span.data(), span.size()) {}

    // Default destructor
    constexpr ~StringView() = default;

    // Returns a read-only pointer to the string view's content as UTF-8 encoded data.
    NEX_NODISCARD constexpr const_pointer data() const noexcept { return view_.data(); }

    // Returns the number of characters in the string view.
    NEX_NODISCARD constexpr size_type size() const noexcept { return view_.size(); }

    // Returns the number of characters in the string view.
    NEX_NODISCARD constexpr size_type length() const noexcept { return view_.length(); }

    // Determines whether the string view is empty.
    NEX_NODISCARD constexpr bool empty() const noexcept { return view_.empty(); }

    // Returns a const iterator to the beginning of the string view.
    NEX_NODISCARD constexpr const_iterator begin() const noexcept { return view_.begin(); }

    // Returns a const iterator to the end of the string view.
    NEX_NODISCARD constexpr const_iterator end() const noexcept { return view_.end(); }

    // Returns a const iterator to the beginning of the string view.
    NEX_NODISCARD constexpr const_iterator cbegin() const noexcept { return view_.cbegin(); }

    // Returns a const iterator to the end of the string view.
    NEX_NODISCARD constexpr const_iterator cend() const noexcept { return view_.cend(); }

    // Returns a const reverse iterator to the beginning of the reversed string view (i.e., end of the normal view)
    NEX_NODISCARD constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    // Returns a const reverse iterator to the end of the reversed string view (i.e., beginning of the normal view)
    NEX_NODISCARD constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    // Returns a const reverse iterator to the beginning of the reversed string view (i.e., end of the normal view)
    NEX_NODISCARD constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    // Returns a const reverse iterator to the end of the reversed string view (i.e., beginning of the normal view)
    NEX_NODISCARD constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    // Returns the character at the specified position without bounds checking.
    NEX_NODISCARD constexpr value_type operator[](size_type pos) const noexcept { return view_[pos]; }

    // Returns the character at the specified position with bounds checking.
    NEX_NODISCARD constexpr value_type at(size_type pos) const { return view_.at(pos); }

    // Returns the first character in the string view.
    NEX_NODISCARD constexpr value_type front() const noexcept { return view_.front(); }

    // Returns the last character in the string view.
    NEX_NODISCARD constexpr value_type back() const noexcept { return view_.back(); }

    // Returns a subview of the string view starting at position with an optional count.
    NEX_NODISCARD constexpr self subview(size_type pos, size_type count = npos) const {
        return self(view_.subview(pos, count));
    }

    // Returns a subview of the first count characters of the string view.
    NEX_NODISCARD constexpr self first(size_type count) const noexcept { return self(view_.first(count)); }

    // Returns a subview of the last count characters of the string view.
    NEX_NODISCARD constexpr self last(size_type count) const noexcept { return self(view_.last(count)); }

    // Returns a subview of the leftmost count characters of the string view.
    NEX_NODISCARD constexpr self left(size_type count) const noexcept { return first(count); }

    // Returns a subview of the rightmost count characters of the string view.
    NEX_NODISCARD constexpr self right(size_type count) const noexcept { return last(count); }

    // Returns a subview of the middle of the string view starting at start with an optional count.
    NEX_NODISCARD constexpr self mid(size_type start, size_type count = npos) const noexcept {
        return self(view_.subview(start, count));
    }

    // Removes the first count characters from the view.
    constexpr void removePrefix(size_type count) noexcept { view_.removePrefix(count); }

    // Removes the last count characters from the view.
    constexpr void removeSuffix(size_type count) noexcept { view_.removeSuffix(count); }

    // Returns the underlying BasicStringView for interoperability with other APIs.
    NEX_NODISCARD constexpr base_view asBasicView() const noexcept { return view_; }

    // Returns a UTF-8 narrow string view for interoperability with APIs that accept narrow strings.
    NEX_NODISCARD constexpr narrow_view asNarrow() const noexcept {
        return narrow_view(reinterpret_cast<narrow_view::const_pointer>(view_.data()), view_.size());
    }

    // Returns a Span of the string view's content for interoperability with APIs that accept spans.
    NEX_NODISCARD constexpr span_type asSpan() const noexcept { return view_.asSpan(); }

    // Finds the first occurrence of a substring, starting from a given position.
    NEX_NODISCARD constexpr size_type find(self other, size_type pos = 0) const {
        return view_.find(other.view_, pos);
    }

    // Finds the first occurrence of a character, starting from a given position.
    NEX_NODISCARD constexpr size_type find(value_type ch, size_type pos = 0) const {
        return view_.find(ch, pos);
    }

    // Finds the last occurrence of a substring, starting from a given position.
    NEX_NODISCARD constexpr size_type rfind(self other, size_type pos = npos) const {
        return view_.rfind(other.view_, pos);
    }

    // Finds the last occurrence of a character, starting from a given position.
    NEX_NODISCARD constexpr size_type rfind(value_type ch, size_type pos = npos) const {
        return view_.rfind(ch, pos);
    }

    // Finds the first occurrence of any character in the set, starting from a given position.
    NEX_NODISCARD constexpr size_type findFirstOf(self other, size_type pos = 0) const {
        if (pos >= size()) {
            return npos;
        }
        if (other.empty()) {
            return pos;
        }
        for (size_type i = pos; i < size(); ++i) {
            for (size_type j = 0; j < other.size(); ++j) {
                if (traits_type::eq(view_[i], other[j])) {
                    return i;
                }
            }
        }
        return npos;
    }

    // Finds the first occurrence of any character not in the set, starting from a given position.
    NEX_NODISCARD constexpr size_type findFirstNotOf(self other, size_type pos = 0) const {
        if (pos >= size()) {
            return npos;
        }
        for (size_type i = pos; i < size(); ++i) {
            bool found = false;
            for (size_type j = 0; j < other.size(); ++j) {
                if (traits_type::eq(view_[i], other[j])) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return i;
            }
        }
        return npos;
    }

    // Finds the last occurrence of any character in the set, starting from a given position.
    NEX_NODISCARD constexpr size_type findLastOf(self other, size_type pos = npos) const {
        if (empty()) {
            return npos;
        }
        if (other.empty()) {
            return pos;
        }
        if (pos >= size()) {
            pos = size() - 1;
        }
        for (size_type i = pos + 1; i-- > 0;) {
            for (size_type j = 0; j < other.size(); ++j) {
                if (traits_type::eq(view_[i], other[j])) {
                    return i;
                }
            }
        }
        return npos;
    }

    // Finds the last occurrence of any character not in the set, starting from a given position.
    NEX_NODISCARD constexpr size_type findLastNotOf(self other, size_type pos = npos) const {
        if (empty()) {
            return npos;
        }
        if (pos >= size()) {
            pos = size() - 1;
        }
        for (size_type i = pos + 1; i-- > 0;) {
            bool found = false;
            for (size_type j = 0; j < other.size(); ++j) {
                if (traits_type::eq(view_[i], other[j])) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return i;
            }
        }
        return npos;
    }

    // Determines whether the string view starts with the specified substring.
    NEX_NODISCARD constexpr bool startsWith(self prefix) const noexcept {
        return view_.startsWith(prefix.view_);
    }

    // Determines whether the string view starts with the specified character.
    NEX_NODISCARD constexpr bool startsWith(value_type ch) const noexcept {
        return !empty() && traits_type::eq(front(), ch);
    }

    // Determines whether the string view ends with the specified substring.
    NEX_NODISCARD constexpr bool endsWith(self suffix) const noexcept {
        return view_.endsWith(suffix.view_);
    }

    // Determines whether the string view ends with the specified character.
    NEX_NODISCARD constexpr bool endsWith(value_type ch) const noexcept {
        return !empty() && traits_type::eq(back(), ch);
    }

    // Determines whether the string view contains the specified substring.
    NEX_NODISCARD constexpr bool contains(self other) const noexcept {
        return find(other) != npos;
    }

    // Determines whether the string view contains the specified character.
    NEX_NODISCARD constexpr bool contains(value_type ch) const noexcept {
        return find(ch) != npos;
    }

    // Compares this string view with another string view lexicographically.
    NEX_NODISCARD constexpr int32 compare(self other) const noexcept { return view_.compare(other.view_); }

    // Compares a substring of this string view with another string view lexicographically.
    NEX_NODISCARD constexpr int32 compare(size_type pos, size_type count, self other) const noexcept {
        return subview(pos, count).compare(other);
    }

    // Compares this string view with another string view for equality.
    friend constexpr bool operator==(self lhs, self rhs) noexcept {
        return lhs.view_.size() == rhs.view_.size() && lhs.compare(rhs) == 0;
    }

    // Compares this string view with another string view for inequality.
    friend constexpr bool operator!=(self lhs, self rhs) noexcept {
        return !(lhs == rhs);
    }

    // Compares this string view with another string view for lexicographical ordering.
    friend constexpr bool operator<(self lhs, self rhs) noexcept {
        return lhs.compare(rhs) < 0;
    }

    // Compares this string view with another string view for lexicographical ordering.
    friend constexpr bool operator<=(self lhs, self rhs) noexcept {
        return !(rhs < lhs);
    }

    // Compares this string view with another string view for lexicographical ordering.
    friend constexpr bool operator>(self lhs, self rhs) noexcept {
        return lhs.compare(rhs) > 0;
    }

    // Compares this string view with another string view for lexicographical ordering.
    friend constexpr bool operator>=(self lhs, self rhs) noexcept {
        return !(lhs < rhs);
    }
};

/**
 * @struct StringViewHash
 * @brief Hash function for StringView to allow usage in unordered containers
 */
struct StringViewHash {
    constexpr usize operator()(StringView sv) const noexcept {
        constexpr usize kOffsetBasis =
            sizeof(usize) == 8 ? static_cast<usize>(14695981039346656037ull)
                               : static_cast<usize>(2166136261u);
        constexpr usize kPrime =
            sizeof(usize) == 8 ? static_cast<usize>(1099511628211ull)
                               : static_cast<usize>(16777619u);

        usize hash = kOffsetBasis;
        for (usize i = 0; i < sv.size(); ++i) {
            hash ^= static_cast<usize>(sv[i]);
            hash *= kPrime;
        }
        return hash;
    }
};

NEX_NAMESPACE_END

/**
 * @brief Explicit specialization of std::hash for StringView to allow usage in unordered containers 
 *        without needing to specify the hash function
 */

NEX_STD_BEGIN

template<>
struct hash<NEX_PREPEND_NAMESPACE(StringView)> {
    constexpr size_t operator()(NEX_PREPEND_NAMESPACE(StringView) sv) const noexcept {
        return NEX_PREPEND_NAMESPACE(StringViewHash){}(sv);
    }
};

NEX_STD_END
