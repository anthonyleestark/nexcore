/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  type_traits.h
 * @brief A collection of type traits and utilities for working with types in C++.
 *
 * @details
 * This header provides a collection of type traits and utilities that simplify the use of standard
 * C++ type traits. It includes aliases for common type traits, as well as additional utilities for
 * working with types in a more concise and readable manner. The traits defined in this header can be
 * used to perform compile-time checks on types, manipulate types, and enable or disable template
 * instantiations based on type properties.
 */

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/casts.h"
#include "nex/base/invoke.h"
#include "nex/base/utility.h"
#include "nex/base/fwd.h"
#include "nex/base/type_concepts.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief IntegralConstant is a template that represents a compile-time constant value of a specified type.
 *        It is the basis for other type traits like BoolConstant, TrueType, and FalseType.
 * @tparam Type The type of the constant value.
 * @tparam Value The constant value of the specified type.
 */
template <class Type, Type Value>
using IntegralConstant = meta::IntegralConstant<Type, Value>;

/**
 * @brief BoolConstant is a specialization of IntegralConstant for boolean values, providing a convenient way
 *        to represent compile-time boolean constants.
 * @tparam BoolValue The boolean constant value.
 */
template <bool BoolValue>
using BoolConstant = meta::BoolConstant<BoolValue>;

/**
 * @brief Represents the compile-time constant value `true`.
 * @note This can be used in template metaprogramming to indicate a true condition.
 */
using TrueType = meta::TrueType;

/**
 * @brief Represents the compile-time constant value `false`.
 * @note This can be used in template metaprogramming to indicate a false condition.
 */
using FalseType = meta::FalseType;

/**
 * @brief AlwaysFalse is a helper type trait that always evaluates to false,
 *        used to delay static_assert until template instantiation.
 * @tparam Args Variadic template parameters (not used in the evaluation).
 */
template <typename ...Args>
using AlwaysFalse = meta::AlwaysFalse<Args...>;

/**
 * @brief Variable template that is always false.
 * @tparam Args Variadic template parameters (not used in the evaluation).
 */
template <typename ...Args>
inline constexpr bool AlwaysFalseV = meta::AlwaysFalseV<Args...>;

/**
 * @brief Determines whether the specified type is an integral type (e.g., int32, int64, nchar, etc.).
 * @tparam Type The specified type to check.
 */
template <class Type>
using IsIntegral = BoolConstant<meta::IsIntegralV<Type>>;

/**
 * @brief Variable template that is true when the specified type is an integral type.
 * @tparam Type The specified type to check.
 */
template <class Type>
inline constexpr bool IsIntegralV = IsIntegral<Type>::value;

/**
 * @brief Determines whether the specified type is a floating-point type (i.e., float32, float64, or ldouble).
 * @tparam Type The specified type to check.
 */
template <class Type>
using IsFloatingPoint = BoolConstant<meta::IsFloatingPointV<Type>>;

/**
 * @brief Variable template that is true when the specified type is a floating-point type.
 * @tparam Type The specified type to check.
 */
template <class Type>
inline constexpr bool IsFloatingPointV = IsFloatingPoint<Type>::value;

/**
 * @brief Determines whether the specified type is an arithmetic type (i.e., integral or floating-point).
 * @tparam Type The specified type to check.
 */
template <class Type>
using IsArithmetic = BoolConstant<Arithmetic<Type>>;

/**
 * @brief Variable template that is true when the specified type is an arithmetic type.
 * @tparam Type The specified type to check.
 */
template <class Type>
inline constexpr bool IsArithmeticV = IsArithmetic<Type>::value;

/**
 * @brief Determines whether the specified type is a literal type (i.e., a type that can be used
 *        in constant expressions).
 * @tparam Type The specified type to check.
 */
template <class Type>
using IsLiteralType = meta::IsLiteralType<Type>;

/**
 * @brief Variable template that is true when the specified type is a literal type.
 * @tparam Type The specified type to check.
 */
template <class Type>
inline constexpr bool IsLiteralTypeV = IsLiteralType<Type>::value;

