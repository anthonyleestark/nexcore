/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

// Include the standard C library header for memory manipulation functions
#include <string.h> 

#include "nex/base/namespace.h"
#include "nex/base/compiler.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/endian.h"

#if NEX_COMPILER_IS_MSVC
    // Forward declare _mm_prefetch to avoid including <intrin.h>
    NEX_EXTERN_C void _mm_prefetch(const char* p, int i);

    // Equivalent to _MM_HINT_T0 from <xmmintrin.h>,
    // avoided to prevent pulling in the intrinsics header
    #define NEX_MM_HINT_T0 3
#endif

// Assert non-null pointer for constructAt and destroyAt functions
#define NEX_MEMORY_ASSERT_NON_NULL(condition, message) (void(0))

// Assert valid input range for isPointerInRange function
#define NEX_MEMORY_ASSERT_VALID_INPUT_RANGE(condition, message) (void(0))

NEX_NAMESPACE_BEGIN

/**
 * @brief Obtains the actual address of an object, safely bypassing any overloaded operator&.
 * @note Utilizes a compiler built-in to guarantee the extraction of the real memory address, 
 *       even if the type has a custom or malicious address-of operator.
 * @tparam Type The type of the object whose address is being taken.
 * @param Value The lvalue reference to the object.
 * @return A pointer to the object (Type*).
 */
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr 
auto addressOf(Type& Value) noexcept -> decltype(meta::_addressOf(Value)) {
    return meta::_addressOf(Value);
}

/**
 * @brief Deleted overload to explicitly prevent taking the address of rvalue temporaries.
 * @note This overload triggers a compile-time error if a temporary object is passed, 
 *       protecting against dangerous dangling pointers to short-lived resources.
 * @tparam Type The type of the rvalue temporary.
 */
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr 
const Type* addressOf(const Type&&) = delete;

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
        _destroyAt_Internal(addressOf(val));
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

/**
 * @brief Assigns the value of `src` to `dest` for trivially copyable types.
 * @tparam To The type of the destination object.
 * @tparam From The type of the source object.
 * @param dest A reference to the destination object.
 * @param src A reference to the source object.
 * @return A reference to the destination object after assignment.
 * @note This function is only enabled for types that are trivially copyable and assignable.
 */
template <class To, class From>
    requires (meta::IsAssignableV<To&, From const&>)
NEX_HIDDEN_FROM_ABI constexpr To& assignTriviallyCopyable(To& dest, From const& src) {
    dest = src;
    return dest;
}

/**
 * @brief Assigns the value of `src` to `dest` for trivially copyable types
 *        that are not assignable but are constructible.
 * @tparam To The type of the destination object.
 * @tparam From The type of the source object.
 * @param dest A reference to the destination object.
 * @param src A reference to the source object.
 * @return A reference to the destination object after assignment.
 * @note This function is only enabled for types that are trivially copyable and constructible from `From`.
 */
template <class To, class From>
    requires (!meta::IsAssignableV<To&, From const&> && meta::IsAssignableV<To&, From&&>)
NEX_HIDDEN_FROM_ABI constexpr To& assignTriviallyCopyable(To& dest, From& src) {
    // this is safe, we're not actually moving anything since the assignment is trivial
    dest = static_cast<From&&>(src);
    return dest;
}

/**
 * @brief Assigns the value of `src` to `dest` for trivially copyable types
 *        that are not assignable but are constructible from `From const&`.
 * @tparam To The type of the destination object.
 * @tparam From The type of the source object.
 * @param dest A reference to the destination object.
 * @param src A reference to the source object.
 * @return A reference to the destination object after assignment.
 * @note This function is only enabled for types that are trivially copyable and constructible from `From const&`.
 */
template <class To, class From>
    requires (!meta::IsAssignableV<To&, From const&> &&
              !meta::IsAssignableV<To&, From&&> &&
              meta::IsConstructibleV<To, From const&>)
NEX_HIDDEN_FROM_ABI constexpr To& assignTriviallyCopyable(To& dest, From const& src) {
    // To is trivially destructible, so we don't need to call its destructor
    // to end the lifetime of the object that was there previously
    constructAt(addressOf(dest), src);
    return dest;
}

/**
 * @brief Assigns the value of `src` to `dest` for trivially copyable types
 *        that are not assignable but are constructible from `From&&`.
 * @tparam To The type of the destination object.
 * @tparam From The type of the source object.
 * @param dest A reference to the destination object.
 * @param src A reference to the source object.
 * @return A reference to the destination object after assignment.
 * @note This function is only enabled for types that are trivially copyable and constructible from `From&&`.
 */
template <class To, class From>
    requires (!meta::IsAssignableV<To&, From const&> && !meta::IsAssignableV<To&, From&&> &&
              !meta::IsConstructibleV<To, From const&> && meta::IsConstructibleV<To, From&&>)
NEX_HIDDEN_FROM_ABI constexpr To& assignTriviallyCopyable(To& dest, From& src) {
    // To is trivially destructible, so we don't need to call its destructor
    // to end the lifetime of the object that was there previously
    constructAt(
        addressOf(dest),
        static_cast<From&&>(src)); // this is safe, we're not actually moving anything since the constructor is trivial
    return dest;
}

// Undefine macros for low-level memory manipulation functions
// to avoid conflicts with standard library functions

#undef memcpy
#undef memmove
#undef memset
#undef memchr
#undef memcmp

