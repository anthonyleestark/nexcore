/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  type_concepts.h
 * @brief A collection of type concepts and utilities for working with types in C++.
 *
 * @details
 * This header provides a collection of type concepts and utilities that simplify the use of standard
 * C++ type concepts. It includes aliases for common type concepts, as well as additional utilities for
 * working with types in a more concise and readable manner. The concepts defined in this header can be
 * used to perform compile-time checks on types, manipulate types, and enable or disable template
 * instantiations based on type properties.
 */

#include <concepts>

#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/fwd.h"
#include "nex/base/invoke.h"
#include "nex/base/utility.h"

NEX_NAMESPACE_BEGIN

// ============================================================================
// Concepts for type properties and relationships
// ============================================================================

/**
 * @brief Determines whether a type is an integral type.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Integral = meta::Integral<Type>;

/**
 * @brief Determines whether a type is a signed integral type.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept SignedIntegral = meta::SignedIntegral<Type>;

/**
 * @brief Determines whether a type is an unsigned integral type.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept UnsignedIntegral = meta::UnsignedIntegral<Type>;

/**
 * @brief Determines whether a type is a floating-point type.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept FloatingPoint = meta::FloatingPoint<Type>;

/**
 * @brief Determines whether a type can be used in arithmetic operations (i.e., integral or floating-point).
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Arithmetic = meta::Arithmetic<Type>;

/**
 * @brief Determines whether a type is an object type (i.e., not a function, reference, or void),
 *        meaning it can be instantiated and used as a value.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept ObjectType = meta::ObjectType<Type>;

/**
 * @brief Determines whether two specified types are the same, meaning they are identical
 *        in type and cv-qualifiers.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
concept SameAs = meta::SameAs<Type1, Type2>;

/**
 * @brief Determines whether a type publicly and unambiguously derives from another type,
 *        meaning it is a derived class of the specified base class.
 * @tparam Derived The derived type to check.
 * @tparam Base The base type to check.
 */
template <typename Derived, typename Base>
concept DerivedFrom = meta::DerivedFrom<Derived, Base>;

/**
 * @brief Determines whether a type is implicitly convertible to another type,
 *        meaning it can be used in contexts where the target type is expected.
 * @tparam From The source type.
 * @tparam To The target type.
 */
template <typename From, typename To>
concept ConvertibleTo = meta::ConvertibleTo<From, To>;

/**
 * @brief Determines whether two specified types share a common reference type,
 *        meaning they can be consistently referenced in a common way.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
concept CommonReferenceWith = meta::CommonReferenceWith<Type1, Type2>;

/**
 * @brief Determines whether two specified types share a common value type and compatible references,
 *        meaning they can be used interchangeably in certain contexts.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
concept CommonWith = meta::CommonWith<Type1, Type2>;

/**
 * @brief Determines whether a type can be referenced (i.e., is referenceable).
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Referenceable = meta::Referenceable<Type>;

/**
 * @brief Determines whether a type can be dereferenced, meaning it can be used with
 *        the dereference operator (*).
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Dereferenceable = requires(Type&& value) {
    *NEX_FORWARD<Type>(value);
};

/**
 * @brief Determines whether a type can be used as a boolean value (i.e., is convertible to bool),
 *        meaning it can be consistently evaluated in boolean contexts.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept BooleanTestable =
    meta::BooleanTestableImpl<Type> &&
    requires(Type&& t) {
        { !NEX_FORWARD<Type>(t) } -> meta::BooleanTestableImpl;
    };

/**
 * @brief Determines whether a type can be assigned from another type,
 *        meaning the target type can be consistently updated with the value of the source type.
 * @tparam To The target type.
 * @tparam From The source type.
 */
template <typename To, typename From>
concept AssignableFrom = _AssignableFrom<To, From>;