/**
 * @brief Determines whether the specified type is the void type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsVoid = meta::IsVoid<Type>;

/**
 * @brief Variable template that is true when the specified type is the void type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsVoidV = meta::IsVoidV<Type>;

/**
 * @brief Determines whether the specified type is the nullptr type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsNullPointer = meta::IsNullPointer<Type>;

/**
 * @brief Variable template that is true when the specified type is the nullptr type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsNullPointerV = meta::IsNullPointerV<Type>;

/**
 * @brief Determines whether the specified type is a pointer type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsPointer = BoolConstant<meta::IsPointerV<Type>>;

/**
 * @brief Variable template that is true when the specified type is a pointer type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsPointerV = IsPointer<Type>::value;

/**
 * @brief Determines whether the specified type is an array type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsArray = meta::IsArray<Type>;

/**
 * @brief Variable template that is true when the specified type is an array type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsArrayV = IsArray<Type>::value;

/**
 * @brief Determines whether the specified type is a bounded array type (i.e., an array with a known size).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsBoundedArray = meta::IsBoundedArray<Type>;

/**
 * @brief Variable template that is true when the specified type
 *        is a bounded array type (i.e., an array with a known size).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsBoundedArrayV = IsBoundedArray<Type>::value;

/**
 * @brief Determines whether the specified type is an unbounded array type (i.e., an array
 *        with an unknown size).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsUnboundedArray = meta::IsUnboundedArray<Type>;

/**
 * @brief Variable template that is true when the specified type is an unbounded array type
 *        (i.e., an array with an unknown size).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsUnboundedArrayV = IsUnboundedArray<Type>::value;

/**
 * @brief Determines whether the specified type is an enumeration type (i.e., an enum).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsEnum = meta::IsEnum<Type>;

/**
 * @brief Variable template that is true when the specified type is an enumeration type (i.e., an enum).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsEnumV = IsEnum<Type>::value;

/**
 * @brief Determines whether the specified type is a scoped enumeration type (i.e., an enum class).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsEnumClass = meta::IsEnumClass<Type>;

/**
 * @brief Variable template that is true when the specified type is a scoped enumeration type (i.e., an enum class).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsEnumClassV = IsEnumClass<Type>::value;

/**
 * @brief Determines whether the specified type is a union type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsUnion = meta::IsUnion<Type>;

/**
 * @brief Variable template that is true when the specified type is a union type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsUnionV = IsUnion<Type>::value;

/**
 * @brief Determines whether the specified type is a monostate type (i.e., a type that has only one possible value).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsMonostate = meta::IsSame<Type, monostate>;

/**
 * @brief Variable template that is true when the specified type is a monostate type (i.e., a type
 *        that has only one possible value).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsMonostateV = IsMonostate<Type>::value;

/**
 * @brief Determines whether the specified type is a class type (i.e., a struct or class).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsClass = meta::IsClass<Type>;

/**
 * @brief Variable template that is true when the specified type is a class type (i.e., a struct or class).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsClassV = IsClass<Type>::value;

/**
 * @brief Determines whether the specified type is a function type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsFunction = meta::IsFunction<Type>;

/**
 * @brief Variable template that is true when the specified type is a function type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsFunctionV = IsFunction<Type>::value;

/**
 * @brief Determines whether the specified type is a pointer to a member of a class or struct.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsMemberPointer = meta::IsMemberPointer<Type>;

/**
 * @brief Variable template that is true when the specified type is a pointer to a member of a class or struct.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsMemberPointerV = IsMemberPointer<Type>::value;

/**
 * @brief Determines whether the specified type is a pointer to a non-function member of a class or struct.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsMemberObjectPointer = meta::IsMemberObjectPointer<Type>;

/**
 * @brief Variable template that is true when the specified type is a pointer
 *        to a non-function member of a class or struct.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsMemberObjectPointerV = IsMemberObjectPointer<Type>::value;

/**
 * @brief Determines whether the specified type is a pointer to a member function of a class or struct.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsMemberFunctionPointer = meta::IsMemberFunctionPointer<Type>;

/**
 * @brief Variable template that is true when the specified type is a pointer
 *        to a member function of a class or struct.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsMemberFunctionPointerV = IsMemberFunctionPointer<Type>::value;

/**
 * @brief Determines whether the specified type is an lvalue reference type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsLvalueReference = meta::IsLvalueReference<Type>;

/**
 * @brief Variable template that is true when the specified type is an lvalue reference type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsLvalueReferenceV = IsLvalueReference<Type>::value;

/**
 * @brief Determines whether the specified type is an rvalue reference type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsRvalueReference = meta::IsRvalueReference<Type>;

/**
 * @brief Variable template that is true when the specified type is an rvalue reference type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsRvalueReferenceV = IsRvalueReference<Type>::value;

/**
 * @brief Determines whether the specified type is a reference type (either lvalue or rvalue reference).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsReference = meta::IsReference<Type>;

/**
 * @brief Variable template that is true when the specified type is
 *        a reference type (either lvalue or rvalue reference).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsReferenceV = IsReference<Type>::value;

/**
 * @brief Determines whether the specified type is a fundamental type
 *        (i.e., arithmetic types, void, nullptr_t, etc.).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsFundamental = meta::IsFundamental<Type>;

/**
 * @brief Variable template that is true when the specified type is a fundamental type
 *        (i.e., arithmetic types, void, nullptr_t, etc.).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsFundamentalV = IsFundamental<Type>::value;

/**
 * @brief Determines whether the specified type is an object type
 *        (i.e., a type that is not a function, reference, or void).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsObject = meta::IsObject<Type>;

/**
 * @brief Variable template that is true when the specified type is an object type
 *        (i.e., a type that is not a function, reference, or void).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsObjectV = IsObject<Type>::value;

/**
 * @brief Determines whether the specified type is a scalar type (i.e., an arithmetic type,
 *        enumeration type, pointer type, or pointer to member type).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsScalar = meta::IsScalar<Type>;

/**
 * @brief Variable template that is true when the specified type is a scalar type
 *        (i.e., an arithmetic type, enumeration type, pointer type, or pointer to member type).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsScalarV = IsScalar<Type>::value;

/**
 * @brief Determines whether the specified type is a compound type (i.e., an array, class, union, or enumeration type).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsCompound = meta::IsCompound<Type>;

/**
 * @brief Variable template that is true when the specified type is a compound type
 *        (i.e., an array, class, union, or enumeration type).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsCompoundV = IsCompound<Type>::value;

/**
 * @brief Determines whether the specified type is a const-qualified type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsConst = meta::IsConst<Type>;

/**
 * @brief Variable template that is true when the specified type is a const-qualified type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsConstV = IsConst<Type>::value;

/**
 * @brief Determines whether the specified type is a volatile-qualified type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsVolatile = meta::IsVolatile<Type>;

/**
 * @brief Variable template that is true when the specified type is a volatile-qualified type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsVolatileV = IsVolatile<Type>::value;

/**
 * @brief Determines whether the specified type is a signed integral type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsSigned = meta::IsSignedIntegral<Type>;

/**
 * @brief Variable template that is true when the specified type is a signed integral type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsSignedV = IsSigned<Type>::value;

/**
 * @brief Determines whether the specified type is an unsigned integral type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsUnsigned = meta::IsUnsignedIntegral<Type>;

/**
 * @brief Variable template that is true when the specified type is an unsigned integral type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsUnsignedV = IsUnsigned<Type>::value;

/**
 * @brief Determines whether the specified types are the same type.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
using IsSame = meta::IsSame<Type1, Type2>;

/**
 * @brief Variable template that is true when the specified types are the same type.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
inline constexpr bool IsSameV = IsSame<Type1, Type2>::value;

/**
 * @brief Determines whether the specified type is a specialization of the given class template.
 * @tparam Type The specified type to check.
 * @tparam Template The class template to check against.
 */
