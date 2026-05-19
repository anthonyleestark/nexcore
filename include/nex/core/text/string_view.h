/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <string>
#include <string_view>
#include <iterator>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/string.h"
#include "nex/base/assert_crash.h"

NEX_CORE_NAMESPACE_BEGIN

/**
 * @class StringView
 * @brief A non-owning view into a UTF-16 string
 * 
 * This class provides a lightweight, non-owning view into a UTF-16 encoded string.
 * It stores a pointer to the string data and its length, without owning the data.
 * 
 * StringView supports:
 * - Construction from String, std::u16string_view and other string types
 * - Substring operations (substr, left, right, mid)
 * - Character access and iteration
 * - Comparison operations
 * - Conversion to String when needed
 * 
 * @note StringView does not own the underlying string data. The user must ensure
 *       that the data remains valid for the lifetime of the StringView.
 */
class NEX_EXPORT StringView {
public:
    // Type aliases for compatibility with standard container conventions
    using value_type = char16;
    using size_type = usize;
    using difference_type = isize;
    using reference = const char16&;
    using const_reference = const char16&;
    using pointer = const char16*;
    using const_pointer = const char16*;
    using const_iterator = const_pointer;
    using iterator = const_iterator;
    using reverse_iterator = NEX_STD reverse_iterator<const_iterator>;
    using const_reverse_iterator = NEX_STD reverse_iterator<const_iterator>;

private:
    // Internal pointer to the string data (non-owning; UTF-16 encoded)
    const_pointer data_;

    // Size of the string view (number of characters)
    size_type size_;

public:
    ////// Construction -----------------------

    // Default constructor (empty view; null)
    constexpr StringView() noexcept 
        : data_(nullptr), size_(0) {}

    // Construct from UTF-16 string view
    constexpr StringView(Utf16StringView view) 
        : data_(view.data()), size_(view.size()) {}
    
    // Construct from UTF-16 string literal with explicit size
    constexpr StringView(const_pointer data, size_type size)
        : data_(data), size_(data ? size : 0) {}

    // Construct from null-terminated UTF-16 string
    constexpr StringView(const_pointer nullTerminatedUtf16)
        : data_(nullTerminatedUtf16)
        , size_(nullTerminatedUtf16 
            ? NEX_STD char_traits<value_type>::length(nullTerminatedUtf16) : 0) {}
    
    // Construct from String
    StringView(const String& data);
    
    // Copy constructor
    constexpr StringView(const StringView& other) = default;
    
    // Copy assignment operator
    constexpr StringView& operator=(const StringView& other) = default;
    
    // Destructor
    constexpr ~StringView() = default;

public:
    // Constant representing "not found" for find operations or "until the end" for substring operations
    static constexpr size_type npos = static_cast<size_type>(-1);

public:
    ////// Element access -----------------------
    
    // Access character at index (no bounds checking)
    constexpr value_type operator[](size_type pos) const noexcept {
        return data_[pos];
    }
    // Access first character of the view
    constexpr value_type front() const noexcept {
        NEX_ASSERT_MSG(!empty(), "front() on empty StringView");
        return data_[0];
    }
    // Access last character of the view
    constexpr value_type back() const noexcept {
        NEX_ASSERT_MSG(!empty(), "back() on empty StringView");
        return data_[size_ - 1];
    }

    // Access character at index (with bounds checking)
    constexpr value_type at(size_type pos) const {
        NEX_ASSERT_MSG(pos < size_, "Position out of range");
        return data_[pos];
    }
    
    // Get pointer to the underlying data (UTF-16 encoded)
    constexpr const_pointer data() const noexcept { 
        return data_; 
    }

    ////// Basic properties -----------------------
    
    // Check if view is empty
    constexpr bool empty() const noexcept { 
        // size_ == 0 implies data_ can be null or non-null, 
        // but we consider it empty regardless
        return size_ == 0;
    }
    // Get size of the view
    constexpr size_type size() const noexcept { 
        return size_; 
    }
    // Get length of the view (same as size)
    constexpr size_type length() const noexcept { 
        return size(); 
    }

    ////// Iterators -----------------------

    // Get an iterator to the beginning of the view
    constexpr iterator begin() const noexcept {
        return data_;
    }

    // Get an iterator to the end of the view
    constexpr iterator end() const noexcept {
        return data_ + size_;
    }

    // Get a const iterator to the beginning of the view
    constexpr const_iterator cbegin() const noexcept {
        return data_;
    }

    // Get a const iterator to the end of the view
    constexpr const_iterator cend() const noexcept {
        return data_ + size_;
    }

    // Get a reverse iterator to the beginning of the reversed view (i.e., end of the normal view)
    constexpr reverse_iterator rbegin() const noexcept {
        return reverse_iterator(data_ + size_);
    }

    // Get a reverse iterator to the end of the reversed view (i.e., beginning of the normal view)
    constexpr reverse_iterator rend() const noexcept {
        return reverse_iterator(data_);
    }