/**
 * @brief Copies `elemCount` of elements from the memory area `src` to the memory area `dest` (compile-time safe).
 * @tparam To The type of the destination pointer.
 * @tparam From The type of the source pointer.
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param elemCount The number of elements to copy.
 * @return A pointer to the destination memory area `dest`.
 * @note The memory areas must not overlap. Use `memmove` if the memory areas do overlap.
 */
template <class To, class From>
NEX_HIDDEN_FROM_ABI constexpr To* constexprMemcpy(To* dest, const From* src, usize elemCount) noexcept {
    // Branch at compile time on whether `From` is `void`, since `sizeof(void)` is ill-formed:
    // for the void case `elemCount` is already a byte count, for the generic case it is an element count.
    // Note: the builtin's return value is discarded (rather than static_cast to `To*`) because casting
    // a `void*` obtained from `__builtin_memcpy` back to a typed pointer is not a valid constant
    // expression prior to C++26; `dest` is already correctly typed, so it is returned directly instead.
    if constexpr (meta::IsVoidV<meta::RemoveCvT<From>>) {
#if NEX_HAS_BUILTIN(__builtin_memcpy)
        __builtin_memcpy(dest, src, elemCount);
#else
        auto* destBytes = static_cast<uint8*>(static_cast<void_ptr>(dest));
        auto* srcBytes = static_cast<const uint8*>(static_cast<const_void_ptr>(src));
        for (usize i = 0; i < elemCount; ++i) {
            destBytes[i] = srcBytes[i];
        }
#endif
        return dest;
    } else {
        auto sizeBytes = elemCount * sizeof(From);
#if NEX_HAS_BUILTIN(__builtin_memcpy)
        __builtin_memcpy(dest, src, sizeBytes);
#else
        for (usize i = 0; i < sizeBytes; ++i) {
            static_cast<uint8*>(static_cast<void_ptr>(dest))[i] =
                static_cast<const uint8*>(static_cast<const_void_ptr>(src))[i];
        }
#endif
        return dest;
    }
}

/**
 * @brief Copies `elemCount` of elements from the memory area `src` to the memory area `dest`.
 * @tparam To The type of the destination pointer.
 * @tparam From The type of the source pointer.
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param elemCount The number of elements to copy.
 * @return A pointer to the destination memory area `dest`.
 * @note The memory areas must not overlap. Use `memmove` if the memory areas do overlap.
 */
template <class To, class From>
NEX_HIDDEN_FROM_ABI constexpr To* memcpy(To* dest, const From* src, usize elemCount) noexcept {
    if NEX_CONSTEVAL_CONTEXT {
        return constexprMemcpy(dest, src, elemCount);
    } else if constexpr (meta::IsVoidV<meta::RemoveCvT<From>>) {
        // `elemCount` is already a byte count when `From` is void, since `sizeof(void)` is ill-formed.
        return static_cast<To*>(::memcpy(
            static_cast<void_ptr>(dest),
            static_cast<const_void_ptr>(src),
            elemCount
        ));
    } else {
        return static_cast<To*>(::memcpy(
            static_cast<void_ptr>(dest),
            static_cast<const_void_ptr>(src),
            elemCount * sizeof(From)
        ));
    }
}

/**
 * @brief Copies `count` of bytes from the memory area `src` to the memory area `dest`.
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param count The number of bytes to copy.
 * @return A pointer to the destination memory area `dest`.
 * @note The memory areas must not overlap. Use `memmove` if the memory areas do overlap.
 */
template <>
NEX_HIDDEN_FROM_ABI constexpr
void* memcpy(void* dest, const void* src, usize count) noexcept {
    if NEX_CONSTEVAL_CONTEXT {
        return constexprMemcpy(dest, src, count);
    } else {
        return ::memcpy(dest, src, count);
    }
}

// Internal helper to determine whether a value of Type1 can be compared
// with a value of Type2 using the less-than operator (<).
template <class Type1 = void, class Type2 = Type1>
struct NEX_HIDDEN_FROM_ABI Less_Internal {};

// Specialization for void types, which allows comparing any two types using the less-than operator (<).
template <>
struct NEX_HIDDEN_FROM_ABI Less_Internal<void, void> {
    template <class Type1, class Type2>
    NEX_HIDDEN_FROM_ABI constexpr bool operator()(const Type1& lhs, const Type2& rhs) const {
        return lhs < rhs;
    }
};

/**
 * @brief Checks if the range [first, last) is valid, meaning that first <= last.
 * @tparam Type The type of the pointers.
 * @param first A pointer to the first element of the range.
 * @param last A pointer to one past the last element of the range.
 * @return true if the range is valid (first <= last), false otherwise.
 * @note This function is constexpr and can be evaluated at compile time.
 */
template <class Type>
NEX_NO_SANITIZE("address") NEX_HIDDEN_FROM_ABI constexpr
bool isValidRange(const Type* first, const Type* last) {
    if NEX_CONSTEVAL_CONTEXT {
        // If this is not a constant during constant evaluation, that is because first and last
        // are not part of the same allocation. If they are part of the same allocation, we must still
        // make sure they are ordered properly.
        return __builtin_constant_p(first <= last) && first <= last;
    }

    return !Less_Internal<>()(last, first);
}

// Internal helper to determine if two pointer types are less-than comparable.
template <class Type1, class Type2, class = void>
struct NEX_HIDDEN_FROM_ABI IsLessThanComparable_Internal : meta::FalseType {};

// Specialization for pointer types that are less-than comparable.
template <class Type1, class Type2>
struct NEX_HIDDEN_FROM_ABI
    IsLessThanComparable_Internal<
        Type1, Type2,
        meta::VoidT<decltype(meta::declval<Type1>() < meta::declval<Type2>())>
    > : meta::TrueType {};

