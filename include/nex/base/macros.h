/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/platform.h"
#include "nex/base/architecture.h"
#include "nex/base/compiler.h"
#include "nex/base/attributes.h"
#include "nex/base/build.h"
#include "nex/base/export.h"
#include "nex/base/namespace.h"

// Undefine min/max macros if defined 
// (to avoid conflicts with std::min/std::max)
#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

/**
 * @section Copy and Move Semantics Macros
 * @brief   Defines macros for default and deleted copy and move constructors and assignment operators for classes.
 * 
 * @details
 * This section defines macros for default and deleted copy and move constructors and assignment operators for classes.
 * The macros can be used to easily specify the copy and move semantics of a class, either by defaulting them or by 
 * deleting them.
 * The macros can be used in the class declaration to specify the copy and move semantics, and they can also be used 
 * in the class definition to provide the default implementations.
 * 
 * @note When using the NEX_DISALLOW_COPY, NEX_DISALLOW_MOVE, or NEX_DISALLOW_COPY_AND_MOVE macros, 
 *       it is recommended to use them in a way that clearly indicates the class being affected, to avoid confusion 
 *       and to improve readability.
 */

// Default copy semantics for a class (copy constructor and copy assignment operator)
#define NEX_DEFAULT_COPY(TypeName) \
    TypeName(const TypeName&) = default; \
    TypeName& operator=(const TypeName&) = default;

// Default move semantics for a class (move constructor and move assignment operator)
#define NEX_DEFAULT_MOVE(TypeName) \
    TypeName(TypeName&&) noexcept = default; \
    TypeName& operator=(TypeName&&) noexcept = default;

// Default copy and move semantics for a class
#define NEX_DEFAULT_COPY_AND_MOVE(TypeName) \
    NEX_DEFAULT_COPY(TypeName) \
    NEX_DEFAULT_MOVE(TypeName)

// Default copy semantics for a class (declaration only)
#define NEX_DECLARE_DEFAULT_COPY(TypeName) \
    TypeName(const TypeName&); \
    TypeName& operator=(const TypeName&);

// Default move semantics for a class (declaration only)
#define NEX_DECLARE_DEFAULT_MOVE(TypeName) \
    TypeName(TypeName&&) noexcept; \
    TypeName& operator=(TypeName&&) noexcept;

// Default copy and move semantics for a class (declaration only)
#define NEX_DECLARE_DEFAULT_COPY_AND_MOVE(TypeName) \
    NEX_DECLARE_DEFAULT_COPY(TypeName) \
    NEX_DECLARE_DEFAULT_MOVE(TypeName)

// Default copy semantics for a class (definition)
#define NEX_DEFINE_DEFAULT_COPY(TypeName) \
    TypeName::TypeName(const TypeName&) = default; \
    TypeName& TypeName::operator=(const TypeName&) = default;

// Default move semantics for a class (definition)
#define NEX_DEFINE_DEFAULT_MOVE(TypeName) \
    TypeName::TypeName(TypeName&&) noexcept = default; \
    TypeName& TypeName::operator=(TypeName&&) noexcept = default;

// Default copy and move semantics for a class (definition)
#define NEX_DEFINE_DEFAULT_COPY_AND_MOVE(TypeName) \
    NEX_DEFINE_DEFAULT_COPY(TypeName) \
    NEX_DEFINE_DEFAULT_MOVE(TypeName)

// Disallow copy semantics for a class
#define NEX_DISALLOW_COPY(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete;

// Disallow move semantics for a class
#define NEX_DISALLOW_MOVE(TypeName) \
    TypeName(TypeName&&) = delete; \
    TypeName& operator=(TypeName&&) = delete;

// Disallow copy and move semantics for a class
#define NEX_DISALLOW_COPY_AND_MOVE(TypeName) \
    NEX_DISALLOW_COPY(TypeName) \
    NEX_DISALLOW_MOVE(TypeName)

/**
 * @section Destructor Macros
 * @brief Defines macros for virtual default and deleted destructors for classes.
 * 
 * @details
 * This section defines macros for virtual default and deleted destructors for classes.
 * The macros can be used to easily specify the destructor semantics of a class, either by defaulting it or by deleting it.
 * The macros can be used in the class declaration to specify the destructor semantics, and they can also be used in the 
 * class definition to provide the default implementation.
 */

// Default destructor for a class
#define NEX_DEFAULT_DTOR(TypeName) \
    ~TypeName() = default;

// Deleted destructor for a class
#define NEX_DELETE_DTOR(TypeName) \
    ~TypeName() = delete;

// Virtual default destructor for a class
#define NEX_VIRTUAL_DEFAULT_DTOR(TypeName) \
    virtual ~TypeName() = default;

