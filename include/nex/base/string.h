/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"
#include "nex/base/init.h"
#include "nex/base/growth.h"
#include "nex/base/memory.h"
#include "nex/base/algorithm.h"
#include "nex/base/assert_crash.h"
#include "nex/base/char_traits.h"
#include "nex/base/string_view.h"

NEX_NAMESPACE_BEGIN

// Default inline capacity for BasicString storage
static constexpr usize BasicStringDefaultInlineCapacity = 23;

/**
 * @class BasicStringStorage
 * @brief Owns null-terminated character storage with a small-string optimization.
 *
 * @details
 * The class stores short strings directly in a union member and switches that union to a heap
 * pointer only when the content exceeds @p InlineCapacity.
 * Capacity and size describe content characters only: the trailing null character is always
 * allocated separately and is never included in either value.
 *
 * @tparam Type Character type. It must be trivially copyable because this is raw storage used
 *         by BasicString, not an owning container for general objects.
 * @tparam InlineCapacity Number of content characters retained inline.
 * @tparam Growth Policy used to grow heap capacity.
 */
template <
    typename Type,
    usize InlineCapacity = BasicStringDefaultInlineCapacity,
    GrowthPolicy Growth = DoubleGrowth
> requires (InlineCapacity > 0)
class NEX_HIDDEN_FROM_ABI BasicStringStorage {
public:
    using value_type = Type;
    using size_type = usize;
    using difference_type = isize;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    // The number of content characters that can be stored inline without heap allocation.
    static constexpr size_type inlineCapacity = InlineCapacity;

private:
    // The character type must be trivially copyable because this is raw storage used by BasicString,
    // not an owning container for general objects.
    static_assert(meta::IsTriviallyCopyableV<value_type>,
        "Error: BasicStringStorage requires a trivially copyable character type");

    /**
     * @note
     * SSO keeps short content in shortData and switches the same memory to longData
     * only after overflowing InlineCapacity. This union avoids paying for both
     * representations at once while size_, capacity_, and isShort_ retain the metadata
     * required to identify the active layout.
     */
    union Storage {
        value_type shortData[inlineCapacity + 1];   // Inline storage for short strings.
        pointer longData;                           // Pointer to heap storage for long strings.

        // Initializes the active union member as zeroed inline storage.
        constexpr Storage() noexcept : shortData{} {}

        // Trivial character storage needs no explicit destruction.
        ~Storage() = default;
    } storage_;

    // The terminator is not counted by either size_ or capacity_.
    size_type size_ = 0;
    size_type capacity_ = inlineCapacity;
    bool isShort_ = true;

