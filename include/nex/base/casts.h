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
#include "nex/base/attributes.h"
#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

// ======================================================================================
// Core type-traits for casting and type manipulation
// ======================================================================================

NEX_SUBNAMESPACE_BEGIN(type_traits)

/**
 * @note
 * Below are our own implementations of some common type traits, which are designed to be used in our casting 
 * utilities and other type manipulation contexts. These traits provide a minimal subset of functionality similar 
 * to the standard `<type_traits>` header, allowing us to perform compile-time type checks and manipulations without 
 * including the full standard library header.
 */

// IntegralConstant is a template that represents a compile-time constant value of a specified type. 
// It is the basis for other type traits like BoolConstant, TrueType, and FalseType.
template <class Type, Type Value>
struct IntegralConstant {
    static constexpr Type value = Value;

    using value_type = Type;
    using type = IntegralConstant;
    
    constexpr operator value_type() const noexcept {
        return value;
    }
    NEX_NODISCARD constexpr value_type operator()() const noexcept {
        return value;
    }
};

// BoolConstant is a specialization of IntegralConstant for boolean values, 
// providing a convenient way to represent compile-time boolean constants.
template <bool Value>
using BoolConstant = IntegralConstant<bool, Value>;

// Represents the compile-time constant value `true`.
using TrueType  = BoolConstant<true>;

// Represents the compile-time constant value `false`.
using FalseType = BoolConstant<false>;

// Maps a sequence of any types to the type void 
template <class... Types>
using Void = void;

// Add top-level const qualifier to a type
template <class Type>
struct AddConst {
    using type = const Type;
};

// Add top-level const qualifier to a type
template <class Type>
using AddConstT = typename AddConst<Type>::type;

// Add top-level volatile qualifier to a type
template <class Type>
struct AddVolatile {
    using type = volatile Type;
};

// Add top-level volatile qualifier to a type
template <class Type>
using AddVolatileT = typename AddVolatile<Type>::type;

// Add top-level const and volatile qualifiers to a type
template <class Type>
struct AddCv {
    using type = const volatile Type;
};

// Add top-level const and volatile qualifiers to a type
template <class Type>
using AddCvT = typename AddCv<Type>::type;

// Add reference to a type (non-referenceable type)
template <class Type, class = void>
struct _AddReference {
    using _Lvalue = Type;
    using _Rvalue = Type;
};

// Add reference to a type (referenceable type)
template <class Type>
struct _AddReference<Type, Void<Type&>> {
    using _Lvalue = Type&;
    using _Rvalue = Type&&;
};

// Add an lvalue reference to a type
template <class Type>
struct AddLvalueReference {
    using type = typename _AddReference<Type>::_Lvalue;
};

// Add an lvalue reference to a type
template <class Type>
using AddLvalueReferenceT = typename _AddReference<Type>::_Lvalue;

// Add an rvalue reference to a type
template <class Type>
struct AddRvalueReference {
    using type = typename _AddReference<Type>::_Rvalue;
};

// Add an rvalue reference to a type
template <class Type>
using AddRvalueReferenceT = typename _AddReference<Type>::_Rvalue;

// A utility function that can be used in unevaluated contexts 
// to obtain a value of a specified type without needing to construct it.
template <class Type>
AddRvalueReferenceT<Type> declval() noexcept;

// Determine whether type argument is an lvalue reference
template <class>
constexpr bool IsLvalueReferenceV = false;

// Specialization of IsLvalueReferenceV for lvalue references
template <class Type>
constexpr bool IsLvalueReferenceV<Type&> = true;

// Determine whether type argument is an rvalue reference
template <class Type>
struct IsLvalueReference : BoolConstant<IsLvalueReferenceV<Type>> {};

// Determine whether type argument is an rvalue reference
template <class>
constexpr bool IsRvalueReferenceV = false;

// Specialization of IsRvalueReferenceV for rvalue references
template <class Type>
constexpr bool IsRvalueReferenceV<Type&&> = true;