/**
 * @brief Determines whether a type can be swapped with another object of the same type,
 *        meaning they can be exchanged in a consistent manner.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Swappable =
    requires(Type& a, Type& b) {
        ranges::swap(a, b);
    };

/**
 * @brief Determines whether two specified types can be swapped with each other,
 *        meaning they can be exchanged in a consistent manner.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
concept SwappableWith =
    CommonReferenceWith<Type1, Type2> &&
    requires(Type1&& x, Type2&& y) {
        ranges::swap(NEX_FORWARD<Type1>(x), NEX_FORWARD<Type1>(x));
        ranges::swap(NEX_FORWARD<Type2>(y), NEX_FORWARD<Type2>(y));
        ranges::swap(NEX_FORWARD<Type1>(x), NEX_FORWARD<Type2>(y));
        ranges::swap(NEX_FORWARD<Type2>(y), NEX_FORWARD<Type1>(x));
    };

// ============================================================================
// Concepts for comparison and ordering
// ============================================================================

/**
 * @brief Determines whether two values of the specified types can be compared for equality,
 *        meaning they can be compared for equality in a consistent manner.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
concept WeaklyEqualityComparableWith =
    requires(meta::RemoveCvrefT<Type1> a, meta::RemoveCvrefT<Type2> b) {
        { a == b } -> BooleanTestable;
        { a != b } -> BooleanTestable;
        { b == a } -> BooleanTestable;
        { b != a } -> BooleanTestable;
    };

/**
 * @brief Determines whether two values of the specified type can be compared for equality,
 *        meaning they can be compared for equality in a consistent manner.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept EqualityComparable = WeaklyEqualityComparableWith<Type, Type>;

/**
 * @brief Determines whether two values of the specified types can be compared for equality with each other,
 *        meaning they can be compared for equality in a consistent manner.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
concept EqualityComparableWith =
    EqualityComparable<Type1> && EqualityComparable<Type2> &&
    CommonReferenceWith<
        const meta::RemoveReferenceT<Type1>&,
        const meta::RemoveReferenceT<Type2>&> &&
    EqualityComparable<
        meta::CommonReferenceT<
            const meta::RemoveReferenceT<Type1>&,
            const meta::RemoveReferenceT<Type2>&>> &&
    WeaklyEqualityComparableWith<Type1, Type2>;

/**
 * @brief Determines whether two values of the specified types can be compared for less-than ordering,
 *        meaning they can be compared to determine which is less than the other.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <class Type1, class Type2>
concept LessThanComparable =
    requires(
        const meta::RemoveCvrefT<Type1>& lhs,
        const meta::RemoveCvrefT<Type2>& rhs)
    {
        { lhs < rhs } -> BooleanTestable;
        { rhs < lhs } -> BooleanTestable;
    };

/**
 * @brief Determines whether two values of the specified types can be compared for greater-than ordering,
 *        meaning they can be compared for ordering in a consistent manner.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <class Type1, class Type2>
concept GreaterThanComparable =
    requires(
        const meta::RemoveCvrefT<Type1>& lhs,
        const meta::RemoveCvrefT<Type2>& rhs)
    {
        { lhs > rhs } -> BooleanTestable;
        { rhs > lhs } -> BooleanTestable;
    };

/**
 * @brief Determines whether two values of the specified types can be compared for partial ordering,
 *        meaning they can be compared for less-than and greater-than ordering, but not necessarily for equality.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
concept PartlyOrderedWith =
    requires(meta::RemoveCvrefT<Type1> a, meta::RemoveCvrefT<Type2> b) {
        { a < b } -> BooleanTestable;
        { a > b } -> BooleanTestable;
        { a <= b } -> BooleanTestable;
        { a >= b } -> BooleanTestable;
        { b < a } -> BooleanTestable;
        { b > a } -> BooleanTestable;
        { b <= a } -> BooleanTestable;
        { b >= a } -> BooleanTestable;
    };

/**
 * @brief Determines whether the specified type has total ordering operators,
 *        meaning it can be compared for equality and ordering in a consistent manner.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept TotallyOrdered =
    EqualityComparable<Type> &&
    PartlyOrderedWith<Type, Type>;

/**
 * @brief Determines whether two values of the specified types are totally ordered with each other,
 *        meaning they can be compared for equality and ordering in a consistent manner.
 * @tparam Type1 The first type to check.
 * @tparam Type2 The second type to check.
 */