    // Returns the inline storage as a writable character pointer.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr pointer shortData() noexcept {
        return storage_.shortData;
    }

    // Returns the inline storage as a read-only character pointer.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_pointer shortData() const noexcept {
        return storage_.shortData;
    }

    // Returns the active storage as a writable character pointer.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr pointer mutableData() noexcept {
        return isShort_ ? shortData() : storage_.longData;
    }

    // Returns the active storage as a read-only character pointer.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_pointer mutableData() const noexcept {
        return isShort_ ? shortData() : storage_.longData;
    }

    // Restores the NUL terminator immediately after the content range.
    NEX_HIDDEN_FROM_ABI constexpr void writeTerminator() noexcept {
        mutableData()[size_] = value_type{};
    }

    // Activates the inline union member and resets it to an empty string.
    NEX_HIDDEN_FROM_ABI constexpr void becomeEmptyShort() noexcept {
        storage_.shortData[0] = value_type{};
        size_ = 0;
        capacity_ = inlineCapacity;
        isShort_ = true;
    }

    // Allocates heap storage including one extra character for the NUL terminator.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI static pointer allocate(size_type capacity) noexcept(false) {
        NEX_ASSERT_MSG(capacity <= maxSize(), "Error: String capacity exceeds maximum size");
        return static_cast<pointer>(::operator new((capacity + 1) * sizeof(value_type)));
    }

    // Releases heap storage previously returned by allocate().
    NEX_HIDDEN_FROM_ABI static void deallocate(pointer data) noexcept {
        ::operator delete(static_cast<void*>(data));
    }

    /**
     * @brief Moves the active storage to a heap allocation of @p newCapacity.
     * @pre newCapacity is at least size() and no greater than maxSize().
     * @details The character range and its NUL terminator are relocated before
     *          the previous heap allocation is released.
     */
    NEX_HIDDEN_FROM_ABI constexpr void reallocate(size_type newCapacity) {
        NEX_ASSERT_MSG(newCapacity >= size_ && newCapacity <= maxSize(),
            "Error: Invalid string reallocation capacity");

        pointer const oldData = mutableData();
        const bool wasShort = isShort_;
        pointer const newData = allocate(newCapacity);

        // Relocate the terminator as well as the content.
        NEX_RELOCATE_RANGE(newData, oldData, oldData + size_ + 1);
        if (!wasShort) {
            deallocate(storage_.longData);
        }

        storage_.longData = newData;
        capacity_ = newCapacity;
        isShort_ = false;
    }

    // Moves heap content back into the inline union member.
    NEX_HIDDEN_FROM_ABI constexpr void moveToShortStorage() noexcept {
        NEX_ASSERT_MSG(!isShort_ && size_ <= inlineCapacity,
            "Error: Heap storage cannot be moved into inline storage");

        pointer const oldData = storage_.longData;
        NEX_RELOCATE_RANGE(storage_.shortData, oldData, oldData + size_ + 1);
        deallocate(oldData);
        capacity_ = inlineCapacity;
        isShort_ = true;
    }

    // Grows heap capacity according to the configured growth policy.
    NEX_HIDDEN_FROM_ABI constexpr void grow(size_type requiredCapacity) {
        NEX_ASSERT_MSG(requiredCapacity > capacity_ && requiredCapacity <= maxSize(),
            "Error: Invalid string growth capacity");
        const size_type newCapacity = Growth::grow(capacity_, requiredCapacity, maxSize());
        NEX_ASSERT_MSG(newCapacity >= requiredCapacity && newCapacity <= maxSize(),
            "Error: Growth policy returned an invalid string capacity");
        reallocate(newCapacity);
    }

    // Returns the offset when source points into this storage. Equality
    // comparison is well-defined for unrelated pointers, unlike ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool sourceOffset(
        const_pointer source,
        size_type count,
        size_type& offset
    ) const noexcept {
        if (source == nullptr || count == 0) return false;
        for (size_type index = 0; index <= size_; ++index) {
            if (source == mutableData() + index) {
                NEX_ASSERT_MSG(count <= size_ - index,
                    "Error: Source range exceeds string storage bounds");
                offset = index;
                return true;
            }
        }
        return false;
    }

    // Copies another storage object's content and terminator into this object.
    NEX_HIDDEN_FROM_ABI constexpr void copyFrom(const BasicStringStorage& other) {
        reserve(other.size_);
        NEX_ASSIGN_RANGE(mutableData(), other.mutableData(), other.mutableData() + other.size_ + 1);
        size_ = other.size_;
    }

    // Moves inline data or takes ownership of another object's heap allocation.
    NEX_HIDDEN_FROM_ABI constexpr void moveFrom(BasicStringStorage&& other) noexcept {
        if (other.isShort_) {
            NEX_ASSIGN_RANGE(shortData(), other.shortData(), other.shortData() + other.size_ + 1);
            size_ = other.size_;
            other.clear();
            return;
        }

        storage_.longData = other.storage_.longData;
        size_ = other.size_;
        capacity_ = other.capacity_;
        isShort_ = false;
        other.becomeEmptyShort();
    }

