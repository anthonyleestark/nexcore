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
#include "nex/base/utility.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(ranges)

// Determines whether BorrowedRange is enabled for a given type.
// By default, this is false for all types.
template <class>
inline constexpr bool EnableBorrowedRange = false;

// Determines whether a Type can provide iterators/sentinels from an rvalue range
// without requiring the range object itself to remain alive.
template <class Type>
concept CanBorrow = meta::IsLvalueReferenceV<Type> || EnableBorrowedRange<meta::RemoveCvrefT<Type>>;

// =================================================================================
// Range access utilities [ranges::begin() / end() / cbegin() / cend()]
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
// Range size utilities [ranges::size() / ranges::ssize()]
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
concept SizedByDifference =
    !MemberSize<Type> && !UnqualifiedSize<Type> &&
    meta::ClassOrEnum<meta::RemoveCvrefT<Type>> && requires(Type&& value) {
        { ranges::begin(value) } -> InputIterator;
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
    template <SizedByDifference Type>
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
// Range empty utilities [ranges::empty()]
// =================================================================================

NEX_SUBNAMESPACE_BEGIN(_empty)

// Determines whether Type has a member function `empty()` that returns a boolean-like value.
template <class Type>
concept MemberEmpty = requires(Type&& value) {
    { value.empty() } -> SameAs<bool>;
};

// Determines whether Type has a member function `size()` that returns an integer-like value,
// which can be used to determine emptiness.
template <class Type>
concept CanInvokeSize =
    !MemberEmpty<Type> &&
    requires(Type&& value) {
        ranges::size(value);
    };

// Determines whether Type can be used with unqualified `begin()` and `end()` function calls
// to determine emptiness.
template <class Type>
concept CanCompareBeginEnd =
    !MemberEmpty<Type> && !CanInvokeSize<Type> &&
    requires(Type&& value) {
        { ranges::begin(value) == ranges::end(value) } -> SameAs<bool>;
        { ranges::begin(value) } -> InputIterator;
    };

struct Fn {
    /**
     * @brief Returns whether a range-like object is empty by invoking its member `empty()` function.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to check for emptiness.
     * @return `true` if the range-like object is empty, `false` otherwise.
     * @note This overload is only enabled for types that have a member `empty()` function.
     */
    template <MemberEmpty Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator()(Type&& value) const
        noexcept(noexcept(bool(value.empty()))) {
        return bool(value.empty());
    }

    /**
     * @brief Returns whether a range-like object is empty by invoking its member `size()` function.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to check for emptiness.
     * @return `true` if the range-like object is empty, `false` otherwise.
     * @note This overload is only enabled for types that have a member `size()` function
     *       and do not have a member `empty()` function.
     */
    template <CanInvokeSize Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator()(Type&& value) const
        noexcept(noexcept(ranges::size(value) == 0)) {
        return ranges::size(value) == 0;
    }

    /**
     * @brief Returns whether a range-like object is empty by comparing its `begin()` and `end()` iterators.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to check for emptiness.
     * @return `true` if the range-like object is empty, `false` otherwise.
     * @note This overload is only enabled for types that can be used with unqualified `begin()` and `end()` calls
     *       and do not have member `empty()` or `size()` functions.
     */
    template <CanCompareBeginEnd Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool operator()(Type&& value) const
        noexcept(noexcept(ranges::begin(value) == ranges::end(value))) {
        return ranges::begin(value) == ranges::end(value);
    }
};

NEX_SUBNAMESPACE_END(_empty)

/**
 * @brief Returns whether a range-like object is empty.
 * @tparam Type The type of the range-like object.
 * @param value The range-like object to check for emptiness.
 * @return `true` if the range-like object is empty, `false` otherwise.
 * @note 
 * This function is a wrapper around the `_empty::Fn` struct and provides a convenient
 * way to check for emptiness for various types of range-like objects, including those
 * with member `empty()` functions, those with member `size()` functions, and those
 * that can be used with unqualified `begin()` and `end()` calls. The return type is
 * always a boolean value indicating whether the range-like object is empty.
 */
inline constexpr auto empty = _empty::Fn{};

// =================================================================================
// Range data utilities [ranges::data() / ranges::cdata()]
// =================================================================================

NEX_SUBNAMESPACE_BEGIN(_data)

// Determines whether Type is a pointer to an object, which means it points to a valid object in memory.
template <class Type>
concept PointerToObject = meta::IsPointerV<Type> && meta::IsObjectV<meta::RemovePointerT<Type>>;

// Determines whether Type has a member function `data()` that returns a pointer to an object.
template <class Type>
concept MemberData = CanBorrow<Type> && requires(Type&& value) {
    { NEX_AUTO_CAST(value.data()) } -> PointerToObject;
};

// Determines whether Type can be used with unqualified `data()` function calls.
template <class Type>
concept RangesBeginInvocable =
    !MemberData<Type> && CanBorrow<Type> &&
    requires(Type&& value) {
        { ranges::begin(value) } -> ContiguousIterator;
    };

struct Fn {
    /**
     * @brief Returns a pointer to the underlying data of a range-like object that has a member `data()` function.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the data pointer from.
     * @return A pointer to the underlying data of the range-like object.
     * @note This overload is only enabled for types that have a member `data()` function and can be borrowed.
     */
    template <MemberData Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(value.data()))
            -> decltype(value.data()) {
        return value.data();
    }

    /**
     * @brief Returns a pointer to the underlying data of a range-like object that can be used
     *        with unqualified `begin()` calls.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the data pointer from.
     * @return A pointer to the underlying data of the range-like object.
     * @note This overload is only enabled for types that can be used with unqualified `begin()` calls
     *       and can be borrowed.
     */
    template <RangesBeginInvocable Type>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(NEX_TO_ADDRESS(ranges::begin(value))))
            -> decltype(NEX_TO_ADDRESS(ranges::begin(value))) {
        return NEX_TO_ADDRESS(ranges::begin(value));
    }
};