// Determine whether type argument is an rvalue reference
template <class Type>
struct IsRvalueReference : BoolConstant<IsRvalueReferenceV<Type>> {};

// Determine whether type argument is a reference (either lvalue or rvalue)
template <class>
constexpr bool IsReferenceV = false;

// Specialization of IsReferenceV for lvalue references
template <class Type>
constexpr bool IsReferenceV<Type&> = true;

// Specialization of IsReferenceV for rvalue references
template <class Type>
constexpr bool IsReferenceV<Type&&> = true;

// Check if a type is a reference (either lvalue or rvalue)
template <class Type>
struct IsReference : BoolConstant<IsReferenceV<Type>> {};

// Determine whether Type can be direct-initialized with Args...
template <class Type, class... Args>
struct IsConstructible : BoolConstant<__is_constructible(Type, Args...)> {};

// Determine whether Type can be direct-initialized with Args...
template <class Type, class... Args>
constexpr bool IsConstructibleV = __is_constructible(Type, Args...);

// Determine whether Type can be direct-initialized with an lvalue const Type
template <class Type>
struct IsCopyConstructible : BoolConstant<__is_constructible(Type, AddLvalueReferenceT<const Type>)> {};

// Determine whether Type can be direct-initialized with an lvalue const Type
template <class Type>
constexpr bool IsCopyConstructibleV = __is_constructible(Type, AddLvalueReferenceT<const Type>);

// Determine whether Type can be value-initialized
template <class Type>
struct IsDefaultConstructible : BoolConstant<__is_constructible(Type)> {};

// Determine whether Type can be default constructed
template <class Type>
constexpr bool IsDefaultConstructibleV = __is_constructible(Type);

// Determine whether Type can be copy-initialized with {}
template <class Type, class = void>
struct _IsImplicitlyDefaultConstructible : FalseType {};

// Helper function to test whether Type can be copy-initialized with {}
template <class Type>
void _ImplicitlyDefaultConstruct(const Type&);

// Specialization of _IsImplicitlyDefaultConstructible for types that can be copy-initialized with {}
template <class Type>
struct _IsImplicitlyDefaultConstructible<Type, Void<decltype(_ImplicitlyDefaultConstruct<Type>({}))>> : TrueType {};

// Determine whether Type can be direct-initialized from an rvalue Type
template <class Type>
struct IsMoveConstructible : BoolConstant<__is_constructible(Type, Type)> {};

// Determine whether Type can be direct-initialized from an rvalue Type
template <class Type>
constexpr bool IsMoveConstructibleV = __is_constructible(Type, Type);

// Determine whether From can be assigned to To
template <class To, class From>
struct IsAssignable : BoolConstant<__is_assignable(To, From)> {};

// Determine whether From can be assigned to To
template <class To, class From>
constexpr bool IsAssignableV = __is_assignable(To, From);

/** 
 * Determine whether From can be assigned to To without checking preconditions
 * (e.g., whether the assignment operator is deleted or inaccessible)
 * This can be used in unevaluated contexts where we only care about the presence of an assignment operator,
 * and not whether it is actually usable.
 */
#if defined(NEX_IS_ASSIGNABLE_NOCHECK_SUPPORTED) && !defined(__CUDACC__)
    template <class To, class From>
    struct _IsAssignableNoPreconditionCheck 
        : BoolConstant<__is_assignable_no_precondition_check(To, From)> {};
#else // Use standard type trait as fallback when intrinsic is not supported
    template <class To, class From>
    using _IsAssignableNoPreconditionCheck = IsAssignable<To, From>;
#endif // intrinsic support

// Determine whether an lvalue const Type can be assigned to an lvalue Type
template <class Type>
struct IsCopyAssignable
    : BoolConstant<__is_assignable(AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>)> {};

// Determine whether Type can be copy assigned
template <class Type>
constexpr bool IsCopyAssignableV = __is_assignable(AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>);

