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
#include "nex/base/limits.h"
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

// Trait to detect the const void pointer type of an allocator,
// falling back to const void* if not present.
template <class Ptr, class Alloc>
using ConstVoidPointerT NEX_NODEBUG =
    typename RebindOrAliasPointer<Alloc, ConstVoidPointerMemberOf, Ptr, const void>::type;

// Trait to detect the size type member of an allocator.
template <class Type>
using SizeTypeMemberOf NEX_NODEBUG = typename Type::size_type;

// Trait to detect the size type of an allocator,
// falling back to the unsigned version of the difference type if not present.
template <class Alloc, class DiffType>
using DetectedSizeTypeT NEX_NODEBUG = meta::DetectedOrT<meta::MakeUnsignedT<DiffType>, SizeTypeMemberOf, Alloc>;

// Trait to detect the difference type of an allocator,
// falling back to the pointer's difference type if not present.
template <class Alloc, class Ptr, class = void>
struct AllocatorTraitsDifferenceType {
    using type NEX_NODEBUG = typename PointerTraits<Ptr>::difference_type;
};

// Trait to detect the difference type of an allocator,
// falling back to the pointer's difference type if not present.
template <class Alloc, class Ptr>
struct AllocatorTraitsDifferenceType<Alloc, Ptr, meta::VoidT<typename Alloc::difference_type>> {
    using type NEX_NODEBUG = typename Alloc::difference_type;
};

// Trait to detect the propagate_on_container_copy_assignment member of an allocator.
template <class Type>
using PropagateOnContainerCopyAssignmentMemberOf NEX_NODEBUG = typename Type::propagate_on_container_copy_assignment;

// Trait to detect the propagate_on_container_move_assignment member of an allocator,
// falling back to false if not present.
template <class Alloc>
using PropagateOnContainerCopyAssignment NEX_NODEBUG =
    meta::DetectedOrT<meta::FalseType, PropagateOnContainerCopyAssignmentMemberOf, Alloc>;

// Trait to detect the propagate_on_container_swap member of an allocator.
template <class Type>
using PropagateOnContainerMoveAssignmentMemberOf NEX_NODEBUG = typename Type::propagate_on_container_move_assignment;

// Trait to detect the propagate_on_container_move_assignment member of an allocator,
// falling back to false if not present.
template <class Alloc>
using PropagateOnContainerMoveAssignment NEX_NODEBUG =
    meta::DetectedOrT<meta::FalseType, PropagateOnContainerMoveAssignmentMemberOf, Alloc>;

// Trait to detect the propagate_on_container_swap member of an allocator.
template <class Type>
using PropagateOnContainerSwapMemberOf NEX_NODEBUG = typename Type::propagate_on_container_swap;

// Trait to detect the propagate_on_container_swap member of an allocator,
// falling back to false if not present.
template <class Alloc>
using PropagateOnContainerSwap NEX_NODEBUG =
    meta::DetectedOrT<meta::FalseType, PropagateOnContainerSwapMemberOf, Alloc>;

// Trait to detect the is_always_equal member of an allocator.
template <class Type>
using IsAlwaysEqualMemberOf NEX_NODEBUG = typename Type::is_always_equal;

// Trait to detect the is_always_equal member of an allocator,
// falling back to false if not present.
template <class Alloc>
using IsAlwaysEqual NEX_NODEBUG = meta::DetectedOrT<meta::FalseType, IsAlwaysEqualMemberOf, Alloc>;

// Trait to detect if an allocator has a rebind member for a given other type, false by default.
template <class Type, class OtherType, class = void>
inline const bool HasRebindOtherV = false;

// Specialization to detect if an allocator has a rebind member for a given other type, true if present.
template <class Type, class OtherType>
inline const bool HasRebindOtherV<Type, OtherType,
    meta::VoidT<typename Type::template rebind<OtherType>::other>
    > = true;

/**
 * @struct AllocatorTraitsRebind
 * @brief Provides a mechanism to rebind an allocator to a different type.
 * 
 * @tparam Type The original allocator type.
 * @tparam OtherType The type to which the allocator should be rebound.
 * @tparam bool Indicates whether the allocator has a rebind member for the given other type.
 */
template <
    class Type,
    class OtherType,
    bool = HasRebindOtherV<Type, OtherType>
>
struct AllocatorTraitsRebind {
    static_assert(HasRebindOtherV<Type, OtherType>, "Error: This allocator has to implement rebind");
    using type NEX_NODEBUG = typename Type::template rebind<OtherType>::other;
};

