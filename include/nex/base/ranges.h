/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/type_concepts.h"
#include "nex/base/iterator.h"
#include "nex/base/init.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(ranges)

// Checks whether BorrowedRange is enabled for a given type.
// By default, this is false for all types.
template <class>
inline constexpr bool EnableBorrowedRange = false;

// Checks whether a Type can be used as a borrowed range, which means it can be safely used
// in range-based for loops and other range algorithms without taking ownership of the underlying data.
template <class Type>
concept CanBorrow = meta::IsLvalueReferenceV<Type> || EnableBorrowedRange<meta::RemoveCvrefT<Type>>;

// =================================================================================
// Range access utilities
// =================================================================================

NEX_SUBNAMESPACE_BEGIN(_begin)

// Determines whether Type has a member function `begin()` that returns an iterator.
template <class Type>
concept MemberBegin = CanBorrow<Type> && requires(Type&& value) {
    { NEX_AUTO_CAST(value.begin()) } -> Iterator;
};

void begin() = delete;

// Determines whether Type can be used with unqualified `begin()` function calls.
template <class Type>
concept UnqualifiedBegin =
    !MemberBegin<Type> && CanBorrow<Type> &&
    meta::ClassOrEnum<meta::RemoveCvrefT<Type>> &&
    requires(Type&& value) {
        { NEX_AUTO_CAST(begin(value)) } -> Iterator;
    };

struct Fn {
    /**
     * @brief Returns an iterator to the beginning of an array.
     * @tparam Type The type of the array elements.
     * @tparam Size The size of the array.
     * @param value The array to get the beginning iterator for.
     * @return A pointer to the first element of the array.
     * @note This overload is only enabled for arrays of known size.
     */
    template <class Type, usize Size>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type (&value)[Size]) const noexcept
        requires(sizeof(Type) >= 0) // Disallow incomplete element types.
    {
        return value + 0;
    }

    /**
     * @brief Returns an iterator to the beginning of a range-like object that has a member `begin()` function.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the beginning iterator for.
     * @return The iterator returned by the member `begin()` function.
     * @note This overload is only enabled for types that have a member `begin()` function and can be borrowed.
     */
    template <class Type>
        requires MemberBegin<Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(NEX_AUTO_CAST(value.begin())))
            -> decltype(NEX_AUTO_CAST(value.begin())) {
        return NEX_AUTO_CAST(value.begin());
    }

    /**
     * @brief Returns an iterator to the beginning of a range-like object that can be used
     *        with unqualified `begin()` calls.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the beginning iterator for.
     * @return The iterator returned by the unqualified `begin()` function.
     * @note This overload is only enabled for types that can be used with unqualified `begin()` calls
     *       and can be borrowed.
     */
    template <class Type>
        requires UnqualifiedBegin<Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(NEX_AUTO_CAST(begin(value))))
            -> decltype(NEX_AUTO_CAST(begin(value))) {
        return NEX_AUTO_CAST(begin(value));
    }
};

NEX_SUBNAMESPACE_END(_begin)

/**
 * @brief Returns an iterator to the beginning of a range-like object.
 * @tparam Type The type of the range-like object.
 * @param value The range-like object to get the beginning iterator for.
 * @return An iterator to the beginning of the range-like object.
 * @note 
 * This function is a wrapper around the `_begin::Fn` struct and provides a convenient
 * way to obtain the beginning iterator for various types of range-like objects,
 * including arrays, containers with member `begin()` functions, and types that can
 * be used with unqualified `begin()` calls.
 */
inline constexpr auto begin = _begin::Fn{};

// Iterator type for a given range-like type, deduced from the return type of `ranges::begin`.
template <class Type>
using RangeIteratorT = decltype(ranges::begin(meta::declval<Type&>()));

NEX_SUBNAMESPACE_BEGIN(_end)

// Determines whether Type has a member function `end()` that returns a sentinel.
template <class Type>
concept MemberEnd = CanBorrow<Type> && requires(Type&& value) {
    { NEX_AUTO_CAST(value.end()) } -> SentinelFor<RangeIteratorT<Type>>;
};

