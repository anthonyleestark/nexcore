/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <utility>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/traits.h"
#include "nex/base/location.h"

#if NEX_USE_STD_SOURCE_LOCATION
    #include <source_location>
#endif

NEX_NAMESPACE_BEGIN

/**
 * @namespace utility
 * @brief   Contains utility functions and classes that provide common functionality and services
 *          across the NEX system, such as helper functions, common data structures, and other
 *          utilities that can be used by various components of the system.
 * 
 * @details
 * The `utility` namespace is intended to contain utility functions and classes that provide common
 * functionality and services across the NEX system, such as helper functions, common data structures,
 * and other utilities that can be used by various components of the system. The utilities in this
 * namespace are designed to be reusable and can be used by different layers and components of the system
 * without introducing dependencies on specific layers or components. The `utility` namespace is intended to
 * provide a centralized location for common utilities that can be used throughout the codebase, improving
 * code reuse and reducing duplication of common functionality.
 */
NEX_SUBNAMESPACE_BEGIN(utility)

////// Helpers functions array/struct/class processing -------------------------------

// Get the size of a statically sized array, which can be used for various purposes such as
// implementing container_of and other utilities that require knowledge of the size of an array.
template <typename Type, usize Capacity>
constexpr usize arraySize(Type (&)[Capacity]) noexcept {
    return Capacity;
}

// Get the offset of a member within a struct/class, which can be used for various purposes such as
// implementing container_of and other utilities that require knowledge of the layout of a struct/class.
template <typename Type, typename MemberT>
constexpr isize offsetOf(MemberT Type::*member) noexcept {
    static_assert(IsStandardLayoutV<Type>, "offsetOf only safe for standard-layout types");
    return reinterpret_cast<isize>(&reinterpret_cast<Type*>(0)->*member);
}

// Get the containing struct/class from a pointer to a member, which can be used for various purposes such as
// implementing container_of and other utilities that require knowledge of the layout of a struct/class.
template <typename Type, auto MemberPtr>
constexpr Type* containerOf(decltype(MemberPtr) ptr) noexcept {
    using MemberT = RemoveReference<decltype(NEX_STD declval<Type>().*MemberPtr)>;
    static_assert(IsStandardLayoutV<Type>, "containerOf only safe for standard-layout types");
    
    return reinterpret_cast<Type*>(
        reinterpret_cast<char*>(ptr) - offsetOf<Type, MemberT>(MemberPtr)
    );
}

////// Helper functions for safe downcast and dereference -------------------------------

// Safely dereference a member pointer, ensuring that the pointer type matches the member type, 
// and returning a reference to the containing struct/class.
template <typename Type, typename MemberT>
constexpr Type& derefMember(MemberT* memberPtr, MemberT Type::*member) noexcept {
    return *containerOf(memberPtr, member);
}

// Safe downcast of a pointer to a member to a pointer to the containing struct/class, 
// ensuring that the pointer type matches the member type.
template <typename Type, typename MemberT>
constexpr Type* downcastMember(MemberT* memberPtr, MemberT Type::*member) noexcept {
    return containerOf(memberPtr, member);
}

// Safe downcast of a pointer to a base class to a pointer to a derived class with checking for inheritance 
// relationship at compile time, ensuring that the pointer type matches the class type.
template <typename Derived, typename Base>
constexpr Derived* safeDowncast(Base* base) noexcept {
    static_assert(IsBaseOfV<Base, Derived>, "safeDowncast requires Derived to be a subclass of Base");
    return dynamic_cast<Derived*>(base);
}

////// Helper function for safe deletion --------------------------------

// Safely delete a pointer to a struct/class, ensuring that the pointer type matches the struct/class type.
template <typename Type>
void deleteObject(Type* objPtr) noexcept {
    if (objPtr) {
        delete objPtr;
        (objPtr) = nullptr; // Set the pointer to nullptr after deletion to prevent dangling pointer
    }
}

// Safely delete an array pointer to a struct/class, ensuring that the pointer type matches the struct/class type.
template <typename Type>
void deleteObjectArray(Type* objPtr) noexcept {
    if (objPtr) {
        delete[] objPtr;
        (objPtr) = nullptr; // Set the pointer to nullptr after deletion to prevent dangling pointer
    }
}

// Safely delete a pointer to a member, ensuring that the pointer type matches the member type,
// and deleting the containing struct/class.
template <typename Type, typename MemberT>
void deleteMember(MemberT* memberPtr, MemberT Type::*member) noexcept {
    if (memberPtr) {
        delete containerOf(memberPtr, member);
    }
}

