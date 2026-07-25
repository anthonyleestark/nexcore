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
#include "nex/base/memory.h"
#include "nex/base/algorithm.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

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
    usize InlineCapacity = 23,
    GrowthPolicy Growth = DoubleGrowth
> requires (InlineCapacity > 0)
class NEX_HIDDEN_FROM_ABI BasicStringStorage {
public:
    using value_type = Type;
    using size_type = usize;
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
    NEX_HIDDEN_FROM_ABI constexpr
    void erase(size_type position, size_type count = 1) noexcept {
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
        NEX_ASSERT_MSG(position < size_, "Error: String index is out of bounds");
        return mutableData()[position];
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    const_reference at(size_type position) const noexcept {
        NEX_ASSERT_MSG(position < size_, "Error: String index is out of bounds");
        return mutableData()[position];
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    reference operator[](size_type position) noexcept {
        return at(position);
    }

    // Returns the character at position with bounds checking.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    const_reference operator[](size_type position) const noexcept {
        return at(position);
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

NEX_NAMESPACE_END