template <typename Type, template <typename...> class Template>
using IsSpecialization = meta::IsSpecialization<Type, Template>;

/**
 * @brief Variable template that is true when the specified type is a specialization of the given class template.
 * @tparam Type The specified type to check.
 * @tparam Template The class template to check against.
 */
template <typename Type, template <typename...> class Template>
inline constexpr bool IsSpecializationV = IsSpecialization<Type, Template>::value;

/**
 * @brief Determines whether the specified type is a base class of another type.
 * @tparam Base The potential base class type.
 * @tparam Derived The potential derived class type.
 */
template <typename Base, typename Derived>
using IsBaseOf = meta::IsBaseOf<Base, Derived>;

/**
 * @brief Variable template that is true when the specified type is a base class of another type.
 * @tparam Base The potential base class type.
 * @tparam Derived The potential derived class type.
 */
template <typename Base, typename Derived>
inline constexpr bool IsBaseOfV = IsBaseOf<Base, Derived>::value;

/**
 * @brief Determines whether the specified type can be implicitly converted to another type.
 * @tparam From The source type.
 * @tparam To The target type.
 */
template <typename From, typename To>
using IsConvertible = meta::IsConvertible<From, To>;

/**
 * @brief Variable template that is true when the specified type can be implicitly converted to another type.
 * @tparam From The source type.
 * @tparam To The target type.
 */
template <typename From, typename To>
inline constexpr bool IsConvertibleV = IsConvertible<From, To>::value;

/**
 * @brief Determines whether the specified type can be constructed with the given argument types.
 * @tparam Type The specified type to check.
 * @tparam Args The argument types to check.
 */
template <typename Type, typename... Args>
using IsConstructible = meta::IsConstructible<Type, Args...>;

/**
 * @brief Variable template that is true when the specified type can be constructed with the given argument types.
 * @tparam Type The specified type to check.
 * @tparam Args The argument types to check.
 */
template <typename Type, typename... Args>
inline constexpr bool IsConstructibleV = IsConstructible<Type, Args...>::value;

/**
 * @brief Determines whether the specified type can be trivially constructed with the given argument types.
 * @tparam Type The specified type to check.
 * @tparam Args The argument types to check.
 */
template <typename Type, typename... Args>
using IsTriviallyConstructible = meta::IsTriviallyConstructible<Type, Args...>;

/**
 * @brief Variable template that is true when the specified type can be trivially constructed
 *        with the given argument types.
 * @tparam Type The specified type to check.
 * @tparam Args The argument types to check.
 */
template <typename Type, typename... Args>
inline constexpr bool IsTriviallyConstructibleV = IsTriviallyConstructible<Type, Args...>::value;

/**
 * @brief Determines whether the specified type can be constructed with the given argument types without throwing.
 * @tparam Type The specified type to check.
 * @tparam Args The argument types to check.
 */
template <typename Type, typename... Args>
using IsNothrowConstructible = meta::IsNothrowConstructible<Type, Args...>;

/**
 * @brief Variable template that is true when the specified type can be constructed
 *        with the given argument types without throwing.
 * @tparam Type The specified type to check.
 * @tparam Args The argument types to check.
 */
template <typename Type, typename... Args>
inline constexpr bool IsNothrowConstructibleV = IsNothrowConstructible<Type, Args...>::value;