// Determines whether Type can be used with unqualified `end()` function calls.
template <class Type>
concept UnqualifiedEnd =
    !MemberEnd<Type> && CanBorrow<Type> &&
    meta::ClassOrEnum<meta::RemoveCvrefT<Type>> &&
    requires(Type&& value) {
        { NEX_AUTO_CAST(end(value)) } -> SentinelFor<RangeIteratorT<Type>>;
    };

struct Fn {
    /**
     * @brief Returns a sentinel to the end of an array.
     * @tparam Type The type of the array elements.
     * @tparam Size The size of the array.
     * @param value The array to get the end sentinel for.
     * @return A pointer to one past the last element of the array.
     * @note This overload is only enabled for arrays of known size.
     */
    template <class Type, usize Size>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type (&value)[Size]) const noexcept
        requires(sizeof(Type) >= 0) // Disallow incomplete element types.
    {
        return value + Size;
    }

    /**
     * @brief Returns a sentinel to the end of a range-like object that has a member `end()` function.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the end sentinel for.
     * @return The sentinel returned by the member `end()` function.
     * @note This overload is only enabled for types that have a member `end()` function and can be borrowed.
     */
    template <class Type>
        requires MemberEnd<Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(NEX_AUTO_CAST(value.end())))
            -> decltype(NEX_AUTO_CAST(value.end())) {
        return NEX_AUTO_CAST(value.end());
    }

    /**
     * @brief Returns a sentinel to the end of a range-like object that can be used
     *        with unqualified `end()` calls.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the end sentinel for.
     * @return The sentinel returned by the unqualified `end()` function.
     * @note This overload is only enabled for types that can be used with unqualified `end()` calls
     *       and can be borrowed.
     */
    template <class Type>
        requires UnqualifiedEnd<Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(NEX_AUTO_CAST(end(value))))
            -> decltype(NEX_AUTO_CAST(end(value))) {
        return NEX_AUTO_CAST(end(value));
    }
};

NEX_SUBNAMESPACE_END(_end)

/**
 * @brief Returns a sentinel to the end of a range-like object.
 * @tparam Type The type of the range-like object.
 * @param value The range-like object to get the end sentinel for.
 * @return A sentinel to the end of the range-like object.
 * @note 
 * This function is a wrapper around the `_end::Fn` struct and provides a convenient
 * way to obtain the end sentinel for various types of range-like objects,
 * including arrays, containers with member `end()` functions, and types that can
 * be used with unqualified `end()` calls.
 */
inline constexpr auto end = _end::Fn{};

NEX_SUBNAMESPACE_BEGIN(_cbegin)

struct Fn {
    template <class Type>
        requires meta::IsLvalueReferenceV<Type&&>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(ranges::begin(static_cast<const meta::RemoveCvrefT<Type>&>(value))))
            -> decltype(ranges::begin(static_cast<const meta::RemoveCvrefT<Type>&>(value))) {
        return ranges::begin(static_cast<const meta::RemoveCvrefT<Type>&>(value));
    }

    template <class Type>
        requires meta::IsRvalueReferenceV<Type&&>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(ranges::begin(static_cast<const Type&&>(value))))
            -> decltype(ranges::begin(static_cast<const Type&&>(value))) {
        return ranges::begin(static_cast<const Type&&>(value));
    }
};

NEX_SUBNAMESPACE_END(_cbegin)

/**
 * @brief Returns a const iterator to the beginning of a range-like object.
 * @tparam Type The type of the range-like object.
 * @param value The range-like object to get the const beginning iterator for.
 * @return A const iterator to the beginning of the range-like object.
 * @note 
 * This function is a wrapper around the `_cbegin::Fn` struct and provides a convenient
 * way to obtain the const beginning iterator for various types of range-like objects,
 * including arrays, containers with member `begin()` functions, and types that can
 * be used with unqualified `begin()` calls.
 */
inline constexpr auto cbegin = _cbegin::Fn{};

NEX_SUBNAMESPACE_BEGIN(_cend)

