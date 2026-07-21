/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  concepts.h
 * @brief Common C++20 concepts used across the Nex-ecosystem.
 *
 * @details
 * This header provides Nex-named aliases for standard library concepts and a
 * small set of project-specific convenience concepts. Prefer these aliases in
 * public Nex APIs when they improve readability or avoid exposing `std::`
 * directly in declarations.
 */

#include <concepts>
#include <type_traits>
#include <functional>
#include <iterator>
#include <ranges>

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/invoke.h"

NEX_NAMESPACE_BEGIN

// ============================================================================
// Concepts for type properties and relationships
// ============================================================================

/// Checks whether Type is an integral type.
template <typename Type>
concept Integral = meta::Integral<Type>;

/// Checks whether Type is a signed integral type.
template <typename Type>
concept SignedIntegral = meta::SignedIntegral<Type>;

/// Checks whether Type is an unsigned integral type.
template <typename Type>
concept UnsignedIntegral = meta::UnsignedIntegral<Type>;

/// Checks whether Type is a floating-point type.
template <typename Type>
concept FloatingPoint = meta::FloatingPoint<Type>;

/// Checks whether Type is either integral or floating-point.
template <typename Type>
concept Arithmetic = meta::Arithmetic<Type>;

/// Checks whether two types are the same type.
template <typename Type1, typename Type2>
concept SameAs = meta::SameAs<Type1, Type2>;

/// Checks whether Derived publicly and unambiguously derives from Base.
template <typename Derived, typename Base>
concept DerivedFrom = meta::DerivedFrom<Derived, Base>;

/// Checks whether From is implicitly convertible to To.
template <typename From, typename To>
concept ConvertibleTo = meta::ConvertibleTo<From, To>;

/// Checks whether two types share a common reference type.
template <typename Type1, typename Type2>
concept CommonReferenceWith = meta::CommonReferenceWith<Type1, Type2>;

/// Checks whether two types share a common value type and compatible references.
template <typename Type1, typename Type2>
concept CommonWith = meta::CommonWith<Type1, Type2>;

/// Checks whether an lvalue of To can be assigned from From.
template <typename To, typename From>
concept AssignableFrom =
    meta::IsLvalueReferenceV<To> &&
    CommonReferenceWith<
        const meta::RemoveReferenceT<To>&,
        const meta::RemoveReferenceT<From>&> &&
    requires(To lhs, From&& rhs) {
        { lhs = NEX_FORWARD<From>(rhs) } -> SameAs<To>;
    };

/// Checks whether Type can be swapped with another Type.
template <typename Type>
concept Swappable = NEX_STD swappable<Type>;

/// Checks whether Type1 and Type2 can be swapped with each other.
template <typename Type1, typename Type2>
concept SwappableWith = NEX_STD swappable_with<Type1, Type2>;

// ============================================================================
// Concepts for comparison and ordering
// ============================================================================

/// Checks whether two values of Type can be compared for equality.
template <typename Type>
concept EqualityComparable = NEX_STD equality_comparable<Type>;

/// Checks whether Type1 and Type2 can be compared for equality with each other.
template <typename Type1, typename Type2>
concept EqualityComparableWith = NEX_STD equality_comparable_with<Type1, Type2>;

/// Checks whether Type has total ordering operators.
template <typename Type>
concept TotallyOrdered = NEX_STD totally_ordered<Type>;

/// Checks whether Type1 and Type2 are totally ordered with each other.
template <typename Type1, typename Type2>
concept TotallyOrderedWith = NEX_STD totally_ordered_with<Type1, Type2>;

// ============================================================================
// Concepts for object properties, lifetime, and operations
// ============================================================================

/// Checks whether Type can be destroyed.
template <typename Type>
concept Destructible = meta::IsNothrowDestructibleV<Type>;

/// Checks whether Type can be constructed from Args.
template <typename Type, typename... Args>
concept ConstructibleFrom =
    Destructible<Type> &&
    meta::IsConstructibleV<Type, Args...>;

/// Checks whether Type can be default-initialized.
template <typename Type>
concept DefaultInitializable =
    ConstructibleFrom<Type> &&
    requires {
        Type{};
        ::new (static_cast<void_ptr>(nullptr)) Type;
    };

/// Checks whether Type can be move-constructed.
template <typename Type>
concept MoveConstructible =
    ConstructibleFrom<Type, Type> &&
    ConvertibleTo<Type, Type>;

/// Checks whether Type can be copy-constructed.
template <typename Type>
concept CopyConstructible = 
    MoveConstructible<Type> && ConstructibleFrom<Type, Type&> && ConvertibleTo<Type&, Type> && 
    ConstructibleFrom<Type, const Type&> && ConvertibleTo<const Type&, Type> && 
    ConstructibleFrom<Type, const Type> && ConvertibleTo<const Type, Type>;

