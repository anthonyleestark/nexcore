/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/fwd.h"
#include "nex/base/invoke.h"
#include "nex/base/memory.h"
#include "nex/base/type_concepts.h"
#include "nex/base/type_traits.h"

NEX_NAMESPACE_BEGIN

NEX_SUBNAMESPACE_BEGIN(hashing_impl)

/**
 * @brief One-shot implementation of the 32-bit XXHash algorithm.
 *
 * XXHash32 maintains all arithmetic modulo 2^32 and reads input words in
 * little-endian order. The constants @c prime1 through @c prime5 are the
 * algorithm's fixed odd primes; multiplication by them is invertible modulo
 * 2^32 and, combined with rotations, rapidly diffuses input bits.
 *
 * For inputs of 16 bytes or more, four accumulators are initialized as
 * @code
 * v1 = seed + prime1 + prime2;  v2 = seed + prime2;
 * v3 = seed;                    v4 = seed - prime1;
 * @endcode
 * Each 16-byte stripe supplies one 32-bit word to each accumulator. A round
 * transforms an accumulator @c v and input word @c w as
 * @code
 * v = rotl(v + w * prime2, 13) * prime1;
 * @endcode
 * The four resulting lanes are merged by rotating them by 1, 7, 12, and 18
 * bits respectively, then adding them. Shorter inputs instead begin from
 * @c seed + prime5. In both cases, the original byte length is added next.
 *
 * The tail is consumed in descending unit sizes. Each remaining 32-bit word
 * is folded with @c hash = rotl(hash + word * prime3, 17) * prime4. Each
 * remaining byte is folded with @c hash = rotl(hash + byte * prime5, 11) *
 * prime1. This makes the result depend on both the content and exact length
 * of inputs that share a prefix.
 *
 * Finally, the avalanche applies the following bijective-style mixing steps:
 * @code
 * hash ^= hash >> 15;  hash *= prime2;
 * hash ^= hash >> 13;  hash *= prime3;
 * hash ^= hash >> 16;
 * @endcode
 * The output is a fast, non-cryptographic 32-bit hash. It is appropriate for
 * in-process data structures and checksums, but not for security-sensitive
 * uses such as untrusted-input collision resistance or message authentication.
 *
 * @param data Pointer to the first byte to hash; it must provide @p length
 *             readable bytes when @p length is nonzero.
 * @param length Number of bytes to process.
 * @param seed Initial 32-bit state; distinct seeds produce distinct hash
 *             functions for the same input.
 * @return The 32-bit digest, zero-extended to @c usize on 64-bit targets.
 */
struct NEX_INTERNAL XXHash32 {
    NEX_HIDDEN_FROM_ABI usize operator()(cvptr data, usize length, uint32 seed = 0) const noexcept {
        constexpr uint32 prime1 = 0x9E3779B1u;
        constexpr uint32 prime2 = 0x85EBCA77u;
        constexpr uint32 prime3 = 0xC2B2AE3Du;
        constexpr uint32 prime4 = 0x27D4EB2Fu;
        constexpr uint32 prime5 = 0x165667B1u;

        const auto* p = static_cast<const uint8*>(data);
        usize remaining = length;
        const auto read32 = [](const uint8* bytes) noexcept -> uint32 {
            return static_cast<uint32>(bytes[0])
                 | (static_cast<uint32>(bytes[1]) << 8)
                 | (static_cast<uint32>(bytes[2]) << 16)
                 | (static_cast<uint32>(bytes[3]) << 24);
        };
        const auto rotl = [](uint32 value, uint32 count) noexcept -> uint32 {
            return (value << count) | (value >> (32 - count));
        };
        const auto round = [rotl](uint32 accumulator, uint32 input) noexcept -> uint32 {
            accumulator += input * prime2;
            accumulator = rotl(accumulator, 13);
            return accumulator * prime1;
        };

        uint32 hash;
        const uint32 seed32 = static_cast<uint32>(seed);
        if (length >= 16) {
            uint32 v1 = seed32 + prime1 + prime2;
            uint32 v2 = seed32 + prime2;
            uint32 v3 = seed32;
            uint32 v4 = seed32 - prime1;

            do {
                v1 = round(v1, read32(p)); p += 4;
                v2 = round(v2, read32(p)); p += 4;
                v3 = round(v3, read32(p)); p += 4;
                v4 = round(v4, read32(p)); p += 4;
                remaining -= 16;
            } while (remaining >= 16);

            hash = rotl(v1, 1) + rotl(v2, 7) + rotl(v3, 12) + rotl(v4, 18);
        } else {
            hash = seed32 + prime5;
        }

        hash += static_cast<uint32>(length);
        while (remaining >= 4) {
            hash += read32(p) * prime3;
            hash = rotl(hash, 17) * prime4;
            p += 4;
            remaining -= 4;
        }
        while (remaining != 0) {
            hash += static_cast<uint32>(*p++) * prime5;
            hash = rotl(hash, 11) * prime1;
            --remaining;
        }

        hash ^= hash >> 15;
        hash *= prime2;
        hash ^= hash >> 13;
        hash *= prime3;
        hash ^= hash >> 16;
        return static_cast<usize>(hash);
    }
};