// Safely delete a pointer to a member and set it to nullptr, ensuring that the pointer type matches 
// the member type, and deleting the containing struct/class.
template <typename Type, typename MemberT>
void deleteMemberAndNull(MemberT*& memberPtr, MemberT Type::*member) noexcept {
    if (memberPtr) {
        delete containerOf(memberPtr, member);
        memberPtr = nullptr;
    }
}

// Safely delete an array pointer to a member and set it to nullptr, ensuring that the pointer type matches 
// the member type, and deleting the containing struct/class.
template <typename Type, typename MemberT>
void deleteMemberArrayAndNull(MemberT*& memberPtr, MemberT Type::*member) noexcept {
    if (memberPtr) {
        delete[] containerOf(memberPtr, member);
        memberPtr = nullptr;
    }
}

////// Helper functions for hashing and combining hash values ---------------------------------

// Combine two hash values into a single hash value, which can be used for various purposes such as implementing 
// hash-based containers that use composite keys, and improving the performance of lookups by providing a way to combine 
// hash values of multiple components of a composite key. The hash combination function uses a common technique to 
// combine hash values, which helps to reduce the likelihood of hash collisions and improve the performance of lookups 
// in hash-based containers.
constexpr int64 combineHash(int64 h1, int64 h2) noexcept {
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

////// Helper functions for enum class keys ---------------------------------

// Get the underlying integer value of an enum class key, which can be used for various purposes such as
// implementing mapping of enum keys to configuration entries and other utilities that require knowledge of 
// the underlying integer value of the enum.
template <typename Enum>
constexpr int64 enumKeyValue(Enum key) noexcept {
    static_assert(IsEnumV<Enum>, "enumKeyValue only works for enum types");
    return static_cast<int64>(key);
}

// Get the underlying integer value of an enum class key, which can be used for various purposes such as
// implementing mapping of enum keys to configuration entries and other utilities that require knowledge of 
// the underlying integer value of the enum.
template <typename Enum>
constexpr auto toUnderlying(Enum key) noexcept {
    static_assert(IsEnumV<Enum>, "toUnderlying only works for enum types");
    return static_cast<NEX_STD underlying_type_t<Enum>>(key);
}

// Get a hash value for an enum class key, which can be used for various purposes such as implementing
// hash-based containers that use enum keys, and improving the performance of lookups by providing a hash function 
// for enum keys. The hash function combines the hash of the enum type and the integer value of the enum key 
// to reduce the likelihood of hash collisions and improve the performance of lookups in hash-based containers.
template <typename Enum>
constexpr int64 enumKeyHash(Enum key) noexcept {
    static_assert(IsEnumV<Enum>, "enumKeyHash only works for enum types");
    return typeid(Enum).hash_code() ^ enumKeyValue(key);
}

// Combine the hash of the enum type and the integer value of the enum key to produce a unique hash value for each 
// unique combination of enum type and value, reducing the likelihood of hash collisions and improving the performance 
// of lookups in hash-based containers.
template <typename Enum>
constexpr int64 enumKeyHashCombined(Enum key) noexcept {
    static_assert(IsEnumV<Enum>, "enumKeyHashCombined only works for enum types");
    int64 h1 = NEX_STD hash<usize>{}(typeid(Enum).hash_code());
    int64 h2 = NEX_STD hash<int64>{}(enumKeyValue(key));
    return combineHash(h1, h2);
}

// Enable/disable bitmask operators for an enum class, which can be used for various purposes such as implementing
// configuration flags and other utilities that require bitmask operations on enum keys. By default, bitmask operators 
// are disabled for all enum classes, and can be enabled for specific enum classes by specializing
template <typename Enum>
struct EnableBitmaskOperators {
    static constexpr bool enable = false;
};

// Bitwise OR operator for enum classes with bitmask operators enabled
template <typename Enum>
constexpr EnableIf<EnableBitmaskOperators<Enum>::enable, Enum>
operator|(Enum lhs, Enum rhs) {
    using Type = NEX_STD underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<Type>(lhs) | static_cast<Type>(rhs));
}

// Bitwise AND operator for enum classes with bitmask operators enabled
template <typename Enum>
constexpr EnableIf<EnableBitmaskOperators<Enum>::enable, Enum>
operator&(Enum lhs, Enum rhs) {
    using Type = NEX_STD underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<Type>(lhs) & static_cast<Type>(rhs));
}

// Bitwise XOR operator for enum classes with bitmask operators enabled
template <typename Enum>
constexpr EnableIf<EnableBitmaskOperators<Enum>::enable, Enum>
operator^(Enum lhs, Enum rhs) {
    using Type = NEX_STD underlying_type_t<Enum>;
    return static_cast<Enum>(static_cast<Type>(lhs) ^ static_cast<Type>(rhs));
}

