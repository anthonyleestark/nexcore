/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/iterator.h"

NEX_NAMESPACE_BEGIN

// Represents a dynamic extent for Span
inline constexpr usize dynamicExtent = static_cast<usize>(-1);

// Forward declaration
template <meta::ObjectType, usize = dynamicExtent>
class Span;

// Determines whether a type is a Span
template <typename Type, usize = dynamicExtent>
struct IsSpan : meta::FalseType {};

// Determines whether a type is a Span
template <meta::ObjectType Type, usize Extent>
struct IsSpan<Span<Type, Extent>> : meta::TrueType {};

// Determines whether a type is a Span
template <typename Type>
inline constexpr bool IsSpanV = IsSpan<Type>::value;

// Determine whether an array type is convertible to a Span of a specific element type
template <class From, class To>
concept SpanArrayConvertible = meta::IsConvertibleV<From (*)[], To (*)[]>;

/**
 * @struct SpanLayoutBase
 * @brief Base layout for Span, providing common members and types.
 */
template <meta::ObjectType Type, usize Extent>
struct NEX_INTERNAL SpanLayoutBase {
protected:
    using pointer = Type*;
    using size_type = usize;
    pointer data_ = nullptr;                                        // Pointer to the underlying data
    constexpr size_type size() const noexcept { return Extent; }    // Size of the span (static extent)
};

/**
 * @struct SpanLayoutBase<Type, dynamicExtent>
 * @brief Specialization of SpanLayoutBase for dynamic extent, providing common members and types.
 */
template <meta::ObjectType Type>
struct NEX_INTERNAL SpanLayoutBase<Type, dynamicExtent> {
protected:
    using pointer = Type*;
    using size_type = usize;
    pointer data_ = nullptr;                                        // Pointer to the underlying data
    size_type size_ = 0;                                            // Size of the span (dynamic extent)
    constexpr size_type size() const noexcept { return size_; }     // Returns the size of the span
};

/**
 * @class Span
 * @brief A lightweight, non-owning view over a contiguous sequence of objects.
 * 
 * @details
 * The Span class provides a way to represent a contiguous sequence of objects in memory without
 * owning the underlying data. It can be used to safely and efficiently access arrays, vectors,
 * or other contiguous containers. The span can have either a static extent (known at compile time)
 * or a dynamic extent (determined at runtime).
 * 
 * @tparam Type The type of the elements in the span.
 * @tparam Extent The number of elements in the span. Use dynamicExtent for a dynamic size.
 */
template <
    meta::ObjectType Type,
    usize Extent = dynamicExtent
> 
class NEX_API Span : private SpanLayoutBase<Type, Extent> {
    using base = SpanLayoutBase<Type, Extent>;
public:
    using element_type = Type;
    using value_type = meta::RemoveCvT<Type>;
    using size_type = typename base::size_type;
    using difference_type = isize;
    using pointer = typename base::pointer;
    using const_pointer = const element_type*;
    using reference = element_type&;
    using const_reference = const element_type&;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;

    // Constant representing the extent of the span
    static constexpr size_type extent = Extent;

    // Default copy semantics
    NEX_DEFAULT_COPY(Span);

    // Default constructor that initializes a null static-extent Span with size 0.
    template <size_type Size = extent>
        requires (extent != dynamicExtent && Size == 0)
    NEX_HIDDEN_FROM_ABI constexpr Span() noexcept = default;

    // Constructs a static extent Span from a pointer and size, ensuring the size matches the extent.
    template <size_type Size = extent>
        requires (extent != dynamicExtent && Size > 0)
    NEX_HIDDEN_FROM_ABI constexpr Span(pointer data) noexcept : base::data_{data} {}

    // Constructs a static extent Span from a static bounded array of the specified extent.
    template <size_type Size = extent>
        requires (extent != dynamicExtent && Size > 0)
    NEX_HIDDEN_FROM_ABI constexpr Span(meta::TypeIdentityT<element_type> (&array)[extent]) noexcept
        : base::data_{array} {}

