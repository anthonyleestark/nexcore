/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"

NEX_NAMESPACE_BEGIN

// Checks whether Type exposes the NexCore iterator specification,
// which includes the presence of nested types such as value_type, difference_type, pointer, and reference.
template <class Type>
concept HasIteratorSpecification = 
    meta::HasMemberValueType<Type> &&
    meta::HasMemberDifferenceType<Type> &&
    meta::HasMemberPointerType<Type> &&
    meta::HasMemberReferenceType<Type>;

// Primary template for IndirectlyReadableTraits.
template <class>
struct NEX_HIDDEN_FROM_ABI IndirectlyReadableTraits {};

// Specialization of IndirectlyReadableTraits for array types.
template <class Type>
    requires meta::IsArrayV<Type>
struct NEX_HIDDEN_FROM_ABI IndirectlyReadableTraits<Type> {
    using value_type = meta::RemoveCvT<meta::RemoveExtentT<Type>>;
};

// Specialization of IndirectlyReadableTraits for constant types.
template <class Type>
struct NEX_HIDDEN_FROM_ABI IndirectlyReadableTraits<const Type> : IndirectlyReadableTraits<Type> {};

// Specialization of IndirectlyReadableTraits for pointer types.
template <class Type>
    requires(meta::IsObjectV<Type>)
struct NEX_HIDDEN_FROM_ABI IndirectlyReadableTraits<Type*> {
    using value_type = meta::RemoveCvT<Type>;
};

// Specialization of IndirectlyReadableTraits for types that have a member type named `value_type`.
template <meta::HasMemberValueType Type>
struct NEX_HIDDEN_FROM_ABI IndirectlyReadableTraits<Type> {
    using value_type = meta::RemoveCvT<typename Type::value_type>;
};

// Specialization of IndirectlyReadableTraits for types that have a member type named `element_type`.
template <meta::HasMemberElementType Type>
struct NEX_HIDDEN_FROM_ABI IndirectlyReadableTraits<Type> {
    using value_type = meta::RemoveCvT<typename Type::element_type>;
};

// Specialization of IndirectlyReadableTraits for types that have both `value_type` and `element_type`,
// and where the `element_type` is the same as the `value_type` after removing const and volatile qualifiers.
template <meta::HasMemberValueType Type>
    requires(meta::HasMemberElementType<Type>) &&
        SameAs<meta::RemoveCvT<typename Type::element_type>, meta::RemoveCvT<typename Type::value_type>>
struct NEX_HIDDEN_FROM_ABI IndirectlyReadableTraits<Type> {
    using value_type = meta::RemoveCvT<typename Type::value_type>;
};

// Primary template for IncrementableTraits,
// which provides the member type `difference_type` for incrementable types.
template <class>
struct NEX_HIDDEN_FROM_ABI IncrementableTraits {};

// Specialization of IncrementableTraits for pointer types, which are treated as incrementable types.
template <class Type>
    requires(meta::IsObjectV<Type>)
struct NEX_HIDDEN_FROM_ABI IncrementableTraits<Type*> {
    using difference_type = isize;
};

// Specialization of IncrementableTraits for constant pointer types.
template <class Type>
struct NEX_HIDDEN_FROM_ABI IncrementableTraits<const Type*> : IncrementableTraits<Type*> {};

// Specialization of IncrementableTraits for types that have a member type named `difference_type`.
template <meta::HasMemberDifferenceType Type>
struct NEX_HIDDEN_FROM_ABI IncrementableTraits<Type> {
    using difference_type = typename Type::difference_type;
};

/**
 * @note
 * Custom iterators must expose a member type named `difference_type` to be compatible with the IncrementableTraits
 * specialization. This member type should represent the difference between two iterators of the same type, typically
 * as a signed integral type.
 * If a custom iterator does not have a `difference_type`, it will not be compatible with the IncrementableTraits
 * specialization, and the user will need to provide an appropriate `difference_type` member type in their iterator
 * implementation.
 * We can also provide a specialization of IncrementableTraits for custom iterators that do not have a `difference_type`,
 * using fallback concept that checks for the presence of an `operator-` that returns a signed integral type, but this is
 * not recommended, as it may lead to unexpected behavior and is less efficient than providing a proper `difference_type`.
 */

// Traits that provides the member type `pointer` for an iterator
template <class>
struct NEX_HIDDEN_FROM_ABI IteratorTraitsMemberPointer {
    using type NEX_NODEBUG = void;
};

// Specialization of IteratorTraitsMemberPointer for types that have a member type named `pointer`.
template <class Iter>
    requires(meta::HasMemberPointerType<Iter>)
struct NEX_HIDDEN_FROM_ABI IteratorTraitsMemberPointer<Iter> {
    using type NEX_NODEBUG = typename Iter::pointer;
};

// Specialization of IteratorTraitsMemberPointer for types that have an `operator->` but
// do not have a member type named `pointer`.
template <class Iter>
    requires requires(Iter& it) { it.operator->(); } && (!meta::HasMemberPointerType<Iter>)
struct NEX_HIDDEN_FROM_ABI IteratorTraitsMemberPointer<Iter> {
    using type NEX_NODEBUG = decltype(meta::declval<Iter&>().operator->());
};

// Traits that provides the member type `reference` for an iterator
template <Dereferenceable Iter>
using IterReferenceT = decltype(*meta::declval<Iter&>());

// Primary template for IteratorTraitsMemberReference.
template <class Iter>
struct NEX_HIDDEN_FROM_ABI IteratorTraitsMemberReference {
    using type NEX_NODEBUG = IterReferenceT<Iter>;
};

// Specialization of IteratorTraitsMemberReference for types that have a member type named `reference`.
template <meta::HasMemberReferenceType Iter>
struct NEX_HIDDEN_FROM_ABI IteratorTraitsMemberReference<Iter> {
    using type NEX_NODEBUG = typename Iter::reference;
};

// Primary template for IteratorTraits
template <class Iter>
struct IteratorTraits {
    using value_type = typename IndirectlyReadableTraits<Iter>::value_type;
    using difference_type = typename IncrementableTraits<Iter>::difference_type;
    using pointer = typename IteratorTraitsMemberPointer<Iter>::type;
    using reference = typename IteratorTraitsMemberReference<Iter>::type;
};

// Specialization of IteratorTraits for types that have the standard iterator specification.
template <HasIteratorSpecification Type>
struct IteratorTraits {
    using value_type = typename Type::value_type;
    using difference_type = typename Type::difference_type;
    using pointer = typename Type::pointer;
    using reference = typename Type::reference;
};

// Specialization of IteratorTraits for pointer types, which are treated as iterators to the pointed-to type.
template <class Type>
    requires(meta::IsObjectV<Type>)
struct IteratorTraits<Type*> {
    using value_type = meta::RemoveCvT<Type>;
    using difference_type = isize;
    using pointer = Type*;
    using reference = Type&;
};

// Traits alias for easier access to the nested `value_type` of IteratorTraits
template <class Iter>
using IteratorValueType = typename IteratorTraits<Iter>::value_type;

// Traits alias for easier access to the nested `difference_type` of IteratorTraits
template <class Iter>
using IteratorDifferenceType = typename IteratorTraits<Iter>::difference_type;

// Traits alias for easier access to the nested `pointer` type of IteratorTraits
template <class Iter>
using IteratorPointerType = typename IteratorTraits<Iter>::pointer;

// Traits alias for easier access to the nested `reference` type of IteratorTraits
template <class Iter>
using IteratorReferenceType = typename IteratorTraits<Iter>::reference;

NEX_NAMESPACE_END