public:
    // Creates empty inline storage containing only a NUL terminator.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringStorage() noexcept = default;

    // Copies another storage object's content and storage representation.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringStorage(const BasicStringStorage& other) {
        copyFrom(other);
    }

    // Replaces this object's content with a copy of another storage object.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringStorage& operator=(const BasicStringStorage& other) {
        if (this != &other) {
            assign(other.data(), other.size());
        }
        return *this;
    }

    // Moves another storage object's content into this object.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringStorage(BasicStringStorage&& other) noexcept {
        moveFrom(NEX_MOVE(other));
    }

    // Releases current content and moves another storage object into this one.
    NEX_HIDDEN_FROM_ABI constexpr BasicStringStorage& operator=(BasicStringStorage&& other) noexcept {
        if (this != &other) {
            reset();
            moveFrom(NEX_MOVE(other));
        }
        return *this;
    }

    // Releases the active heap allocation, if any.
    NEX_HIDDEN_FROM_ABI constexpr ~BasicStringStorage() noexcept {
        if (!isShort_) {
            deallocate(storage_.longData);
        }
    }

    // Returns the number of content characters, excluding the NUL terminator.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr size_type size() const noexcept { return size_; }

    // Determines whether this storage contains no content characters.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool empty() const noexcept { return size_ == 0; }

    // Returns content capacity, excluding the always-reserved NUL terminator.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr size_type capacity() const noexcept { return capacity_; }

    // Determines whether the active union member is the inline buffer.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool usingInlineStorage() const noexcept { return isShort_; }

    // Returns the largest content size that still leaves room for a NUL terminator.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI static constexpr size_type maxSize() noexcept {
        return (NumericLimits<size_type>::max() / sizeof(value_type)) - 1;
    }

    // Returns writable active storage. Call commitSize() after extending content.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr pointer data() noexcept { return mutableData(); }

    // Returns read-only active storage with a NUL terminator at data()[size()].
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_pointer data() const noexcept { return mutableData(); }

    // Returns the read-only NUL-terminated character sequence.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_pointer cStr() const noexcept { return mutableData(); }

    // Reserves capacity for at least newCapacity content characters.
    NEX_HIDDEN_FROM_ABI constexpr void reserve(size_type newCapacity) {
        NEX_ASSERT_MSG(newCapacity <= maxSize(), "Error: Requested string capacity exceeds maximum size");
        if (newCapacity > capacity_) {
            grow(newCapacity);
        }
    }

    /**
     * @brief Commits content written through data() and restores its terminator.
     * @pre newSize does not exceed capacity() and excludes the NUL terminator.
     */
    NEX_HIDDEN_FROM_ABI constexpr void commitSize(size_type newSize) noexcept {
        NEX_ASSERT_MSG(newSize <= capacity_, "Error: Committed string size exceeds capacity");
        size_ = newSize;
        writeTerminator();
    }

    // Resizes content, value-initializing added characters.
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type newSize) {
        resize(newSize, value_type{});
    }

    // Resizes content, filling added characters with value.
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type newSize, value_type value) {
        if (newSize > size_) {
            reserve(newSize);
            // NEX_FILL_RANGE is byte-wise and cannot preserve a non-zero
            // char16/char32 value, so fill character elements directly.
            for (size_type index = size_; index < newSize; ++index) {
                mutableData()[index] = value;
            }
        }
        size_ = newSize;
        writeTerminator();
    }

    // Replaces content with exactly count characters from source.
    NEX_HIDDEN_FROM_ABI constexpr void assign(const_pointer source, size_type count) {
        NEX_ASSERT_MSG(source != nullptr || count == 0, 
            "Error: Source must not be null when count is non-zero");
        if (count == 0) {
            clear();
            return;
        }

        size_type sourceIndex = 0;
        if (sourceOffset(source, count, sourceIndex)) {
            NEX_MEMMOVE(mutableData(), mutableData() + sourceIndex, count);
            size_ = count;
            writeTerminator();
            return;
        }

        reserve(count);
        NEX_ASSIGN_RANGE(mutableData(), source, source + count);
        size_ = count;
        writeTerminator();
    }

    // Replaces content with count copies of value.
    NEX_HIDDEN_FROM_ABI constexpr void assign(size_type count, value_type value) {
        clear();
        resize(count, value);
    }

    // Appends exactly count characters; appending a subrange of self is supported.
    NEX_HIDDEN_FROM_ABI constexpr void append(const_pointer source, size_type count) {
        NEX_ASSERT_MSG(source != nullptr || count == 0, 
            "Error: Source must not be null when count is non-zero");
        if (count == 0) return;
        NEX_ASSERT_MSG(count <= maxSize() - size_, "Error: Appended content exceeds maximum string size");

        size_type sourceIndex = 0;
        const bool aliasesStorage = sourceOffset(source, count, sourceIndex);
        reserve(size_ + count);
        if (aliasesStorage) {
            source = mutableData() + sourceIndex;
        }

        NEX_COPY_RANGE(mutableData() + size_, source, source + count);
        size_ += count;
        writeTerminator();
    }

    // Appends count copies of value.
    NEX_HIDDEN_FROM_ABI constexpr void append(size_type count, value_type value) {
        NEX_ASSERT_MSG(count <= maxSize() - size_, "Error: Appended content exceeds maximum string size");
        resize(size_ + count, value);
    }

    // Appends one character, including an embedded NUL when requested.
    NEX_HIDDEN_FROM_ABI constexpr void append(value_type value) {
        reserve(size_ + 1);
        mutableData()[size_++] = value;
        writeTerminator();
    }

    // Appends one character to the end of the content.
    NEX_HIDDEN_FROM_ABI constexpr void pushBack(value_type value) {
        append(value);
    }

    // Removes the final content character.
    NEX_HIDDEN_FROM_ABI constexpr void popBack() noexcept {
        NEX_ASSERT_MSG(!empty(), "Error: Cannot pop from an empty string storage");
        --size_;
        writeTerminator();
    }

    // Inserts count characters from source before position.
    NEX_HIDDEN_FROM_ABI constexpr void insert(
        size_type position,
        const_pointer source,
        size_type count
    ) {
        replace(position, 0, source, count);
    }

    /**
     * @brief Replaces up to removedCount characters at position with source.
     * @details A source range that aliases this storage is copied to temporary
     *          storage before the content range is shifted.
     */
    NEX_HIDDEN_FROM_ABI constexpr void replace(
        size_type position,
        size_type removedCount,
        const_pointer source,
        size_type count
    ) {
        NEX_ASSERT_MSG(position <= size_, "Error: Replace position is out of bounds");
        NEX_ASSERT_MSG(source != nullptr || count == 0,
            "Error: Source must not be null when count is non-zero");

        const size_type removable = size_ - position;
        const size_type removed = removedCount < removable ? removedCount : removable;
        NEX_ASSERT_MSG(count <= maxSize() - (size_ - removed),
            "Error: Replaced content exceeds maximum string size");

        BasicStringStorage sourceCopy;
        size_type sourceIndex = 0;
        if (sourceOffset(source, count, sourceIndex)) {
            sourceCopy.assign(source, count);
            source = sourceCopy.data();
        }

        const size_type newSize = size_ - removed + count;
        reserve(newSize);
        NEX_MEMMOVE(
            mutableData() + position + count,
            mutableData() + position + removed,
            (removable - removed + 1)
        );
        if (count > 0) {
            NEX_COPY_RANGE(mutableData() + position, source, source + count);
        }
        size_ = newSize;
        writeTerminator();
    }

    // Removes up to count content characters starting at position.
    NEX_HIDDEN_FROM_ABI constexpr void erase(size_type position, size_type count = 1) noexcept {
        replace(position, count, nullptr, 0);
    }

    // Clears content while retaining the current allocation and terminator slot.
    NEX_HIDDEN_FROM_ABI constexpr void clear() noexcept {
        size_ = 0;
        writeTerminator();
    }

    // Releases heap storage and restores the initial inline representation.
    NEX_HIDDEN_FROM_ABI constexpr void reset() noexcept {
        if (!isShort_) {
            deallocate(storage_.longData);
        }
        becomeEmptyShort();
    }

    // Reduces heap allocation to size() or returns to inline storage.
    NEX_HIDDEN_FROM_ABI constexpr void shrinkToFit() {
        if (isShort_) return;
        if (size_ <= inlineCapacity) {
            moveToShortStorage();
        } else if (size_ < capacity_) {
            reallocate(size_);
        }
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    reference at(size_type position) noexcept {
        NEX_VERIFY_MSG(position < size_, "Error: String index is out of bounds");
        return mutableData()[position];
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    const_reference at(size_type position) const noexcept {
        NEX_VERIFY_MSG(position < size_, "Error: String index is out of bounds");
        return mutableData()[position];
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    reference operator[](size_type position) noexcept {
        return mutableData()[position];
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    const_reference operator[](size_type position) const noexcept {
        return mutableData()[position];
    }

    // Returns the first content character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr reference front() noexcept {
        NEX_ASSERT_MSG(!empty(), "Error: Cannot access front of an empty string storage");
        return mutableData()[0];
    }

    // Returns the first content character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference front() const noexcept {
        NEX_ASSERT_MSG(!empty(), "Error: Cannot access front of an empty string storage");
        return mutableData()[0];
    }

    // Returns the final content character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr reference back() noexcept {
        NEX_ASSERT_MSG(!empty(), "Error: Cannot access back of an empty string storage");
        return mutableData()[size_ - 1];
    }

    // Returns the final content character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference back() const noexcept {
        NEX_ASSERT_MSG(!empty(), "Error: Cannot access back of an empty string storage");
        return mutableData()[size_ - 1];
    }

    // Exchanges the storage representation and content with another object.
    NEX_HIDDEN_FROM_ABI constexpr void swap(BasicStringStorage& other) noexcept {
        if (this == &other) return;
        BasicStringStorage temporary(NEX_MOVE(*this));
        *this = NEX_MOVE(other);
        other = NEX_MOVE(temporary);
    }
};

