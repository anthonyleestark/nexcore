/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/platform.h"
#include "nex/base/architecture.h"
#include "nex/base/compiler.h"
#include "nex/base/abi.h"
#include "nex/base/runtime.h"
#include "nex/base/compiler_features.h"

#include "nex/base/attributes.h"
#include "nex/base/visibility.h"
#include "nex/base/export.h"

#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/assert_crash.h"

#if defined(NEX_RUNTIME_VCRUNTIME)
    #include <vcruntime_typeinfo.h>
#endif

// TODO: Add support for Microsoft C++ ABI later.
#if defined(NEX_ABI_CXX_MICROSOFT)
    #include <typeinfo>
#endif

NEX_NAMESPACE_BEGIN

#if defined(NEX_ABI_CXX_MICROSOFT)

/**
 * FIXME: Here we temporarily alias the Microsoft C++ ABI type_info as TypeInfo.
 * This is a temporary measure until proper support for the Microsoft C++ ABI is added.
 */

using TypeInfo = NEX_STD type_info;

#else  // For other ABIs

NEX_SUBNAMESPACE_BEGIN(type_info_impl)

struct StringImplBase {
    using type_name = ncstring;

    // Converts the type name to a string representation.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr static
    ncstring typeNameToString(type_name value) noexcept {
        return value;
    }

    // Converts a string representation back to the type name.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE constexpr static
    type_name stringToTypeName(ncstring value) noexcept {
        return value;
    }
};

/**
 * @struct UniqueImpl
 *
 * @details
 * This implementation of TypeInfo assumes a unique copy of the RTTI for a given type
 * inside a program. This is a valid assumption when abiding to the Itanium ABI.
 * Under this assumption, we can always compare the addresses of the type names to implement
 * equality-comparison of TypeInfos instead of having to perform a deep string comparison.
 *
 * @see http://itanium-cxx-abi.github.io/cxx-abi/abi.html#vtable-components
 */
struct UniqueImpl : StringImplBase {
    // Computes the hash of the type name.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static usize hash(type_name value) noexcept {
        return reinterpret_cast<usize>(value);
    }

    // Compares two type names for equality.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static bool equals(type_name lhs, type_name rhs) noexcept {
        return lhs == rhs;
    }

    // Compares two type names to determine if the first is less than the second.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static bool lessThan(type_name lhs, type_name rhs) noexcept {
        return lhs < rhs;
    }
};


/**
 * @struct NonUniqueImpl
 * @details
 * This implementation of TypeInfo does not assume there is always a unique copy of the RTTI
 * for a given type inside a program. For various reasons the linker may have failed to merge
 * every copy of a types RTTI (For example: -Bsymbolic or llvm.org/PR37398).
 * Under this assumption, two TypeInfos are equal if their addresses are equal or if a deep
 * string comparison is equal.
 */
struct NonUniqueImpl : StringImplBase {
    // Computes the hash of the type name.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static usize hash(type_name ptr) noexcept {
        // Compute the hash using the DJB2a algorithm (XOR variant of DJB2).
        usize hash = 5381;   // Magic prime constant
        while (uchar ch = static_cast<uchar>(*ptr++)) {
            hash = (hash * 33) ^ ch;
        }
        return hash;
    }

    // Compares two type names for equality.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static bool equals(type_name lhs, type_name rhs) noexcept {
        return lhs == rhs || __builtin_strcmp(lhs, rhs) == 0;
    }

    // Compares two type names to determine if the first is less than the second.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static bool lessThan(type_name lhs, type_name rhs) noexcept {
        return __builtin_strcmp(lhs, rhs) < 0;
    }
};

/**
 * @struct NonUniqueArmRttiBitImpl
 * @note This implementation is specific to ARM64 on Apple platforms.
 *
 * @details
 * This implementation of type_info does not assume always a unique copy of the RTTI for a given type
 * inside a program. When constructing the TypeInfo, the compiler packs the pointer to the type name
 * into a `uintptr` and reserves the high bit of that pointer, which is assumed to be free for use
 * under that ABI. If that high bit is set, that specific copy of the RTTI can't be assumed to be unique
 * within the program. If the high bit is unset, then the RTTI can be assumed to be unique within the program.
 *
 * When comparing TypeInfos, if both RTTIs can be assumed to be unique, it suffices to compare their addresses.
 * If both the RTTIs can't be assumed to be unique, we must perform a deep string comparison of the type names.
 * However, if one of the RTTIs is guaranteed unique and the other one isn't, then both RTTIs are necessarily
 * not to be considered equal.
 *
 * The intent of this design is to remove the need for weak symbols. Specifically, if a type would normally
 * have a default-visibility RTTI emitted as a weak symbol, it is given hidden visibility instead and
 * the non-unique bit is set. Otherwise, types declared with hidden visibility are always considered
 * to have a unique RTTI: the RTTI is emitted with linkonce_odr linkage and is assumed to be deduplicated
 * by the linker within the linked image. Across linked image boundaries, such types are thus considered
 * different types.
 */
struct NonUniqueArmRttiBitImpl {
    using type_name = uintptr;

