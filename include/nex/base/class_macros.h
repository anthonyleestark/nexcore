/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file  class_macros.h
 * @brief Provides macros for common class special member function declarations.
 *
 * @details
 * This header defines convenience macros for declaring or deleting common special member functions
 * in C++ classes.
 *
 * It provides reusable definitions for copy and move constructors, copy and move assignment operators,
 * and destructor-related declarations.
 *
 * These macros help reduce repetitive declarations while keeping class semantics explicit and consistent
 * throughout NexCore.
 *
 * @note
 * These macros are intended as declaration conveniences and do not introduce additional runtime behavior.
 */

// =================================================================================
// Defines macros for default and deleted copy and move semantics for classes
// =================================================================================

/**
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

// =================================================================================
// Defines macros for virtual default and deleted destructors for classes
// =================================================================================

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

// Override deleted destructor for a class
#define NEX_OVERRIDE_DELETE_DTOR(TypeName) \
    ~TypeName() override = delete;

// Default destructor for a class (declaration only)
#define NEX_DECLARE_DEFAULT_DTOR(TypeName) \
    ~TypeName();

// Virtual default destructor for a class (declaration only)
#define NEX_DECLARE_VIRTUAL_DTOR(TypeName) \
    virtual ~TypeName();

// Override default destructor for a class (declaration only)
#define NEX_DECLARE_OVERRIDE_DTOR(TypeName) \
    ~TypeName() override;

// Override deleted destructor for a class (declaration only)
#define NEX_DECLARE_OVERRIDE_DELETE_DTOR(TypeName) \
    ~TypeName() override = delete;

// Default destructor for a class (definition)
#define NEX_DEFINE_DEFAULT_DTOR(TypeName) \
    TypeName::~TypeName() = default;

// Virtual default destructor for a class (definition)
#define NEX_DEFINE_VIRTUAL_DTOR(TypeName) \
    TypeName::~TypeName() = default;

// Override default destructor for a class (definition)
#define NEX_DEFINE_OVERRIDE_DTOR(TypeName) \
    TypeName::~TypeName() = default;

// Virtual deleted destructor for a class (definition)
#define NEX_DEFINE_VIRTUAL_DELETE_DTOR(TypeName) \
    TypeName::~TypeName() = delete;

// Override deleted destructor for a class (definition)
#define NEX_DEFINE_OVERRIDE_DELETE_DTOR(TypeName) \
    TypeName::~TypeName() = delete;