/**
 * @brief Checks if the pointer `ptr` is within the range [begin, end).
 * @tparam Type1 The type of the begin and end pointers.
 * @tparam Type2 The type of the ptr pointer.
 * @param begin A pointer to the first element of the range.
 * @param end A pointer to one past the last element of the range.
 * @param ptr A pointer to check if it is within the range.
 * @return true if ptr is within the range [begin, end), false otherwise.
 * @note This function is constexpr and can be evaluated at compile time.
 */
template <class Type1, class Type2>
    requires (IsLessThanComparable_Internal<const Type1*, const Type2*>::value)
NEX_NO_SANITIZE("address") NEX_HIDDEN_FROM_ABI constexpr
bool isPointerInRange(const Type1* begin, const Type1* end, const Type2* ptr) {
  NEX_MEMORY_ASSERT_VALID_INPUT_RANGE(isValidRange(begin, end), "[begin, end) is not a valid range");

    if NEX_CONSTEVAL_CONTEXT {
        // If this is not a constant during constant evaluation we know that ptr is not part of the allocation where
        // [begin, end) is.
        if (!__builtin_constant_p(begin <= ptr && ptr < end))
        return false;
    }

    return !Less_Internal<>()(ptr, begin) && Less_Internal<>()(ptr, end);
}

/**
 * @brief Checks if the pointer `ptr` is within the range [begin, end) for types that are not less-than comparable.
 * @tparam Type1 The type of the begin and end pointers.
 * @tparam Type2 The type of the ptr pointer.
 * @param begin A pointer to the first element of the range.
 * @param end A pointer to one past the last element of the range.
 * @param ptr A pointer to check if it is within the range.
 * @return true if ptr is within the range [begin, end), false otherwise.
 * @note This function is constexpr and can be evaluated at compile time.
 */
template <class Type1, class Type2>
    requires (!IsLessThanComparable_Internal<const Type1*, const Type2*>::value)
NEX_NO_SANITIZE("address") NEX_HIDDEN_FROM_ABI constexpr
bool isPointerInRange(const Type1* begin, const Type1* end, const Type2* ptr) {
    if NEX_CONSTEVAL_CONTEXT {
        return false;
    }

    return reinterpret_cast<const char*>(begin) <= reinterpret_cast<const char*>(ptr) &&
            reinterpret_cast<const char*>(ptr) < reinterpret_cast<const char*>(end);
}

/**
 * @brief Checks if the range [begin, end) overlaps with the range
 *        starting at begin2 and having the same size as [begin, end).
 * @tparam Type1 The type of the begin and end pointers.
 * @tparam Type2 The type of the begin2 pointer.
 * @param begin A pointer to the first element of the first range.
 * @param end A pointer to one past the last element of the first range.
 * @param begin2 A pointer to the first element of the second range.
 * @return true if the ranges overlap, false otherwise.
 * @note This function is constexpr and can be evaluated at compile time.
 */
template <class Type1, class Type2>
NEX_HIDDEN_FROM_ABI constexpr
bool isOverlappingRange(const Type1* begin, const Type1* end, const Type2* begin2) {
    auto size = end - begin;
    auto end2 = begin2 + size;
    return isPointerInRange(begin, end, begin2) || isPointerInRange(begin2, end2, begin);
}

/**
 * Checks whether an object of type `From` can always be bit-cast to an object of type `To` and represent a valid value
 * of type `To`. In other words, `From` and `To` have the same value representation and the set of values of `From` is
 * a subset of the set of values of `To`.
 * Note that types that cannot be assigned to each other using built-in assignment (e.g. arrays) might still be
 * considered bit-castable.
 */
template <class From, class To>
struct NEX_HIDDEN_FROM_ABI IsAlwaysBitcastable_Internal {
    using UnqualFrom NEX_NODEBUG = meta::RemoveCvT<From>;
    using UnqualTo NEX_NODEBUG   = meta::RemoveCvT<To>;

    static const bool value =
        // First, the simple case -- `From` and `To` are the same object type.
        (meta::IsSame<UnqualFrom, UnqualTo>::value && meta::IsTriviallyCopyable<UnqualFrom>::value) ||

        /**
         * @note
         * Beyond the simple case, we say that one type is "always bit-castable" to another if:
         * - (1) `From` and `To` have the same value representation, and in addition every possible value of `From` has
         *   a corresponding value in the `To` type (in other words, the set of values of `To` is a superset of the set of
         *   values of `From`);
         * - (2) When the corresponding values are not the same value (as, for example, between an unsigned and a signed
         *   integer, where a large positive value of the unsigned integer corresponds to a negative value in the signed
         *   integer type), the value of `To` that results from a bitwise copy of `From` is the same what would be
         *   produced by the built-in assignment (if it were defined for the two types, to which there are minor
         *   exceptions, e.g. built-in arrays).
         *
         * In practice, that means:
         * - all integral types (except `bool`, see below) -- that is, character types and `int` types, both signed and
         *   unsigned...
         * - as well as arrays of such types...
         * - ...that have the same size.
         *
         * Other trivially-copyable types can't be validly bit-cast outside of their own type:
         * - floating-point types normally have different sizes and thus aren't bit-castable between each other (fails
         * #1);
         * - integral types and floating-point types use different representations, so for example bit-casting an integral
         *   `1` to `float` results in a very small less-than-one value, unlike built-in assignment that produces `1.0`
         *   (fails #2);
         * - booleans normally use only a single bit of their object representation; bit-casting an integer to a boolean
         *   will result in a boolean object with an incorrect representation, which is undefined behavior (fails #2).
         *   Bit-casting from a boolean into an integer, however, is valid;
         * - enumeration types may have different ranges of possible values (fails #1);
         * - for pointers, it is not guaranteed that pointers to different types use the same set of values to represent
         *   addresses, and the conversion results are explicitly unspecified for types with different alignments
         *   (fails #1);
         * - for structs and unions it is impossible to determine whether the set of values of one of them is a subset of
         *   the other (fails #1);
         * - there is no need to consider `nullptr` for practical purposes.
         */
        (
            sizeof(From) == sizeof(To) &&
            meta::IsIntegralV<From> &&
            meta::IsIntegralV<To> &&
            !meta::IsSameV<UnqualTo, bool>
        );
};

