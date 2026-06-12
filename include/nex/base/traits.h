/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  traits.h
 * @brief A collection of type traits and utilities for working with types in C++.
 * 
 * @details
 * This header provides a collection of type traits and utilities that simplify the use of standard 
 * C++ type traits. It includes aliases for common type traits, as well as additional utilities for 
 * working with types in a more concise and readable manner. The traits defined in this header can be 
 * used to perform compile-time checks on types, manipulate types, and enable or disable template 
 * instantiations based on type properties.
 */

#include <type_traits>
#include <iterator>
#include <tuple>
#include <utility>

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/casts.h"
#include "nex/base/concepts.h"

NEX_NAMESPACE_BEGIN

/// IntegralConstant is a template that represents a compile-time constant value of a specified type. 
/// It is the basis for other type traits like BoolConstant, TrueType, and FalseType.
template <class Type, Type Value>
using IntegralConstant = meta::IntegralConstant<Type, Value>;

/// BoolConstant is a specialization of IntegralConstant for boolean values, 
/// providing a convenient way to represent compile-time boolean constants.
template <bool BoolValue>
using BoolConstant = meta::BoolConstant<BoolValue>;

/// Represents the compile-time constant value `true`.
using TrueType = meta::TrueType;

/// Represents the compile-time constant value `false`.
using FalseType = meta::FalseType;

/// Checks whether Type is an integral type (e.g., int32, int64, nchar, etc.).
template <class Type>
using IsIntegral = BoolConstant<meta::IsIntegralV<Type>>;
template <class Type>
inline constexpr bool IsIntegralV = IsIntegral<Type>::value;

/// Checks whether Type is a floating-point type (i.e., float32, float64, or ldouble).
template <class Type>
using IsFloatingPoint = BoolConstant<meta::IsFloatingPointV<Type>>;
template <class Type>
inline constexpr bool IsFloatingPointV = IsFloatingPoint<Type>::value;

/// Checks whether Type is an arithmetic type (i.e., integral or floating-point).
template <class Type>
using IsArithmetic = BoolConstant<Arithmetic<Type>>;
template <class Type>
inline constexpr bool IsArithmeticV = IsArithmetic<Type>::value;

/// Checks whether Type is a literal type (i.e., a type that can be used in constant expressions).
template <class Type>
using IsLiteralType = meta::IsLiteralType<Type>;
template <class Type>
inline constexpr bool IsLiteralTypeV = IsLiteralType<Type>::value;

/// Checks whether Type is the void type.
template <typename Type>
using IsVoid = meta::IsVoid<Type>;
template <typename Type>
inline constexpr bool IsVoidV = meta::IsVoidV<Type>;

/// Checks whether Type is the nullptr type.
template <typename Type>
using IsNullPointer = meta::IsNullPointer<Type>;
template <typename Type>
inline constexpr bool IsNullPointerV = meta::IsNullPointerV<Type>;

/// Checks whether Type is a pointer type.
template <typename Type>
using IsPointer = BoolConstant<meta::IsPointerV<Type>>;
template <typename Type>
inline constexpr bool IsPointerV = IsPointer<Type>::value;

/// Checks whether Type is an array type.
template <typename Type>
using IsArray = meta::IsArray<Type>;
template <typename Type>
inline constexpr bool IsArrayV = IsArray<Type>::value;

/// Checks whether Type is a bounded array type (i.e., an array with a known size).
template <typename Type>
using IsBoundedArray = meta::IsBoundedArray<Type>;
template <typename Type>
inline constexpr bool IsBoundedArrayV = IsBoundedArray<Type>::value;

/// Checks whether Type is an unbounded array type (i.e., an array with an unknown size).
template <typename Type>
using IsUnboundedArray = meta::IsUnboundedArray<Type>;
template <typename Type>
inline constexpr bool IsUnboundedArrayV = IsUnboundedArray<Type>::value;

/// Checks whether Type is an enumeration type (i.e., an enum).
template <typename Type>
using IsEnum = meta::IsEnum<Type>;
template <typename Type>
inline constexpr bool IsEnumV = IsEnum<Type>::value;

/// Checks whether Type is a scoped enumeration type (i.e., an enum class).
template <typename Type>
using IsEnumClass = meta::IsEnumClass<Type>;
template <typename Type>
inline constexpr bool IsEnumClassV = IsEnumClass<Type>::value;

/// Checks whether Type is a union type.
template <typename Type>
using IsUnion = meta::IsUnion<Type>;
template <typename Type>
inline constexpr bool IsUnionV = IsUnion<Type>::value;