/// Checks whether Type can be moved, assigned, and swapped.
template <typename Type>
concept Movable = meta::IsObjectV<Type> && MoveConstructible<Type> && AssignableFrom<Type&, Type> && Swappable<Type>;

/// Checks whether Type is movable and copyable.
template <typename Type>
concept Copyable =
    CopyConstructible<Type> &&
    Movable<Type> &&
    AssignableFrom<Type&, Type&> &&
    AssignableFrom<Type&, const Type&> &&
    AssignableFrom<Type&, const Type>;

/// Checks whether Type is copyable and default-initializable.
template <typename Type>
concept Semiregular =
    Copyable<Type> &&
    DefaultInitializable<Type>;

/// Checks whether Type behaves like a regular value type.
template <typename Type>
concept Regular = Semiregular<Type> && EqualityComparable<Type>;

/// Checks whether the decayed type is copy constructible and move constructible.
template <typename Type>
concept DecayCopyable =
    CopyConstructible<meta::DecayT<Type>> &&
    MoveConstructible<meta::DecayT<Type>>;

/// Alias kept for existing code that uses RegularValue.
template <typename Type>
concept RegularValue = Regular<Type>;

// ============================================================================
// Concepts for invocability and predicates
// ============================================================================

/// Checks whether Fn can be invoked with Args.
template <class Fn, class... Args>
concept Invocable = requires(Fn&& fn, Args&&... args) {
    invoke(NEX_FORWARD<Fn>(fn), NEX_FORWARD<Args>(args)...); // not required to be equality preserving
};

/// Checks whether Fn can be invoked with Args without modifying observable state.
template <class Fn, class... Args>
concept RegularInvocable = Invocable<Fn, Args...>;

/// Checks whether Fn can be invoked with Args and returns exactly Return.
template <typename Fn, typename Return, typename... Args>
concept CallableReturns =
    Invocable<Fn, Args...> &&
    SameAs<InvokeResultT<Fn, Args...>, Return>;

/// Checks whether Fn can be invoked with Args and returns a type convertible to Return.
template <typename Fn, typename Return, typename... Args>
concept CallableConvertibleTo =
    Invocable<Fn, Args...> &&
    ConvertibleTo<InvokeResultT<Fn, Args...>, Return>;

/// Helper concept to determine if a type is boolean testable (i.e., can be used in a boolean context).
template <class Type>
concept BooleanTestableImpl = ConvertibleTo<Type, bool>;

/// Helper concept to determine if a type is boolean testable (i.e., can be used in a boolean context).
template <class Type>
concept BooleanTestable = BooleanTestableImpl<Type> && requires(Type&& t) {
    { !NEX_FORWARD<Type>(t) } -> BooleanTestableImpl;
};

/// Checks whether Fn can be used as a predicate for Args.
template <typename Fn, typename... Args>
concept Predicate =
    RegularInvocable<Fn, Args...> &&
    BooleanTestable<InvokeResultT<Fn, Args...>>;

/// Checks whether Fn is a binary relation over the supplied argument types.
template <typename Fn, typename Type1, typename Type2>
concept Relation =
    Predicate<Fn, Type1, Type1> && Predicate<Fn, Type2, Type2> &&
    Predicate<Fn, Type1, Type2> && Predicate<Fn, Type2, Type1>;

/// Checks whether Fn is an equivalence relation over the supplied argument types.
template <typename Fn, typename Type1, typename Type2>
concept EquivalenceRelation = Relation<Fn, Type1, Type2>;

/// Checks whether Fn imposes a strict weak ordering over the supplied argument types.
template <typename Fn, typename Type1, typename Type2>
concept StrictWeakOrder = Relation<Fn, Type1, Type2>;

// ============================================================================
// Concepts for iterators, ranges, and containers
// ============================================================================

/// Checks whether Type supports weak increment operations.
template <typename Type>
concept WeaklyIncrementable = NEX_STD weakly_incrementable<Type>;

/// Checks whether Type supports regular increment operations.
template <typename Type>
concept Incrementable = NEX_STD incrementable<Type>;

/// Checks whether Type is an input or output iterator.
template <typename Type>
concept InputOrOutputIterator = NEX_STD input_or_output_iterator<Type>;

/// Checks whether Sentinel can terminate iteration for Iterator.
template <typename Sentinel, typename Iterator>
concept SentinelFor = NEX_STD sentinel_for<Sentinel, Iterator>;

