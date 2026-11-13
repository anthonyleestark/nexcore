/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/limits.h"
#include "nex/base/memory.h"
#include "nex/base/utility.h"
#include "nex/base/type_traits.h"
#include "nex/base/compressed_pair.h"
#include "nex/base/allocator_traits.h"
#include "nex/base/pointer_traits.h"
#include "nex/base/hash.h"
#include "nex/base/assert_crash.h"

NEX_NAMESPACE_BEGIN

/**
 * @struct DefaultDelete<Type>
 * @brief Provides a default mechanism for deleting objects managed by UniquePtr.
 * @tparam Type The type of object to delete.
 * @details
 * The DefaultDelete struct provides a default mechanism for deleting objects managed by UniquePtr.
 * It deletes the object pointed to by the given pointer using the delete operator.
 */
template <class Type>
    requires (!IsFunctionV<Type>)
struct DefaultDelete {
    NEX_HIDDEN_FROM_ABI constexpr DefaultDelete() noexcept = default;

    template <class OtherType>
        requires (IsConvertibleV<OtherType*, Type*>)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 DefaultDelete(const DefaultDelete<OtherType>&) noexcept {}

    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 void operator()(Type* ptr) const noexcept {
        static_assert(sizeof(Type) >= 0, "Error: Cannot delete an incomplete type");
        static_assert(!IsVoidV<Type>, "Error: Cannot delete an incomplete type");
        delete ptr;
    }
};

/**
 * @struct DefaultDelete<Type[]>
 * @brief Provides a default mechanism for deleting arrays managed by UniquePtr.
 * @tparam Type The type of array elements to delete.
 * @details
 * The DefaultDelete struct provides a default mechanism for deleting arrays managed by UniquePtr.
 * It deletes the array pointed to by the given pointer using the delete[] operator.
 */
template <class Type>
struct DefaultDelete<Type[]> {
    NEX_HIDDEN_FROM_ABI constexpr DefaultDelete() noexcept = default;

    template <class OtherType>
        requires (IsConvertibleV<OtherType (*)[], Type (*)[]>)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 DefaultDelete(const DefaultDelete<OtherType[]>&) noexcept {}

    template <class OtherType>
        requires (IsConvertibleV<OtherType (*)[], Type (*)[]>)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 void operator()(OtherType* ptr) const noexcept {
        static_assert(sizeof(OtherType) >= 0, "Error: Cannot delete an incomplete type");
        delete[] ptr;
    }
};

// Trait to check if a type is a DefaultDelete.
template <class Type>
inline constexpr bool IsDefaultDeleter = false;

// Specialization for DefaultDelete<Type[]>
template <class Type>
inline constexpr bool IsDefaultDeleter<DefaultDelete<Type>> = true;

/**
 * @struct UniquePtrDeleterValidator
 * @brief Validates the deleter type for UniquePtr.
 * @tparam Deleter The type of deleter to validate.
 * @details
 * The UniquePtrDeleterValidator struct provides compile-time checks and type aliases
 * to ensure that the deleter type used with UniquePtr is valid and correctly handled.
 */
template <class Deleter>
    requires (!IsRvalueReferenceV<Deleter>)
struct NEX_INTERNAL UniquePtrDeleterValidator {
    // Is the deleter an lvalue reference?
    static constexpr bool IsLvalReference = IsLvalueReferenceV<Deleter>;

    // The lvalue-reference type we accept in the constructor
    using LValRefType = ConditionalT<
        IsLvalReference,
        Deleter&,                           // already an lvalue reference -> keep it
        const Deleter&                      // otherwise, accept a const lvalue reference
    >;

    // The “good” rvalue-reference type (only meaningful for non-ref deleters)
    using GoodRValRefType = ConditionalT<
        IsLvalReference,
        void,                               // lvalue references don’t have a good rvalue type
        Deleter&&                           // otherwise, the good rvalue type is Deleter&&
    >;

    // The “bad” rvalue-reference type (used to delete the overload for ref deleters)
    using BadRValRefType = ConditionalT<
        IsLvalReference,
        RemoveReferenceT<Deleter>&&,        // rvalue reference for lvalue reference deleters
        void                                // otherwise, no bad rvalue type
    >;

    // Whether we should enable the rvalue-reference overload
    using EnableRvalOverload = BoolConstant<!IsLvalReference>;
};

/**
 * @class UniquePtr
 * @brief A smart pointer that owns and manages a dynamically allocated object.
 *
 * @details
 * UniquePtr provides exclusive ownership of a dynamically allocated object.
 * It ensures that the object is properly deleted when the UniquePtr goes out of scope.
 * The deleter type can be customized, and the pointer type is deduced from the deleter if available.
 *
 * @tparam Type The type of the object being managed.
 * @tparam Deleter The type of the deleter used to destroy the object.
 */
template <
    class Type,
    class Deleter = DefaultDelete<Type>
