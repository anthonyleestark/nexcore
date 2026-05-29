/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    casts.h
 * @brief   Common casting utilities and functions for type conversions.
 * 
 * @details
 * This header defines common casting utilities and functions that can be used throughout the codebase for type 
 * conversions. It includes functions for safe downcasting, bitwise casting, and other common casting operations 
 * that may be needed when working with different types in C++. The goal is to provide a centralized location 
 * for casting-related utilities to promote code reuse and consistency across the Nex-ecosystem.
 */

#include "nex/base/compiler.h"
#include "nex/base/build.h"
#include "nex/base/attributes.h"
#include "nex/base/namespace.h"
#include "nex/base/intrinsics.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

// ======================================================================================
// Core casting utilities
// ======================================================================================

#if NEX_COMPILER_MSVC_COMPATIBLE
    #pragma warning(push)
    // Disable uninitialized variable warnings in this section, 
    // as some of our casting utilities may intentionally leave variables uninitialized
    #pragma warning(disable: 26495)
    #pragma warning(disable: 5240)  // 'nodiscard': attribute is ignored in this syntactic position
#endif

/**
 * @brief Forwards an lvalue argument as either an lvalue or an rvalue reference 
 *        depending on the template parameter Type.
 * @tparam Type The target value category to forward to.
 * @param arg The lvalue argument to be forwarded.
 * @return The forwarded reference (Type&&).
 */
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Type&& forwardCast(type_traits::RemoveReferenceT<Type>& arg) noexcept {
    return static_cast<Type&&>(arg);
}

/**
 * @brief Forwards an rvalue argument as an rvalue reference.
 * @note Enforces at compile-time that an rvalue cannot be mistakenly forwarded as an lvalue reference.
 * @tparam Type The target value category to forward to.
 * @param arg The rvalue temporary to be forwarded.
 * @return The forwarded rvalue reference (Type&&).
 */
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Type&& forwardCast(type_traits::RemoveReferenceT<Type>&& arg) noexcept {
    static_assert(!type_traits::IsLvalueReferenceV<Type>, 
        "Error: Bad forward call, cannot forward an rvalue as an lvalue reference!");
    return static_cast<Type&&>(arg);
}
/**
 * @brief Casts an lvalue object to an rvalue reference to enable move semantics.
 * @note Strictly enforces that the object is non-const. Moving a const object triggers a compile-time error 
 *       to prevent a silent, expensive deep copy (Unreal Engine's MoveTemp() style).
 * @tparam Type The deduced type of the lvalue object.
 * @param arg The lvalue object to be moved.
 * @return An rvalue reference pointing to the object.
 */
template <class Type> 
requires (!type_traits::IsLvalueReferenceV<Type>)
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
type_traits::RemoveReferenceT<Type>&& moveCast(Type& arg) noexcept {
    static_assert(!type_traits::IsConstV<type_traits::RemoveReferenceT<Type>>, 
        "Error: Cannot move from a const-qualified type, as it would result in a silent copy instead of a move!");
    return static_cast<type_traits::RemoveReferenceT<Type>&&>(arg);
}

/**
 * @brief Overload that catches rvalue temporaries passed to moveCast.
 * @note Triggers a compile-time error because invoking a move cast on an object 
 *       that is already an rvalue temporary is redundant and unnecessary.
 * @tparam Type The deduced type of the rvalue temporary.
 * @param arg The rvalue temporary object.
 * @return An rvalue reference pointing to the object.
 */
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
type_traits::RemoveReferenceT<Type>&& moveCast(Type&& arg) noexcept {
    static_assert(!type_traits::IsLvalueReferenceV<Type>, 
        "Error: Moving on an rvalue temporary object is redundant and unnecessary!");
    return static_cast<type_traits::RemoveReferenceT<Type>&&>(arg);
}

/**
 * @brief Conditionally casts an object to an rvalue reference if its move constructor 
 *        is guaranteed not to throw exceptions, otherwise falls back to a const lvalue reference.
 * @tparam Type The type of the object being evaluated.
 * @param arg The object to be conditionally moved or copied.
 * @return An rvalue reference if noexcept move-constructible; otherwise, a const lvalue reference for safe copying.
 */
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr
type_traits::ConditionalT<
    !type_traits::IsNothrowMoveConstructibleV<Type> && type_traits::IsCopyConstructibleV<Type>, 
    const Type&, 
    type_traits::RemoveReferenceT<Type>&&
>
moveIfNoexcept(Type& arg) noexcept {
    return static_cast<type_traits::RemoveReferenceT<Type>&&>(arg);
}

