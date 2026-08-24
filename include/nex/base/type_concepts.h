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
#include "nex/base/invoke.h"
#include "nex/base/utility.h"

NEX_NAMESPACE_BEGIN

// ============================================================================
// Concepts for type properties and relationships
// ============================================================================

// Determines whether Type is an integral type.
template <typename Type>
concept Integral = meta::Integral<Type>;

// Determines whether Type is a signed integral type.
template <typename Type>
concept SignedIntegral = meta::SignedIntegral<Type>;

// Determines whether Type is an unsigned integral type.
template <typename Type>
concept UnsignedIntegral = meta::UnsignedIntegral<Type>;

// Determines whether Type is a floating-point type.
template <typename Type>
concept FloatingPoint = meta::FloatingPoint<Type>;

// Determines whether Type is either integral or floating-point.
template <typename Type>
concept Arithmetic = meta::Arithmetic<Type>;

// Determines whether Type is an object type (i.e., not a function, reference, or void).
template <typename Type>
concept ObjectType = meta::ObjectType<Type>;

// Determines whether two types are the same type.
template <typename Type1, typename Type2>
concept SameAs = meta::SameAs<Type1, Type2>;

// Determines whether Derived publicly and unambiguously derives from Base.
template <typename Derived, typename Base>
concept DerivedFrom = meta::DerivedFrom<Derived, Base>;

// Determines whether From is implicitly convertible to To.
template <typename From, typename To>
concept ConvertibleTo = meta::ConvertibleTo<From, To>;

// Determines whether two types share a common reference type.
template <typename Type1, typename Type2>
concept CommonReferenceWith = meta::CommonReferenceWith<Type1, Type2>;

// Determines whether two types share a common value type and compatible references.
template <typename Type1, typename Type2>
concept CommonWith = meta::CommonWith<Type1, Type2>;

// Determines whether Type can be referenced (i.e., is referenceable).
template <typename Type>
concept Referenceable = meta::Referenceable<Type>;

// Determines whether Type can be dereferenced.
template <typename Type>
concept Dereferenceable = requires(Type&& value) {
    *NEX_FORWARD<Type>(value);
};

// Determines whether Type can be used as a boolean value (i.e., is convertible to bool).
template <typename Type>
concept BooleanTestable =
    meta::BooleanTestableImpl<Type> &&
    requires(Type&& t) {
        { !NEX_FORWARD<Type>(t) } -> meta::BooleanTestableImpl;
    };

// Determines whether two values of Type1 and Type2 can be compared for equality.
template <typename Type1, typename Type2>
concept WeaklyEqualityComparableWith =
    requires(meta::RemoveCvrefT<Type1> a, meta::RemoveCvrefT<Type2> b) {
        { a == b } -> BooleanTestable;
        { a != b } -> BooleanTestable;
        { b == a } -> BooleanTestable;
        { b != a } -> BooleanTestable;
    };

// Determines whether an lvalue of To can be assigned from From.
template <typename To, typename From>
concept AssignableFrom =
    meta::IsLvalueReferenceV<To> &&
    CommonReferenceWith<
        const meta::RemoveReferenceT<To>&,
        const meta::RemoveReferenceT<From>&> &&
    requires(To lhs, From&& rhs) {
        { lhs = NEX_FORWARD<From>(rhs) } -> SameAs<To>;
    };

// Determines whether Type can be swapped with another Type.
template <typename Type>
concept Swappable = NEX_STD swappable<Type>;

// Determines whether Type1 and Type2 can be swapped with each other.
template <typename Type1, typename Type2>
concept SwappableWith = NEX_STD swappable_with<Type1, Type2>;

// ============================================================================
// Concepts for comparison and ordering
// ============================================================================

// Determines whether two values of Type can be compared for equality.
template <typename Type>
concept EqualityComparable = WeaklyEqualityComparableWith<Type, Type>;

// Determines whether Type1 and Type2 can be compared for equality with each other.
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

// Determines whether Type has total ordering operators.
template <typename Type>
concept TotallyOrdered =
    EqualityComparable<Type> &&
    PartlyOrderedWith<Type, Type>;

// Determines whether Type1 and Type2 are totally ordered with each other.
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

// Determines whether Type can be destroyed.
template <typename Type>
concept Destructible = meta::Destructible<Type>;

// Determines whether Type can be constructed from Args.
template <typename Type, typename... Args>
concept ConstructibleFrom = meta::ConstructibleFrom<Type, Args...>;

// Determines whether Type can be default-initialized.
template <typename Type>
concept DefaultInitializable = meta::DefaultInitializable<Type>;

// Determines whether Type can be move-constructed.
template <typename Type>
concept MoveConstructible = meta::MoveConstructible<Type>;

// Determines whether Type can be copy-constructed.
template <typename Type>
concept CopyConstructible = meta::CopyConstructible<Type>;

