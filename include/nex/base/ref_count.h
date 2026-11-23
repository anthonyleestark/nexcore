/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/namespace.h"
#include "nex/base/compiler_features.h"
#include "nex/base/data_model.h"
#include "nex/base/attributes.h"
#include "nex/base/visibility.h"
#include "nex/base/export.h"
#include "nex/base/meta.h"
#include "nex/base/types.h"
#include "nex/base/typeinfo.h"

#if NEX_COMPILER_IS_MSVC
    // Include intrinsics for atomic operations on MSVC.
    #include <intrin.h>
#endif  // ^^NEX_COMPILER_IS_MSVC

NEX_NAMESPACE_BEGIN

/**
 * @brief Increments a reference counter by 1, atomically when supported.
 * @tparam Type Counter integer type.
 * @param refCount Counter value to increment.
 * @return The new counter value after incrementing.
 * 
 * @note
 * Uses atomic operations when available; otherwise falls back to a non-atomic increment.
 * Synchronization of the managed object's lifetime is handled separately by the caller.
 */
template <class Type>
    requires (meta::IsIntegralV<Type>)
NEX_HIDDEN_FROM_ABI inline Type atomicRefCountIncrement(Type& refCount) noexcept {
#if NEX_HAS_BUILTIN(__atomic_add_fetch)
    return __atomic_add_fetch(meta::_addressOf(refCount), 1, __ATOMIC_RELAXED);
#elif NEX_COMPILER_IS_MSVC
    #if NEX_DATA_MODEL_IS_64_BIT
        return _InterlockedIncrement64(
            reinterpret_cast<volatile int64*>(meta::_addressOf(refCount))
        );
    #elif NEX_DATA_MODEL_IS_32_BIT
        return _InterlockedIncrement(
            reinterpret_cast<volatile int32*>(meta::_addressOf(refCount))
        );
    #else
        #error Unsupported data model for MSVC atomic increment.
    #endif
#else  // Fallback to non-atomic increment
    return refCount += 1;
#endif
}

/**
 * @brief Decrements a reference counter by 1, atomically when supported.
 * @tparam Type Counter integer type.
 * @param refCount Counter value to decrement.
 * @return The new counter value after decrementing.
 * @note Uses atomic operations if available, otherwise it falls back to a non-atomic decrement.
 */
template <class Type>
    requires (meta::IsIntegralV<Type>)
NEX_HIDDEN_FROM_ABI inline Type atomicRefCountDecrement(Type& refCount) noexcept {
#if NEX_HAS_BUILTIN(__atomic_sub_fetch)
    return __atomic_sub_fetch(meta::_addressOf(refCount), 1, __ATOMIC_ACQ_REL);
#elif NEX_COMPILER_IS_MSVC
    #if NEX_DATA_MODEL_IS_64_BIT
        return _InterlockedDecrement64(
            reinterpret_cast<volatile int64*>(meta::_addressOf(refCount))
        );
    #elif NEX_DATA_MODEL_IS_32_BIT
        return _InterlockedDecrement(
            reinterpret_cast<volatile int32*>(meta::_addressOf(refCount))
        );
    #else
        #error Unsupported data model for MSVC atomic decrement.
    #endif
#else  // Fallback to non-atomic decrement
    return refCount -= 1;
#endif
}

/**
 * @class SharedCount
 * @brief Base class for managing strong shared ownership counts.
 *
 * @details
 * Provides thread-safe reference counting for shared ownership semantics.
 * Derived classes implement resource cleanup when the last strong owner is released.
 */
class NEX_EXPORTED_FROM_ABI SharedCount {
    SharedCount(const SharedCount&);
    SharedCount& operator=(const SharedCount&);

public:
    using value_type = uint64;

protected:
    value_type sharedOwners_;
    virtual ~SharedCount();

private:
    /**
     * @brief Invoked when the last strong shared owner is released.
     * @note Derived classes must implement this hook to release the managed resource.
     */
    virtual void onZeroShared() = 0;

public:
    // Constructs a shared reference counter.
    NEX_HIDDEN_FROM_ABI explicit SharedCount(value_type refs = 0) noexcept
        : sharedOwners_(refs) {}

