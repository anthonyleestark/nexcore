/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <string>
#include <string_view>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/iterator.h"
#include "nex/base/string.h"
#include "nex/base/result.h"
#include "nex/base/assert_crash.h"
#include "nex/core/text/string_view.h"

NEX_NAMESPACE_BEGIN

/**
 * @class String
 * @brief A muti-encoding Unicode string class that supports UTF-8 and UTF-16.
 * 
 * This class provides a convenient way to store and manipulate Unicode strings.
 * It uses std::u16string (UTF-16) as the internal buffer to store string data.
 * 
 * String supports:
 * - Construction from UTF-8 and UTF-16 strings, as well as from StringView
 * - Creation from numbers (integers and floating-point) with specified formatting
 * - Conversion to/from UTF-8 and UTF-16 encodings
 * - Comparison and search operations
 * - Compatibility with StringView
 * 
 * @note The internal encoding is UTF-16, which allows for efficient storage of Unicode characters 
 *       while maintaining compatibility with Windows APIs and other UTF-16 based systems.
 * 
 * @see StringView for non-owning string views
 */
class NEX_API String {
public:
    // Forward declaration of iterators
    class Iterator;
    class ConstIterator;

    // Type aliases for compatibility with standard container conventions
    using value_type = char16;
    using size_type = usize;
    using difference_type = isize;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = Iterator;
    using const_iterator = ConstIterator;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;

private:
    // Using UTF-16 as the internal buffer
    Utf16String buffer_;

public:
     ////// Construction ------------------------

     // Default constructor (empty string)
    explicit String();

    // Construct from UTF-8 string view
    String(Utf8StringView utf8);

    // Construct from UTF-8 string literal
    String(cstring utf8);
    
    // Construct from UTF-16 string view
    String(Utf16StringView utf16);

    // Construct from UTF-16 string literal
    String(const char16* utf16);

    // Construct from StringView
    explicit String(StringView view);

    // Copy constructor
    String(const String& other) {
        buffer_.assign(other.buffer_);
    }

    // Copy assignment operator
    String& operator=(const String& other) {
        if (this != &other)
            buffer_.assign(other.buffer_);
        return *this;
    }

    // Move constructor
    String(String&& other) noexcept 
        : buffer_(NEX_MOVE(other.buffer_)) {}

    // Move assignment operator
    String& operator=(String&& other) noexcept {
        if (this != &other)
            buffer_ = NEX_MOVE(other.buffer_);
        return *this;
    }

    // Destructor
    constexpr ~String() = default;

public:
    ////// Create string from numbers -----------------------

    // Create string from a signed integer with specified base (default is 10)
    static String fromInt(int64 value, int32 base = 10);

    // Create string from an unsigned integer with specified base (default is 10)
    static String fromUInt(uint64 value, int32 base = 10);

    // Create string from a floating-point number
    static String fromFloat(float64 value, nchar format = 'g', int32 precision = 6);

    ////// Create string from specified encoding -----------------------

    // Create string from UTF-8 string
    static String fromUtf8(Utf8StringView utf8);

    // Create string from UTF-16 string
    static String fromUtf16(Utf16StringView utf16);

    ////// Convert to specified encoding (instance methods) -----------------------

    // Convert to UTF-8 string
    NEX_NODISCARD Result<Utf8String> toUtf8() const;

    // Convert to UTF-16 string
    NEX_NODISCARD Result<Utf16String> toUtf16() const;

private:
    // Internal constants
    static constexpr value_type kSpaceChar = u' ';
    static constexpr const value_type* kSpace = u" ";
    static constexpr const value_type* kEmpty = u"";
    
public:
    // Constant representing "not found" for find operations
    static constexpr size_type npos = static_cast<size_type>(-1);

public:
    ////// Subscript operators -----------------------

    // Access character at index (without bounds checking)
    constexpr reference operator[](size_type pos) {
        return buffer_[pos];
    }
    // Access character at index (without bounds checking, const version)
    constexpr const_reference operator[](size_type pos) const {
        return buffer_[pos];
    }

    ////// Concatenation operator -----------------------

    // Concatenate with another string
    String operator+(StringView view) const;

    // Concatenate with another string (in-place)
    String& operator+=(StringView view);

