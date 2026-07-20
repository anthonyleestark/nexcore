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

template<bool BoolCond, typename Type = void>
struct DisableIf {};

// DisableIf is the opposite of EnableIf, 
// providing a way to disable a template specialization when a condition is true.
template <typename Type>
struct DisableIf<false, Type> {
    using type = Type;
};

template <bool BoolCond, class Type = void>
using DisableIfT = typename DisableIf<BoolCond, Type>::type;

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

// Performs a compile-time logical AND operation on a pack of type traits, 
// evaluating to true if all traits are true, and false otherwise.
template <class...>
struct Conjunction : TrueType {};

template <class Arg>
struct Conjunction<Arg> : Arg {};

template <class Arg1, class... Args>
struct Conjunction<Arg1, Args...> 
    : ConditionalT<!bool(Arg1::value), Arg1, Conjunction<Args...>> {};

// Performs a compile-time logical OR operation on a pack of type traits,
// evaluating to true if at least one trait is true, and false otherwise.
template <class...>
struct Disjunction : FalseType {};

template <class Arg>
struct Disjunction<Arg> : Arg {};

template <class Arg1, class... Args>
struct Disjunction<Arg1, Args...> 
    : ConditionalT<bool(Arg1::value), Arg1, Disjunction<Args...>> {};

// Performs a compile-time logical NOT operation on a type trait,
// evaluating to true if the trait is false, and false if the trait is true.
template <class Pred>
struct Negation : BoolConstant<!bool(Pred::value)> {};

// Determine whether arguments are the same type
#if NEX_HAS_BUILTIN(__is_same)
    template <class Type1, class Type2>
    constexpr bool IsSameV = __is_same(Type1, Type2);
#else  // No builtin support, fallback to template specialization
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

// Determine whether From is convertible to To
template <class From, class To>
struct IsConvertible : BoolConstant<__is_convertible_to(From, To)> {};

template <class From, class To>
constexpr bool IsConvertibleV = IsConvertible<From, To>::value;

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

    // apply cv-qualifiers from the class template argument to Fn<Type>
    template <template <class> class Fn>
    using Apply = Fn<Type>;
};

template <class Type>
struct RemoveCv<const Type> {
    using type = Type;
    template <template <class> class Fn>
    using Apply = const Fn<Type>;
};

template <class Type>
struct RemoveCv<volatile Type> {
    using type = Type;
    template <template <class> class Fn>
    using Apply = volatile Fn<Type>;
};

template <class Type>
struct RemoveCv<const volatile Type> {
    using type = Type;
    template <template <class> class Fn>
    using Apply = const volatile Fn<Type>;
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
    #if NEX_HAS_BUILTIN_CHAR8_T
        char8_t,                            // UTF-8 code unit type (since C++20)
    #endif  // ^^NEX_HAS_BUILTIN_CHAR8_T
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
struct NEX_HIDDEN_FROM_ABI SignCheckBase {
    using Underlying = RemoveCvT<Type>;

    static constexpr bool Signed   = static_cast<Underlying>(-1) < static_cast<Underlying>(0);
    static constexpr bool Unsigned = !Signed;
};

// Specialization of _SignCheckBase for non-integral types
template <class Type>
struct NEX_HIDDEN_FROM_ABI SignCheckBase<Type, false> {
    static constexpr bool Signed   = IsFloatingPointV<Type>;   // floating-point Type is signed
    static constexpr bool Unsigned = false;                    // non-arithmetic Type is neither signed nor unsigned
};

// Determine whether an integral type is signed
template <class Type>
struct IsSignedIntegral : BoolConstant<SignCheckBase<Type>::Signed> {};

template <class Type>
constexpr bool IsSignedIntegralV = IsSignedIntegral<Type>::value;

// Determine whether an integral type is unsigned
template <class Type>
struct IsUnsignedIntegral : BoolConstant<SignCheckBase<Type>::Unsigned> {};

template <class Type>
constexpr bool IsUnsignedIntegralV = IsUnsignedIntegral<Type>::value;

// Determine whether a type is an arithmetic type (either integral or floating-point)
template <class Type>
NEX_HIDDEN_FROM_ABI constexpr bool IsArithmeticVImpl = IsIntegralV<Type> || IsFloatingPointV<Type>;

template <class Type>
struct IsArithmetic : BoolConstant<IsArithmeticVImpl<Type>> {};

template <class Type>
constexpr bool IsArithmeticV = IsArithmetic<Type>::value;

// Determine whether a type is a literal type (i.e., a type that can be used in constant expressions)
template <class Type>
struct IsLiteralType : BoolConstant<__is_literal_type(RemoveCvT<Type>)> {};

template <class Type>
constexpr bool IsLiteralTypeV = IsLiteralType<Type>::value;

// Determine whether a type is the void type
template <class Type>
constexpr bool IsVoidV = IsSameV<RemoveCvT<Type>, void>;

template <class Type>
struct IsVoid : BoolConstant<IsVoidV<Type>> {};

// Determine whether a type is a raw pointer
template <class>
NEX_HIDDEN_FROM_ABI constexpr bool IsPointerVImpl = false;

// Specialization of _IsPointerV for pointer types, 
// which checks if the type is a pointer by checking if it is of the form Type*
template <class Type>
NEX_HIDDEN_FROM_ABI constexpr bool IsPointerVImpl<Type*> = true;

// Determine whether a type is a raw pointer
template <class Type>
constexpr bool IsPointerV = IsPointerVImpl<RemoveCvT<Type>>;

// Determine whether a cv-qualified type is a null-pointer type
template <class Type>
constexpr bool IsNullPointerV = IsSameV<RemoveCvT<Type>, decltype(nullptr)>;

template <class Type>
struct IsNullPointer : BoolConstant<IsNullPointerV<Type>> {};

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
struct IsEnumClass 
    : BoolConstant<IsEnumV<Type> && (!IsReferenceV<Type> && !IsVoidV<Type> && !IsConvertibleV<RemoveCvrefT<Type>, int>)> {};

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

// Determine whether a type is a fundamental type (i.e., an arithmetic type, void, nullptr_t, etc.)
template <class Type>
constexpr bool IsFundamentalV = IsArithmeticV<Type> || IsVoidV<Type> || IsNullPointerV<Type>;

template <class Type>
struct IsFundamental : BoolConstant<IsFundamentalV<Type>> {};

// Determine whether a type is a compound type (i.e., an array, class, union, or enumeration type)
template <class Type>
struct IsCompound : BoolConstant<!IsFundamentalV<Type>> {};

template <class Type>
constexpr bool IsCompoundV = IsCompound<Type>::value;

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

// Determine whether a type is an aggregate type
// (i.e., a type that can be initialized with aggregate initialization)
#if NEX_HAS_BUILTIN(__is_aggregate)
    template <class Type>
    struct IsAggregate : BoolConstant<__is_aggregate(Type)> {};
#else  // No compiler built-in support, so we always return false
    template <typename Type>
    struct IsAggregate : public FalseType {};
#endif

template <class Type>
constexpr bool IsAggregateV = IsAggregate<Type>::value;

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
    using ConstThruRefType = const Type;
};