/**
 * @brief Determines whether the specified type can be default constructed.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsDefaultConstructible = meta::IsDefaultConstructible<Type>;

/**
 * @brief Variable template that is true when the specified type can be default constructed.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsDefaultConstructibleV = IsDefaultConstructible<Type>::value;

/**
 * @brief Determines whether the specified type can be trivially default constructed.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsTriviallyDefaultConstructible = meta::IsTriviallyDefaultConstructible<Type>;

/**
 * @brief Variable template that is true when the specified type can be trivially default constructed.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsTriviallyDefaultConstructibleV = IsTriviallyDefaultConstructible<Type>::value;

/**
 * @brief Determines whether the specified type can be default constructed without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsNothrowDefaultConstructible = meta::IsNothrowDefaultConstructible<Type>;

/**
 * @brief Variable template that is true when the specified type can be default constructed without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsNothrowDefaultConstructibleV = IsNothrowDefaultConstructible<Type>::value;

/**
 * @brief Determines whether the specified type is copy constructible.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsCopyable = meta::IsCopyConstructible<Type>;

/**
 * @brief Variable template that is true when the specified type is copy constructible.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsCopyableV = IsCopyable<Type>::value;

/**
 * @brief Determines whether the specified type has a trivial copy constructor.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsTriviallyCopyableConstructible = meta::IsTriviallyCopyConstructible<Type>;

/**
 * @brief Variable template that is true when Type has a trivial copy constructor.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsTriviallyCopyableConstructibleV = IsTriviallyCopyableConstructible<Type>::value;

/**
 * @brief Determines whether the specified type is copy constructible without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsNothrowCopyConstructible = meta::IsNothrowCopyConstructible<Type>;

/**
 * @brief Variable template that is true when the specified type is copy constructible without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsNothrowCopyConstructibleV = IsNothrowCopyConstructible<Type>::value;

/**
 * @brief Determines whether the specified type is move constructible.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsMoveConstructible = meta::IsMoveConstructible<Type>;

/**
 * @brief Variable template that is true when the specified type is move constructible.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsMoveConstructibleV = IsMoveConstructible<Type>::value;

/**
 * @brief Determines whether the specified type is movable (i.e., can be move constructed).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsMovable = IsMoveConstructible<Type>;

/**
 * @brief Variable template that is true when the specified type is movable (i.e., can be move constructed).
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsMovableV = IsMoveConstructibleV<Type>;

/**
 * @brief Determines whether the specified type has a trivial move constructor.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsTriviallyMovableConstructible = meta::IsTriviallyMoveConstructible<Type>;

/**
 * @brief Variable template that is true when Type has a trivial move constructor.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsTriviallyMovableConstructibleV = IsTriviallyMovableConstructible<Type>::value;

/**
 * @brief Determines whether the specified type is move constructible without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsNothrowMoveConstructible = meta::IsNothrowMoveConstructible<Type>;

/**
 * @brief Variable template that is true when the specified type is move constructible without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsNothrowMoveConstructibleV = IsNothrowMoveConstructible<Type>::value;

/**
 * @brief Determines whether the specified type can be assigned from another type.
 * @tparam To The specified type to assign to.
 * @tparam From The specified type to assign from.
 */
template <typename To, typename From = To>
using IsAssignable = meta::IsAssignable<To, From>;

/**
 * @brief Variable template that is true when the specified type can be assigned from another type.
 * @tparam To The specified type to assign to.
 * @tparam From The specified type to assign from.
 */
template <typename To, typename From = To>
inline constexpr bool IsAssignableV = IsAssignable<To, From>::value;

/**
 * @brief Determines whether the specified type can be trivially assigned from another type.
 * @tparam To The specified type to assign to.
 * @tparam From The specified type to assign from.
 */
template <typename To, typename From = To>
using IsTriviallyAssignable = meta::IsTriviallyAssignable<To, From>;

/**
 * @brief Variable template that is true when the specified type can be trivially assigned from another type.
 * @tparam To The specified type to assign to.
 * @tparam From The specified type to assign from.
 */
template <typename To, typename From = To>
inline constexpr bool IsTriviallyAssignableV = IsTriviallyAssignable<To, From>::value;

/**
 * @brief Determines whether the specified type can be assigned from another type without throwing.
 * @tparam To The specified type to assign to.
 * @tparam From The specified type to assign from.
 */
template <typename To, typename From = To>
using IsNothrowAssignable = meta::IsNothrowAssignable<To, From>;

/**
 * @brief Variable template that is true when the specified type can be assigned from another type without throwing.
 * @tparam To The specified type to assign to.
 * @tparam From The specified type to assign from.
 */
template <typename To, typename From = To>
inline constexpr bool IsNothrowAssignableV = IsNothrowAssignable<To, From>::value;