NEX_SUBNAMESPACE_END(_data)

/**
 * @brief Returns a pointer to the underlying data of a range-like object.
 * @tparam Type The type of the range-like object.
 * @param value The range-like object to get the data pointer from.
 * @return A pointer to the underlying data of the range-like object.
 * @note 
 * This function is a wrapper around the `_data::Fn` struct and provides a convenient
 * way to obtain a pointer to the underlying data for various types of range-like objects,
 * including those with member `data()` functions and those that can be used with unqualified
 * `begin()` calls. The return type is determined based on the underlying data type, ensuring
 * that it is appropriately typed.
 */
inline constexpr auto data = _data::Fn{};

NEX_SUBNAMESPACE_BEGIN(_cdata)

struct Fn {
    /**
     * @brief Returns const pointer to the underlying data of an l-value range-like object.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the const data pointer from.
     * @return A const pointer to the underlying data of the range-like object.
     * @note This overload is only enabled for l-value range-like objects.
     */
    template <class Type>
        requires meta::IsLvalueReferenceV<Type&&>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(ranges::data(static_cast<const meta::RemoveReferenceT<Type>&>(value))))
            -> decltype(ranges::data(static_cast<const meta::RemoveReferenceT<Type>&>(value))) {
        return ranges::data(static_cast<const meta::RemoveReferenceT<Type>&>(value));
    }

    /**
     * @brief Returns const pointer to the underlying data of an r-value range-like object.
     * @tparam Type The type of the range-like object.
     * @param value The range-like object to get the const data pointer from.
     * @return A const pointer to the underlying data of the range-like object.
     * @note This overload is only enabled for r-value range-like objects.
     */
    template <class Type>
        requires meta::IsRvalueReferenceV<Type&&>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto operator()(Type&& value) const
        noexcept(noexcept(ranges::data(static_cast<const Type&&>(value))))
            -> decltype(ranges::data(static_cast<const Type&&>(value))) {
        return ranges::data(static_cast<const Type&&>(value));
    }
};

NEX_SUBNAMESPACE_END(_cdata)