> requires (!IsRvalueReferenceV<Deleter> && !IsBoundedArrayV<Type>)
class NEX_API UniquePtr {
public:
    using element_type = Type;
    using deleter_type = Deleter;

    // We'd rather use the pointer type detected from the deleter, if available.
    using pointer = DetectedPointerT<Type, Deleter>;

private:
    // storage_ := {{ ptr, deleter }}
    CompressedPair<pointer, deleter_type> storage_;

    // Internal accessor to the stored pointer.
    NEX_HIDDEN_FROM_ABI inline pointer& ptr() noexcept { return storage_.first; }
    NEX_HIDDEN_FROM_ABI inline pointer ptr() const noexcept { return storage_.first; }

    // Internal accessor to the stored deleter.
    NEX_HIDDEN_FROM_ABI inline deleter_type& deleter() noexcept { return storage_.second; }
    NEX_HIDDEN_FROM_ABI inline const deleter_type& deleter() const noexcept { return storage_.second; }

    // Validator for the deleter type to ensure it meets the requirements for UniquePtr.
    // This checks if the deleter type is suitable for use with UniquePtr, including
    // whether it is default constructible, move constructible, and assignable as needed.
    using DeleterValidator = UniquePtrDeleterValidator<deleter_type>;

    template <bool Dummy = true>
    using LValRefType = typename meta::DependentType<DeleterValidator, Dummy>::LValRefType;

    template <bool Dummy = true>
    using GoodRValRefType = typename meta::DependentType<DeleterValidator, Dummy>::GoodRValRefType;

    template <bool Dummy = true>
    using BadRValRefType = typename meta::DependentType<DeleterValidator, Dummy>::BadRValRefType;

    // Validator for checking if the deleter type is default constructible and not a pointer.
    // This ensures that the deleter can be safely default constructed and is not a raw pointer type,
    // which could lead to undefined behavior.
    template <
        bool Dummy = true,
        class DeleterT = meta::DependentType<TypeIdentityT<deleter_type>, Dummy>::type
    >
    struct IsDeleterDefaultConstructible
        : meta::And<
            IsDefaultConstructible<DeleterT>,
            meta::Not<IsPointerV<DeleterT> >
        > {};

    // Validator for checking if the deleter type is constructible with a given argument type.
    // This ensures that the deleter can be properly constructed with the provided argument.
    template <class ArgType>
    struct IsDeleterConstructible : IsConstructible<deleter_type, ArgType> {};

    template <class OtherPtr, class OtherType>
    struct IsMoveConvertible
        : meta::And<
            IsConvertible<typename OtherPtr::pointer, pointer>,
            meta::Not<IsArrayV<OtherType> >
        > {};

    // Validator for checking if the deleter type is convertible from another deleter type.
    // This ensures that the deleter can be assigned from a different deleter type if needed.
    template <class OtherDeleter>
    struct IsDeleterConvertible
        : meta::Or<
            meta::And<
                IsReference<deleter_type>,
                IsSame<deleter_type, OtherDeleter>
            >,
            meta::And<
                meta::Not<IsReferenceV<deleter_type> >,
                IsConvertible<OtherDeleter, deleter_type>
            >
        > {};

    // Validator for checking if the deleter type is assignable from another deleter type.
    // This ensures that the deleter can be properly assigned when needed.
    template <class OtherDeleter>
    struct IsDeleterAssignable : IsAssignable<deleter_type&, OtherDeleter&&> {};

public:
    /**
     * @brief Constructs a UniquePtr with a default-constructed deleter.
     * @note Requires that the deleter type is default constructible and not a pointer.
     */
    template <bool Dummy = true>
        requires (IsDeleterDefaultConstructible<Dummy>::value)
    NEX_HIDDEN_FROM_ABI constexpr UniquePtr() noexcept
        : storage_(pointer(), deleter_type()) {}

    /**
     * @brief Constructs a UniquePtr with a null pointer and a default-constructed deleter.
     * @note Requires that the deleter type is default constructible and not a pointer.
     */
    template <bool Dummy = true>
        requires (IsDeleterDefaultConstructible<Dummy>::value)
    NEX_HIDDEN_FROM_ABI constexpr UniquePtr(null) noexcept
        : storage_(pointer(), deleter_type()) {}

    /**
     * @brief Constructs a UniquePtr with a given pointer and a default-constructed deleter.
     * @note Requires that the deleter type is default constructible and not a pointer.
     */
    template <bool Dummy = true>
        requires (IsDeleterDefaultConstructible<Dummy>::value)
    NEX_HIDDEN_FROM_ABI constexpr UniquePtr(pointer ptr) noexcept
        : storage_(ptr, deleter_type()) {}

    /**
     * @brief Constructs a UniquePtr with a given pointer and a specified lvalue reference deleter.
     * @note Requires that the deleter type is constructible from the provided lvalue reference.
     */
    template <bool Dummy = true>
        requires (IsDeleterConstructible<LValRefType<Dummy> >::value)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(pointer ptr, LValRefType<Dummy> deleter) noexcept
        : storage_(ptr, deleter) {}

