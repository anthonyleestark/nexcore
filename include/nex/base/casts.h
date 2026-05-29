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
#include "nex/base/assert_crash.h"

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

// EnableIf implementation for SFINAE
template <bool BoolCond, class Type = void>
struct EnableIf {};     // no member "type" when !BoolCond

template <class Type>
struct EnableIf<true, Type> {   // type is Type for BoolCond
    using type = Type;
};

template <bool BoolCond, class Type = void>
using EnableIfT = typename EnableIf<BoolCond, Type>::type;

// Conditional implementation to select one of two types based on a boolean condition
template <bool Test, class Type1, class Type2>
struct Conditional {
    using type = Type1;
};

template <class Type1, class Type2>
struct Conditional<false, Type1, Type2> {
    using type = Type2;
};

template <bool Test, class Type1, class Type2>
using ConditionalT = typename Conditional<Test, Type1, Type2>::type;

#if NEX_COMPILER_IS_CLANG
    // Clang provides a builtin type trait for is_same, 
    // which is more efficient than our implementation, so use it when available
    template <class Type1, class Type2>
    constexpr bool IsSameV = __is_same(Type1, Type2);
#else
    // Determine whether arguments are the same type
    template <class, class>
    constexpr bool IsSameV = false;
    template <class Type>
    constexpr bool IsSameV<Type, Type> = true;
#endif

// Determine whether arguments are the same type
template <class Type1, class Type2>
struct IsSame : BoolConstant<IsSameV<Type1, Type2>> {};

// RemoveConst implementation to remove top-level const qualifier from a type
template <class Type>
struct RemoveConst {
    using type = Type;
};

template <class Type>
struct RemoveConst<const Type> {
    using type = Type;
};

template <class Type>
using RemoveConstT = typename RemoveConst<Type>::type;

// RemoveVolatile implementation to remove top-level volatile qualifier from a type
template <class Type>
struct RemoveVolatile {
    using type = Type;
};

template <class Type>
struct RemoveVolatile<volatile Type> {
    using type = Type;
};

template <class Type>
using RemoveVolatileT = typename RemoveVolatile<Type>::type;

// RemoveCv implementation to remove top-level const and volatile qualifiers
template <class Type>
struct RemoveCv {
    using type = Type;

    // apply cv-qualifiers from the class template argument to __Fn<Type>
    template <template <class> class Fn>
    using _Apply = Fn<Type>;
};

template <class Type>
struct RemoveCv<const Type> {
    using type = Type;
    template <template <class> class Fn>
    using _Apply = const Fn<Type>;
};

template <class Type>
struct RemoveCv<volatile Type> {
    using type = Type;
    template <template <class> class Fn>
    using _Apply = volatile Fn<Type>;
};

template <class Type>
struct RemoveCv<const volatile Type> {
    using type = Type;
    template <template <class> class Fn>
    using _Apply = const volatile Fn<Type>;
};

template <class Type>
using RemoveCvT = typename RemoveCv<Type>::type;

// Determine whether a type is any of a list of types
template <class Type, class... Types>
constexpr bool IsAnyOfV = (IsSameV<Type, Types> || ...);

#if NEX_HAS_CXX20
    // Check if we are in a constant evaluation context
    NEX_NODISCARD constexpr bool IsConstantEvaluated() noexcept {
        return __builtin_is_constant_evaluated();
    }
#endif  // NEX_HAS_CXX20

// Determine whether a type is an integral type
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

// Determine whether a type is a floating-point type
template <class Type>
constexpr bool IsFloatingPointV = IsAnyOfV<RemoveCvT<Type>, float, double, long double>;

// Determine whether integral type Type is signed or unsigned
template <class Type, bool = IsIntegralV<Type>>
struct _SignCheckBase {
    using _Underlying = RemoveCvT<Type>;

    static constexpr bool _Signed   = static_cast<_Underlying>(-1) < static_cast<_Underlying>(0);
    static constexpr bool _Unsigned = !_Signed;
};

// Specialization of _SignCheckBase for non-integral types
template <class Type>
struct _SignCheckBase<Type, false> {
    static constexpr bool _Signed   = IsFloatingPointV<Type>;   // floating-point Type is signed
    static constexpr bool _Unsigned = false;                    // non-arithmetic Type is neither signed nor unsigned
};

// Determine whether an integral type is signed
template <class Type>
struct IsSignedIntegral : BoolConstant<_SignCheckBase<Type>::_Signed> {};

template <class Type>
constexpr bool IsSignedIntegralV = IsSignedIntegral<Type>::value;

// Determine whether an integral type is unsigned
template <class Type>
struct IsUnsignedIntegral : BoolConstant<_SignCheckBase<Type>::_Unsigned> {};

template <class Type>
constexpr bool IsUnsignedIntegralV = IsUnsignedIntegral<Type>::value;