    // Converts the type name to a string representation.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static ncstring typeNameToString(type_name value) noexcept {
        return reinterpret_cast<ncstring>(value & ~NonUniqueRttiBit::value);
    }

    // Converts a string representation back to the type name.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static type_name stringToTypeName(ncstring value) noexcept {
        return reinterpret_cast<type_name>(value);
    }

    // Computes the hash of the type name.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static usize hash(type_name value) noexcept {
        if (isTypeNameUnique(value)) {
            return value;
        }
        return NonUniqueImpl::hash(typeNameToString(value));
    }

    // Compares two type names for equality.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static bool equals(type_name lhs, type_name rhs) noexcept {
        if (lhs == rhs) {
            return true;
        }
        if (isTypeNameUnique(lhs) || isTypeNameUnique(rhs)) {
            // Either both are unique and have a different address, or one of them
            // is unique and the other one isn't. In both cases they are unequal.
            return false;
        }
        return __builtin_strcmp(typeNameToString(lhs), typeNameToString(rhs)) == 0;
    }

    // Compares two type names to determine if the first is less than the second.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static bool lessThan(type_name lhs, type_name rhs) noexcept {
        if (isTypeNameUnique(lhs) && isTypeNameUnique(rhs)) {
            return lhs < rhs;
        }
        return __builtin_strcmp(typeNameToString(lhs), typeNameToString(rhs)) < 0;
    }

private:
    // The unique bit is the top bit.
    // It is expected that type_name is 64 bits when this implementation is actually used.
    using NonUniqueRttiBit NEX_NODEBUG =
        meta::IntegralConstant<type_name, (1ULL << ((NEX_CHAR_BIT * sizeof(type_name)) - 1))>;

    // Checks if the specified type name is unique.
    NEX_HIDDEN_FROM_ABI NEX_ALWAYS_INLINE static bool isTypeNameUnique(type_name lhs) noexcept {
        return !(lhs & NonUniqueRttiBit::value);
    }
};

// Windows and AIX binaries can't merge typeinfos, so use the NonUnique implementation.
#if NEX_PLATFORM_IS_WINDOWS || defined(NEX_PLATFORM_AIX)
    using impl = NonUniqueImpl;

// On arm64 on Apple platforms, use the special NonUniqueARMRTTIBit implementation.
#elif NEX_PLATFORM_FAMILY_IS_APPLE && defined(NEX_DATA_MODEL_LP64) && !defined(NEX_ARCH_CPU_X86_64)
    using impl = NonUniqueArmRttiBitImpl;

// On all other platforms, assume the Itanium C++ ABI and use the Unique implementation.
#else
    using impl = UniqueImpl;
#endif

NEX_SUBNAMESPACE_END(type_info_impl)

#if NEX_HAS_FEATURE(ptrauth_type_info_vtable_pointer_discrimination)
    #define NEX_TYPE_INFO_VTABLE_POINTER_AUTH \
        NEX_NO_PTRAUTH_VTABLE_POINTER(process_independent, address_discrimination, type_discrimination)
#else
    #define NEX_TYPE_INFO_VTABLE_POINTER_AUTH \
        NEX_NO_PTRAUTH_VTABLE_POINTER(process_independent, no_address_discrimination, no_extra_discrimination)
#endif

/**
 * @class TypeInfo
 * @brief Represents type information for a C++ type.
 * 
 * @details
 * This class encapsulates type information for a C++ type, providing functionality to retrieve the type's name,
 * compare type information, and compute hash codes for type information objects.
 */
class NEX_EXPORTED_FROM_ABI NEX_TYPE_INFO_VTABLE_POINTER_AUTH TypeInfo {
    TypeInfo& operator=(const TypeInfo&);
    TypeInfo(const TypeInfo&);

private:
    using Impl = type_info_impl::impl;
    Impl::type_name typeName;

    // Private constructor that initializes the type info with the specified type name.
    NEX_HIDDEN_FROM_ABI explicit TypeInfo(ncstring name)
        : typeName(Impl::stringToTypeName(name)) {}

public:
    virtual ~TypeInfo();

    // Returns the name of the type as a null-terminated string.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI ncstring name() const noexcept {
        return Impl::typeNameToString(typeName);
    }

    // Returns true if this type info precedes the specified type info in the implementation-defined order.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI bool before(const TypeInfo& other) const noexcept {
        return Impl::lessThan(typeName, other.typeName);
    }

    // Returns the hash code of the type info.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI usize hashCode() const noexcept {
        return Impl::hash(typeName);
    }

    // Compares this type info with the specified type info for equality.
    NEX_HIDDEN_FROM_ABI NEX_CONSTEXPR23 bool operator==(const TypeInfo& other) const noexcept {
        // When evaluated in a constant expression, both type infos simply can't come
        // from different translation units, so it is sufficient to compare their addresses.
        if (meta::IsConstantEvaluated()) {
            return this == &other;
        }
        return Impl::equals(typeName, other.typeName);
    }
}; 

#endif

NEX_NAMESPACE_END