/// Check whether Type is a monostate type (i.e., a type that has only one possible value).
template <typename Type>
using IsMonostate = meta::IsSame<Type, monostate>;
template <typename Type>
inline constexpr bool IsMonostateV = IsMonostate<Type>::value;

/// Checks whether Type is a class type (i.e., a struct or class).
template <typename Type>
using IsClass = meta::IsClass<Type>;
template <typename Type>
inline constexpr bool IsClassV = IsClass<Type>::value;

/// Checks whether Type is a function type.
template <typename Type>
using IsFunction = meta::IsFunction<Type>;
template <typename Type>
inline constexpr bool IsFunctionV = IsFunction<Type>::value;

/// Checks whether Type is a pointer to a member of a class or struct.
template <typename Type>
using IsMemberPointer = NEX_STD is_member_pointer<Type>;
template <typename Type>
inline constexpr bool IsMemberPointerV = IsMemberPointer<Type>::value;

/// Checks whether Type is a pointer to a non-function member of a class or struct.
template <typename Type>
using IsMemberObjectPointer = NEX_STD is_member_object_pointer<Type>;
template <typename Type>
inline constexpr bool IsMemberObjectPointerV = IsMemberObjectPointer<Type>::value;

/// Checks whether Type is a pointer to a member function of a class or struct.
template <typename Type>
using IsMemberFunctionPointer = NEX_STD is_member_function_pointer<Type>;
template <typename Type>
inline constexpr bool IsMemberFunctionPointerV = IsMemberFunctionPointer<Type>::value;

/// Checks whether Type is an lvalue reference type.
template <typename Type>
using IsLvalueReference = meta::IsLvalueReference<Type>;
template <typename Type>
inline constexpr bool IsLvalueReferenceV = IsLvalueReference<Type>::value;

/// Checks whether Type is an rvalue reference type.
template <typename Type>
using IsRvalueReference = meta::IsRvalueReference<Type>;
template <typename Type>
inline constexpr bool IsRvalueReferenceV = IsRvalueReference<Type>::value;

/// Checks whether Type is a reference type (either lvalue or rvalue reference).
template <typename Type>
using IsReference = meta::IsReference<Type>;
template <typename Type>
inline constexpr bool IsReferenceV = IsReference<Type>::value;

/// Checks whether Type is a fundamental type (i.e., arithmetic types, void, nullptr_t, etc.).
template <typename Type>
using IsFundamental = meta::IsFundamental<Type>;
template <typename Type>
inline constexpr bool IsFundamentalV = IsFundamental<Type>::value;

/// Checks whether Type is an object type (i.e., a type that is not a function, reference, or void).
template <typename Type>
using IsObject = meta::IsObject<Type>;
template <typename Type>
inline constexpr bool IsObjectV = IsObject<Type>::value;

/// Checks whether Type is a scalar type (i.e., an arithmetic type, enumeration type, pointer type, 
/// or pointer to member type).
template <typename Type>
using IsScalar = NEX_STD is_scalar<Type>;
template <typename Type>
inline constexpr bool IsScalarV = IsScalar<Type>::value;

/// Checks whether Type is a compound type (i.e., an array, class, union, or enumeration type).
template <typename Type>
using IsCompound = meta::IsCompound<Type>;
template <typename Type>
inline constexpr bool IsCompoundV = IsCompound<Type>::value;

/// Checks whether Type is a const-qualified type.
template <typename Type>
using IsConst = meta::IsConst<Type>;
template <typename Type>
inline constexpr bool IsConstV = IsConst<Type>::value;

/// Checks whether Type is a volatile-qualified type.
template <typename Type>
using IsVolatile = meta::IsVolatile<Type>;
template <typename Type>
inline constexpr bool IsVolatileV = IsVolatile<Type>::value;

/// Checks whether Type is signed.
template <typename Type>
using IsSigned = meta::IsSignedIntegral<Type>;
template <typename Type>
inline constexpr bool IsSignedV = IsSigned<Type>::value;

/// Checks whether Type is an unsigned type (i.e., an integral type that is not signed).
template <typename Type>
using IsUnsigned = meta::IsUnsignedIntegral<Type>;
template <typename Type>
inline constexpr bool IsUnsignedV = IsUnsigned<Type>::value;

/// Checks whether Type1 and Type2 are the same type.
template <typename Type1, typename Type2>
using IsSame = meta::IsSame<Type1, Type2>;
template <typename Type1, typename Type2>
inline constexpr bool IsSameV = IsSame<Type1, Type2>::value;

