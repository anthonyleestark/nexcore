/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file    meta.h
 * @brief   Compiler-intrinsics type traits and concepts for compile-time type manipulation.
 * 
 * @details
 * This header defines a collection of type traits and concepts that are implemented using compiler intrinsics 
 * where available, providing efficient and accurate type information for compile-time type manipulation.
 * These traits serves as the foundation for more complex type traits and utilities defined in other headers, 
 * enabling advanced compile-time optimization and metaprogramming techniques for the Nex-ecosystem.
 */

#include "nex/base/compiler.h"
#include "nex/base/attributes.h"
#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(meta)

// =================================================================================
// Compiler intrinsic type traits implementations
// =================================================================================

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

// Determine whether a type is a specialization of a class template
template <class Type, template <class...> class Template>
constexpr bool IsSpecializationV = false;

template <template <class...> class Template, class... Types>
constexpr bool IsSpecializationV<Template<Types...>, Template> = true;

template <class Type, template <class...> class Template>
struct IsSpecialization : BoolConstant<IsSpecializationV<Type, Template>> {};

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

// Check if we are in a constant evaluation context
NEX_NODISCARD constexpr bool IsConstantEvaluated() noexcept {
    return __builtin_is_constant_evaluated();
}

#if NEX_HAS_CXX23
    // C++23 introduces the consteval specifier, 
    // which enforces that a function is evaluated at compile time.
    #define NEX_CONSTEVAL_CONTEXT consteval
#else
    // For C++20, we can use the IsConstantEvaluated() function 
    // to check if we are in a constant evaluation context.
    #define NEX_CONSTEVAL_CONTEXT (meta::IsConstantEvaluated())
#endif  // ^^NEX_CONSTEVAL_CONTEXT

#if NEX_HAS_BUILTIN(__is_integral)
    // Determine whether a type is an integral type
    template <class Type>
    constexpr bool IsIntegralV = __is_integral(RemoveCvT<Type>);
#else
    // Determine whether a type is an integral type
    template <class Type>
    constexpr bool IsIntegralV = IsAnyOfV<RemoveCvT<Type>, 
        bool,                               // Boolean type
        char, signed char, unsigned char,   // Narrow character types
        wchar_t,                            // Wide character type (platform-dependent)
    #if defined(__cpp_char8_t)
        char8_t,                            // UTF-8 code unit type (since C++20)
    #endif // ^^defined(__cpp_char8_t)
        char16_t, char32_t,                 // UTF-16 and UTF-32 code unit types (since C++11)
        short, unsigned short,              // 16-bit integer types
        int, unsigned int,                  // 32-bit integer types
        long, unsigned long,                // Platform-dependent integer types (32-bit or 64-bit)
        long long, unsigned long long       // 64-bit integer types
    #if NEX_HAS_BUILTIN_INT128
        , __int128, unsigned __int128       // 128-bit integer types (if supported by the compiler)
    #endif  // ^^NEX_HAS_BUILTIN_INT128
    >;
#endif

#if NEX_HAS_BUILTIN(__is_floating_point)
    // Determine whether a type is a floating-point type
    template <class Type>
    constexpr bool IsFloatingPointV = __is_floating_point(RemoveCvT<Type>);
#else
    // Determine whether a type is a floating-point type
    template <class Type>
    constexpr bool IsFloatingPointV = IsAnyOfV<RemoveCvT<Type>, 
    #if NEX_HAS_BUILTIN_FLOAT16
        __float16_t,                        // 16-bit half-precision IEEE 754 (binary16)
    #endif  // ^^NEX_HAS_BUILTIN_FLOAT16
        float,                              // 32-bit single-precision IEEE 754 (binary32)
        double,                             // 64-bit double-precision IEEE 754 (binary64)
        long double                         // Extended-precision IEEE 754 (platform-dependent width)
    #if NEX_HAS_BUILTIN_FLOAT128
        , __float128                        // 128-bit quadruple-precision IEEE 754 (binary128)
    #endif  // ^^NEX_HAS_BUILTIN_FLOAT128
    >;
#endif

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

// Determine whether type argument is const qualified
template <class>
constexpr bool IsConstV = false;

template <class Type>
constexpr bool IsConstV<const Type> = true;

template <class Type>
struct IsConst : BoolConstant<IsConstV<Type>> {};

// Determine whether type argument is volatile qualified
template <class>
constexpr bool IsVolatileV = false;

template <class Type>
constexpr bool IsVolatileV<volatile Type> = true;

template <class Type>
struct IsVolatile : BoolConstant<IsVolatileV<Type>> {};