    /**
     * @brief Constructs a UniquePtr with a given pointer and a specified rvalue reference deleter.
     * @note Requires that the deleter type is constructible from the provided rvalue reference
     *       and is not a reference type.
     */
    template <bool Dummy = true>
        requires (
            IsDeleterConstructible<GoodRValRefType<Dummy> >::value &&
            !IsReferenceV<deleter_type>)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(pointer ptr, GoodRValRefType<Dummy> deleter) noexcept
        : storage_(ptr, NEX_MOVE(deleter)) {}

    /**
     * @brief Deleted constructor for a UniquePtr with a given pointer and a specified bad rvalue reference deleter.
     * @note Ensures that the deleter type cannot be constructed from the provided bad rvalue reference.
     */
    template <bool Dummy = true>
        requires (IsDeleterConstructible<BadRValRefType<Dummy> >::value)
    NEX_HIDDEN_FROM_ABI UniquePtr(pointer ptr, BadRValRefType<Dummy> deleter) noexcept = delete;

    /**
     * @brief Move constructor for UniquePtr.
     * @note Requires that the deleter type is convertible from the other UniquePtr's deleter type.
     */
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(UniquePtr&& other) noexcept
        requires (IsDeleterConvertible<deleter_type>::value)
        : storage_(other.release(), NEX_FORWARD<deleter_type>(other.getDeleter())) {}

    /**
     * @brief Move constructor for UniquePtr from another UniquePtr with a different type and deleter.
     * @note Requires that the other UniquePtr is move convertible to this UniquePtr's pointer type
     *       and that the deleter is convertible.
     */
    template <class OtherType, class OtherDeleter>
        requires (
            IsMoveConvertible<UniquePtr<OtherType, OtherDeleter>, OtherType>::value &&
            IsDeleterConvertible<OtherDeleter>::value)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(UniquePtr<OtherType, OtherDeleter>&& other) noexcept
        : storage_(other.release(), NEX_FORWARD<OtherDeleter>(other.getDeleter())) {}

    /**
     * @brief Move assignment operator for UniquePtr.
     * @note Requires that the deleter type is assignable from an rvalue reference of the deleter type.
     */
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr& operator=(UniquePtr&& other) noexcept
        requires (IsDeleterAssignable<deleter_type>::value) {
        reset(other.release());
        deleter() = NEX_FORWARD<deleter_type>(other.getDeleter());
        return *this;
    }

    /**
     * @brief Move assignment operator for UniquePtr from another UniquePtr with a different type and deleter.
     * @note Requires that the other UniquePtr is move convertible to this UniquePtr's pointer type
     *       and that the deleter is assignable.
     */
    template <class OtherType, class OtherDeleter>
        requires (
            IsMoveConvertible<UniquePtr<OtherType, OtherDeleter>, OtherType>::value &&
            IsDeleterAssignable<OtherDeleter>::value)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr& operator=(UniquePtr<OtherType, OtherDeleter>&& other) noexcept {
        reset(other.release());
        deleter() = NEX_FORWARD<OtherDeleter>(other.getDeleter());
        return *this;
    }

    // Destructor for UniquePtr that resets the managed pointer and invokes the deleter if necessary.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 ~UniquePtr() { reset(); }

    // Assignment operator for assigning nullptr to the UniquePtr, which resets the managed pointer.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr& operator=(null) noexcept {
        reset();
        return *this;
    }

    // Dereference operator for UniquePtr. Returns a reference to the managed object.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 auto operator*() const
        noexcept(noexcept(*declval<pointer>())) -> AddLvalueReferenceT<element_type> {
        return *ptr();
    }

    // Arrow operator for UniquePtr. Returns the raw pointer managed by the UniquePtr.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 pointer operator->() const noexcept { return ptr(); }

    // Returns the raw pointer managed by the UniquePtr.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 pointer get() const noexcept { return ptr(); }

    // Returns a reference to the deleter.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 deleter_type& getDeleter() noexcept { return deleter(); }
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 const deleter_type& getDeleter() const noexcept { return deleter(); }

    // Returns true if the UniquePtr manages a non-null pointer, false otherwise.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 explicit operator bool() const noexcept { return ptr() != nullptr; }

    // Releases ownership of the managed pointer and returns it.
    // The UniquePtr will no longer manage the pointer after this call.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 pointer release() noexcept {
        pointer tmp = ptr();
        ptr() = pointer();
        // The deleter remains unchanged.
        return tmp;
    }

    // Resets the UniquePtr with a new pointer. Invokes the deleter on the old pointer if necessary.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 void reset(pointer ptr = pointer()) noexcept {
        pointer tmp = ptr();
        ptr() = ptr;
        if (tmp != nullptr) {
            deleter()(tmp);
        }
    }

    /** 
     * @brief Swaps the contents of this UniquePtr with another UniquePtr.
     * @note Requires that the deleter type is swappable.
     */
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 void swap(UniquePtr& other)
        noexcept(IsNothrowSwappableV<deleter_type>)
        requires (IsSwappableV<deleter_type>) {
        NEX_SWAP(ptr(), other.ptr());
        NEX_SWAP(deleter(), other.deleter());
    }
};