/// Checks whether Type is a specialization of the class template Template.
template <typename Type, template <typename...> class Template>
using IsSpecialization = meta::IsSpecialization<Type, Template>;
template <typename Type, template <typename...> class Template>
inline constexpr bool IsSpecializationV = IsSpecialization<Type, Template>::value;

/// Checks whether Base is a base class of Derived.
template <typename Base, typename Derived>
using IsBaseOf = meta::IsBaseOf<Base, Derived>;
template <typename Base, typename Derived>
inline constexpr bool IsBaseOfV = IsBaseOf<Base, Derived>::value;

/// Checks whether From can be implicitly converted to To.
template <typename From, typename To>
using IsConvertible = meta::IsConvertible<From, To>;
template <typename From, typename To>
inline constexpr bool IsConvertibleV = IsConvertible<From, To>::value;

/// Checks whether Type can be constructed with Args.
template <typename Type, typename... Args>
using IsConstructible = meta::IsConstructible<Type, Args...>;
template <typename Type, typename... Args>
inline constexpr bool IsConstructibleV = IsConstructible<Type, Args...>::value;

/// Checks whether Type can be trivially constructed with Args.
template <typename Type, typename... Args>
using IsTriviallyConstructible = meta::IsTriviallyConstructible<Type, Args...>;
template <typename Type, typename... Args>
inline constexpr bool IsTriviallyConstructibleV = IsTriviallyConstructible<Type, Args...>::value;

/// Checks whether Type can be constructed with Args without throwing.
template <typename Type, typename... Args>
using IsNothrowConstructible = meta::IsNothrowConstructible<Type, Args...>;
template <typename Type, typename... Args>
inline constexpr bool IsNothrowConstructibleV = IsNothrowConstructible<Type, Args...>::value;

/// Checks whether Type can be default constructed.
template <typename Type>
using IsDefaultConstructible = meta::IsDefaultConstructible<Type>;
template <typename Type>
inline constexpr bool IsDefaultConstructibleV = IsDefaultConstructible<Type>::value;

/// Checks whether Type can be trivially default constructed.
template <typename Type>
using IsTriviallyDefaultConstructible = meta::IsTriviallyDefaultConstructible<Type>;
template <typename Type>
inline constexpr bool IsTriviallyDefaultConstructibleV = IsTriviallyDefaultConstructible<Type>::value;

/// Checks whether Type can be default constructed without throwing.
template <typename Type>
using IsNothrowDefaultConstructible = meta::IsNothrowDefaultConstructible<Type>;
template <typename Type>
inline constexpr bool IsNothrowDefaultConstructibleV = IsNothrowDefaultConstructible<Type>::value;

/// Checks whether Type is copy constructible.
template <typename Type>
using IsCopyable = meta::IsCopyConstructible<Type>;
template <typename Type>
inline constexpr bool IsCopyableV = IsCopyable<Type>::value;

/// Checks whether Type is trivially copy constructible (i.e., can be copy constructed without throwing 
/// and with a trivial copy constructor).
template <typename Type>
using IsTriviallyCopyableConstructible = meta::IsTriviallyCopyConstructible<Type>;
template <typename Type>
inline constexpr bool IsTriviallyCopyableConstructibleV = IsTriviallyCopyableConstructible<Type>::value;

/// Checks whether Type is copy constructible without throwing.
template <typename Type>
using IsNothrowCopyConstructible = meta::IsNothrowCopyConstructible<Type>;
template <typename Type>
inline constexpr bool IsNothrowCopyConstructibleV = IsNothrowCopyConstructible<Type>::value;

/// Checks whether Type is move constructible.
template <typename Type>
using IsMoveable = meta::IsMoveConstructible<Type>;
template <typename Type>
using IsMovable = IsMoveable<Type>;
template <typename Type>
inline constexpr bool IsMoveableV = IsMoveable<Type>::value;
template <typename Type>
inline constexpr bool IsMovableV = IsMoveableV<Type>;

/// Checks whether Type is trivially move constructible (i.e., can be move constructed without throwing 
/// and with a trivial move constructor).
template <typename Type>
using IsTriviallyMovableConstructible = meta::IsTriviallyMoveConstructible<Type>;
template <typename Type>
inline constexpr bool IsTriviallyMovableConstructibleV = IsTriviallyMovableConstructible<Type>::value;