    // Concatenate with a character
    String operator+(value_type ch) const {
        String result(*this);
        result.append(ch);
        return result;
    }
    // Concatenate with a character (in-place)
    String& operator+=(value_type ch) {
        append(ch);
        return *this;
    }

    ////// Equality operators -----------------------

    // Equality operator with another string
    constexpr bool operator==(StringView other) const noexcept {
        return this->view() == other;
    }

    ////// Inequality operators -----------------------

    // Inequality operator with another string
    constexpr bool operator!=(StringView other) const noexcept {
        return !(operator==(other));
    }

    ////// Comparison operators -----------------------

    // Compare with another string (less than)
    constexpr bool operator<(StringView other) const noexcept {
        return this->view() < other;
    }
    // Compare with another string (greater than)
    constexpr bool operator>(StringView other) const noexcept {
        return this->view() > other;
    }
    // Compare with another string (less than or equal)
    constexpr bool operator<=(StringView other) const noexcept {
        return this->view() <= other;
    }
    // Compare with another string (greater than or equal)
    constexpr bool operator>=(StringView other) const noexcept {
        return this->view() >= other;
    }

    ////// Basic string properties -----------------------
    
    // Check if the string is empty
    constexpr bool empty() const noexcept {
        return buffer_.empty();
    }
    // Get the size of the string (number of characters)
    constexpr size_type size() const noexcept {
        return buffer_.size();
    }
    // Get the length of the string (number of characters)
    constexpr size_type length() const noexcept {
        return size();
    }
    // Get the capacity of the string (allocated storage)
    constexpr size_type capacity() const noexcept {
        return buffer_.capacity();
    }

    ////// Raw access -----------------------
    
    // View the internal string as a StringView
    constexpr StringView view() const noexcept  {
        return StringView(buffer_.data(), buffer_.size());
    }

    // Raw access to the internal buffer (UTF-16)
    constexpr const_pointer data() const noexcept {
        return buffer_.data();
    }

    ////// Element access -----------------------

    // Access character at index (with bounds checking)
    constexpr reference at(size_type pos) {
        NEX_ASSERT_MSG(pos < buffer_.size(), "Position out of range");
        return buffer_.at(pos);
    }
    // Access character at index (with bounds checking, const version)
    constexpr const_reference at(size_type pos) const {
        NEX_ASSERT_MSG(pos < buffer_.size(), "Position out of range");
        return buffer_.at(pos);
    }
    // Access first character
    constexpr const_reference front() const noexcept {
        return buffer_.front();
    }
    // Access last character
    constexpr const_reference back() const noexcept {
        return buffer_.back();
    }

    ////// Iterators -----------------------

    /**
     * @brief Iterator class for String
     * 
     * This iterator class provides a way to iterate over the characters in the String with read-write access.
     * The iterator supports standard iterator operations such as increment, decrement, dereference, and comparison.
     * 
     * @note The iterator operates on UTF-16 code units, which may not correspond to complete Unicode code points
     *       if the string contains characters outside the Basic Multilingual Plane (BMP).
     * 
     * @see ConstIterator for a const version of the iterator that provides read-only access to the characters.
     */
    class Iterator {
    public:
        // Type aliases for iterator traits (compatible with standard library iterators)
        using iterator_category = RandomAccessIteratorTag;
        using value_type = String::value_type;
        using difference_type = String::difference_type;
        using pointer = String::pointer;
        using const_pointer = String::const_pointer;
        using reference = String::reference;
        using const_reference = String::const_reference;

    private:
         // Using UTF-16 string pointer for iteration
        pointer ptr_;

        // Allow ConstIterator to access private members of Iterator
        friend class ConstIterator;

    public:
        // Construct an iterator from a UTF-16 string pointer
        constexpr explicit Iterator(pointer ptr) : ptr_(ptr) {}

        // Pointer operator for member access
        constexpr pointer operator->() { return ptr_; }
        constexpr const_pointer operator->() const { return ptr_; }

        // Dereference operators
        constexpr reference operator*() { return *ptr_; }
        constexpr const_reference operator*() const { return *ptr_; }

        // Iterator operations
        constexpr Iterator& operator++() { ++ptr_; return *this; }
        constexpr Iterator operator++(int32) { Iterator tmp = *this; ++ptr_; return tmp; }

        constexpr Iterator& operator--() { --ptr_; return *this; }
        constexpr Iterator operator--(int32) { Iterator tmp = *this; --ptr_; return tmp; }