// Bitwise NOT operator for enum classes with bitmask operators enabled
template <typename Enum>
constexpr EnableIf<EnableBitmaskOperators<Enum>::enable, Enum>
operator~(Enum key) {
    using Type = NEX_STD underlying_type_t<Enum>;
    return static_cast<Enum>(~static_cast<Type>(key));
}

// Bitwise OR assignment operator for enum classes with bitmask operators enabled
template <typename Enum>
constexpr EnableIf<EnableBitmaskOperators<Enum>::enable, Enum&>
operator|=(Enum& lhs, Enum rhs) {
    lhs = lhs | rhs;
    return lhs;
}

// Bitwise AND assignment operator for enum classes with bitmask operators enabled
template <typename Enum>
constexpr EnableIf<EnableBitmaskOperators<Enum>::enable, Enum&>
operator&=(Enum& lhs, Enum rhs) {
    lhs = lhs & rhs;
    return lhs;
}

// Bitwise XOR assignment operator for enum classes with bitmask operators enabled
template <typename Enum>
constexpr EnableIf<EnableBitmaskOperators<Enum>::enable, Enum&>
operator^=(Enum& lhs, Enum rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

////// Utility classes for memory blocks ---------------------------------

/**
 * @struct MemoryBlock
 * @brief  Non-owning mutable memory block described by pointer and byte size.
 */
struct MemoryBlock {
    void_ptr data = nullptr;    // Pointer to the start of the memory block
    usize size = 0;             // Size of the memory block in bytes

    // Default constructor
    constexpr MemoryBlock() noexcept = default;

    // Constructor with data pointer and size parameters
    constexpr MemoryBlock(void_ptr dataValue, usize sizeValue) noexcept
        : data(dataValue), size(sizeValue) {}

    // Check if the memory block is empty (data is null or size is zero)
    constexpr bool empty() const noexcept {
        return data == nullptr || size == 0;
    }

    // Get a byte pointer to the start of the memory block
    constexpr byte_ptr bytes() const noexcept {
        return static_cast<byte_ptr>(data);
    }

    // Get a byte pointer to the end of the memory block
    constexpr byte_ptr end() const noexcept {
        return empty() ? nullptr : bytes() + size;
    }

    // Equality operator
    constexpr bool operator==(const MemoryBlock&) const noexcept = default;
};

/**
 * @struct ConstMemoryBlock
 * @brief  Non-owning immutable memory block described by pointer and byte size.
 */
struct ConstMemoryBlock {
    const_void_ptr data = nullptr;    // Pointer to the start of the memory block
    usize size = 0;                   // Size of the memory block in bytes

    // Default constructor
    constexpr ConstMemoryBlock() noexcept = default;

    // Constructor with data pointer and size parameters
    constexpr ConstMemoryBlock(const_void_ptr dataValue, usize sizeValue) noexcept
        : data(dataValue), size(sizeValue) {}

    // Constructor from a mutable MemoryBlock
    constexpr ConstMemoryBlock(MemoryBlock block) noexcept
        : data(block.data), size(block.size) {}

    // Check if the memory block is empty (data is null or size is zero)
    constexpr bool empty() const noexcept {
        return data == nullptr || size == 0;
    }

    // Get a byte pointer to the start of the memory block
    constexpr const_byte_ptr bytes() const noexcept {
        return static_cast<const_byte_ptr>(data);
    }

    // Get a byte pointer to the end of the memory block
    constexpr const_byte_ptr end() const noexcept {
        return empty() ? nullptr : bytes() + size;
    }

    // Equality operator
    constexpr bool operator==(const ConstMemoryBlock&) const noexcept = default;
};

////// Utility classes for RAII and generic patterns -------------------------------

/**
 * @class AutoReset
 * @brief RAII utility class for automatically resetting a variable to its original value
 * 
 * This template class provides a convenient way to temporarily set a variable to a new value
 * within a particular scope. The variable is automatically reset to its original value when
 * the AutoReset object is destroyed, eliminating the need to manually reset the variable
 * at all exit points of a block.
 * 
 * AutoReset supports:
 * - Construction from a reference to a variable (saves original value)
 * - Construction from a reference and a new value (sets new value immediately)
 * - Factory methods (maybe) for handling nullable pointers
 * - Automatic restoration of the original value upon destruction (RAII pattern)
 * - Move semantics for transferring ownership
 * - Template type support for any assignable type
 * 
 * @note The AutoReset instance must have a shorter lifetime than the scoped variable
 *       to prevent invalid memory writes when the AutoReset object is destroyed.
 * 
 * Example usage:
 * @code
 * bool flag = false;
 * {
 *     AutoReset<bool> reset(flag, true);  // flag is now true
 *     // ... do work with flag == true ...
 * }  // flag is automatically reset to false when reset goes out of scope
 * 
 * // With nullable pointer:
 * bool* pFlag = getFlagPointer();
 * auto reset = AutoReset<bool>::maybe(pFlag, true);  // Safe if pFlag is null
 * @endcode
 */
template <typename Type>
class NEX_API AutoReset {
public:
    // Constructor: saves the original value of the variable
    explicit AutoReset(Type& var)
        : ptrScopedVar_(&var), originalVal_(var) {}

    // Constructor: sets the variable to new value and saves the original
    AutoReset(Type& var, Type newValue)
        : ptrScopedVar_(&var), originalVal_(NEX_STD exchange(var, NEX_STD move(newValue))) {}

    // Factory method for handling nullable pointers (saves original value)
    static AutoReset maybe(Type* ptr) {
        return ptr ? AutoReset(*ptr) : AutoReset();
    }

    // Factory method for handling nullable pointers (sets new value if not null)
    static AutoReset maybe(Type* ptr, Type newValue) {
        return ptr ? AutoReset(*ptr, NEX_STD move(newValue)) : AutoReset();
    }

    // Move constructor: transfers ownership from another AutoReset instance
    AutoReset(AutoReset&& other) noexcept
        : ptrScopedVar_(NEX_STD exchange(other.ptrScopedVar_, nullptr)),
        originalVal_(NEX_STD move(other.originalVal_)) {}

    // Move assignment operator: transfers ownership from another AutoReset instance
    AutoReset& operator=(AutoReset&& rhs) noexcept {
        if (this != &rhs) {
            reset();
            ptrScopedVar_ = NEX_STD exchange(rhs.ptrScopedVar_, nullptr);
            originalVal_ = NEX_STD move(rhs.originalVal_);
        }
        return *this;
    }

    // Destructor: resets the scoped variable to its original value
    ~AutoReset() {
        reset();
    }

    // --- Disable copy ---
    NEX_DISALLOW_COPY(AutoReset);

private:
    // Null object state
    AutoReset() : ptrScopedVar_(nullptr) {}

    // Reset the scoped variable to its original value
    void reset() noexcept {
        if (ptrScopedVar_) {
            *ptrScopedVar_ = NEX_STD move(originalVal_);
            ptrScopedVar_ = nullptr;
        }
    }

    // The scoped variable pointer
    Type* ptrScopedVar_;

    // The original value of the scoped variable
    Type originalVal_;
};

/**
 * @class AutoFlag
 * @brief A template for setting a flag on a variable as long as the
 * object that set it is in scope. Flag resets when object goes
 * out of scope. Works on anything that looks like bool.
 */
template <class Type = bool>
class NEX_API AutoFlag {
public:
    // Constructor: Sets referent to true.
    AutoFlag(Type& ref) : referent_(ref) {
        referent_ = true;
    }

    // Destructor: Sets referent passed to constructor to false.
    ~AutoFlag() {
        referent_ = false;
    }

private:
    Type& referent_;
};

/**
 * @class ScopeGuard
 * @brief A template for executing a callable object when the ScopeGuard goes out of scope
 * 
 * @details
 * This template class provides a convenient way to execute a callable object (e.g., a lambda function) 
 * when the ScopeGuard object goes out of scope. This is useful for ensuring that certain cleanup or finalization 
 * code is executed regardless of how the scope is exited (e.g., normal return, exception, etc.). The callable 
 * object is executed in the destructor of the ScopeGuard, following the RAII pattern.
 * 
 * @tparam FuncType The type of the callable object (e.g., lambda) to be executed on scope exit.
 */
template <typename FuncType>
class NEX_API ScopeGuard {
public:
    // Constructor: Takes a callable object (e.g., lambda) to be called on scope exit
    explicit ScopeGuard(FuncType f) 
        : func_(NEX_STD move(f)) {}

    // Destructor: Calls the stored callable object if not dismissed
    // The destructor is marked noexcept to ensure that it does not throw exceptions, 
    // as throwing exceptions from a destructor can lead to std::terminate if another exception is active. 
    // The callable object is only called if the ScopeGuard has not been dismissed, allowing for 
    // conditional execution of the cleanup code.
    ~ScopeGuard() noexcept { 
        if (!dismissed_) func_(); 
    }

    // Dismiss the scope guard, preventing the callable from being called on scope exit
    void dismiss() noexcept { dismissed_ = true; }
    
    // Disable copy semantics for ScopeGuard to prevent accidental copying 
    // which could lead to multiple calls of the callable object
    NEX_DISALLOW_COPY(ScopeGuard);
    
    // Enable move semantics for ScopeGuard to allow transferring ownership of the callable object
    ScopeGuard(ScopeGuard&& other) noexcept
        : func_(NEX_STD move(other.func_)), dismissed_(other.dismissed_) {
        other.dismissed_ = true; // Prevent the moved-from object from calling the callable
    }
    ScopeGuard& operator=(ScopeGuard&& rhs) noexcept {
        if (this != &rhs) {
            if (!dismissed_) func_(); // Call the current callable if not dismissed
            func_ = NEX_STD move(rhs.func_);
            dismissed_ = rhs.dismissed_;
            rhs.dismissed_ = true; // Prevent the moved-from object from calling the callable
        }
        return *this;
    }

private:
    FuncType func_;
    bool dismissed_ = false;
};

/**
 * @class   OnScopeSuccess
 * @brief   Scope guard that only executes the callable if the scope is exited successfully without an exception
 * 
 * @details
 * This template class provides a convenient way to execute a callable object (e.g., a lambda function) when 
 * the scope is exited successfully without an exception. The callable object is executed in the destructor 
 * of the OnScopeSuccess, following the RAII pattern. The destructor checks if any exceptions were thrown 
 * during the lifetime of the OnScopeSuccess object, and only calls the callable if no exceptions were thrown, 
 * allowing for conditional execution of the cleanup code based on whether the scope was exited successfully 
 * or due to an exception.
 * 
 * @tparam FuncType The type of the callable object (e.g., lambda) to be executed on successful scope exit.
 * 
 * @note The OnScopeSuccess object must have a shorter lifetime than the scope it is guarding to ensure that 
 *       it can correctly detect whether the scope was exited successfully or due to an exception.
 */
template <typename FuncType>
class NEX_API OnScopeSuccess {
public:
    // Constructor: Takes a callable object (e.g., lambda) to be called on scope exit 
    // if no exceptions were thrown
    explicit OnScopeSuccess(FuncType f)
        : func_(NEX_STD move(f))
        , exceptionsAtConstruction_(NEX_STD uncaught_exceptions()) {}

    // Destructor: Calls the stored callable object if no exceptions were thrown 
    // during the lifetime of this object
    ~OnScopeSuccess() noexcept {
        // Only call the function if no exceptions were thrown during the lifetime of this object, 
        // indicating that the scope was exited successfully without an exception
        if (NEX_STD uncaught_exceptions() == exceptionsAtConstruction_ && !dismissed_)
            func_();
    }

    // Dismiss the scope guard, preventing the callable from being called on scope exit
    void dismiss() noexcept { dismissed_ = true; }

    // Disable copy semantics for OnScopeSuccess to prevent accidental copying
    // which could lead to multiple calls of the callable object
    NEX_DISALLOW_COPY(OnScopeSuccess);

    // Enable move semantics for OnScopeSuccess to allow transferring ownership of the callable object
    OnScopeSuccess(OnScopeSuccess&& other) noexcept
        : func_(NEX_STD move(other.func_))
        , exceptionsAtConstruction_(other.exceptionsAtConstruction_)
        , dismissed_(other.dismissed_)
    {
        other.dismissed_ = true;
    }
    OnScopeSuccess& operator=(OnScopeSuccess&& rhs) noexcept {
        if (this != &rhs) {
            if (NEX_STD uncaught_exceptions() == exceptionsAtConstruction_ && !dismissed_)
                func_(); // Call the current callable if not dismissed and no exceptions were thrown
            func_ = NEX_STD move(rhs.func_);
            exceptionsAtConstruction_ = rhs.exceptionsAtConstruction_;
            dismissed_ = rhs.dismissed_;
            rhs.dismissed_ = true; // Prevent the moved-from object from calling the callable
        }
        return *this;
    }

private:
    FuncType func_;
    int      exceptionsAtConstruction_;
    bool     dismissed_ = false;
};

/**
 * @class   OnScopeFailure
 * @brief   Scope guard that only executes the callable if the scope is exited due to an exception
 * 
 * @details
 * This template class provides a convenient way to execute a callable object (e.g., a lambda function) 
 * when the scope is exited due to an exception. The callable object is executed in the destructor of the 
 * OnScopeFailure, following the RAII pattern. The destructor checks if any exceptions were thrown during 
 * the lifetime of the OnScopeFailure object, and only calls the callable if an exception was thrown, 
 * allowing for conditional execution of the cleanup code based on whether the scope was exited successfully 
 * or due to an exception.
 * 
 * @tparam FuncType The type of the callable object (e.g., lambda) to be executed on exceptional scope exit.
 * 
 * @note The OnScopeFailure object must have a shorter lifetime than the scope it is guarding to ensure that 
 *       it can correctly detect whether the scope was exited successfully or due to an exception.
 */
template <typename FuncType>
class NEX_API OnScopeFailure {
public:
    // Constructor: Takes a callable object (e.g., lambda) to be called on scope exit if an exception was thrown
    explicit OnScopeFailure(FuncType f)
        : func_(NEX_STD move(f))
        , exceptionsAtConstruction_(NEX_STD uncaught_exceptions()) {}

    // Destructor: Calls the stored callable object if an exception was thrown during the lifetime of this object
    ~OnScopeFailure() noexcept {
        // Only call the function if an exception was thrown during the lifetime of this object,
        // indicating that the scope was exited due to an exception
        if (NEX_STD uncaught_exceptions() > exceptionsAtConstruction_ && !dismissed_)
            func_();
    }

    // Dismiss the scope guard, preventing the callable from being called on scope exit
    void dismiss() noexcept { dismissed_ = true; }

    // Disable copy semantics for OnScopeFailure to prevent accidental copying
    // which could lead to multiple calls of the callable object
    NEX_DISALLOW_COPY(OnScopeFailure);

    // Enable move semantics for OnScopeFailure to allow transferring ownership of the callable object
    OnScopeFailure(OnScopeFailure&& other) noexcept
        : func_(NEX_STD move(other.func_))
        , exceptionsAtConstruction_(other.exceptionsAtConstruction_)
        , dismissed_(other.dismissed_)
    {
        other.dismissed_ = true;
    }
    OnScopeFailure& operator=(OnScopeFailure&& rhs) noexcept {
        if (this != &rhs) {
            if (NEX_STD uncaught_exceptions() > exceptionsAtConstruction_ && !dismissed_)
                func_(); // Call the current callable if not dismissed and an exception was thrown
            func_ = NEX_STD move(rhs.func_);
            exceptionsAtConstruction_ = rhs.exceptionsAtConstruction_;
            dismissed_ = rhs.dismissed_;
            rhs.dismissed_ = true; // Prevent the moved-from object from calling the callable
        }
        return *this;
    }

private:
    FuncType func_;
    int      exceptionsAtConstruction_;
    bool     dismissed_ = false;
};

// Helper function for quickly creating a ScopeGuard with type deduction for the callable object, 
// similar to fbfolly::makeGuard and llvm::make_scope_exit, allowing for convenient creation of ScopeGuard objects 
// without having to explicitly specify the type of the callable object.
template <typename FuncType>
NEX_NODISCARD auto makeScopeGuard(FuncType&& f) {
    return ScopeGuard<Decay<FuncType>>(NEX_STD forward<FuncType>(f));
}

// DEFER macro for scope-based excution of code blocks, similar to the DEFER statement in languages like Go
#define NEX_DEFER(...) \
    auto ANONYMOUS_DEFER_##__LINE__ = \
        NEX_PREPEND_NAMESPACE(utility::ScopeGuard)([&]() { __VA_ARGS__; })

// SCOPE_EXIT macro for scope-based excution of code blocks, similar to the SCOPE_EXIT statement 
// in libraries like Boost.Scope or llvm::scope_exit or std::scope_exit in C++23
#define NEX_SCOPE_EXIT(...) \
    auto ANONYMOUS_SCOPE_EXIT_##__LINE__ = \
        NEX_PREPEND_NAMESPACE(utility::ScopeGuard)([&]() { __VA_ARGS__; })

// SCOPE_SUCCESS macro for scope-based excution of code blocks only if the scope is exited successfully 
// without an exception, similar to the SCOPE_SUCCESS statement in libraries like Boost.Scope or Folly, 
// or languages like D with its scope(success) statement
#define NEX_SCOPE_SUCCESS(...) \
    auto ANONYMOUS_SCOPE_SUCCESS_##__LINE__ = \
        NEX_PREPEND_NAMESPACE(utility::OnScopeSuccess)([&]() { __VA_ARGS__; })

// SCOPE_FAILURE macro for scope-based excution of code blocks only if the scope is exited due to an exception,
// similar to the SCOPE_FAILURE statement in libraries like Boost.Scope or Folly, or languages like D with 
// its scope(failure) statement
#define NEX_SCOPE_FAILURE(...) \
    auto ANONYMOUS_SCOPE_FAILURE_##__LINE__ = \
        NEX_PREPEND_NAMESPACE(utility::OnScopeFailure)([&]() { __VA_ARGS__; })

/**
 * @struct SourceLocation
 * @brief A struct representing a source code location (file, line, function)
 * 
 * This struct is used to capture and represent a specific location in the source code, including 
 * the file name, line number, and function name. It can be used for logging, error reporting, 
 * debugging, and other purposes where information about the source location is valuable.
 */
struct SourceLocation {
    const char* file;       // The source file name where the SourceLocation was created
    int line;               // The line number in the source file where the SourceLocation was created
    const char* function;   // The function name where the SourceLocation was created

    // Default constructor: initializes members to default values
    constexpr SourceLocation() noexcept
        : file(""), line(0), function("") {}

    // Constructs a SourceLocation with the given file path, line number, and function name
    constexpr SourceLocation(const char* filePath, int lineNumber, const char* functionName) noexcept
        : file(filePath), line(lineNumber), function(functionName) {}

#if NEX_USE_STD_SOURCE_LOCATION
    // Create a SourceLocation object representing the current source location using compiler built-ins
    static consteval SourceLocation current(NEX_STD source_location loc = NEX_STD source_location::current()) {
        return { 
            stripFilePath(loc.file_name()), 
            static_cast<int>(loc.line()), 
            loc.function_name() 
        };
    }
#else
    // Create a SourceLocation object representing the current source location using compiler built-ins
    static consteval SourceLocation current() {
        return { 
            stripFilePath(NEX_SOURCE_FILE_PATH), 
            NEX_SOURCE_LINE_NUMBER, 
            NEX_SOURCE_FUNCTION_NAME 
        };
    }
#endif
};

// Define macro for capturing the current source location, which can be used for logging, error reporting, 
// debugging, and other purposes where information about the source location is valuable.
#define NEX_SOURCE_LOCATION \
    NEX_PREPEND_NAMESPACE(utility::SourceLocation::current())

/**
 * @class Comparable
 * @brief Mix-in class that provides a full set of comparison operators
 * 
 * This template class is a mix-in that provides a complete set of comparison
 * operators (==, !=, <, <=, >, >=) based on a single compare() method.
 * 
 * By inheriting publicly from Comparable and implementing the pure virtual
 * compare() method, a subclass automatically gains all comparison operators,
 * as they are all implemented in terms of compare().
 * 
 * Comparable supports:
 * - Equality operators (==, !=)
 * - Relational operators (<, <=, >, >=)
 * - Single method implementation (only compare() needs to be implemented)
 * - CRTP pattern (Curiously Recurring Template Pattern)
 * 
 * @note The destructor is protected and virtual to prevent deletion through
 *       base class pointers, as this class is designed as a mix-in, not for
 *       polymorphism.
 * 
 * @note This class uses the CRTP pattern where Type is the derived class type.
 * 
 * Example usage:
 * @code
 * class MyClass : public Comparable<MyClass> {
 * protected:
 *     int compare(const MyClass& other) const override {
 *         if (value < other.value) return -1;
 *         if (value > other.value) return 1;
 *         return 0;
 *     }
 * public:
 *     int value;
 * };
 * 
 * MyClass a{5}, b{10};
 * if (a < b) {  // Works automatically!
 *     // ...
 * }
 * @endcode
 */
template <class Type>
class NEX_API Comparable {
public:
    // Equality operator: returns true if objects are equal
    bool operator==(const Type& other) const {
        return !compare(other);
    }

    // Inequality operator: returns true if objects are not equal
    bool operator!=(const Type& other) const {
        return compare(other);
    }

    // Less-than operator: returns true if this object is less than other
    bool operator<(const Type& other) const {
        return compare(other) < 0;
    }

    // Less-than-or-equal operator: returns true if this object is less than or equal to other
    bool operator<=(const Type& other) const {
        return compare(other) <= 0;
    }

    // Greater-than operator: returns true if this object is greater than other
    bool operator>(const Type& other) const {
        return compare(other) > 0;
    }

    // Greater-than-or-equal operator: returns true if this object is greater than or equal to other
    bool operator>=(const Type& other) const {
        return compare(other) >= 0;
    }

protected:
    // Destructor: protected and virtual to prevent deletion through base class pointers
    // This class is designed as a mix-in, not for polymorphism
    virtual ~Comparable() = default;

    // Compare this object to another of the same type
    // Returns -1 if this object is "before" the other,
    //         0 if they are equal,
    //         1 if this object is "after" the other
    virtual int compare(const Type& other) const = 0;
};

/**
 * @struct NonCopyable
 * @brief A base class that disables copy semantics for derived classes
 */
struct NonCopyable {
    NonCopyable() = default;
    NEX_DISALLOW_COPY(NonCopyable);
};

/**
 * @struct NonMovable
 * @brief A base class that disables move semantics for derived classes
 */
struct NonMovable {
    NonMovable() = default;
    NEX_DISALLOW_MOVE(NonMovable);
};

/**
 * @struct Immobile
 * @brief A base class that disables both copy and move semantics for derived classes
 */
struct Immobile {
    Immobile() = default;
    NEX_DISALLOW_COPY(Immobile);
    NEX_DISALLOW_MOVE(Immobile);
};

/**
 * @class Singleton
 * @brief A template for implementing the singleton design pattern
 * 
 * This template class provides a convenient way to implement the singleton design pattern, which ensures that 
 * a class has only one instance and provides a global point of access to it. By inheriting from Singleton and 
 * specifying the derived class type, a class can easily become a singleton with thread-safe lazy initialization.
 * 
 * @note
 * The derived class must have a default constructor (which can be private or protected) for the singleton 
 * instance to be created.
 * The singleton instance is created on first access in a thread-safe manner, and will be destroyed automatically 
 * when the program exits.
 * 
 * Example usage:
 * @code
 * class MySingleton : public Singleton<MySingleton> {
 *     friend class Singleton<MySingleton>; // Allow Singleton to access the private constructor
 * private:
 *     MySingleton() = default; // Private constructor to prevent direct instantiation
 * public:
 *     void doSomething() {
 *         // ...
 *     }
 * };
 * 
 * // Access the singleton instance and call a method
 * MySingleton::instance().doSomething();
 * @endcode
 */
template <typename Derived>
class NEX_API Singleton : private NonCopyable, private NonMovable {
protected:
    // Protected constructor to prevent direct instantiation while allowing construction by derived classes. 
    // The default constructor is sufficient for most cases, but can be customized if needed by the derived class.
    Singleton() = default;

public:
    // Get the singleton instance of the derived class
    // Using Meyers' singleton pattern, which is thread-safe in C++11 and later
    static Derived& instance() {
        static Derived instance;
        return instance;
    }
};

/**
 * @class NamedType
 * @brief A template for creating strong typedefs with unique types and no implicit conversions
 * 
 * This template class provides a convenient way to create strong typedefs, which are distinct types 
 * that wrap an underlying type (e.g., int, std::string) and provide type safety without implicit conversions. 
 * By specifying a unique Tag type for each NamedType, you can create multiple strong typedefs that wrap 
 * the same underlying type but are treated as distinct types by the compiler, preventing accidental misuse 
 * and improving code clarity.
 * 
 * @tparam Type The underlying type that the NamedType will wrap (e.g., int, std::string)
 * @tparam Tag A unique type used to distinguish this NamedType from other NamedTypes that wrap 
 *         the same underlying type. This can be an empty struct or any unique type.
 * 
 * @note
 * The NamedType class provides explicit constructors and accessors to prevent implicit conversions,
 * ensuring that the strong typedef is used correctly and intentionally. Comparison operators are disabled
 * between different NamedType instances to prevent accidental misuse, but can be implemented if desired.
 * 
 * Example usage:
 * @code
 * using Width = utility::NamedType<int, struct WidthTag>;
 * using Height = utility::NamedType<int, struct HeightTag>;
 * 
 * void setDimensions(Width width, Height height) {
 *     // ...
 * }
 * setDimensions(Width(800), Height(600)); // Works, and prevents accidental misuse
 * setDimensions(Width(800), Width(600)); // Compile-time error, prevents misuse
 * @endcode
 */
template <typename Type, typename Tag>
class NamedType : private NonCopyable, private NonMovable {
public:
    using UnderlyingType = Type;

    // Construction
    explicit NamedType(const Type& value) : value_(value) {}
    explicit NamedType(Type&& value) noexcept(NEX_STD is_nothrow_move_constructible_v<Type>)
        : value_(NEX_STD move(value)) {}

    // Accessors
    NEX_NODISCARD const Type& get() const noexcept { return value_; }
    NEX_NODISCARD Type& get() noexcept { return value_; }

    // Explicit conversion
    explicit operator const Type&() const noexcept { return value_; }
    explicit operator Type&() noexcept { return value_; }

    // Comparison (if desired)
    // By default, disable comparison between different NamedType instances to prevent accidental misuse
    template <typename OtherTag>
    bool operator==(const NamedType<Type, OtherTag>&) const = delete;

private:
    Type value_;
};

NEX_SUBNAMESPACE_END(utility)

NEX_NAMESPACE_END
