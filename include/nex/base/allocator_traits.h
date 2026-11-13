/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file   allocator_traits.h
 * @brief  Defines allocator traits and related utility types for memory allocation.
 */

#include "nex/base/attributes.h"
#include "nex/base/namespace.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/casts.h"
#include "nex/base/pointer_traits.h"

NEX_NAMESPACE_BEGIN

// =================================================================================
// UsesAllocator implementation and related traits
// =================================================================================

/**
 * @struct HasAllocatorType
 * @brief  Checks if a type has an allocator_type member.
 * @tparam Type The type to check for an allocator_type member.
 */
template <class Type>
struct HasAllocatorType {
    static constexpr bool value = requires { typename Type::allocator_type; };
};

// Implementation detail for UsesAllocator: checks if a type uses a specific allocator.
template <class Type, class Alloc, bool = HasAllocatorType<Type>::value>
struct NEX_HIDDEN_FROM_ABI UsesAllocatorImpl
    : public meta::BoolConstant<meta::IsConvertible<Alloc, typename Type::allocator_type>::value> {};

// Specialization for when the type does not have an allocator_type member.
template <class Type, class Alloc>
struct NEX_HIDDEN_FROM_ABI UsesAllocatorImpl<Type, Alloc, false> : public meta::FalseType {};

/**
 * @struct UsesAllocator
 * @brief  Checks if a type uses a specific allocator for construction.
 * @tparam Type  The type to check.
 * @tparam Alloc The allocator type to check against.
 */
template <class Type, class Alloc>
struct UsesAllocator : public UsesAllocatorImpl<Type, Alloc> {};

/**
 * @brief  Convenience variable template for UsesAllocator.
 * @tparam Type  The type to check.
 * @tparam Alloc The allocator type to check against.
 */
template <class Type, class Alloc>
inline constexpr bool UsesAllocatorV = UsesAllocator<Type, Alloc>::value;

/**
 * @struct AllocatorTypeOf
 * @brief  Retrieves the allocator_type of a type if it exists, otherwise void.
 * @tparam Type The type to query for an allocator_type member.
 */
template <class Type, bool = HasAllocatorType<Type>::value>
struct AllocatorTypeOf {
    using type NEX_NODEBUG = void;
};

/**
 * @struct AllocatorTypeOf<Type, true>
 * @brief  Specialization of AllocatorTypeOf for types that have an allocator_type member.
 * @tparam Type The type to query for an allocator_type member.
 */
template <class Type>
struct AllocatorTypeOf<Type, true> {
    using type NEX_NODEBUG = typename Type::allocator_type;
};

/**
 * @brief  Convenience alias template for AllocatorTypeOf.
 * @tparam Type The type to query for an allocator_type member.
 * @tparam HasAllocator Indicates if the type has an allocator_type member (default: deduced from Type).
 */
template <class Type, bool HasAllocator = HasAllocatorType<Type>::value>
using AllocatorTypeOfT NEX_NODEBUG = typename AllocatorTypeOf<Type, HasAllocator>::type;

/**
 * @struct UsesAllocatorConstruction
 * @brief  Checks if a type can be constructed with a specific allocator and arguments.
 * @tparam Type  The type to check.
 * @tparam Alloc The allocator type to check against.
 * @tparam Args  The argument types for construction.
 */
template <class Type, class Alloc, class... Args>
struct UsesAllocatorConstruction
    : public meta::IntegralConstant<
        int32,
        meta::If<
            UsesAllocatorV<
                Type,
                meta::RemoveCvrefT<Alloc>
            >,
            0,          // Does not use allocator for construction
            meta::If<
                meta::IsConstructibleV<Type, allocator_arg_tag, Alloc, Args&&...>,
                1,      // Uses allocator for construction
                2       // Uses allocator as last argument for construction
            >
        >
    > {
    using RawAlloc NEX_NODEBUG = meta::RemoveCvrefT<Alloc>;
};

/**
 * @brief  Convenience variable template for UsesAllocatorConstruction.
 * @tparam Type  The type to check.
 * @tparam Alloc The allocator type to check against.
 * @tparam Args  The argument types for construction.
 */
template <class Type, class Alloc, class... Args>
inline constexpr int32 UsesAllocatorConstructionV = UsesAllocatorConstruction<Type, Alloc, Args...>::value;

/**
 * @struct UserAllocConstructImpl
 * @brief  Implementation of user-allocator-based construction strategies.
 * @tparam Type   The type to construct.
 * @tparam Alloc  The allocator type to use.
 * @tparam Args   The argument types for construction.
 * @param tag     Integral constant indicating the construction strategy.
 * @param storage Pointer to the storage where the object will be constructed.
 * @param alloc   The allocator to use.
 * @param args    The arguments for construction.
 */