// Determine whether a type is an arithmetic type (either integral or floating-point)
template <class Type>
constexpr bool _IsArithmeticV = IsIntegralV<Type> || IsFloatingPointV<Type>;

template <class Type>
struct IsArithmetic : BoolConstant<_IsArithmeticV<Type>> {};

template <class Type>
constexpr bool IsArithmeticV = IsArithmetic<Type>::value;

// Determine whether a type is the void type
template <class Type>
constexpr bool IsVoidV = IsSameV<RemoveCvT<Type>, void>;

template <class Type>
struct IsVoid : BoolConstant<IsVoidV<Type>> {};

// Determine whether a type is a raw pointer
template <class>
constexpr bool _IsPointerV = false;

// Specialization of _IsPointerV for pointer types, 
// which checks if the type is a pointer by checking if it is of the form Type*
template <class Type>
constexpr bool _IsPointerV<Type*> = true;

// Determine whether a type is a raw pointer
template <class Type>
constexpr bool IsPointerV = _IsPointerV<RemoveCvT<Type>>;

// Determine whether a cv-qualified type is a null-pointer type
template <class Type>
constexpr bool IsNullPointerV = IsSameV<RemoveCvT<Type>, decltype(nullptr)>;

template <class Type>
struct IsNullPointer : BoolConstant<IsNullPointerV<Type>> {};