/**
 * @brief One-shot implementation of the 64-bit XXHash algorithm.
 *
 * XXHash64 performs all arithmetic modulo 2^64 and decodes input words in
 * little-endian order. Its five fixed odd primes provide invertible modular
 * multiplications; interleaving those multiplications with rotations rapidly
 * diffuses changes in the input through the 64-bit state.
 *
 * Inputs of at least 32 bytes initialize four lanes as
 * @code
 * v1 = seed + prime1 + prime2;  v2 = seed + prime2;
 * v3 = seed;                    v4 = seed - prime1;
 * @endcode
 * Each 32-byte stripe contributes one 64-bit word to every lane. A round
 * transforms a lane @c v with input word @c w as
 * @code
 * v = rotl(v + w * prime2, 31) * prime1;
 * @endcode
 * The lanes are combined by rotating them by 1, 7, 12, and 18 bits and adding
 * the results. Each lane is then merged by xoring @c round(0, v) into the
 * hash and applying @c hash = hash * prime1 + prime4. Shorter inputs instead
 * begin from @c seed + prime5. The original byte length is added in both
 * cases.
 *
 * The tail is consumed from largest units to smallest. Each remaining 64-bit
 * word is mixed with @c round(0, word), then rotated by 27 bits and combined
 * with @c prime1 and @c prime4. A remaining 32-bit word is folded by rotating
 * 23 bits after multiplication by @c prime1, then combining @c prime2 and
 * @c prime3. Remaining bytes are folded with @c prime5, an 11-bit rotation,
 * and @c prime1. Thus, both input contents and exact length affect the digest.
 *
 * Finally, the avalanche applies the following mixing steps:
 * @code
 * hash ^= hash >> 33;  hash *= prime2;
 * hash ^= hash >> 29;  hash *= prime3;
 * hash ^= hash >> 32;
 * @endcode
 * The output is a fast, non-cryptographic 64-bit hash. It is suitable for
 * in-process data structures and checksums, but not security-sensitive uses
 * such as untrusted-input collision resistance or message authentication.
 *
 * @param data Pointer to the first byte to hash; it must provide @p length
 *             readable bytes when @p length is nonzero.
 * @param length Number of bytes to process.
 * @param seed Initial 64-bit state; distinct seeds produce distinct hash
 *             functions for the same input.
 * @return The 64-bit digest, represented as @c usize.
 */