    // Increments the strong shared ownership count.
    NEX_HIDDEN_FROM_ABI void addShared() noexcept { atomicRefCountIncrement(sharedOwners_); }

    /**
     * @brief Decrements the strong shared ownership count.
     * @return `true` if the last strong owner was released, `false` otherwise.
     */
    NEX_HIDDEN_FROM_ABI bool releaseShared() noexcept {
        if (atomicRefCountDecrement(sharedOwners_) == -1) {
            onZeroShared();
            return true;
        }
        return false;
    }

    // Returns the number of active strong shared owners.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI value_type useCount() const noexcept {
#if NEX_HAS_BUILTIN(__atomic_load_n)
        return __atomic_load_n(&sharedOwners_, __ATOMIC_RELAXED) + 1;
#elif NEX_COMPILER_IS_MSVC
    #if NEX_DATA_MODEL_IS_64_BIT
        return _InterlockedOr64(reinterpret_cast<volatile long*>(&sharedOwners_), 0) + 1;
    #elif NEX_DATA_MODEL_IS_32_BIT
        return _InterlockedOr(reinterpret_cast<volatile long*>(&sharedOwners_), 0) + 1;
    #else
        #error Unsupported data model for MSVC atomic load.
    #endif
#else  // Fallback to non-atomic load.
        return sharedOwners_ + 1;
#endif
    }
};

/**
 * @class SharedWeakCount
 * @brief Base class for managing both strong shared and weak ownership counts.
 *
 * @details
 * Extends SharedCount to track weak references independently from strong
 * ownership, allowing the control block to outlive the managed object until
 * all weak references are released.
 */
class NEX_EXPORTED_FROM_ABI SharedWeakCount : public SharedCount {
    value_type sharedWeakOwners_;

public:
    // Constructs a combined shared and weak reference counter.
    NEX_HIDDEN_FROM_ABI explicit SharedWeakCount(value_type refs = 0) noexcept
        : SharedCount(refs), sharedWeakOwners_(refs) {}

protected:
    /**
     * @brief Destructor for SharedWeakCount.
     * @note Implementations should release any resources associated with both shared and weak ownership.
     */
    ~SharedWeakCount() override;

public:
    // Increments the strong shared ownership count.
    NEX_HIDDEN_FROM_ABI void addShared() noexcept { SharedCount::addShared(); }

    // Increments the weak ownership count.
    NEX_HIDDEN_FROM_ABI void addWeak() noexcept { atomicRefCountIncrement(sharedWeakOwners_); }

    /**
     * @brief Decrements the strong shared ownership count.
     * @note Releases the associated weak reference if the last strong owner is released.
     */
    NEX_HIDDEN_FROM_ABI void releaseShared() noexcept {
        if (SharedCount::releaseShared()) {
            releaseWeak();
        }
    }

    // Returns the number of active strong shared owners.
    NEX_NODISCARD NEX_HIDDEN_FROM_ABI value_type useCount() const noexcept { return SharedCount::useCount(); }

    // Decrements the weak ownership count and cleans up when no owners remain.
    void releaseWeak() noexcept;

    /**
     * @brief Attempts to acquire strong shared ownership from a weak reference.
     * @return A pointer to this control block on success, or `nullptr` if expired.
     */
    SharedWeakCount* lock() noexcept;

    /**
     * @brief Retrieves the deleter associated with the managed object, if any.
     * @param typeInfo Type descriptor of the expected deleter.
     * @return A pointer to the deleter if available and matching, or `nullptr` otherwise.
     */
    virtual const void* getDeleter(const TypeInfo&) const noexcept = 0;

private:
    /**
     * @brief Invoked when both strong and weak ownership counts reach zero.
     * @note Derived classes must implement this hook to destroy the control block.
     */
    virtual void onZeroSharedWeak() = 0;
};

NEX_NAMESPACE_END
