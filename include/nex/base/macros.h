/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @section Namespace Macros
 * @brief Defines macros for the NexSuite global namespace and its sub-namespaces (infrastructure, platform, services, UI).
 * 
 * @details 
 * This section defines macros for the NexSuite global namespace and its sub-namespaces (infrastructure, platform, services, UI). 
 * The macros include the namespace declaration, the namespace end, and a using directive for each namespace.
 * The macros can be used throughout the codebase to ensure consistent namespace usage and to simplify the declaration 
 * of namespaces and using directives.
 * 
 * @note When using the NEXSUITE_PREPEND_NAMESPACE macro, it is recommended to use it in a way that clearly indicates 
 * the namespace being prepended, to avoid confusion and to improve readability.
 */

////// NexSuite global namespace macros =========================================================

#define NEXSUITE_NAMESPACE                       nexsuite
#define NEXSUITE_NAMESPACE_BEGIN                 namespace NEXSUITE_NAMESPACE {
#define NEXSUITE_NAMESPACE_END                   } // namespace nexsuite
#define NEXSUITE_USING_NAMESPACE                 using namespace NEXSUITE_NAMESPACE;
#define NEXSUITE_PREPEND_NAMESPACE(name)         ::NEXSUITE_NAMESPACE::name

/** 
 * @brief Using global ::std:: namespace
 * @note Do not expose std:: anywhere in the public API of NexSuite. 
 *       Only use std:: in the implementation files and private members of classes.
 *       This is to avoid potential conflicts with other libraries that may be used by the users of NexSuite, 
 *       and to keep the public API clean and consistent.
 */
#define NEXSUITE_STD                                      ::std::
#define NEXSUITE_STD_BEGIN                                namespace ::std {
#define NEXSUITE_STD_END                                  } // namespace ::std
#define NEXSUITE_STD_USING_NAMESPACE                      using namespace ::std;

////// NexSuite layer namespace macros =========================================================

#define NEXSUITE_LAYER_NAMESPACE_BEGIN(layer)             \
    NEXSUITE_NAMESPACE_BEGIN                              \
    namespace layer {
#define NEXSUITE_LAYER_NAMESPACE_END(layer)               \
    }                                                     \
    NEXSUITE_NAMESPACE_END
#define NEXSUITE_USING_LAYER_NAMESPACE(layer)             \
    NEXSUITE_USING_NAMESPACE                              \
    using namespace NEXSUITE_PREPEND_NAMESPACE(layer);
#define NEXSUITE_PREPEND_LAYER_NAMESPACE(layer, name)     \
    NEXSUITE_PREPEND_NAMESPACE(layer)::name

/**
 * @brief NexSuite infrastructure layer namespace macros
 * 
 * @details 
 * The infrastructure layer includes components that provide core functionality and services to the rest of 
 * the NexSuite system, such as configuration management, logging, error handling, and utility functions.
 * The macros defined in this section can be used to declare the infrastructure namespace and to use it in 
 * the implementation files and public API of the infrastructure layer.
 * 
 * @note When using the NEXSUITE_INFRA_NAMESPACE macros, it is recommended to use them in a way that clearly 
 *       indicates the namespace being used, to avoid confusion and to improve readability.
 */
#define NEXSUITE_INFRA_NAMESPACE_BEGIN                    \
    NEXSUITE_LAYER_NAMESPACE_BEGIN(infra)
#define NEXSUITE_INFRA_NAMESPACE_END                      \
    NEXSUITE_LAYER_NAMESPACE_END(infra)
#define NEXSUITE_USING_INFRA_NAMESPACE                    \
    NEXSUITE_USING_LAYER_NAMESPACE(infra)
#define NEXSUITE_PREPEND_INFRA_NAMESPACE(name)            \
    NEXSUITE_PREPEND_LAYER_NAMESPACE(infra, name)