template <class Type>
struct RemoveReference<Type&> {
    using type = Type;
    using ConstThruRefType = const Type&;
};

template <class Type>
struct RemoveReference<Type&&> {
    using type = Type;
    using ConstThruRefType = const Type&&;
};

template <class Type>
using RemoveReferenceT = typename RemoveReference<Type>::type;

template <class Type>
using ConstThruRefImpl = typename RemoveReference<Type>::ConstThruRefType;

template <class Type>
using RemoveCvrefTImpl NEX_MSVC_KNOWN_SEMANTICS = RemoveCvT<RemoveReferenceT<Type>>;

// Remove reference and cv-qualifiers from a type
template <class Type>
using RemoveCvrefT = RemoveCvrefTImpl<Type>;

template <class Type>
struct RemoveCvref {
    using type = RemoveCvrefT<Type>;
};

// Remove array extent from a type
template <class Type>
struct RemoveExtent {
    using type = Type;
};

template <class Type, unsigned long long Size>
struct RemoveExtent<Type[Size]> {
    using type = Type;
};

template <class Type>
struct RemoveExtent<Type[]> {
    using type = Type;
};

template <class Type>
using RemoveExtentT = typename RemoveExtent<Type>::type;

// Remove all array extents from a type
template <class Type>
struct RemoveAllExtents {
    using type = Type;
};

template <class Type, unsigned long long Size>
struct RemoveAllExtents<Type[Size]> {
    using type = typename RemoveAllExtents<Type>::type;
};

template <class Type>
struct RemoveAllExtents<Type[]> {
    using type = typename RemoveAllExtents<Type>::type;
};

template <class Type>
using RemoveAllExtentsT = typename RemoveAllExtents<Type>::type;

// Remove pointer qualifiers from a type
template <class Type>
struct RemovePointer {
    using type = Type;
};

template <class Type>
struct RemovePointer<Type*> {
    using type = Type;
};

template <class Type>
struct RemovePointer<Type* const> {
    using type = Type;
};

template <class Type>
struct RemovePointer<Type* volatile> {
    using type = Type;
};

template <class Type>
struct RemovePointer<Type* const volatile> {
    using type = Type;
};

template <class Type>
using RemovePointerT = typename RemovePointer<Type>::type;

// Copy const-qualifier from one type to another
template <class From, class To>
struct CopyConst {
    using type = To;
};

template <class From, class To>
struct CopyConst<const From, To> {
    using type = const To;
};

template <class From, class To>
using CopyConstT = typename CopyConst<From, To>::type;

// Copy volatile-qualifier from one type to another
template <class From, class To>
struct CopyVolatile {
    using type = To;
};

template <class From, class To>
struct CopyVolatile<volatile From, To> {
    using type = volatile To;
};

template <class From, class To>
using CopyVolatileT = typename CopyVolatile<From, To>::type;

// Copy cv-qualifiers from one type to another
template <class From, class To>
struct CopyCv {
    using type = To;
};

template <class From, class To>
struct CopyCv<const From, To> {
    using type = const To;
};

template <class From, class To>
struct CopyCv<volatile From, To> {
    using type = volatile To;
};

template <class From, class To>
struct CopyCv<const volatile From, To> {
    using type = const volatile To;
};

template <class From, class To>
using CopyCvT = typename CopyCv<From, To>::type;

// Copy reference from one type to another
template <class From, class To>
struct CopyReference {
    using type = To;
};

template <class From, class To>
struct CopyReference<From&, To> {
    using type = To&;
};

template <class From, class To>
struct CopyReference<From&&, To> {
    using type = To&&;
};

template <class From, class To>
using CopyReferenceT = typename CopyReference<From, To>::type;

// Copy reference and cv-qualifiers from one type to another
template <class From, class To>
struct CopyCvref {
    using type = typename CopyCv<From, To>::type;
};

template <class From, class To>
struct CopyCvref<From&, To> {
    using type = typename CopyCv<From, To>::type&;
};