/**
 * @struct AllocatorTraitsRebind
 * @brief Specialization for allocators that are class templates with variadic arguments.
 * 
 * @tparam Alloc The allocator template.
 * @tparam Type The original type of the allocator.
 * @tparam Args Additional template arguments for the allocator.
 * @tparam OtherType The type to which the allocator should be rebound.
 */
template <
    template <class, class...> class Alloc,
    class Type,
    class... Args,
    class OtherType
>
struct AllocatorTraitsRebind<Alloc<Type, Args...>, OtherType, true> {
    using type NEX_NODEBUG = typename Alloc<Type, Args...>::template rebind<OtherType>::other;
};

/**
 * @struct AllocatorTraitsRebind
 * @brief Specialization for allocators that are class templates with variadic arguments
 *        and do not have a rebind member.
 * 
 * @tparam Alloc The allocator template.
 * @tparam Type The original type of the allocator.
 * @tparam Args Additional template arguments for the allocator.
 * @tparam OtherType The type to which the allocator should be rebound.
 */
template <
    template <class, class...> class Alloc,
    class Type,
    class... Args,
    class OtherType
>
struct AllocatorTraitsRebind<Alloc<Type, Args...>, OtherType, false> {
    using type NEX_NODEBUG = Alloc<OtherType, Args...>;
};

// Helper alias to extract the rebind type from an allocator for a given type.
template <class Alloc, class Type>
using AllocatorTraitsRebindType NEX_NODEBUG = typename AllocatorTraitsRebind<Alloc, Type>::type;

/**
 * @struct AllocatorTraits
 * @brief Provides traits and utility functions for allocators.
 * 
 * @details
 * This struct provides a standardized interface to query and utilize various properties and functions
 * of allocators, such as allocation, deallocation, construction, and destruction of objects.
 * It also provides mechanisms to rebind allocators to different types and to query propagation traits
 * for container operations.
 * 
 * @tparam Alloc The allocator type for which traits are being defined.
 */
template <class Alloc>
struct AllocatorTraits {
    using allocator_type     = Alloc;
    using value_type         = typename allocator_type::value_type;
    using pointer            = DetectedPointerT<value_type, allocator_type>;
    using const_pointer      = ConstPointerT<value_type, pointer, allocator_type>;
    using void_pointer       = VoidPointerT<pointer, allocator_type>;
    using const_void_pointer = ConstVoidPointerT<pointer, allocator_type>;
    using difference_type    = typename AllocatorTraitsDifferenceType<allocator_type, pointer>::type;
    using size_type          = DetectedSizeTypeT<allocator_type, difference_type>;

    using propagate_on_container_copy_assignment = PropagateOnContainerCopyAssignment<allocator_type>;
    using propagate_on_container_move_assignment = PropagateOnContainerMoveAssignment<allocator_type>;
    using propagate_on_container_swap            = PropagateOnContainerSwap<allocator_type>;
    using is_always_equal                        = IsAlwaysEqual<allocator_type>;

    template <class Type>
    using rebind_alloc = AllocatorTraitsRebindType<allocator_type, Type>;
    template <class Type>
    using rebind_traits = AllocatorTraits<rebind_alloc<Type> >;

    // Allocates memory using the allocator's allocate function
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    static pointer allocate(allocator_type& alloc, size_type n) {
        return alloc.allocate(n);
    }

    // Deallocates memory using the allocator's deallocate function
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    static void deallocate(allocator_type& alloc, pointer ptr, size_type n) noexcept {
        return alloc.deallocate(ptr, n);
    }

    // Constructs an object of the given type at the given pointer using the provided arguments
    template <class Type, class... Args>
    NEX_HIDDEN_FROM_ABI constexpr
    static void construct(allocator_type&, pointer ptr, Args&&... args) {
        NEX_CONSTRUCT_AT(ptr, NEX_FORWARD<Args>(args)...);
    }

    // Destroys the object of the given type at the given pointer
    template <class Type>
    NEX_HIDDEN_FROM_ABI constexpr static void destroy(allocator_type&, pointer ptr) {
        NEX_DESTROY_AT(ptr);
    }

private:
    template <class Alloc>
    struct HasMaxAllocationSize {
        static constexpr bool value = requires(Alloc& alloc) {
            { alloc.maxAllocationSize() } -> meta::ConvertibleTo<usize>;
        };
    };

public:
    // Returns the maximum allocation size supported by the allocator
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    static size_type maxAllocationSize(const allocator_type& alloc) noexcept {
        if constexpr (HasMaxAllocationSize<const allocator_type>::value) {
            return alloc.maxAllocationSize();
        } else {
            return NumericLimits<size_type>::max() / sizeof(value_type);
        }
    }
};

NEX_NAMESPACE_END