struct NEX_INTERNAL XXHash64 {
    NEX_HIDDEN_FROM_ABI usize operator()(cvptr data, usize length, uint64 seed = 0) const noexcept {
        constexpr uint64 prime1 = 0x9E3779B185EBCA87ull;
        constexpr uint64 prime2 = 0xC2B2AE3D27D4EB4Full;
        constexpr uint64 prime3 = 0x165667B19E3779F9ull;
        constexpr uint64 prime4 = 0x85EBCA77C2B2AE63ull;
        constexpr uint64 prime5 = 0x27D4EB2F165667C5ull;

        const auto* p = static_cast<const uint8*>(data);
        usize remaining = length;
        const auto read32 = [](const uint8* bytes) noexcept -> uint32 {
            return static_cast<uint32>(bytes[0])
                 | (static_cast<uint32>(bytes[1]) << 8)
                 | (static_cast<uint32>(bytes[2]) << 16)
                 | (static_cast<uint32>(bytes[3]) << 24);
        };
        const auto read64 = [](const uint8* bytes) noexcept -> uint64 {
            return static_cast<uint64>(bytes[0])
                 | (static_cast<uint64>(bytes[1]) << 8)
                 | (static_cast<uint64>(bytes[2]) << 16)
                 | (static_cast<uint64>(bytes[3]) << 24)
                 | (static_cast<uint64>(bytes[4]) << 32)
                 | (static_cast<uint64>(bytes[5]) << 40)
                 | (static_cast<uint64>(bytes[6]) << 48)
                 | (static_cast<uint64>(bytes[7]) << 56);
        };
        const auto rotl = [](uint64 value, uint32 count) noexcept -> uint64 {
            return (value << count) | (value >> (64 - count));
        };
        const auto round = [rotl](uint64 accumulator, uint64 input) noexcept -> uint64 {
            accumulator += input * prime2;
            accumulator = rotl(accumulator, 31);
            return accumulator * prime1;
        };

        uint64 hash;
        const uint64 seed64 = static_cast<uint64>(seed);
        if (length >= 32) {
            uint64 v1 = seed64 + prime1 + prime2;
            uint64 v2 = seed64 + prime2;
            uint64 v3 = seed64;
            uint64 v4 = seed64 - prime1;

            do {
                v1 = round(v1, read64(p)); p += 8;
                v2 = round(v2, read64(p)); p += 8;
                v3 = round(v3, read64(p)); p += 8;
                v4 = round(v4, read64(p)); p += 8;
                remaining -= 32;
            } while (remaining >= 32);

            hash = rotl(v1, 1) + rotl(v2, 7) + rotl(v3, 12) + rotl(v4, 18);
            hash ^= round(0, v1); hash = hash * prime1 + prime4;
            hash ^= round(0, v2); hash = hash * prime1 + prime4;
            hash ^= round(0, v3); hash = hash * prime1 + prime4;
            hash ^= round(0, v4); hash = hash * prime1 + prime4;
        } else {
            hash = seed64 + prime5;
        }

        hash += static_cast<uint64>(length);
        while (remaining >= 8) {
            hash ^= round(0, read64(p));
            hash = rotl(hash, 27) * prime1 + prime4;
            p += 8;
            remaining -= 8;
        }
        if (remaining >= 4) {
            hash ^= static_cast<uint64>(read32(p)) * prime1;
            hash = rotl(hash, 23) * prime2 + prime3;
            p += 4;
            remaining -= 4;
        }
        while (remaining != 0) {
            hash ^= static_cast<uint64>(*p++) * prime5;
            hash = rotl(hash, 11) * prime1;
            --remaining;
        }

        hash ^= hash >> 33;
        hash *= prime2;
        hash ^= hash >> 29;
        hash *= prime3;
        hash ^= hash >> 32;
        return static_cast<usize>(hash);
    }
};

/**
 * @brief Hashes a block of memory.
 * @details Provides a hash function for arbitrary memory blocks.
 * @param data Pointer to the memory block.
 * @param length Size of the memory block in bytes.
 * @param seed Optional seed value for the hash function.
 * @return The computed hash value as usize.
 */
NEX_HIDDEN_FROM_ABI inline usize hashMemory(cvptr data, usize length, usize seed = 0) noexcept {
    static_assert(sizeof(usize) == 4 || sizeof(usize) == 8,
        "Error: Unsupported pointer size for hashMemory, expected 4 or 8 bytes (32-bit or 64-bit)");
    if constexpr (sizeof(usize) == 4) {
        return XXHash32()(data, length, static_cast<uint32>(seed));
    } else if constexpr (sizeof(usize) == 8) {
        return XXHash64()(data, length, static_cast<uint64>(seed));
    } else {
        // This branch should never be reached due to the static_assert above.
        NEX_UNREACHABLE();
    }
}

template <class Type, usize = (sizeof(Type) / sizeof(usize))>
struct ScalarHash;

template <class Type>
struct NEX_INTERNAL ScalarHash<Type, 0> {
    NEX_HIDDEN_FROM_ABI usize operator()(Type value) const noexcept {
        union {
            Type t;
            usize a;
        } u;
        u.a = 0;
        u.t = value;
        return u.a;
    }
};