/**
 * @brief Maintains the array bound information for a UniquePtr array specialization.
 *
 * Unlike the scalar UniquePtr specialization, UniquePtr<T[]> may need to retain
 * additional information about the number of elements in the managed array.
 * This information can be used to perform runtime bounds checking when accessing
 * an element through the array specialization.
 *
 * The checker stores the number of elements in the managed array, rather than
 * attempting to determine the array bound from the managed pointer itself.
 * Consequently, the bound is known only when it is explicitly provided at
 * construction time, such as when an array is created through a size-aware
 * factory function or constructor.
 *
 * If a UniquePtr<T[]> is constructed from a raw pointer without an explicitly
 * supplied element count, the actual array extent cannot be determined.
 * In that case, the checker retains an unknown-bound sentinel value (NEX_SIZE_MAX).
 * This value indicates that no reliable array bound is available; it must not
 * be interpreted as the actual number of elements allocated for the array.
 *
 * Array Cookie support is intentionally not implemented at this stage.
 * Some C++ implementations may store allocation metadata, commonly referred
 * to as an array cookie, alongside an array allocation. Such metadata may
 * contain information about the number of elements in the allocation and can
 * be used by the implementation to recover the array extent from a pointer.
 *
 * However, the representation and availability of array cookies are
 * implementation-specific. They are not part of the C++ language-level
 * interface for retrieving an array's bound, and their layout, location, and
 * usage are not guaranteed to be consistent across compilers, standard
 * libraries, allocation mechanisms, or platforms.
 *
 * NexCore deliberately does not inspect or depend on such implementation-
 * specific allocation metadata. The array bound is tracked explicitly as
 * part of the UniquePtr<T[]> object instead. This keeps the implementation
 * independent of compiler-specific allocation details and avoids relying on
 * undocumented assumptions about the underlying array allocation.
 *
 * The trade-off is that UniquePtr<T[]> cannot automatically recover the
 * actual extent of an arbitrary array from a raw pointer. When the bound is
 * unknown, runtime bounds checking cannot provide a meaningful guarantee
 * about whether an index is within the actual allocation.
 *
 * The checker is maintained together with the managed pointer throughout
 * ownership operations:
 * - Construction with a known element count initializes the stored bound.
 * - Construction without a known element count marks the bound as unknown.
 * - Move operations transfer the bound together with the managed pointer.
 * - Resetting the managed pointer updates the bound according to the
 *   information available for the new pointer.
 * - Swapping two UniquePtr<T[]> objects also swaps their bound information.
 *
 * The checker does not own or manage the array allocation. It only retains
 * the bound information associated with the managed pointer. The actual
 * allocation and deallocation responsibilities remain with UniquePtr and
 * its deleter.
 *
 * Note:
 * The stored bound represents the number of elements, not the number of bytes.
 * It is meaningful only while it corresponds to the currently managed array.
 * Users must not assume that a bound can be inferred or validated for pointers
 * whose extent was not explicitly provided.
 */
struct NEX_INTERNAL UniquePtrArrayBoundChecker {
    NEX_HIDDEN_FROM_ABI constexpr UniquePtrArrayBoundChecker() noexcept
        : size(NEX_SIZE_MAX) {}
    NEX_HIDDEN_FROM_ABI constexpr explicit UniquePtrArrayBoundChecker(usize size) noexcept
        : size(size) {}

    template <class Deleter, class Type>
    NEX_HIDDEN_FROM_ABI constexpr bool isInBounds(Type*, usize index) const noexcept {
        return index < size;
    }

private:
    usize size;
};

/**
 * @class UniquePtr<Type[], Deleter>
 * @brief A specialization of UniquePtr for array types.
 *
 * @details
 * This specialization of UniquePtr manages dynamically allocated arrays.
 * It provides bounds checking and ensures that the array is properly deleted
 * when the UniquePtr goes out of scope. The deleter type can be customized,
 * and the pointer type is deduced from the deleter if available.
 *
 * @tparam Type The type of the elements in the array.
 * @tparam Deleter The type of the deleter used to destroy the array.
 *
 * @note
 * This specialization is intended for managing arrays and provides additional
 * bounds checking compared to the primary UniquePtr template.
 */
template <
    class Type,
    class Deleter
> requires (!IsRvalueReferenceV<Deleter>)
class NEX_API UniquePtr<Type[], Deleter> {
public:
    using element_type = Type;
    using deleter_type = Deleter;

    // We'd rather use the pointer type detected from the deleter, if available.
    using pointer = DetectedPointerT<Type, Deleter>;

private:
    // storage_ := {{ ptr, deleter }}
    CompressedPair<pointer, deleter_type> storage_;

