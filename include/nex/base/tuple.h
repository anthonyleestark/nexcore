/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/attributes.h"
#include "nex/base/export.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/fwd.h"
#include "nex/base/casts.h"
#include "nex/base/invoke.h"
#include "nex/base/utility.h"
#include "nex/base/type_concepts.h"
#include "nex/base/type_traits.h"
#include "nex/base/allocator_traits.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief A helper variable template that checks if a type is an empty type and not final.
 * @tparam Type The specified type to check.
 * @note This variable template evaluates to true if the type is empty and not final, and false otherwise.
 */
template <class Type>
inline constexpr bool IsTupleLeafEmptyTypeV = IsEmptyV<Type> && !IsFinalV<Type>;

// Forward declaration of the TupleLeaf struct template, which represents a single element in a Tuple.
template <usize Index, class Type, bool = IsTupleLeafEmptyTypeV<Type>>
struct TupleLeaf;

/**
 * @brief Swaps the values of two TupleLeaf instances.
 * @tparam Index The index of the TupleLeaf in the Tuple.
 * @tparam Type The type of the element in the TupleLeaf.
 * @tparam Empty A boolean indicating whether the TupleLeaf is empty.
 * @param lhs The left-hand side TupleLeaf to swap.
 * @param rhs The right-hand side TupleLeaf to swap.
 */
template <usize Index, class Type, bool Empty>
NEX_HIDDEN_FROM_ABI inline constexpr
void swap(TupleLeaf<Index, Type, Empty>& lhs, TupleLeaf<Index, Type, Empty>& rhs)
    noexcept(IsNothrowSwappableV<Type>) {
    NEX_SWAP(lhs.get(), rhs.get());
}

/**
 * @brief Swaps the values of two const TupleLeaf instances.
 * @tparam Index The index of the TupleLeaf in the Tuple.
 * @tparam Type The type of the element in the TupleLeaf.
 * @tparam Empty A boolean indicating whether the TupleLeaf is empty.
 * @param lhs The left-hand side const TupleLeaf to swap.
 * @param rhs The right-hand side const TupleLeaf to swap.
 */
template <usize Index, class Type, bool Empty>
NEX_HIDDEN_FROM_ABI constexpr
void swap(const TupleLeaf<Index, Type, Empty>& lhs, const TupleLeaf<Index, Type, Empty>& rhs)
    noexcept(IsNothrowSwappableV<const Type>) {
    NEX_SWAP(lhs.get(), rhs.get());
}

/**
 * @struct TupleLeaf
 * @brief A struct template that represents a single element in a Tuple.
 * @tparam Index The index of the element in the Tuple.
 * @tparam Type The type of the element in the Tuple.
 * @tparam Empty A boolean indicating whether the TupleLeaf is empty (default is determined
 *         by IsTupleLeafEmptyTypeV).
 */
template <usize Index, class Type, bool>
struct NEX_INTERNAL TupleLeaf {
    using value_type = Type;
    value_type value;

    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf& operator=(const TupleLeaf&) = delete;

    // Default constructor for TupleLeaf, which initializes the value member.
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf() noexcept(IsNothrowDefaultConstructibleV<value_type>) : value() {
        static_assert(!IsReferenceV<value_type>,
            "Error: Attempted to default construct a reference element in a tuple");
    }

    // Default-constructs a TupleLeaf without using an allocator despite specifying one.
    // This constructor is only enabled if the TupleLeaf can be default-constructed (not a reference type).
    template <class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 0>, const Alloc&) : value() {
        static_assert(!IsReferenceV<value_type>,
            "Error: Attempted to default construct a reference element in a tuple");
    }

    // Constructs a TupleLeaf using an allocator.
    // This constructor is only enabled if the TupleLeaf can be default-constructed (not a reference type).
    template <class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 1>, const Alloc& alloc)
        : value(allocator_arg, alloc) {
        static_assert(!IsReferenceV<value_type>,
            "Error: Attempted to default construct a reference element in a tuple");
    }

    // Constructs a TupleLeaf using an allocator.
    // This constructor is only enabled if the TupleLeaf can be constructed with the given allocator.
    template <class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 2>, const Alloc& alloc)
        : value(alloc) {
        static_assert(!IsReferenceV<value_type>,
            "Error: Attempted to default construct a reference element in a tuple");
    }

    // Constructor for TupleLeaf that accepts a single argument and forwards it to the value_type constructor.
    // This constructor is only enabled if the TupleLeaf can be constructed from the given argument type.
    template <class OtherType>
        requires (!IsSameV<RemoveCvrefT<OtherType>, TupleLeaf> && IsConstructibleV<value_type, OtherType>)
    NEX_HIDDEN_FROM_ABI constexpr explicit TupleLeaf(OtherType&& value)
        noexcept(IsNothrowConstructibleV<value_type, OtherType>)
        : value(NEX_FORWARD<OtherType>(value)) {
        static_assert(!meta::ReferenceConstructsFromTemporaryV<value_type, OtherType&&>,
            "Error: Attempted construction of reference element binds to a temporary whose lifetime has ended");
    }

    // Constructs a TupleLeaf from a value and an allocator without using the allocator.
    // This constructor is only enabled if the TupleLeaf can be constructed from the given argument type.
    template <class OtherType, class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 0>, const Alloc&, OtherType&& value)
        : value(NEX_FORWARD<OtherType>(value)) {
        static_assert(!meta::ReferenceConstructsFromTemporaryV<value_type, OtherType&&>,
            "Error: Attempted construction of reference element binds to a temporary whose lifetime has ended");
    }

    // Constructs a TupleLeaf from a value and an allocator using the allocator.
    // This constructor is only enabled if the TupleLeaf can be constructed from the given argument type.
    template <class OtherType, class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 1>, const Alloc& alloc, OtherType&& value)
        : value(allocator_arg, alloc, NEX_FORWARD<OtherType>(value)) {
        static_assert(!IsReferenceV<value_type>,
            "Error: Attempted to uses-allocator construct a reference element in a tuple");
    }

    // Constructs a TupleLeaf from a value and an allocator using the allocator in a different order.
    // This constructor is only enabled if the TupleLeaf can be constructed from the given argument type.
    template <class OtherType, class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 2>, const Alloc& alloc, OtherType&& value)
        : value(NEX_FORWARD<OtherType>(value), alloc) {
        static_assert(!IsReferenceV<value_type>,
            "Error: Attempted to uses-allocator construct a reference element in a tuple");
    }

    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(const TupleLeaf&) = default;
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(TupleLeaf&&) = default;

    // Swaps the values of two TupleLeaf instances.
    NEX_HIDDEN_FROM_ABI constexpr
    void swap(TupleLeaf& rhs) noexcept(IsNothrowSwappableV<value_type>) {
        NEX_SWAP(get(), rhs.get());
    }

    // Swaps the values of two const TupleLeaf instances.
    NEX_HIDDEN_FROM_ABI constexpr
    void swap(const TupleLeaf& rhs) const noexcept(IsNothrowSwappableV<const value_type>) {
        NEX_SWAP(get(), rhs.get());
    }

    // Retrieves a reference to the value member of the TupleLeaf.
    NEX_HIDDEN_FROM_ABI constexpr value_type& get() noexcept { return value; }
    NEX_HIDDEN_FROM_ABI constexpr const value_type& get() const noexcept { return value; }
};