/// Checks whether Sentinel can terminate and measure distance for Iterator.
template <typename Sentinel, typename Iterator>
concept SizedSentinelFor = NEX_STD sized_sentinel_for<Sentinel, Iterator>;

/// Checks whether Type is an input iterator.
template <typename Type>
concept InputIterator = NEX_STD input_iterator<Type>;

/// Checks whether Type can write Value through iterator assignment.
template <typename Type, typename Value>
concept OutputIterator = NEX_STD output_iterator<Type, Value>;

/// Checks whether Type is a forward iterator.
template <typename Type>
concept ForwardIterator = NEX_STD forward_iterator<Type>;

/// Checks whether Type is a bidirectional iterator.
template <typename Type>
concept BidirectionalIterator = NEX_STD bidirectional_iterator<Type>;

/// Checks whether Type is a random-access iterator.
template <typename Type>
concept RandomAccessIterator = NEX_STD random_access_iterator<Type>;

/// Checks whether Type is a contiguous iterator.
template <typename Type>
concept ContiguousIterator = NEX_STD contiguous_iterator<Type>;

/// Checks whether Type can be read through indirection.
template <typename Type>
concept IndirectlyReadable = NEX_STD indirectly_readable<Type>;

/// Checks whether Value can be written through Out.
template <typename Out, typename Value>
concept IndirectlyWritable = NEX_STD indirectly_writable<Out, Value>;

// ============================================================================
// Concepts for ranges and range properties
// ============================================================================

/// Checks whether Type is a standard range.
template <typename Type>
concept RangeLike = NEX_STD ranges::range<Type>;

/// Checks whether Type is a borrowed range.
template <typename Type>
concept BorrowedRange = NEX_STD ranges::borrowed_range<Type>;

/// Checks whether Type is a sized range.
template <typename Type>
concept SizedRange = NEX_STD ranges::sized_range<Type>;

/// Checks whether Type is a view.
template <typename Type>
concept View = NEX_STD ranges::view<Type>;

/// Checks whether Type is an input range.
template <typename Type>
concept InputRange = NEX_STD ranges::input_range<Type>;

/// Checks whether Type is an output range for Value.
template <typename Type, typename Value>
concept OutputRange = NEX_STD ranges::output_range<Type, Value>;

/// Checks whether Type is a forward range.
template <typename Type>
concept ForwardRange = NEX_STD ranges::forward_range<Type>;

/// Checks whether Type is a bidirectional range.
template <typename Type>
concept BidirectionalRange = NEX_STD ranges::bidirectional_range<Type>;

/// Checks whether Type is a random-access range.
template <typename Type>
concept RandomAccessRange = NEX_STD ranges::random_access_range<Type>;

/// Checks whether Type is a contiguous range.
template <typename Type>
concept ContiguousRange = NEX_STD ranges::contiguous_range<Type>;

/// Checks whether Type can be iterated with standard begin/end.
template <typename Type>
concept Iterable = RangeLike<Type>;

/// Checks whether Type exposes begin/end, size, and a value_type.
template <typename Type>
concept ContainerLike =
    RangeLike<Type> &&
    requires(Type& value) {
        typename meta::RemoveCvrefT<Type>::value_type;
        { value.size() } -> ConvertibleTo<usize>;
    };

/// Checks whether Type is iterable and supports subscript access by usize.
template <typename Type>
concept Indexable =
    RangeLike<Type> &&
    requires(Type& value, usize index) {
        value[index];
    };

// ============================================================================
// Concepts for type properties and relationships (NexCore-specific)
// ============================================================================

/// Checks whether Type is a raw pointer.
template <typename Type>
concept RawPointer = meta::IsPointerV<meta::RemoveCvrefT<Type>>;

/// Checks whether Type is an enum.
template <typename Type>
concept Enum = meta::IsEnumV<meta::RemoveCvrefT<Type>>;

/// Checks whether Type is class-like.
template <typename Type>
concept Class = meta::IsClassV<meta::RemoveCvrefT<Type>>;

/// Checks whether Type can be dereferenced.
template <typename Type>
concept Dereferenceable = requires(Type&& value) {
    *NEX_FORWARD<Type>(value);
};

/// Checks whether Type behaves like a raw or smart pointer.
template <typename Type>
concept PointerLike =
    Dereferenceable<Type> && (
        RawPointer<Type> ||
        requires(meta::RemoveReferenceT<Type>& value) {
            value.operator->();
        }
    );

/// Checks whether Type can be hashed by std::hash.
template <typename Type>
concept Hashable = requires(const meta::RemoveCvrefT<Type>& value) {
    { NEX_STD hash<meta::RemoveCvrefT<Type>>{}(value) } -> ConvertibleTo<usize>;
};

NEX_NAMESPACE_END