    using BoundChecker = UniquePtrArrayBoundChecker;

    NEX_NO_UNIQUE_ADDRESS BoundChecker checker_;

    // Internal accessor to the stored pointer.
    NEX_HIDDEN_FROM_ABI inline pointer& ptr() noexcept { return storage_.first; }
    NEX_HIDDEN_FROM_ABI inline pointer ptr() const noexcept { return storage_.first; }

    // Internal accessor to the stored deleter.
    NEX_HIDDEN_FROM_ABI inline deleter_type& deleter() noexcept { return storage_.second; }
    NEX_HIDDEN_FROM_ABI inline const deleter_type& deleter() const noexcept { return storage_.second; }

    // Validator for the deleter type to ensure it meets the requirements for UniquePtr.
    // This checks if the deleter type is suitable for use with UniquePtr, including
    // whether it is default constructible, move constructible, and assignable as needed.
    using DeleterValidator = UniquePtrDeleterValidator<deleter_type>;

    template <bool Dummy = true>
    using LValRefType = typename meta::DependentType<DeleterValidator, Dummy>::LValRefType;

    template <bool Dummy = true>
    using GoodRValRefType = typename meta::DependentType<DeleterValidator, Dummy>::GoodRValRefType;

    template <bool Dummy = true>
    using BadRValRefType = typename meta::DependentType<DeleterValidator, Dummy>::BadRValRefType;

    // Validator for checking if the deleter type is default constructible and not a pointer.
    // This ensures that the deleter can be safely default constructed and is not a raw pointer type,
    // which could lead to undefined behavior.
    template <
        bool Dummy = true,
        class DeleterT = meta::DependentType<TypeIdentityT<deleter_type>, Dummy>::type
    >
    struct IsDeleterDefaultConstructible
        : meta::And<
            IsDefaultConstructible<DeleterT>,
            meta::Not<IsPointerV<DeleterT> >
        > {};

    // Validator for checking if the deleter type is constructible with a given argument type.
    // This ensures that the deleter can be properly constructed with the provided argument.
    template <class ArgType>
    struct IsDeleterConstructible : IsConstructible<deleter_type, ArgType> {};

    // Validator for checking if a pointer type is convertible to the UniquePtr's pointer type.
    // This ensures that the pointer can be safely used with the UniquePtr.
    template <class From>
    struct IsPointerConvertible : IsSame<From, pointer> {};

    template <class FromElement>
    struct IsPointerConvertible<FromElement*>
        : meta::Or<
            IsSame<FromElement*, pointer>,
            meta::And<
                IsSame<pointer, element_type*>,
                IsConvertible<FromElement (*)[], element_type (*)[]>
            >
        > {};

    // Validator for checking if a UniquePtr can be move constructed from another UniquePtr.
    // This ensures that the move operation is valid for array types and compatible pointer types.
    template <class OtherPtr, class OtherType, class ElementType = typename OtherPtr::element_type>
    struct IsMoveConvertible
        : meta::And<
            IsArray<OtherType>,
            IsSame<pointer, element_type*>,
            IsSame<typename OtherPtr::pointer, ElementType*>,
            IsConvertible<ElementType (*)[], element_type (*)[]>
        > {};

    // Validator for checking if a deleter type is convertible from another deleter type.
    // This ensures that the deleter can be properly assigned from a different deleter type if needed.
    template <class OtherDeleter>
    struct IsDeleterConvertible
        : meta::Or<
            meta::And<
                IsReference<deleter_type>,
                IsSame<deleter_type, OtherDeleter>
            >,
            meta::And<
                meta::Not<IsReferenceV<deleter_type> >,
                IsConvertible<OtherDeleter, deleter_type>
            >
        > {};

    // Validator for checking if a deleter type is assignable from another deleter type.
    // This ensures that the deleter can be properly assigned when needed.
    template <class OtherDeleter>
    struct IsDeleterAssignable : IsAssignable<deleter_type&, OtherDeleter&&> {};

public:
    /**
     * @brief Constructs a UniquePtr with a null pointer and a default-constructed deleter.
     * @note Requires that the deleter type is default constructible.
     */
    template <bool Dummy = true>
        requires (IsDeleterDefaultConstructible<Dummy>::value)
    NEX_HIDDEN_FROM_ABI constexpr UniquePtr() noexcept
        : storage_(pointer(), deleter_type()) {}

    /**
     * @brief Constructs a UniquePtr with a null pointer and a default-constructed deleter.
     * @note Requires that the deleter type is default constructible.
     */
    template <bool Dummy = true>
        requires (IsDeleterDefaultConstructible<Dummy>::value)
    NEX_HIDDEN_FROM_ABI constexpr UniquePtr(null) noexcept
        : storage_(pointer(), deleter_type()) {}