/// Checks whether Type is move constructible without throwing.
template <typename Type>
using IsNothrowMoveConstructible = meta::IsNothrowMoveConstructible<Type>;
template <typename Type>
inline constexpr bool IsNothrowMoveConstructibleV = IsNothrowMoveConstructible<Type>::value;

/// Checks whether To can be assigned from From.
template <typename To, typename From = To>
using IsAssignable = meta::IsAssignable<To, From>;
template <typename To, typename From = To>
inline constexpr bool IsAssignableV = IsAssignable<To, From>::value;

/// Checks whether To can be trivially assigned from From.
template <typename To, typename From = To>
using IsTriviallyAssignable = meta::IsTriviallyAssignable<To, From>;
template <typename To, typename From = To>
inline constexpr bool IsTriviallyAssignableV = IsTriviallyAssignable<To, From>::value;

/// Checks whether To can be assigned from From without throwing.
template <typename To, typename From = To>
using IsNothrowAssignable = meta::IsNothrowAssignable<To, From>;
template <typename To, typename From = To>
inline constexpr bool IsNothrowAssignableV = IsNothrowAssignable<To, From>::value;

/// Checks whether Type can be copy assigned.
template <typename Type>
using IsCopyAssignable = meta::IsCopyAssignable<Type>;
template <typename Type>
inline constexpr bool IsCopyAssignableV = IsCopyAssignable<Type>::value;

/// Checks whether Type can be trivially copy assigned.
template <typename Type>
using IsTriviallyCopyAssignable = meta::IsTriviallyCopyAssignable<Type>;
template <typename Type>
inline constexpr bool IsTriviallyCopyAssignableV = IsTriviallyCopyAssignable<Type>::value;

/// Checks whether Type can be copy assigned without throwing.
template <typename Type>
using IsNothrowCopyAssignable = meta::IsNothrowCopyAssignable<Type>;
template <typename Type>
inline constexpr bool IsNothrowCopyAssignableV = IsNothrowCopyAssignable<Type>::value;

/// Checks whether Type can be move assigned.
template <typename Type>
using IsMoveAssignable = meta::IsMoveAssignable<Type>;
template <typename Type>
inline constexpr bool IsMoveAssignableV = IsMoveAssignable<Type>::value;

/// Checks whether Type can be trivially move assigned.
template <typename Type>
using IsTriviallyMoveAssignable = meta::IsTriviallyMoveAssignable<Type>;
template <typename Type>
inline constexpr bool IsTriviallyMoveAssignableV = IsTriviallyMoveAssignable<Type>::value;

/// Checks whether Type can be move assigned without throwing.
template <typename Type>
using IsNothrowMoveAssignable = meta::IsNothrowMoveAssignable<Type>;
template <typename Type>
inline constexpr bool IsNothrowMoveAssignableV = IsNothrowMoveAssignable<Type>::value;

/// Checks whether Type can be destructed.
template <typename Type>
using IsDestructible = meta::IsDestructible<Type>;
template <typename Type>
inline constexpr bool IsDestructibleV = IsDestructible<Type>::value;

/// Checks whether Type can be trivially destructed.
template <typename Type>
using IsTriviallyDestructible = meta::IsTriviallyDestructible<Type>;
template <typename Type>
inline constexpr bool IsTriviallyDestructibleV = IsTriviallyDestructible<Type>::value;

/// Checks whether Type can be destructed without throwing.
template <typename Type>
using IsNothrowDestructible = meta::IsNothrowDestructible<Type>;
template <typename Type>
inline constexpr bool IsNothrowDestructibleV = IsNothrowDestructible<Type>::value;

/// Checks whether Type is a trivial type (i.e., a type that can be trivially constructed, copied, and destroyed).
template <typename Type>
using IsTrivial = meta::IsTrivial<Type>;
template <typename Type>
inline constexpr bool IsTrivialV = IsTrivial<Type>::value;

/// Checks whether Type is trivially copyable (i.e., a type that can be copied with a simple bitwise copy 
/// and does not require a user-defined copy constructor, copy assignment operator, or destructor).
template <typename Type>
using IsTriviallyCopyable = meta::IsTriviallyCopyable<Type>;
template <typename Type>
inline constexpr bool IsTriviallyCopyableV = IsTriviallyCopyable<Type>::value;

/// Checks whether Type is a standard layout type (i.e., a type that has a standard memory layout 
/// and can be used in C-style APIs).
template <typename Type>
using IsStandardLayout = meta::IsStandardLayout<Type>;
template <typename Type>
inline constexpr bool IsStandardLayoutV = IsStandardLayout<Type>::value;