// Determine whether a type is an enumeration type
template <class Type>
struct IsEnum : BoolConstant<__is_enum(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsEnumV = IsEnum<Type>::value;

// Determine whether a type is a class type
template <class Type>
struct IsClass : BoolConstant<__is_class(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsClassV = IsClass<Type>::value;

// Determine whether From is convertible to To
template <class From, class To>
struct IsConvertible : BoolConstant<__is_convertible_to(From, To)> {};

template <class From, class To>
constexpr bool IsConvertibleV = IsConvertible<From, To>::value;

// RemoveReference implementation to remove reference qualifiers
template <class Type>
struct RemoveReference {
    using type = Type;
    using _ConstThruRefType = const Type;
};

template <class Type>
struct RemoveReference<Type&> {
    using type = Type;
    using _ConstThruRefType = const Type&;
};

template <class Type>
struct RemoveReference<Type&&> {
    using type = Type;
    using _ConstThruRefType = const Type&&;
};

template <class Type>
using RemoveReferenceT = typename RemoveReference<Type>::type;

template <class Type>
using _ConstThruRef = typename RemoveReference<Type>::_ConstThruRefType;

template <class Type>
using _RemoveCvrefT NEX_MSVC_KNOWN_SEMANTICS = RemoveCvT<RemoveReferenceT<Type>>;

#if NEX_HAS_CXX20
    // Remove reference and cv-qualifiers from a type
    template <class Type>
    using RemoveCvrefT = _RemoveCvrefT<Type>;

    template <class Type>
    struct RemoveCvref {
        using type = RemoveCvrefT<Type>;
    };
#endif  // NEX_HAS_CXX20

// Maps a sequence of any types to the type void 
template <class... Types>
using VoidT = void;

// Add top-level const qualifier to a type
template <class Type>
struct AddConst {
    using type = const Type;
};

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
struct _AddReference<Type, VoidT<Type&>> {
    using _Lvalue = Type&;
    using _Rvalue = Type&&;
};

// Add an lvalue reference to a type
template <class Type>
struct AddLvalueReference {
    using type = typename _AddReference<Type>::_Lvalue;
};

template <class Type>
using AddLvalueReferenceT = typename _AddReference<Type>::_Lvalue;

// Add an rvalue reference to a type
template <class Type>
struct AddRvalueReference {
    using type = typename _AddReference<Type>::_Rvalue;
};

template <class Type>
using AddRvalueReferenceT = typename _AddReference<Type>::_Rvalue;

// A utility function that can be used in unevaluated contexts 
// to obtain a value of a specified type without needing to construct it.
template <class Type>
AddRvalueReferenceT<Type> declval() noexcept;

// Determine whether type argument is an lvalue reference
template <class>
constexpr bool IsLvalueReferenceV = false;

template <class Type>
constexpr bool IsLvalueReferenceV<Type&> = true;

// Determine whether type argument is an rvalue reference
template <class Type>
struct IsLvalueReference : BoolConstant<IsLvalueReferenceV<Type>> {};

// Determine whether type argument is an rvalue reference
template <class>
constexpr bool IsRvalueReferenceV = false;

template <class Type>
constexpr bool IsRvalueReferenceV<Type&&> = true;

// Determine whether type argument is an rvalue reference
template <class Type>
struct IsRvalueReference : BoolConstant<IsRvalueReferenceV<Type>> {};

// Determine whether type argument is a reference (either lvalue or rvalue)
template <class>
constexpr bool IsReferenceV = false;

template <class Type>
constexpr bool IsReferenceV<Type&> = true;

template <class Type>
constexpr bool IsReferenceV<Type&&> = true;

// Check if a type is a reference (either lvalue or rvalue)
template <class Type>
struct IsReference : BoolConstant<IsReferenceV<Type>> {};

// Determine whether Type is a trivial type
template <class Type>
struct IsTrivial : BoolConstant<__is_trivial(Type)> {};

template <class Type>
constexpr bool IsTrivialV = IsTrivial<Type>::value;

// Determine whether Type is a trivially copyable type
template <class Type>
struct IsTriviallyCopyable : BoolConstant<__is_trivially_copyable(Type)> {};

template <class Type>
constexpr bool IsTriviallyCopyableV = IsTriviallyCopyable<Type>::value;

// Determine whether Type can be direct-initialized with Args...
template <class Type, class... Args>
struct IsConstructible : BoolConstant<__is_constructible(Type, Args...)> {};

template <class Type, class... Args>
constexpr bool IsConstructibleV = IsConstructible<Type, Args...>::value;

// Determine whether Type can be direct-initialized with an lvalue const Type
template <class Type>
struct IsCopyConstructible : BoolConstant<__is_constructible(Type, AddLvalueReferenceT<const Type>)> {};

template <class Type>
constexpr bool IsCopyConstructibleV = IsCopyConstructible<Type>::value;

// Determine whether Type can be value-initialized
template <class Type>
struct IsDefaultConstructible : BoolConstant<__is_constructible(Type)> {};

template <class Type>
constexpr bool IsDefaultConstructibleV = IsDefaultConstructible<Type>::value;

// Determine whether Type can be copy-initialized with {}
template <class Type, class = void>
struct _IsImplicitlyDefaultConstructible : FalseType {};

// Helper function to test whether Type can be copy-initialized with {}
template <class Type>
void _ImplicitlyDefaultConstruct(const Type&);

template <class Type>
struct _IsImplicitlyDefaultConstructible<Type, VoidT<decltype(_ImplicitlyDefaultConstruct<Type>({}))>> : TrueType {};

// Determine whether Type can be direct-initialized from an rvalue Type
template <class Type>
struct IsMoveConstructible : BoolConstant<__is_constructible(Type, Type)> {};

template <class Type>
constexpr bool IsMoveConstructibleV = IsMoveConstructible<Type>::value;

// Determine whether From can be assigned to To
template <class To, class From>
struct IsAssignable : BoolConstant<__is_assignable(To, From)> {};

template <class To, class From>
constexpr bool IsAssignableV = IsAssignable<To, From>::value;

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

template <class Type>
constexpr bool IsCopyAssignableV = IsCopyAssignable<Type>::value;

// Determine whether an rvalue Type can be assigned to an lvalue Type without checking preconditions
#if defined(NEX_IS_ASSIGNABLE_NOCHECK_SUPPORTED) && !defined(__CUDACC__)
    template <class Type>
    struct _IsCopyAssignableNoPreconditionCheck
        : BoolConstant<__is_assignable_no_precondition_check(
            AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>)> {};

    template <class Type>
    constexpr bool _IsCopyAssignableUncheckedV = _IsCopyAssignableNoPreconditionCheck<Type>::value;
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
constexpr bool IsMoveAssignableV = IsMoveAssignable<Type>::value;

#if defined(NEX_IS_ASSIGNABLE_NOCHECK_SUPPORTED) && !defined(__CUDACC__)
    template <class Type>
    struct _IsMoveAssignableNoPreconditionCheck
        : BoolConstant<__is_assignable_no_precondition_check(AddLvalueReferenceT<Type>, Type)> {};

    template <class Type>
    constexpr bool _IsMoveAssignableUncheckedV = _IsMoveAssignableNoPreconditionCheck<Type>::value;
#else // Use standard type trait as fallback when intrinsic is not supported
    template <class Type>
    using _IsMoveAssignableNoPreconditionCheck = IsMoveAssignable<Type>;

    template <class Type>
    constexpr bool _IsMoveAssignableUncheckedV = IsMoveAssignableV<Type>;
#endif // intrinsic support

// True if RemoveAllExtentsT<Type> is a reference type, or can be explicitly destroyed
template <class Type>
struct IsDestructible : BoolConstant<__is_destructible(Type)> {};

template <class Type>
constexpr bool IsDestructibleV = IsDestructible<Type>::value;

// Determine whether direct-initialization of Type with Args... is trivial
template <class Type, class... Args>
struct IsTriviallyConstructible : BoolConstant<__is_trivially_constructible(Type, Args...)> {};

template <class Type, class... Args>
constexpr bool IsTriviallyConstructibleV = IsTriviallyConstructible<Type, Args...>::value;

// Determine whether direct-initialization of Type with an lvalue const Type is trivial
template <class Type>
struct IsTriviallyCopyConstructible
    : BoolConstant<__is_trivially_constructible(Type, AddLvalueReferenceT<const Type>)> {};

template <class Type>
constexpr bool IsTriviallyCopyConstructibleV = IsTriviallyCopyConstructible<Type>::value;

// Determine whether value-initialization of Type is trivial
template <class Type>
struct IsTriviallyDefaultConstructible : BoolConstant<__is_trivially_constructible(Type)> {};

template <class Type>
constexpr bool IsTriviallyDefaultConstructibleV = IsTriviallyDefaultConstructible<Type>::value;

// Determine whether direct-initialization of Type with an rvalue Type is trivial
template <class Type>
struct IsTriviallyMoveConstructible : BoolConstant<__is_trivially_constructible(Type, Type)> {};

template <class Type>
constexpr bool IsTriviallyMoveConstructibleV = IsTriviallyMoveConstructible<Type>::value;

// Determine whether From can be trivially assigned to To
template <class To, class From>
struct IsTriviallyAssignable : BoolConstant<__is_trivially_assignable(To, From)> {};

template <class To, class From>
constexpr bool IsTriviallyAssignableV = IsTriviallyAssignable<To, From>::value;

// Determine whether an lvalue const Type can be trivially assigned to an lvalue Type
template <class Type>
struct IsTriviallyCopyAssignable
    : BoolConstant<__is_trivially_assignable(AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>)> {};

template <class Type>
constexpr bool IsTriviallyCopyAssignableV = IsTriviallyCopyAssignable<Type>::value;

// Determine whether an rvalue Type can be trivially assigned to an lvalue Type
template <class Type>
struct IsTriviallyMoveAssignable : BoolConstant<__is_trivially_assignable(AddLvalueReferenceT<Type>, Type)> {};

template <class Type>
constexpr bool IsTriviallyMoveAssignableV = IsTriviallyMoveAssignable<Type>::value;

// Determine whether remove_all_extents_t<Type> is a reference type or can trivially be explicitly destroyed
template <class Type>
struct IsTriviallyDestructible : BoolConstant<__is_trivially_destructible(Type)> {};

template <class Type>
constexpr bool IsTriviallyDestructibleV = IsTriviallyDestructible<Type>::value;

// Determine whether direct-initialization of Type from Args... is both valid and not potentially-throwing
template <class Type, class... Args>
struct IsNothrowConstructible : BoolConstant<__is_nothrow_constructible(Type, Args...)> {};

template <class Type, class... Args>
constexpr bool IsNothrowConstructibleV = IsNothrowConstructible<Type, Args...>::value;

// Determine whether direct-initialization of Type from an lvalue const Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowCopyConstructible
    : BoolConstant<__is_nothrow_constructible(Type, AddLvalueReferenceT<const Type>)> {};

template <class Type>
constexpr bool IsNothrowCopyConstructibleV = IsNothrowCopyConstructible<Type>::value;

// Determine whether value-initialization of Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowDefaultConstructible : BoolConstant<__is_nothrow_constructible(Type)> {};

template <class Type>
constexpr bool IsNothrowDefaultConstructibleV = IsNothrowDefaultConstructible<Type>::value;

// Determine whether direct-initialization of Type from an rvalue Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowMoveConstructible : BoolConstant<__is_nothrow_constructible(Type, Type)> {};

template <class Type>
constexpr bool IsNothrowMoveConstructibleV = IsNothrowMoveConstructible<Type>::value;

// Determine whether assignment of From to To is both valid and not potentially-throwing
template <class To, class From>
struct IsNothrowAssignable : BoolConstant<__is_nothrow_assignable(To, From)> {};

template <class To, class From>
constexpr bool IsNothrowAssignableV = IsNothrowAssignable<To, From>::value;

// Determine whether assignment of an lvalue const Type to an lvalue Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowCopyAssignable
    : BoolConstant<__is_nothrow_assignable(AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>)> {};

template <class Type>
constexpr bool IsNothrowCopyAssignableV = IsNothrowCopyAssignable<Type>::value;

// Determine whether assignment of an rvalue Type to an lvalue Type is both valid and not potentially-throwing
template <class Type>
struct IsNothrowMoveAssignable : BoolConstant<__is_nothrow_assignable(AddLvalueReferenceT<Type>, Type)> {};

template <class Type>
constexpr bool IsNothrowMoveAssignableV = IsNothrowMoveAssignable<Type>::value;

// Determine whether remove_all_extents_t<Type> is a reference type or has non-potentially-throwing explicit destruction
template <class Type>
struct IsNothrowDestructible : BoolConstant<__is_nothrow_destructible(Type)> {};

template <class Type>
constexpr bool IsNothrowDestructibleV = IsNothrowDestructible<Type>::value;

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

NEX_NAMESPACE_END