/**
 * @struct TupleLeaf<Index, Type, true>
 * @brief A specialization of the TupleLeaf struct template for empty types that are not final.
 * @tparam Index The index of the element in the Tuple.
 * @tparam Type The type of the element in the Tuple.
 */
template <usize Index, class Type>
struct NEX_INTERNAL TupleLeaf<Index, Type, true> : private RemoveCvT<Type> {
    using value_type = Type;
    using base_type = RemoveCvT<Type>;

    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf& operator=(const TupleLeaf&) = delete;

    // Default constructor for TupleLeaf of an empty type.
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf() noexcept(IsNothrowDefaultConstructibleV<value_type>) {}

    // Constructor for TupleLeaf of an empty type that accepts an allocator without using it.
    template <class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 0>, const Alloc&) {}

    // Constructor for TupleLeaf of an empty type that accepts an allocator.
    template <class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 1>, const Alloc& alloc)
        : base_type(allocator_arg, alloc) {}

    // Constructor for TupleLeaf of an empty type that accepts an allocator in a different order.
    template <class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 2>, const Alloc& alloc)
        : base_type(alloc) {}

    // Constructor for TupleLeaf that accepts a single argument and forwards it to the value_type constructor.
    template <class OtherType>
        requires (!IsSameV<RemoveCvrefT<OtherType>, TupleLeaf> && IsConstructibleV<value_type, OtherType>)
    NEX_HIDDEN_FROM_ABI constexpr explicit TupleLeaf(OtherType&& value)
        noexcept(IsNothrowConstructibleV<value_type, OtherType>)
        : base_type(NEX_FORWARD<OtherType>(value)) {}

    // Constructor for TupleLeaf that accepts a value and an allocator without using the allocator.
    template <class OtherType, class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 0>, const Alloc&, OtherType&& value)
        : base_type(NEX_FORWARD<OtherType>(value)) {}

    // Constructor for TupleLeaf that accepts a value and an allocator using the allocator.
    template <class OtherType, class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 1>, const Alloc& alloc, OtherType&& value)
        : base_type(allocator_arg, alloc, NEX_FORWARD<OtherType>(value)) {}

    // Constructor for TupleLeaf that accepts a value and an allocator in a different order.
    template <class OtherType, class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(IntegralConstant<int32, 2>, const Alloc& alloc, OtherType&& value)
        : base_type(NEX_FORWARD<OtherType>(value), alloc) {}

    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(const TupleLeaf&) = default;
    NEX_HIDDEN_FROM_ABI constexpr TupleLeaf(TupleLeaf&&) = default;

    // Swaps the values of two TupleLeaf instances.
    NEX_HIDDEN_FROM_ABI constexpr
    void swap(TupleLeaf& rhs) noexcept(IsNothrowSwappableV<value_type>) {
        NEX_SWAP(get(), rhs.get());
    }

    // Swaps the values of two const TupleLeaf instances.
    NEX_HIDDEN_FROM_ABI constexpr
    void swap(const TupleLeaf& rhs) const noexcept(IsNothrowSwappableV<const value_type>) {
        NEX_SWAP(get(), rhs.get());
    }

    // Retrieves a reference to the base class (empty type) of the TupleLeaf.
    NEX_HIDDEN_FROM_ABI constexpr value_type& get() noexcept { return static_cast<value_type&>(*this); }
    NEX_HIDDEN_FROM_ABI constexpr const value_type& get() const noexcept {
        return static_cast<const value_type&>(*this);
    }
};

/**
 * @brief A helper struct that provides utility functions for working with Tuples.
 * @note  FIXME: This needs to be optimized later for compile-time usage, should not use nested structs.
 */
struct NEX_INTERNAL TupleHelper {
    /**
     * @brief A helper function that swallows a parameter pack of types.
     * @tparam Types The types to swallow.
     * @param args The parameter pack of types to swallow.
     */
    template <class ... Types>
    NEX_HIDDEN_FROM_ABI static constexpr void swallowTypes(Types&&...) noexcept {}

    /**
     * @brief A helper function that performs copy assignment for Tuple elements.
     * @tparam Dest The destination type.
     * @tparam Source The source type.
     * @tparam Indices The compile-time sequence of indices for the Tuple elements.
     * @param dest The destination Tuple to assign to.
     * @param source The source Tuple to assign from.
     * @param indices The compile-time sequence of indices for the Tuple elements.
     */
    template <class Dest, class Source, usize... Indices>
    NEX_HIDDEN_FROM_ABI static constexpr
    void copyAssign(Dest& dest, const Source& source, IndexSequence<Indices...>) {
        swallowTypes(((get<Indices>(dest) = get<Indices>(source)), void(), 0)...);
    }