/// Checks whether Type is an empty class (i.e., a class with no non-static data members).
template <typename Type>
using IsEmpty = meta::IsEmpty<Type>;
template <typename Type>
inline constexpr bool IsEmptyV = IsEmpty<Type>::value;

/// Checks whether Type is a polymorphic class (i.e., a class that has at least one virtual function).
template <typename Type>
using IsPolymorphic = meta::IsPolymorphic<Type>;
template <typename Type>
inline constexpr bool IsPolymorphicV = IsPolymorphic<Type>::value;

/// Checks whether Type is an abstract class (i.e., a class that cannot be instantiated).
template <typename Type>
using IsAbstract = meta::IsAbstract<Type>;
template <typename Type>
inline constexpr bool IsAbstractV = IsAbstract<Type>::value;

/// Checks whether Type is a final class (i.e., a class that cannot be inherited from).
template <typename Type>
using IsFinal = meta::IsFinal<Type>;
template <typename Type>
inline constexpr bool IsFinalV = IsFinal<Type>::value;

/// Checks whether Type is an aggregate type (i.e., a type that can be initialized with aggregate initialization).
template <typename Type>
using IsAggregate = NEX_STD is_aggregate<Type>;
template <typename Type>
inline constexpr bool IsAggregateV = IsAggregate<Type>::value;

/// Removes the const qualifier from a Type.
template <typename Type>
using RemoveConst = meta::RemoveConst<Type>;
template <typename Type>
using RemoveConstT = typename RemoveConst<Type>::type;

/// Removes the volatile qualifier from a Type.
template <typename Type>
using RemoveVolatile = meta::RemoveVolatile<Type>;
template <typename Type>
using RemoveVolatileT = typename RemoveVolatile<Type>::type;

/// Removes const and volatile qualifiers from a Type.
template <typename Type>
using RemoveCv = meta::RemoveCv<Type>;
template <typename Type>
using RemoveCvT = typename RemoveCv<Type>::type;

/// Removes reference qualifiers from a Type.
template <typename Type>
using RemoveReference = meta::RemoveReference<Type>;
template <typename Type>
using RemoveReferenceT = typename RemoveReference<Type>::type;

/// Removes const, volatile, and reference qualifiers from a Type.
template <typename Type>
using RemoveCvref = meta::RemoveCvref<Type>;
template <typename Type>
using RemoveCvrefT = typename RemoveCvref<Type>::type;

/// Removes the pointer qualifier from a Type.
template <typename Type>
using RemovePointer = meta::RemovePointer<Type>;
template <typename Type>
using RemovePointerT = typename RemovePointer<Type>::type;

/// Removes one level of array extent from a Type.
template <typename Type>
using RemoveExtent = meta::RemoveExtent<Type>;
template <typename Type>
using RemoveExtentT = typename RemoveExtent<Type>::type;

/// Removes all levels of array extent from a Type.
template <typename Type>
using RemoveAllExtents = meta::RemoveAllExtents<Type>;
template <typename Type>
using RemoveAllExtentsT = typename RemoveAllExtents<Type>::type;

/// Adds the const qualifier to a Type.
template <typename Type>
using AddConst = meta::AddConst<Type>;
template <typename Type>
using AddConstT = typename AddConst<Type>::type;

/// Adds the volatile qualifier to a Type.
template <typename Type>
using AddVolatile = meta::AddVolatile<Type>;
template <typename Type>
using AddVolatileT = typename AddVolatile<Type>::type;

/// Adds const and volatile qualifiers to a Type.
template <typename Type>
using AddCv = meta::AddCv<Type>;
template <typename Type>
using AddCvT = typename AddCv<Type>::type;

/// Adds an lvalue reference to a Type.
template <typename Type>
using AddLvalueReference = meta::AddLvalueReference<Type>;
template <typename Type>
using AddLvalueReferenceT = typename AddLvalueReference<Type>::type;

/// Adds an rvalue reference to a Type.
template <typename Type>
using AddRvalueReference = meta::AddRvalueReference<Type>;
template <typename Type>
using AddRvalueReferenceT = typename AddRvalueReference<Type>::type;

/// Adds a pointer qualifier to a Type.
template <typename Type>
using AddPointer = meta::AddPointer<Type>;
template <typename Type>
using AddPointerT = typename AddPointer<Type>::type;

/// Converts an integral Type to its corresponding signed type.
template <typename Type>
using MakeSigned = meta::MakeSigned<Type>;
template <typename Type>
using MakeSignedT = typename MakeSigned<Type>::type;