#if (NEX_HAS_KEYWORD(__datasizeof) || NEX_HAS_EXTENSION(datasizeof)) && 0
    template <class Type>
    inline const usize datasizeofV = __datasizeof(Type);
#else
    template <class Type>
    struct NEX_HIDDEN_FROM_ABI FirstPaddingByte_Internal {
        NEX_NO_UNIQUE_ADDRESS Type v;
        char firstPaddingByte;
    };

    template <class Type>
    inline const usize datasizeofV = __builtin_offsetof(FirstPaddingByte_Internal<Type>, firstPaddingByte);
#endif

/**
 * @brief Moves `elemCount` of elements from the memory area `src` to the memory area `dest` (compile-time safe).
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param elemCount The number of elements to move.
 * @return A pointer to the destination memory area `dest`.
 * @note The memory areas may overlap. Use `memcpy` if the memory areas do not overlap.
 */
template <class To, class From>
NEX_HIDDEN_FROM_ABI constexpr To* constexprMemmove(To* dest, From* src, usize elemCount) noexcept {
    static_assert(IsAlwaysBitcastable_Internal<From, To>::value);

    if NEX_CONSTEVAL_CONTEXT {
#if NEX_HAS_BUILTIN(__builtin_memmove)
        if constexpr (meta::IsSameV<meta::RemoveCvT<To>, meta::RemoveCvT<From>>) {
            __builtin_memmove(dest, src, elemCount * sizeof(To));
            return dest;
        } else
#endif
        {
            if (isPointerInRange(src, src + elemCount, dest)) {
                for (; elemCount > 0; --elemCount) {
                    assignTriviallyCopyable(dest[elemCount - 1], src[elemCount - 1]);
                }
            } else {
                for (usize i = 0; i != elemCount; ++i) {
                    assignTriviallyCopyable(dest[i], src[i]);
                }
            }
        }
    } else if (elemCount > 0) {
#if NEX_HAS_BUILTIN(__builtin_memmove)
        __builtin_memmove(dest, src, (elemCount - 1) * sizeof(To) + datasizeofV<To>);
#else
        for (usize i = 0; i != elemCount; ++i) {
            assignTriviallyCopyable(dest[i], src[i]);
        }
#endif
    }

    return dest;
}

/**
 * @brief Moves `elemCount` of elements from the memory area `src` to the memory area `dest`.
 * @tparam To The type of the destination pointer.
 * @tparam From The type of the source pointer.
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param elemCount The number of elements to move.
 * @return A pointer to the destination memory area `dest`.
 * @note The memory areas may overlap. Use `memcpy` if the memory areas do not overlap.
 */
template <class To, class From>
NEX_HIDDEN_FROM_ABI constexpr To* memmove(To* dest, const From* src, usize elemCount) noexcept {
    if NEX_CONSTEVAL_CONTEXT {
        return constexprMemmove(dest, src, elemCount);
    } else {
        return static_cast<To*>(::memmove(static_cast<void_ptr>(dest), static_cast<const_void_ptr>(src), elemCount * sizeof(To)));
    }
}

/**
 * @brief Moves `count` bytes from the memory area `src` to the memory area `dest` (compile-time safe).
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param count The number of bytes to move.
 * @return A pointer to the destination memory area `dest`.
 * @note The memory areas may overlap. Unlike the generic overload, `void` pointers carry no element
 *       type, so there is no per-element assignment/bitcastable machinery to apply here -- this simply
 *       defers to the builtin/runtime memmove, which already handles overlap correctly on its own.
 */
template <>
NEX_HIDDEN_FROM_ABI constexpr
void* constexprMemmove(void* dest, const void* src, usize count) noexcept {
#if NEX_HAS_BUILTIN(__builtin_memmove)
    __builtin_memmove(dest, src, count);
#else
    if (isPointerInRange(static_cast<const uint8*>(src), static_cast<const uint8*>(src) + count, static_cast<const uint8*>(dest))) {
        for (usize i = count; i > 0; --i) {
            static_cast<uint8*>(dest)[i - 1] = static_cast<const uint8*>(src)[i - 1];
        }
    } else {
        for (usize i = 0; i != count; ++i) {
            static_cast<uint8*>(dest)[i] = static_cast<const uint8*>(src)[i];
        }
    }
#endif
    return dest;
}

/**
 * @brief Moves `count` bytes from the memory area `src` to the memory area `dest`.
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param count The number of bytes to move.
 * @return A pointer to the destination memory area `dest`.
 * @note The memory areas may overlap. Use `memcpy` if the memory areas do not overlap.
 */
template <>
NEX_HIDDEN_FROM_ABI constexpr
void* memmove(void* dest, const void* src, usize count) noexcept {
    if NEX_CONSTEVAL_CONTEXT {
        return constexprMemmove(dest, src, count);
    } else {
        return ::memmove(dest, src, count);
    }
}