    /**
     * @brief A helper function that performs forward assignment for Tuple elements.
     * @tparam Dest The destination type.
     * @tparam Source The source type.
     * @tparam Types The types of the elements in the Tuple.
     * @tparam Indices The compile-time sequence of indices for the Tuple elements.
     * @param dest The destination Tuple to assign to.
     * @param source The source Tuple to assign from.
     * @param types The types of the elements in the Tuple.
     * @param indices The compile-time sequence of indices for the Tuple elements.
     */
    template <class Dest, class Source, class... Types, usize... Indices>
    NEX_HIDDEN_FROM_ABI static constexpr
    void forwardAssign(Dest& dest, Source&& source, TupleTypes<Types...>, IndexSequence<Indices...>) {
        swallowTypes(((get<Indices>(dest) = NEX_FORWARD<Types>(get<Indices>(source))), void(), 0)...);
    }

    /**
     * @brief A helper function that compares Tuple elements for equality.
     * @tparam Index The current index being compared.
     * @tparam Tuple1 The first Tuple type.
     * @tparam Tuple2 The second Tuple type.
     * @param lhs The first Tuple to compare.
     * @param rhs The second Tuple to compare.
     * @return true if all compared elements are equal, false otherwise.
     */
    template <usize Index, class Tuple1, class Tuple2>
    NEX_HIDDEN_FROM_ABI static constexpr
    bool compareEqual(const Tuple1& lhs, const Tuple2& rhs) {
        if constexpr (Index == 0) {
            return true;
        } else {
            return compareEqual<Index - 1>(lhs, rhs) && (get<Index - 1>(lhs) == get<Index - 1>(rhs));
        }
    }

    /**
     * @brief A helper struct that provides compile-time type searching utilities for Tuples.
     */
    struct Find {
        static constexpr usize notFound     = static_cast<usize>(-1);
        static constexpr usize ambiguous    = notFound - 1;

        /**
         * @brief Helper function to determine the index to return based on current index, result, and match status.
         * @param curIdx The current index being checked.
         * @param res The result index from the recursive search.
         * @param matches Whether the current index matches the type being searched for.
         * @return The appropriate index to return based on the search outcome.
         */
        NEX_HIDDEN_FROM_ABI inline static constexpr usize findIdxReturn(usize curIdx, usize res, bool matches) {
            return !matches ? res : (res == Find::notFound ? curIdx : Find::ambiguous);
        }

        /**
         * @brief Helper function to recursively find the index of a type match in a boolean array.
         * @tparam N The size of the boolean array.
         * @param idx The current index being checked.
         * @param matches The boolean array indicating type matches.
         * @return The index of the matching type, or notFound/ambiguous as appropriate.
         */
        template <usize N>
        NEX_HIDDEN_FROM_ABI inline static constexpr usize findIdx(usize idx, const bool (&matches)[N]) {
            return idx == N ? Find::notFound : findIdxReturn(idx, findIdx(idx + 1, matches), matches[idx]);
        }

        /**
         * @struct FindExactlyOne
         * @brief A helper struct that finds exactly one occurrence of a type in a type list.
         *        Triggers static assertions if the type is not found or occurs more than once.
         */
        template <class Type, class... Args>
        struct FindExactlyOne {
            static constexpr bool matches[sizeof...(Args)] = { IsSameV<Type, Args>... };
            static constexpr usize value = Find::findIdx(0, matches);
            static_assert(value != Find::notFound,  "Error: Type not found in type list");
            static_assert(value != Find::ambiguous, "Error: Type occurs more than once in type list");
        };

        /**
         * @struct FindExactlyOne<Type>
         * @brief Specialization of FindExactlyOne for an empty type list.
         *        Triggers a static assertion failure indicating the type is not found.
         */
        template <class Type>
        struct FindExactlyOne<Type> {
            static_assert(AlwaysFalseV<Type>, "Error: Type not in empty type list");
        };
    };
};

// Primary template for the TupleStorage struct template, which represents the storage of a Tuple.
template <class Index, class... Types>
struct TupleStorage;

// Internal marker struct used to indicate conversion from a Tuple.
struct NEX_INTERNAL from_tuple {};

/**
 * @struct TupleStorage
 * @brief A specialization of the TupleStorage struct template for a Tuple
 *        with a specific index sequence and types.
 * @tparam Indices The compile-time sequence of indices for the Tuple.
 * @tparam Types The types of the elements in the Tuple.
 */
template <usize... Indices, class... Types>
struct NEX_INTERNAL TupleStorage<IndexSequence<Indices...>, Types...> : public TupleLeaf<Indices, Types>... {
    // Default constructor for TupleStorage, which initializes the base classes (TupleLeaf instances).
    NEX_HIDDEN_FROM_ABI constexpr TupleStorage()
        noexcept(meta::AllTrueV<IsNothrowDefaultConstructibleV<Types>...>) {}

    // Constructor for TupleStorage that forwards arguments to the base classes (TupleLeaf instances).
    template <class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit TupleStorage(forward_args_tag, Args&&... args)
        : TupleLeaf<Indices, Types>(NEX_FORWARD<Args>(args))... {}

    // Constructor for TupleStorage that initializes the base classes (TupleLeaf instances)
    // using an allocator and value initialization.
    template <class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr explicit TupleStorage(allocator_arg_tag, const Alloc& alloc, value_init_tag)
        : TupleLeaf<Indices, Types>(UsesAllocatorConstruction<Types, Alloc>(), alloc)... {}

    // Constructor for TupleStorage that initializes the base classes (TupleLeaf instances)
    // using an allocator and forwarded arguments.
    template <class Alloc, class... Args>
    NEX_HIDDEN_FROM_ABI constexpr explicit
    TupleStorage(allocator_arg_tag, const Alloc& alloc, forward_args_tag, Args&&... args)
        : TupleLeaf<Indices, Types>(UsesAllocatorConstruction<Types, Alloc, Args>(), alloc, NEX_FORWARD<Args>(args))... {}