        // Arithmetic operators for random access iterator
        constexpr Iterator operator+(difference_type offset) const { return Iterator(ptr_ + offset); }
        constexpr Iterator& operator+=(difference_type n) { ptr_ += n; return *this; }

        constexpr Iterator operator-(difference_type offset) const { return Iterator(ptr_ - offset); }
        constexpr Iterator& operator-=(difference_type n) { ptr_ -= n; return *this; }

        // Friend function for addition with difference_type on the left
        friend constexpr Iterator operator+(difference_type n, const Iterator& it) { 
            return Iterator(it.ptr_ + n); 
        }

        // Difference operator for random access iterator
        constexpr difference_type operator-(const Iterator& other) const { 
            return ptr_ - other.ptr_; 
        }

        // Subscript operators for random access iterator
        reference operator[](difference_type offset) { return ptr_[offset]; }
        const_reference operator[](difference_type offset) const { return ptr_[offset]; }

        // Equality operators for iterators
        constexpr bool operator==(const Iterator& other) const { return ptr_ == other.ptr_; }
        constexpr bool operator!=(const Iterator& other) const { return ptr_ != other.ptr_; }

        // Comparison operators for random access iterator
        constexpr bool operator<(const Iterator& other) const { return ptr_ < other.ptr_; }
        constexpr bool operator>(const Iterator& other) const { return ptr_ > other.ptr_; }
        constexpr bool operator<=(const Iterator& other) const { return ptr_ <= other.ptr_; }
        constexpr bool operator>=(const Iterator& other) const { return ptr_ >= other.ptr_; }
    };

    // Get an iterator to the beginning of the string
    constexpr iterator begin() noexcept {
        return iterator(buffer_.data());
    }

    // Get an iterator to the end of the string
    constexpr iterator end() noexcept {
        return iterator(buffer_.data() + buffer_.size());
    }

    /**
     * @brief ConstIterator class for String
     * 
     * This const iterator class provides a way to iterate over the characters in the String with read-only access.
     * The const iterator supports standard iterator operations such as increment, decrement, dereference, and comparison.
     * 
     * @note The const iterator operates on UTF-16 code units, which may not correspond to complete Unicode code points
     *       if the string contains characters outside the Basic Multilingual Plane (BMP).
     * 
     * @see Iterator for a non-const version of the iterator that provides read-write access to the characters.
     */
    class ConstIterator {

    public:
         // Type aliases for iterator traits (compatible with standard library iterators)
        using iterator_category = RandomAccessIteratorTag;
        using value_type = String::value_type;
        using difference_type = String::difference_type;
        using pointer = String::const_pointer;
        using const_pointer = String::const_pointer;
        using reference = String::const_reference;
        using const_reference = String::const_reference;

    private:
         // Using UTF-16 string const pointer for iteration
        const_pointer ptr_;

    public:
        // Construct a const iterator from a UTF-16 string const pointer
        constexpr explicit ConstIterator(const_pointer ptr) : ptr_(ptr) {}

        // Construct a const iterator from a non-const iterator
        constexpr ConstIterator(const Iterator& it) : ptr_(it.ptr_) {}

        // Pointer operator for member access
        constexpr pointer operator->() const { return ptr_; }

        // Dereference operators
        constexpr reference operator*() const { return *ptr_; }

        // Iterator operations
        constexpr ConstIterator& operator++() { ++ptr_; return *this; }
        constexpr ConstIterator operator++(int32) { ConstIterator tmp = *this; ++ptr_; return tmp; }

        constexpr ConstIterator& operator--() { --ptr_; return *this; }
        constexpr ConstIterator operator--(int32) { ConstIterator tmp = *this; --ptr_; return tmp; }

        // Arithmetic operators for random access iterator
        constexpr ConstIterator operator+(difference_type offset) const { return ConstIterator(ptr_ + offset); }
        constexpr ConstIterator& operator+=(difference_type n) { ptr_ += n; return *this; }

        constexpr ConstIterator operator-(difference_type offset) const { return ConstIterator(ptr_ - offset); }
        constexpr ConstIterator& operator-=(difference_type n) { ptr_ -= n; return *this; }

        // Friend function for addition with difference_type on the left
        friend constexpr ConstIterator operator+(difference_type n, const ConstIterator& it) { 
            return ConstIterator(it.ptr_ + n); 
        }