/**
 * @brief Determines whether the specified type can be copy assigned.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsCopyAssignable = meta::IsCopyAssignable<Type>;

/**
 * @brief Variable template that is true when the specified type can be copy assigned.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsCopyAssignableV = IsCopyAssignable<Type>::value;

/**
 * @brief Determines whether the specified type can be trivially copy assigned.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsTriviallyCopyAssignable = meta::IsTriviallyCopyAssignable<Type>;

/**
 * @brief Variable template that is true when the specified type can be trivially copy assigned.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsTriviallyCopyAssignableV = IsTriviallyCopyAssignable<Type>::value;

/**
 * @brief Determines whether the specified type can be copy assigned without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsNothrowCopyAssignable = meta::IsNothrowCopyAssignable<Type>;

/**
 * @brief Variable template that is true when the specified type can be copy assigned without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsNothrowCopyAssignableV = IsNothrowCopyAssignable<Type>::value;

/**
 * @brief Determines whether the specified type can be move assigned.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsMoveAssignable = meta::IsMoveAssignable<Type>;

/**
 * @brief Variable template that is true when the specified type can be move assigned.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsMoveAssignableV = IsMoveAssignable<Type>::value;

/**
 * @brief Determines whether the specified type can be trivially move assigned.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsTriviallyMoveAssignable = meta::IsTriviallyMoveAssignable<Type>;

/**
 * @brief Variable template that is true when the specified type can be trivially move assigned.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsTriviallyMoveAssignableV = IsTriviallyMoveAssignable<Type>::value;

/**
 * @brief Determines whether the specified type can be move assigned without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsNothrowMoveAssignable = meta::IsNothrowMoveAssignable<Type>;

/**
 * @brief Variable template that is true when the specified type can be move assigned without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsNothrowMoveAssignableV = IsNothrowMoveAssignable<Type>::value;

/**
 * @brief Determines whether the specified type can be destructed.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsDestructible = meta::IsDestructible<Type>;

/**
 * @brief Variable template that is true when the specified type can be destructed.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsDestructibleV = IsDestructible<Type>::value;

/**
 * @brief Determines whether the specified type can be trivially destructed.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsTriviallyDestructible = meta::IsTriviallyDestructible<Type>;

/**
 * @brief Variable template that is true when the specified type can be trivially destructed.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsTriviallyDestructibleV = IsTriviallyDestructible<Type>::value;

/**
 * @brief Determines whether the specified type can be destructed without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsNothrowDestructible = meta::IsNothrowDestructible<Type>;

/**
 * @brief Variable template that is true when the specified type can be destructed without throwing.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsNothrowDestructibleV = IsNothrowDestructible<Type>::value;

/**
 * @brief Determines whether the specified type is a trivial type
 *        (i.e., a type that can be trivially constructed, copied, and destroyed).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsTrivial = meta::IsTrivial<Type>;

/**
 * @brief Variable template that is true when the specified type is a trivial type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsTrivialV = IsTrivial<Type>::value;

/**
 * @brief Determines whether the specified type is trivially copyable (i.e., a type that can be copied
 *        with a simple bitwise copy and does not require a user-defined copy constructor, copy assignment
 *        operator, or destructor).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsTriviallyCopyable = meta::IsTriviallyCopyable<Type>;

/**
 * @brief Variable template that is true when the specified type is trivially copyable.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsTriviallyCopyableV = IsTriviallyCopyable<Type>::value;

/**
 * @brief Determines whether the specified type is a standard layout type
 *       (i.e., a type that has a standard memory layout and can be used in C-style APIs).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsStandardLayout = meta::IsStandardLayout<Type>;

/**
 * @brief Variable template that is true when the specified type is a standard layout type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsStandardLayoutV = IsStandardLayout<Type>::value;

/**
 * @brief Determines whether the specified type is an empty class (i.e., a class with no non-static data members).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsEmpty = meta::IsEmpty<Type>;

/**
 * @brief Variable template that is true when the specified type is an empty class.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsEmptyV = IsEmpty<Type>::value;

/**
 * @brief Determines whether the specified type is a polymorphic class
 *        (i.e., a class that has at least one virtual function).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsPolymorphic = meta::IsPolymorphic<Type>;

/**
 * @brief Variable template that is true when the specified type is a polymorphic class.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsPolymorphicV = IsPolymorphic<Type>::value;

/**
 * @brief Determines whether the specified type is an abstract class (i.e., a class that cannot be instantiated).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsAbstract = meta::IsAbstract<Type>;

/**
 * @brief Variable template that is true when the specified type is an abstract class.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsAbstractV = IsAbstract<Type>::value;

/**
 * @brief Determines whether the specified type is a final class (i.e., a class that cannot be inherited from).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsFinal = meta::IsFinal<Type>;

/**
 * @brief Variable template that is true when the specified type is a final class.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsFinalV = IsFinal<Type>::value;

/**
 * @brief Determines whether the specified type is an aggregate type
 *        (i.e., a type that can be initialized with aggregate initialization).
 * @tparam Type The specified type to check.
 */
template <typename Type>
using IsAggregate = meta::IsAggregate<Type>;

/**
 * @brief Variable template that is true when the specified type is an aggregate type.
 * @tparam Type The specified type to check.
 */
template <typename Type>
inline constexpr bool IsAggregateV = IsAggregate<Type>::value;

