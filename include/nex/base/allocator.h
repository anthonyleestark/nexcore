/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/compiler_features.h"
#include "nex/base/attributes.h"
#include "nex/base/visibility.h"
#include "nex/base/export.h"
#include "nex/base/preprocessor.h"
#include "nex/base/class_macros.h"
#include "nex/base/allocator_traits.h"
#include "nex/base/type_concepts.h"
#include "nex/base/type_traits.h"
#include "nex/base/fwd.h"
#include "nex/base/location.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @struct AllocatorTypeValidator
 * @brief Validates that a type is suitable for use with an allocator.
 * @tparam Type The type to validate.
 * @note User-defined allocators should conform to the requirements specified by this validator.
 */
template <class Type>
struct AllocatorTypeValidator : TrueType {
    static_assert(!IsVoidV<Type>,     "Error: Allocator cannot be instantiated with void type.");
    static_assert(!IsConstV<Type>,    "Error: Allocator cannot be instantiated with const type.");
    static_assert(!IsVolatileV<Type>, "Error: Allocator cannot be instantiated with volatile type.");
    static_assert(sizeof(Type) > 0,   "Error: Allocator cannot be instantiated with incomplete type.");
};

/**
 * @brief Concept that checks if a type satisfies the allocator requirements.
 * @tparam Type The type to check.
 * @note
 * A type satisfies this concept if it provides allocate and deallocate functions
 * with the expected signatures and defines the necessary associated types.
 */
template <class Type>
concept Allocator = requires (
    Type& alloc,
    typename AllocatorTraits<Type>::size_type count,
    typename AllocatorTraits<Type>::pointer ptr
) {
    typename Type::value_type;
    { alloc.allocate(count) } -> SameAs<typename AllocatorTraits<Type>::pointer>;
    { alloc.deallocate(ptr, count) } -> SameAs<void>;
};

/**
 * @class DefaultAllocator
 * @brief Default allocator for NexCore types and containers.
 * 
 * @details
 * The DefaultAllocator uses the global new and delete operators to manage memory.
 * It provides allocate and deallocate functions that conform to the Allocator concept.
 * 
 * @tparam Type The type of elements to allocate.
 */
template <class Type>
class NEX_API DefaultAllocator {
    static_assert(AllocatorTypeValidator<Type>::value);

public:
    using value_type = Type;
    using size_type = usize;
    using difference_type = isize;

    using self = DefaultAllocator<Type>;
    using trait_type = AllocatorTraits<self>;

    using pointer = trait_type::pointer;
    using const_pointer = trait_type::const_pointer;

    using propagate_on_container_move_assignment = TrueType;
    using is_always_equal = TrueType;

    // Default constructs the allocator
    NEX_HIDDEN_FROM_ABI constexpr DefaultAllocator() noexcept = default;

    // Constructs a DefaultAllocator for a different type,
    // allowing conversion between allocators of different types.
    template <class OtherType>
    NEX_HIDDEN_FROM_ABI constexpr DefaultAllocator(const DefaultAllocator<OtherType>&) noexcept {}

    // Allocates memory for the given number of elements
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr pointer allocate(size_type elementCount) {
        if (elementCount > trait_type::maxAllocationSize(*this)) {
            NEX_ASSERT_MSG_OR_THROW(false, "Error: Allocation size exceeds maximum allowed.");
        }
        if (meta::IsConstantEvaluated()) {
            return static_cast<pointer>(::operator new(elementCount * sizeof(value_type)));
        } else {
            return static_cast<pointer>(
                // The global allocation function requires the element count
                // to be wrapped in an element_count type.
                NEX_PREPEND_NAMESPACE(allocate)<value_type>(element_count(elementCount))
            );
        }
    }

    // Deallocates memory previously allocated by this allocator
    NEX_HIDDEN_FROM_ABI constexpr void deallocate(pointer ptr, size_type elementCount) noexcept {
        if (meta::IsConstantEvaluated()) {
            ::operator delete(static_cast<void*>(ptr));
        } else {
            // The global deallocation function requires the element count
            // to be wrapped in an element_count type.
            NEX_PREPEND_NAMESPACE(deallocate)<value_type>(ptr, element_count(elementCount));
        }
    }
};

/**
 * @brief Equality comparison for DefaultAllocator instances of potentially different types
 * @param Type1 The type of elements for the first allocator.
 * @param Type2 The type of elements for the second allocator.
 * @note Returns true for all instances, as DefaultAllocator instances are considered equal regardless of type.
 */
template <class Type1, class Type2>
inline NEX_HIDDEN_FROM_ABI constexpr
bool operator==(const DefaultAllocator<Type1>&, const DefaultAllocator<Type2>&) noexcept {
    return true;
}

/**
 * @brief Allocates a raw block of memory with the specified byte count and alignment.
 * @param byteCount The number of bytes to allocate.
 * @param alignment The alignment requirement for the allocation.
 * @return A void pointer (void*) to the allocated memory.
 * @note The returned pointer must be casted to the appropriate type before use.
 */
NEX_EXPORTED_FROM_ABI void_ptr allocateRaw(
    usize byteCount,
    usize alignment
);