template <class From, class To>
struct CopyCvref<From&&, To> {
    using type = typename CopyCv<From, To>::type&&;
};

template <class From, class To>
using CopyCvrefT = typename CopyCvref<From, To>::type;

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

// Add pointer (pointer type cannot be formed)
template <class Type, class = void>
struct NEX_HIDDEN_FROM_ABI AddPointerImpl {
    using type = Type;
};

template <class Type>
struct NEX_HIDDEN_FROM_ABI AddPointerImpl<Type, VoidT<RemoveReferenceT<Type>*>> {
    using type = RemoveReferenceT<Type>*;
};

template <class Type>
struct AddPointer {
    using type = typename AddPointerImpl<Type>::type;
};

template <class Type>
using AddPointerT = typename AddPointerImpl<Type>::type;

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
struct NEX_HIDDEN_FROM_ABI AddReferenceImpl {
    using Lvalue = Type;
    using Rvalue = Type;
};

// Add reference to a type (referenceable type)
template <class Type>
struct NEX_HIDDEN_FROM_ABI AddReferenceImpl<Type, VoidT<Type&>> {
    using Lvalue = Type&;
    using Rvalue = Type&&;
};

// Add an lvalue reference to a type
template <class Type>
struct AddLvalueReference {
    using type = typename AddReferenceImpl<Type>::Lvalue;
};

template <class Type>
using AddLvalueReferenceT = typename AddReferenceImpl<Type>::Lvalue;

// Add an rvalue reference to a type
template <class Type>
struct AddRvalueReference {
    using type = typename AddReferenceImpl<Type>::Rvalue;
};

template <class Type>
using AddRvalueReferenceT = typename AddReferenceImpl<Type>::Rvalue;

// A utility function that can be used in unevaluated contexts 
// to obtain a value of a specified type without needing to construct it.
template <class Type>
AddRvalueReferenceT<Type> declval() noexcept {
    static_assert(IsSameV<Type, void> && !IsSameV<Type, void>, 
        "Error: declval not allowed in an evaluated context");
}

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

// Determine whether Type is a pointer to a member function of a class or struct

#if NEX_HAS_BUILTIN(__is_member_pointer)
    template <class Type>
    constexpr bool IsMemberPointerV = __is_member_pointer(Type);

    template <class Type>
    struct IsMemberPointer : BoolConstant<IsMemberPointerV<Type>> {};
#else  // No builtin support
    template <typename Type>
    struct NEX_HIDDEN_FROM_ABI IsMemberPointerImpl : public FalseType {};

    template <typename Type1, typename Type2>
    struct NEX_HIDDEN_FROM_ABI IsMemberPointerImpl<Type2 Type1::*> : public TrueType {};

    template<typename Type>
    struct IsMemberPointer : public IsMemberPointerImpl<RemoveCvT<Type>> {};

    template<typename Type>
    constexpr bool IsMemberPointerV = IsMemberPointer<Type>::value;
#endif

// Determine whether Type is a pointer to a member object of a class or struct
template <class>
struct NEX_HIDDEN_FROM_ABI IsMemberObjectPointerImpl {
    static constexpr bool value = false;
};

template <class Type1, class Type2>
struct NEX_HIDDEN_FROM_ABI IsMemberObjectPointerImpl<Type1 Type2::*> {
    static constexpr bool value = !IsFunctionV<Type1>;
    using ClassType             = Type2;
};

#if NEX_HAS_BUILTIN(__is_member_object_pointer)
    template <class Type>
    constexpr bool IsMemberObjectPointerV = __is_member_object_pointer(Type);
#else  // No builtin support
    template <class Type>
    constexpr bool IsMemberObjectPointerV = IsMemberObjectPointerImpl<RemoveCvT<Type>>::value;
#endif

template <class Type>
struct IsMemberObjectPointer : BoolConstant<IsMemberObjectPointerV<Type>> {};

// Determine whether Type is a pointer to a member function of a class or struct
#if NEX_HAS_BUILTIN(__is_member_function_pointer)
    template <class Type>
    constexpr bool IsMemberFunctionPointerV = __is_member_function_pointer(Type);

    template <class Type>
    struct IsMemberFunctionPointer : BoolConstant<IsMemberFunctionPointerV<Type>> {};
#else  // No builtin support
    template<typename Type>
    struct NEX_HIDDEN_FROM_ABI IsMemberFunctionPointerImpl : FalseType {};

    template<typename Type1, typename Type2>
    struct NEX_HIDDEN_FROM_ABI IsMemberFunctionPointerImpl<Type1 Type2::*> : IsFunction<Type1> {};

    template<typename Type>
    struct IsMemberFunctionPointer : IsMemberFunctionPointerImpl<RemoveCvT<Type>> {};

    template<typename Type>
    constexpr bool IsMemberFunctionPointerV = IsMemberFunctionPointer<Type>::value;
#endif

// Determine whether Type is a scalar type
// (i.e., an arithmetic type, enumeration type, pointer type, member pointer type, or nullptr)
#if NEX_HAS_BUILTIN(__is_scalar)
    template <typename Type>
    constexpr bool IsScalarV = __is_scalar(Type);

    template <typename Type>
    struct IsScalar : BoolConstant<IsScalarV<Type>> {};