/**
 * @brief Sets the first `count` bytes of the memory area pointed to by `dest`
 *        to the specified value `value` (compile-time safe).
 * @tparam Type The type of the pointer to the memory area (must be a byte-sized type).
 * @param dest A pointer to the memory area to be set.
 * @param value The value to be set (interpreted as an unsigned char).
 * @param count The number of bytes to set.
 * @return A pointer to the memory area `dest`.
 * @note Only available for byte-sized `Type`s: filling a wider type with a repeating byte
 *       pattern requires reinterpreting its object representation, which cannot be expressed
 *       as a valid constant expression in general (this mirrors why libc++ does not provide
 *       a `__constexpr_memset` at all). Wider types always defer to the runtime `memset` below.
 */
template <class Type>
    requires (sizeof(Type) == 1)
NEX_HIDDEN_FROM_ABI constexpr Type* constexprMemset(Type* dest, int32 value, usize count) noexcept {
    auto byteValue = static_cast<Type>(static_cast<uint8>(value));
    for (usize i = 0; i < count; ++i) {
        dest[i] = byteValue;
    }
    return dest;
}

/**
 * @brief Sets the first `count` bytes of the memory area pointed to by `dest` to the specified value `value`.
 * @tparam Type The type of the pointer to the memory area.
 * @param dest A pointer to the memory area to be set.
 * @param value The value to be set (interpreted as an unsigned char).
 * @param count The number of bytes to set.
 * @return A pointer to the memory area `dest`.
 * @note Compile-time evaluation is only available when `sizeof(Type) == 1`; for wider types
 *       `memset` always defers to the runtime C library implementation (see `constexprMemset`).
 * @note `Type` is checked for `void` in its own `if constexpr` branch (rather than combined into
 *       a single `sizeof(Type) == 1` condition via `&&`) because `sizeof(void)` is ill-formed --
 *       a dependent `sizeof` expression must never be type-checked for `Type = void`, and only a
 *       genuinely separate/nested `if constexpr` branch (not short-circuit evaluation) guarantees
 *       that. `void` has no element representation to write through anyway, so it always defers
 *       straight to the runtime `::memset` below, matching `void* memset(void*, int, size_t)`.
 */
template <class Type>
NEX_HIDDEN_FROM_ABI constexpr Type* memset(Type* dest, int32 value, usize count) noexcept {
    if constexpr (meta::IsVoidV<meta::RemoveCvT<Type>>) {
        // no-op: falls through to the runtime memset call below.
    } else if constexpr (sizeof(Type) == 1) {
        if NEX_CONSTEVAL_CONTEXT {
            return constexprMemset(dest, value, count);
        }
    }
    return static_cast<Type*>(::memset(static_cast<void_ptr>(dest), value, count));
}

// Internal helper to determine whether a value of Type1 can be compared
// with a value of Type2 using the equality operator (==).
template <class Type1, class Type2, class = void>
struct NEX_HIDDEN_FROM_ABI IsEqualityComparable_Internal : meta::FalseType {};

template <class Type1, class Type2>
struct NEX_HIDDEN_FROM_ABI
    IsEqualityComparable_Internal<
        Type1, Type2, 
        meta::VoidT<
            decltype(meta::declval<Type1>() == meta::declval<Type2>())>
        > : meta::TrueType {};

template <class Type1, class Type2, class = void>
struct IsTriviallyEqualityComparableImpl_Internal : meta::FalseType {};

template <class Type1, class Type2>
struct NEX_HIDDEN_FROM_ABI
    IsTriviallyEqualityComparableImpl_Internal<Type1, Type2>
#if NEX_HAS_BUILTIN(__is_trivially_equality_comparable)
        : meta::IntegralConstant<bool,
            __is_trivially_equality_comparable(Type1) && 
            IsEqualityComparable_Internal<Type1, Type2>::value> {};
#else
        : meta::IsIntegral<Type1> {};
#endif

template <class Type1, class Type2>
struct NEX_HIDDEN_FROM_ABI
    IsTriviallyEqualityComparableImpl_Internal<
        Type1,
        Type2,
        meta::EnableIfT<
            meta::IsIntegralV<Type1> &&
            meta::IsIntegralV<Type2> &&
            !meta::IsSameV<Type1, Type2> &&
            meta::IsSignedIntegralV<Type1> == meta::IsSignedIntegralV<Type2> &&
            sizeof(Type1) == sizeof(Type2)>
        > : meta::TrueType {};

template <class Type1>
struct NEX_HIDDEN_FROM_ABI
    IsTriviallyEqualityComparableImpl_Internal<Type1*, Type1*> : meta::TrueType {};

template <class Type1, class Type2>
struct NEX_HIDDEN_FROM_ABI
    IsTriviallyEqualityComparableImpl_Internal<Type1*, Type2*>
        : meta::IntegralConstant<
            bool,
            IsEqualityComparable_Internal<Type1*, Type2*>::value &&
            (meta::IsSameV<meta::RemoveCvT<Type1>, meta::RemoveCvT<Type2>> ||
                meta::IsVoidV<Type1> || meta::IsVoidV<Type2>)> {};

// Internal helper to determine if two types are trivially equality comparable.
template <class Type1, class Type2>
using IsTriviallyEqualityComparable_Internal NEX_NODEBUG =
    IsTriviallyEqualityComparableImpl_Internal<meta::RemoveCvT<Type1>, meta::RemoveCvT<Type2> >;