    // Constructs a dynamic extent Span from a pointer and size.
    template <size_type Size = extent>
        requires (extent == dynamicExtent)
    NEX_HIDDEN_FROM_ABI constexpr Span(pointer data, size_type size) noexcept
        : base::data_{data}, base::size_{size} {}

    // Constructs a dynamic extent Span from a static bounded array, capturing its data and size.
    template <size_type Size>
        requires (extent == dynamicExtent && Size > 0)
    NEX_HIDDEN_FROM_ABI constexpr Span(meta::TypeIdentityT<element_type> (&array)[Size]) noexcept
        : base::data_{array}, base::size_{Size} {}

    // Copy constructor that allows constructing a static extent Span from another static extent Span
    // with a potentially different element type, as long as the array types are convertible.
    template <SpanArrayConvertible<element_type> OtherElementType>
        requires (extent != dynamicExtent)
    NEX_HIDDEN_FROM_ABI constexpr Span(const Span<OtherElementType, extent>& other) noexcept
        : base::data_{other.data()} {}

    // Copy constructor that allows constructing a dynamic extent Span from another dynamic extent Span
    // with a potentially different element type, as long as the array types are convertible.
    template <SpanArrayConvertible<element_type> OtherElementType>
        requires (extent != dynamicExtent)
    NEX_HIDDEN_FROM_ABI constexpr Span(const Span<OtherElementType, dynamicExtent>& other) noexcept
        : base::data_{other.data()}, base::size_{other.size()} {
        NEX_ASSERT_VALID_ELEMENT_ACCESS(extent == other.size(), "Error: size mismatch in Span's constructor (other span)");
    }

    // Copy constructor that allows constructing a dynamic extent Span from another Span (either static or dynamic extent)
    // with a potentially different element type and extent, as long as the array types are convertible.
    template <SpanArrayConvertible<element_type> OtherElementType, usize OtherExtent>
        requires (extent == dynamicExtent)
    NEX_HIDDEN_FROM_ABI constexpr Span(const Span<OtherElementType, OtherExtent>& other) noexcept
        : base::data_{other.data()}, base::size_{other.size()} {}

    // Retrieves the first Count elements of the span as a new Span, with Count being a compile-time constant.
    template <size_type Count>
        requires (extent != dynamicExtent)
    NEX_HIDDEN_FROM_ABI constexpr Span<element_type, Count> first() const noexcept {
        static_assert(Count <= extent, "Span<T, N>::first<Count>(): Count out of range");
        return Span<element_type, Count>{data()};
    }

    // Retrieves the last Count elements of the span as a new Span, with Count being a compile-time constant.
    template <size_type Count>
        requires (extent != dynamicExtent)
    NEX_HIDDEN_FROM_ABI constexpr Span<element_type, Count> last() const noexcept {
        static_assert(Count <= extent, "Span<T, N>::last<Count>(): Count out of range");
        return Span<element_type, Count>{data() + size() - Count};
    }

    // Retrieves the first count elements of the span as a new dynamic extent Span, with count being a runtime value.
    NEX_HIDDEN_FROM_ABI constexpr Span<element_type, dynamicExtent> first(size_type count) const noexcept {
        NEX_ASSERT_VALID_ELEMENT_ACCESS(count <= size(), "Span<T, N>::first(count): count out of range");
        return {data(), count};
    }

    // Retrieves the last count elements of the span as a new dynamic extent Span, with count being a runtime value.
    NEX_HIDDEN_FROM_ABI constexpr Span<element_type, dynamicExtent> last(size_type count) const noexcept {
        NEX_ASSERT_VALID_ELEMENT_ACCESS(count <= size(), "Span<T, N>::last(count): count out of range");
        return {data() + size() - count, count};
    }