#else  // No builtin support
    template <typename Type>
    struct IsScalar
        : public IntegralConstant<
                    bool,
                    IsArithmeticV<Type> || IsEnumV<Type> || IsPointerV<Type> ||
                    IsMemberPointerV<Type> || IsNullPointerV<Type>
                > {};

    template <typename Type> struct IsScalar<Type*>                : public TrueType {};
    template <typename Type> struct IsScalar<Type* const>          : public TrueType {};
    template <typename Type> struct IsScalar<Type* volatile>       : public TrueType {};
    template <typename Type> struct IsScalar<Type* const volatile> : public TrueType {};

    template<typename Type>
    constexpr bool IsScalarV = IsScalar<Type>::value;
#endif

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
struct NEX_HIDDEN_FROM_ABI IsImplicitlyDefaultConstructibleImpl : FalseType {};

// Helper function to test whether Type can be copy-initialized with {}
template <class Type>
NEX_HIDDEN_FROM_ABI void ImplicitlyDefaultConstructImpl(const Type&);

template <class Type>
struct NEX_HIDDEN_FROM_ABI IsImplicitlyDefaultConstructibleImpl<
    Type, 
    VoidT<decltype(ImplicitlyDefaultConstructImpl<Type>({}))>
    > : TrueType {};

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
    struct NEX_HIDDEN_FROM_ABI IsAssignableNoPreconditionCheckImpl 
        : BoolConstant<__is_assignable_no_precondition_check(To, From)> {};
#else // Use standard type trait as fallback when intrinsic is not supported
    template <class To, class From>
    using IsAssignableNoPreconditionCheckImpl = IsAssignable<To, From>;
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
    struct NEX_HIDDEN_FROM_ABI IsCopyAssignableNoPreconditionCheckImpl
        : BoolConstant<__is_assignable_no_precondition_check(
            AddLvalueReferenceT<Type>, AddLvalueReferenceT<const Type>)> {};

    template <class Type>
    NEX_HIDDEN_FROM_ABI constexpr 
    bool IsCopyAssignableUncheckedVImpl = IsCopyAssignableNoPreconditionCheckImpl<Type>::value;
#else // Use standard type trait as fallback when intrinsic is not supported
    template <class Type>
    using IsCopyAssignableNoPreconditionCheckImpl = IsCopyAssignable<Type>;

    template <class Type>
    NEX_HIDDEN_FROM_ABI constexpr bool IsCopyAssignableUncheckedVImpl = IsCopyAssignableV<Type>;
#endif // intrinsic support

// Determine whether an rvalue Type can be assigned to an lvalue Type
template <class Type>
struct IsMoveAssignable : BoolConstant<__is_assignable(AddLvalueReferenceT<Type>, Type)> {};

template <class Type>
constexpr bool IsMoveAssignableV = IsMoveAssignable<Type>::value;

#if defined(NEX_IS_ASSIGNABLE_NOCHECK_SUPPORTED) && !defined(__CUDACC__)
    template <class Type>
    struct NEX_HIDDEN_FROM_ABI IsMoveAssignableNoPreconditionCheckImpl
        : BoolConstant<__is_assignable_no_precondition_check(AddLvalueReferenceT<Type>, Type)> {};

    template <class Type>
    NEX_HIDDEN_FROM_ABI constexpr 
    bool IsMoveAssignableUncheckedVImpl = IsMoveAssignableNoPreconditionCheckImpl<Type>::value;
#else // Use standard type trait as fallback when intrinsic is not supported
    template <class Type>
    using IsMoveAssignableNoPreconditionCheckImpl = IsMoveAssignable<Type>;

    template <class Type>
    NEX_HIDDEN_FROM_ABI constexpr bool IsMoveAssignableUncheckedVImpl = IsMoveAssignableV<Type>;
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

// Determine whether Type can be replaced with another type (i.e., whether it is trivially copyable)
template <class Type, class = void>
struct IsReplaceable : IsTriviallyCopyable<Type> {};

template <class Type>
struct IsReplaceable<Type, EnableIfT<IsSameV<Type, typename Type::Replaceable>>> : TrueType {};

template <class Type>
constexpr bool IsReplaceableV = IsReplaceable<Type>::value;

// Transform a type into a form suitable for use in most contexts (e.g., as a function argument or return type)
#if NEX_HAS_BUILTIN(__decay)
    template <class Type>
    using Decay = __decay(Type);
#else
    template <class Type>
    struct Decay {
        using Type1 = RemoveReferenceT<Type>;
        using Type2 = ConditionalT<IsFunctionV<Type1>, AddPointerT<Type1>, RemoveCvT<Type1>>;
        using type = ConditionalT<IsArrayV<Type1>, AddPointerT<RemoveExtentT<Type1>>, Type2>;
    };
#endif
template <class Type>
using DecayT = typename Decay<Type>::type;

// Determine the common type of a set of types
template<typename... Types>
struct CommonType;

// Determine the common type of a single type (i.e., the type itself)
template<typename Type>
struct CommonType<Type> {
    /**
     * @note
     * Question: Should we use Type or decay_t<Type> here?
     * The C++11 Standard specifically (20.9.7.6,p3) specifies that it be without decay,
     * but libc++ uses decay.
     */
    using type = DecayT<Type>;
};

// Determine the common type of two types
template<typename Type1, typename Type2>
struct CommonType<Type1, Type2> {
    // The type of a tertiary expression is set by the compiler to be the common type of the two result types.
    using type = DecayT<decltype(true ? declval<Type1>() : declval<Type2>())>;
};

// Determine the common type of three or more types
template<typename Type1, typename Type2, typename... Types>
struct CommonType<Type1, Type2, Types...> { 
    using type = typename CommonType<typename CommonType<Type1, Type2>::type, Types...>::type;
};

template <typename... T>
using CommonTypeT = typename CommonType<T...>::type;