/**
 * @brief Obtains the actual address of an object, safely bypassing any overloaded operator&.
 * @note Utilizes a compiler built-in to guarantee the extraction of the real memory address, 
 *       even if the type has a custom or malicious address-of operator.
 * @tparam Type The type of the object whose address is being taken.
 * @param Value The lvalue reference to the object.
 * @return A pointer to the object (Type*).
 */
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Type* addressOf(Type& Value) noexcept {
    return __builtin_addressof(Value);
}

/**
 * @brief Deleted overload to explicitly prevent taking the address of rvalue temporaries.
 * @note This overload triggers a compile-time error if a temporary object is passed, 
 *       protecting against dangerous dangling pointers to short-lived resources.
 * @tparam Type The type of the rvalue temporary.
 */
template <class Type>
const Type* addressOf(const Type&&) = delete;

/**
 * @brief Reinterprets the bits of a source value as a destination type.
 * @note Safe alternative to reinterpret_cast or memcpy for bit-blitting. 
 *       Requires types to be trivially copyable and of identical size.
 * @tparam Dest The target type to convert to.
 * @tparam Source The source type to convert from.
 */
template <class Dest, class Source>
    requires (sizeof(Dest) == sizeof(Source) && 
              type_traits::IsTriviallyCopyableV<Source> && 
              type_traits::IsTriviallyCopyableV<Dest>)
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Dest bitCast(const Source& source) noexcept {
    return __builtin_bit_cast(Dest, source);
}

/**
 * @brief Performs a safe, implicit upcast or const-cast that is checked by the compiler.
 * @note Ensures that the conversion could happen implicitly without a forceful static_cast.
 * @tparam Dest The target type (must be implicitly convertible from Source).
 */
template <class Dest, class Source>
requires type_traits::IsConvertibleV<Source, Dest>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Dest implicitCast(Source&& source) noexcept {
    return forwardCast<Source>(source);
}

/**
 * @brief Safely casts between numeric types, checking for overflow/underflow in Debug builds.
 * @note Acts as a standard static_cast in Release builds, but panics in Debug if data loss occurs.
 */
template <class Dest, class Source>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr Dest numericCast(Source value) noexcept {
    #if NEX_BUILD_MODE_IS_DEBUG
        // Perform a checked cast that panics if the value cannot be represented in the destination type
        Dest casted = static_cast<Dest>(value);
        NEX_ASSERT_MSG(static_cast<Source>(casted) == value, "Error: Integer overflow or truncation detected!");
        return casted;
    #else
        // In Release builds, perform a regular static_cast without the overhead of checks
        return static_cast<Dest>(value);
    #endif
}

/**
 * @brief Safely downcasts a polymorphic pointer. 
 *        Uses dynamic_cast in Debug builds, but optimizes to static_cast in Release builds.
 * @note Requires RTTI to be enabled only during Debug builds for verification.
 */
template <class Dest, class Source>
NEX_NODISCARD NEX_MSVC_INTRINSIC Dest polymorphicCast(Source* polyPointer) noexcept {
    #if NEX_BUILD_MODE_IS_DEBUG
        if (polyPointer == nullptr) return nullptr;
        Dest result = dynamic_cast<Dest>(polyPointer);
        NEX_ASSERT_MSG(result != nullptr, "Error: Invalid downcast detected!");
        return result;
    #else
        return static_cast<Dest>(polyPointer);
    #endif
}

/**
 * @brief Get the containing struct/class from a pointer to a member.
 * @tparam Type The type of the containing struct/class.
 * @tparam MemberPtr A pointer to the member within the struct/class.
 */
template <typename Type, auto MemberPtr>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Type* containerOf(
    type_traits::RemoveReferenceT<decltype(type_traits::declval<Type>().*MemberPtr)>* ptr
) noexcept {
    static_assert(type_traits::IsStandardLayoutV<Type>, 
        "Error: 'containerOf' only safe for standard-layout types");

    if (ptr == nullptr) return nullptr;

#if NEX_COMPILER_IS_MSVC
    #if NEX_BUILD_ENV_IS_64_BIT
        // 64-bit build, where pointer size is 8 bytes
        using ptrsize_type = unsigned __int64;
        using ptrdiff_type = __int64;
    #else
        // 32-bit build, where pointer size is 4 bytes
        using ptrsize_type = unsigned __int32;
        using ptrdiff_type = __int32;
    #endif
#else
    // On GCC/Clang (or on non-Windows platforms), 
    // we can use the built-in types that match the pointer size
    using ptrsize_type = __UINTPTR_TYPE__;
    using ptrdiff_type = __PTRDIFF_TYPE__;
#endif

    return reinterpret_cast<Type*>(
        reinterpret_cast<ptrsize_type>(ptr) - static_cast<ptrsize_type>(
            reinterpret_cast<ptrdiff_type>(&reinterpret_cast<Type*>(0)->*MemberPtr)
        )
    );
}