template <class Type>
struct NEX_INTERNAL ScalarHash<Type, 1> {
    NEX_HIDDEN_FROM_ABI usize operator()(Type value) const noexcept {
        union {
            Type t;
            usize a;
        } u;
        u.t = value;
        return u.a;
    }
};

template <class Type>
struct NEX_INTERNAL ScalarHash<Type, 2> {
    NEX_HIDDEN_FROM_ABI usize operator()(Type value) const noexcept {
        union {
            Type t;
            struct {
                usize a;
                usize b;
            } s;
        } u;
        u.t = value;
        return hashMemory(NEX_ADDRESS_OF(u), sizeof(u));
    }
};

template <class Type>
struct NEX_INTERNAL ScalarHash<Type, 3> {
    NEX_HIDDEN_FROM_ABI usize operator()(Type value) const noexcept {
        union {
            Type t;
            struct {
                usize a;
                usize b;
                usize c;
            } s;
        } u;
        u.t = value;
        return hashMemory(NEX_ADDRESS_OF(u), sizeof(u));
    }
};

template <class Type>
struct NEX_INTERNAL ScalarHash<Type, 4> {
    NEX_HIDDEN_FROM_ABI usize operator()(Type value) const noexcept {
        union {
            Type t;
            struct {
                usize a;
                usize b;
                usize c;
                usize d;
            } s;
        } u;
        u.t = value;
        return hashMemory(NEX_ADDRESS_OF(u), sizeof(u));
    }
};