    /**
     * @brief Constructs a UniquePtr with a given pointer and a default-constructed deleter.
     * @note Requires that the deleter type is default constructible and the pointer is convertible.
     */
    template <class PointerType, bool Dummy = true>
        requires (
            IsDeleterDefaultConstructible<Dummy>::value &&
            IsPointerConvertible<PointerType>::value)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 explicit UniquePtr(PointerType ptr) noexcept
        : storage_(ptr, deleter_type()) {}

private:
    // Allows access to the private members of UniquePtr for other instantiations of the template.
    // This is necessary for operations like converting between UniquePtr types with different deleters.
    template <class OtherType, class OtherDeleter>
    friend class UniquePtr;

    template <class Type>
    friend NEX_CONSTEXPR23 UniquePtr<Type> makeUnique(usize);

    template <class Type>
    friend NEX_CONSTEXPR23 UniquePtr<Type> makeUniqueForOverwrite(usize);

    // Private constructor used by makeUnique & friends to pass the size that was allocated.
    template <class PointerType>
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 explicit UniquePtr(PointerType ptr, usize size) noexcept
        : storage_(ptr, deleter_type()), checker_(size) {}

public:
    /**
     * @brief Constructs a UniquePtr with a given pointer and a specified lvalue reference deleter.
     * @note Requires that the deleter type is constructible from the provided lvalue reference.
     */
    template <class PointerType, bool Dummy = true>
        requires (
            IsDeleterConstructible<LValRefType<Dummy> >::value &&
            IsPointerConvertible<PointerType>::value)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(PointerType ptr, LValRefType<Dummy> deleter) noexcept
        : storage_(ptr, deleter) {}

    /**
     * @brief Constructs a UniquePtr with a null pointer and a specified lvalue reference deleter.
     * @note Requires that the deleter type is constructible from the provided lvalue reference.
     */
    template <bool Dummy = true>
        requires (IsDeleterConstructible<LValRefType<Dummy> >::value)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(null, LValRefType<Dummy> deleter) noexcept
        : storage_(nullptr, deleter) {}

    /**
     * @brief Constructs a UniquePtr with a given pointer and a specified rvalue reference deleter.
     * @note Requires that the deleter type is constructible from the provided rvalue reference
     *       and the pointer is convertible.
     */
    template <class PointerType, bool Dummy = true>
        requires (
            IsDeleterConstructible<GoodRValRefType<Dummy> >::value &&
            IsPointerConvertible<PointerType>::value &&
            !IsReferenceV<deleter_type>)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(PointerType ptr, GoodRValRefType<Dummy> deleter) noexcept
        : storage_(ptr, NEX_MOVE(deleter)) {}

    /**
     * @brief Constructs a UniquePtr with a null pointer and a specified rvalue reference deleter.
     * @note Requires that the deleter type is constructible from the provided rvalue reference
     *       and is not a reference type.
     */
    template <bool Dummy = true>
        requires (
            IsDeleterConstructible<GoodRValRefType<Dummy> >::value &&
            !IsReferenceV<deleter_type>)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(null, GoodRValRefType<Dummy> deleter) noexcept
        : storage_(nullptr, NEX_MOVE(deleter)) {}

    /**
     * @brief Deleted constructor for a UniquePtr with a given pointer and a specified bad rvalue reference deleter.
     * @note Ensures that the deleter type cannot be constructed from the provided bad rvalue reference.
     */
    template <class PointerType, bool Dummy = true>
        requires (
            IsDeleterConstructible<BadRValRefType<Dummy> >::value &&
            IsPointerConvertible<PointerType>::value)
    NEX_HIDDEN_FROM_ABI UniquePtr(PointerType ptr, BadRValRefType<Dummy> deleter) noexcept = delete;

    /**
     * @brief Constructs a UniquePtr by moving from another UniquePtr.
     * @note Requires that the deleter type is convertible.
     */
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(UniquePtr&& other) noexcept
        requires (IsDeleterConvertible<deleter_type>::value)
        : storage_(other.release(), NEX_FORWARD<deleter_type>(other.getDeleter())),
          checker_(NEX_MOVE(other.checker_)) {}

    /**
     * @brief Move assignment operator for UniquePtr.
     * @note Requires that the deleter type is assignable.
     */
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr& operator=(UniquePtr&& other) noexcept
        requires (IsDeleterAssignable<deleter_type>::value) {
        reset(other.release());
        deleter() = NEX_FORWARD<deleter_type>(other.getDeleter());
        checker_ = NEX_MOVE(other.checker_);
        return *this;
    }

    /**
     * @brief Constructs a UniquePtr by moving from another UniquePtr with a different pointer and deleter type.
     * @note Requires that the other UniquePtr is move convertible to this UniquePtr and the deleter is convertible.
     */
    template <class OtherType, class OtherDeleter>
        requires (
            IsMoveConvertible<UniquePtr<OtherType, OtherDeleter>, OtherType>::value &&
            IsDeleterConvertible<OtherDeleter>::value)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr(UniquePtr<OtherType, OtherDeleter>&& other) noexcept
        : storage_(other.release(), NEX_FORWARD<OtherDeleter>(other.getDeleter())),
          checker_(NEX_MOVE(other.checker_)) {}