// Internal helper (boolean form) to determine if two types are trivially equality comparable.
template <class Type1, class Type2>
inline const bool IsTriviallyEqualityComparableV_Internal =
    IsTriviallyEqualityComparable_Internal<Type1, Type2>::value;

/**
 * @brief Searches for the first occurrence of the character `c` (interpreted as an unsigned char)
 *        in the first `count` bytes of the memory area pointed to by `ptr` (compile-time safe).
 * @tparam Type1 The type of the pointer to the memory area.
 * @tparam Type2 The type of the character to be searched for.
 * @param ptr A pointer to the memory area to be searched.
 * @param c The character to be searched for (interpreted as an unsigned char).
 * @param count The number of bytes to be searched.
 * @return A pointer to the first occurrence of the character `c` in the memory area, or `nullptr`
 *         if the character is not found.
 */
template <class Type1, class Type2>
NEX_HIDDEN_FROM_ABI constexpr
Type1* constexprMemchr(Type1* str, Type2 value, usize count) {
    static_assert(sizeof(Type1) == 1 && IsTriviallyEqualityComparableV_Internal<Type1, Type2>,
                    "Calling memchr on non-trivially equality comparable types is unsafe.");

    if NEX_CONSTEVAL_CONTEXT {
    // use __builtin_char_memchr to optimize constexpr evaluation if we can
#if NEX_HAS_BUILTIN(__builtin_char_memchr)
        if constexpr (meta::IsSameV<meta::RemoveCvT<Type1>, char> && meta::IsSameV<meta::RemoveCvT<Type2>, char>) {
            return __builtin_char_memchr(str, value, count);
        } else
#endif
        {
            for (; count; --count) {
                if (*str == value)
                    return str;
                ++str;
            }
            return nullptr;
        }
    } else {
#if NEX_HAS_BUILTIN(__builtin_memcpy) && NEX_HAS_BUILTIN(__builtin_memchr)
        char value_buffer = 0;
        __builtin_memcpy(&value_buffer, addressOf(value), sizeof(char));
        return static_cast<Type1*>(__builtin_memchr(str, value_buffer, count));
#else
        for (; count; --count) {
            if (*str == value)
                return str;
            ++str;
        }
        return nullptr;
#endif
    }
}

/**
 * @brief Searches for the first occurrence of the character `c` (interpreted as an unsigned char)
 *        in the first `count` bytes of the memory area pointed to by `ptr`.
 * @tparam Type1 The type of the pointer to the memory area.
 * @tparam Type2 The type of the character to be searched for.
 * @param ptr A pointer to the memory area to be searched.
 * @param c The character to be searched for (interpreted as an unsigned char).
 * @param count The number of bytes to be searched.
 * @return A pointer to the first occurrence of the character `c` in the memory area, or `nullptr`
 *         if the character is not found.
 */
template <class Type1, class Type2>
NEX_HIDDEN_FROM_ABI constexpr
Type1* memchr(const Type1* str, Type2 value, usize count) {
    if NEX_CONSTEVAL_CONTEXT {
        return constexprMemchr(const_cast<Type1*>(str), value, count);
    } else {
        return static_cast<Type1*>(::memchr(const_cast<Type1*>(str), static_cast<int32>(value), count));
    }
}

/**
 * @brief Searches for the first occurrence of the character `c` (interpreted as an unsigned char)
 *        in the first `count` bytes of the memory area pointed to by `ptr` (compile-time safe).
 * @param ptr A pointer to the memory area to be searched.
 * @param value The character to be searched for (interpreted as an unsigned char).
 * @param count The number of bytes to be searched.
 * @return A pointer to the first occurrence of the character `value` in the memory area, or `nullptr`
 *         if the character is not found.
 * @note Unlike the generic overload, a `void` pointer has no element type, so the trivial-equality-
 *       comparability restriction does not apply here -- this always searches byte-by-byte.
 */
template <>
NEX_HIDDEN_FROM_ABI constexpr
void* constexprMemchr(void* str, int32 value, usize count) {
#if NEX_HAS_BUILTIN(__builtin_memchr)
    return __builtin_memchr(str, value, count);
#else
    auto* bytes = static_cast<uint8*>(str);
    auto byteValue = static_cast<uint8>(value);
    for (; count; --count) {
        if (*bytes == byteValue)
            return bytes;
        ++bytes;
    }
    return nullptr;
#endif
}

/**
 * @brief Searches for the first occurrence of the character `c` (interpreted as an unsigned char)
 *        in the first `count` bytes of the memory area pointed to by `ptr`.
 * @param ptr A pointer to the memory area to be searched.
 * @param value The character to be searched for (interpreted as an unsigned char).
 * @param count The number of bytes to be searched.
 * @return A pointer to the first occurrence of the character `value` in the memory area, or `nullptr`
 *         if the character is not found.
 */
template <>
NEX_HIDDEN_FROM_ABI constexpr
void* memchr(const void* str, int32 value, usize count) {
    if NEX_CONSTEVAL_CONTEXT {
        return constexprMemchr(const_cast<void*>(str), value, count);
    } else {
        return const_cast<void*>(::memchr(str, value, count));
    }
}

// Internal helper to determine if a type is a byte type (i.e., `byte`).
template <class Type>
inline const bool IsByteV_Internal = false;

// Specialization for the `byte` type, indicating that it is a byte type.
template <>
inline const bool IsByteV_Internal<byte> = true;

// Internal helper to determine if two types are trivially lexicographically comparable.
template <class Type1, class Type2>
inline const bool IsTriviallyLexicographicallyComparableV_Internal =
    meta::IsSameV<meta::RemoveCvT<Type1>, meta::RemoveCvT<Type2> > &&