/// Converts an integral Type to its corresponding unsigned type.
template <typename Type>
using MakeUnsigned = meta::MakeUnsigned<Type>;
template <typename Type>
using MakeUnsignedT = typename MakeUnsigned<Type>::type;

/// A type alias for the decayed type of a given Type.
template <typename Type>
using Decay = meta::Decay<Type>;
template <typename Type>
using DecayT = typename Decay<Type>::type;

/// Provides the identity of a Type (i.e., it simply returns the same type).
template <typename Type>
using TypeIdentity = meta::TypeIdentity<Type>;
template <typename Type>
using TypeIdentityT = typename TypeIdentity<Type>::type;

/// Represents the void type for a given list of types. 
/// It is commonly used in SFINAE (Substitution Failure Is Not An Error) contexts to detect the presence 
/// of certain types or expressions.
template <typename... Types>
using VoidT = meta::VoidT<Types...>;

/// Represents the common type among a list of types. 
/// The common type is the type that all types in the list can be implicitly converted to.
template <typename... Types>
using CommonType = NEX_STD common_type_t<Types...>;

/// Represents the common reference type among a list of types. 
/// The common reference type is the type that all types in the list can be implicitly converted to 
/// when treated as references.
template <typename... Types>
using CommonReference = NEX_STD common_reference_t<Types...>;

/// Represents one of two types (TrueType or FalseType) based on a boolean condition. 
/// If Condition is true, Conditional evaluates to TrueType; otherwise, it evaluates to FalseType.
template <bool Condition, typename TrueType, typename FalseType>
using Conditional = meta::ConditionalT<Condition, TrueType, FalseType>;

/// Represents the logical conjunction (AND) of a list of type traits Traits. 
/// Conjunction evaluates to true if all traits in the list are true; otherwise, it evaluates to false.
template <typename... Traits>
using Conjunction = meta::Conjunction<Traits...>;
template <typename... Traits>
inline constexpr bool ConjunctionV = Conjunction<Traits...>::value;

/// Represents the logical disjunction (OR) of a list of type traits Traits. 
/// Disjunction evaluates to true if at least one trait in the list is true; otherwise, it evaluates to false.
template <typename... Traits>
using Disjunction = meta::Disjunction<Traits...>;
template <typename... Traits>
inline constexpr bool DisjunctionV = Disjunction<Traits...>::value;

/// Represents the logical negation (NOT) of a type trait Trait. 
/// Negation evaluates to true if Trait is false; otherwise, it evaluates to false.
template <typename Trait>
using Negation = meta::Negation<Trait>;
template <typename Trait>
inline constexpr bool NegationV = Negation<Trait>::value;

/// A type trait that enables a template only if a boolean condition is true.
template <bool Condition, typename Type = void>
using EnableIf = meta::EnableIfT<Condition, Type>;

/// A type trait that enables a template only if the Type is a numeric type (arithmetic type).
template <typename Type>
using RequiresNumeric = EnableIf<Arithmetic<RemoveCvref<Type>>>;

/// A type trait that enables a template only if the Type is an integral type.
template <typename Type>
using RequiresIntegral = EnableIf<Integral<RemoveCvref<Type>>>;

/// A type trait that enables a template only if the Type is a floating-point type.
template <typename Type>
using RequiresFloatingPoint = EnableIf<FloatingPoint<RemoveCvref<Type>>>;

/// A type trait that enables a template only if the Type is a pointer type.
template <typename Type>
using RequiresPointer = EnableIf<RawPointer<Type>>;

/// A type trait that enables a template only if the Type is an enumeration type (i.e., an enum).
template <typename Type>
using RequiresEnum = EnableIf<Enum<Type>>;

/// A type trait that enables a template only if the Type is a class type (i.e., a struct or class).
template <typename Type>
using RequiresClass = EnableIf<Class<Type>>;

/// A type alias for the result type of invoking a callable with specific arguments.
template<typename Fn, typename... Args>
using InvokeResult = NEX_STD invoke_result_t<Fn, Args...>;

/// A type alias for the underlying type of an enumeration type.
template <typename Type>
using UnderlyingType = meta::UnderlyingType<Type>;
template <typename Type>
using UnderlyingTypeT = typename UnderlyingType<Type>::type;

/// Checks whether Fn can be invoked with Args.
template <typename Fn, typename... Args>
using IsInvocable = BoolConstant<Invocable<Fn, Args...>>;
template <typename Fn, typename... Args>
inline constexpr bool IsInvocableV = IsInvocable<Fn, Args...>::value;