    /**
     * @brief Move assignment operator for a UniquePtr with a different pointer and deleter type.
     * @note Requires that the other UniquePtr is move convertible to this UniquePtr and the deleter is assignable.
     */
    template <class OtherType, class OtherDeleter>
        requires (
            IsMoveConvertible<UniquePtr<OtherType, OtherDeleter>, OtherType>::value &&
            IsDeleterAssignable<OtherDeleter>::value)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr& operator=(UniquePtr<OtherType, OtherDeleter>&& other) noexcept {
        reset(other.release());
        deleter() = NEX_FORWARD<OtherDeleter>(other.getDeleter());
        checker_ = NEX_MOVE(other.checker_);
        return *this;
    }

    // Destructor for UniquePtr that resets the managed pointer and invokes the deleter if necessary.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 ~UniquePtr() { reset(); }

    // Assignment operator for assigning nullptr to the UniquePtr, which resets the managed pointer.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr& operator=(null) noexcept {
        reset();
        return *this;
    }

    // Subscript operator for accessing elements in the managed array with bounds checking.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 auto operator[](usize index) const -> AddLvalueReference<element_type> {
        NEX_ASSERT_VALID_ELEMENT_ACCESS(checker_.isInBounds<deleter_type>(NEX_TO_ADDRESS(ptr()), index),
            "Error in UniquePtr<T[]>::operator[]: Index out of bounds.");
        return ptr()[index];
    }

    // Returns the raw pointer managed by the UniquePtr.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 pointer get() const noexcept { return ptr(); }

    // Returns the deleter associated with the UniquePtr.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 deleter_type& getDeleter() noexcept { return deleter(); }
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 const deleter_type& getDeleter() const noexcept { return deleter(); }

    // Returns true if the UniquePtr manages a non-null pointer, false otherwise.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 explicit operator bool() const noexcept { return ptr() != nullptr; }

    // Releases ownership of the managed pointer and returns it, leaving the UniquePtr empty.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 pointer release() noexcept {
        pointer tmp = ptr();
        ptr() = pointer();
        // The deleter and the optional bounds-checker are left unchanged.
        // The bounds-checker will be reinitialized appropriately when/if the UniquePtr gets assigned-to or reset.
        return tmp;
    }

    // Resets the UniquePtr with a new pointer, invoking the deleter on the old pointer if necessary.
    template <class PointerType>
        requires (IsPointerConvertible<PointerType>::value)
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 void reset(PointerType newPtr) noexcept {
        pointer tmp = ptr();
        ptr() = newPtr;
        checker_ = BoundChecker();
        if (tmp != nullptr) {
            deleter()(tmp);
        }
    }

    // Resets the UniquePtr to nullptr, invoking the deleter on the old pointer if necessary.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 void reset(null = nullptr) noexcept {
        pointer tmp = ptr();
        ptr() = nullptr;
        checker_ = BoundChecker();
        if (tmp != nullptr) {
            deleter()(tmp);
        }
    }

    /**
     * @brief Swaps the contents of this UniquePtr with another UniquePtr.
     * @details
     * This function exchanges the managed pointer, deleter, and optional bounds-checker
     * between this UniquePtr and the specified other UniquePtr.
     * It requires that the deleter type is swappable.
     */
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 void swap(UniquePtr& other)
        noexcept(IsNothrowSwappableV<deleter_type>)
        requires (IsSwappableV<deleter_type>) {
        NEX_SWAP(ptr(), other.ptr());
        NEX_SWAP(deleter(), other.deleter());
        NEX_SWAP(checker_, other.checker_);
    }
};

/**
 * @brief Swaps two UniquePtr instances.
 * @tparam Type The type of object managed by the UniquePtr.
 * @tparam Deleter The type of deleter used by the UniquePtr.
 * @param lhs The first UniquePtr to swap.
 * @param rhs The second UniquePtr to swap.
 * @details
 * This function swaps the contents of two UniquePtr instances, including their pointers,
 * deleters, and optional bounds-checkers. It requires that the deleter type is swappable.
 */
template <class Type, class Deleter>
    requires (IsSwappableV<Deleter>)
NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 void swap(UniquePtr<Type, Deleter>& lhs, UniquePtr<Type, Deleter>& rhs)
    noexcept(IsNothrowSwappableV<Deleter>) {
    lhs.swap(rhs);
}

/**
 * @brief Compares two UniquePtr instances for equality.
 * @tparam Type1 The type of object managed by the first UniquePtr.
 * @tparam Deleter1 The type of deleter used by the first UniquePtr.
 * @tparam Type2 The type of object managed by the second UniquePtr.
 * @tparam Deleter2 The type of deleter used by the second UniquePtr.
 * @param lhs The first UniquePtr to compare.
 * @param rhs The second UniquePtr to compare.
 * @return True if both UniquePtr instances point to the same object, false otherwise.
 * @details This function compares the raw pointers held by two UniquePtr instances for equality.
 */