#ifdef NEX_LITTLE_ENDIAN
    sizeof(Type1) == 1 &&
#endif
    (meta::IsUnsignedIntegralV<Type1> || IsByteV_Internal<Type1>);

/**
 * @brief Compares the first `elemCount` of elements of the memory areas `ptr1` and `ptr2`.
 * @tparam Type1 The type of the elements in the first memory area.
 * @tparam Type2 The type of the elements in the second memory area.
 * @param ptr1 A pointer to the first memory area to be compared.
 * @param ptr2 A pointer to the second memory area to be compared.
 * @param elemCount The number of elements to be compared.
 * @return An integer less than, equal to, or greater than zero if the first `elemCount` elements of `ptr1`
 *         are found, respectively, to be less than, to match, or be greater than the first `elemCount`
 *         elements of `ptr2`.
 */
template <class Type1, class Type2>
NEX_HIDDEN_FROM_ABI constexpr
int constexprMemcmp(const Type1* lhs, const Type2* rhs, usize elemCount) {
    // Because of IsTriviallyLexicographicallyComparableV_Internal we know that comparing the object
    // representations is equivalent to a ::memcmp. Since we have multiple objects contiguously in memory,
    // we can call ::memcmp once instead of invoking it on every object individually.
    static_assert(IsTriviallyLexicographicallyComparableV_Internal<Type1, Type2>,
                    "Type1 and Type2 have to be trivially lexicographically comparable");

    if NEX_CONSTEVAL_CONTEXT {
#if NEX_HAS_BUILTIN(__builtin_memcmp)
        if (sizeof(Type1) == 1 && !meta::IsSameV<Type1, bool>) {
            return __builtin_memcmp(lhs, rhs, elemCount * sizeof(Type1));
        }
#endif
        while (elemCount != 0) {
            if (*lhs < *rhs)
                return -1;
            if (*rhs < *lhs)
                return 1;

            --elemCount;
            ++lhs;
            ++rhs;
        }
        return 0;
    } else {
#if NEX_HAS_BUILTIN(__builtin_memcmp)
        return __builtin_memcmp(lhs, rhs, elemCount * sizeof(Type1));
#else
        while (elemCount != 0) {
            if (*lhs < *rhs)
                return -1;
            if (*rhs < *lhs)
                return 1;

            --elemCount;
            ++lhs;
            ++rhs;
        }
        return 0;
#endif
    }
}

/**
 * @brief Compares the first `elemCount` of elements of the memory areas `ptr1` and `ptr2`.
 * @tparam Type1 The type of the elements in the first memory area.
 * @tparam Type2 The type of the elements in the second memory area.
 * @param ptr1 A pointer to the first memory area to be compared.
 * @param ptr2 A pointer to the second memory area to be compared.
 * @param elemCount The number of elements to be compared.
 * @return true if the first `elemCount` elements of `ptr1` are equal to the first `elemCount` elements of `ptr2`,
 *         false otherwise.
 */
template <class Type1, class Type2>
NEX_HIDDEN_FROM_ABI constexpr
bool constexprMemcmpEqual(const Type1* lhs, const Type2* rhs, usize elemCount) {
    // Because of IsTriviallyEqualityComparableV_Internal we know that comparing the object representations
    // is equivalent to a ::memcmp(...) == 0. Since we have multiple objects contiguously in memory, we can
    // call memcmp once instead of invoking it on every object individually.
    static_assert(IsTriviallyEqualityComparableV_Internal<Type1, Type2>,
                    "Type1 and Type2 have to be trivially equality comparable");

    if NEX_CONSTEVAL_CONTEXT {
#if NEX_HAS_BUILTIN(__builtin_memcmp)
        if (sizeof(Type1) == 1 && meta::IsIntegralV<Type1> && !meta::IsSameV<Type1, bool>) {
            return __builtin_memcmp(lhs, rhs, elemCount * sizeof(Type1)) == 0;
        }
#endif
        while (elemCount != 0) {
            if (*lhs != *rhs)
                return false;

            --elemCount;
            ++lhs;
            ++rhs;
        }
        return true;
    } else {
#if NEX_HAS_BUILTIN(__builtin_memcmp)
        return ::__builtin_memcmp(lhs, rhs, elemCount * sizeof(Type1)) == 0;
#else
        while (elemCount != 0) {
            if (*lhs != *rhs)
                return false;

            --elemCount;
            ++lhs;
            ++rhs;
        }
        return true;
#endif
    }
}

/**
 * @brief Compares the first `elemCount` of elements of the memory areas `ptr1` and `ptr2`.
 * @param ptr1 A pointer to the first memory area to be compared.
 * @param ptr2 A pointer to the second memory area to be compared.
 * @param elemCount The number of elements to be compared.
 * @return An integer less than, equal to, or greater than zero if the first `elemCount` elements of `ptr1`
 *         are found, respectively, to be less than, to match, or be greater than the first `elemCount`
 *         elements of `ptr2`.
 */
template <class Type1, class Type2>
NEX_HIDDEN_FROM_ABI constexpr
int memcmp(const Type1* lhs, const Type2* rhs, usize elemCount) {
    if NEX_CONSTEVAL_CONTEXT {
        return constexprMemcmp(lhs, rhs, elemCount);
    } else {
        return ::memcmp(
            static_cast<const_void_ptr>(lhs),
            static_cast<const_void_ptr>(rhs),
            elemCount * sizeof(Type1)
        );
    }
}