// Determines whether Type can be moved, assigned, and swapped.
template <typename Type>
concept Movable =
    meta::IsObjectV<Type> &&
    MoveConstructible<Type> &&
    AssignableFrom<Type&, Type> &&
    Swappable<Type>;

// Determines whether Type is movable and copyable.
template <typename Type>
concept Copyable =
    CopyConstructible<Type> &&
    Movable<Type> &&
    AssignableFrom<Type&, Type&> &&
    AssignableFrom<Type&, const Type&> &&
    AssignableFrom<Type&, const Type>;

// Determines whether Type is copyable and default-initializable.
template <typename Type>
concept Semiregular =
    Copyable<Type> &&
    DefaultInitializable<Type>;

// Determines whether Type behaves like a regular value type.
template <typename Type>
concept Regular = Semiregular<Type> && EqualityComparable<Type>;

// Determines whether the decayed type is copy constructible and move constructible.
template <typename Type>
concept DecayCopyable =
    CopyConstructible<meta::DecayT<Type>> &&
    MoveConstructible<meta::DecayT<Type>>;

// Alias kept for existing code that uses RegularValue.
template <typename Type>
concept RegularValue = Regular<Type>;

// ============================================================================
// Concepts for invocability and predicates
// ============================================================================

// Determines whether Fn can be invoked with Args.
template <class Fn, class... Args>
concept Invocable = requires(Fn&& fn, Args&&... args) {
    invoke(NEX_FORWARD<Fn>(fn), NEX_FORWARD<Args>(args)...); // not required to be equality preserving
};

// Determines whether Fn can be invoked with Args without modifying observable state.
template <class Fn, class... Args>
concept RegularInvocable = Invocable<Fn, Args...>;

// Determines whether Fn can be invoked with Args and returns exactly Return.
template <typename Fn, typename Return, typename... Args>
concept CallableReturns =
    Invocable<Fn, Args...> &&
    SameAs<InvokeResultT<Fn, Args...>, Return>;

// Determines whether Fn can be invoked with Args and returns a type convertible to Return.
template <typename Fn, typename Return, typename... Args>
concept CallableConvertibleTo =
    Invocable<Fn, Args...> &&
    ConvertibleTo<InvokeResultT<Fn, Args...>, Return>;

// Helper concept to determine if a type is boolean testable (i.e., can be used in a boolean context).
template <class Type>
concept BooleanTestableImpl = ConvertibleTo<Type, bool>;

// Helper concept to determine if a type is boolean testable (i.e., can be used in a boolean context).
template <class Type>
concept BooleanTestable = BooleanTestableImpl<Type> && requires(Type&& t) {
    { !NEX_FORWARD<Type>(t) } -> BooleanTestableImpl;
};

// Determines whether Fn can be used as a predicate for Args.
template <typename Fn, typename... Args>
concept Predicate =
    RegularInvocable<Fn, Args...> &&
    BooleanTestable<InvokeResultT<Fn, Args...>>;

// Determines whether Fn is a binary relation over the supplied argument types.
template <typename Fn, typename Type1, typename Type2>
concept Relation =
    Predicate<Fn, Type1, Type1> && Predicate<Fn, Type2, Type2> &&
    Predicate<Fn, Type1, Type2> && Predicate<Fn, Type2, Type1>;

// Determines whether Fn is an equivalence relation over the supplied argument types.
template <typename Fn, typename Type1, typename Type2>
concept EquivalenceRelation = Relation<Fn, Type1, Type2>;

// Determines whether Fn imposes a strict weak ordering over the supplied argument types.
template <typename Fn, typename Type1, typename Type2>
concept StrictWeakOrder = Relation<Fn, Type1, Type2>;

// ============================================================================
// Concepts for type properties and relationships (NexCore-specific)
// ============================================================================

// Determines whether Type is a raw pointer.
template <typename Type>
concept RawPointer = meta::IsPointerV<meta::RemoveCvrefT<Type>>;

// Determines whether Type is an enum.
template <typename Type>
concept Enum = meta::IsEnumV<meta::RemoveCvrefT<Type>>;

// Determines whether Type is class-like.
template <typename Type>
concept Class = meta::IsClassV<meta::RemoveCvrefT<Type>>;

// Determines whether Type behaves like a raw or smart pointer.
template <typename Type>
concept PointerLike =
    Dereferenceable<Type> && (
        RawPointer<Type> ||
        requires(meta::RemoveReferenceT<Type>& value) {
            value.operator->();
        }
    );

// Determines whether Type can be hashed by std::hash.
template <typename Type>
concept Hashable = requires(const meta::RemoveCvrefT<Type>& value) {
    { NEX_STD hash<meta::RemoveCvrefT<Type>>{}(value) } -> ConvertibleTo<usize>;
};

// Determines whether Type is an indexable range, which means it supports random access
// to elements using the subscript operator [].
template <typename Type>
concept Indexable =
    requires(Type& value, usize index) {
        value[index];
    };

NEX_NAMESPACE_END
