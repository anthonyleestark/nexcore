/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

// Include the standard C library header for memory manipulation functions
#include <string.h> 

#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"

NEX_NAMESPACE_BEGIN

// Assert non-null pointer for constructAt and destroyAt functions
#define NEX_MEMORY_ASSERT_NON_NULL(condition, message) (void(0))

/**
 * @brief Constructs an object of type `Type` at the specified memory location `location` using the provided arguments.
 * @tparam Type The type of the object to be constructed.
 * @tparam Args The types of the arguments to be forwarded to the constructor of `Type`.
 * @param location A pointer to the memory location where the object will be constructed.
 * @param args The arguments to be forwarded to the constructor of `Type`.
 * @return A pointer to the newly constructed object of type `Type`.
 * @note This function uses placement new to construct the object in the specified memory location.
 */
template <class Type, class... Args, class = decltype(::new(meta::declval<void*>()) Type(meta::declval<Args>()...))>
NEX_HIDDEN_FROM_ABI constexpr Type* constructAt(Type* location, Args&&... args) {
    NEX_MEMORY_ASSERT_NON_NULL(location != nullptr, "Error: null pointer given to constructAt");
    return ::new (static_cast<void*>(location)) Type(NEX_FORWARD<Args>(args)...);
}

/**
 * @note
 * The internal functions are available regardless of the language version
 * (with the exception of the `_destroyAt_Internal` taking an array).
 */

// Internal destroyAt function for non-array types.
template <class Type, meta::EnableIfT<!meta::IsArrayV<Type>, int32> = 0>
NEX_HIDDEN_FROM_ABI constexpr void _destroyAt_Internal(Type* location) {
    NEX_MEMORY_ASSERT_NON_NULL(location != nullptr, "Error: null pointer given to destroyAt");
    location->~Type();
}

// Internal destroyAt function for array types.
template <class Type, meta::EnableIfT<meta::IsArrayV<Type>, int32> = 0>
NEX_HIDDEN_FROM_ABI constexpr void _destroyAt_Internal(Type* location) {
    NEX_MEMORY_ASSERT_NON_NULL(location != nullptr, "Error: null pointer given to destroyAt");
    for (auto&& val : *location) {
        _destroyAt_Internal(NEX_ADDRESS_OF(val));
    }
}

/**
 * @brief Destroys the object of type `Type` at the specified memory location `location`.
 * @tparam Type The type of the object to be destroyed.
 * @param location A pointer to the memory location where the object will be destroyed.
 * @note This function calls the destructor of the object at the specified memory location.
 */
template <class Type, meta::EnableIfT<!meta::IsArrayV<Type>, int32> = 0>
NEX_HIDDEN_FROM_ABI constexpr void destroyAt(Type* location) {
    _destroyAt_Internal(location);
}

/**
 * @brief Destroys the array of objects of type `Type` at the specified memory location `location`.
 * @tparam Type The type of the objects in the array to be destroyed.
 * @param location A pointer to the memory location where the array of objects will be destroyed.
 * @note This function calls the destructor of each object in the array at the specified memory location.
 */
template <class Type, meta::EnableIfT<meta::IsArrayV<Type>, int32> = 0>
NEX_HIDDEN_FROM_ABI constexpr void destroyAt(Type* location) {
    _destroyAt_Internal(location);
}

// Undefine macros for low-level memory manipulation functions
// to avoid conflicts with standard library functions

#undef memcpy
#undef memmove
#undef memset
#undef memchr
#undef memcmp

/**
 * @brief Copies `count` bytes from the memory area `src` to the memory area `dest`.
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param count The number of bytes to copy.
 * @return A pointer to the destination memory area `dest`.
 * @note The memory areas must not overlap. Use `memmove` if the memory areas do overlap.
 */
using ::memcpy;

/**
 * @brief Moves `count` bytes from the memory area `src` to the memory area `dest`.
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param count The number of bytes to move.
 * @return A pointer to the destination memory area `dest`.
 * @note The memory areas may overlap. Use `memcpy` if the memory areas do not overlap.
 */
using ::memmove;