/**
 * @brief Removes the const qualifier from the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveConst = meta::RemoveConst<Type>;

/**
 * @brief Type alias for Type with its const qualifier removed.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveConstT = typename RemoveConst<Type>::type;

/**
 * @brief Removes the volatile qualifier from the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveVolatile = meta::RemoveVolatile<Type>;

/**
 * @brief Type alias for Type with its volatile qualifier removed.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveVolatileT = typename RemoveVolatile<Type>::type;

/**
 * @brief Removes const and volatile qualifiers from the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveCv = meta::RemoveCv<Type>;

/**
 * @brief Type alias for Type with its const and volatile qualifiers removed.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveCvT = typename RemoveCv<Type>::type;

/**
 * @brief Removes reference qualifiers from the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveReference = meta::RemoveReference<Type>;
/**
 * @brief Type alias for Type with its reference qualifiers removed.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveReferenceT = typename RemoveReference<Type>::type;

/**
 * @brief Removes const, volatile, and reference qualifiers from the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveCvref = meta::RemoveCvref<Type>;
/**
 * @brief Type alias for Type with its const, volatile, and reference qualifiers removed.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveCvrefT = typename RemoveCvref<Type>::type;

/**
 * @brief Removes the pointer qualifier from the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemovePointer = meta::RemovePointer<Type>;

/**
 * @brief Removes the pointer qualifier from the specified type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemovePointerT = typename RemovePointer<Type>::type;

/**
 * @brief Removes one level of array extent from the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveExtent = meta::RemoveExtent<Type>;

/**
 * @brief Removes one level of array extent from the specified type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveExtentT = typename RemoveExtent<Type>::type;

/**
 * @brief Removes all levels of array extent from the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveAllExtents = meta::RemoveAllExtents<Type>;

/**
 * @brief Removes all levels of array extent from the specified type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using RemoveAllExtentsT = typename RemoveAllExtents<Type>::type;

/**
 * @brief Adds the const qualifier to the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddConst = meta::AddConst<Type>;

/**
 * @brief Adds the const qualifier to the specified type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddConstT = typename AddConst<Type>::type;

/**
 * @brief Adds the volatile qualifier to the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddVolatile = meta::AddVolatile<Type>;

/**
 * @brief Adds the volatile qualifier to the specified type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddVolatileT = typename AddVolatile<Type>::type;

/**
 * @brief Adds const and volatile qualifiers to the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddCv = meta::AddCv<Type>;

/**
 * @brief Adds const and volatile qualifiers to the specified type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddCvT = typename AddCv<Type>::type;

/**
 * @brief Adds an lvalue reference to the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddLvalueReference = meta::AddLvalueReference<Type>;

/**
 * @brief Adds an lvalue reference to the specified type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddLvalueReferenceT = typename AddLvalueReference<Type>::type;

/**
 * @brief Adds an rvalue reference to the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddRvalueReference = meta::AddRvalueReference<Type>;

/**
 * @brief Adds an rvalue reference to the specified type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddRvalueReferenceT = typename AddRvalueReference<Type>::type;

/**
 * @brief Adds a pointer qualifier to the specified type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddPointer = meta::AddPointer<Type>;

/**
 * @brief Adds a pointer qualifier to the specified type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using AddPointerT = typename AddPointer<Type>::type;

/**
 * @brief Converts an integral type to its corresponding signed type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using MakeSigned = meta::MakeSigned<Type>;

/**
 * @brief Converts an integral type to its corresponding signed type and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using MakeSignedT = typename MakeSigned<Type>::type;

/**
 * @brief Converts an integral type to its corresponding unsigned type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using MakeUnsigned = meta::MakeUnsigned<Type>;

/**
 * @brief Converts an integral type to its corresponding unsigned type
 *        and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using MakeUnsignedT = typename MakeUnsigned<Type>::type;

/**
 * @brief Removes const, volatile, and reference qualifiers from the specified type
 *        and decays it to its underlying form.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using Decay = meta::Decay<Type>;

/**
 * @brief Adds const, volatile, and reference qualifiers to the specified type
 *        and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using DecayT = typename Decay<Type>::type;

/**
 * @brief Provides the identity of the specified type (i.e., it simply returns the same type).
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using TypeIdentity = meta::TypeIdentity<Type>;

/**
 * @brief Provides the identity of the specified type (i.e., it simply returns the same type)
 *        and provides a type alias for the resulting type.
 * @tparam Type The specified type to modify.
 */
template <typename Type>
using TypeIdentityT = typename TypeIdentity<Type>::type;

/**
 * @brief Represents the void type for a given list of types.
 * @note It is commonly used in SFINAE (Substitution Failure Is Not An Error) contexts to detect
 *       the presence of certain types or expressions.
 */
template <typename... Types>
using VoidT = meta::VoidT<Types...>;

/**
 * @brief Represents the common type among a list of types.
 * @tparam Types The list of types to evaluate.
 * @note The common type is the type that all types in the list can be implicitly converted to.
 */
template <typename... Types>
using CommonType = meta::CommonType<Types...>;

/**
 * @brief Represents the common type among a list of types and provides a type alias for the resulting type.
 * @tparam Types The list of types to evaluate.
 * @note The common type is the type that all types in the list can be implicitly converted to.
 */
template <typename... Types>
using CommonTypeT = typename CommonType<Types...>::type;