NEX_HIDDEN_FROM_ABI inline usize hashCombine(usize seed, usize value) noexcept {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

NEX_SUBNAMESPACE_END(hashing_impl)

/**
 * @brief Primary template for the Hash struct.
 * @tparam Type The type to be hashed.
 */
template <class Type>
struct NEX_INTERNAL HashImpl {
    HashImpl() = delete;
    NEX_DISALLOW_COPY(HashImpl);
};

/**
 * @brief Specialization of the Hash struct for enum types.
 * @details Provides a hash function for enum values by hashing their underlying integral type.
 * @tparam Type The enum type.
 */
template <class Type>
    requires (IsEnumV<Type> && meta::IsUnqualifiedV<Type>)
struct NEX_INTERNAL HashImpl<Type> {
    NEX_HIDDEN_FROM_ABI usize operator()(Type value) const noexcept {
        using type = UnderlyingType<Type>;
        return Hash<type>()(static_cast<type>(value));
    }
};

/**
 * @brief Specialization of the Hash struct for integral types that fit within usize.
 * @details Provides a hash function for integral values that are smaller than or equal to the size of usize.
 * @tparam Type The integral type.
 */
template <class Type>
    requires (IsIntegralV<Type> && meta::IsUnqualifiedV<Type> && (sizeof(Type) <= sizeof(usize)))
struct NEX_INTERNAL HashImpl<Type> {
    NEX_HIDDEN_FROM_ABI usize operator()(Type value) const noexcept {
        return static_cast<usize>(value);
    }
};

/**
 * @brief Specialization of the Hash struct for integral types that are larger than usize.
 * @details Provides a hash function for integral values that are larger than the size of usize.
 * @tparam Type The integral type.
 */
template <class Type>
    requires (IsIntegralV<Type> && meta::IsUnqualifiedV<Type> && (sizeof(Type) > sizeof(usize)))
struct NEX_INTERNAL HashImpl<Type> : hashing_impl::ScalarHash<Type> {};

/**
 * @brief Specialization of the Hash struct for floating-point types.
 * @details Provides a hash function for floating-point values, handling -0.0 and 0.0 equivalence.
 * @tparam Type The floating-point type.
 */
template <class Type>
    requires (IsFloatingPointV<Type> && meta::IsUnqualifiedV<Type>)
struct NEX_INTERNAL HashImpl<Type> : hashing_impl::ScalarHash<Type> {
    NEX_HIDDEN_FROM_ABI usize operator()(Type value) const noexcept {
        // -0.0 and 0.0 should return same hash
        if (value == 0.0f) {
            return 0;
        } else {
            return hashing_impl::ScalarHash<Type>()(value);
        }
    }
};

/**
 * @brief Specialization of the Hash struct for long double.
 * @details Provides a hash function for long double values, handling -0.0 and 0.0 equivalence.
 * @tparam ldouble The long double type.
 */
template <>
struct NEX_INTERNAL HashImpl<ldouble> : hashing_impl::ScalarHash<ldouble> {
    NEX_HIDDEN_FROM_ABI usize operator()(ldouble value) const noexcept {
        // -0.0 and 0.0 should return same hash
        if (value == 0.0L) {
            return 0;
        } else {
#if defined(NEX_ARCH_CPU_X86_LONG_DOUBLE_SIZE) && NEX_ARCH_CPU_X86_LONG_DOUBLE_SIZE == 12
            // Zero out padding bits
            union {
                ldouble t;
                struct {
                    usize a;
                    usize b;
                    usize c;
                    usize d;
                } s;
            } u;
            u.s.a = 0;
            u.s.b = 0;
            u.s.c = 0;
            u.s.d = 0;
            u.t   = value;
            return u.s.a ^ u.s.b ^ u.s.c ^ u.s.d;
#elif defined(NEX_ARCH_CPU_X86_LONG_DOUBLE_SIZE) && NEX_ARCH_CPU_X86_LONG_DOUBLE_SIZE == 16
            // Zero out padding bits
            union {
                ldouble t;
                struct {
                    usize a;
                    usize b;
                } s;
            } u;
            u.s.a = 0;
            u.s.b = 0;
            u.t   = value;
            return u.s.a ^ u.s.b;
#else
            return hashing_impl::ScalarHash<ldouble>()(value);
#endif
        }
    }
};

/**
 * @struct Hash
 * @brief General-purpose hash struct.
 * @tparam Type The type for which the hash is defined.
 * @details This struct provides a general-purpose hash function for the specified type.
 */
template <class Type>
struct NEX_API Hash : HashImpl<Type> {};

/**
 * @brief Specialization of the Hash struct for pointer types.
 * @details Provides a hash function for pointers by hashing their memory representation.
 * @tparam Type The type of the pointer.
 */
template <class Type>
struct NEX_API Hash<Type*> {
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI usize operator()(Type* value) const noexcept {
        union {
            Type* t;
            usize a;
        } u;
        u.t = value;
        return hashing_impl::hashMemory(NEX_ADDRESS_OF(u), sizeof(u));
    }
};

/**
 * @struct Hash<null>
 * @brief Specialization of the Hash struct for the nullptr type.
 * @tparam null The type representing the nullptr.
 */
template <>
struct NEX_API Hash<null> {
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI usize operator()(null) const noexcept {
        return 662607004ull;   // Arbitrary fixed hash value for nullptr
    }
};

/**
 * @brief Concept to check if a hasher meets the required hash requirements for a given key type.
 * @tparam Hasher The hasher type.
 * @tparam Key The key type.
 * @details
 * This concept ensures that the hasher is copy-constructible, move-constructible, invocable
 * with the key type, and returns a value convertible to usize.
 */
template <class Hasher, class Key>
concept HashRequirements =
    CopyConstructible<Hasher> &&
    MoveConstructible<Hasher> &&
    Invocable<usize, Hasher, Key const&> &&
    ConvertibleTo<InvokeResultT<Hasher, Key const&>, usize>;

/**
 * @brief Concept to check if a hasher meets the enabled hash requirements for a given key type.
 * @tparam Hasher The hasher type.
 * @tparam Key The key type.
 * @details
 * This concept ensures that the hasher meets the basic hash requirements and is also default-initializable.
 * This concept is useful for determining if a hasher can be used in contexts where default construction
 * is required, such as in certain container types.
 */
template <class Hasher, class Key>
concept EnabledHash =
    HashRequirements<Hasher, Key> &&
    DefaultInitializable<Hasher>;

/**
 * @brief Concept to check if a key type has an enabled hash.
 * @tparam Key The key type.
 * @details
 * This concept checks if the Hash struct for the given key type meets the enabled hash requirements.
 * This is typically used to determine if a key type can be safely used in hash-based containers.
 */
template <class Key>
concept HasEnabledHash = EnabledHash<Hash<Key>, Key>;

/**
 * @brief Concept to check if all given key types have an enabled hash.
 * @tparam Keys The key types.
 * @details
 * This concept checks if each key type in the parameter pack meets the enabled hash requirements.
 * It is useful for determining if multiple key types can be safely used in hash-based containers.
 */
template <class... Keys>
concept EnabledHashFor = (HasEnabledHash<Keys> && ...);

NEX_NAMESPACE_END