    // Constructor for TupleStorage that initializes the base classes (TupleLeaf instances) from another tuple.
    template <class OtherTuple>
    NEX_HIDDEN_FROM_ABI constexpr TupleStorage(from_tuple, OtherTuple&& tuple)
        noexcept(
            meta::AllTrueV<IsNothrowConstructibleV<
                Types,
                meta::CopyCvrefT<OtherTuple, TupleElementT<Indices, RemoveCvrefT<OtherTuple>>>
            >...>
        ) : TupleLeaf<Indices, Types>(
                NEX_FORWARD<meta::CopyCvrefT<OtherTuple, TupleElementT<Indices, RemoveCvrefT<OtherTuple>>>>(
                    get<Indices>(tuple)
                ))... {}

    // Constructor for TupleStorage that initializes the base classes (TupleLeaf instances)
    // from another tuple using an allocator.
    template <class Alloc, class OtherTuple>
    NEX_HIDDEN_FROM_ABI constexpr TupleStorage(allocator_arg_tag, const Alloc& alloc, from_tuple, OtherTuple&& tuple)
        : TupleLeaf<Indices, Types>(
                UsesAllocatorConstruction<
                    Types,
                    Alloc,
                    meta::CopyCvrefT<OtherTuple, TupleElementT<Indices, RemoveCvrefT<OtherTuple>>>
                >(),
                alloc,
                NEX_FORWARD<meta::CopyCvrefT<OtherTuple, TupleElementT<Indices, RemoveCvrefT<OtherTuple>>>>(
                        get<Indices>(tuple)
                )
          )... {}

    // Constructor for TupleStorage that initializes the base classes (TupleLeaf instances) with provided values.
    template <usize... Indices1, class... Types1, usize... Indices2, class... Types2, class... Types3>
    NEX_HIDDEN_FROM_ABI constexpr explicit
    TupleStorage(
        IndexSequence<Indices1...>,
        TupleTypes<Types1...>,
        IndexSequence<Indices2...>,
        TupleTypes<Types2...>,
        Types3&&... values
    ) noexcept(
        meta::AllTrueV<IsNothrowConstructibleV<Types1, Types3>...> &&
        meta::AllTrueV<IsNothrowDefaultConstructibleV<Types2>...>
    ) : TupleLeaf<Indices1, Types1>(NEX_FORWARD<Types3>(values))...,
        TupleLeaf<Indices2, Types2>()... {}

    NEX_HIDDEN_FROM_ABI constexpr TupleStorage(const TupleStorage&)     = default;
    NEX_HIDDEN_FROM_ABI constexpr TupleStorage(TupleStorage&&)          = default;

    // Swaps the values of two TupleStorage instances.
    NEX_HIDDEN_FROM_ABI constexpr
    void swap(TupleStorage& other) noexcept(meta::AllTrueV<IsNothrowSwappableV<Types>...>) {
        TupleHelper::swallowTypes(
            TupleLeaf<Indices, Types>::swap(static_cast<TupleLeaf<Indices, Types>&>(other))...);
    }

    // Swaps the values of two const TupleStorage instances.
    NEX_HIDDEN_FROM_ABI constexpr
    void swap(const TupleStorage& other) const noexcept(meta::AllTrueV<IsNothrowSwappableV<const Types>...>) {
        TupleHelper::swallowTypes(
            TupleLeaf<Indices, Types>::swap(static_cast<const TupleLeaf<Indices, Types>&>(other))...);
    }
};

/**
 * @class Tuple
 * @brief A Tuple class template that holds a fixed-size collection of elements of different types.
 * @tparam Types The types of the elements in the Tuple.
 */
template <class... Types>
class NEX_API Tuple {
    using base_storage = TupleStorage<IndexSequenceFor<sizeof...(Types)>, Types...>;
    base_storage base_;

    template <usize Index, class... Types>
    friend constexpr TupleElementT<Index, Tuple<Types...>>& get(Tuple<Types...>&) noexcept;

    template <usize Index, class... Types>
    friend constexpr const TupleElementT<Index, Tuple<Types...>>& get(const Tuple<Types...>&) noexcept;

    template <usize Index, class... Types>
    friend constexpr TupleElementT<Index, Tuple<Types...>>&& get(Tuple<Types...>&&) noexcept;

    template <usize Index, class... Types>
    friend constexpr const TupleElementT<Index, Tuple<Types...>>&& get(const Tuple<Types...>&&) noexcept;

public:
    // Default constructor for the Tuple class.
    template <
        template <class...> class IsImpDefault     = IsImplicitlyDefaultConstructible,
        template <class...> class IsDefault        = IsDefaultConstructible
    > requires (meta::AndV<IsDefault<Types>...>)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<meta::AndV<IsImpDefault<Types>...> >)
    Tuple() noexcept(meta::AndV<IsNothrowDefaultConstructible<Types>...>) {}

    // Default-constructs the Tuple using the provided allocator.
    template <
        class Alloc,
        template <class...> class IsImpDefault     = IsImplicitlyDefaultConstructible,
        template <class...> class IsDefault        = IsDefaultConstructible
    > requires (meta::AndV<IsDefault<Types>...>)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<meta::AndV<IsImpDefault<Types>...> >)
    Tuple(allocator_arg_tag, Alloc const& alloc)
        : base_(allocator_arg, alloc, value_init) {}

    // Constructs the Tuple from const references to all element types.
    template <template <class...> class And = meta::And>
        requires (And<BoolConstant<sizeof...(Types) >= 1>, IsCopyConstructible<Types>...>::value)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<And<IsConvertible<const Types&, Types>...>::value>)
    Tuple(const Types&... t) noexcept(And<IsNothrowCopyConstructible<Types>...>::value)
        : base_(forward_args, t...) {}

    // Constructs the Tuple from const references to all element types using the provided allocator.
    template <class Alloc, template <class...> class And = meta::And>
        requires (And<BoolConstant<sizeof...(Types) >= 1>, IsCopyConstructible<Types>...>::value)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<And<IsConvertible<const Types&, Types>...>::value>)
    Tuple(allocator_arg_tag, Alloc const& alloc, const Types&... t)
        : base_(allocator_arg, alloc, forward_args, t...) {}