/**
 * @brief Deallocates a raw block of memory previously allocated.
 * @param ptr The pointer to the memory to deallocate.
 * @param byteCount The number of bytes to deallocate.
 * @param alignment The alignment requirement for the allocation.
 * 
 * @note
 * For unsized deallocation, the byte count is unknown, so 0 is passed by convention.
 * If user passes a zero byte count, it will be treated as an unsized deallocation,
 * not a sized deallocation with the size zero.
 */
NEX_EXPORTED_FROM_ABI void deallocateRaw(
    void_ptr ptr,
    usize byteCount,
    usize alignment
) noexcept;

#if NEX_HAS_ALIGNED_ALLOCATION
    // Helper function to determine if an alignment requirement exceeds the default new alignment.
    NEX_HIDDEN_FROM_ABI inline constexpr bool isOverAlignedForNew(usize alignment) noexcept {
    #if defined(NEX_DEFAULT_NEW_ALIGNMENT)
        return alignment > NEX_DEFAULT_NEW_ALIGNMENT;
    #else
        return alignment > NEX_ALIGNOF(max_align);
    #endif
    }
#endif  // ^^NEX_HAS_ALIGNED_ALLOCATION

/**
 * @brief Allocates memory for a specific type with the given element count and alignment.
 * @tparam Type The type of elements to allocate.
 * @param elementCount The number of elements to allocate.
 * @param alignment The alignment requirement for the allocation.
 * @return A pointer to the allocated memory of the specified type.
 */
template <class Type>
NEX_NODISCARD inline NEX_HIDDEN_FROM_ABI NEX_NO_CFI
Type* allocate(element_count elementCount, NEX_MAYBE_UNUSED usize alignment /* = NEX_ALIGNOF(Type) */) {
    usize byteCount = static_cast<usize>(elementCount) * sizeof(Type);
#if !NEX_ALLOCATION_DIAGNOSTIC_ENABLED
    #if NEX_HAS_ALIGNED_ALLOCATION
        if (isOverAlignedForNew(alignment)) {
            return static_cast<Type*>(
                // Overload resolution for the standard global operator new requires align_val_t here.
                // There's no other way for this even if we define an identical custom align_val type ourselves.
                ::operator new(byteCount, static_cast<NEX_STD align_val_t>(alignment))
            );
        }
    #endif
    return static_cast<Type*>(::operator new(byteCount));
#else
    // Use raw allocation for diagnostic purposes
    return static_cast<Type*>(allocateRaw(byteCount, alignment));
#endif
}

/**
 * @brief Deallocates memory for a specific type with the given element count and alignment.
 * @tparam Type The type of elements to deallocate.
 * @param ptr The pointer to the memory to deallocate.
 * @param elementCount The number of elements to deallocate.
 * @param alignment The alignment requirement for the allocation.
 */
template <class Type>
inline NEX_HIDDEN_FROM_ABI void deallocate(
    meta::TypeIdentityT<Type>* ptr,
    element_count elementCount,
    NEX_MAYBE_UNUSED usize alignment /* = NEX_ALIGNOF(Type) */
) noexcept {
    NEX_MAYBE_UNUSED usize byteCount = static_cast<usize>(elementCount) * sizeof(Type);
#if !NEX_ALLOCATION_DIAGNOSTIC_ENABLED
    #if NEX_HAS_ALIGNED_ALLOCATION
        if (isOverAlignedForNew(alignment)) {
            return ::operator delete(
                ptr
                NEX_ONLY_IF_SIZED_DEALLOCATION(byteCount)
                // Overload resolution for the standard global operator delete requires align_val_t here.
                // There's no other way for this even if we define an identical custom align_val type ourselves.
                , static_cast<NEX_STD align_val_t>(alignment)
            );
        }
    #endif
    return ::operator delete(ptr NEX_ONLY_IF_SIZED_DEALLOCATION(byteCount));
#else
    // Use raw deallocation for diagnostic purposes
    deallocateRaw(static_cast<void_ptr>(ptr), byteCount, alignment);
#endif
}

/**
 * @brief Deallocates memory for a specific type without specifying the element count.
 * @tparam Type The type of elements to deallocate.
 * @param ptr The pointer to the memory to deallocate.
 * @param alignment The alignment requirement for the allocation.
 */
template <class Type>
inline NEX_HIDDEN_FROM_ABI void deallocateUnsized(
    meta::TypeIdentityT<Type>* ptr,
    NEX_MAYBE_UNUSED usize alignment /* = NEX_ALIGNOF(Type) */
) noexcept {
#if !NEX_ALLOCATION_DIAGNOSTIC_ENABLED
    #if NEX_HAS_ALIGNED_ALLOCATION
        if (isOverAlignedForNew(alignment)) {
            // Overload resolution for the standard global operator delete requires align_val_t here.
            // There's no other way for this even if we define an identical custom align_val type ourselves.
            return ::operator delete(ptr, static_cast<NEX_STD align_val_t>(alignment));
        }
    #endif
    return ::operator delete(ptr);
#else
    /**
     * @note Use raw deallocation for diagnostic purposes
     * For unsized deallocation, the byte count is unknown, so we pass 0 by convention for now,
     * but it may be updated in the future if a better convention is established.
     */
    deallocateRaw(static_cast<void_ptr>(ptr), 0, alignment);
#endif
}