// Determine whether an rvalue Type can be assigned to an lvalue Type without checking preconditions
#if defined(NEX_IS_ASSIGNABLE_NOCHECK_SUPPORTED) && !defined(__CUDACC__)
    template <class Type>
    struct _IsCopyAssignableNoPreconditionCheck
        : BoolConstant<__is_assignable_no_precondition_check(
            AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>)> {};

    template <class Type>
    constexpr bool _IsCopyAssignableUncheckedV =
        __is_assignable_no_precondition_check(AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>);
#else // Use standard type trait as fallback when intrinsic is not supported
    template <class Type>
    using _IsCopyAssignableNoPreconditionCheck = IsCopyAssignable<Type>;

    template <class Type>
    constexpr bool _IsCopyAssignableUncheckedV = IsCopyAssignableV<Type>;
#endif // intrinsic support

// Determine whether an rvalue Type can be assigned to an lvalue Type
template <class Type>
struct IsMoveAssignable : BoolConstant<__is_assignable(AddLvalueReferenceT<Type>, Type)> {};

template <class Type>
constexpr bool IsMoveAssignableV = __is_assignable(AddLvalueReferenceT<Type>, Type);

#if defined(NEX_IS_ASSIGNABLE_NOCHECK_SUPPORTED) && !defined(__CUDACC__)
    template <class Type>
    struct _IsMoveAssignableNoPreconditionCheck
        : BoolConstant<__is_assignable_no_precondition_check(AddLvalueReferenceT<Type>, Type)> {};

    template <class Type>
    constexpr bool _IsMoveAssignableUncheckedV =
        __is_assignable_no_precondition_check(AddLvalueReferenceT<Type>, Type);
#else // Use standard type trait as fallback when intrinsic is not supported
    template <class Type>
    using _IsMoveAssignableNoPreconditionCheck = IsMoveAssignable<Type>;

    template <class Type>
    constexpr bool _IsMoveAssignableUncheckedV = IsMoveAssignableV<Type>;
#endif // intrinsic support

// True if RemoveAllExtentsT<Type> is a reference type, or can be explicitly destroyed
template <class Type>
struct IsDestructible : BoolConstant<__is_destructible(Type)> {};

// True if RemoveAllExtentsT<Type> is a reference type, or can be explicitly destroyed
template <class Type>
constexpr bool IsDestructibleV = __is_destructible(Type);

// Determine whether direct-initialization of Type with Args... is trivial
template <class Type, class... Args>
struct IsTriviallyConstructible : BoolConstant<__is_trivially_constructible(Type, Args...)> {};

// Determine whether direct-initialization of Type with Args... is trivial
template <class Type, class... Args>
constexpr bool IsTriviallyConstructibleV = __is_trivially_constructible(Type, Args...);

// Determine whether direct-initialization of Type with an lvalue const Type is trivial
template <class Type>
struct IsTriviallyCopyConstructible
    : BoolConstant<__is_trivially_constructible(Type, AddLvalueReferenceT<const Type>)> {};

// Determine whether direct-initialization of Type with an lvalue const Type is trivial
template <class Type>
constexpr bool IsTriviallyCopyConstructibleV = __is_trivially_constructible(Type, AddLvalueReferenceT<const Type>);

// Determine whether value-initialization of Type is trivial
template <class Type>
struct IsTriviallyDefaultConstructible : BoolConstant<__is_trivially_constructible(Type)> {};

// Determine whether value-initialization of Type is trivial
template <class Type>
constexpr bool IsTriviallyDefaultConstructibleV = __is_trivially_constructible(Type);

// Determine whether direct-initialization of Type with an rvalue Type is trivial
template <class Type>
struct IsTriviallyMoveConstructible : BoolConstant<__is_trivially_constructible(Type, Type)> {};