/// Checks whether Fn can be invoked with Args and returns a type convertible to Return.
template <typename Return, typename Fn, typename... Args>
using IsInvocableR = NEX_STD is_invocable_r<Return, Fn, Args...>;
template <typename Return, typename Fn, typename... Args>
inline constexpr bool IsInvocableRV = IsInvocableR<Return, Fn, Args...>::value;

/// Checks whether Fn can be invoked with Args without throwing.
template <typename Fn, typename... Args>
using IsNothrowInvocable = NEX_STD is_nothrow_invocable<Fn, Args...>;
template <typename Fn, typename... Args>
inline constexpr bool IsNothrowInvocableV = IsNothrowInvocable<Fn, Args...>::value;

/// Checks whether Fn can be invoked with Args and returns a type convertible to Return without throwing.
template <typename Return, typename Fn, typename... Args>
using IsNothrowInvocableR = NEX_STD is_nothrow_invocable_r<Return, Fn, Args...>;
template <typename Return, typename Fn, typename... Args>
inline constexpr bool IsNothrowInvocableRV = IsNothrowInvocableR<Return, Fn, Args...>::value;

/// A type alias for the resolved return type of invoking a callable with specific arguments.
template <typename Fn, typename... Args>
using ResolvedReturnType = DecayT<InvokeResult<Fn, Args...>>;

/// Extracts the element type from a container Type.
template <typename Type>
using Element = typename RemoveCvref<Type>::value_type;

/// Extracts the type of a member from a pointer to member.
template <typename MemberPtr>
struct MemberPointerTraits;

/// A specialization of MemberPointerTraits for pointers to members, 
/// which extracts the class type and member type.
template <typename Type, typename MemberT>
struct MemberPointerTraits<MemberT Type::*> {
    using ClassType = Type;
    using ValueType = MemberT;
};

/// Extracts the class type from a pointer to member.
template <typename MemberPtr>
using MemberClass = typename MemberPointerTraits<MemberPtr>::ClassType;

/// Extracts the member type from a pointer to member.
template <typename MemberPtr>
using MemberType = typename MemberPointerTraits<MemberPtr>::ValueType;

/// Represents a pointer to a member within a struct/class.
template <typename Type, typename MemberT>
using MemberPointer = MemberT Type::*;

/// Represents a reference to a member within a struct/class.
template <typename Type, auto MemberPtr>
using MemberReference = decltype(meta::declval<Type&>().*MemberPtr);

/// Represents a const reference to a member within a struct/class.
template <typename Type, auto MemberPtr>
using ConstMemberReference = decltype(meta::declval<const Type&>().*MemberPtr);

/// Represents an rvalue reference to a member within a struct/class.
template <typename Type, auto MemberPtr>
using MemberRvalueReference = decltype(meta::declval<Type&&>().*MemberPtr);

/// Checks whether Type is callable with specific argument types.
template <typename Type, typename... Args>
inline constexpr bool IsCallable = Invocable<Type, Args...>;

/// Checks whether Type is iterable (i.e., has begin() and end() functions).
template <typename Type, typename = void>
struct IsIterable : BoolConstant<Iterable<Type>> {};

template <typename Type>
inline constexpr bool IsIterableV = IsIterable<Type>::value;

/// Extracts information about a function type, such as its return type and argument types. 
/// This primary template is designed to handle callable objects (e.g., lambdas, functors) 
/// by deducing their operator() member function.
template <typename Type>
struct FunctionTraits : FunctionTraits<decltype(&RemoveReference<Type>::operator())> {};

/// Extracts information about a function type, such as its return type and argument types.
template <typename R, typename... Args>
struct FunctionTraits<R(Args...)> {
    using ReturnType = R;
    static constexpr usize argCount = sizeof...(Args);

    template <usize N>
    using Arg = NEX_STD tuple_element_t<N, NEX_STD tuple<Args...>>;
};

/// A specialization of FunctionTraits for function pointer types, 
/// which inherits from the primary template to extract the return type and argument types.
template <typename R, typename... Args>
struct FunctionTraits<R(*)(Args...)> : FunctionTraits<R(Args...)> {};

/// A specialization of FunctionTraits for function reference types, 
/// which inherits from the primary template to extract the return type and argument types.
template <typename R, typename... Args>
struct FunctionTraits<R(&)(Args...)> : FunctionTraits<R(Args...)> {};