/**
 * @brief Represents the common reference type among a list of types.
 * @tparam Types The list of types to evaluate.
 * @note The common reference type is the type that all types in the list can be implicitly converted to
 *       when treated as references.
 */
template <typename... Types>
using CommonReference = meta::CommonReference<Types...>;

/**
 * @brief Represents the common reference type among a list of types and provides a type alias for the resulting type.
 * @tparam Types The list of types to evaluate.
 * @note The common reference type is the type that all types in the list can be implicitly converted to
 *       when treated as references.
 */
template <typename... Types>
using CommonReferenceT = typename CommonReference<Types...>::type;

/**
 * @brief Internal implementation detail for unwrapping reference types.
 * @tparam Type The specified type to unwrap.
 */
template <typename Type>
struct NEX_INTERNAL UnwrapRefImpl {
    using type NEX_NODEBUG = Type;
};

/**
 * @brief Specialization of UnwrapRefImpl for Reference wrapper class.
 * @tparam Type The underlying type of the Reference wrapper.
 */
template <typename Type>
struct NEX_INTERNAL UnwrapRefImpl<Reference<Type>> {
    using type NEX_NODEBUG = Type&;
};

/**
 * @brief Retrieves the underlying type of a Reference wrapper class or returns the type itself
 *        if it is not a Reference object.
 * @tparam Type The specified type to unwrap.
 */
template <typename Type>
struct UnwrapReference : UnwrapRefImpl<RemoveCvref<Type>> {};

/**
 * @brief Trait alias for conveniently unwrapping reference types.
 * @tparam Type The specified type to unwrap.
 */
template <typename Type>
using UnwrapReferenceT = typename UnwrapReference<Type>::type;

/**
 * @brief Unwraps reference types and decays the type to its underlying form.
 * @tparam Type The specified type to unwrap and decay.
 */
template <typename Type>
struct UnwrapRefDecay : UnwrapReference<Decay<Type>> {};

/**
 * @brief Trait alias for conveniently unwrapping reference types and decaying the type to its underlying form.
 * @tparam Type The specified type to unwrap and decay.
 */
template <typename Type>
using UnwrapRefDecayT = typename UnwrapRefDecay<Type>::type;

/**
 * @brief Represents one of two types (TrueType or FalseType) based on a boolean condition.
 * @tparam Condition The boolean condition to evaluate.
 * @tparam TrueType The type to select if Condition is true.
 * @tparam FalseType The type to select if Condition is false.
 * @note If Condition is true, Conditional evaluates to TrueType; otherwise, it evaluates to FalseType.
 */
template <bool Condition, typename TrueType, typename FalseType>
using Conditional = meta::Conditional<Condition, TrueType, FalseType>;

/**
 * @brief Represents one of two types (TrueType or FalseType) based on a boolean condition.
 * @tparam Condition The boolean condition to evaluate.
 * @tparam TrueType The type to select if Condition is true.
 * @tparam FalseType The type to select if Condition is false.
 * @note If Condition is true, ConditionalT evaluates to TrueType; otherwise, it evaluates to FalseType.
 */
template <bool Condition, typename TrueType, typename FalseType>
using ConditionalT = typename Conditional<Condition, TrueType, FalseType>::type;

/**
 * @brief Represents the logical conjunction (AND) of a list of type traits.
 * @tparam Traits The list of type traits to evaluate.
 * @note Conjunction evaluates to true if all traits in the list are true; otherwise, it evaluates to false.
 */
template <typename... Traits>
using Conjunction = meta::Conjunction<Traits...>;

/**
 * @brief Variable template that provides a convenient way to access the value of Conjunction.
 * @tparam Traits The list of type traits to evaluate.
 */
template <typename... Traits>
inline constexpr bool ConjunctionV = Conjunction<Traits...>::value;

/**
 * @brief Represents the logical disjunction (OR) of a list of type traits.
 * @tparam Traits The list of type traits to evaluate.
 * @note Disjunction evaluates to true if at least one trait in the list is true; otherwise, it evaluates to false.
 */
template <typename... Traits>
using Disjunction = meta::Disjunction<Traits...>;

/**
 * @brief Variable template that provides a convenient way to access the value of Disjunction.
 * @tparam Traits The list of type traits to evaluate.
 */
template <typename... Traits>
inline constexpr bool DisjunctionV = Disjunction<Traits...>::value;

/**
 * @brief Represents the logical negation (NOT) of a type trait.
 * @tparam Trait The type trait to negate.
 * @note Negation evaluates to true if Trait is false; otherwise, it evaluates to false.
 */
template <typename Trait>
using Negation = meta::Negation<Trait>;

/**
 * @brief Variable template that provides a convenient way to access the value of Negation.
 * @tparam Trait The type trait to negate.
 */
template <typename Trait>
inline constexpr bool NegationV = Negation<Trait>::value;

/**
 * @brief Enables a template only if a boolean condition is true.
 * @tparam Condition The boolean condition to evaluate.
 * @tparam Type The type to enable if the condition is true (default is void).
 */
template <bool Condition, typename Type = void>
using EnableIf = meta::EnableIfT<Condition, Type>;