// Determine whether direct-initialization of Type with an rvalue Type is trivial
template <class Type>
constexpr bool IsTriviallyMoveConstructibleV = __is_trivially_constructible(Type, Type);

// Determine whether From can be trivially assigned to To
template <class To, class From>
struct IsTriviallyAssignable : BoolConstant<__is_trivially_assignable(To, From)> {};

// Determine whether From can be trivially assigned to To
template <class To, class From>
constexpr bool IsTriviallyAssignableV = __is_trivially_assignable(To, From);

// Determine whether an lvalue const Type can be trivially assigned to an lvalue Type
template <class Type>
struct IsTriviallyCopyAssignable
    : BoolConstant<__is_trivially_assignable(AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>)> {};

// Determine whether an lvalue const Type can be trivially assigned to an lvalue Type
template <class Type>
constexpr bool IsTriviallyCopyAssignableV =
    __is_trivially_assignable(AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>);

// Determine whether an rvalue Type can be trivially assigned to an lvalue Type
template <class Type>
struct IsTriviallyMoveAssignable : BoolConstant<__is_trivially_assignable(AddLvalueReferenceT<Type>, Type)> {};

// Determine whether an rvalue Type can be trivially assigned to an lvalue Type
template <class Type>
constexpr bool IsTriviallyMoveAssignableV = __is_trivially_assignable(AddLvalueReferenceT<Type>, Type);

// Determine whether remove_all_extents_t<Type> is a reference type or can trivially be explicitly destroyed
template <class Type>
struct IsTriviallyDestructible : BoolConstant<__is_trivially_destructible(Type)> {};

// Determine whether remove_all_extents_t<Type> is a reference type or can trivially be explicitly destroyed
template <class Type>
constexpr bool IsTriviallyDestructibleV = __is_trivially_destructible(Type);

// Determine whether direct-initialization of Type from Args... is both valid and not potentially-throwing
template <class Type, class... Args>
struct IsNothrowConstructible : BoolConstant<__is_nothrow_constructible(Type, Args...)> {};

// Determine whether direct-initialization of Type from Args... is both valid and not potentially-throwing
template <class Type, class... Args>
constexpr bool IsNothrowConstructibleV = __is_nothrow_constructible(Type, Args...);

// Determine whether direct-initialization of Type from an lvalue const Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowCopyConstructible
    : BoolConstant<__is_nothrow_constructible(Type, AddLvalueReferenceT<const Type>)> {};

// Determine whether direct-initialization of Type from an lvalue const Type is both valid and not potentially-throwing
template <class Type>
constexpr bool IsNothrowCopyConstructibleV = __is_nothrow_constructible(Type, AddLvalueReferenceT<const Type>);

// Determine whether value-initialization of Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowDefaultConstructible : BoolConstant<__is_nothrow_constructible(Type)> {};

// Determine whether value-initialization of Type is both valid and not potentially-throwing
template <class Type>
constexpr bool IsNothrowDefaultConstructibleV = __is_nothrow_constructible(Type);

// Determine whether direct-initialization of Type from an rvalue Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowMoveConstructible : BoolConstant<__is_nothrow_constructible(Type, Type)> {};

// Determine whether direct-initialization of Type from an rvalue Type is both valid and not potentially-throwing
template <class Type>
constexpr bool IsNothrowMoveConstructibleV = __is_nothrow_constructible(Type, Type);

// Determine whether assignment of From to To is both valid and not potentially-throwing
template <class To, class From>
struct IsNothrowAssignable : BoolConstant<__is_nothrow_assignable(To, From)> {};

// Determine whether assignment of From to To is both valid and not potentially-throwing
template <class To, class From>
constexpr bool IsNothrowAssignableV = __is_nothrow_assignable(To, From);

// Determine whether assignment of an lvalue const Type to an lvalue Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowCopyAssignable
    : BoolConstant<__is_nothrow_assignable(AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>)> {};