// Represents a list of types as a single type
template <class... Types>
struct NEX_HIDDEN_FROM_ABI TypeList {};

// Find the first type in TypeList that is compatible with InType and has a size greater than or equal to Size
template <class TypeList, class InType, unsigned long long Size, bool = false>
struct NEX_HIDDEN_FROM_ABI FindFirstCompatible;

template <class Head, class... Tail, class InType, unsigned long long Size>
struct NEX_HIDDEN_FROM_ABI FindFirstCompatible<TypeList<Head, Tail...>, InType, Size, false> {
    using type = typename ConditionalT<
        IsSameV<InType, Head>, TypeIdentity<Head>,  // Returns immediately if we find an exact match
        FindFirstCompatible<TypeList<Tail...>, InType, Size, (Size <= sizeof(Head))>  // keep searching for compatible size
    >::type;
};

// Return the first type that is compatible with the size requirement, even if it is not an exact match
template <class Head, class... Tail, class InType, unsigned long long Size>
struct NEX_HIDDEN_FROM_ABI FindFirstCompatible<TypeList<Head, Tail...>, InType, Size, true> {
    using type = Head;
};

template <class InType, unsigned long long Size, bool Any>
struct NEX_HIDDEN_FROM_ABI FindFirstCompatible<TypeList<>, InType, Size, Any> {
    using type = void;
};

// Determine the signed integer type corresponding to an unsigned integer type
#if NEX_HAS_BUILTIN(__make_signed)
    template <class Type>
    struct MakeSigned {
        using type = __make_signed(Type);
    };
#else  // No builtin support, fallback to manual implementation
    using SignedTypes = TypeList<
        signed char, 
        short, 
        int, 
        long, 
        long long
    #if NEX_HAS_BUILTIN_INT128
        , __int128
    #endif
    >;

    template <class Type>
    struct MakeSigned {
        using CleanType = RemoveCvT<Type>;
        using SignedBase = typename FindFirstCompatible<SignedTypes, CleanType, sizeof(CleanType)>::type;
        using type = CopyCvT<CleanType, SignedBase>;
    };
#endif

template <class Type>
using MakeSignedT = typename MakeSigned<Type>::type;

// Determine the unsigned integer type corresponding to a signed integer type
#if NEX_HAS_BUILTIN(__make_unsigned)
    template <class Type>
    struct MakeUnsigned {
        using type = __make_unsigned(Type);
    };
#else  // No builtin support, fallback to manual implementation
    using UnsignedTypes = TypeList<
        unsigned char, 
        unsigned short, 
        unsigned int, 
        unsigned long, 
        unsigned long long
    #if NEX_HAS_BUILTIN_INT128
        , unsigned __int128
    #endif
    >;

    template <class Type>
    struct MakeUnsigned {
        using CleanType = RemoveCvT<Type>;
        using UnsignedBase = typename FindFirstCompatible<UnsignedTypes, CleanType, sizeof(CleanType)>::type;
        using type = CopyCvT<CleanType, UnsignedBase>;
    };
#endif

template <class Type>
using MakeUnsignedT = typename MakeUnsigned<Type>::type;

// =================================================================================
// C++20 CommonReference implementation (ISO Standard Compliant)
// =================================================================================

// Let COND_RES(X, Y) be:
template <class X, class Y>
using CondResT NEX_NODEBUG = decltype(false ? declval<X (&)()>()() : declval<Y (&)()>()());

// Let `XREF(A)` denote a unary alias template `T` such that `T<U>` denotes the same type as `U`
// with the addition of `A`'s cv and reference qualifiers, for a non-reference cv-unqualified type `U`.
// [Note: `XREF(A)` is `Xref<A>::template Apply`]
template <class Type>
struct Xref {
    template <class Up>
    using Apply NEX_NODEBUG = CopyCvrefT<Type, Up>;
};

// Given types A and B, let X be RemoveReferenceT<A>, let Y be RemoveReferenceT<B>,
// and let COMMON-REF(A, B) be:
template <class A, class B, class X = RemoveReferenceT<A>, class Y = RemoveReferenceT<B>>
struct CommonRef;

template <class X, class Y>
using CommonRefT NEX_NODEBUG = typename CommonRef<X, Y>::type;

template <class X, class Y>
using CvCondResT NEX_NODEBUG = CondResT<CopyCvT<X, Y>&, CopyCvT<Y, X>&>;

// If A and B are both lvalue reference types, COMMON-REF(A, B) is
// COND-RES(COPYCV(X, Y)&, COPYCV(Y, X)&) if that type exists and is a reference type.
template <class A, class B, class X, class Y>
    requires requires { typename CvCondResT<X, Y>; } && IsReferenceV<CvCondResT<X, Y>>
struct CommonRef<A&, B&, X, Y> {
    using type NEX_NODEBUG = CvCondResT<X, Y>;
};

// Otherwise, let C be RemoveReferenceT<COMMON-REF(X&, Y&)>&&. ...
template <class X, class Y>
using CommonRefCT NEX_NODEBUG = RemoveReferenceT<CommonRefT<X&, Y&>>&&;

// .... If A and B are both rvalue reference types, C is well-formed, and
// IsConvertibleV<A, C> && IsConvertibleV<B, C> is true, then COMMON-REF(A, B) is C.
template <class A, class B, class X, class Y>
    requires
        requires { typename CommonRefCT<X, Y>; } &&
             IsConvertibleV<A&&, CommonRefCT<X, Y>> &&
             IsConvertibleV<B&&, CommonRefCT<X, Y>>