private:
    template <class... OtherTypes>
    struct IsThisTuple : FalseType {};

    template <class OtherTuple>
    struct IsThisTuple<OtherTuple> : IsSame<RemoveCvrefT<OtherTuple>, Tuple> {};

    template <class... OtherTypes>
    struct EnableOtherTypesConstruction
        : meta::And<
            BoolConstant<sizeof...(Types) >= 1>,
            meta::Not<IsThisTuple<OtherTypes...> >,
            IsConstructible<Types, OtherTypes>...
        > {};

public:
    // Constructs the Tuple from arbitrary types for each element.
    template <class... OtherTypes>
        requires (meta::AndV<
            BoolConstant<sizeof...(Types) == sizeof...(OtherTypes)>,
            EnableOtherTypesConstruction<OtherTypes...> >)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<meta::AndV<IsConvertible<OtherTypes, Types>...> >)
    Tuple(OtherTypes&&... other) noexcept(meta::AndV<IsNothrowConstructible<Types, OtherTypes>...>)
        : base_(forward_args, NEX_FORWARD<OtherTypes>(other)...) {}

    // Constructs the Tuple from arbitrary types for each element with an allocator.
    template <class Alloc, class... OtherTypes>
        requires (meta::AndV<
            BoolConstant<sizeof...(Types) == sizeof...(OtherTypes)>,
            EnableOtherTypesConstruction<OtherTypes...> >)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<meta::AndV<IsConvertible<OtherTypes, Types>...> >)
    Tuple(allocator_arg_tag, Alloc const& alloc, OtherTypes&&... other)
        : base_(allocator_arg, alloc, forward_args, NEX_FORWARD<OtherTypes>(other)...) {}

    // Default copy and move constructors.
    NEX_HIDDEN_FROM_ABI constexpr Tuple(const Tuple&)   = default;
    NEX_HIDDEN_FROM_ABI constexpr Tuple(Tuple&&)        = default;

    // Constructs the Tuple by copying from another tuple using the provided allocator.
    template <class Alloc, template <class...> class And = meta::And>
        requires (And<IsCopyConstructible<Types>...>::value)
    NEX_HIDDEN_FROM_ABI constexpr Tuple(allocator_arg_tag, Alloc const& alloc, const Tuple& other)
        : base_(allocator_arg, alloc, from_tuple(), other) {}

    // Constructs the Tuple by moving from another tuple using the provided allocator.
    template <class Alloc, template <class...> class And = meta::And>
        requires (And<IsMoveConstructible<Types>...>::value)
    NEX_HIDDEN_FROM_ABI constexpr Tuple(allocator_arg_tag, Alloc const& alloc, Tuple&& other)
        : base_(allocator_arg, alloc, from_tuple(), NEX_MOVE(other)) {}

private:
    template <class OtherTuple, class DecayedOtherTuple = RemoveCvrefT<OtherTuple>, class = void>
    struct EnableConstructionFromOtherTypesTuple : FalseType {};

    template <class OtherTuple, class... OtherTypes>
    struct EnableConstructionFromOtherTypesTuple<
        OtherTuple,
        Tuple<OtherTypes...>,
        // the length of the packs needs to checked first otherwise the 2 packs cannot be expanded simultaneously below
        EnableIfT<sizeof...(OtherTypes) == sizeof...(Types)>
    > : meta::And<
            /**
             * @note The two conditions below are not in spec.
             * The purpose is to disable the OtherTypes Ctor when copy/move Ctor can work.
             * Otherwise, IsConstructible can trigger hard errors in those cases.
             * @see https://godbolt.org/z/M94cGdKcE
             */
            meta::Not<IsSame<OtherTuple, const Tuple&> >,
            meta::Not<IsSame<OtherTuple, Tuple&&> >,
            IsConstructible<Types, meta::CopyCvrefT<OtherTuple, OtherTypes> >...,
            meta::Or<
                BoolConstant<sizeof...(Types) != 1>,
                meta::And<
                    meta::Not<IsSame<Types, OtherTypes> >...,
                    meta::Not<IsConvertible<OtherTuple, Types> >...,
                    meta::Not<IsConstructible<Types, OtherTuple> >...
                >
            >
        > {};

public:
    // Copy constructor from another Tuple with potentially different types.
    template <class... OtherTypes>
        requires (meta::AndV<EnableConstructionFromOtherTypesTuple<const Tuple<OtherTypes...>&> >)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<meta::And<IsConvertible<const OtherTypes&, Types>...> >)
    Tuple(const Tuple<OtherTypes...>& other) noexcept(meta::AndV<IsNothrowConstructible<Types, const OtherTypes&>...>)
        : base_(from_tuple(), other) {}

    // Constructs the Tuple by copying from another tuple using the provided allocator.
    template <class Alloc, class... OtherTypes>
        requires (meta::AndV<EnableConstructionFromOtherTypesTuple<const Tuple<OtherTypes...>&> >)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<meta::And<IsConvertible<const OtherTypes&, Types>...> >)
    Tuple(allocator_arg_tag, Alloc const& alloc, const Tuple<OtherTypes...>& other)
        : base_(allocator_arg, alloc, from_tuple(), other) {}

    // Constructs the Tuple by moving from another tuple with potentially different types.
    template <class... OtherTypes>
        requires (meta::AndV<EnableConstructionFromOtherTypesTuple<Tuple<OtherTypes...>&&> >)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<meta::And<IsConvertible<OtherTypes, Types>...> >)
    Tuple(Tuple<OtherTypes...>&& other) noexcept(meta::AndV<IsNothrowConstructible<Types, OtherTypes>...>)
        : base_(from_tuple(), NEX_MOVE(other)) {}

    // Constructs the Tuple by moving from another tuple using the provided allocator.
    template <class Alloc, class... OtherTypes>
        requires (meta::AndV<EnableConstructionFromOtherTypesTuple<Tuple<OtherTypes...>&&> >)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<meta::And<IsConvertible<OtherTypes, Types>...> >)
    Tuple(allocator_arg_tag, Alloc const& alloc, Tuple<OtherTypes...>&& other)
        : base_(allocator_arg, alloc, from_tuple(), NEX_MOVE(other)) {}