/**
 * @brief Sets the first `count` bytes of the memory area pointed to by `dest` to the specified value `value`.
 * @param dest A pointer to the memory area to be set.
 * @param value The value to be set (interpreted as an unsigned char).
 * @param count The number of bytes to set.
 * @return A pointer to the memory area `dest`.
 */
using ::memset;

/**
 * @brief Searches for the first occurrence of the character `c` (interpreted as an unsigned char)
 *        in the first `count` bytes of the memory area pointed to by `ptr`.
 * @param ptr A pointer to the memory area to be searched.
 * @param c The character to be searched for (interpreted as an unsigned char).
 * @param count The number of bytes to be searched.
 * @return A pointer to the first occurrence of the character `c` in the memory area, or `nullptr`
 *         if the character is not found.
 */
using ::memchr;

/**
 * @brief Compares the first `count` bytes of the memory areas `ptr1` and `ptr2`.
 * @param ptr1 A pointer to the first memory area to be compared.
 * @param ptr2 A pointer to the second memory area to be compared.
 * @param count The number of bytes to be compared.
 * @return An integer less than, equal to, or greater than zero if the first `count` bytes of `ptr1`
 *         are found, respectively, to be less than, to match, or be greater than the first `count`
 *         bytes of `ptr2`.
 */
using ::memcmp;

/**
 * @brief Returns the offset of a member within a struct/class.
 * @tparam Type The type of the struct/class.
 * @tparam MemberT The type of the member.
 * @param member A pointer to the member within the struct/class.
 * @return The offset of the member within the struct/class, represented as a signed integer type.
 * @note This function is only safe for standard-layout types. It uses reinterpret_cast to calculate
 *       the offset of the member from the beginning of the struct/class.
 */
template <typename Type, typename MemberT>
NEX_HIDDEN_FROM_ABI constexpr isize offsetOf(MemberT Type::*member) noexcept {
    static_assert(meta::IsStandardLayoutV<Type>, "Error: 'offsetOf' only safe for standard-layout types");
    return reinterpret_cast<isize>(&reinterpret_cast<Type*>(0)->*member);
}

// ======================================================================================
// Define macros for constructing and destroying objects in a specified memory location
// ======================================================================================

// Constructs an object of type `Type` at the specified memory location `ptr` using the provided arguments.
#define NEX_CONSTRUCT_AT(ptr, ...) \
    NEX_PREPEND_NAMESPACE(constructAt(ptr, __VA_ARGS__))

// Destroys the object of type `Type` at the specified memory location `ptr`.
#define NEX_DESTROY_AT(ptr) \
    NEX_PREPEND_NAMESPACE(destroyAt(ptr))

// ======================================================================================
// Define macros for low-level memory manipulation functions
// ======================================================================================

// Copies `count` bytes from the memory area `src` to the memory area `dest`.
#define NEX_MEMCPY(dest, src, count) \
    NEX_PREPEND_NAMESPACE(memcpy(dest, src, count))

// Moves `count` bytes from the memory area `src` to the memory area `dest`.
// The memory areas may overlap.
#define NEX_MEMMOVE(dest, src, count) \
    NEX_PREPEND_NAMESPACE(memmove(dest, src, count))

// Sets the first `count` bytes of the memory area pointed to by `dest` to the specified value `value`.
#define NEX_MEMSET(dest, value, count) \
    NEX_PREPEND_NAMESPACE(memset(dest, value, count))

// Searches for the first occurrence of the character `c` (interpreted as an unsigned char)
// in the first `count` bytes of the memory area pointed to by `ptr`.
#define NEX_MEMCHR(ptr, c, count) \
    NEX_PREPEND_NAMESPACE(memchr(ptr, c, count))

// Compares the first `count` bytes of the memory areas `ptr1` and `ptr2`.
#define NEX_MEMCMP(ptr1, ptr2, count) \
    NEX_PREPEND_NAMESPACE(memcmp(ptr1, ptr2, count))

// ======================================================================================
// Define macros for getting the offset of a member within a struct/class
// ======================================================================================

#define NEX_OFFSET_OF(type, member) \
    NEX_PREPEND_NAMESPACE(offsetOf<type>(&type::member))

NEX_NAMESPACE_END