// Determine whether assignment of an lvalue const Type to an lvalue Type is both valid and not potentially-throwing
template <class Type>
constexpr bool IsNothrowCopyAssignableV =
    __is_nothrow_assignable(AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>);

// Determine whether assignment of an rvalue Type to an lvalue Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowMoveAssignable : BoolConstant<__is_nothrow_assignable(AddLvalueReferenceT<Type>, Type)> {};

// Determine whether assignment of an rvalue Type to an lvalue Type is both valid and not potentially-throwing
template <class Type>
constexpr bool IsNothrowMoveAssignableV = __is_nothrow_assignable(AddLvalueReferenceT<Type>, Type);

// Determine whether remove_all_extents_t<Type> is a reference type or has non-potentially-throwing explicit destruction
template <class Type>
struct IsNothrowDestructible : BoolConstant<__is_nothrow_destructible(Type)> {};

// Determine whether remove_all_extents_t<Type> is a reference type or has non-potentially-throwing explicit destruction
template <class Type>
constexpr bool IsNothrowDestructibleV = __is_nothrow_destructible(Type);

// EnableIf implementation for SFINAE
template <bool BoolCond, class Type = void>
struct EnableIf {};             // no member "type" when !BoolCond

// Specialization of EnableIf for when the condition is true
template <class Type>
struct EnableIf<true, Type> {   // type is Type for BoolCond
    using type = Type;
};

// Alias template for EnableIf to simplify usage
template <bool BoolCond, class Type = void>
using EnableIfT = typename EnableIf<BoolCond, Type>::type;

// Choose Type1 if Test is true
template <bool Test, class Type1, class Type2>
struct Conditional {
    using type = Type1;
};

// Choose Type2 otherwise
template <class Type1, class Type2>
struct Conditional<false, Type1, Type2> {
    using type = Type2;
};

// Alias template for Conditional to simplify usage
template <bool Test, class Type1, class Type2>
using ConditionalT = typename Conditional<Test, Type1, Type2>::type;

// RemoveCv implementation to remove top-level const and volatile qualifiers
template <class Type>
struct RemoveCv {
    using type = Type;

    // apply cv-qualifiers from the class template argument to __Fn<Type>
    template <template <class> class Fn>
    using _Apply = Fn<Type>;
};

// Specializations of RemoveCv for const, volatile, and const volatile types
template <class Type>
struct RemoveCv<const Type> {
    using type = Type;
    template <template <class> class Fn>
    using _Apply = const Fn<Type>;
};

// Specialization of RemoveCv for volatile types
template <class Type>
struct RemoveCv<volatile Type> {
    using type = Type;
    template <template <class> class Fn>
    using _Apply = volatile Fn<Type>;
};

// Specialization of RemoveCv for const volatile types
template <class Type>
struct RemoveCv<const volatile Type> {
    using type = Type;
    template <template <class> class Fn>
    using _Apply = const volatile Fn<Type>;
};

// Alias template for RemoveCv to simplify usage
template <class Type>
using RemoveCvT = typename RemoveCv<Type>::type;

#if NEX_COMPILER_IS_CLANG
    // Clang provides a builtin type trait for is_same, 
    // which is more efficient than our implementation, so use it when available
    template <class Type1, class Type2>
    constexpr bool IsSameV = __is_same(Type1, Type2);
#else
    // Check if two types are the same
    template <class, class>
    constexpr bool IsSameV = false;   // determine whether arguments are the same type
    template <class Type>
    constexpr bool IsSameV<Type, Type> = true;
#endif

// Check if a type is any of a list of types
template <class Type, class... Types>
constexpr bool IsAnyOfV =             // true if and only if Type is in Types
#if NEX_HAS_CXX17
    (IsSameV<Type, Types> || ...);
#else // C++14 or earlier
    // disjunction_v is defined in <xtr1common>, 
    // but we do NOT need it here since we only supports C++20 or later
    disjunction_v<is_same<Type, Types>...>;
#endif // NEX_HAS_CXX17