/**
 * @brief Compares the first `count` bytes of the memory areas `ptr1` and `ptr2`.
 * @param ptr1 A pointer to the first memory area to be compared.
 * @param ptr2 A pointer to the second memory area to be compared.
 * @param count The number of bytes to be compared.
 * @return An integer less than, equal to, or greater than zero if the first `count` bytes of `ptr1`
 *         are found, respectively, to be less than, to match, or be greater than the first `count`
 *         bytes of `ptr2`.
 * @note A `void` pointer has no element type, so it cannot be forwarded to `constexprMemcmp` --
 *       punning `const void*` to a typed pointer (e.g. `const uint8*`) is not a valid constant
 *       expression prior to C++26. This relies directly on `__builtin_memcmp`, which Clang/GCC
 *       special-case for constant evaluation even when passed `void*` arguments.
 */
template <>
NEX_HIDDEN_FROM_ABI constexpr
int memcmp(const void* lhs, const void* rhs, usize count) {
    if NEX_CONSTEVAL_CONTEXT {
        return __builtin_memcmp(lhs, rhs, count);
    } else {
        return ::memcmp(lhs, rhs, count);
    }
}

/**
 * @brief Calculates the offset of a member within a struct/class.
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

/**
 * @brief Retrieves the containing struct/class from a pointer to a member.
 * @tparam Type The type of the containing struct/class.
 * @tparam MemberPtr A pointer to the member within the struct/class.
 * @param ptr A pointer to the member within the struct/class.
 * @return A pointer to the containing struct/class.
 * @note This function is only safe for standard-layout types. It uses reinterpret_cast to calculate
 *       the address of the containing struct/class from the address of the member.
 */
template <typename Type, auto MemberPtr>
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr 
Type* containerOf(
    meta::RemoveReferenceT<decltype(meta::declval<Type>().*MemberPtr)>* ptr
) noexcept {
    return _containerOf_Impl<Type, MemberPtr>(ptr);
}

/**
 * @brief Checks if a pointer is aligned to a specified byte boundary.
 * @param ptr The pointer to check for alignment.
 * @param alignment The alignment boundary in bytes (must be a power of two).
 * @return True if the pointer is aligned to the specified boundary; otherwise, false.
 * @note This function is useful for ensuring that pointers meet specific alignment requirements, 
 *       which can be critical for performance and correctness in low-level programming.
 */
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr 
bool isAligned(const_void_ptr ptr, usize alignment) noexcept {
    return (reinterpret_cast<uintptr>(ptr) & (alignment - 1)) == 0;
}

/**
 * @brief Aligns a given address up to the next multiple of the specified alignment.
 * @param addr The address to be aligned.
 * @param alignment The alignment boundary in bytes (must be a power of two).
 * @return The aligned address, which is the next multiple of the specified alignment.
 * @note This function is useful for ensuring that memory addresses meet specific alignment requirements, 
 *       which can be critical for performance and correctness in low-level programming.
 */
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr 
uintptr alignUp(uintptr addr, usize alignment) noexcept {
    return (addr + (alignment - 1)) & ~(alignment - 1);
}

/**
 * @brief Aligns a given pointer up to the next multiple of the specified alignment.
 * @tparam T The type of the pointer.
 * @param ptr The pointer to be aligned.
 * @param alignment The alignment boundary in bytes (must be a power of two).
 * @return The aligned pointer, which is the next multiple of the specified alignment.
 * @note This function is useful for ensuring that pointers meet specific alignment requirements, 
 *       which can be critical for performance and correctness in low-level programming.
 */
template <typename Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr 
Type* alignUpPointer(Type* ptr, usize alignment) noexcept {
    return reinterpret_cast<Type*>(alignUp(reinterpret_cast<uintptr>(ptr), alignment));
}

/**
 * @brief Prefetches a memory address into the CPU cache to improve access speed.
 * @param ptr A pointer to the memory address to prefetch.
 * @note This function is a hint to the CPU that the specified memory address will be accessed soon, 
 *       allowing the CPU to load it into the cache ahead of time. This can improve performance in 
 *       scenarios where memory access patterns are predictable.
 */
NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr
void prefetch(const_void_ptr ptr) noexcept {
#if NEX_HAS_BUILTIN(__builtin_prefetch)
    __builtin_prefetch(ptr, 0, 3);   // Read, high temporal locality
#elif NEX_COMPILER_IS_MSVC
    _mm_prefetch(static_cast<const_nchar_ptr>(ptr), NEX_MM_HINT_T0);
#endif  // ^^^prefetch
}

// ======================================================================================
// Define macro for object address retrieval
// ======================================================================================

// Obtains the actual address of an object, safely bypassing any overloaded operator&.
#define NEX_ADDRESS_OF \
    NEX_PREPEND_NAMESPACE(addressOf)

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

// Copies `count` elements from the memory area `src` to the memory area `dest`.
#define NEX_MEMCPY(dest, src, count) \
    NEX_PREPEND_NAMESPACE(memcpy(dest, src, count))

// Moves `count` elements from the memory area `src` to the memory area `dest`.
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
// Define macros for struct/class member offset and container retrieval
// ======================================================================================

// Calculates the offset of a member within a struct/class.
#define NEX_OFFSET_OF(type, member) \
    NEX_PREPEND_NAMESPACE(offsetOf<type>(&type::member))

// Retrieves the containing struct/class from a pointer to a member.
#define NEX_CONTAINER_OF \
    NEX_PREPEND_NAMESPACE(containerOf)

NEX_NAMESPACE_END