struct Fn {
    template <class Type>
        requires meta::IsLvalueReferenceV<Type&&>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(ranges::end(static_cast<const meta::RemoveCvrefT<Type>&>(value))))
            -> decltype(ranges::end(static_cast<const meta::RemoveCvrefT<Type>&>(value))) {
        return ranges::end(static_cast<const meta::RemoveCvrefT<Type>&>(value));
    }

    template <class Type>
        requires meta::IsRvalueReferenceV<Type&&>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(ranges::end(static_cast<const Type&&>(value))))
            -> decltype(ranges::end(static_cast<const Type&&>(value))) {
        return ranges::end(static_cast<const Type&&>(value));
    }
};

NEX_SUBNAMESPACE_END(_cend)

/**
 * @brief Returns a const sentinel to the end of a range-like object.
 * @tparam Type The type of the range-like object.
 * @param value The range-like object to get the const end sentinel for.
 * @return A const sentinel to the end of the range-like object.
 * @note 
 * This function is a wrapper around the `_cend::Fn` struct and provides a convenient
 * way to obtain the const end sentinel for various types of range-like objects,
 * including arrays, containers with member `end()` functions, and types that can
 * be used with unqualified `end()` calls.
 */
inline constexpr auto cend = _cend::Fn{};

// =================================================================================
// Range size utilities
// =================================================================================

// Determines whether sized range utilities are disabled for a given type.
template <class>
inline constexpr bool DisableSizedRange = false;

NEX_SUBNAMESPACE_BEGIN(_size)

void size() = delete;

// Determined whether sized range utilities are enabled for a given type.
template <class Type>
concept SizeEnabled = !DisableSizedRange<meta::RemoveCvrefT<Type>>;

// Determines whether Type has a member function `size()` that returns an integer-like value.
template <class Type>
concept MemberSize = SizeEnabled<Type> && requires(Type&& value) {
    { NEX_AUTO_CAST(value.size()) } -> meta::IntegerLike;
};

// Determines whether Type can be used with unqualified `size()` function calls.
template <class Type>
concept UnqualifiedSize =
    SizeEnabled<Type> && !MemberSize<Type> &&
    meta::ClassOrEnum<meta::RemoveCvrefT<Type>> &&
    requires(Type&& value) {
        { NEX_AUTO_CAST(size(value)) } -> meta::IntegerLike;
    };

// Determines whether Type is a difference range, which means it has a known size
// and can be used with `ranges::begin()` and `ranges::end()`.
template <class Type>
concept Difference =
    !MemberSize<Type> && !UnqualifiedSize<Type> &&
    meta::ClassOrEnum<meta::RemoveCvrefT<Type>> && requires(Type&& value) {
        { ranges::begin(value) } -> ForwardIterator;
        { ranges::end(value) } -> SizedSentinelFor<decltype(ranges::begin(meta::declval<Type&>()))>;
    };

struct Fn {
    /**
     * @brief Returns the size of an array.
     * @tparam Type The type of the array elements.
     * @tparam Size The size of the array.
     * @param value The array to get the size of.
     * @return The size of the array.
     * @note This overload is only enabled for arrays of known size.
     */
    template <class Type, usize Size>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr usize operator()(Type (&&)[Size]) const noexcept {
        return Size;
    }

    /**
     * @brief Returns the size of an array.
     * @tparam Type The type of the array elements.
     * @tparam Size The size of the array.
     * @param value The array to get the size of.
     * @return The size of the array.
     * @note This overload is only enabled for arrays of known size.
     */
    template <class Type, usize Size>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr usize operator()(Type (&)[Size]) const noexcept {
        return Size;
    }

    /**
     * @brief Returns the size of a range-like object that has a member `size()` function.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the size of.
     * @return The size of the range-like object.
     * @note This overload is only enabled for types that have a member `size()` function and can be borrowed.
     */
    template <MemberSize Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr meta::IntegerLike auto operator()(Type&& value) const
        noexcept(noexcept(NEX_AUTO_CAST(value.size()))) {
        return NEX_AUTO_CAST(value.size());
    }