/**
 * @brief NexSuite platform layer namespace macros
 * 
 * @details
 * The platform layer includes components that provide platform-specific implementations and abstractions 
 * for the rest of the NexSuite system, such as file system access, registry access, process management, 
 * and system information retrieval.
 * The macros defined in this section can be used to declare the platform namespace and to use it in the 
 * implementation files and public API of the platform layer.
 * 
 * @note When using the NEXSUITE_PLATFORM_NAMESPACE macros, it is recommended to use them in a way that clearly 
 *       indicates the namespace being used, to avoid confusion and to improve readability.
 *       NexSuite only supports Windows, Linux, and macOS for now. Other platforms will result in a compilation error.
 */
#define NEXSUITE_PLATFORM_NAMESPACE_BEGIN                 \
    NEXSUITE_LAYER_NAMESPACE_BEGIN(platform)
#define NEXSUITE_PLATFORM_NAMESPACE_END                   \
    NEXSUITE_LAYER_NAMESPACE_END(platform)
#define NEXSUITE_USING_PLATFORM_NAMESPACE                 \
    NEXSUITE_USING_LAYER_NAMESPACE(platform)
#define NEXSUITE_PREPEND_PLATFORM_NAMESPACE(name)         \
    NEXSUITE_PREPEND_LAYER_NAMESPACE(platform, name)

/**
 * @brief NexSuite services layer namespace macros
 * 
 * @details
 * The services layer includes components that provide higher-level services and functionality to the rest of 
 * the NexSuite system, such as power management, network management, scheduler, and other system services.
 * The macros defined in this section can be used to declare the services namespace and to use it in the 
 * implementation files and public API of the services layer.
 * 
 * @note When using the NEXSUITE_SERVICES_NAMESPACE macros, it is recommended to use them in a way that clearly 
 *       indicates the namespace being used, to avoid confusion and to improve readability.
 */
#define NEXSUITE_SERVICES_NAMESPACE_BEGIN                 \
    NEXSUITE_LAYER_NAMESPACE_BEGIN(services)
#define NEXSUITE_SERVICES_NAMESPACE_END                   \
    NEXSUITE_LAYER_NAMESPACE_END(services)
#define NEXSUITE_USING_SERVICES_NAMESPACE                 \
    NEXSUITE_USING_LAYER_NAMESPACE(services)
#define NEXSUITE_PREPEND_SERVICES_NAMESPACE(name)         \
    NEXSUITE_PREPEND_LAYER_NAMESPACE(services, name)

/**
 * @brief NexSuite UI layer namespace macros
 * 
 * @details
 * The UI layer includes components that provide user interface elements and interactions for the rest of the
 * NexSuite system, such as windows, dialogs, notifications, and other UI components.
 * The macros defined in this section can be used to declare the UI namespace and to use it in the implementation 
 * files and public API of the UI layer.
 * 
 * @note When using the NEXSUITE_UI_NAMESPACE macros, it is recommended to use them in a way that clearly 
 *       indicates the namespace being used, to avoid confusion and to improve readability.
 */
#define NEXSUITE_UI_NAMESPACE_BEGIN                       \
    NEXSUITE_LAYER_NAMESPACE_BEGIN(ui)
#define NEXSUITE_UI_NAMESPACE_END                         \
    NEXSUITE_LAYER_NAMESPACE_END(ui)
#define NEXSUITE_USING_UI_NAMESPACE                       \
    NEXSUITE_USING_LAYER_NAMESPACE(ui)
#define NEXSUITE_PREPEND_UI_NAMESPACE(name)               \
    NEXSUITE_PREPEND_LAYER_NAMESPACE(ui, name)

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
 * @note When using the NEXSUITE_DISALLOW_COPY, NEXSUITE_DISALLOW_MOVE, or NEXSUITE_DISALLOW_COPY_AND_MOVE macros, 
 *       it is recommended to use them in a way that clearly indicates the class being affected, to avoid confusion 
 *       and to improve readability.
 */

// Default copy semantics for a class (copy constructor and copy assignment operator)
#define NEXSUITE_DEFAULT_COPY(TypeName) \
    TypeName(const TypeName&) = default; \
    TypeName& operator=(const TypeName&) = default;

// Default move semantics for a class (move constructor and move assignment operator)
#define NEXSUITE_DEFAULT_MOVE(TypeName) \
    TypeName(TypeName&&) noexcept = default; \
    TypeName& operator=(TypeName&&) noexcept = default;