/**
 * @brief Returns a const pointer to the underlying data of a range-like object.
 * @tparam Type The type of the range-like object.
 * @param value The range-like object to get the const data pointer from.
 * @return A const pointer to the underlying data of the range-like object.
 * @note 
 * This function is a wrapper around the `_cdata::Fn` struct and provides a convenient
 * way to obtain a const pointer to the underlying data for various types of range-like objects,
 * including those with member `data()` functions and those that can be used with unqualified
 * `begin()` calls. The return type is determined based on the underlying data type, ensuring
 * that it is appropriately typed.
 */
inline constexpr auto cdata = _cdata::Fn{};

// =================================================================================
// View base / interface declarations
// =================================================================================

// Base class for all view types.
struct ViewBase {};

// Forward declaration of the ViewInterface class template,
// which provides a common interface for view-like objects through CRTP.
template <class Derived>
    requires(meta::IsClassV<Derived> && SameAs<Derived, meta::RemoveCvT<Derived>>)
class ViewInterface;

// Forward declaration of the isDerivedFromViewInterface function template,
// which determines whether a given type is derived from the ViewInterface class template.
template <class Type1, class Type2>
    requires(!SameAs<Type1, ViewInterface<Type2>>)
void isDerivedFromViewInterface(ViewInterface<Type2>*);

// Determines whether View is enabled for a given type.
template <class Type>
inline constexpr bool EnableView =
    DerivedFrom<Type, ViewBase> ||
    requires {
        ranges::isDerivedFromViewInterface<meta::RemoveCvT<Type>>(
            static_cast<meta::RemoveCvT<Type>*>(nullptr)
        );
    };

// =================================================================================
// Concepts for ranges and range properties
// =================================================================================

