/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/allocator.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Allocates a raw block of memory with the specified byte count and alignment.
 * @param byteCount The number of bytes to allocate.
 * @param alignment The alignment requirement for the allocation.
 * @return A void pointer (void*) to the allocated memory.
 * @note The returned pointer must be casted to the appropriate type before use.
 */
void_ptr allocateRaw(
    usize byteCount,
    usize alignment
) {
#if NEX_ALLOCATION_DIAGNOSTIC_ENABLED
    // Allocation diagnostic logic can be added here
#endif  // ^^NEX_ALLOCATION_DIAGNOSTIC_ENABLED

#if NEX_HAS_ALIGNED_ALLOCATION
    if (isOverAlignedForNew(alignment)) {
        // Overload resolution for the standard global operator new requires align_val_t here.
        // There's no other way for this even if we define an identical custom align_val type ourselves.
        return ::operator new(byteCount, static_cast<NEX_STD align_val_t>(alignment));
    }
#endif  // ^^NEX_HAS_ALIGNED_ALLOCATION

    return ::operator new(byteCount);
}

/**
 * @brief Deallocates a raw block of memory previously allocated.
 * @param ptr The pointer to the memory to deallocate.
 * @param byteCount The number of bytes to deallocate.
 * @param alignment The alignment requirement for the allocation.
 * 
 * @note
 * For unsized deallocation, the byte count is unknown, so 0 is passed by convention.
 * If user passes a zero byte count, it will be treated as an unsized deallocation,
 * not a sized deallocation with the size zero.
 */
void deallocateRaw(
    void_ptr ptr,
    usize byteCount,
    usize alignment
) noexcept {
    /**
     * @note
     * For unsized deallocation, the byte count is unknown, so we pass 0 by convention for now,
     * but it may be updated in the future if a better convention is established.
     */
    bool isUnsized = (byteCount == 0);

#if NEX_ALLOCATION_DIAGNOSTIC_ENABLED
    // Allocation diagnostic logic can be added here
#endif  // ^^NEX_ALLOCATION_DIAGNOSTIC_ENABLED

#if NEX_HAS_ALIGNED_ALLOCATION
    if (isOverAlignedForNew(alignment)) {
        if (isUnsized) {
            // Overload resolution for the standard global operator delete requires align_val_t here.
            // There's no other way for this even if we define an identical custom align_val type ourselves.
            return ::operator delete(ptr, static_cast<NEX_STD align_val_t>(alignment));
        }

        return ::operator delete(
            ptr
            NEX_ONLY_IF_SIZED_DEALLOCATION(byteCount)
            // Standard align_val_t type is required here
            , static_cast<NEX_STD align_val_t>(alignment)
        );
    }
#endif  // ^^NEX_HAS_ALIGNED_ALLOCATION

    if (isUnsized) {
        return ::operator delete(ptr);
    } else {
        return ::operator delete(ptr NEX_ONLY_IF_SIZED_DEALLOCATION(byteCount));
    }
}

NEX_NAMESPACE_END
