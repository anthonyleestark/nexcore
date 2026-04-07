/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file      namespace.h
 * @defgroup  namespace_macros Namespace Macros
 * @brief     Defines macros for the Nex-ecosystem global namespace and helper macros for namespace usage.
 * 
 * @details 
 * This file defines macros for the Nex-ecosystem global namespace and helper macros for namespace usage.
 * The macros include the namespace declaration, the namespace end, and a using directive for the global namespace 
 * and for each sub-module/layer namespace. The macros can be used throughout the Nex-ecosystem codebases to ensure 
 * consistent namespace usage and to simplify the declaration of namespaces and using directives.
 * 
 * @note When using the NEX_PREPEND_NAMESPACE macro, it is recommended to use it in a way that clearly indicates 
 * the namespace being prepended, to avoid confusion and to improve readability.
 */

/**
 * @section C/C++ name mangling and prepending macros
 * @brief   Define macros for name mangling and prepending namespaces to names, to avoid potential conflicts 
 *          and to improve readability.
 * @note    
 * The NEX_MANGLE_NAME macro is used to create a unique name by concatenating the namespace and the name with an underscore, 
 * while the NEX_PREPEND_NAME macro is used to prepend the namespace to the name using the scope resolution operator (::).
 * The NEX_MANGLE_JOIN macro is a helper macro used by NEX_MANGLE_NAME to concatenate the namespace and the name. 
 * These macros can be used to create unique names for functions, classes, variables, etc., to avoid potential conflicts 
 * with other libraries or with the global namespace, and to improve readability by clearly indicating the namespace to 
 * which the name belongs. It is recommended to use these macros consistently throughout the Nex-ecosystem codebases to 
 * ensure that all names are properly mangled and prepended with the appropriate namespaces, and to avoid potential 
 * conflicts and improve readability.
 */

#define NEX_MANGLE_JOIN(a, b)                                       a##_##b
#define NEX_MANGLE_NAME(a, b)                                       NEX_MANGLE_JOIN(a, b)
#define NEX_PREPEND_NAME(a, b)                                      ::a::b

/**
 * @section Nex-ecosystem foundation namespace macros
 * @brief   Define macros for the Nex-ecosystem global namespace and helper macros for namespace usage
 * @note
 * The NEX_NAMESPACE macro defines the name of the global namespace for the Nex-ecosystem, while the NEX_NAMESPACE_BEGIN 
 * and NEX_NAMESPACE_END macros define the beginning and end of the global namespace, respectively. 
 * The NEX_USING_NAMESPACE macro is a using directive for the global namespace, and the NEX_PREPEND_NAMESPACE macro is 
 * a helper macro used to prepend the global namespace to a name. 
 * These macros can be used throughout the Nex-ecosystem codebases to ensure consistent namespace usage and to simplify 
 * the declaration of namespaces and using directives. It is recommended to use these macros consistently throughout 
 * the Nex-ecosystem codebases to ensure that all namespaces are properly declared and used, and to avoid potential 
 * conflicts and improve readability.
 */

#define NEX_NAMESPACE                                               nex
#define NEX_NAMESPACE_BEGIN                                         namespace NEX_NAMESPACE {
#define NEX_NAMESPACE_END                                           } // namespace nex
#define NEX_USING_NAMESPACE                                         using namespace NEX_NAMESPACE;
#define NEX_PREPEND_NAMESPACE(name)                                 NEX_PREPEND_NAME(NEX_NAMESPACE, name)

/**
 * @section Inline namespace macros
 * @brief   Define inline namespace macros for versioning and internal implementation details
 * @note
 * The NEX_INLINE_NAMESPACE_BEGIN and NEX_INLINE_NAMESPACE_END macros define the beginning and end of an inline 
 * namespace, respectively. 
 * Inline namespaces are used for versioning and for internal implementation details that should not be exposed 
 * in the public API. 
 * By using inline namespaces, we can ensure that the public API remains clean and consistent, while still allowing for 
 * internal changes and improvements without breaking the public API. It is recommended to use these macros consistently 
 * throughout the Nex-ecosystem codebases to ensure that all inline namespaces are properly declared and used, and to 
 * avoid potential conflicts and improve readability.
 */

#define NEX_INLINE_NAMESPACE_BEGIN(name)                            inline namespace name {
#define NEX_INLINE_NAMESPACE_END(name)                              } // inline namespace

/** 
 * @section Using global ::std:: namespace
 * @note Do not expose std:: anywhere in the public API of Nex-ecosystem. 
 *       Only use std:: in the implementation files and private members of classes.
 *       This is to avoid potential conflicts with other libraries that may be used by the users 
 *       of Nex-ecosystem, and to keep the public API clean and consistent.
 */
#define NEX_STD                                                     ::std::
#define NEX_STD_BEGIN                                               namespace ::std {
#define NEX_STD_END                                                 } // namespace ::std
#define NEX_USING_STD_NAMESPACE                                     using namespace ::std;

/**
 * @section Nested namespace macros for convenience
 * @brief   Define macros for nested namespaces to simplify the declaration of nested namespaces
 * @note
 * The NEX_NAMESPACE_BEGIN2 and NEX_NAMESPACE_END2 macros define the beginning and end of a nested namespace with 
 * two levels, while the NEX_NAMESPACE_BEGIN3 and NEX_NAMESPACE_END3 macros define the beginning and end of a nested namespace 
 * with three levels. 
 * These macros can be used to simplify the declaration of nested namespaces, and to improve readability by clearly indicating 
 * the structure of the namespaces. 
 * It is recommended to use these macros consistently throughout the Nex-ecosystem codebases to ensure that all nested namespaces 
 * are properly declared and used, and to avoid potential conflicts and improve readability.
 */