    /**
     * @brief Returns the size of a range-like object that can be used with unqualified `size()` calls.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the size of.
     * @return The size of the range-like object.
     * @note This overload is only enabled for types that can be used with unqualified `size()` calls
     *       and can be borrowed.
     */
    template <UnqualifiedSize Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr meta::IntegerLike auto operator()(Type&& value) const
        noexcept(noexcept(NEX_AUTO_CAST(size(value)))) {
        return NEX_AUTO_CAST(size(value));
    }

    /**
     * @brief Returns the size of a difference range, which is calculated as the difference
     *        between the end and begin iterators.
     * @tparam Type The type of the difference range.
     * @param value The difference range to get the size of.
     * @return The size of the difference range.
     * @note This overload is only enabled for types that are difference ranges and can be borrowed.
     */
    template <Difference Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr meta::IntegerLike auto operator()(Type&& value) const
        noexcept(noexcept(meta::toUnsignedLike(ranges::end(value) - ranges::begin(value))))
            -> decltype(meta::toUnsignedLike(ranges::end(value) - ranges::begin(value))) {
        return meta::toUnsignedLike(ranges::end(value) - ranges::begin(value));
    }
};

NEX_SUBNAMESPACE_END(_size)

/**
 * @brief Returns the size of a range-like object.
 * @tparam Type The type of the range-like object.
 * @param value The range-like object to get the size of.
 * @return The size of the range-like object.
 * @note 
 * This function is a wrapper around the `_size::Fn` struct and provides a convenient
 * way to obtain the size for various types of range-like objects, including arrays,
 * containers with member `size()` functions, types that can be used with unqualified
 * `size()` calls, and difference ranges.
 */
inline constexpr auto size = _size::Fn{};

NEX_SUBNAMESPACE_BEGIN(_ssize)

struct Fn {
    /**
     * @brief Returns the signed size of a range-like object.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the signed size of.
     * @return The signed size of the range-like object.
     */
    template <class Type>
        requires requires(Type&& value) { ranges::size(value); }
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Integral auto operator()(Type&& value) const
        noexcept(noexcept(ranges::size(value))) {
        using SignedType = meta::MakeSignedT<decltype(ranges::size(value))>;
        if constexpr (sizeof(isize) > sizeof(SignedType)) {
            return static_cast<isize>(ranges::size(value));
        } else {
            return static_cast<SignedType>(ranges::size(value));
        }
    }
};

NEX_SUBNAMESPACE_END(_ssize)

/**
 * @brief Returns the signed size of a range-like object.
 * @tparam Type The type of the range-like object.
 * @param value The range-like object to get the signed size of.
 * @return The signed size of the range-like object.
 * @note 
 * This function is a wrapper around the `_ssize::Fn` struct and provides a convenient
 * way to obtain the signed size for various types of range-like objects, including arrays,
 * containers with member `size()` functions, types that can be used with unqualified
 * `size()` calls, and difference ranges. The return type is determined based on the
 * underlying size type, ensuring that it is appropriately signed.
 */
inline constexpr auto ssize = _ssize::Fn{};

// =================================================================================
// Range data utilities
// =================================================================================

NEX_SUBNAMESPACE_BEGIN(_data)

template <class Type>
concept PointerToObject = meta::IsPointerV<Type> && meta::IsObjectV<meta::RemovePointerT<Type>>;

template <class Type>
concept MemberData = CanBorrow<Type> && requires(Type&& value) {
    { NEX_AUTO_CAST(value.data()) } -> PointerToObject;
};

template <class Type>
concept RangesBeginInvocable =
    !MemberData<Type> && CanBorrow<Type> &&
    requires(Type&& value) {
        { ranges::begin(value) } -> ContiguousIterator;
    };

NEX_SUBNAMESPACE_END(_data)

// =================================================================================
// Concepts for ranges and range properties
// =================================================================================

// Determines whether Type is a range, which means it can be used in range-based
// for loops and other range algorithms.
template <typename Type>
concept Range =
    requires(Type&& value) {
        { ranges::begin(value) } -> Iterator;
        { ranges::end(value) } -> SentinelFor<RangeIteratorT<Type>>;
    };