template <class Type1, class Deleter1, class Type2, class Deleter2>
NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23
bool operator==(const UniquePtr<Type1, Deleter1>& lhs, const UniquePtr<Type2, Deleter2>& rhs) noexcept {
    return lhs.get() == rhs.get();
}

// TODO: Inequality comparison operators will be later implemented if needed.

/**
 * @brief Compares a UniquePtr instance with a null pointer for equality.
 * @tparam Type The type of object managed by the UniquePtr.
 * @tparam Deleter The type of deleter used by the UniquePtr.
 * @param ptr The UniquePtr to compare.
 * @param null The null pointer to compare against.
 * @return True if the UniquePtr does not hold an object, false otherwise.
 * @details This function checks if the UniquePtr instance is empty (i.e., does not hold a pointer).
 */
template <class Type, class Deleter>
NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 bool operator==(const UniquePtr<Type, Deleter>& ptr, null) noexcept {
    return !ptr;
}

// TODO: Inequality comparison operators for null-pointer will be later implemented if needed.

/**
 * @brief Creates a UniquePtr instance by forwarding arguments to the constructor of the managed object.
 * @tparam Type The type of object to manage.
 * @tparam Args The types of arguments to forward to the constructor of the managed object.
 * @param args The arguments to forward to the constructor of the managed object.
 * @return A UniquePtr managing the newly created object.
 * @details
 * This function constructs a new object of type Type using the provided arguments
 * and returns a UniquePtr managing it.
 */
template <class Type, class... Args>
    requires (!IsArrayV<Type> && IsConstructibleV<Type, Args...>)
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr<Type> makeUnique(Args&&... args) {
    return UniquePtr<Type>(new Type(NEX_FORWARD<Args>(args)...));
}

/**
 * @brief Creates a UniquePtr instance managing an unbounded array.
 * @tparam Type The type of array to manage.
 * @param elementCount The number of elements in the array.
 * @return A UniquePtr managing the newly created array.
 * @details
 * This function constructs a new array of type Type with the specified number of elements
 * and returns a UniquePtr managing it.
 */
template <class Type>
    requires (IsUnboundedArrayV<Type>)
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr<Type> makeUnique(usize elementCount) {
    using ElementType = RemoveExtentT<Type>;
    return UniquePtr<Type>(new ElementType[elementCount](), elementCount);
}

// makeUnique for bounded arrays is deleted to prevent misuse.
template <class Type, class... Args>
    requires (IsBoundedArrayV<Type>)
void makeUnique(Args&&...) = delete;

/**
 * @brief Creates a UniquePtr instance for overwriting an object.
 * @tparam Type The type of object to manage.
 * @return A UniquePtr managing the newly created object.
 * @details
 * This function constructs a new object of type Type without initializing it
 * and returns a UniquePtr managing it.
 */
template <class Type>
    requires (!IsArrayV<Type>)
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr<Type> makeUniqueForOverwrite() {
    return UniquePtr<Type>(new Type);
}

/**
 * @brief Creates a UniquePtr instance for overwriting an unbounded array.
 * @tparam Type The type of array to manage.
 * @param elementCount The number of elements in the array.
 * @return A UniquePtr managing the newly created array.
 * @details
 * This function constructs a new array of type Type with the specified number of elements
 * without initializing them and returns a UniquePtr managing it.
 */
template <class Type>
    requires (IsUnboundedArrayV<Type>)
NEX_NODISCARD NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 UniquePtr<Type> makeUniqueForOverwrite(usize elementCount) {
    using ElementType = RemoveExtentT<Type>;
    return UniquePtr<Type>(new ElementType[elementCount], elementCount);
}

// makeUniqueForOverwrite for bounded arrays is deleted to prevent misuse.
template <class Type, class... Args>
    requires (IsBoundedArrayV<Type>)
void makeUniqueForOverwrite(Args&&...) = delete;

/**
 * @brief Hash specialization for UniquePtr.
 * @tparam Type The type of object managed by the UniquePtr.
 * @tparam Deleter The deleter type used by the UniquePtr.
 * @details
 * This specialization allows UniquePtr instances to be used in hash-based containers
 * by providing a hash function based on the managed pointer.
 */
template <class Type, class Deleter>
    requires HasEnabledHash<typename UniquePtr<Type, Deleter>::pointer>
struct Hash<UniquePtr<Type, Deleter>> {
    using argument_type = UniquePtr<Type, Deleter>;
    using result_type = usize;

    NEX_NODISCARD NEX_HIDDEN_FROM_ABI usize operator()(const UniquePtr<Type, Deleter>& ptr) const {
        using pointer = UniquePtr<Type, Deleter>::pointer;
        return Hash<pointer>()(ptr.get());
    }
};

NEX_NAMESPACE_END