struct CommonRef<A&&, B&&, X, Y> {
    using type NEX_NODEBUG = CommonRefCT<X, Y>;
};

// Otherwise, let D be COMMON-REF(const X&, Y&). ...
template <class T, class U>
using CommonRefDT NEX_NODEBUG = CommonRefT<const T&, U&>;

// ... If A is an rvalue reference and B is an lvalue reference and D is well-formed and
// IsConvertibleV<A, D> is true, then COMMON-REF(A, B) is D.
template <class A, class B, class X, class Y>
    requires
        requires { typename CommonRefDT<X, Y>; } && 
            IsConvertibleV<A&&, CommonRefDT<X, Y>>
struct CommonRef<A&&, B&, X, Y> {
    using type NEX_NODEBUG = CommonRefDT<X, Y>;
};

// Otherwise, if A is an lvalue reference and B is an rvalue reference, then
// COMMON-REF(A, B) is COMMON-REF(B, A).
template <class A, class B, class X, class Y>
struct CommonRef<A&, B&&, X, Y> : CommonRef<B&&, A&> {};

// Otherwise, COMMON-REF(A, B) is ill-formed.
template <class A, class B, class X, class Y>
struct CommonRef {};

// Note C: For the common_reference trait applied to a parameter pack [...]

template <class, class, template <class> class, template <class> class>
struct BasicCommonReference {};

template <class...>
struct CommonReference;

template <class... Types>
using CommonReferenceT = typename CommonReference<Types...>::type;

// Bullet 1: sizeof...(T) == 0
template <>
struct CommonReference<> {};

// Bullet 2: sizeof...(T) == 1
template <class Type>
struct CommonReference<Type> {
    using type NEX_NODEBUG = Type;
};

// Bullet 3: sizeof...(T) == 2

// Sub-bullet 1:
// Let R be COMMON-REF(T1, T2). If T1 and T2 are reference types, R is well-formed, and
// IsConvertibleV<AddPointerT<T1>, AddPointerT<R>> && IsConvertibleV<AddPointerT<T2>, AddPointerT<R>>
// is true, then the member typedef type denotes R.
//
// Sub-bullet 2:
// Otherwise, if BasicCommonReference<RemoveCvrefT<T1>, RemoveCvrefT<T2>, XREF(T1), XREF(T2)>::type
// is well-formed, then the member typedef `type` denotes that type.
//
// Sub-bullet 3:
// Otherwise, if COND-RES(T1, T2) is well-formed, then the member typedef `type` denotes that type.
//
// Sub-bullet 4 & 5:
// Otherwise, if common_type_t<T1, T2> is well-formed, then the member typedef `type` denotes that type.
// - Otherwise, there shall be no member `type`.

template <class T1, class T2>
struct CommonReferenceSub3 : CommonType<T1, T2> {}; // Sub-bullet 4 & 5

template <class T1, class T2>
    requires requires { typename CondResT<T1, T2>; }
struct CommonReferenceSub3<T1, T2> {
    using type NEX_NODEBUG = CondResT<T1, T2>; // Sub-bullet 3
};

template <class T1, class T2>
using BasicCommonRefImplT = typename BasicCommonReference<
    RemoveCvrefT<T1>, RemoveCvrefT<T2>, 
    Xref<T1>::template Apply, Xref<T2>::template Apply
>::type;

template <class T1, class T2>
struct CommonReferenceSub2 : CommonReferenceSub3<T1, T2> {};

template <class T1, class T2>
    requires requires { typename BasicCommonRefImplT<T1, T2>; }
struct CommonReferenceSub2<T1, T2> {
    using type NEX_NODEBUG = BasicCommonRefImplT<T1, T2>; // Sub-bullet 2
};

template <class T1, class T2>
struct CommonReferenceSub1 : CommonReferenceSub2<T1, T2> {};

template <class T1, class T2>
    requires IsReferenceV<T1> && IsReferenceV<T2> && 
        requires { typename CommonRefT<T1, T2>; } &&
             IsConvertibleV<AddPointerT<T1>, AddPointerT<CommonRefT<T1, T2>>> &&
             IsConvertibleV<AddPointerT<T2>, AddPointerT<CommonRefT<T1, T2>>>
struct CommonReferenceSub1<T1, T2> {
    using type NEX_NODEBUG = CommonRefT<T1, T2>; // Sub-bullet 1
};

template <class T1, class T2>
struct CommonReference<T1, T2> : CommonReferenceSub1<T1, T2> {};

// Bullet 4:
// If there is such a type `C`, the member typedef type shall denote the same type,
// if any, as `common_reference_t<C, Rest...>`.
template <class T1, class T2, class V, class... Rest>
    requires requires { typename CommonReferenceT<T1, T2>; }
struct CommonReference<T1, T2, V, Rest...> : CommonReference<CommonReferenceT<T1, T2>, V, Rest...> {};

// Bullet 5: Otherwise, there shall be no member `type`.
template <class...>
struct CommonReference {};

// =================================================================================
// Compiler intrinsic concepts implementation
// =================================================================================

// Checks whether two types are the same type.
template <typename Type1, typename Type2>
concept SameAs =
    IsSameV<Type1, Type2> &&
    IsSameV<Type2, Type1>;

// Checks whether Derived publicly and unambiguously derives from Base.
template <typename Derived, typename Base>
concept DerivedFrom = 
    IsBaseOfV<Base, Derived> && 
    IsConvertibleV<const volatile Derived*, const volatile Base*>;

// Checks whether From is implicitly convertible to To.
template <typename From, typename To>
concept ConvertibleTo = 
    IsConvertibleV<From, To> &&
    requires {
        static_cast<To>(declval<From>());
    };

