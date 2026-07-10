/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/range.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

// Forward declaration
class ConstMemoryBlock;

/**
 * @class  MemoryBlock
 * @brief  Non-owning mutable memory block described by pointer and byte size.
 * 
 * @details
 * The MemoryBlock class represents a non-owning view of a contiguous block of memory.
 * It is defined by a pointer to the start of the memory and its size in bytes.
 * This class is useful for managing and manipulating raw memory without taking ownership of it.
 * It provides methods to access the memory, check its size, and create sub-blocks.
 * 
 * @note
 * MemoryBlock does not manage the lifetime of the memory it points to. 
 * The caller is responsible for ensuring that the memory remains valid for the duration 
 * of the MemoryBlock's use.
 */
class NEX_API MemoryBlock {
public:
    using value_type = void;
    using size_type = usize;
    using pointer_type = void_ptr;
    using difference_type = isize;

private:
    pointer_type data_;     // Pointer to the start of the memory block
    size_type size_;        // Size of the memory block in bytes

    // Allow ConstMemoryBlock to access private members of MemoryBlock
    friend class ConstMemoryBlock;

public:
    // Default constructor
    constexpr MemoryBlock() noexcept
        : data_(nullptr), size_(0) {}

    // Constructs a MemoryBlock with a data pointer and size
    constexpr MemoryBlock(pointer_type data, size_type size) noexcept
        : data_(data), size_(size) {}

    // Determine if the memory block is empty (size is zero)
    NEX_NODISCARD constexpr bool empty() const noexcept {
        return size_ == 0;
    }

    // Determine if the memory block is valid (data is not null and size is greater than zero)
    NEX_NODISCARD constexpr bool valid() const noexcept {
        return data_ != nullptr && size_ > 0;
    }

    // Get the size of the memory block, which is the number of bytes it contains
    NEX_NODISCARD constexpr size_type size() const noexcept {
        return size_;
    }

    // Get a raw pointer to the start of the memory block
    NEX_NODISCARD constexpr pointer_type data() const noexcept {
        return data_;
    }

    // Get a byte pointer to the start of the memory block
    NEX_NODISCARD constexpr byte_ptr bytes() const noexcept {
        return static_cast<byte_ptr>(data_);
    }

    // Get a byte pointer to the end of the memory block
    NEX_NODISCARD constexpr byte_ptr end() const noexcept {
        return bytes() + size_;
    }

    // Get a sub-block of the memory block defined by a ByteRange
    NEX_NODISCARD constexpr MemoryBlock subBlock(ByteRange range) const noexcept {
        if (empty() || range.empty() || range.start() >= size_) {
            return {};
        }

        const size_type actualLength = 
            (range.start() + range.length() > size_) 
                ? (size_ - range.start()) 
                : range.length();

        return { bytes() + range.start(), actualLength };
    }

    // Casts the memory block to a typed pointer of the specified TargetType
    template <typename TargetType>
    constexpr TargetType* as() noexcept {
        NEX_ASSERT_MSG(size() >= sizeof(TargetType), "Error: MemoryBlock size is smaller than the size of TargetType");
        NEX_ASSERT(reinterpret_cast<uintptr>(data()) % alignof(TargetType) == 0);
        return NEX_MEMORY_CAST<TargetType>(data_);
    }

    // Casts the memory block to a typed const pointer of the specified TargetType
    template <typename TargetType>
    constexpr const TargetType* as() const noexcept {
        NEX_ASSERT_MSG(size() >= sizeof(TargetType), "Error: MemoryBlock size is smaller than the size of TargetType");
        NEX_ASSERT(reinterpret_cast<uintptr>(data()) % alignof(TargetType) == 0);
        return NEX_MEMORY_CAST<TargetType>(data_);
    }

    // Equality operator for comparing two MemoryBlock instances
    constexpr bool operator==(const MemoryBlock&) const noexcept = default;
};

/**
 * @class  ConstMemoryBlock
 * @brief  Non-owning immutable memory block described by pointer and byte size.
 * 
 * @details
 * The ConstMemoryBlock class represents a non-owning view of a contiguous block of memory.
 * It is defined by a pointer to the start of the memory and its size in bytes.
 * This class is useful for managing and manipulating raw memory without taking ownership of it.
 * It provides methods to access the memory, check its size, and create sub-blocks.
 * 
 * @note
 * ConstMemoryBlock does not manage the lifetime of the memory it points to. 
 * The caller is responsible for ensuring that the memory remains valid for the duration 
 * of the ConstMemoryBlock's use.
 */
class NEX_API ConstMemoryBlock {
    using value_type = const void;
    using size_type = usize;
    using pointer_type = const_void_ptr;
    using difference_type = isize;

private:
    pointer_type data_;     // Pointer to the start of the memory block
    size_type size_;        // Size of the memory block in bytes

    // Allow MemoryBlock to access private members of ConstMemoryBlock
    friend class MemoryBlock;

public:
    // Default constructor
    constexpr ConstMemoryBlock() noexcept
        : data_(nullptr), size_(0) {}

    // Constructs a ConstMemoryBlock with a data pointer and size
    constexpr ConstMemoryBlock(pointer_type data, size_type size) noexcept
        : data_(data), size_(size) {}

    // Implicitly convert a mutable MemoryBlock to an immutable ConstMemoryBlock
    constexpr ConstMemoryBlock(MemoryBlock block) noexcept
        : data_(block.data_), size_(block.size_) {}

    // Determine if the memory block is empty (size is zero)
    NEX_NODISCARD constexpr bool empty() const noexcept {
        return size_ == 0;
    }

    // Determine if the memory block is valid (data is not null and size is greater than zero)
    NEX_NODISCARD constexpr bool valid() const noexcept {
        return data_ != nullptr && size_ > 0;
    }

    // Get the size of the memory block, which is the number of bytes it contains
    NEX_NODISCARD constexpr size_type size() const noexcept {
        return size_;
    }

    // Get a raw pointer to the start of the memory block
    NEX_NODISCARD constexpr pointer_type data() const noexcept {
        return data_;
    }

    // Get a byte pointer to the start of the memory block
    NEX_NODISCARD constexpr const_byte_ptr bytes() const noexcept {
        return static_cast<const_byte_ptr>(data_);
    }

    // Get a byte pointer to the end of the memory block
    NEX_NODISCARD constexpr const_byte_ptr end() const noexcept {
        return bytes() + size_;
    }

    // Get a sub-block of the memory block defined by a ByteRange
    NEX_NODISCARD constexpr ConstMemoryBlock subBlock(ByteRange range) const noexcept {
        if (empty() || range.empty() || range.start() >= size_) {
            return {};
        }

        const size_type actualLength = 
            (range.start() + range.length() > size_) 
                ? (size_ - range.start()) 
                : range.length();

        return { bytes() + range.start(), actualLength };
    }

    // Casts the const memory block to a typed const pointer of the specified TargetType
    template <typename TargetType>
    constexpr const TargetType* as() const noexcept {
        NEX_ASSERT_MSG(size() >= sizeof(TargetType), "Error: MemoryBlock size is smaller than the size of TargetType");
        NEX_ASSERT(reinterpret_cast<uintptr>(data()) % alignof(TargetType) == 0);
        return NEX_MEMORY_CAST<TargetType>(data_);
    }

    // Equality operator for comparing two ConstMemoryBlock instances
    constexpr bool operator==(const ConstMemoryBlock&) const noexcept = default;
};

NEX_NAMESPACE_END