/**
 * @brief Enables a template only if the specified type is a numeric type (arithmetic type).
 * @note RequiresNumeric evaluates to true if Type is a numeric type; otherwise, it evaluates to false.
 * @tparam Type The type to check for numeric properties.
 */
template <typename Type>
using RequiresNumeric = EnableIf<Arithmetic<RemoveCvref<Type>>>;

/**
 * @brief Enables a template only if the specified type is an integral type.
 * @tparam Type The type to check for integral properties.
 */
template <typename Type>
using RequiresIntegral = EnableIf<Integral<RemoveCvref<Type>>>;

/**
 * @brief Enables a template only if the specified type is a floating-point type.
 * @tparam Type The type to check for floating-point properties.
 */
template <typename Type>
using RequiresFloatingPoint = EnableIf<FloatingPoint<RemoveCvref<Type>>>;

/**
 * @brief Enables a template only if the specified type is a pointer type (i.e., a raw pointer).
 * @tparam Type The type to check for pointer properties.
 */
template <typename Type>
using RequiresPointer = EnableIf<RawPointer<Type>>;

/**
 * @brief Enables a template only if the specified type is an enumeration type (i.e., an enum).
 * @tparam Type The type to check for enumeration properties.
 */
template <typename Type>
using RequiresEnum = EnableIf<Enum<Type>>;

/**
 * @brief Enables a template only if the specified type is a class type (i.e., a struct or class).
 * @tparam Type The type to check for class properties.
 */
template <typename Type>
using RequiresClass = EnableIf<Class<Type>>;

/**
 * @brief Obtains the underlying type of an enumeration type.
 * @tparam Type The enumeration type to obtain the underlying type for.
 */
template <typename Type>
using UnderlyingType = meta::UnderlyingType<Type>;

/**
 * @brief Obtains the underlying type of an enumeration type, providing a convenient way
 *        to access the resulting type.
 * @tparam Type The enumeration type to obtain the underlying type for.
 */
template <typename Type>
using UnderlyingTypeT = typename UnderlyingType<Type>::type;

/**
 * @brief Obtains the resolved return type of invoking a callable with specific arguments.
 * @tparam Fn The callable type.
 * @tparam Args The argument types used for the invocation.
 */
template <typename Fn, typename... Args>
using ResolvedReturnType = DecayT<InvokeResult<Fn, Args...>>;

/**
 * @brief Extracts the element type from a container type.
 * @tparam Type The container type to extract the element type from.
 */
template <typename Type>
using Element = typename RemoveCvref<Type>::value_type;

/**
 * @brief Extracts the type of a member from a pointer to member.
 * @tparam MemberPtr The pointer to member type to extract the member from.
 */
template <typename MemberPtr>
struct MemberPointerTraits;

/**
 * @brief A specialization of MemberPointerTraits for pointers to members,
 *        which extracts the class type and member type from the pointer to member.
 * @tparam Type The class type containing the member.
 * @tparam MemberT The type of the member.
 */
template <typename Type, typename MemberT>
struct MemberPointerTraits<MemberT Type::*> {
    using ClassType = Type;
    using ValueType = MemberT;
};

/**
 * @brief Extracts the class type from a pointer to member.
 * @tparam MemberPtr The pointer to member type to extract the class from.
 */
template <typename MemberPtr>
using MemberClass = typename MemberPointerTraits<MemberPtr>::ClassType;

/**
 * @brief Extracts the member type from a pointer to member.
 * @tparam MemberPtr The pointer to member type to extract the member from.
 */
template <typename MemberPtr>
using MemberType = typename MemberPointerTraits<MemberPtr>::ValueType;

/**
 * @brief Represents a pointer to a member within a struct/class.
 * @tparam Type The class type containing the member.
 * @tparam MemberT The type of the member.
 */
template <typename Type, typename MemberT>
using MemberPointer = MemberT Type::*;

/**
 * @brief Represents a reference to a member within a struct/class.
 * @tparam MemberPtr The pointer to member within the class.
 */
template <typename Type, auto MemberPtr>
using MemberReference = decltype(meta::declval<Type&>().*MemberPtr);

/**
 * @brief Represents a const reference to a member within a struct/class.
 * @tparam Type The class type containing the member.
 * @tparam MemberPtr The pointer to member within the class.
 */
template <typename Type, auto MemberPtr>
using ConstMemberReference = decltype(meta::declval<const Type&>().*MemberPtr);

/**
 * @brief Represents an rvalue reference to a member within a struct/class.
 * @tparam Type The class type containing the member.
 * @tparam MemberPtr The pointer to member within the class.
 */
template <typename Type, auto MemberPtr>
using MemberRvalueReference = decltype(meta::declval<Type&&>().*MemberPtr);

/**
 * @brief Variable template that is true when the specified callable object is invocable
 *        with the given argument types.
 * @tparam Fn The callable type to check.
 * @tparam Args The argument types used for the invocation check.
 */
template <typename Fn, typename... Args>
inline constexpr bool IsCallable = Invocable<Fn, Args...>;

NEX_NAMESPACE_END