// Checks whether Tp and Up have a common reference type that is the same in both directions,
// and that both Tp and Up are convertible to that common reference type.
template <class Tp, class Up>
concept CommonReferenceWith =
    SameAs<CommonReferenceT<Tp, Up>, CommonReferenceT<Up, Tp>> &&
    ConvertibleTo<Tp, CommonReferenceT<Tp, Up>> && ConvertibleTo<Up, CommonReferenceT<Tp, Up>>;

// Checks whether Tp and Up have a common type that is the same in both directions,
// and that both Tp and Up are convertible to that common type.
template <class Tp, class Up>
concept CommonWith =
    SameAs<CommonTypeT<Tp, Up>, CommonTypeT<Up, Tp>> &&
    requires {
        static_cast<CommonTypeT<Tp, Up>>(declval<Tp>());
        static_cast<CommonTypeT<Tp, Up>>(declval<Up>());
    } &&
    CommonReferenceWith<
        AddLvalueReferenceT<const Tp>,
        AddLvalueReferenceT<const Up>> &&
    CommonReferenceWith<
        AddLvalueReferenceT<CommonTypeT<Tp, Up>>,
        CommonReferenceT<
            AddLvalueReferenceT<const Tp>,
            AddLvalueReferenceT<const Up>>>;

// =================================================================================
// Internal utilities for compile-time logic processing to support metaprogramming
// =================================================================================

// Compute the minimum of two values at compile time
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _minOf(const Type& left NEX_LIFETIMEBOUND, const Type& right NEX_LIFETIMEBOUND)
noexcept(noexcept(left < right)) {
    return left < right ? left : right;
}

// Compute the maximum of two values at compile time
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _maxOf(const Type& left NEX_LIFETIMEBOUND, const Type& right NEX_LIFETIMEBOUND)
noexcept(noexcept(left < right)) {
    return left < right ? right : left;
}

// Compute the minimum of two values at compile time based on a custom predicate
template <class Type, class Predicate>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _minOfIf(const Type& left NEX_LIFETIMEBOUND, const Type& right NEX_LIFETIMEBOUND, Predicate pred)
noexcept(noexcept(pred(left, right))) {
    return pred(left, right) ? left : right;
}

// Compute the maximum of two values at compile time based on a custom predicate
template <class Type, class Predicate>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _maxOfIf(const Type& left NEX_LIFETIMEBOUND, const Type& right NEX_LIFETIMEBOUND, Predicate pred)
noexcept(noexcept(pred(left, right))) {
    return pred(left, right) ? right : left;
}

// Base case for computing the minimum of a variadic list of values at compile time
template <typename Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _minOfVariadic(const Type& first NEX_LIFETIMEBOUND) noexcept {
    return first;
}

// Compute the minimum of a variadic list of values at compile time
template <typename Type, typename... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _minOfVariadic(const Type& first NEX_LIFETIMEBOUND, const Types&... rest NEX_LIFETIMEBOUND)
noexcept(noexcept((_minOf(first, _minOfVariadic(rest...))))) {
    return _minOf(first, _minOfVariadic(rest...));
}

// Base case for computing the maximum of a variadic list of values at compile time
template <typename Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _maxOfVariadic(const Type& first NEX_LIFETIMEBOUND) noexcept {
    return first;
}

// Compute the maximum of a variadic list of values at compile time
template <typename Type, typename... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _maxOfVariadic(const Type& first NEX_LIFETIMEBOUND, const Types&... rest NEX_LIFETIMEBOUND)
noexcept(noexcept((_maxOf(first, _maxOfVariadic(rest...))))) {
    return _maxOf(first, _maxOfVariadic(rest...));
}

// Base case for computing the minimum of a variadic list of values at compile time based on a custom predicate
template <typename Predicate, typename Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _minOfVariadicIf(Predicate pred, const Type& first NEX_LIFETIMEBOUND) noexcept {
    return first;
}

// Compute the minimum of a variadic list of values at compile time based on a custom predicate
template <typename Predicate, typename Type, typename... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _minOfVariadicIf(Predicate pred, const Type& first NEX_LIFETIMEBOUND, const Types&... rest NEX_LIFETIMEBOUND)
noexcept(noexcept((_minOfIf(first, _minOfVariadicIf(pred, rest...), pred)))) {
    return _minOfIf(first, _minOfVariadicIf(pred, rest...), pred);
}

// Base case for computing the maximum of a variadic list of values at compile time based on a custom predicate
template <typename Predicate, typename Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _maxOfVariadicIf(Predicate pred, const Type& first NEX_LIFETIMEBOUND) noexcept {
    return first;
}

// Compute the maximum of a variadic list of values at compile time based on a custom predicate
template <typename Predicate, typename Type, typename... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
const Type& _maxOfVariadicIf(Predicate pred, const Type& first NEX_LIFETIMEBOUND, const Types&... rest NEX_LIFETIMEBOUND)
noexcept(noexcept((_maxOfIf(first, _maxOfVariadicIf(pred, rest...), pred)))) {
    return _maxOfIf(first, _maxOfVariadicIf(pred, rest...), pred);
}

// Compute the minimum of the sizes of a variadic list of types at compile time
template <class Type, class... Types>
constexpr auto MinSizeOfV = _minOfVariadic(sizeof(Type), sizeof(Types)...);

// Compute the maximum of the sizes of a variadic list of types at compile time
template <class Type, class... Types>
constexpr auto MaxSizeOfV = _maxOfVariadic(sizeof(Type), sizeof(Types)...);

