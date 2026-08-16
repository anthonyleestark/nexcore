/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/string.h"
#include "nex/base/result.h"
#include "nex/base/assert_crash.h"
#include "nex/base/charconv.h"
#include "nex/base/encoding.h"
#include "nex/core/text/string_view.h"

NEX_NAMESPACE_BEGIN

/**
 * @class String
 * @brief An owning UTF-16 Unicode string class for the NexCore core layer.
 * 
 * @details
 * This class provides a convenient way to store and manipulate Unicode strings.
 * It uses BasicString<char16> as the internal buffer to store string data.
 * 
 * String supports:
 * - Construction from UTF-8 and UTF-16 strings, as well as from StringView
 * - Creation from numbers (integers and floating-point) with specified formatting
 * - Conversion to/from multiple encodings (UTF-8, UTF-16, UTF-32, ANSI, Local)
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
    using self = String;
    using self_view = StringView;
    using base_string = BasicString<char16>;
    using base_view = BasicStringView<char16>;

    using value_type = base_string::value_type;
    using size_type = base_string::size_type;
    using difference_type = base_string::difference_type;

    using char_type = base_string::char_type;
    using traits_type = base_string::traits_type;
    using buffer_type = base_string;

    using pointer = base_string::pointer;
    using const_pointer = base_string::const_pointer;
    using reference = base_string::reference;
    using const_reference = base_string::const_reference;

    using iterator = base_string::iterator;
    using const_iterator = self_view::const_iterator;

    // Representing an invalid position in the string or "not found" for find operations
    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    buffer_type buffer_;  // Underlying buffer that stores the UTF-16 string data

public:
    // Default constructor creates an empty string
    constexpr explicit String() noexcept = default;

    // Default copy constructor and copy assignment operator
    constexpr String(const String& other) = default;
    constexpr String& operator=(const String& other) = default;

    // Default move constructor and move assignment operator
    constexpr String(String&& other) noexcept = default;
    constexpr String& operator=(String&& other) noexcept = default;

    // Constructor that takes a string view of UTF-8 and converts it to UTF-16 internally.
    explicit String(Utf8StringView utf8) { buffer_ = fromUtf8(utf8).buffer_; }

    // Constructor that takes a pointer to a UTF-8 string and its length, converting it to UTF-16 internally.
    explicit String(u8cstring utf8, size_type length) {
        NEX_ASSERT_MSG(utf8 != nullptr, "Error: Cannot construct String from a null pointer");
        buffer_ = fromUtf8(Utf8StringView(utf8, length)).buffer_;
    }

    // Constructor that takes a null-terminated UTF-8 string and converts it to UTF-16 internally.
    explicit String(u8cstring utf8) {
        NEX_ASSERT_MSG(utf8 != nullptr, "Error: Cannot construct String from a null pointer");
        buffer_ = fromUtf8(Utf8StringView(utf8)).buffer_;
    }

    // Constructor that takes a string object of UTF-16 and initializes the internal buffer directly.
    constexpr String(const base_string& utf16) noexcept : buffer_(utf16) {}

    // Constructor that takes a string object of UTF-16 and moves it into the internal buffer.
    constexpr String(base_string&& utf16) noexcept : buffer_(NEX_MOVE(utf16)) {}

    // Constructor that takes a string view of UTF-16 and initializes the internal buffer directly.
    constexpr String(base_view utf16) noexcept
        : buffer_(utf16.data(), utf16.size()) {}

    // Constructor that takes a pointer to a UTF-16 string and its length, initializing the internal buffer directly.
    constexpr String(u16cstring utf16, size_type length) noexcept {
        NEX_ASSERT_MSG(utf16 != nullptr, "Error: Cannot construct String from a null pointer");
        buffer_ = buffer_type(utf16, length);
    }

    // Constructor that takes a null-terminated UTF-16 string and initializes the internal buffer directly.
    constexpr String(u16cstring utf16) noexcept {
        NEX_ASSERT_MSG(utf16 != nullptr, "Error: Cannot construct String from a null pointer");
        buffer_ = buffer_type(utf16);
    }

    // Constructor that takes a StringView and initializes the internal buffer directly.
    constexpr String(self_view view) noexcept
        : buffer_(view.data(), view.size()) {}

    // Destructor that cleans up the string data.
    constexpr ~String() noexcept = default;

private:
    // Creates a String from an ASCII string, converting each character to UTF-16 code unit.
    static constexpr self fromAscii(const nchar* ascii, size_type length) {
        NEX_ASSERT_MSG(ascii != nullptr, "Error: Cannot construct String from a null pointer");
        self result;
        result.resize(length);
        for (size_type i = 0; i < length; ++i) {
            result.buffer_[i] = static_cast<char_type>(ascii[i]);
        }
        return result;
    }

public:
    // Creates string from a signed integer with specified base (default is 10)
    NEX_NODISCARD static constexpr self fromInt(int64 value, int32 base = BASE_DECIMAL) {
        nchar buffer[65]; // Enough to hold the largest 64-bit integer in binary plus null terminator
        const usize length = charconv::formatInteger(buffer, sizeof(buffer), value, base);
        if (length > 0) {
            return fromAscii(buffer, length);
        } else {
            NEX_ASSERT_MSG(false, "Error: Unable to format integer to string");
            return self();
        }
    }

    // Creates string from an unsigned integer with specified base (default is 10)
    NEX_NODISCARD static constexpr self fromUInt(uint64 value, int32 base = BASE_DECIMAL) {
        nchar buffer[65]; // Enough to hold the largest 64-bit unsigned integer in binary plus null terminator
        const usize length = charconv::formatUnsigned(buffer, sizeof(buffer), value, base);
        if (length > 0) {
            return fromAscii(buffer, length);
        } else {
            NEX_ASSERT_MSG(false, "Error: Unable to format unsigned integer to string");
            return self();
        }
    }

    // Creates string from a floating-point number
    NEX_NODISCARD static self fromFloat(float64 value, nchar format = 'g', int32 precision = DEFAULT_FLOAT_PRECISION) {
        nchar buffer[128]; // Buffer to hold the formatted floating-point number
        const usize length = charconv::formatFloating(buffer, sizeof(buffer), value, precision);
        if (length > 0) {
            return fromAscii(buffer, length);
        } else {
            NEX_ASSERT_MSG(false, "Error: Unable to format floating-point number to string");
            return self();
        }
    }

    // Creates string from a UTF-8 string view data
    NEX_NODISCARD static self fromUtf8(Utf8StringView utf8) {
        auto result = encoding::utf8ToUtf16(utf8);
        if (result.isOk()) {
            return fromUtf16(result.value().asView());
        } else {
            NEX_ASSERT_MSG(false, "Error: Unable to convert UTF-8 to UTF-16 string");
            return self();
        }
    }

    // Creates string from a UTF-16 string view data
    NEX_NODISCARD static constexpr self fromUtf16(Utf16StringView utf16) { return self(utf16); }

    // Creates string from a UTF-32 string view data
    NEX_NODISCARD static self fromUtf32(Utf32StringView utf32) {
        auto result = encoding::utf32ToUtf16(utf32);
        if (result.isOk()) {
            return fromUtf16(result.value().asView());
        } else {
            NEX_ASSERT_MSG(false, "Error: Unable to convert UTF-32 to UTF-16 string");
            return self();
        }
    }

    // Creates string from an ANSI string view data
    NEX_NODISCARD static self fromAnsi(AnsiStringView ansi) {
        auto result = encoding::ansiToUtf16(ansi);
        if (result.isOk()) {
            return fromUtf16(result.value().asView());
        } else {
            NEX_ASSERT_MSG(false, "Error: Unable to convert ANSI to UTF-16 string");
            return self();
        }
    }

    // Creates string from a Local string view data
    NEX_NODISCARD static self fromLocal(LocalStringView local) {
        auto result = encoding::localToUtf16(local);
        if (result.isOk()) {
            return fromUtf16(result.value().asView());
        } else {
            NEX_ASSERT_MSG(false, "Error: Unable to convert Local to UTF-16 string");
            return self();
        }
    }

    // Converts the string to a UTF-8 string, returning a Result that may contain an error if the conversion fails.
    NEX_NODISCARD Result<Utf8String> toUtf8() const { return encoding::utf16ToUtf8(buffer_.asView()); }

    // Converts the string to a UTF-16 string, returning a Result that may contain an error if the conversion fails.
    NEX_NODISCARD constexpr Result<Utf16String> toUtf16() const { return ok(buffer_); }

    // Converts the string to a UTF-32 string, returning a Result that may contain an error if the conversion fails.
    NEX_NODISCARD Result<Utf32String> toUtf32() const { return encoding::utf16ToUtf32(buffer_.asView()); }

    // Converts the string to an ANSI string, returning a Result that may contain an error if the conversion fails.
    NEX_NODISCARD Result<AnsiString> toAnsi() const { return encoding::utf16ToAnsi(buffer_.asView()); }

    // Converts the string to a Local string, returning a Result that may contain an error if the conversion fails.
    NEX_NODISCARD Result<LocalString> toLocal() const { return encoding::utf16ToLocal(buffer_.asView()); }

    // Returns the number of characters in the string, not including the null terminator.
    NEX_NODISCARD constexpr size_type size() const noexcept { return buffer_.size(); }

    // Returns the number of characters in the string, not including the null terminator.
    NEX_NODISCARD constexpr size_type length() const noexcept { return buffer_.length(); }

    // Determines whether the string is empty (contains no characters).
    NEX_NODISCARD constexpr bool empty() const noexcept { return buffer_.empty(); }

    // Returns the maximum number of content characters that can be stored in the string.
    NEX_NODISCARD static constexpr size_type maxSize() noexcept { return base_string::maxSize(); }

    // Returns the current capacity of the string,
    // which is the number of content characters that can be stored without requiring a reallocation.
    NEX_NODISCARD constexpr size_type capacity() const noexcept { return buffer_.capacity(); }

    // Reserves capacity for at least newCapacity content characters.
    constexpr void reserve(size_type newCapacity) { buffer_.reserve(newCapacity); }

    // Reduces capacity to fit the current size, potentially releasing heap storage.
    constexpr void shrinkToFit() { buffer_.shrinkToFit(); }

    // Resizes the string to contain newSize content characters, value-initializing added characters.
    constexpr void resize(size_type newSize) { buffer_.resize(newSize); }

    // Resizes the string to contain newSize content characters, filling added characters with value.
    constexpr void resize(size_type newSize, char_type ch) { buffer_.resize(newSize, ch); }

    // Returns a writable pointer to the string's content, including a NUL terminator.
    NEX_NODISCARD constexpr pointer data() noexcept { return buffer_.data(); }

    // Returns a read-only pointer to the string's content, including a NUL terminator.
    NEX_NODISCARD constexpr const_pointer data() const noexcept { return buffer_.data(); }

    // Returns a read-only pointer to the string's content, including a NUL terminator.
    NEX_NODISCARD constexpr const_pointer cStr() const noexcept { return buffer_.cStr(); }

    // Returns a mutable iterator to the first character in the string.
    NEX_NODISCARD constexpr iterator begin() noexcept { return buffer_.begin(); }

    // Returns an immutable const iterator to the first character in the string.
    NEX_NODISCARD constexpr const_iterator begin() const noexcept { return buffer_.begin(); }

    // Returns a mutable iterator to one past the last character in the string.
    NEX_NODISCARD constexpr iterator end() noexcept { return buffer_.end(); }

    // Returns an immutable const iterator to one past the last character in the string.
    NEX_NODISCARD constexpr const_iterator end() const noexcept { return buffer_.end(); }

    // Returns an immutable const iterator to the first character in the string.
    NEX_NODISCARD constexpr const_iterator cbegin() const noexcept { return buffer_.cbegin(); }

    // Returns an immutable const iterator to one past the last character in the string.
    NEX_NODISCARD constexpr const_iterator cend() const noexcept { return buffer_.cend(); }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD constexpr reference at(size_type pos) noexcept { return buffer_.at(pos); }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD constexpr const_reference at(size_type pos) const noexcept { return buffer_.at(pos); }

    // Returns the character at position without bounds checking.
    NEX_NODISCARD constexpr reference operator[](size_type pos) { return buffer_[pos]; }

    // Returns the character at position without bounds checking.
    NEX_NODISCARD constexpr const_reference operator[](size_type pos) const { return buffer_[pos]; }

    // Returns the first content character.
    NEX_NODISCARD constexpr reference front() noexcept { return buffer_.front(); }

    // Returns the first content character.
    NEX_NODISCARD constexpr const_reference front() const noexcept { return buffer_.front(); }

    // Returns the last content character.
    NEX_NODISCARD constexpr reference back() noexcept { return buffer_.back(); }

    // Returns the last content character.
    NEX_NODISCARD constexpr const_reference back() const noexcept { return buffer_.back(); }

    // Returns a non-owning view of the string's content as a StringView.
    NEX_NODISCARD constexpr self_view asView() const noexcept { return self_view(buffer_.data(), buffer_.size()); }

    // Returns a non-owning view of the string's content as a UTF-16 string view.
    NEX_NODISCARD constexpr base_view asUtf16() const noexcept { return base_view(buffer_.data(), buffer_.size()); }

    // Assigns the content of another string object to this string, replacing its content.
    constexpr self& assign(const self& other) noexcept { return *this = other; }

    // Moves the content of another string object to this string, replacing its content.
    constexpr self& assign(self&& other) noexcept {
        *this = NEX_MOVE(other);
        return *this;
    }

    // Assigns a substring of another string object to this string, replacing its content.
    constexpr self& assign(const self& other, size_type position, size_type count = npos) {
        buffer_.assign(other.buffer_, position, count);
        return *this;
    }

    // Replaces the string's content with exactly count characters from source.
    constexpr self& assign(const_pointer source, size_type count) {
        buffer_.assign(source, count);
        return *this;
    }

    // Replaces the string's content with a null-terminated character sequence.
    constexpr self& assign(const_pointer source) {
        buffer_.assign(source, traits_type::length(source));
        return *this;
    }

    // Replaces the string's content with a string view, copying its content.
    constexpr self& assign(self_view view) {
        buffer_.assign(view.data(), view.size());
        return *this;
    }

    // Replaces the string's content with count copies of character.
    constexpr self& assign(size_type count, char_type ch) {
        buffer_.assign(count, ch);
        return *this;
    }

    // Clears the string's content while retaining the current allocation and terminator slot.
    constexpr void clear() noexcept { buffer_.clear(); }

    // Resets the string's content and deallocates any allocated memory.
    constexpr void reset() noexcept { buffer_.reset(); }

    // Appends content from a string view to the end of the string.
    constexpr void append(self_view view) { buffer_.append(view.data(), view.size()); }

    // Appends exactly count characters from source to the end of the string.
    constexpr void append(const_pointer source, size_type count) {
        buffer_.append(source, count);
    }

    // Appends a null-terminated character sequence to the end of the string.
    constexpr void append(const_pointer source) {
        buffer_.append(source, traits_type::length(source));
    }

    // Appends count copies of character to the end of the string.
    constexpr void append(size_type count, char_type ch) { buffer_.append(count, ch); }

    // Appends a single character to the end of the string.
    constexpr void append(char_type ch) { buffer_.append(ch); }

    // Appends a single character to the end of the string.
    constexpr void pushBack(char_type ch) { buffer_.pushBack(ch); }

    // Removes the last character from the string.
    constexpr void popBack() noexcept { buffer_.popBack(); }

    // Inserts count characters from source before the specified position in the string.
    constexpr void insert(size_type position, const_pointer source, size_type count) {
        buffer_.insert(position, source, count);
    }

    // Inserts content from a BasicStringView at the specified position in the string.
    constexpr void insert(size_type position, self_view view) {
        buffer_.insert(position, view.data(), view.size());
    }

    // Replaces up to removedCount characters at position with count characters from source.
    constexpr void replace(size_type position, size_type removedCount, const_pointer source, size_type count ) {
        buffer_.replace(position, removedCount, source, count);
    }

    // Replaces up to removedCount characters at position with content from a string view.
    constexpr void replace(size_type position, size_type removedCount, self_view view) {
        buffer_.replace(position, removedCount, view.data(), view.size());
    }

    // Removes up to count characters starting at position.
    constexpr void erase(size_type position, size_type count = 1) noexcept {
        buffer_.erase(position, count);
    }

    // Returns a substring of the string starting at position and containing up to count characters.
    constexpr self substr(size_type pos = 0, size_type count = npos) const {
        return self(NEX_MOVE(buffer_.substr(pos, count)));
    }

    // Concatenates two string objects and returns the result.
    constexpr self operator+(const self& other) const {
        self result(*this);
        result.append(other.asView());
        return result;
    }

    // Appends another string object to this string and returns the result.
    constexpr self& operator+=(const self& other) {
        append(other.asView());
        return *this;
    }

    // Concatenates a string object with a string view and returns the result.
    constexpr self operator+(self_view view) const {
        self result(*this);
        result.append(view);
        return result;
    }

    // Appends a string view to this string and returns the result.
    constexpr self& operator+=(self_view view) {
        append(view);
        return *this;
    }

    // Concatenates a string object with a character and returns the result.
    constexpr self operator+(char_type ch) const {
        self result(*this);
        result.append(ch);
        return result;
    }

    // Appends a character to this string and returns the result.
    constexpr self& operator+=(char_type ch) {
        append(ch);
        return *this;
    }

    // Exchanges the content and storage representation with another string object.
    constexpr void swap(self& other) noexcept { buffer_.swap(other.buffer_); }

    // Compares this string with another string object lexicographically.
    NEX_NODISCARD constexpr int32 compare(const self& other) const noexcept {
        return this->asView().compare(other.asView());
    }

    NEX_NODISCARD constexpr int32 compare(self_view view) const noexcept {
        return this->asView().compare(view);
    }

    // Compares this string with another string object for equality.
    constexpr bool operator==(const self& other) const noexcept {
        return this->asView() == other.asView();
    }

    // Compares this string with another string view for equality.
    constexpr bool operator==(self_view other) const noexcept {
        return this->asView() == other;
    }

    // Compares this string with another string object for inequality.
    constexpr bool operator!=(const self& other) const noexcept {
        return !(operator==(other));
    }

    // Compares this string with another string view for inequality.
    constexpr bool operator!=(self_view other) const noexcept {
        return !(operator==(other));
    }

    // Compares this string with another string object for lexicographical ordering.
    constexpr bool operator<(const self& other) const noexcept {
        return this->asView() < other.asView();
    }

    // Compares this string with another string view for lexicographical ordering.
    constexpr bool operator<(self_view other) const noexcept {
        return this->asView() < other;
    }

    // Compares this string with another string object for lexicographical ordering.
    constexpr bool operator>(const self& other) const noexcept {
        return this->asView() > other.asView();
    }

    // Compares this string with another string view for lexicographical ordering.
    constexpr bool operator>(self_view other) const noexcept {
        return this->asView() > other;
    }

    // Compares this string with another string object for lexicographical ordering.
    constexpr bool operator<=(const self& other) const noexcept {
        return this->asView() <= other.asView();
    }

    // Compares this string with another string view for lexicographical ordering.
    constexpr bool operator<=(self_view other) const noexcept {
        return this->asView() <= other;
    }

    // Compares this string with another string object for lexicographical ordering.
    constexpr bool operator>=(const self& other) const noexcept {
        return this->asView() >= other.asView();
    }

    // Compares this string with another string view for lexicographical ordering.
    constexpr bool operator>=(self_view other) const noexcept {
        return this->asView() >= other;
    }

    // Finds the first occurrence of a substring, starting from a given position.
    constexpr size_type find(self_view substring, size_type pos = 0) const {
        return this->asView().find(substring, pos);
    }

    // Finds the first occurrence of a character, starting from a given position.
    constexpr size_type find(value_type ch, size_type pos = 0) const {
        return this->asView().find(ch, pos);
    }

    // Finds the last occurrence of a substring, starting from a given position.
    constexpr size_type rfind(self_view substring, size_type pos = npos) const {
        return this->asView().rfind(substring, pos);
    }

    // Finds the last occurrence of a character, starting from a given position.
    constexpr size_type rfind(value_type ch, size_type pos = npos) const {
        return this->asView().rfind(ch, pos);
    }

    // Determines whether the string contains the specified substring.
    constexpr bool contains(self_view substring) const {
        return this->asView().contains(substring);
    }

    // Determines whether the string contains the specified character.
    constexpr bool contains(value_type ch) const {
        return this->asView().contains(ch);
    }
};

NEX_NAMESPACE_END