/**
 * @class BasicString
 * @brief A string class that owns a null-terminated character sequence.
 *
 * @details
 *
 * @tparam CharType The character type (e.g., `nchar, char8, char16, char32, wchar).
 * @tparam InlineCapacity The number of characters that can be stored inline without heap allocation.
 * @tparam Growth The growth policy used to determine how the string grows when more capacity is needed.
 */
template <
    Character CharType,
    usize InlineCapacity = BasicStringDefaultInlineCapacity,
    GrowthPolicy Growth = DoubleGrowth
> requires (InlineCapacity > 0)
class NEX_HIDDEN_FROM_ABI BasicString {
public:
    using value_type = CharType;
    using size_type = usize;
    using difference_type = isize;

    using char_type = CharType;
    using traits_type = CharTraits<char_type>;

    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    using self = BasicString<char_type, InlineCapacity, Growth>;
    using self_view = BasicStringView<char_type>;

    using iterator = pointer;
    using const_iterator = self_view::const_iterator;

    // The underlying storage type that manages the character sequence.
    using storage_type = BasicStringStorage<CharType, InlineCapacity, Growth>;

    // The number of content characters that can be stored inline without heap allocation.
    static constexpr size_type inlineCapacity = InlineCapacity;

    // Represents an invalid position in the string.
    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    storage_type storage_;

public:
    // Creates an empty string.
    NEX_HIDDEN_FROM_ABI constexpr BasicString() noexcept = default;