/**
 * @brief Safely downcasts a pointer to a member to a pointer to its containing structure.
 * @tparam Type The type of the containing structure/class.
 * @tparam MemberPtr The compile-time member pointer.
 */
template <typename Type, auto MemberPtr>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Type* downcastMember(
    type_traits::RemoveReferenceT<decltype(type_traits::declval<Type>().*MemberPtr)>* memberPtr
) noexcept {
    return containerOf<Type, MemberPtr>(memberPtr);
}

/**
 * @brief Safely dereferences a member pointer, returning a reference to its containing structure.
 * @tparam Type The type of the containing structure/class.
 * @tparam MemberPtr The compile-time member pointer.
 */
template <typename Type, auto MemberPtr>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Type& derefMember(
    type_traits::RemoveReferenceT<decltype(type_traits::declval<Type>().*MemberPtr)>* memberPtr
) noexcept {
    return *containerOf<Type, MemberPtr>(memberPtr);
}

/**
 * @brief Safely downcasts a base class pointer to a derived class pointer 
 *        with compile-time inheritance verification.
 * @note Fully supports constexpr environments. 
 *       In runtime debug builds, it falls back to polymorphicCast for dynamic verification.
 */
template <typename Derived, typename Base>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Derived* safeDowncast(Base* base) noexcept {
    static_assert(type_traits::IsBaseOfV<Base, Derived>, 
        "Error: 'safeDowncast' requires 'Derived' to be a valid subclass of 'Base'. "
        "If you are attempting cross-casting between independent interfaces, consider using 'polymorphicCast' instead.");
    if consteval {
        // In a constant evaluation context, we can safely use static_cast() 
        // since the compiler will enforce the inheritance relationship at compile time
        return static_cast<Derived*>(base);
    } else {
        // In a runtime context, we can use polymorphicCast() 
        // to perform a safe downcast with RTTI checks in Debug builds
        return polymorphicCast<Derived>(base);
    }
}

#if NEX_COMPILER_MSVC_COMPATIBLE
    #pragma warning(pop)
#endif

// =====================================================================================
// Define macros for the clear usage of the casting utilities
// =====================================================================================

// Forward an argument as either an lvalue or rvalue reference based on the template parameter Type
#define NEX_FORWARD \
    NEX_PREPEND_NAMESPACE(forwardCast)

// Cast an lvalue to an rvalue reference to enable move semantics, 
// with strict compile-time checks to prevent misuse
#define NEX_MOVE \
    NEX_PREPEND_NAMESPACE(moveCast)

// Conditionally casts to an rvalue reference if the type is nothrow move constructible, 
// otherwise falls back to a const lvalue reference
#define NEX_MOVE_IF_NOEXCEPT \
    NEX_PREPEND_NAMESPACE(moveIfNoexcept)

// Obtains the actual address of an object
#define NEX_ADDRESS_OF \
    NEX_PREPEND_NAMESPACE(addressOf)

// Reinterprets the bits of a source value as a destination type, 
// with safety checks for trivial copyability and size
#define NEX_BIT_CAST \
    NEX_PREPEND_NAMESPACE(bitCast)

// Performs a safe, implicit upcast or const-cast that is checked by the compiler
#define NEX_IMPLICIT_CAST \
    NEX_PREPEND_NAMESPACE(implicitCast)

// Safely casts between numeric types, checking for overflow/underflow in Debug builds
#define NEX_NUMERIC_CAST \
    NEX_PREPEND_NAMESPACE(numericCast)

// Safely downcasts a polymorphic pointer, 
// using dynamic_cast in Debug builds and static_cast in Release builds
#define NEX_POLYMORPHIC_CAST \
    NEX_PREPEND_NAMESPACE(polymorphicCast)

// Get the containing struct/class from a pointer to a member
#define NEX_CONTAINER_OF \
    NEX_PREPEND_NAMESPACE(containerOf)

// Safely downcasts a pointer to a member to a pointer to its containing structure
#define NEX_DOWNCAST_MEMBER \
    NEX_PREPEND_NAMESPACE(downcastMember)

// Safely dereferences a member pointer, returning a reference to its containing structure
#define NEX_DEREF_MEMBER \
    NEX_PREPEND_NAMESPACE(derefMember)

// Safely downcasts a base class pointer to a derived class pointer 
// with compile-time inheritance verification
#define NEX_SAFE_DOWNCAST \
    NEX_PREPEND_NAMESPACE(safeDowncast)

NEX_NAMESPACE_END
