/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/compiler.h"
#include "nex/base/export.h"
#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/casts.h"

NEX_NAMESPACE_BEGIN

/**
 * @class  Reference
 * @brief  A wrapper that makes references "assignable" and "copyable".
 * 
 * @details 
 * Enables storing references in standard containers (like Vec) which otherwise require elements 
 * to be erasable/assignable. It acts as a non-nullable pointer with reference semantics.
 * 
 * @tparam ObjectType The type of the object being referenced.
 * 
 * @note 
 * This class is designed to be used in contexts where references need to be stored in containers or 
 * passed around, while still maintaining the semantics of a reference. It provides a safe and efficient 
 * way to manage references without the overhead of pointers or the risk of dangling references.
 */
template <typename ObjectType>
class NEX_API Reference {
public:
    using value_type = ObjectType;
    using pointer_type = value_type*;
    using const_pointer_type = const value_type*;
    using reference_type = value_type&;
    using const_reference_type = const value_type&;

private:
    value_type* ptr_;

    static void foo(value_type&) noexcept;
    static void foo(value_type&&) = delete; // NOLINT(modernize-use-equals-delete)

public:
    // Construct a Reference from an lvalue of the underlying type, 
    // ensuring that the object is not a temporary.
    template <class AnyObject,
        class = meta::VoidT<decltype(foo(meta::declval<AnyObject>()))>,
        meta::EnableIfT<!meta::IsSameV<meta::RemoveCvrefT<AnyObject>, Reference>, int> = 0>
    NEX_HIDDEN_FROM_ABI constexpr Reference(AnyObject&& obj)
        noexcept(noexcept(foo(meta::declval<AnyObject>()))) {
        reference_type ref = static_cast<AnyObject&&>(obj);
        ptr_ = NEX_ADDRESS_OF(ref);
    }

    // Default copy semantics
    NEX_HIDDEN_FROM_ABI constexpr Reference(const Reference&) noexcept = default;
    NEX_HIDDEN_FROM_ABI constexpr Reference& operator=(const Reference&) noexcept = default;

    // Access the underlying object through dereference semantics
    NEX_HIDDEN_FROM_ABI constexpr reference_type operator*() const noexcept { return *ptr_; }

    // Access the underlying object through pointer semantics
    NEX_HIDDEN_FROM_ABI constexpr pointer_type operator->() const noexcept { return ptr_; }

    // Implicit conversion to a reference of the underlying type
    NEX_HIDDEN_FROM_ABI constexpr operator value_type&() const noexcept { return *ptr_; }

    // Get the underlying object as a reference
    NEX_HIDDEN_FROM_ABI constexpr reference_type get() const noexcept { return *ptr_; }

    // Invoke the underlying object if it is callable, forwarding arguments
    template <typename... Args>
    NEX_HIDDEN_FROM_ABI constexpr auto operator()(Args&&... args) const 
        -> decltype((*ptr_)(NEX_FORWARD<Args>(args)...)) {
        return (*ptr_)(NEX_FORWARD<Args>(args)...);
    }

    // Equality comparison operator for Reference objects
    NEX_HIDDEN_FROM_ABI friend constexpr 
    bool operator==(Reference lhs, Reference rhs) 
        requires requires {
            { lhs.get() == rhs.get() } -> meta::ConvertibleTo<bool>;
        }
    {
        return lhs.get() == rhs.get();
    }

    // Equality comparison operator for Reference and another type, 
    // ensuring the other type is not a Reference, and that the comparison is valid.
    template <typename OtherType>
    NEX_HIDDEN_FROM_ABI friend constexpr 
    bool operator==(Reference lhs, const OtherType& rhs)
        requires (!meta::IsSameV<meta::RemoveCvrefT<OtherType>, Reference>)
            && requires {
                { lhs.get() == rhs } -> meta::ConvertibleTo<bool>;
            }
    {
        return lhs.get() == rhs;
    }

    // Equality comparison operator for Reference and a const Reference of another type,
    // ensuring the other type is not const, and that the comparison is valid.
    template <typename OtherType>
    NEX_HIDDEN_FROM_ABI friend constexpr 
    bool operator==(Reference lhs, Reference<const OtherType> rhs)
        requires(!meta::IsSameV<meta::RemoveCvrefT<OtherType>, Reference> && !meta::IsConstV<OtherType>)
            && requires {
                { lhs.get() == rhs.get() } -> meta::ConvertibleTo<bool>;
            }
    {
        return lhs.get() == rhs.get();
    }
};

/**
 * @brief A wrapper for constant references, ensuring read-only access.
 * @details 
 * Similar to Reference, but holds a 'const T&'. 
 * Useful for storing collections of read-only observers without the overhead of pointers.
 */
template <typename ObjectType>
using ConstReference = Reference<const ObjectType>;

// Allow deduction of the template parameter for Reference when constructed from an lvalue of the underlying type.
template <typename ObjectType>
Reference(ObjectType&) -> Reference<ObjectType>;

// Generates a Reference from a given object type.
template <class ObjectType>
inline NEX_HIDDEN_FROM_ABI constexpr 
auto ref(ObjectType& obj) noexcept -> Reference<ObjectType> {
    return Reference<ObjectType>(obj);
}

// Generates a Reference from a given Reference object, which also returns a Reference of the same type.
template <class ObjectType>
inline NEX_HIDDEN_FROM_ABI constexpr 
auto ref(Reference<ObjectType> obj) noexcept -> Reference<ObjectType> {
    return obj;
}

// Generates a ConstReference from a given object type.
template <class ObjectType>
inline NEX_HIDDEN_FROM_ABI constexpr 
auto cref(const ObjectType& obj) noexcept -> ConstReference<ObjectType> {
    return ConstReference<ObjectType>(obj);
}

// Generates a ConstReference from a given Reference object, which returns a ConstReference of the same type.
template <class ObjectType>
inline NEX_HIDDEN_FROM_ABI constexpr 
auto cref(Reference<ObjectType> obj) noexcept -> ConstReference<ObjectType> {
    return ConstReference<ObjectType>(obj);
}

// Delete overload for rvalue references to prevent creating References from temporaries, 
// which would lead to dangling references.
template <class ObjectType>
void ref(const ObjectType&&) = delete;

// Delete overload for rvalue references to prevent creating ConstReferences from temporaries, 
// which would lead to dangling references.
template <class ObjectType>
void cref(const ObjectType&&) = delete;

// Type trait to determine if a type is a Reference wrapper.
template <class Type>
struct IsRefWrapper : meta::BoolConstant<meta::IsSpecializationV<Type, Reference>> {};

// Variable template for easier usage of IsRefWrapper.
template <class Type>
inline constexpr bool IsRefWrapperV = IsRefWrapper<Type>::value;

// Type trait to determine if a type is a ConstReference wrapper.
template <class Type>
struct IsConstRefWrapper : meta::BoolConstant<meta::IsSpecializationV<Type, ConstReference>> {};

// Variable template for easier usage of IsConstRefWrapper.
template <class Type>
inline constexpr bool IsConstRefWrapperV = IsConstRefWrapper<Type>::value;

NEX_NAMESPACE_END
