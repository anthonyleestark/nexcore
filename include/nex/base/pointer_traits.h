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
#include "nex/base/memory.h"

NEX_NAMESPACE_BEGIN

// Implementation of PointerTraitsElementType
template <class Pointer>
struct NEX_HIDDEN_FROM_ABI PointerTraitsElementTypeImpl {};

// Specialization of PointerTraitsElementTypeImpl for template classes
// that take a type as their first template parameter.
template <template <class, class...> class Type1, class Type2, class... _Args>
struct NEX_HIDDEN_FROM_ABI PointerTraitsElementTypeImpl<Type1<Type2, _Args...> > {
    using type NEX_NODEBUG = Type2;
};

// Primary template for PointerTraitsElementType, which determines the element type of a pointer-like type.
template <class Pointer, class = void>
struct PointerTraitsElementType : PointerTraitsElementTypeImpl<Pointer> {};

// Specialization of PointerTraitsElementType for types that have a member type named `element_type`.
template <class Pointer>
struct PointerTraitsElementType<Pointer, meta::VoidT<typename Pointer::element_type> > {
    using type NEX_NODEBUG = typename Pointer::element_type;
};

// Primary template for PointerTraitsRebindImpl (implementation detail),
// which provides a mechanism to rebind a pointer-like type to a different element type.
template <class Type1, class Type2>
struct NEX_HIDDEN_FROM_ABI PointerTraitsRebindImpl {
    static_assert(false, "Cannot rebind pointer; did you forget to add a rebind member to your pointer?");
};

// Specialization of PointerTraitsRebindImpl for template classes that take a type as their first template parameter.
template <template <class, class...> class Type1, class Type2, class... Args, class Type3>
struct NEX_HIDDEN_FROM_ABI PointerTraitsRebindImpl<Type1<Type2, Args...>, Type3> {
    using type NEX_NODEBUG = Type1<Type3, Args...>;
};

// Primary template for PointerTraitsRebind,
// which provides a mechanism to rebind a pointer-like type to a different element type.
template <class Type1, class Type2, class = void>
struct PointerTraitsRebind : PointerTraitsRebindImpl<Type1, Type2> {};

// Specialization of PointerTraitsRebind for types that have a member template named `rebind`.
template <class Type1, class Type2>
struct PointerTraitsRebind<Type1, Type2, meta::VoidT<typename Type1::template rebind<Type2> > > {
    using type NEX_NODEBUG = typename Type1::template rebind<Type2>;
};

// PointerDifferenceTypeMember is a helper type trait
// that extracts the `difference_type` member from a pointer-like type.
template <class Pointer>
using PointerDifferenceTypeMember = typename Pointer::difference_type;

// Helper struct NAT (Not A Type) to represent a type that cannot be instantiated.
struct NEX_HIDDEN_FROM_ABI NAT {
    NAT()                      = delete;
    NAT(const NAT&)            = delete;
    NAT& operator=(const NAT&) = delete;
    ~NAT()                     = delete;
};

// Primary template for PointerTraitsImpl (implementation detail).
template <class Pointer, class = void>
struct PointerTraitsImpl {};

// Specialization of PointerTraitsImpl for pointer-like types that have an `element_type` member.
template <class Pointer>
struct PointerTraitsImpl<Pointer, meta::VoidT<typename PointerTraitsElementType<Pointer>::type> > {
    typedef Pointer pointer;
    typedef typename PointerTraitsElementType<pointer>::type element_type;
    using difference_type = meta::DetectedOrT<ptrdiff_t, PointerDifferenceTypeMember, pointer>;

    template <class Type>
    using rebind = typename PointerTraitsRebind<pointer, Type>::type;

public:
    // Returns a pointer to the given reference, using the pointer's `pointer_to` member function.
    NEX_HIDDEN_FROM_ABI constexpr static pointer
    pointerTo(meta::ConditionalT<meta::IsVoidV<element_type>, NAT, element_type>& r) {
        return pointer::pointerTo(r);
    }
};

// Primary template for PointerTraits,
// which provides a uniform interface for working with pointer-like types.
template <class Pointer>
struct PointerTraits : PointerTraitsImpl<Pointer> {};

// Specialization of PointerTraits for raw pointer types.
template <class Type>
struct PointerTraits<Type*> {
    using pointer         = Type*;
    using element_type    = Type;
    using difference_type = isize;

    template <class Type2>
    using rebind = Type2*;

public:
    // Returns the address of the given reference.
    NEX_HIDDEN_FROM_ABI constexpr static pointer
    pointerTo(meta::ConditionalT<meta::IsVoidV<element_type>, NAT, element_type>& r) {
        return NEX_ADDRESS_OF(r);
    }
};