private:
    template <
        template <class...> class Pred,
        class PairType,
        class DecayedPair   = RemoveCvrefT<PairType>,
        class TupleType     = Tuple>
    struct PredicateConstructionFromPair : FalseType {};

    template <
        template <class...> class Pred,
        class PairType,
        class First, class Second,
        class Type1, class Type2>
    struct PredicateConstructionFromPair<Pred, PairType, Pair<First, Second>, Tuple<Type1, Type2> >
        : meta::And<
            Pred<Type1, meta::CopyCvrefT<PairType, First> >,
            Pred<Type2, meta::CopyCvrefT<PairType, Second> >
        > {};

    template <class PairType>
    struct EnableConstructionFromPair : PredicateConstructionFromPair<IsConstructible, PairType> {};

    template <class PairType>
    struct IsNothrowConstructibleFromPair : PredicateConstructionFromPair<IsNothrowConstructible, PairType> {};

    template <
        class PairType,
        class DecayedPair = RemoveCvrefT<PairType>,
        class TupleType = Tuple>
    struct IsBothImplicitlyConvertible : FalseType {};

    template <
        class PairType,
        class First, class Second,
        class Type1, class Type2>
    struct IsBothImplicitlyConvertible<PairType, Pair<First, Second>, Tuple<Type1, Type2> >
        : meta::And<
            IsConvertible<meta::CopyCvrefT<PairType, First>, Type1>,
            IsConvertible<meta::CopyCvrefT<PairType, Second>, Type2>
        > {};

public:
    // Constructs the Tuple by copying values from a Pair.
    template <class First, class Second, template <class...> class And = meta::And>
        requires (And<EnableConstructionFromPair<const Pair<First, Second>&> >::value)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<IsBothImplicitlyConvertible<const Pair<First, Second>&>::value>)
    Tuple(const Pair<First, Second>& pair)
        noexcept(IsNothrowConstructibleFromPair<const Pair<First, Second>&>::value)
        : base_(from_tuple(), pair) {}

    // Constructs the Tuple by copying values from a Pair using an allocator.
    template <class Alloc, class First, class Second, template <class...> class And = meta::And>
        requires (And<EnableConstructionFromPair<const Pair<First, Second>&> >::value)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<IsBothImplicitlyConvertible<const Pair<First, Second>&>::value>)
    Tuple(allocator_arg_tag, Alloc const& alloc, const Pair<First, Second>& pair)
        : base_(allocator_arg, alloc, from_tuple(), pair) {}

    // Constructs the Tuple by moving values from a Pair.
    template <class First, class Second, template <class...> class And = meta::And>
        requires (And<EnableConstructionFromPair<Pair<First, Second>&&> >::value)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<IsBothImplicitlyConvertible<Pair<First, Second>&&>::value>)
    Tuple(Pair<First, Second>&& pair)
        noexcept(IsNothrowConstructibleFromPair<Pair<First, Second>&&>::value)
        : base_(from_tuple(), NEX_MOVE(pair)) {}

    // Constructs the Tuple by moving values from a Pair using an allocator.
    template <class Alloc, class First, class Second, template <class...> class And = meta::And>
        requires (And<EnableConstructionFromPair<Pair<First, Second>&&> >::value)
    NEX_HIDDEN_FROM_ABI constexpr explicit(meta::NotV<IsBothImplicitlyConvertible<Pair<First, Second>&&>::value>)
    Tuple(allocator_arg_tag, Alloc const& alloc, Pair<First, Second>&& pair)
        : base_(allocator_arg, alloc, from_tuple(), NEX_MOVE(pair)) {}

public:
    // Copy assignment operator from another Tuple with the same types.
    // This operator is only enabled if all elements are copy assignable.
    NEX_HIDDEN_FROM_ABI constexpr
    Tuple& operator=(meta::If<meta::AndV<IsCopyAssignable<Types>...>, Tuple, meta::NAT> const& other)
        noexcept(meta::AndV<IsNothrowCopyAssignable<Types>...>) {
        TupleHelper::copyAssign(*this, other, IndexSequenceFor<Types...>());
        return *this;
    }

    // Move assignment operator from another Tuple with the same types.
    // This operator is only enabled if all elements are move assignable.
    NEX_HIDDEN_FROM_ABI constexpr
    Tuple& operator=(meta::If<meta::AndV<IsMoveAssignable<Types>...>, Tuple, meta::NAT>&& other)
        noexcept(meta::AndV<IsNothrowMoveAssignable<Types>...>) {
        TupleHelper::forwardAssign(*this, NEX_MOVE(other), meta::TypeList<Types...>(), IndexSequenceFor<Types...>());
        return *this;
    }

    // Copy assignment operator from another Tuple with potentially different types.
    // This operator is only enabled if the other Tuple can be used to assign to this Tuple's elements.
    template <class... OtherTypes>
        requires (meta::AndV<
                BoolConstant<sizeof...(Types) == sizeof...(OtherTypes)>,
                IsAssignable<Types&, OtherTypes const&>...
            >)
    NEX_HIDDEN_FROM_ABI constexpr Tuple& operator=(Tuple<OtherTypes...> const& other)
        noexcept(meta::AndV<IsNothrowAssignable<Types&, OtherTypes const&>...>) {
        TupleHelper::copyAssign(*this, other, IndexSequenceFor<Types...>());
        return *this;
    }
    // Move assignment operator from another Tuple with potentially different types.
    // This operator is only enabled if the other Tuple can be used to assign to this Tuple's elements.
    template <class... OtherTypes>
        requires (meta::AndV<
                BoolConstant<sizeof...(Types) == sizeof...(OtherTypes)>,
                IsAssignable<Types&, OtherTypes>...
            >)
    NEX_HIDDEN_FROM_ABI constexpr Tuple& operator=(Tuple<OtherTypes...>&& other)
        noexcept(meta::AndV<IsNothrowAssignable<Types&, OtherTypes>...>) {
        TupleHelper::forwardAssign(*this, NEX_MOVE(other), meta::TypeList<OtherTypes...>(), IndexSequenceFor<Types...>());
        return *this;
    }