// Virtual deleted destructor for a class
#define NEX_VIRTUAL_DELETE_DTOR(TypeName) \
    virtual ~TypeName() = delete;

// Override default destructor for a class
#define NEX_OVERRIDE_DEFAULT_DTOR(TypeName) \
    ~TypeName() override = default;

// Default destructor for a class (declaration only)
#define NEX_DECLARE_DEFAULT_DTOR(TypeName) \
    ~TypeName();

// Virtual default destructor for a class (declaration only)
#define NEX_DECLARE_VIRTUAL_DTOR(TypeName) \
    virtual ~TypeName();

// Override default destructor for a class (declaration only)
#define NEX_DECLARE_OVERRIDE_DTOR(TypeName) \
    ~TypeName() override;

// Default destructor for a class (definition)
#define NEX_DEFINE_DEFAULT_DTOR(TypeName) \
    TypeName::~TypeName() = default;

/**
 * @section Utility Macros
 * @brief Defines utility macros for common operations such as disallowing copy/move semantics, 
 *        getting the size of an array, and getting the offset of a member within a struct/class.
 * 
 * @details
 * This section defines utility macros for common operations such as getting the size of a statically sized array, 
 * getting the offset of a member within a struct/class, and getting the type of a struct/class member.
 * The macros can be used throughout the codebase to simplify common operations and to improve code 
 * readability and maintainability.
 */

// Get the number of elements in a statically sized array
#define NEX_SIZEOF(arr) \
    (sizeof(arr) / sizeof((arr)[0]))

// Get the offset of a member within a struct/class
#define NEX_OFFSET_OF(type, member) \
    offsetOf(type, member)

// Get the size of a struct/class member
#define NEX_MEMBER_SIZE(type, member) \
    sizeof(((type*)0)->member)

// Get the type of a struct/class member
#define NEX_MEMBER_TYPE(type, member) \
    decltype(((type*)0)->member)

// Get the type of a struct/class member as a pointer
#define NEX_MEMBER_POINTER_TYPE(type, member) \
    decltype(&((type*)0)->member)

// Get the type of a struct/class member as a reference
#define NEX_MEMBER_REFERENCE_TYPE(type, member) \
    decltype(((type*)0)->member)

// Get the type of a struct/class member as a const pointer
#define NEX_MEMBER_CONST_POINTER_TYPE(type, member) \
    decltype(&((const type*)0)->member)

// Get the type of a struct/class member as a const reference
#define NEX_MEMBER_CONST_REFERENCE_TYPE(type, member) \
    decltype(((const type*)0)->member)

// Get the type of a struct/class member as an rvalue reference
#define NEX_MEMBER_RVALUE_REFERENCE_TYPE(type, member) \
    decltype(((type*)0)->member)&&

// Get the type of a struct/class member as a const rvalue reference
#define NEX_MEMBER_CONST_RVALUE_REFERENCE_TYPE(type, member) \
    decltype(((const type*)0)->member)&&

// Safe delete of a pointer to an object and set the pointer to nullptr to avoid dangling pointers
#define NEX_DELETE_OBJECT(objPtr) \
    do { \
        if (objPtr) { \
            delete (objPtr); \
            (objPtr) = nullptr; \
        } \
    } while (0)

// Safe delete of a pointer to an array and set the pointer to nullptr to avoid dangling pointers
#define NEX_DELETE_ARRAY(arrPtr) \
    do { \
        if (arrPtr) { \
            delete[] (arrPtr); \
            (arrPtr) = nullptr; \
        } \
    } while (0)

// Safe delete of a pointer to a member to delete the containing struct/class
#define NEX_DELETE_MEMBER(ptr, type, member) \
    delete NEX_DOWNCAST_MEMBER(ptr, type, member)

// Safe delete of a pointer to a member to delete the containing struct/class and set the pointer to nullptr
#define NEX_DELETE_MEMBER_AND_NULL(ptr, type, member) \
    do { \
        NEX_DELETE_MEMBER(ptr, type, member); \
        (ptr) = nullptr; \
    } while (0)

// Safe delete of a pointer to a member to delete the containing struct/class 
// and set the pointer to nullptr (for arrays)
#define NEX_DELETE_MEMBER_AND_NULL_ARRAY(ptr, type, member) \
    do { \
        delete[] NEX_DOWNCAST_MEMBER(ptr, type, member); \
        (ptr) = nullptr; \
    } while (0)

// Safe delete of a pointer to a member to delete the containing struct/class 
// and set the pointer to nullptr (for single objects)
#define NEX_DELETE_MEMBER_AND_NULL_SINGLE(ptr, type, member) \
    do { \
        delete NEX_DOWNCAST_MEMBER(ptr, type, member); \
        (ptr) = nullptr; \
    } while (0)