// Determine whether a type is an array type
template <class>
constexpr bool IsArrayV = false;

// Determine whether a type is a bounded array type (i.e., an array with a known size)
template <class Type>
constexpr bool IsBoundedArrayV = false;

// Determine whether a type is an unbounded array type (i.e., an array with an unknown size)
template <class Type>
constexpr bool IsUnboundedArrayV = false;

// Specialization of IsBoundedArrayV for bounded array types, which checks if the type is of the form Type[Size]
template <class Type, unsigned long long Size>
constexpr bool IsBoundedArrayV<Type[Size]> = true;

// Specialization of IsUnboundedArrayV for unbounded array types, which checks if the type is of the form Type[]
template <class Type>
constexpr bool IsUnboundedArrayV<Type[]> = true;

template <class Type>
struct IsArray : BoolConstant<IsBoundedArrayV<Type> || IsUnboundedArrayV<Type>> {};

template <class Type>
struct IsBoundedArray : BoolConstant<IsBoundedArrayV<Type>> {};

template <class Type>
struct IsUnboundedArray : BoolConstant<IsUnboundedArrayV<Type>> {};

// Determine whether a type is an enumeration type
template <class Type>
struct IsEnum : BoolConstant<__is_enum(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsEnumV = IsEnum<Type>::value;

// Determine whether an enumeration type is a scoped enum (i.e., an enum class)
template <class Type>
struct IsEnumClass : BoolConstant<IsEnumV<Type> && !IsConvertibleV<Type, int>> {};

template <class Type>
constexpr bool IsEnumClassV = IsEnumClass<Type>::value;

// Determine whether a type is a union type
template <class Type>
struct IsUnion : BoolConstant<__is_union(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsUnionV = IsUnion<Type>::value;

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

// Determine whether a class is empty 
// (i.e., a class type with no non-static data members, no virtual functions, and no virtual base classes)
template <class Type>
struct IsEmpty : BoolConstant<__is_empty(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsEmptyV = IsEmpty<Type>::value;

// Determine whether a class is polymorphic (i.e., a class that has at least one virtual function)
template <class Type>
struct IsPolymorphic : BoolConstant<__is_polymorphic(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsPolymorphicV = IsPolymorphic<Type>::value;

// Determine whether a class is abstract 
// (i.e., a class that cannot be instantiated and has at least one pure virtual function)
template <class Type>
struct IsAbstract : BoolConstant<__is_abstract(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsAbstractV = IsAbstract<Type>::value;

// Determine whether a class is final (i.e., a class that cannot be inherited from)
template <class Type>
struct IsFinal : BoolConstant<__is_final(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsFinalV = IsFinal<Type>::value;

// Determine whether Type is a standard-layout type
template <class Type>
struct IsStandardLayout : BoolConstant<__is_standard_layout(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsStandardLayoutV = IsStandardLayout<Type>::value;

// Determine whether Base is a base of or the same as Derived
template <class Base, class Derived>
struct IsBaseOf : BoolConstant<__is_base_of(Base, Derived)> {};

template <class Base, class Derived>
constexpr bool IsBaseOfV = IsBaseOf<Base, Derived>::value;

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

// Remove reference and cv-qualifiers from a type
template <class Type>
using RemoveCvrefT = _RemoveCvrefT<Type>;

template <class Type>
struct RemoveCvref {
    using type = RemoveCvrefT<Type>;
};

// Retrieve the underlying type of an enumeration type
template <class Type, bool = IsEnumV<Type>>
struct UnderlyingType {
    using type = __underlying_type(Type);
};

template <class Type>
struct UnderlyingType<Type, false> {
    using type = void;   // Not an enumeration type, so no underlying type
};

template <class Type>
using UnderlyingTypeT = typename UnderlyingType<Type>::type;

// Retrive the identity of a type, which is the type itself. 
// This can be used in unevaluated contexts to obtain a type without needing to construct it.
template <class Type>
struct TypeIdentity {
    using type = Type;
};

template <class Type>
using TypeIdentityT = typename TypeIdentity<Type>::type;

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
AddRvalueReferenceT<Type> declval() noexcept {
    static_assert(false, "declval not allowed in an evaluated context");
}

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

// Determine whether Type is a function type 
// (i.e., a type that can be called with a function call syntax, excluding reference types and void types)
template <class Type>
constexpr bool IsFunctionV = !IsConstV<const Type> && !IsReferenceV<Type>;

template <class Type>
struct IsFunction : BoolConstant<IsFunctionV<Type>> {};

// Determine whether Type is an object type (i.e., a type that is not a function, reference, or void)
template <class Type>
constexpr bool IsObjectV = IsConstV<const Type> && !IsVoidV<Type>;

template <class Type>
struct IsObject : BoolConstant<IsObjectV<Type>> {};

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

// =================================================================================
// Internal utilities for compile-time logic processing to support metaprogramming
// =================================================================================

// Compute the minimum of two values at compile time
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __minOf(const Type& left NEX_LIFETIMEBOUND, const Type& right NEX_LIFETIMEBOUND)
noexcept(noexcept(left < right)) {
    return left < right ? left : right;
}

// Compute the maximum of two values at compile time
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __maxOf(const Type& left NEX_LIFETIMEBOUND, const Type& right NEX_LIFETIMEBOUND)
noexcept(noexcept(left < right)) {
    return left < right ? right : left;
}

// Compute the minimum of two values at compile time based on a custom predicate
template <class Type, class Predicate>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __minOfIf(const Type& left NEX_LIFETIMEBOUND, const Type& right NEX_LIFETIMEBOUND, Predicate pred)
noexcept(noexcept(pred(left, right))) {
    return pred(left, right) ? left : right;
}

// Compute the maximum of two values at compile time based on a custom predicate
template <class Type, class Predicate>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __maxOfIf(const Type& left NEX_LIFETIMEBOUND, const Type& right NEX_LIFETIMEBOUND, Predicate pred)
noexcept(noexcept(pred(left, right))) {
    return pred(left, right) ? right : left;
}

// Base case for computing the minimum of a variadic list of values at compile time
template <typename Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __minOfVariadic(const Type& first NEX_LIFETIMEBOUND) noexcept {
    return first;
}

// Compute the minimum of a variadic list of values at compile time
template <typename Type, typename... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __minOfVariadic(const Type& first NEX_LIFETIMEBOUND, const Types&... rest NEX_LIFETIMEBOUND)
noexcept(noexcept((__minOf(first, __minOfVariadic(rest...))))) {
    return __minOf(first, __minOfVariadic(rest...));
}

// Base case for computing the maximum of a variadic list of values at compile time
template <typename Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __maxOfVariadic(const Type& first NEX_LIFETIMEBOUND) noexcept {
    return first;
}

// Compute the maximum of a variadic list of values at compile time
template <typename Type, typename... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __maxOfVariadic(const Type& first NEX_LIFETIMEBOUND, const Types&... rest NEX_LIFETIMEBOUND)
noexcept(noexcept((__maxOf(first, __maxOfVariadic(rest...))))) {
    return __maxOf(first, __maxOfVariadic(rest...));
}

// Base case for computing the minimum of a variadic list of values at compile time based on a custom predicate
template <typename Predicate, typename Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __minOfVariadicIf(Predicate pred, const Type& first NEX_LIFETIMEBOUND) noexcept {
    return first;
}

// Compute the minimum of a variadic list of values at compile time based on a custom predicate
template <typename Predicate, typename Type, typename... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __minOfVariadicIf(Predicate pred, const Type& first NEX_LIFETIMEBOUND, const Types&... rest NEX_LIFETIMEBOUND)
noexcept(noexcept((__minOfIf(first, __minOfVariadicIf(pred, rest...), pred)))) {
    return __minOfIf(first, __minOfVariadicIf(pred, rest...), pred);
}

// Base case for computing the maximum of a variadic list of values at compile time based on a custom predicate
template <typename Predicate, typename Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __maxOfVariadicIf(Predicate pred, const Type& first NEX_LIFETIMEBOUND) noexcept {
    return first;
}

// Compute the maximum of a variadic list of values at compile time based on a custom predicate
template <typename Predicate, typename Type, typename... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& __maxOfVariadicIf(Predicate pred, const Type& first NEX_LIFETIMEBOUND, const Types&... rest NEX_LIFETIMEBOUND)
noexcept(noexcept((__maxOfIf(first, __maxOfVariadicIf(pred, rest...), pred)))) {
    return __maxOfIf(first, __maxOfVariadicIf(pred, rest...), pred);
}

// Compute the minimum of the sizes of a variadic list of types at compile time
template <class Type, class... Types>
constexpr auto MinSizeOfV = __minOfVariadic(sizeof(Type), sizeof(Types)...);

// Compute the maximum of the sizes of a variadic list of types at compile time
template <class Type, class... Types>
constexpr auto MaxSizeOfV = __maxOfVariadic(sizeof(Type), sizeof(Types)...);

// Compute the minimum of the alignments of a variadic list of types at compile time
template <class Type, class... Types>
constexpr auto MinAlignOfV = __minOfVariadic(alignof(Type), alignof(Types)...);

// Compute the maximum of the alignments of a variadic list of types at compile time
template <class Type, class... Types>
constexpr auto MaxAlignOfV = __maxOfVariadic(alignof(Type), alignof(Types)...);

// Perform a bitwise cast from Source to Dest at compile time, 
// ensuring that the types have the same size and are trivially copyable
template <class Dest, class Source>
    requires (sizeof(Dest) == sizeof(Source) && IsTriviallyCopyableV<Source> && IsTriviallyCopyableV<Dest>)
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
Dest __bitCastImpl(const Source& source) noexcept {
    return __builtin_bit_cast(Dest, source);
}

// Convert a character representing a digit in base 2, 8, 10, or 16 to its integer value
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
int __char2Val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

// Parse a sequence of characters representing an integer literal in base 2, 8, 10, or 16 
// into its integer value at compile time
template <typename TargetType, char... Chars>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr 
TargetType __parseRawInteger() noexcept {
    static_assert(IsIntegralV<TargetType>, "Error: TargetType must be an integral type");
    static_assert(sizeof...(Chars) > 0, 
        "Error: At least one character is required to parse an integer literal");

    // Pack the template characters into a compile-time array
    constexpr char arr[] = { Chars... };
    constexpr unsigned long long len = sizeof...(Chars);

    unsigned long long idx = 0; 
    unsigned long long base = 10;

    // Detect base prefixes (0x, 0b, 0)
    if constexpr (len >= 2) {
        if (arr[0] == '0') {
            if (arr[1] == 'x' || arr[1] == 'X') { base = 16; idx = 2; } 
            else if (arr[1] == 'b' || arr[1] == 'B') { base = 2; idx = 2; } 
            else { base = 8; idx = 1; }
        }
    }

    TargetType result = 0;
    for (; idx < len; ++idx) {
        if (arr[idx] == '\'') continue;   // ignore digit separators
        result = static_cast<TargetType>(result * base + __char2Val(arr[idx]));
    }
    return result;
}

// Parse a sequence of characters representing a floating-point literal in base 10 
// into its value at compile time
template <typename TargetType, char... Chars>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr 
TargetType __parseRawFloating() noexcept {
    static_assert(IsFloatingPointV<TargetType>, "Error: TargetType must be a floating-point type");
    static_assert(sizeof...(Chars) > 0, 
        "Error: At least one character is required to parse a floating-point literal");

    // Pack the template characters into a compile-time array
    constexpr char arr[] = { Chars... };
    constexpr unsigned long long len = sizeof...(Chars);

    TargetType result = 0;
    TargetType decimalPlace = 10;
    bool isFraction = false;
    unsigned long long idx = 0;

    // Leading sign handling 
    // (if the literal is passed as character sequence)
    bool isNegative = false;
    if constexpr (len > 0) {
        if (arr[0] == '-') { isNegative = true; idx = 1; }
        else if (arr[0] == '+') { idx = 1; }
    }

    // Loop through the characters to parse the integer 
    // and fractional parts (not handling exponent part yet)
    for (; idx < len; ++idx) {
        char c = arr[idx];
        if (c == '\'') continue;   // ignore digit separators
        if (c == '.') {
            isFraction = true;
            continue;
        }
        // If we encounter an exponent character (e or E),
        // break to handle the exponent part
        if (c == 'e' || c == 'E') {
            ++idx;
            break;
        }

        int val = __char2Val(c);
        if (val < 0 || val > 9) continue; // ignore invalid characters or f/F suffixes

        if (!isFraction) {
            result = result * 10 + val;
        } else {
            result = result + static_cast<TargetType>(val) / decimalPlace;
            decimalPlace *= 10;
        }
    }

    // Handle the exponent part (e.g., 1e-5 or 1e5)
    if (idx < len) {
        bool expNegative = false;
        if (arr[idx] == '-') { expNegative = true; ++idx; }
        else if (arr[idx] == '+') { ++idx; }

        long long exponent = 0;
        for (; idx < len; ++idx) {
            if (arr[idx] == '\'') continue;   // ignore digit separators
            int val = __char2Val(arr[idx]);
            if (val >= 0 && val <= 9) exponent = exponent * 10 + val;
        }

        TargetType eScale = 1;
        for (long long i = 0; i < exponent; ++i) eScale *= 10;

        if (expNegative) result /= eScale;
        else result *= eScale;
    }

    return isNegative ? -result : result;
}

NEX_SUBNAMESPACE_END(meta)

NEX_NAMESPACE_END