    // Retrieves a subspan starting at the specified offset and with the specified count,
    // with both offset and count being compile-time constants.
    template <size_type Offset, size_type Count = dynamicExtent>
        requires (extent != dynamicExtent)
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr auto
    subspan() const noexcept -> Span<element_type, Count != dynamicExtent ? Count : extent - Offset> {
        static_assert(Offset <= extent, "Span<T, N>::subspan<Offset, Count>(): Offset out of range");
        static_assert(Count == dynamicExtent || Count <= extent - Offset,
                    "Span<T, N>::subspan<Offset, Count>(): Offset + Count out of range");

        using ReturnType = Span<element_type, Count != dynamicExtent ? Count : extent - Offset>;
        return ReturnType{data() + Offset};
    }

    // Retrieves a subspan starting at the specified offset and with the specified count,
    // with both offset and count being runtime values.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr Span<element_type, dynamicExtent>
    subspan(size_type offset, size_type count = dynamicExtent) const noexcept {
        NEX_ASSERT_VALID_ELEMENT_ACCESS(offset <= size(),
            "Span<T, N>::subspan(offset, count): offset out of range");

        if (count == dynamicExtent)
            return {data() + offset, size() - offset};

        NEX_ASSERT_VALID_ELEMENT_ACCESS(count <= size() - offset,
            "Span<T, N>::subspan(offset, count): offset + count out of range");

        return {data() + offset, count};
    }

    // Retrieves the number of elements in the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    size_type size() const noexcept { return base::size(); }

    // Retrieves the size of the span in bytes.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    size_type sizeBytes() const noexcept { return size() * sizeof(element_type); }

    // Retrieves the length of the span (same as size).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    size_type length() const noexcept { return size(); }

    // Determines whether the span is empty (size is zero).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    bool empty() const noexcept { return size() == 0; }

    // Accesses the underlying data pointer of the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    pointer data() const noexcept { return base::data_; }

    // Access the first element of the span (mutable).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr reference front() noexcept {
        NEX_ASSERT_MSG(!empty(), "Span<T, N>::front() on empty span.");
        return *data();
    }

    // Access the first element of the span (immutable).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference front() const noexcept {
        NEX_ASSERT_MSG(!empty(), "Span<T, N>::front() on empty span.");
        return *data();
    }

    // Access the last element of the span (mutable).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr reference back() noexcept {
        NEX_ASSERT_MSG(!empty(), "Span<T, N>::back() on empty span.");
        return *(data() + size() - 1);
    }

    // Access the last element of the span (immutable).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference back() const noexcept {
        NEX_ASSERT_MSG(!empty(), "Span<T, N>::back() on empty span.");
        return *(data() + size() - 1);
    }

    // Accesses the element at the specified index with bounds checking (mutable).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr reference at(size_type index) {
        NEX_ASSERT_MSG(index < size(), "Span<T, N>::at(index): index out of range.");
        return base::data_[index];
    }

    // Accesses the element at the specified index with bounds checking (immutable).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr const_reference at(size_type index) const {
        NEX_ASSERT_MSG(index < size(), "Span<T, N>::at(index): index out of range.");
        return base::data_[index];
    }

    // Accesses the element at the specified index without bounds checking (mutable).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    reference operator[](size_type index) { return base::data_[index]; }

    // Accesses the element at the specified index without bounds checking (immutable).
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    const_reference operator[](size_type index) const { return base::data_[index]; }

    // Retrieves an iterator to the beginning of the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    iterator begin() const noexcept { return data(); }

    // Retrieves a constant iterator to the beginning of the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    const_iterator cbegin() const noexcept { return data(); }

    // Retrieves an iterator to the end of the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    iterator end() const noexcept { return data() + size(); }

    // Retrieves a constant iterator to the end of the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    const_iterator cend() const noexcept { return data() + size(); }

    // Retrieves a reverse iterator to the beginning of the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }

    // Retrieves a reverse iterator to the end of the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }

    // Retrieves a constant reverse iterator to the beginning of the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

    // Retrieves a constant reverse iterator to the end of the span.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI constexpr
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

    // Creates a static extent Span of bytes from the current span,
    // allowing for byte-level access to the underlying data.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI
    Span<const byte, extent * sizeof(element_type)> asBytes() const noexcept
        requires (extent != dynamicExtent) {
        return Span<const byte, extent * sizeof(element_type)>{reinterpret_cast<const byte*>(data())};
    }

    // Creates a writable static extent Span of bytes from the current span,
    // allowing for byte-level access to the underlying data.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI
    Span<byte, extent * sizeof(element_type)> asWritableBytes() const noexcept
        requires (extent != dynamicExtent && !meta::IsConstV<element_type>) {
        return Span<byte, extent * sizeof(element_type)>{reinterpret_cast<byte*>(data())};
    }

    // Creates a  dynamic extent Span of bytes from the current span,
    // allowing for byte-level access to the underlying data.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI
    Span<const byte, dynamicExtent> asBytes() const noexcept
        requires (extent == dynamicExtent) {
        return {reinterpret_cast<const byte*>(data()), sizeBytes()};
    }

    // Creates a writable dynamic extent Span of bytes from the current span,
    // allowing for byte-level access to the underlying data.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI
    Span<byte, dynamicExtent> asWritableBytes() const noexcept
        requires (extent == dynamicExtent && !meta::IsConstV<element_type>) {
        return {reinterpret_cast<byte*>(data()), sizeBytes()};
    }
};

