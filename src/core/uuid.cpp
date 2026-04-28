/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <functional> // for std::hash

#include "nex/core/uuid.h"
#include "nex/base/assert_crash.h"
#include <stduuid/uuid.h>

#ifndef NEX_STDUUID_SUPPORTS_V7
    // Because stduuid does not yet support UUID version 7, we will need to implement our own time-ordered 
    // UUID generation logic until stduuid adds support for it.
    // For now, we will generate version 7 UUIDs using a combination of the current timestamp and random data, 
    // following the general structure of a version 7 UUID as defined in the UUID specification.
    // This is a temporary workaround and should be replaced with proper version 7 UUID generation 
    // once stduuid adds support for it.
    #include <chrono>
    #include <random>
    #include <array>
#endif

NEX_CORE_NAMESPACE_BEGIN

// Internal implementation of Uuid class using stduuid library
struct Uuid::Impl {
    // The actual UUID data is stored using the stduuid library's uuid class
    uuids::uuid uuid{};
};

// Default constructor (create a nil/invalid UUID with all bits set to zero)
Uuid::Uuid() noexcept
    : impl_(NEX_STD make_unique<Impl>()) {}

// Copy constructor (deep copy of the internal implementation)
Uuid::Uuid(const Uuid& other)
    : impl_(NEX_STD make_unique<Impl>(*other.impl_)) {}

// Copy assignment operator
Uuid& Uuid::operator=(const Uuid& other) {
    if (this != &other) {
        *impl_ = *other.impl_;
    }
    return *this;
}

// Default move semantics
NEX_DEFINE_DEFAULT_MOVE(Uuid)

// Default destructor
NEX_DEFINE_DEFAULT_DTOR(Uuid)

// Generate a new UUID (universally unique identifier; uses V4 random generation by default)
Uuid Uuid::generate() noexcept {
    // Setup a random number generator with a non-deterministic seed
    NEX_STD random_device rd;
    auto seed_data = NEX_STD array<int, 6> {};
    NEX_STD generate(NEX_STD begin(seed_data), NEX_STD end(seed_data), NEX_STD ref(rd));
    NEX_STD seed_seq seq(NEX_STD begin(seed_data), NEX_STD end(seed_data));
    NEX_STD ranlux48_base generator(seq);

    // Initialize the UUID generator with the random number generator
    uuids::basic_uuid_random_generator<NEX_STD ranlux48_base> gen(&generator);

    // Generate a new UUID using the generator
    Uuid newUuid{};
    newUuid.impl_->uuid = gen();

    // Validate that the generator produces valid UUIDs (this is a sanity check and should always pass)
    NEX_ASSERT_MSG(!newUuid.impl_->uuid.is_nil(), "Generated UUID should not be nil");
    NEX_ASSERT_MSG(newUuid.impl_->uuid.as_bytes().size() == 16, "Generated UUID should be 16 bytes");
    NEX_ASSERT_MSG(newUuid.impl_->uuid.variant() == uuids::uuid_variant::rfc, 
                        "Generated UUID should have RFC variant");
    NEX_ASSERT_MSG(newUuid.impl_->uuid.version() == uuids::uuid_version::random_number_based, 
                        "Generated UUID should be version 4 (random number based)");

    // Successfully generated a new UUID, return it
    return newUuid;
}

// Get a nil/invalid UUID (all bits set to zero)
Uuid Uuid::nil() noexcept {
    Uuid nilUuid{};
    nilUuid.impl_->uuid = uuids::uuid{}; // Default constructor creates a nil UUID
    return nilUuid;
}

// Generate a new version 4 (random) UUID
Uuid Uuid::generateV4() noexcept {
    return generate(); // The default generate method uses V4 random generation
}