        // Difference operator for random access iterator
        constexpr difference_type operator-(const ConstIterator& other) const { 
            return ptr_ - other.ptr_; 
        }

        // Subscript operators for random access iterator
        constexpr reference operator[](difference_type offset) const { 
            return ptr_[offset]; 
        }

        // Equality operators for const iterators
        constexpr bool operator==(const ConstIterator& other) const { return ptr_ == other.ptr_;}
        constexpr bool operator!=(const ConstIterator& other) const { return ptr_ != other.ptr_; }

        // Comparison operators for random access iterator
        constexpr bool operator<(const ConstIterator& other) const { return ptr_ < other.ptr_; }
        constexpr bool operator>(const ConstIterator& other) const { return ptr_ > other.ptr_; }
        constexpr bool operator<=(const ConstIterator& other) const { return ptr_ <= other.ptr_; }
        constexpr bool operator>=(const ConstIterator& other) const { return ptr_ >= other.ptr_; }
    };

    // Get a const iterator to the beginning of the string
    constexpr const_iterator begin() const noexcept {
        return const_iterator(buffer_.data());
    }

    // Get a const iterator to the end of the string
    constexpr const_iterator end() const noexcept {
        return const_iterator(buffer_.data() + buffer_.size());
    }

    // Get a const iterator to the beginning of the string
    constexpr const_iterator cbegin() const noexcept {
        return const_iterator(buffer_.data());
    }

    // Get a const iterator to the end of the string
    constexpr const_iterator cend() const noexcept {
        return const_iterator(buffer_.data() + buffer_.size());
    }

    // Get a reverse iterator to the beginning of the reversed string (i.e., end of the normal string)
    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    // Get a reverse iterator to the end of the reversed string (i.e., beginning of the normal string)
    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    
    // Get a const reverse iterator to the beginning of the reversed string (i.e., end of the normal string)
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    // Get a const reverse iterator to the end of the reversed string (i.e., beginning of the normal string)
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // Get a const reverse iterator to the beginning of the reversed string (i.e., end of the normal string)
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    // Get a const reverse iterator to the end of the reversed string (i.e., beginning of the normal string)
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    ////// Size and capacity management -----------------------

    // Reserve capacity for the string
    void reserve(size_type newCapacity) {
        buffer_.reserve(newCapacity);
    }

    // Shrink the capacity to fit the size
    void shrinkToFit() {
        buffer_.shrink_to_fit();
    }

    // Resize the string to a new length, 
    // clear the entire string if new length is zero,
    // filling new characters with a specified character if the string is expanded
    String& resize(size_type newLength, value_type ch = value_type()) {
        buffer_.resize(newLength, ch);
        return *this;
    }

    ////// String manipulation & modification -----------------------

    // Append another string to this string
    String& append(StringView view);

    // Append a character
    String& append(value_type ch) {
        buffer_.append(1, ch);
        return *this;
    }

    // Push back a character
    String& pushBack(value_type ch) {
        buffer_.push_back(ch);
        return *this;
    }

    // Remove the last character
    String& popBack() {
        if (!buffer_.empty()) buffer_.pop_back();
        return *this;
    }

    // Remove all characters
    constexpr void clear() noexcept {
        buffer_.clear();
    }

    ////// Extract a part of the string -----------------------

    // Extract a substring from the string
    String substr(size_type pos = 0, size_type count = npos) const;

    ////// Searching -----------------------

    // Find the first match of a substring
    size_type find(StringView substring, size_type pos = 0) const;

    // Find the first match of a character
    size_type find(value_type ch, size_type pos = 0) const;

    // Find the last match of a substring
    size_type rfind(StringView substring, size_type pos = npos) const;

    // Find the last match of a character
    size_type rfind(value_type ch, size_type pos = npos) const;

    // Check if the string contains a substring
    bool contains(StringView substring) const;

    // Check if the string contains a character
    bool contains(value_type ch) const;

    ////// Comparison -----------------------

    // Compare two string (case-sensitive)
    // Returns:
    // - -1 if this string is less than the other string
    // - 0 if this string is equal to the other string
    // - 1 if this string is greater than the other string
    int32 compare(StringView other) const noexcept;
};

NEX_NAMESPACE_END