// Type deduction guide for Span, allowing for automatic deduction of the element type and extent
// when constructing a Span from a static bounded array.
template <meta::ObjectType Type, usize Size>
Span(Type (&)[Size]) -> Span<Type, Size>;

// Type deduction guide for Span, allowing for automatic deduction of the element type and dynamic extent
// when constructing a Span from a static bounded array of a potentially different element type.
template <meta::ObjectType Type, usize Size>
Span(const Type (&)[Size]) -> Span<Type, Size>;

// Type deduction guide for Span, allowing for automatic deduction of the element type and dynamic extent
// when constructing a Span from a pointer and size.
template <meta::ObjectType Type>
Span(Type*, usize) -> Span<Type, dynamicExtent>;

// Type deduction guide for Span, allowing for automatic deduction of the element type and dynamic extent
// when constructing a Span from another Span with a potentially different element type and extent.
template <meta::ObjectType Type, usize Extent>
Span(const Span<Type, Extent>&) -> Span<Type, dynamicExtent>;

// Type deduction guide for Span, allowing for automatic deduction of the element type and dynamic extent
// when constructing a Span from another Span with a potentially different element type and extent.
template <meta::ObjectType Type, usize Extent>
Span(Span<Type, Extent>&) -> Span<Type, dynamicExtent>;

// Type deduction guide for Span, allowing for automatic deduction of the element type and dynamic extent
// when constructing a Span from another Span with a potentially different element type and extent.
template <meta::ObjectType Type, usize Extent>
Span(Span<Type, Extent>&&) -> Span<Type, dynamicExtent>;

// Type deduction guide for Span, allowing for automatic deduction of the element type and dynamic extent
// when constructing a Span from another Span with a potentially different element type and extent.
template <meta::ObjectType Type, usize Extent>
Span(const Span<Type, Extent>&&) -> Span<Type, dynamicExtent>;

// =================================================================================
// Common span type aliases for convenience
// =================================================================================

using ByteSpan = Span<const byte, dynamicExtent>;        // Represents a span of bytes (immutable)
using WritableByteSpan = Span<byte, dynamicExtent>;      // Represents a span of bytes (mutable)
using ConstByteSpan = Span<const byte, dynamicExtent>;   // Represents a span of bytes (immutable, const)
using IndexSpan = Span<usize, dynamicExtent>;            // Represents a span of indices (immutable)
using SignedSpan = Span<isize, dynamicExtent>;           // Represents a span of signed offsets or indexes (immutable)

template <typename Type>
using MutableSpan = Span<Type, dynamicExtent>;           // Represents a mutable span of a specific type

template <typename Type>
using ImmutableSpan = Span<const Type, dynamicExtent>;   // Represents an immutable span of a specific type

NEX_NAMESPACE_END