/// A specialization of FunctionTraits for rvalue function reference types, 
/// which inherits from the primary template to extract the return type and argument types.
template <typename R, typename... Args>
struct FunctionTraits<R(&&)(Args...)> : FunctionTraits<R(Args...)> {};

/// A specialization of FunctionTraits for member function pointer types, 
/// which inherits from the primary template to extract the return type and argument types, 
/// and also defines the ClassType representing the class that the member function belongs to.
template <typename Class, typename R, typename... Args>
struct FunctionTraits<R(Class::*)(Args...)> : FunctionTraits<R(Args...)> {
    using ClassType = Class;
};

/// A specialization of FunctionTraits for const member function pointer types, 
/// which inherits from the primary template to extract the return type and argument types, 
/// and also defines the ClassType representing the class that the member function belongs to.
template <typename Class, typename R, typename... Args>
struct FunctionTraits<R(Class::*)(Args...) const> : FunctionTraits<R(Args...)> {
    using ClassType = Class;
};

/// A specialization of FunctionTraits for noexcept member function pointer types, 
/// which inherits from the primary template to extract the return type and argument types, 
/// and also defines the ClassType representing the class that the member function belongs to.
template <typename Class, typename R, typename... Args>
struct FunctionTraits<R(Class::*)(Args...) noexcept> : FunctionTraits<R(Args...)> {
    using ClassType = Class;
};

/// A specialization of FunctionTraits for const noexcept member function pointer types, 
/// which inherits from the primary template to extract the return type and argument types, 
/// and also defines the ClassType representing the class that the member function belongs to.
template <typename Class, typename R, typename... Args>
struct FunctionTraits<R(Class::*)(Args...) const noexcept> : FunctionTraits<R(Args...)> {
    using ClassType = Class;
};

/// Represents the return type of a function Fn.
template <typename Fn>
using FunctionReturnType = typename FunctionTraits<Fn>::ReturnType;

/// Represents the type of the N-th argument of a function Fn.
template <typename Fn, usize N>
using FunctionArg = typename FunctionTraits<Fn>::template Arg<N>;

/// Represents the number of arguments of a function Fn.
template <typename Fn>
inline constexpr usize FunctionArgCount = FunctionTraits<Fn>::argCount;

/// Extracts information about an iterator type, such as its value type, reference type, pointer type, 
/// difference type, and iterator category.
template <typename Ip>
struct IteratorTraits {
    using ValueType = typename NEX_STD iterator_traits<Ip>::value_type;
    using Reference = typename NEX_STD iterator_traits<Ip>::reference;
    using Pointer = typename NEX_STD iterator_traits<Ip>::pointer;
    using DifferenceType = typename NEX_STD iterator_traits<Ip>::difference_type;
    using IteratorCategory = typename NEX_STD iterator_traits<Ip>::iterator_category;
};

/// Represents the value type of an iterator Ip.
template <typename Ip>
using IteratorValueType = typename IteratorTraits<Ip>::ValueType;

/// Represents the reference type of an iterator Ip.
template <typename Ip>
using IteratorReference = typename IteratorTraits<Ip>::Reference;

/// Represents the pointer type of an iterator Ip.
template <typename Ip>
using IteratorPointer = typename IteratorTraits<Ip>::Pointer;

/// Represents the difference type of an iterator Ip.
template <typename Ip>
using IteratorDifferenceType = typename IteratorTraits<Ip>::DifferenceType;

/// Represents the iterator category of an iterator Ip.
template <typename Ip>
using IteratorCategory = typename IteratorTraits<Ip>::IteratorCategory;

/// Represents the standard iterator traits (STL) for an iterator type Ip.
template <typename Ip>
using StdIteratorTraits = NEX_STD iterator_traits<Ip>;

/// Represents the value type of an iterator Ip.
template <typename Ip>
using StdIteratorValueType = typename StdIteratorTraits<Ip>::value_type;

/// Represents the reference type of an iterator Ip.
template <typename Ip>
using StdIteratorReference = typename StdIteratorTraits<Ip>::reference;

/// Represents the pointer type of an iterator Ip.
template <typename Ip>
using StdIteratorPointer = typename StdIteratorTraits<Ip>::pointer;

/// Represents the difference type of an iterator Ip.
template <typename Ip>
using StdIteratorDifferenceType = typename StdIteratorTraits<Ip>::difference_type;

/// Represents the iterator category of an iterator Ip.
template <typename Ip>
using StdIteratorCategory = typename StdIteratorTraits<Ip>::iterator_category;

NEX_NAMESPACE_END