// Generate a new version 7 (time-ordered) UUID
Uuid Uuid::generateV7() noexcept {
#ifndef NEX_STDUUID_SUPPORTS_V7
    // Since stduuid does not support UUID version 7, we will implement our own generation logic here.

    using namespace NEX_STD chrono;

    // 48-bit timestamp (milliseconds since Unix epoch)
    auto now = system_clock::now().time_since_epoch();
    uint64 timestamp_ms = duration_cast<milliseconds>(now).count();

    // 74 bits of randomness
    NEX_STD random_device rd;
    NEX_STD mt19937_64 gen(rd());
    NEX_STD uniform_int_distribution<uint64> dist;

    uint64 random_high = dist(gen) & 0x0FFFFFFFFFFFFFFFULL;  // 60 bits
    uint64 random_low  = dist(gen) & 0xFFFFFFFFFFFFFFFFULL;  // 64 bits → we'll use 14 bits

    // Construct UUID v7:
    // - 48 bits timestamp
    // - 4 bits version = 0b0111 (7)
    // - 12 bits random (part of the 74)
    // - 2 bits variant = 0b10
    // - 62 bits random
    uint64 high = (timestamp_ms << 16) | (0x7ULL << 12) | (random_high >> 48);

    uint64 low  = ((random_high & 0x0000FFFFFFFFFFFFULL) << 16) |
                    (0x2ULL << 62) | (random_low >> 2);   // variant 10xx

    // Pack into uuid (stduuid stores as array<uint8_t,16>)
    NEX_STD array<uint8, 16> data{};
    for (int i = 0; i < 8; ++i) {
        data[i]     = (high >> (56 - i*8)) & 0xFF;
        data[8 + i] = (low  >> (56 - i*8)) & 0xFF;
    }

    // Create a Uuid instance to hold the generated UUID
    Uuid newUuid{};
    newUuid.impl_->uuid = uuids::uuid{data};

    // Validate that the generator produces valid UUIDs (this is a sanity check and should always pass)
    NEX_ASSERT_MSG(!newUuid.impl_->uuid.is_nil(), "Generated UUID should not be nil");
    NEX_ASSERT_MSG(newUuid.impl_->uuid.as_bytes().size() == 16, "Generated UUID should be 16 bytes");
    NEX_ASSERT_MSG(newUuid.impl_->uuid.variant() == uuids::uuid_variant::rfc, 
                        "Generated UUID should have RFC variant");
    NEX_ASSERT_MSG(newUuid.impl_->uuid.version() == uuids::uuid_version::time_based, 
                        "Generated UUID should be version 7 (time-based)");

    // Successfully generated a new version 7 UUID, return it
    return newUuid;
#endif
}

// Parse a Uuid from a string representation
Result<Uuid, Error> Uuid::fromString(StringView str) noexcept {
    // Convert the input string to UTF-8 for validation and parsing
    auto res = str.toString().toUtf8();
    if (!res.isOk()) {
        return Result<Uuid, Error>::error({
            ErrorCode::InvalidFormat, 
            "Failed to convert input string to UTF-8 for UUID parsing"
        });
    }

    // Validate the UTF-8 string format for a UUID (e.g., "123e4567-e89b-12d3-a456-426614174000")
    NEX_STD string utf8 = res.value();
    if (utf8.empty() || !uuids::uuid::is_valid_uuid(utf8)) {
        return Result<Uuid, Error>::error({
            ErrorCode::InvalidFormat, 
            "Input string is not a valid UUID format"
        });
    }

    // Parse the UUID using the stduuid library
    auto resParse = uuids::uuid::from_string(utf8);
    if (!resParse.has_value()) {
        return Result<Uuid, Error>::error({
            ErrorCode::InvalidFormat,
            "Failed to parse UUID from string"
        });
    }

    // Successfully parsed the UUID, create a Uuid instance to hold it
    Uuid parsedUuid{};
    parsedUuid.impl_->uuid = *resParse;

    // Successfully parsed the UUID, return it wrapped in a Result
    return Result<Uuid, Error>::ok(parsedUuid);
}

// Check if the Uuid is nil/invalid (all bits equal to zero)
bool Uuid::isNil() const noexcept {
    return impl_->uuid.is_nil();
}

// Convert to string
String Uuid::toString() const {
    // Convert the internal UUID to its string representation using the stduuid library
    NEX_STD string utf8 = uuids::to_string<char>(impl_->uuid);
    return String::fromUtf8(utf8);
}

// Get a hash value for the Uuid (enables use in hash-based containers)
usize Uuid::hash() const noexcept {
    // Use the stduuid library's built-in hash function for the uuid class
    return NEX_STD hash<uuids::uuid>{}(impl_->uuid);
}

NEX_CORE_NAMESPACE_END