    // Get a const reverse iterator to the beginning of the reversed view (i.e., end of the normal view)
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(data_ + size_);
    }

    // Get a const reverse iterator to the end of the reversed view (i.e., beginning of the normal view)
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(data_);
    }

    ////// Conversion -----------------------

    // Convert to String (makes a copy of the data)
    String toString() const;

    // Convert to std::u16string (makes a copy of the data)
    Utf16String toStdU16String() const {
        return Utf16String(data_, size_);
    }

    // Convert to std::u16string_view (returns a view of the same data)
    constexpr Utf16StringView toStdU16StringView() const noexcept {
        return Utf16StringView(data_, size_);
    }

    ////// Operations -----------------------

    // Copy substring to destination
    String copy(size_type count, size_type pos = 0) const;

    // Remove prefix from the view
    constexpr void removePrefix(size_type n) noexcept {
        if (n > size_) { n = size_; }
        data_ += n; size_ -= n;
    }

    // Remove suffix from the view
    constexpr void removeSuffix(size_type n) noexcept {
        if (n > size_) { n = size_; }
        size_ -= n;
    }

    ////// Subview operations -----------------------
    
    // Get substring view
    StringView substr(size_type pos, size_type count = npos) const;
    
    // Get left part of the view
    StringView left(size_type count) const noexcept;
    
    // Get right part of the view
    StringView right(size_type count) const noexcept;
    
    // Get middle part of the view
    StringView mid(size_type start, size_type count = npos) const noexcept;

    ////// Comparison -----------------------
    
    // Compare with another view
    constexpr int compare(StringView other) const noexcept  {
        int result = NEX_STD char_traits<char16>::compare(data_, other.data_, 0);
        if (result != 0) { return result; } // Data differs in the common prefix
        if (size() < other.size()) { return -1; } // Data is identical in the common prefix, but this view is shorter
        if (size() > other.size()) { return 1; } // Data is identical in the common prefix, but this view is longer
        return 0;  // Data is identical and sizes are equal, so the views are equal
    }
    
    // Compare substring with another view
    int compare(size_type pos, size_type count, StringView other) const;

    ////// Searching -----------------------

    // Find first occurrence of substring
    size_type find(StringView substring, size_type pos = 0) const;
    
    // Find first occurrence of character
    size_type find(value_type ch, size_type pos = 0) const;
    
    // Find last occurrence of substring
    size_type rfind(StringView substring, size_type pos = npos) const;
    
    // Find last occurrence of character
    size_type rfind(value_type ch, size_type pos = npos) const;
    
    // Find first occurrence of any character in the set
    size_type findFirstOf(StringView input, size_type pos = 0) const;
    
    // Find first occurrence of any character not in the set
    size_type findFirstNotOf(StringView input, size_type pos = 0) const;
    
    // Find last occurrence of any character in the set
    size_type findLastOf(StringView input, size_type pos = npos) const;
    
    // Find last occurrence of any character not in the set
    size_type findLastNotOf(StringView input, size_type pos = npos) const;
    
    // Check if view starts with prefix
    bool startsWith(StringView prefix) const;
    
    // Check if view starts with character
    constexpr bool startsWith(value_type ch) const noexcept {
        return !empty() && front() == ch;
    }
    
    // Check if view ends with suffix
    bool endsWith(StringView suffix) const;
    
    // Check if view ends with character
    constexpr bool endsWith(value_type ch) const noexcept {
        return !empty() && back() == ch;
    }
    
    // Check if view contains substring
    bool contains(StringView str) const noexcept {
        return find(str) != npos;
    }
    
    // Check if view contains character
    bool contains(value_type ch) const noexcept {
        return find(ch) != npos;
    }

    ////// Comparison operators
public:
    // Equality operator
    friend constexpr bool operator==(StringView lhs, StringView rhs) noexcept {
        return lhs.compare(rhs) == 0;
    }
    
    // Inequality operator
    friend constexpr bool operator!=(StringView lhs, StringView rhs) noexcept {
        return !(lhs == rhs);
    }
    
    // Less-than operator
    friend constexpr bool operator<(StringView lhs, StringView rhs) noexcept {
        return lhs.compare(rhs) < 0;
    }
    
    // Less-than-or-equal operator
    friend constexpr bool operator<=(StringView lhs, StringView rhs) noexcept {
        return !(rhs < lhs);
    }
    
    // Greater-than operator
    friend constexpr bool operator>(StringView lhs, StringView rhs) noexcept {
        return rhs < lhs;
    }
    
    // Greater-than-or-equal operator
    friend constexpr bool operator>=(StringView lhs, StringView rhs) noexcept {
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
            hash ^= static_cast<unsigned char>(sv[i]);
            hash *= kPrime;
        }
        return hash;
    }
};

NEX_CORE_NAMESPACE_END

/**
 * @brief Alias-lifting to make StringView available in the public API namespace without the core qualifier
 * @details 
 * This allows users to use nex::StringView instead of nex::core::StringView, while still keeping the implementation 
 * details hidden in the core namespace.
 */

NEX_NAMESPACE_BEGIN

/**
 * @class StringView
 * @brief A non-owning view into a UTF-16 string
 */
NEX_ALIAS_TYPE_FROM_LAYER(core, StringView)

/**
 * @struct StringViewHash
 * @brief Hash function for StringView to allow usage in unordered containers
 */
NEX_ALIAS_TYPE_FROM_LAYER(core, StringViewHash)

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