template <typename Type1, typename Type2>
concept TotallyOrderedWith =
    TotallyOrdered<Type1> && TotallyOrdered<Type2> &&
    EqualityComparableWith<Type1, Type2> &&
    TotallyOrdered<
        meta::CommonReferenceT<
            const meta::RemoveReferenceT<Type1>&,
            const meta::RemoveReferenceT<Type2>&>> &&
    PartlyOrderedWith<Type1, Type2>;

// ============================================================================
// Concepts for object properties, lifetime, and operations
// ============================================================================

/**
 * @brief Determines whether the specified type can be destroyed (i.e., has a valid destructor).
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Destructible = meta::Destructible<Type>;

/**
 * @brief Determines whether the specified type can be constructed from the given argument types,
 *        meaning it can be initialized with those arguments.
 * @tparam Type The given type to check.
 * @tparam Args The argument types to check.
 */
template <typename Type, typename... Args>
concept ConstructibleFrom = meta::ConstructibleFrom<Type, Args...>;

/**
 * @brief Determines whether the specified type can be default-initialized,
 *        meaning it can be constructed without any arguments.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept DefaultInitializable = meta::DefaultInitializable<Type>;

/**
 * @brief Determines whether the specified type can be move-constructed,
 *        meaning it can be constructed from an rvalue reference to itself.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept MoveConstructible = meta::MoveConstructible<Type>;

/**
 * @brief Determines whether the specified type can be copy-constructed,
 *        meaning it can be constructed from a const reference to itself.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept CopyConstructible = meta::CopyConstructible<Type>;

/**
 * @brief Determines whether the specified type can be moved, assigned, and swapped,
 *        meaning it can be used as a regular value type.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Movable =
    meta::IsObjectV<Type> &&
    MoveConstructible<Type> &&
    AssignableFrom<Type&, Type> &&
    Swappable<Type>;

/**
 * @brief Determines whether the specified type is movable and copyable,
 *        meaning it can be used as a regular value type.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Copyable =
    CopyConstructible<Type> &&
    Movable<Type> &&
    AssignableFrom<Type&, Type&> &&
    AssignableFrom<Type&, const Type&> &&
    AssignableFrom<Type&, const Type>;

/**
 * @brief Determines whether the specified type is copyable and default-initializable,
 *        meaning it can be used as a regular value type.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Semiregular =
    Copyable<Type> &&
    DefaultInitializable<Type>;

/**
 * @brief Determines whether the specified type behaves like a regular value type,
 *        supporting copy, move, and equality operations.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Regular = Semiregular<Type> && EqualityComparable<Type>;

/**
 * @brief Determines whether the decayed form of the specified type is copy constructible and
 *        move constructible, meaning it can be safely copied and moved after type decay.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept DecayCopyable =
    CopyConstructible<meta::DecayT<Type>> &&
    MoveConstructible<meta::DecayT<Type>>;

// ============================================================================
// Concepts for invocability and predicates
// ============================================================================

/**
 * @brief Determines whether a callable object can be invoked with the given argument types.
 * @tparam Fn The function type to check.
 * @tparam Args The argument types to check.
 */
template <class Fn, class... Args>
concept Invocable = requires(Fn&& fn, Args&&... args) {
    NEX_INVOKE(fn, NEX_FORWARD<Args>(args)...); // not required to be equality preserving
};

/**
 * @brief Determines whether a callable object can be invoked with the given argument types
 *        without modifying observable state.
 * @tparam Fn The function type to check.
 * @tparam Args The argument types to check.
 */
template <class Fn, class... Args>
concept RegularInvocable = Invocable<Fn, Args...>;

/**
 * @brief Determines whether a callable object can be invoked with the given argument types
 *        and returns exactly the specified return type.
 * @tparam Fn The function type to check.
 * @tparam Return The return type to check.
 * @tparam Args The argument types to check.
 */