/**
 * @class AllocatorDestructor
 * @brief Functor that deallocates a buffer using an allocator and element count.
 * @tparam Alloc Allocator type providing the pointer and size types used for deallocation.
 *
 * @details
 * Stores a reference to an allocator and an element count so it can be invoked as a callable
 * (such as a custom deleter) to release memory via `AllocatorTraits<Alloc>::deallocate`.
 */
template <class Alloc>
class NEX_INTERNAL AllocatorDestructor {
    using trait_type NEX_NODEBUG = AllocatorTraits<Alloc>;

public:
    using pointer NEX_NODEBUG = typename trait_type::pointer;
    using size_type NEX_NODEBUG = typename trait_type::size_type;

private:
    Alloc& alloc_;      // Reference to the allocator instance.
    size_type count_;   // The number of elements to deallocate.

public:
    // Constructor that initializes the allocator reference and element count.
    NEX_HIDDEN_FROM_ABI AllocatorDestructor(Alloc& alloc, size_type elementCount) noexcept
        : alloc_(alloc), count_(elementCount) {}

    // Functor call operator that deallocates the memory using the allocator.
    NEX_HIDDEN_FROM_ABI void operator()(pointer ptr) noexcept { trait_type::deallocate(alloc_, ptr, count_); }
};

/**
 * @class AllocationGuard
 * @brief A guard class that manages the lifetime of an allocated memory block.
 * @tparam Alloc The type of the allocator used for memory allocation.
 *
 * @details
 * The AllocationGuard ensures that the allocated memory is properly deallocated
 * when the guard goes out of scope, preventing memory leaks.
 *
 * @par Example usage:
 * @code
 * void allocateMemory() {
 *    // 1. First, we initialize the guard with the allocator and element count.
 *    AllocationGuard<MyAllocator> guard(myAllocator, 10);
 *    {
 *        // 2. Then we do something with the allocated memory.
 *        initializeMemoryThatMayThrowExceptions(guard.get());
 *        // 3. If an exception is thrown, the guard will automatically deallocate the memory.
 *        //    The C++ exception handling mechanism itself will continue to propagate the exception
 *        //    after the guard has deallocated the memory.
 *    }
 *    // 4. If it reaches this point without exceptions, the pointer can be safely released
 *    //    in an operation that can't throw exceptions.
 *    saveMemoryStateWhenNoExceptions(guard.release());
 *    // 5. Finally, after this point, the guard is no longer responsible for the memory anymore.
 * }
 * @endcode
 */
template <class Alloc>
class NEX_INTERNAL AllocationGuard {
    using trait_type NEX_NODEBUG = AllocatorTraits<Alloc>;
    using pointer NEX_NODEBUG    = typename trait_type::pointer;
    using size NEX_NODEBUG       = typename trait_type::size_type;

private:
    Alloc alloc_;   // The allocator instance used for memory allocation.
    size count_;    // The number of elements allocated.
    pointer ptr_;   // The pointer to the allocated memory.

public:
    /**
     * @brief Constructs an AllocationGuard with the given allocator and element count.
     * @tparam OtherAlloc The type of the allocator (potentially different).
     * @param otherAlloc The allocator instance to use for allocation.
     * @param count The number of elements to allocate.
     * @note The conversion of the allocator type is performed inside the constructor.
     */
    template <class OtherAlloc>
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR26 explicit AllocationGuard(OtherAlloc&& otherAlloc, size count)
        : alloc_(NEX_MOVE(otherAlloc)), count_(count),
          ptr_(trait_type::allocate(alloc_, count_)) {}

    // Destructor that destroys the allocated memory if it hasn't been released.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR26 ~AllocationGuard() noexcept { destroy(); }

    // No copy semantics allowed for AllocationGuard.
    NEX_DISALLOW_COPY(AllocationGuard);

    // Move constructor that transfers ownership of the allocated memory from another guard.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR26 AllocationGuard(AllocationGuard&& other) noexcept
        : alloc_(NEX_MOVE(other.alloc_)), count_(other.count_), ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    // Move assignment operator that transfers ownership of the allocated memory from another guard.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR26 AllocationGuard& operator=(AllocationGuard&& other) noexcept {
        if (NEX_ADDRESS_OF(other) != this) {
            destroy();
            alloc_ = NEX_MOVE(other.alloc_);
            count_ = other.count_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // Releases ownership of the allocated memory and returns the pointer.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR26 pointer release() noexcept {
        pointer temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    // Returns the pointer to the allocated memory without releasing ownership.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR26 pointer get() const noexcept { return ptr_; }
    
private:
    // Destroys the allocated memory if it hasn't been released.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR26 void destroy() noexcept {
        if (ptr_ != nullptr) {
            trait_type::deallocate(alloc_, ptr_, count_);
            ptr_ = nullptr;
        }
    }
};

NEX_NAMESPACE_END