    // Creates a string by copying another BasicString object.
    NEX_HIDDEN_FROM_ABI constexpr BasicString(const BasicString& other) {
        storage_ = other.storage_;
    }

    // Copy assignment operator for BasicString.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& operator=(const BasicString& other) {
        if (this != &other) {
            storage_ = other.storage_;
        }
        return *this;
    }

    // Moves another BasicString object into this one.
    NEX_HIDDEN_FROM_ABI constexpr BasicString(BasicString&& other) noexcept {
        storage_ = NEX_MOVE(other.storage_);
    }

    // Move assignment operator for BasicString.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& operator=(BasicString&& other) noexcept {
        if (this != &other) {
            storage_ = NEX_MOVE(other.storage_);
        }
        return *this;
    }

    // Creates a string by copying a null-terminated character sequence.
    NEX_HIDDEN_FROM_ABI constexpr BasicString(const_pointer str) {
        NEX_ASSERT_MSG(str != nullptr, "Error: Cannot construct BasicString from a null pointer");
        storage_.assign(str, traits_type::length(str));
    }

    // Creates a string by copying a specified number of characters from a character sequence.
    NEX_HIDDEN_FROM_ABI constexpr BasicString(const_pointer str, size_type count) {
        NEX_ASSERT_MSG(str != nullptr, "Error: Cannot construct BasicString from a null pointer");
        storage_.assign(str, count);
    }

    // Creates a string by copying content from a BasicStringView.
    NEX_HIDDEN_FROM_ABI constexpr BasicString(self_view view) {
        storage_.assign(view.data(), view.size());
    }

    // Creates a string by copying content from an InitList of characters.
    NEX_HIDDEN_FROM_ABI constexpr BasicString(InitList<char_type> initList) {
        storage_.assign(initList.begin(), initList.size());
    }

    // Creates a string by copying count copies of a character.
    NEX_HIDDEN_FROM_ABI constexpr explicit BasicString(size_type count, char_type ch) {
        storage_.assign(count, ch);
    }

    // Releases resources and destroys the string.
    NEX_HIDDEN_FROM_ABI constexpr ~BasicString() noexcept = default;

    // Returns the number of content characters in the string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr size_type size() const noexcept { return storage_.size(); }

    // Returns the number of content characters in the string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr size_type length() const noexcept { return storage_.size(); }

    // Determines whether the string is empty (contains no content characters).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool empty() const noexcept { return storage_.empty(); }

    // Returns the maximum number of content characters that can be stored in the string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI static constexpr size_type maxSize() noexcept { return storage_type::maxSize(); }

    // Returns the current capacity of the string,
    // which is the number of content characters that can be stored without requiring a reallocation.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr size_type capacity() const noexcept { return storage_.capacity(); }

    // Reserves capacity for at least newCapacity content characters.
    NEX_HIDDEN_FROM_ABI constexpr void reserve(size_type newCapacity) { storage_.reserve(newCapacity); }

    // Reduces capacity to fit the current size, potentially releasing heap storage.
    NEX_HIDDEN_FROM_ABI constexpr void shrinkToFit() { storage_.shrinkToFit(); }

    // Resizes the string to contain newSize content characters, value-initializing added characters.
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type newSize) { storage_.resize(newSize); }

    // Resizes the string to contain newSize content characters, filling added characters with value.
    NEX_HIDDEN_FROM_ABI constexpr void resize(size_type newSize, char_type ch) { storage_.resize(newSize, ch); }

    // Returns a writable pointer to the string's content, including a NUL terminator.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr pointer data() noexcept { return storage_.data(); }

    // Returns a read-only pointer to the string's content, including a NUL terminator.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_pointer data() const noexcept { return storage_.data(); }

    // Returns a read-only pointer to the string's content, including a NUL terminator.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_pointer cStr() const noexcept { return storage_.cStr(); }

    // Returns a mutable iterator to the first character in the string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr iterator begin() noexcept { return storage_.data(); }

    // Returns an immutable const iterator to the first character in the string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_iterator begin() const noexcept { return storage_.data(); }

    // Returns a mutable iterator to one past the last character in the string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr iterator end() noexcept {
        return storage_.data() + storage_.size();
    }

    // Returns an immutable const iterator to one past the last character in the string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_iterator end() const noexcept {
        return storage_.data() + storage_.size();
    }

    // Returns an immutable const iterator to the first character in the string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_iterator cbegin() const noexcept { return storage_.data(); }

    // Returns an immutable const iterator to one past the last character in the string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_iterator cend() const noexcept {
        return storage_.data() + storage_.size();
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr reference at(size_type position) noexcept {
        return storage_.at(position);
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference at(size_type position) const noexcept {
        return storage_.at(position);
    }

    // Returns the character at position without bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr reference operator[](size_type position) noexcept {
        return storage_.data()[position];
    }

    // Returns the character at position without bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference operator[](size_type position) const noexcept {
        return storage_.data()[position];
    }

    // Returns the first content character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr reference front() noexcept {
        return storage_.front();
    }

    // Returns the first content character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference front() const noexcept {
        return storage_.front();
    }

    // Returns the last content character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr reference back() noexcept {
        return storage_.back();
    }

    // Returns the last content character.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference back() const noexcept {
        return storage_.back();
    }

    // Returns a non-owning view of the string's content as a BasicStringView.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr self_view asView() const noexcept {
        return self_view(storage_.data(), storage_.size());
    }

    // Assign a null-terminated character sequence to the string, replacing its content.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& operator=(const_pointer str) {
        NEX_ASSERT_MSG(str != nullptr, "Error: Cannot assign BasicString from a null pointer");
        storage_.assign(str, traits_type::length(str));
        return *this;
    }

    // Assign a specified number of characters from a character sequence to the string, replacing its content.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& operator=(char_type ch) {
        storage_.assign(1, ch);
        return *this;
    }

    // Assigns content from a BasicStringView to the string, replacing its content.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& operator=(self_view view) {
        storage_.assign(view.data(), view.size());
        return *this;
    }

    // Assigns content from an InitList of characters to the string, replacing its content.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& operator=(InitList<char_type> initList) {
        storage_.assign(initList.begin(), initList.size());
        return *this;
    }

    // Assigns the content of another BasicString object to this string, replacing its content.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& assign(const self& other) {
        return *this = other;
    }

    // Moves the content of another BasicString object to this string, replacing its content.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& assign(self&& other) noexcept {
        *this = NEX_MOVE(other);
        return *this;
    }

    // Assigns a substring of another BasicString object to this string, replacing its content.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& assign(
        const self& other, size_type position, size_type count = npos
    ) {
        NEX_ASSERT_MSG(position <= other.size(), "Error: Assign position is out of bounds");
        const size_type removable = other.size() - position;
        const size_type assignedCount = count < removable ? count : removable;
        storage_.assign(other.data() + position, assignedCount);
        return *this;
    }

    // Replaces the string's content with exactly count characters from source.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& assign(const_pointer source, size_type count) {
        NEX_ASSERT_MSG(source != nullptr, "Error: Cannot assign BasicString from a null pointer");
        storage_.assign(source, count);
        return *this;
    }

    // Replaces the string's content with a null-terminated character sequence.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& assign(const_pointer source) {
        NEX_ASSERT_MSG(source != nullptr, "Error: Cannot assign BasicString from a null pointer");
        storage_.assign(source, traits_type::length(source));
        return *this;
    }

    // Replaces the string's content with a BasicStringView, copying its content.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& assign(self_view view) {
        storage_.assign(view.data(), view.size());
        return *this;
    }

    // Replaces the string's content with count copies of character.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& assign(size_type count, char_type ch) {
        storage_.assign(count, ch);
        return *this;
    }

    // Assigns content from an InitList of characters to the string, replacing its content.
    NEX_HIDDEN_FROM_ABI constexpr BasicString& assign(InitList<char_type> initList) {
        storage_.assign(initList.begin(), initList.size());
        return *this;
    }

    // Clears the string's content while retaining the current allocation and terminator slot.
    NEX_HIDDEN_FROM_ABI constexpr void clear() noexcept { storage_.clear(); }

    // Resets the string's content and deallocates any allocated memory.
    NEX_HIDDEN_FROM_ABI constexpr void reset() noexcept { storage_.reset(); }

    // Appends content from a BasicStringView to the end of the string.
    NEX_HIDDEN_FROM_ABI constexpr void append(self_view view) {
        storage_.append(view.data(), view.size());
    }

    // Appends exactly count characters from source to the end of the string.
    NEX_HIDDEN_FROM_ABI constexpr void append(const_pointer source, size_type count) {
        NEX_ASSERT_MSG(source != nullptr, "Error: String source must not be null");
        storage_.append(source, count);
    }

    // Appends a null-terminated character sequence to the end of the string.
    NEX_HIDDEN_FROM_ABI constexpr void append(const_pointer source) {
        NEX_ASSERT_MSG(source != nullptr, "Error: String source must not be null");
        storage_.append(source, traits_type::length(source));
    }

    // Appends count copies of character to the end of the string.
    NEX_HIDDEN_FROM_ABI constexpr void append(size_type count, char_type ch) {
        storage_.append(count, ch);
    }

    // Appends a single character to the end of the string.
    NEX_HIDDEN_FROM_ABI constexpr void append(char_type ch) {
        storage_.append(ch);
    }

    // Appends a single character to the end of the string.
    NEX_HIDDEN_FROM_ABI constexpr void pushBack(char_type ch) {
        storage_.pushBack(ch);
    }

    // Removes the last character from the string.
    NEX_HIDDEN_FROM_ABI constexpr void popBack() noexcept {
        storage_.popBack();
    }

    // Inserts count characters from source before the specified position in the string.
    NEX_HIDDEN_FROM_ABI constexpr void insert(size_type position, const_pointer source, size_type count) {
        NEX_ASSERT_MSG(source != nullptr, "Error: String source must not be null");
        storage_.insert(position, source, count);
    }

    // Inserts content from a BasicStringView at the specified position in the string.
    NEX_HIDDEN_FROM_ABI constexpr void insert(size_type position, self_view view) {
        storage_.insert(position, view.data(), view.size());
    }

    // Replaces up to removedCount characters at position with count characters from source.
    NEX_HIDDEN_FROM_ABI constexpr void replace(
        size_type position,
        size_type removedCount,
        const_pointer source,
        size_type count
    ) {
        NEX_ASSERT_MSG(source != nullptr, "Error: String source must not be null");
        storage_.replace(position, removedCount, source, count);
    }

    // Replaces up to removedCount characters at position with content from a BasicStringView.
    NEX_HIDDEN_FROM_ABI constexpr void replace(
        size_type position,
        size_type removedCount,
        self_view view
    ) {
        storage_.replace(position, removedCount, view.data(), view.size());
    }

    // Removes up to count characters starting at position.
    NEX_HIDDEN_FROM_ABI constexpr void erase(size_type position, size_type count = 1) noexcept {
        storage_.erase(position, count);
    }

    // Returns a substring of the string starting at position and containing up to count characters.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr self substr(size_type position, size_type count = npos) const {
        NEX_ASSERT_MSG(position <= size(), "Error: Substring position is out of bounds");
        const size_type removable = size() - position;
        const size_type substringCount = count < removable ? count : removable;
        return self(data() + position, substringCount);
    }

    // Concatenates two BasicString objects and returns the result.
    NEX_HIDDEN_FROM_ABI constexpr self operator+(const self& other) const {
        self result(*this);
        result.append(other.data(), other.size());
        return result;
    }

    // Appends another BasicString object to this string and returns the result.
    NEX_HIDDEN_FROM_ABI constexpr self& operator+=(const self& other) {
        append(other.data(), other.size());
        return *this;
    }

    // Concatenates a BasicString object with a BasicStringView and returns the result.
    NEX_HIDDEN_FROM_ABI constexpr self operator+(self_view view) const {
        self result(*this);
        result.append(view.data(), view.size());
        return result;
    }

    // Appends a BasicStringView to this string and returns the result.
    NEX_HIDDEN_FROM_ABI constexpr self& operator+=(self_view view) {
        append(view.data(), view.size());
        return *this;
    }

    // Concatenates a BasicString object with a null-terminated character sequence and returns the result.
    NEX_HIDDEN_FROM_ABI constexpr self operator+(const_pointer str) const {
        NEX_ASSERT_MSG(str != nullptr, "Error: String source must not be null");
        self result(*this);
        result.append(str, traits_type::length(str));
        return result;
    }

    // Appends a null-terminated character sequence to this string and returns the result.
    NEX_HIDDEN_FROM_ABI constexpr self& operator+=(const_pointer str) {
        NEX_ASSERT_MSG(str != nullptr, "Error: String source must not be null");
        append(str, traits_type::length(str));
        return *this;
    }

    // Concatenates a BasicString object with a single character and returns the result.
    NEX_HIDDEN_FROM_ABI constexpr self operator+(char_type ch) const {
        self result(*this);
        result.append(ch);
        return result;
    }

    // Appends a single character to this string and returns the result.
    NEX_HIDDEN_FROM_ABI constexpr self& operator+=(char_type ch) {
        append(ch);
        return *this;
    }

    // Exchanges the content and storage representation with another BasicString object.
    NEX_HIDDEN_FROM_ABI constexpr void swap(self& other) noexcept {
        storage_.swap(other.storage_);
    }

    // Compares this string with another BasicString object lexicographically.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr int32 compare(const self& other) const noexcept {
        const size_type commonSize = meta::_minOf(size(), other.size());
        const int32 result = traits_type::compare(data(), other.data(), commonSize);
        if (result != 0) {
            return result;
        }
        if (size() < other.size()) {
            return -1;
        }
        if (size() > other.size()) {
            return 1;
        }
        return 0;
    }

    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr int32 compare(self_view view) const noexcept {
        const size_type commonSize = meta::_minOf(size(), view.size());
        const int32 result = traits_type::compare(data(), view.data(), commonSize);
        if (result != 0) {
            return result;
        }
        if (size() < view.size()) {
            return -1;
        }
        if (size() > view.size()) {
            return 1;
        }
        return 0;
    }

    // Compares this string with another BasicString object for equality.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator==(const self& other) const noexcept {
        return size() == other.size() && compare(other) == 0;
    }

    // Compares this string with another BasicStringView object for equality.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator==(self_view view) const noexcept {
        return size() == view.size() && compare(view) == 0;
    }

    // Compares this string with another BasicString object for inequality.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator!=(const self& other) const noexcept {
        return !(*this == other);
    }

    // Compares this string with another BasicStringView object for inequality.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator!=(self_view view) const noexcept {
        return !(*this == view);
    }

    // Compares this string with another BasicString object for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator<(const self& other) const noexcept {
        return compare(other) < 0;
    }

    // Compares this string with another BasicStringView object for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator<(self_view view) const noexcept {
        return compare(view) < 0;
    }

    // Compares this string with another BasicString object for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator<=(const self& other) const noexcept {
        return compare(other) <= 0;
    }

    // Compares this string with another BasicStringView object for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator<=(self_view view) const noexcept {
        return compare(view) <= 0;
    }

    // Compares this string with another BasicString object for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator>(const self& other) const noexcept {
        return compare(other) > 0;
    }

    // Compares this string with another BasicStringView object for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator>(self_view view) const noexcept {
        return compare(view) > 0;
    }

    // Compares this string with another BasicString object for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator>=(const self& other) const noexcept {
        return compare(other) >= 0;
    }

    // Compares this string with another BasicStringView object for lexicographical ordering.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator>=(self_view view) const noexcept {
        return compare(view) >= 0;
    }
};