private:
    template <class Type, bool IsConst>
    using MaybeConst NEX_NODEBUG = ConditionalT<IsConst, const Type, Type>;

    template <
        template <class...> class Pred,
        bool IsConst,
        class PairType,
        class DecayedPair   = RemoveCvrefT<PairType>,
        class TupleType     = Tuple>
    struct PredicateAssignFromPair : FalseType {};

    template <
        template <class...> class Pred,
        bool IsConst,
        class PairType,
        class First, class Second,
        class Type1, class Type2>
    struct PredicateAssignFromPair<Pred, IsConst, PairType, Pair<First, Second>, Tuple<Type1, Type2> >
        : meta::And<
            Pred<MaybeConst<Type1, IsConst>&, meta::CopyCvrefT<PairType, First> >,
            Pred<MaybeConst<Type2, IsConst>&, meta::CopyCvrefT<PairType, Second> >
        > {};

    template <class PairType, bool IsConst = false>
    struct EnableAssignFromPair : PredicateAssignFromPair<IsAssignable, IsConst, PairType> {};

    template <class PairType, bool IsConst = false>
    struct IsNothrowAssignableFromPair : PredicateAssignFromPair<IsNothrowAssignable, IsConst, PairType> {};

public:
    // Copy assignment operator from a Pair.
    // The assignment is only enabled if the types are assignable from the Pair.
    template <class First, class Second>
        requires (EnableAssignFromPair<Pair<First, Second> const&>::value)
    NEX_HIDDEN_FROM_ABI constexpr Tuple& operator=(Pair<First, Second> const& pair)
        noexcept(IsNothrowAssignableFromPair<Pair<First, Second> const&>::value) {
        get<0>(*this) = pair.first;
        get<1>(*this) = pair.second;
        return *this;
    }

    // Move assignment operator from a Pair.
    // The assignment is only enabled if the types are assignable from the Pair.
    template <class First, class Second>
        requires (EnableAssignFromPair<Pair<First, Second>&&>::value)
    NEX_HIDDEN_FROM_ABI constexpr Tuple& operator=(Pair<First, Second>&& pair)
        noexcept(IsNothrowAssignableFromPair<Pair<First, Second>&&>::value) {
        get<0>(*this) = NEX_FORWARD<First>(pair.first);
        get<1>(*this) = NEX_FORWARD<Second>(pair.second);
        return *this;
    }

    // Copy assignment operator from an Array.
    // The assignment is only enabled if the types are assignable from the Array.
    template <class ElementType, usize Capacity>
        requires (meta::AndV<BoolConstant<Capacity == sizeof...(Types)>, IsAssignable<Types&, Type const&>...>)
    NEX_HIDDEN_FROM_ABI constexpr Tuple& operator=(Array<ElementType, Capacity> const& array)
        noexcept(meta::AndV<IsNothrowAssignable<Types&, ElementType const&>...>) {
        TupleHelper::copyAssign(*this, array, IndexSequenceFor<Types...>());
        return *this;
    }

    // Move assignment operator from an Array.
    // The assignment is only enabled if the types are assignable from the Array.
    template <class ElementType, usize Capacity, class = void>
        requires (meta::AndV<BoolConstant<Capacity == sizeof...(Types)>, IsAssignable<Types&, ElementType&&>...>)
    NEX_HIDDEN_FROM_ABI constexpr Tuple& operator=(Array<ElementType, Capacity>&& array)
        noexcept(meta::AndV<IsNothrowAssignable<Types&, ElementType&&>...>) {
        TupleHelper::forwardAssign(
            *this,
            NEX_MOVE(array),
            meta::TypeList<meta::If<true, ElementType, Types>...>(),
            IndexSequenceFor<Types...>()
        );
        return *this;
    }

    // Swaps the values of this Tuple with another Tuple.
    NEX_HIDDEN_FROM_ABI constexpr void swap(Tuple& other) noexcept(meta::AllTrueV<IsNothrowSwappableV<Types>...>) {
        base_.swap(other.base_);
    }
};

/**
 * @class Tuple<>
 * @brief Specialization of the Tuple class template for an empty tuple.
 */
template <>
class NEX_API Tuple<> {
public:
    NEX_HIDDEN_FROM_ABI constexpr Tuple() noexcept      = default;
    NEX_HIDDEN_FROM_ABI constexpr Tuple(const Tuple&)   = default;
    NEX_HIDDEN_FROM_ABI constexpr Tuple(Tuple&&)        = default;

    template <class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr Tuple(allocator_arg_tag, const Alloc&) noexcept {}

    template <class Alloc>
    NEX_HIDDEN_FROM_ABI constexpr Tuple(allocator_arg_tag, const Alloc&, const Tuple&) noexcept {}

    template <class OtherType>
    NEX_HIDDEN_FROM_ABI constexpr Tuple(Array<OtherType, 0>) noexcept {}

    template <class Alloc, class OtherType>
    NEX_HIDDEN_FROM_ABI constexpr Tuple(allocator_arg_tag, const Alloc&, Array<OtherType, 0>) noexcept {}

    NEX_HIDDEN_FROM_ABI constexpr void swap(Tuple&) noexcept {}
};

// Deduction guide for variadic types.
template <class... Types>
Tuple(Types...) -> Tuple<Types...>;

// Deduction guide for Pair to Tuple conversion.
template <class First, class Second>
Tuple(Pair<First, Second>) -> Tuple<First, Second>;

// Deduction guide for allocator-aware construction with variadic types.
template <class Alloc, class... Types>
Tuple(allocator_arg_tag, const Alloc&, Types...) -> Tuple<Types...>;

// Deduction guide for allocator-aware construction with Pair.
template <class Alloc, class First, class Second>
Tuple(allocator_arg_tag, const Alloc&, Pair<First, Second>) -> Tuple<First, Second>;

// Deduction guide for allocator-aware construction with Tuple.
template <class Alloc, class... Types>
Tuple(allocator_arg_tag, const Alloc&, Tuple<Types...>) -> Tuple<Types...>;