// Default copy and move semantics for a class
#define NEXSUITE_DEFAULT_COPY_AND_MOVE(TypeName) \
    NEXSUITE_DEFAULT_COPY(TypeName) \
    NEXSUITE_DEFAULT_MOVE(TypeName)

// Default copy semantics for a class (declaration only)
#define NEXSUITE_DECLARE_DEFAULT_COPY(TypeName) \
    TypeName(const TypeName&); \
    TypeName& operator=(const TypeName&);

// Default move semantics for a class (declaration only)
#define NEXSUITE_DECLARE_DEFAULT_MOVE(TypeName) \
    TypeName(TypeName&&) noexcept; \
    TypeName& operator=(TypeName&&) noexcept;

// Default copy and move semantics for a class (declaration only)
#define NEXSUITE_DECLARE_DEFAULT_COPY_AND_MOVE(TypeName) \
    NEXSUITE_DECLARE_DEFAULT_COPY(TypeName) \
    NEXSUITE_DECLARE_DEFAULT_MOVE(TypeName)

// Default copy semantics for a class (definition)
#define NEXSUITE_DEFINE_DEFAULT_COPY(TypeName) \
    TypeName::TypeName(const TypeName&) = default; \
    TypeName& TypeName::operator=(const TypeName&) = default;

// Default move semantics for a class (definition)
#define NEXSUITE_DEFINE_DEFAULT_MOVE(TypeName) \
    TypeName::TypeName(TypeName&&) noexcept = default; \
    TypeName& TypeName::operator=(TypeName&&) noexcept = default;

// Default copy and move semantics for a class (definition)
#define NEXSUITE_DEFINE_DEFAULT_COPY_AND_MOVE(TypeName) \
    NEXSUITE_DEFINE_DEFAULT_COPY(TypeName) \
    NEXSUITE_DEFINE_DEFAULT_MOVE(TypeName)

// Disallow copy semantics for a class
#define NEXSUITE_DISALLOW_COPY(TypeName) \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete;

// Disallow move semantics for a class
#define NEXSUITE_DISALLOW_MOVE(TypeName) \
    TypeName(TypeName&&) = delete; \
    TypeName& operator=(TypeName&&) = delete;

// Disallow copy and move semantics for a class
#define NEXSUITE_DISALLOW_COPY_AND_MOVE(TypeName) \
    NEXSUITE_DISALLOW_COPY(TypeName) \
    NEXSUITE_DISALLOW_MOVE(TypeName)

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
#define NEXSUITE_DEFAULT_DTOR(TypeName) \
    ~TypeName() = default;

// Deleted destructor for a class
#define NEXSUITE_DELETE_DTOR(TypeName) \
    ~TypeName() = delete;

// Virtual default destructor for a class
#define NEXSUITE_VIRTUAL_DEFAULT_DTOR(TypeName) \
    virtual ~TypeName() = default;

// Virtual deleted destructor for a class
#define NEXSUITE_VIRTUAL_DELETE_DTOR(TypeName) \
    virtual ~TypeName() = delete;

// Override default destructor for a class
#define NEXSUITE_OVERRIDE_DEFAULT_DTOR(TypeName) \
    ~TypeName() override = default;

// Default destructor for a class (declaration only)
#define NEXSUITE_DECLARE_DEFAULT_DTOR(TypeName) \
    ~TypeName();

// Virtual default destructor for a class (declaration only)
#define NEXSUITE_DECLARE_VIRTUAL_DTOR(TypeName) \
    virtual ~TypeName();

// Override default destructor for a class (declaration only)
#define NEXSUITE_DECLARE_OVERRIDE_DTOR(TypeName) \
    ~TypeName() override;

// Default destructor for a class (definition)
#define NEXSUITE_DEFINE_DEFAULT_DTOR(TypeName) \
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
#define NEXSUITE_SIZEOF(arr) \
    (sizeof(arr) / sizeof((arr)[0]))

// Get the offset of a member within a struct/class
#define NEXSUITE_OFFSET_OF(type, member) \
    offsetof(type, member)