#if NEX_HAS_CXX20
    // Check if we are in a constant evaluation context
    NEX_NODISCARD constexpr bool IsConstantEvaluated() noexcept {
        return __builtin_is_constant_evaluated();
    }
#endif  // NEX_HAS_CXX20

// Check if a type is an integral type
template <class Type>
constexpr bool IsIntegralV = IsAnyOfV<
    RemoveCvT<Type>, bool, 
    char, signed char, unsigned char, wchar_t,
#if defined(__cpp_char8_t)
    char8_t,
#endif // defined(__cpp_char8_t)
    char16_t, char32_t, 
    short, unsigned short, 
    int, unsigned int, 
    long, unsigned long, 
    long long, unsigned long long
>;

// RemoveReference implementation to remove reference qualifiers
template <class Type>
struct RemoveReference {
    using type = Type;
    using _ConstThruRefType = const Type;
};

// Specialization of RemoveReference for lvalue references
template <class Type>
struct RemoveReference<Type&> {
    using type = Type;
    using _ConstThruRefType = const Type&;
};

// Specialization of RemoveReference for rvalue references
template <class Type>
struct RemoveReference<Type&&> {
    using type = Type;
    using _ConstThruRefType = const Type&&;
};

// Alias template for RemoveReference to simplify usage
template <class Type>
using RemoveReferenceT = typename RemoveReference<Type>::type;

// Alias template to get the const-qualified type through reference for a given type
template <class Type>
using _ConstThruRef = typename RemoveReference<Type>::_ConstThruRefType;

// Alias template to remove both const/volatile qualifiers and reference qualifiers from a type
template <class Type>
using _RemoveCvrefT NEX_MSVC_KNOWN_SEMANTICS = RemoveCvT<RemoveReferenceT<Type>>;

#if NEX_HAS_CXX20
    // Alias template for RemoveCvref to simplify usage in C++20
    template <class Type>
    using RemoveCvrefT = _RemoveCvrefT<Type>;

    // RemoveCvref implementation to remove both const/volatile qualifiers and reference qualifiers
    template <class Type>
    struct RemoveCvref {
        using type = RemoveCvrefT<Type>;
    };
#endif  // NEX_HAS_CXX20

NEX_SUBNAMESPACE_END(type_traits)

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

// Forward an lvalue as either an lvalue or an rvalue, 
// depending on the value category of the argument
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Type&& forward(type_traits::RemoveReferenceT<Type>& arg) noexcept {
    return static_cast<Type&&>(arg);
}

// Forward an rvalue as an rvalue
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Type&& forward(type_traits::RemoveReferenceT<Type>&& arg) noexcept {
    static_assert(!type_traits::IsLvalueReferenceV<Type>, "Bad forward call");
    return static_cast<Type&&>(arg);
}

// Move an object to allow it to be moved from, even if it is an lvalue
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
type_traits::RemoveReferenceT<Type>&& move(Type&& arg) noexcept {
    return static_cast<type_traits::RemoveReferenceT<Type>&&>(arg);
}

// Move an object if it is nothrow move constructible, otherwise copy it
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr
type_traits::ConditionalT<!type_traits::IsNothrowMoveConstructibleV<Type> 
    && type_traits::IsCopyConstructibleV<Type>, const Type&, Type&&>
moveIfNoexcept(Type& arg) noexcept {
    return move(arg);
}

// Implementation of addressof that works even if the type has an overloaded operator&.
template <class Type>
NEX_NODISCARD NEX_MSVC_INTRINSIC constexpr 
Type* addressOf(Type& Value) noexcept {
    return __builtin_addressof(Value);
}

// Delete the overload of addressOf for rvalue references to prevent taking the address of a temporary
template <class Type>
const Type* addressOf(const Type&&) = delete;

#if NEX_COMPILER_MSVC_COMPATIBLE
    #pragma warning(pop)
#endif

NEX_NAMESPACE_END