template <typename Fn, typename Return, typename... Args>
concept CallableReturns =
    Invocable<Fn, Args...> &&
    SameAs<InvokeResultT<Fn, Args...>, Return>;

/**
 * @brief Determines whether a callable object can be invoked with the given argument types
 *        and returns a type convertible to the specified return type.
 * @tparam Fn The function type to check.
 * @tparam Return The return type to check.
 * @tparam Args The argument types to check.
 */
template <typename Fn, typename Return, typename... Args>
concept CallableConvertibleTo =
    Invocable<Fn, Args...> &&
    ConvertibleTo<InvokeResultT<Fn, Args...>, Return>;

/**
 * @brief Determines whether a callable object can be used as a predicate for the given argument types,
 *        meaning it returns a value that can be used in boolean contexts.
 * @tparam Fn The function type to check.
 * @tparam Args The argument types to check.
 */
template <typename Fn, typename... Args>
concept Predicate =
    RegularInvocable<Fn, Args...> &&
    BooleanTestable<InvokeResultT<Fn, Args...>>;

/**
 * @brief Determines whether a callable object is a binary relation over the supplied argument types,
 *        meaning it satisfies certain properties like reflexivity, symmetry, and transitivity.
 * @tparam Fn The function type to check.
 * @tparam Type1 The first argument type to check.
 * @tparam Type2 The second argument type to check.
 */
template <typename Fn, typename Type1, typename Type2>
concept Relation =
    Predicate<Fn, Type1, Type1> && Predicate<Fn, Type2, Type2> &&
    Predicate<Fn, Type1, Type2> && Predicate<Fn, Type2, Type1>;

/**
 * @brief Determines whether a callable object is an equivalence relation over the supplied
 *        argument types, meaning it satisfies reflexivity, symmetry, and transitivity.
 * @tparam Fn The function type to check.
 * @tparam Type1 The first argument type to check.
 * @tparam Type2 The second argument type to check.
 */
template <typename Fn, typename Type1, typename Type2>
concept EquivalenceRelation = Relation<Fn, Type1, Type2>;

/**
 * @brief Determines whether a callable object imposes a strict weak ordering over the supplied
 *        argument types, which is a requirement for sorting and ordering algorithms.
 * @tparam Fn The function type to check.
 * @tparam Type1 The first argument type to check.
 * @tparam Type2 The second argument type to check.
 */
template <typename Fn, typename Type1, typename Type2>
concept StrictWeakOrder = Relation<Fn, Type1, Type2>;

// ============================================================================
// Concepts for type properties and relationships (NexCore-specific)
// ============================================================================

/**
 * @brief Determines whether the specified type is a raw pointer.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept RawPointer = meta::IsPointerV<meta::RemoveCvrefT<Type>>;

/**
 * @brief Determines whether the specified type is an enum.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Enum = meta::IsEnumV<meta::RemoveCvrefT<Type>>;

/**
 * @brief Determines whether the specified type is class-like.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Class = meta::IsClassV<meta::RemoveCvrefT<Type>>;

/**
 * @brief Determines whether the specified type behaves like a raw or smart pointer.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept PointerLike =
    Dereferenceable<Type> && (
        RawPointer<Type> ||
        requires(meta::RemoveReferenceT<Type>& value) {
            value.operator->();
        }
    );

/**
 * @brief Determines whether the specified type can be hashed.
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Hashable = requires(const meta::RemoveCvrefT<Type>& value) {
    { NEX_STD hash<meta::RemoveCvrefT<Type>>{}(value) } -> ConvertibleTo<usize>;
};

/**
 * @brief Determines whether the specified type is an indexable range, which means it supports
 *        random access to elements using the subscript operator [].
 * @tparam Type The given type to check.
 */
template <typename Type>
concept Indexable =
    requires(Type& value, usize index) {
        value[index];
    };

NEX_NAMESPACE_END