// RebindPointerT is a helper type trait that provides a convenient way
// to rebind a pointer-like type to a different element type.
template <class From, class To>
using RebindPointerT = typename PointerTraits<From>::template rebind<To>;

template <class Pointer, class = void>
struct NEX_HIDDEN_FROM_ABI ToAddressHelper {};

// Internal implementation of `toAddress` function.
template <class Type>
NEX_HIDDEN_FROM_ABI constexpr Type* toAddressImpl(Type* ptr) noexcept {
    static_assert(!meta::IsFunctionV<Type>, "Error: Type is a function type");
    return ptr;
}

// Primary template for `HasToAddress` trait.
template <class Pointer, class = void>
struct HasToAddress : meta::FalseType {};

// Specialization of `HasToAddress` for pointer-like types that have a `toAddress` member function.
template <class Pointer>
struct HasToAddress<
    Pointer,
    decltype((void)PointerTraits<Pointer>::toAddress(meta::declval<const Pointer&>()))
> : meta::TrueType {};

// Primary template for `HasArrow` trait.
template <class Pointer, class = void>
struct HasArrow : meta::FalseType {};

// Specialization of `HasArrow` for pointer-like types that have an `operator->`.
template <class Pointer>
struct HasArrow<
    Pointer,
    decltype((void)meta::declval<const Pointer&>().operator->())
> : meta::TrueType {};

// Helper trait to determine if a pointer-like type is a "fancy" pointer (i.e., has `operator->` or `toAddress`).
template <class Pointer>
struct IsFancyPointer {
    static const bool value = HasArrow<Pointer>::value || HasToAddress<Pointer>::value;
};

// Helper variable template for `IsFancyPointer`.
template <class Pointer>
inline constexpr bool IsFancyPointerV = IsFancyPointer<Pointer>::value;

// Implementation of `toAddress` function for fancy pointers.
template <class Pointer>
    requires(meta::IsClassV<Pointer> && IsFancyPointerV<Pointer>)
NEX_HIDDEN_FROM_ABI constexpr
meta::DecayT<
    decltype(ToAddressHelper<Pointer>::call(meta::declval<const Pointer&>()))
> toAddressImpl(const Pointer& ptr) noexcept {
    return ToAddressHelper<Pointer>::call(ptr);
}

// Implementation of `toAddress` function for raw pointers.
template <class Pointer, class>
struct NEX_HIDDEN_FROM_ABI ToAddressHelper {
    NEX_HIDDEN_FROM_ABI constexpr
    static decltype(meta::toAddressImpl(meta::declval<const Pointer&>().operator->()))
    call(const Pointer& ptr) noexcept {
        return meta::toAddressImpl(ptr.operator->());
    }
};

// Implementation of `toAddress` function for pointer-like types that have a `toAddress` member function.
template <class Pointer>
struct NEX_HIDDEN_FROM_ABI ToAddressHelper<
    Pointer,
    decltype((void)PointerTraits<Pointer>::toAddress(meta::declval<const Pointer&>()))
> {
    // Returns the address of the object pointed to by the given pointer-like type.
    NEX_HIDDEN_FROM_ABI constexpr
    static decltype(PointerTraits<Pointer>::toAddress(meta::declval<const Pointer&>()))
    call(const Pointer& ptr) noexcept {
        return PointerTraits<Pointer>::toAddress(ptr);
    }
};

/**
 * @brief Returns the address of the object pointed to by a pointer-like type.
 * @tparam Type The type of the pointer-like object.
 * @param ptr The pointer-like object to get the address from.
 * @return The address of the object pointed to by the pointer-like type.
 * @note This function is a wrapper around the `toAddressImpl` function and provides a convenient way to obtain
 *       the address for various types of pointer-like objects, including raw pointers and fancy pointers
 */
template <class Type>
inline NEX_HIDDEN_FROM_ABI constexpr auto toAddress(Type* ptr) noexcept {
    return toAddressImpl(ptr);
}

/**
 * @brief Returns the address of the object pointed to by a pointer-like type.
 * @tparam Pointer The type of the pointer-like object.
 * @param ptr The pointer-like object to get the address from.
 * @return The address of the object pointed to by the pointer-like type.
 * @note This function is a wrapper around the `toAddressImpl` function and provides a convenient way to obtain
 *       the address for various types of pointer-like objects, including raw pointers and fancy pointers
 */
template <class Pointer>
inline NEX_HIDDEN_FROM_ABI constexpr
auto toAddress(const Pointer& ptr) noexcept -> decltype(toAddressImpl(ptr)) {
    return toAddressImpl(ptr);
}

// Returns the address of the object pointed to by a pointer-like type.
#define NEX_TO_ADDRESS(ptr) \
    NEX_PREPEND_NAMESPACE(toAddress)(ptr)

NEX_NAMESPACE_END