template <class Type, class Alloc, class... Args>
struct UserAllocConstructImpl {
    // Constructs the object using the default constructor without the allocator.
    NEX_HIDDEN_FROM_ABI inline
    void operator()(meta::IntegralConstant<int32, 0>, Type* storage, const Alloc&, Args&&... args) {
        new (storage) Type(NEX_FORWARD<Args>(args)...);
    }

    // Constructs the object using the allocator-arg constructor strategy.
    NEX_HIDDEN_FROM_ABI inline
    void operator()(meta::IntegralConstant<int32, 1>, Type* storage, const Alloc& alloc, Args&&... args) {
        new (storage) Type(allocator_arg, alloc, NEX_FORWARD<Args>(args)...);
    }

    // Constructs the object using the constructor that takes the allocator as the last argument.
    NEX_HIDDEN_FROM_ABI inline
    void operator()(meta::IntegralConstant<int32, 2>, Type* storage, const Alloc& alloc, Args&&... args) {
        new (storage) Type(NEX_FORWARD<Args>(args)..., alloc);
    }
};

// =================================================================================
// Allocator traits implementation
// =================================================================================

// Trait to detect the pointer type member of a type.
template <class Type>
using PointerMemberOf NEX_NODEBUG = typename Type::pointer;

// Trait to detect the pointer type of an allocator, falling back to Type* if not present.
template <class Type, class Alloc>
using DetectedPointerT NEX_NODEBUG = meta::DetectedOrT<Type*, PointerMemberOf, meta::RemoveReferenceT<Alloc>>;

// Trait to rebind a pointer type to a different type, using an alias if available.
template <class Alloc, template <class> class Alias, class Ptr, class Type, class = void>
struct RebindOrAliasPointer {
    using type NEX_NODEBUG = typename PointerTraits<Ptr>::template rebind<Type>;
};

// Specialization of RebindOrAliasPointer when an alias is available.
template <class Ptr, class Alloc, class Type, template <class> class Alias>
struct RebindOrAliasPointer<Alloc, Alias, Ptr, Type, meta::VoidT<Alias<Alloc>>> {
    using type NEX_NODEBUG = Alias<Alloc>;
};

// Trait to detect the const pointer type member of an allocator.
template <class Alloc>
using ConstPointerMemberOf NEX_NODEBUG = typename Alloc::const_pointer;

// Trait to detect the const pointer type of an allocator, falling back to const Type* if not present.
template <class Type, class Ptr, class Alloc>
using ConstPointerT NEX_NODEBUG =
    typename RebindOrAliasPointer<Alloc, ConstPointerMemberOf, Ptr, const Type>::type;

// Trait to detect the void pointer type member of an allocator.
template <class Alloc>
using VoidPointerMemberOf NEX_NODEBUG = typename Alloc::void_pointer;

// Trait to detect the void pointer type of an allocator, falling back to void* if not present.
template <class Ptr, class Alloc>
using VoidPointerT NEX_NODEBUG =
    typename RebindOrAliasPointer<Alloc, VoidPointerMemberOf, Ptr, void>::type;

// Trait to detect the const void pointer type member of an allocator.
template <class Alloc>
using ConstVoidPointerMemberOf NEX_NODEBUG = typename Alloc::const_void_pointer;

// Trait to detect the const void pointer type of an allocator, falling back to const void* if not present.
template <class Ptr, class Alloc>
using ConstVoidPointerT NEX_NODEBUG =
    typename RebindOrAliasPointer<Alloc, ConstVoidPointerMemberOf, Ptr, const void>::type;

// Trait to detect the size type member of an allocator.
template <class Type>
using SizeTypeMemberOf NEX_NODEBUG = typename Type::size_type;

// Trait to detect the size type of an allocator, falling back to the unsigned version of the difference type if not present.
template <class Alloc, class DiffType>
using DetectedSizeTypeT NEX_NODEBUG = meta::DetectedOrT<meta::MakeUnsignedT<DiffType>, SizeTypeMemberOf, Alloc>;

// Trait to detect the difference type of an allocator, falling back to the pointer's difference type if not present.
template <class Alloc, class Ptr, class = void>
struct AllocatorTraitsDifferenceType {
    using type NEX_NODEBUG = typename PointerTraits<Ptr>::difference_type;
};

// Trait to detect the difference type of an allocator, falling back to the pointer's difference type if not present.
template <class Alloc, class Ptr>
struct AllocatorTraitsDifferenceType<Alloc, Ptr, meta::VoidT<typename Alloc::difference_type>> {
    using type NEX_NODEBUG = typename Alloc::difference_type;
};

NEX_NAMESPACE_END