/**
 * @brief Swaps the values of two Tuples.
 * @tparam Types The types of the elements in the Tuples.
 * @param lhs The first Tuple to swap.
 * @param rhs The second Tuple to swap.
 */
template <class... Types>
    requires (meta::AllTrueV<IsSwappableV<Types>...>)
NEX_HIDDEN_FROM_ABI inline constexpr
void swap(Tuple<Types...>& lhs, Tuple<Types...>& rhs) noexcept(meta::AllTrueV<IsNothrowSwappableV<Types>...>) {
    lhs.swap(rhs);
}

/**
 * @brief Retrieves a reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
TupleElementT<Index, Tuple<Types...>>& get(Tuple<Types...>& tuple) noexcept {
    using type = TupleElementT<Index, Tuple<Types...>>;
    return static_cast<TupleLeaf<Index, type>&>(tuple.base_).get();
}

/**
 * @brief Retrieves a const reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A const reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
const TupleElementT<Index, Tuple<Types...>>& get(const Tuple<Types...>& tuple) noexcept {
    using type = TupleElementT<Index, Tuple<Types...>>;
    return static_cast<const TupleLeaf<Index, type>&>(tuple.base_).get();
}

/**
 * @brief Retrieves an rvalue reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return An rvalue reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
TupleElementT<Index, Tuple<Types...>>&& get(Tuple<Types...>&& tuple) noexcept {
    using type = TupleElementT<Index, Tuple<Types...>>;
    return static_cast<TupleLeaf<Index, type>&&>(tuple.base_).get();
}

/**
 * @brief Retrieves a const rvalue reference to the element at the specified index in a Tuple.
 * @tparam Index The index of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A const rvalue reference to the element at the specified index.
 */
template <usize Index, class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
const TupleElementT<Index, Tuple<Types...>>&& get(const Tuple<Types...>&& tuple) noexcept {
    using type = TupleElementT<Index, Tuple<Types...>>;
    return static_cast<const TupleLeaf<Index, type>&&>(tuple.base_).get();
}

/**
 * @brief Retrieves a reference to the element of the specified type in a Tuple.
 * @tparam Type1 The type of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A reference to the element of the specified type.
 */
template <class Type1, class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type1& get(Tuple<Types...>& tuple) noexcept {
    return get<TupleHelper::Find::FindExactlyOne<Type1, Types...>::value>(tuple);
}

/**
 * @brief Retrieves a const reference to the element of the specified type in a Tuple.
 * @tparam Type1 The type of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A const reference to the element of the specified type.
 */
template <class Type1, class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const Type1& get(const Tuple<Types...>& tuple) noexcept {
    return get<TupleHelper::Find::FindExactlyOne<Type1, Types...>::value>(tuple);
}

/**
 * @brief Retrieves an rvalue reference to the element of the specified type in a Tuple.
 * @tparam Type1 The type of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return An rvalue reference to the element of the specified type.
 */
template <class Type1, class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Type1&& get(Tuple<Types...>&& tuple) noexcept {
    return get<TupleHelper::Find::FindExactlyOne<Type1, Types...>::value>(NEX_MOVE(tuple));
}

/**
 * @brief Retrieves a const rvalue reference to the element of the specified type in a Tuple.
 * @tparam Type1 The type of the element to retrieve.
 * @tparam Types The types of the elements in the Tuple.
 * @param tuple The Tuple from which to retrieve the element.
 * @return A const rvalue reference to the element of the specified type.
 */
template <class Type1, class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const Type1&& get(const Tuple<Types...>&& tuple) noexcept {
    return get<TupleHelper::Find::FindExactlyOne<Type1, Types...>::value>(NEX_MOVE(tuple));
}

/**
 * @brief Creates a Tuple of references to the provided arguments.
 * @tparam Types The types of the arguments.
 * @param args The arguments to tie into a Tuple.
 * @return A Tuple of references to the provided arguments.
 */
template <class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Tuple<Types&...> tie(Types&... args)
    noexcept (meta::AllTrueV<IsNothrowConstructibleV<Types&, Types&>...>) {
    return Tuple<Types&...>(args...);
}

/**
 * @brief Creates a Tuple by decaying and unwrapping the provided arguments.
 * @tparam Types The types of the arguments.
 * @param args The arguments to include in the Tuple.
 * @return A Tuple containing the provided arguments.
 */
template <class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto makeTuple(Types&&... args)
    noexcept (meta::AllTrueV<IsNothrowConstructibleV<UnwrapRefDecayT<Types>, Types&&>...>)
    -> Tuple<UnwrapRefDecayT<Types>...> {
    return Tuple<UnwrapRefDecayT<Types>...>(NEX_FORWARD<Types>(args)...);
}

/**
 * @brief Creates a Tuple by forwarding the provided arguments as rvalue references.
 * @tparam Types The types of the arguments.
 * @param args The arguments to include in the Tuple.
 * @return A Tuple containing the provided arguments as rvalue references.
 */
template <class... Types>
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Tuple<Types&&...> forwardAsTuple(Types&&... args)
    noexcept (meta::AllTrueV<IsNothrowConstructibleV<Types&&, Types&&>...>) {
    return Tuple<Types&&...>(NEX_FORWARD<Types>(args)...);
}

/**
 * @brief Compares two Tuples for equality.
 * @tparam Types1 The types of the elements in the first Tuple.
 * @tparam Types2 The types of the elements in the second Tuple.
 * @param lhs The first Tuple to compare.
 * @param rhs The second Tuple to compare.
 * @return true if the Tuples are equal, false otherwise.
 */
template <class... Types1, class... Types2>
    requires (sizeof...(Types1) == sizeof...(Types2))
NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
bool operator==(const Tuple<Types1...>& lhs, const Tuple<Types2...>& rhs) {
    return TupleHelper::compareEqual<sizeof...(Types1)>(lhs, rhs);
}

/**
 * @brief Indicates that a Tuple uses an allocator.
 * @tparam Types The types of the elements in the Tuple.
 * @tparam Alloc The allocator type.
 */
template <class... Types, class Alloc>
struct UsesAllocator<Tuple<Types...>, Alloc> : TrueType {};

NEX_NAMESPACE_END