#define NEX_NAMESPACE_BEGIN2(ns1, ns2)                              \
    NEX_NAMESPACE_BEGIN                                             \
        namespace ns1 {                                             \
            namespace ns2 {
#define NEX_NAMESPACE_END2(ns1, ns2)                                \
    } }                                                             \
    NEX_NAMESPACE_END

#define NEX_NAMESPACE_BEGIN3(ns1, ns2, ns3)                         \
    NEX_NAMESPACE_BEGIN                                             \
        namespace ns1 {                                             \
            namespace ns2 {                                         \
                namespace ns3 {
#define NEX_NAMESPACE_END3(ns1, ns2, ns3)                           \
    } } }                                                           \
    NEX_NAMESPACE_END

/**
 * @section Nex-ecosystem foundation layer namespace macros
 * @brief   Define macros for the Nex-ecosystem foundation layer namespaces and helper macros for namespace usage
 * @note
 * The NEX_LAYER_NAMESPACE_BEGIN and NEX_LAYER_NAMESPACE_END macros define the beginning and end of a layer namespace, 
 * respectively. 
 * The NEX_USING_LAYER_NAMESPACE macro is a using directive for a layer namespace, and the NEX_PREPEND_LAYER_NAMESPACE macro 
 * is a helper macro used to prepend a layer namespace to a name. 
 * These macros can be used throughout the Nex-ecosystem codebases to ensure consistent namespace usage and to simplify the 
 * declaration of namespaces and using directives for the foundation layers of the Nex-ecosystem. 
 * It is recommended to use these macros consistently throughout the Nex-ecosystem codebases to ensure that all layer 
 * namespaces are properly declared and used, and to avoid potential conflicts and improve readability.
 */

#define NEX_LAYER_NAMESPACE_BEGIN(layer)                            \
    NEX_NAMESPACE_BEGIN                                             \
    namespace layer {
#define NEX_LAYER_NAMESPACE_END(layer)                              \
    }                                                               \
    NEX_NAMESPACE_END
#define NEX_USING_LAYER_NAMESPACE(layer)                            \
    NEX_USING_NAMESPACE                                             \
    using namespace NEX_PREPEND_NAMESPACE(layer);
#define NEX_PREPEND_LAYER_NAMESPACE(layer, name)                    \
    NEX_PREPEND_NAMESPACE(layer)::name

/**
 * @section Nex-ecosystem project/layer namespace macros
 * @brief   Define macros for project/layer namespaces and helper macros for namespace usage
 * @note
 * The NEX_PROJECT_NAMESPACE_BEGIN and NEX_PROJECT_NAMESPACE_END macros define the beginning and end of a project 
 * namespace, respectively. 
 * The NEX_USING_PROJECT_NAMESPACE macro is a using directive for a project namespace, and the NEX_PREPEND_PROJECT_NAMESPACE 
 * macro is a helper macro used to prepend a project namespace to a name. 
 * The NEX_PROJECT_LAYER_NAMESPACE_BEGIN and NEX_PROJECT_LAYER_NAMESPACE_END macros define the beginning and end of 
 * a project/layer namespace, respectively. 
 * The NEX_USING_PROJECT_LAYER_NAMESPACE macro is a using directive for a project/layer namespace, and the 
 * NEX_PREPEND_PROJECT_LAYER_NAMESPACE macro is a helper macro used to prepend a project/layer namespace to a name. 
 * These macros can be used throughout the Nex-ecosystem codebases to ensure consistent namespace usage and to simplify 
 * the declaration of namespaces and using directives for the projects and layers of the Nex-ecosystem. 
 * It is recommended to use these macros consistently throughout the Nex-ecosystem codebases to ensure that all project 
 * and layer namespaces are properly declared and used, and to avoid potential conflicts and improve readability.
 */

#define NEX_PROJECT_NAMESPACE_BEGIN(project)                        namespace project {
#define NEX_PROJECT_NAMESPACE_END(project)                          } // namespace project
#define NEX_USING_PROJECT_NAMESPACE(project)                        using namespace project
#define NEX_PREPEND_PROJECT_NAMESPACE(project, name)                \
    NEX_PREPEND_NAME(project, name)

#define NEX_PROJECT_LAYER_NAMESPACE_BEGIN(project, layer)           \
    NEX_PROJECT_NAMESPACE_BEGIN(project)                            \
    namespace layer {
#define NEX_PROJECT_LAYER_NAMESPACE_END(project, layer)             \
    }                                                               \
    NEX_PROJECT_NAMESPACE_END(project)
#define NEX_USING_PROJECT_LAYER_NAMESPACE(project, layer)           \
    NEX_USING_PROJECT_NAMESPACE(project)                            \
    using namespace                                                 \
    NEX_PREPEND_PROJECT_NAMESPACE(project, layer);
#define NEX_PREPEND_PROJECT_LAYER_NAMESPACE(project, layer, name)   \
    NEX_PREPEND_PROJECT_NAMESPACE(project, layer)::name