// =================================================================================
// Defines common type aliases for BasicString with specific character types.
// =================================================================================

/**
 * @class NString
 * @brief A string class that owns a null-terminated sequence of native narrow characters (`nchar`).
 */
using NString = BasicString<nchar>;

/**
 * @class WString
 * @brief A string class that owns a null-terminated sequence of wide characters (`wchar`).
 */
using WString = BasicString<wchar>;

/**
 * @class AnsiString
 * @brief A string class that owns a null-terminated sequence of ANSI characters (`char`).
 */
using AnsiString = BasicString<char>;

#if NEX_HAS_BUILTIN_CHAR8_T
    /**
     * @class Utf8String
     * @brief A string class that owns a null-terminated sequence of UTF-8 characters (`char8`).
     */
    using Utf8String = BasicString<char8>;
#endif  // ^^NEX_HAS_BUILTIN_CHAR8_T

/**
 * @class Utf16String
 * @brief A string class that owns a null-terminated sequence of UTF-16 characters (`char16`).
 */
using Utf16String = BasicString<char16>;

/**
 * @class Utf32String
 * @brief A string class that owns a null-terminated sequence of UTF-32 characters (`char32`).
 */
using Utf32String = BasicString<char32>;

#if NEX_PLATFORM_IS_WINDOWS
    /**
     * @typedef LocalString
     * @brief   A string class that owns a null-terminated sequence of local characters.
     * @details
     * The LocalString type is defined as BasicString<wchar> on Windows platforms,
     * as Windows uses wide characters with UTF-16 encoding for local strings.
     */
    using LocalString = BasicString<wchar>;
#elif NEX_PLATFORM_FAMILY_IS_POSIX
    /**
     * @typedef LocalString
     * @brief   A string class that owns a null-terminated sequence of local characters.
     * @details
     * The LocalString type is defined as BasicString<char8> on POSIX platforms (macOS/Linux),
     * as POSIX uses narrow characters with UTF-8 encoding for local strings.
     */
    using LocalString = BasicString<char8>;
#endif

NEX_NAMESPACE_END