// Determines whether Type is a range, which means it can be used in range-based
// for loops and other range algorithms.
template <typename Type>
concept Range =
    requires(Type&& value) {
        { ranges::begin(value) } -> Iterator;  // TODO: Should be tested first. 
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

// Determines whether Type is a borrowed range, which permits obtaining iterators/sentinels
// from an rvalue range without requiring the range object itself to remain alive.
template <typename Type>
concept BorrowedRange =
    Range<Type> &&
    CanBorrow<Type>;

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
concept View =
    Range<Type> &&
    Movable<Type> &&
    EnableView<Type>;

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

template <typename Type>
concept ContiguousRange =
    RandomAccessRange<Type> &&
    ContiguousIterator<RangeIteratorT<Type>> &&
    requires(Type&& value) {
        { ranges::data(value) } -> SameAs<meta::AddPointerT<RangeReferenceT<Type>>>;
    };

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

// =================================================================================
// Range advance utilities [ranges::advance()]
// =================================================================================

/**
 * @brief Advances an input iterator by a specified number of steps.
 * @tparam InputIter The type of the input iterator.
 * @param it The input iterator to advance.
 * @param n The number of steps to advance the iterator.
 */
template <InputIterator InputIter>
NEX_HIDDEN_FROM_ABI constexpr
void advanceImpl(InputIter& it, typename IteratorTraits<InputIter>::difference_type n) {
    for (; n > 0; --n) {
        ++it;  // Advance the iterator forward by incrementing it.
    }
}

/**
 * @brief Advances a bidirectional iterator by a specified number of steps.
 * @tparam BiDirIter The type of the bidirectional iterator.
 * @param it The bidirectional iterator to advance.
 * @param n The number of steps to advance the iterator. Can be negative for backward movement.
 */
template <BidirectionalIterator BiDirIter>
NEX_HIDDEN_FROM_ABI constexpr
void advanceImpl(BiDirIter& it, typename IteratorTraits<BiDirIter>::difference_type n) {
    if (n >= 0) {
        for (; n > 0; --n) {
            ++it;  // Advance the iterator forward by incrementing it.
        }
    } else {
        for (; n < 0; ++n) {
            --it;  // Move the iterator backward by decrementing it.
        }
    }
}

/**
 * @brief Advances a random access iterator by a specified number of steps.
 * @tparam RandIter The type of the random access iterator.
 * @param it The random access iterator to advance.
 * @param n The number of steps to advance the iterator. Can be negative for backward movement.
 */
template <RandomAccessIterator RandIter>
NEX_HIDDEN_FROM_ABI constexpr
void advanceImpl(RandIter& it, typename IteratorTraits<RandIter>::difference_type n) {
    it += n;  // Advance the iterator by adding the difference `n` to it.
}


/**
 * @brief Advances an iterator by a specified number of steps.
 * @tparam InputIter The type of the input iterator.
 * @tparam Distance The type of the distance to advance the iterator.
 * @param it The iterator to advance.
 * @param origN The number of steps to advance the iterator. Can be negative for bidirectional iterators.
 * @note 
 * This function advances the given iterator `it` by `origN` steps. If `origN` is positive, the iterator is advanced forward;
 * if `origN` is negative and the iterator is bidirectional, it is moved backward. The function ensures that the advancement
 * respects the capabilities of the iterator type.
 */
template <
    class InputIter,
    class Distance,
    class IntegralDistance = decltype(convertToIntegral(meta::declval<Distance>()))
> requires(Integral<IntegralDistance>)
NEX_HIDDEN_FROM_ABI constexpr void advance(InputIter& it, Distance origN) {
    using Difference = typename IteratorTraits<InputIter>::difference_type;
    Difference n = static_cast<Difference>(convertToIntegral(origN));
    if constexpr (!BidirectionalIterator<InputIter>) {
        NEX_ASSERT_MSG(n >= 0, "Error: Can only pass a negative `n` with a BidirectionalIterator.");
    }
    advanceImpl(it, n);
}

NEX_SUBNAMESPACE_BEGIN(ranges)

struct AdvanceImpl {
private:
    // Helper function to move the iterator forward by `n` steps.
    template <class Iter>
    NEX_HIDDEN_FROM_ABI static constexpr void forward(Iter& it, IteratorDifferenceType<Iter> n) {
        while (n > 0) {
            --n; ++it;
        }
    }

    // Helper function to move the iterator backward by `n` steps.
    template <class Iter>
    NEX_HIDDEN_FROM_ABI static constexpr void backward(Iter& it, IteratorDifferenceType<Iter> n) {
        while (n < 0) {
            ++n; --it;
        }
    }

public:
    /**
     * @brief Advances the given iterator by `n` steps.
     * @tparam Iter The type of the iterator.
     * @param it The iterator to advance.
     * @param n The number of steps to advance the iterator. Can be negative for bidirectional iterators.
     */
    template <Iterator Iter>
    NEX_HIDDEN_FROM_ABI constexpr void operator()(Iter& it, IteratorDifferenceType<Iter> n) const {
        // If the iterator is a random access iterator, equivalent to `it += n`.
        if constexpr (RandomAccessIterator<Iter>) {
            it += n;
            return;
        } else if constexpr (BidirectionalIterator<Iter>) {
            // Otherwise, if `n` is non-negative, move forward.
            if (n >= 0) {
                forward(it, n);
            } else {
                // Otherwise, move backward.
                backward(it, n);
            }
            return;
        } else {
            NEX_ASSERT_MSG(n >= 0, "Error: Can only pass a negative `n` with a BidirectionalIterator.");
            // Otherwise, if `n` is non-negative, move forward.
            forward(it, n);
            return;
        }
    }

    /**
     * @brief Advances the given iterator to the specified sentinel.
     * @tparam Iter The type of the iterator.
     * @tparam Sentinel The type of the sentinel.
     * @param it The iterator to advance.
     * @param bound The sentinel to advance the iterator to.
     */
    template <Iterator Iter, SentinelFor<Iter> Sentinel>
    NEX_HIDDEN_FROM_ABI constexpr void operator()(Iter& it, Sentinel bound) const {
        // If the iterator can be assigned from the sentinel, assign it directly.
        if constexpr (AssignableFrom<Iter&, Sentinel>) {
            it = NEX_MOVE(bound);
        } else if constexpr (SizedSentinelFor<Sentinel, Iter>) {
            // Otherwise, if the sentinel is a sized sentinel for the iterator,
            // advance the iterator by the difference.
            (*this)(it, bound - it);
        } else {
            // Otherwise, advance the iterator until it reaches the sentinel.
            while (it != bound) {
                ++it;
            }
        }
    }

    /**
     * @brief Advances the given iterator by `n` steps or until it reaches the specified sentinel.
     * @tparam Iter The type of the iterator.
     * @tparam Sentinel The type of the sentinel.
     * @param it The iterator to advance.
     * @param n The number of steps to advance the iterator. Can be negative for bidirectional iterators.
     * @param bound The sentinel to advance the iterator to.
     * @return The remaining number of steps after advancing the iterator, which is `n - diff`,
     *         where `diff` is the difference between the ending and starting position.
     * @note
     * Preconditions:
     *     * If `n > 0`, [i, bound) denotes a range.
     *     * If `n == 0`, [i, bound) or [bound, i) denotes a range.
     *     * If `n < 0`, [bound, i) denotes a range, `Iter` models `BidirectionalIterator`,
     *       and `Iter` and `Sentinel` model `SameAs<Iter, Sentinel>`.
     * Returns: `n - diff`, where `diff` is the difference between the ending and starting position.
     */
    template <Iterator Iter, SentinelFor<Iter> Sentinel>
    NEX_HIDDEN_FROM_ABI constexpr
    IteratorDifferenceType<Iter> operator()(Iter& it, IteratorDifferenceType<Iter> n, Sentinel bound) const {
        if constexpr (BidirectionalIterator<Iter> && SameAs<Iter, Sentinel>) {
            // negative `n` is allowed for bidirectional iterators
        } else {
            NEX_VERIFY_MSG(n >= 0, "Error: Can only pass a negative `n` with a BidirectionalIterator.");
        }

        // If the sentinel is a sized sentinel for the iterator, calculate the difference
        if constexpr (SizedSentinelFor<Sentinel, Iter>) {
            auto magnitudeGeq = [](auto a, auto b) {
                return a == 0 ? b == 0 : a > 0 ? a >= b : a <= b; 
            };

            // If the magnitude of `n` is greater than or equal to the difference between the iterator and sentinel,
            // advance the iterator to the sentinel and return the remaining `n`.
            if (const auto diff = bound - it; magnitudeGeq(n, diff)) {
                (*this)(it, bound);
                return n - diff;
            }

            // Otherwise, advance the iterator by `n` and return 0.
            (*this)(it, n);
            return 0;
        } else {
            // Otherwise, advance the iterator until it reaches the sentinel or `n` becomes zero.
            while (n > 0 && it != bound) {
                ++it; --n;
            }

            // If the iterator is a bidirectional iterator and the sentinel is the same type as the iterator,
            // move the iterator backward until it reaches the sentinel or `n` becomes zero.
            if constexpr (BidirectionalIterator<Iter> && SameAs<Iter, Sentinel>) {
                while (n < 0 && it != bound) {
                    --it; ++n;
                }
            }

            return n;
        }

        NEX_UNREACHABLE();
    }
};

/**
 * @brief Advances the given iterator by `n` steps or until it reaches the specified sentinel.
 * @tparam Iter The type of the iterator.
 * @tparam Sentinel The type of the sentinel.
 * @param it The iterator to advance.
 * @param n The number of steps to advance the iterator. Can be negative for bidirectional iterators.
 * @param bound The sentinel to advance the iterator to.
 * @return The remaining number of steps after advancing the iterator, which is `n - diff`,
 *         where `diff` is the difference between the ending and starting position.
 */
inline constexpr auto advance = AdvanceImpl{};

NEX_SUBNAMESPACE_END(ranges)

// =================================================================================
// Range prev utilities [ranges::prev()]
// =================================================================================

template <InputIterator InputIter>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr
InputIter prev(InputIter it, IteratorDifferenceType<InputIter> n) {
    advance(it, -n);
    return it;
}

template <InputIterator InputIter>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr InputIter prev(InputIter it) {
    static_assert(BidirectionalIterator<InputIter>, "Error: `prev(it)` requires a BidirectionalIterator.");
    return prev(NEX_MOVE(it), 1);
}

NEX_SUBNAMESPACE_BEGIN(ranges)

struct PrevImpl {
    /**
     * @brief Returns the previous iterator by decrementing the given bidirectional iterator.
     * @tparam Iter The type of the bidirectional iterator.
     * @param it The bidirectional iterator to decrement.
     * @return The previous iterator obtained by decrementing `it`.
     */
    template <BidirectionalIterator Iter>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Iter operator()(Iter it) const {
        return --it;
    }

    /**
     * @brief Returns the previous iterator by decrementing the given bidirectional iterator by `n` steps.
     * @tparam Iter The type of the bidirectional iterator.
     * @param it The bidirectional iterator to decrement.
     * @param n The number of steps to decrement the iterator.
     * @return The previous iterator obtained by decrementing `it` by `n` steps.
     */
    template <BidirectionalIterator Iter>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Iter operator()(Iter it, IteratorDifferenceType<Iter> n) const {
        ranges::advance(it, -n);
        return it;
    }

    /**
     * @brief Returns the previous iterator by decrementing the given bidirectional iterator by `n` steps,
     *        but not going beyond the specified sentinel.
     * @tparam Iter The type of the bidirectional iterator.
     * @tparam Sentinel The type of the sentinel.
     * @param it The bidirectional iterator to decrement.
     * @param n The number of steps to decrement the iterator.
     * @param bound The sentinel to limit the decrementing of the iterator.
     * @return The previous iterator obtained by decrementing `it` by `n` steps, but not going beyond `bound`.
     */
    template <BidirectionalIterator Iter>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    Iter operator()(Iter it, IteratorDifferenceType<Iter> n, Iter bound) const {
        ranges::advance(it, -n, bound);
        return it;
    }
};

/**
 * @brief Returns the previous iterator by decrementing the given bidirectional iterator.
 * @tparam Iter The type of the bidirectional iterator.
 * @param it The bidirectional iterator to decrement.
 * @return The previous iterator obtained by decrementing `it`.
 */
inline constexpr auto prev = PrevImpl{};

NEX_SUBNAMESPACE_END(ranges)

// =================================================================================
// Range next utilities [ranges::next()]
// =================================================================================

template <InputIterator InputIter>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI inline constexpr
InputIter next(InputIter it, IteratorDifferenceType<InputIter> n = 1) {
    advance(it, n);
    return it;
}

NEX_SUBNAMESPACE_BEGIN(ranges)

struct NextImpl {
    /**
     * @brief Returns the next iterator by incrementing the given input iterator.
     * @tparam Iter The type of the input iterator.
     * @param it The input iterator to increment.
     * @return The next iterator obtained by incrementing `it`.
     */
    template <InputIterator Iter>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Iter operator()(Iter it) const {
        return ++it;
    }

    /**
     * @brief Returns the next iterator by incrementing the given input iterator by `n` steps.
     * @tparam Iter The type of the input iterator.
     * @param it The input iterator to increment.
     * @param n The number of steps to increment the iterator.
     * @return The next iterator obtained by incrementing `it` by `n` steps.
     */
    template <InputIterator Iter>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Iter operator()(Iter it, IteratorDifferenceType<Iter> n) const {
        ranges::advance(it, n);
        return it;
    }

    /**
     * @brief Returns the next iterator by incrementing the given input iterator until
     *        it reaches the specified sentinel.
     * @tparam Iter The type of the input iterator.
     * @tparam Sentinel The type of the sentinel.
     * @param it The input iterator to increment.
     * @param bound The sentinel to limit the incrementing of the iterator.
     * @return The next iterator obtained by incrementing `it` until it reaches `bound`.
     */
    template <InputIterator Iter, SentinelFor<Iter> Sentinel>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Iter operator()(Iter it, Sentinel bound) const {
        ranges::advance(it, bound);
        return it;
    }

    /**
     * @brief Returns the next iterator by incrementing the given input iterator by `n` steps,
     *        but not going beyond the specified sentinel.
     * @tparam Iter The type of the input iterator.
     * @tparam Sentinel The type of the sentinel.
     * @param it The input iterator to increment.
     * @param n The number of steps to increment the iterator.
     * @param bound The sentinel to limit the incrementing of the iterator.
     * @return The next iterator obtained by incrementing `it` by `n` steps, but not going beyond `bound`.
     */
    template <InputIterator Iter, SentinelFor<Iter> Sentinel>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    Iter operator()(Iter it, IteratorDifferenceType<Iter> n, Sentinel bound) const {
        ranges::advance(it, n, bound);
        return it;
    }
};

/**
 * @brief Returns the next iterator by incrementing the given input iterator.
 * @tparam Iter The type of the input iterator.
 * @param it The input iterator to increment.
 * @return The next iterator obtained by incrementing `it`.
 */
inline constexpr auto next = NextImpl{};

NEX_SUBNAMESPACE_END(ranges)

// =================================================================================
// View interface definitions
// =================================================================================

NEX_SUBNAMESPACE_BEGIN(ranges)

template <class Derived>
    requires(meta::IsClassV<Derived> && SameAs<Derived, meta::RemoveCvT<Derived>>)
class ViewInterface {
private:
    // Helper function to obtain a reference to the derived class instance.
    NEX_HIDDEN_FROM_ABI constexpr Derived& derived() noexcept {
        static_assert(sizeof(Derived) && DerivedFrom<Derived, ViewInterface> && View<Derived>);
        return static_cast<Derived&>(*this);
    }

    // Helper function to obtain a const reference to the derived class instance.
    NEX_HIDDEN_FROM_ABI constexpr Derived const& derived() const noexcept {
        static_assert(sizeof(Derived) && DerivedFrom<Derived, ViewInterface> && View<Derived>);
        return static_cast<Derived const&>(*this);
    }

public:
    /**
     * @brief Determines whether the view is empty.
     * @tparam Type The type of the derived view class.
     * @return `true` if the view is empty, `false` otherwise.
     * @note This function is only enabled for types that are either sized ranges or forward ranges.
     */
    template <class Type = Derived>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool empty()
        requires requires(Type& view) { ranges::empty(view); }
    {
        if constexpr (SizedRange<Type>) {
            return ranges::size(derived()) == 0;
        } else {
            return ranges::begin(derived()) == ranges::end(derived());
        }
    }

    /**
     * @brief Determines whether the view is empty (const version).
     * @tparam Type The type of the derived view class.
     * @return `true` if the view is empty, `false` otherwise.
     * @note This function is only enabled for types that are either sized ranges or forward ranges.
     */
    template <class Type = Derived>
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr bool empty() const
        requires requires(const Type& view) { ranges::empty(view); }
    {
        if constexpr (SizedRange<const Type>) {
            return ranges::size(derived()) == 0;
        } else {
            return ranges::begin(derived()) == ranges::end(derived());
        }
    }

    /**
     * @brief Explicitly converts the view to a boolean value indicating whether it is not empty.
     * @tparam Type The type of the derived view class.
     * @return `true` if the view is not empty, `false` otherwise.
     * @note This operator is only enabled for types that are either sized ranges or forward ranges.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr explicit operator bool()
        requires requires(Type& view) { ranges::empty(view); }
    {
        return !ranges::empty(derived());
    }

    /**
     * @brief Explicitly converts the view to a boolean value indicating whether it is not empty (const version).
     * @tparam Type The type of the derived view class.
     * @return `true` if the view is not empty, `false` otherwise.
     * @note This operator is only enabled for types that are either sized ranges or forward ranges.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr explicit operator bool() const
        requires requires(const Type& view) { ranges::empty(view); }
    {
        return !ranges::empty(derived());
    }

    /**
     * @brief Returns a pointer to the underlying data of the view.
     * @tparam Type The type of the derived view class.
     * @return A pointer to the underlying data of the view.
     * @note This function is only enabled for types that are contiguous ranges.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr auto data()
        requires ContiguousIterator<Type>
    {
        return ranges::data(derived());
    }

    /**
     * @brief Returns a pointer to the underlying data of the view (const version).
     * @tparam Type The type of the derived view class.
     * @return A pointer to the underlying data of the view.
     * @note This function is only enabled for types that are contiguous ranges.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr auto data() const
        requires ContiguousIterator<const Type>
    {
        return ranges::data(derived());
    }

    /**
     * @brief Returns the size of the view.
     * @tparam Type The type of the derived view class.
     * @return The size of the view as an unsigned integer.
     * @note This function is only enabled for types that are forward ranges and have a sized sentinel.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr auto size()
        requires SizedRange<Type>
    {
        return ranges::size(derived());
    }

    /**
     * @brief Returns the size of the view (const version).
     * @tparam Type The type of the derived view class.
     * @return The size of the view as an unsigned integer.
     * @note This function is only enabled for types that are forward ranges and have a sized sentinel.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr auto size() const
        requires SizedRange<const Type>
    {
        return ranges::size(derived());
    }

    /**
     * @brief Returns a reference to the first element of the view.
     * @tparam Type The type of the derived view class.
     * @return A reference to the first element of the view.
     * @note This function is only enabled for types that are forward ranges.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr decltype(auto) front()
        requires ForwardRange<Type>
    {
        NEX_ASSERT_MSG(!empty(), "Error: Cannot call `.front()` on an empty view.");
        return *ranges::begin(derived());
    }

    /**
     * @brief Returns a reference to the first element of the view (const version).
     * @tparam Type The type of the derived view class.
     * @return A reference to the first element of the view.
     * @note This function is only enabled for types that are forward ranges.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr decltype(auto) front() const
        requires ForwardRange<const Type>
    {
        NEX_ASSERT_MSG(!empty(), "Error: Cannot call `.front()` on an empty view.");
        return *ranges::begin(derived());
    }

    /**
     * @brief Returns a reference to the last element of the view.
     * @tparam Type The type of the derived view class.
     * @return A reference to the last element of the view.
     * @note This function is only enabled for types that are bidirectional ranges and common ranges.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr decltype(auto) back()
        requires BidirectionalRange<Type> && CommonRange<Type>
    {
        NEX_ASSERT_MSG(!empty(), "Error: Cannot call `.back()` on an empty view.");
        return *ranges::prev(ranges::end(derived()));
    }

    /**
     * @brief Returns a reference to the last element of the view (const version).
     * @tparam Type The type of the derived view class.
     * @return A reference to the last element of the view.
     * @note This function is only enabled for types that are bidirectional ranges and common ranges.
     */
    template <class Type = Derived>
    NEX_HIDDEN_FROM_ABI constexpr decltype(auto) back() const
        requires BidirectionalRange<const Type> && CommonRange<const Type>
    {
        NEX_ASSERT_MSG(!empty(), "Error: Cannot call `.back()` on an empty view.");
        return *ranges::prev(ranges::end(derived()));
    }

    /**
     * @brief Returns a reference to the element at the specified index in the view.
     * @tparam RandAccRange The type of the derived view class, which must be a random access range.
     * @param index The index of the element to access.
     * @return A reference to the element at the specified index in the view.
     * @note This function is only enabled for types that are random access ranges.
     */
    template <RandomAccessRange RandAccRange = Derived>
    NEX_HIDDEN_FROM_ABI constexpr decltype(auto) operator[](RangeDifferenceT<RandAccRange> index) {
        return ranges::begin(derived())[index];
    }

    /**
     * @brief Returns a reference to the element at the specified index in the view (const version).
     * @tparam RandAccRange The type of the derived view class, which must be a random access range.
     * @param index The index of the element to access.
     * @return A reference to the element at the specified index in the view.
     * @note This function is only enabled for types that are random access ranges.
     */
    template <RandomAccessRange RandAccRange = const Derived>
    NEX_HIDDEN_FROM_ABI constexpr decltype(auto) operator[](RangeDifferenceT<RandAccRange> index) const {
        return ranges::begin(derived())[index];
    }
};

NEX_SUBNAMESPACE_END(ranges)

NEX_NAMESPACE_END