// Compute the minimum of the alignments of a variadic list of types at compile time
template <class Type, class... Types>
constexpr auto MinAlignOfV = _minOfVariadic(alignof(Type), alignof(Types)...);

// Compute the maximum of the alignments of a variadic list of types at compile time
template <class Type, class... Types>
constexpr auto MaxAlignOfV = _maxOfVariadic(alignof(Type), alignof(Types)...);

// Obtains the actual address of an object, safely bypassing any overloaded operator&,
// with checking for rvalue references to prevent misuse on temporaries.
template <class Type>
    requires (!IsVoidV<Type> && !IsRvalueReferenceV<Type&&>)
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_ALWAYS_INLINE constexpr 
RemoveReferenceT<Type>* _addressOf(Type&& Value) noexcept {
    return __builtin_addressof(Value);
}

// Overload of _minOf that takes pointers and compares their addresses at compile time
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
Type* _minOf(Type* left NEX_LIFETIMEBOUND, Type* right NEX_LIFETIMEBOUND) noexcept {
    // It's 100% safe to cast pointers to a 64-bit unsigned integer for comparison purposes, 
    // as long as we don't return or dereference them or perform any arithmetic on them.
    using addr_type = unsigned long long;
    return static_cast<addr_type>(left) < static_cast<addr_type>(right) ? left : right;
}

// Base case for computing the minimum address of a variadic list of objects at compile time
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
auto MinAddressOfV(Type&& first NEX_LIFETIMEBOUND) noexcept 
    -> decltype(_addressOf(static_cast<Type&&>(first))) {
    return _addressOf(static_cast<Type&&>(first));
}

// Compute the minimum address of a variadic list of objects at compile time
template <class Type, class... Args>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
auto MinAddressOfV(Type&& first NEX_LIFETIMEBOUND, Args&&... rest NEX_LIFETIMEBOUND) noexcept 
    -> decltype(_minOf(_addressOf(static_cast<Type&&>(first)), MinAddressOfV(static_cast<Args&&>(rest)...))) {
    return _minOf(_addressOf(static_cast<Type&&>(first)), MinAddressOfV(static_cast<Args&&>(rest)...));
}

// Overload of _maxOf that takes pointers and compares their addresses at compile time
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
Type* _maxOf(Type* left NEX_LIFETIMEBOUND, Type* right NEX_LIFETIMEBOUND) noexcept {
    // It's 100% safe to cast pointers to a 64-bit unsigned integer for comparison purposes, 
    // as long as we don't return or dereference them or perform any arithmetic on them.
    using addr_type = unsigned long long;
    return static_cast<addr_type>(left) < static_cast<addr_type>(right) ? right : left;
}

// Base case for computing the maximum address of a variadic list of objects at compile time
template <class Type>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
auto MaxAddressOfV(Type&& first NEX_LIFETIMEBOUND) noexcept 
    -> decltype(_addressOf(static_cast<Type&&>(first))) {
    return _addressOf(static_cast<Type&&>(first));
}

// Compute the maximum address of a variadic list of objects at compile time
template <class Type, class... Args>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
auto MaxAddressOfV(Type&& first NEX_LIFETIMEBOUND, Args&&... rest NEX_LIFETIMEBOUND) noexcept 
    -> decltype(_maxOf(_addressOf(static_cast<Type&&>(first)), MaxAddressOfV(static_cast<Args&&>(rest)...))) {
    return _maxOf(_addressOf(static_cast<Type&&>(first)), MaxAddressOfV(static_cast<Args&&>(rest)...));
}

// Perform a bitwise cast from Source to Dest at compile time, 
// ensuring that the types have the same size and are trivially copyable
template <class Dest, class Source>
    requires (sizeof(Dest) == sizeof(Source) && IsTriviallyCopyableV<Source> && IsTriviallyCopyableV<Dest>)
NEX_NODISCARD NEX_MSVC_INTRINSIC NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr
Dest _bitCastImpl(Source source) noexcept {
    return __builtin_bit_cast(Dest, source);
}

// Convert a character representing a digit in base 2, 8, 10, or 16 to its integer value
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr 
int _char2Val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

// Parse a sequence of characters representing an integer literal in base 2, 8, 10, or 16 
// into its integer value at compile time
template <typename TargetType, char... Chars>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr 
TargetType _parseRawInteger() noexcept {
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
        result = static_cast<TargetType>(result * base + _char2Val(arr[idx]));
    }
    return result;
}

// Parse a sequence of characters representing a floating-point literal in base 10 
// into its value at compile time
template <typename TargetType, char... Chars>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr 
TargetType _parseRawFloating() noexcept {
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

        int val = _char2Val(c);
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
            int val = _char2Val(arr[idx]);
            if (val >= 0 && val <= 9) exponent = exponent * 10 + val;
        }

        TargetType eScale = 1;
        for (long long i = 0; i < exponent; ++i) eScale *= 10;

        if (expNegative) result /= eScale;
        else result *= eScale;
    }

    return isNegative ? -result : result;
}

// This function returns whether the type `Second` can be stuffed into the tail padding
// of the `First` type if both of them are given `[[no_unique_address]]`.
template <class First, class Second>
NEX_HIDDEN_FROM_ABI inline constexpr 
bool _fitsInTailPadding = []() {
    struct X {
        NEX_NO_UNIQUE_ADDRESS First first;
        NEX_NO_UNIQUE_ADDRESS Second second;
    };

    return sizeof(X) == sizeof(First);
}();

NEX_SUBNAMESPACE_END(meta)

NEX_NAMESPACE_END