// Determines whether Type is an input range, which means it supports input iteration.
template <typename Type>
concept InputRange =
    Range<Type> &&
    InputIterator<RangeIteratorT<Type>>;

// Determines whether Type is an output range for Value, which means it supports output iteration.
template <typename Type, typename Value>
concept OutputRange =
    Range<Type> &&
    OutputIterator<RangeIteratorT<Type>, Value>;

// Determines whether Type is a borrowed range, which means it can be used after being moved from.
template <typename Type>
concept BorrowedRange =
    Range<Type> &&
    (meta::IsLvalueReferenceV<Type> || EnableBorrowedRange<meta::RemoveCvrefT<Type>>);

// SentinelT is the type of the sentinel for a given range type,
// which is used to determine the end of the range.
template <Range RangeType>
using SentinelT = decltype(ranges::end(meta::declval<RangeType&>()));

// RangeDifferenceT is the difference type associated with a given range type,
// which is used to represent the distance between two iterators in the range.
template <Range RangeType>
using RangeDifferenceT = IteratorDifferenceType<RangeIteratorT<RangeType>>;

// RangeValueT is the value type associated with a given range type,
// which is used to represent the type of elements in the range.
template <Range RangeType>
using RangeValueT = IteratorValueType<RangeIteratorT<RangeType>>;

// RangeReferenceT is the reference type associated with a given range type,
// which is used to represent the type of references to elements in the range.
template <Range RangeType>
using RangeReferenceT = IteratorReferenceType<RangeIteratorT<RangeType>>;

// Determines whether Type is a sized range, which means it has a known size.
template <typename Type>
concept SizedRange =
    Range<Type> &&
    requires(Type&& value) {
        { ranges::size(value) } -> ConvertibleTo<usize>;
    };

// RangeSizeT is the size type associated with a given sized range type,
// which is used to represent the number of elements in the range.
template <SizedRange RangeType>
using RangeSizeT = decltype(ranges::size(meta::declval<RangeType&>()));

// Determines whether Type is a view, which means it can be used in range-based
// for loops and other range algorithms without taking ownership of the underlying data.
template <typename Type>
concept View = NEX_STD ranges::view<Type>;

// Determines whether Type is a forward range,
// which means it supports input iteration and multiple passes.
template <typename Type>
concept ForwardRange =
    InputRange<Type> &&
    ForwardIterator<RangeIteratorT<Type>>;

// Determines whether Type is a bidirectional range,
// which means it supports forward iteration and backward iteration.
template <typename Type>
concept BidirectionalRange =
    ForwardRange<Type> &&
    BidirectionalIterator<RangeIteratorT<Type>>;

// Determines whether Type is a random-access range, which means
// it supports bidirectional iteration and random access to elements.
template <typename Type>
concept RandomAccessRange = 
    BidirectionalRange<Type> &&
    RandomAccessIterator<RangeIteratorT<Type>>;

// Determines whether Type is a common range, which means that the iterator and sentinel types are the same.
template <typename Type>
concept CommonRange =
    Range<Type> &&
    SameAs<RangeIteratorT<Type>, SentinelT<Type>>;

// Determines whether Type is a viewable range, which means it can be used in range-based
// for loops and other range algorithms without taking ownership of the underlying data.
template <typename Type>
concept ViewableRange =
    Range<Type> &&
    ((View<meta::RemoveCvrefT<Type>> && ConstructibleFrom<meta::RemoveCvrefT<Type>, Type>) ||
        (!View<meta::RemoveCvrefT<Type>> &&
        (meta::IsLvalueReferenceV<Type> ||
        (Movable<meta::RemoveReferenceT<Type>> &&
        !InitListType<meta::RemoveCvrefT<Type>>))
    ));

// Determines whether Type is a container-like range, which means it has a size() member function
// and a value_type member type.
template <typename Type>
concept ContainerLike =
    Range<Type> &&
    requires(Type& value) {
        typename meta::RemoveCvrefT<Type>::value_type;
        { value.size() } -> ConvertibleTo<usize>;
    };

NEX_SUBNAMESPACE_END(ranges)

NEX_NAMESPACE_END