// Get the containing struct/class from a pointer to a member
#define NEXSUITE_CONTAINER_OF(ptr, type, member) \
    ([](auto* __mptr) -> type* { \
        static_assert(NEXSUITE_STD is_same_v<decltype(__mptr), \
            NEXSUITE_MEMBER_POINTER_TYPE(type, member)>, \
            "Pointer type does not match the member type"); \
        return reinterpret_cast<type*>( \
            reinterpret_cast<char*>(__mptr) - NEXSUITE_OFFSET_OF(type, member)); \
    }(ptr))

// Get the size of a struct/class member
#define NEXSUITE_MEMBER_SIZE(type, member) \
    sizeof(((type*)0)->member)

// Get the type of a struct/class member
#define NEXSUITE_MEMBER_TYPE(type, member) \
    decltype(((type*)0)->member)

// Get the type of a struct/class member as a pointer
#define NEXSUITE_MEMBER_POINTER_TYPE(type, member) \
    decltype(&((type*)0)->member)

// Get the type of a struct/class member as a reference
#define NEXSUITE_MEMBER_REFERENCE_TYPE(type, member) \
    decltype(((type*)0)->member)

// Get the type of a struct/class member as a const pointer
#define NEXSUITE_MEMBER_CONST_POINTER_TYPE(type, member) \
    decltype(&((const type*)0)->member)

// Get the type of a struct/class member as a const reference
#define NEXSUITE_MEMBER_CONST_REFERENCE_TYPE(type, member) \
    decltype(((const type*)0)->member)

// Get the type of a struct/class member as an rvalue reference
#define NEXSUITE_MEMBER_RVALUE_REFERENCE_TYPE(type, member) \
    decltype(((type*)0)->member)&&

// Get the type of a struct/class member as a const rvalue reference
#define NEXSUITE_MEMBER_CONST_RVALUE_REFERENCE_TYPE(type, member) \
    decltype(((const type*)0)->member)&&

// Safe downcast of a pointer to a member to a pointer to the containing struct/class
#define NEXSUITE_DOWNCAST_MEMBER(ptr, type, member) \
    ((type*)NEXSUITE_CONTAINER_OF(ptr, type, member))

// Safe deref of a pointer to a member to a reference to the containing struct/class
#define NEXSUITE_DEREF_MEMBER(ptr, type, member) \
    (*NEXSUITE_DOWNCAST_MEMBER(ptr, type, member))

// Safe delete of a pointer to an object and set the pointer to nullptr to avoid dangling pointers
#define NEXSUITE_DELETE_OBJECT(objPtr) \
    do { \
        if (objPtr) { \
            delete (objPtr); \
            (objPtr) = nullptr; \
        } \
    } while (0)

// Safe delete of a pointer to an array and set the pointer to nullptr to avoid dangling pointers
#define NEXSUITE_DELETE_ARRAY(arrPtr) \
    do { \
        if (arrPtr) { \
            delete[] (arrPtr); \
            (arrPtr) = nullptr; \
        } \
    } while (0)

// Safe delete of a pointer to a member to delete the containing struct/class
#define NEXSUITE_DELETE_MEMBER(ptr, type, member) \
    delete NEXSUITE_DOWNCAST_MEMBER(ptr, type, member)

// Safe delete of a pointer to a member to delete the containing struct/class and set the pointer to nullptr
#define NEXSUITE_DELETE_MEMBER_AND_NULL(ptr, type, member) \
    do { \
        NEXSUITE_DELETE_MEMBER(ptr, type, member); \
        (ptr) = nullptr; \
    } while (0)

// Safe delete of a pointer to a member to delete the containing struct/class 
// and set the pointer to nullptr (for arrays)
#define NEXSUITE_DELETE_MEMBER_AND_NULL_ARRAY(ptr, type, member) \
    do { \
        delete[] NEXSUITE_DOWNCAST_MEMBER(ptr, type, member); \
        (ptr) = nullptr; \
    } while (0)

// Safe delete of a pointer to a member to delete the containing struct/class 
// and set the pointer to nullptr (for single objects)
#define NEXSUITE_DELETE_MEMBER_AND_NULL_SINGLE(ptr, type, member) \
    do { \
        delete NEXSUITE_DOWNCAST_MEMBER(ptr, type, member); \
        (ptr) = nullptr; \
    } while (0)
