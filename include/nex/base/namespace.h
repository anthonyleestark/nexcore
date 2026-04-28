/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file      namespace.h
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
 * which the name belongs.
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
 * By using inline namespaces, we can ensure that the public API remains clean and consistent, while still allowing 
 * for internal changes and improvements without breaking the public API.
 */

#define NEX_INLINE_NAMESPACE_BEGIN(name)                            inline namespace name {
#define NEX_INLINE_NAMESPACE_END(name)                              } // inline namespace

/**
 * @section Detail namespace macros
 * @brief   Define macros for the detail namespace used for internal implementation details
 * @note
 * The NEX_DETAIL_NAMESPACE_BEGIN and NEX_DETAIL_NAMESPACE_END macros define the beginning and end of the detail 
 * namespace, respectively. 
 * The detail namespace is used for internal implementation details that should not be exposed in the public API. 
 * By using a separate namespace for these details, we can keep the public API clean and consistent, while still 
 * allowing for internal changes and improvements without breaking the public API.
 * The NEX_DETAIL macro is a helper macro used to refer to the detail namespace in a concise way, and it can be used 
 * to access internal implementation details from the public API when necessary.
 * The detail namespace is not intended to be used directly by users of the Nex-ecosystem; it is an implementation
 * detail that supports the functionality of the public API and is subject to change without notice.
 * The detail namespace macros should be used within the range of the Nex-ecosystem namespace to avoid potential 
 * conflicts with other libraries or with the global namespace.
 */

#define NEX_DETAIL                                                  detail::
#define NEX_DETAIL_NAMESPACE_BEGIN                                  namespace detail {
#define NEX_DETAIL_NAMESPACE_END                                    } // namespace detail

/** 
 * @section Using global ::std:: namespace
 * @brief   Define macros for using the global ::std:: namespace in public APIs and implementation files
 * @note 
 * Do not expose std:: anywhere in the public API of Nex-ecosystem. 
 * Only use std:: in the implementation files and private members of classes.
 * This is to avoid potential conflicts with other libraries that may be used by the users 
 * of Nex-ecosystem, and to keep the public API clean and consistent.
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
 * two levels, while the NEX_NAMESPACE_BEGIN3 and NEX_NAMESPACE_END3 macros define the beginning and end of a nested 
 * namespace with three levels.
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
 * The NEX_PROJECT_NAMESPACE_BEGIN and NEX_PROJECT_NAMESPACE_END macros define the start and end of a project namespace. 
 * The NEX_USING_PROJECT_NAMESPACE macro provides a using directive for the project namespace, while the 
 * NEX_PREPEND_PROJECT_NAMESPACE is a helper to prepend the project namespace to a name. 
 * The NEX_PROJECT_LAYER_NAMESPACE_BEGIN and NEX_PROJECT_LAYER_NAMESPACE_END macros define the start and end of a 
 * project/layer namespace.
 * The NEX_USING_PROJECT_LAYER_NAMESPACE and NEX_PREPEND_PROJECT_LAYER_NAMESPACE macros serve similar purposes for 
 * project/layer namespaces.
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

/**
 * @section Cross-layer type importing macros
 * @brief   Define macros for importing types from other layers and projects/layers
 * @note
 * The NEX_IMPORT_TYPE_FROM_LAYER macro is used to import a type from another layer within the same project, while the 
 * NEX_IMPORT_TYPE_FROM_PROJECT_LAYER macro is used to import a type from another project/layer. 
 * These macros help manage dependencies, allowing to use the types directly inside the current scope while respecting 
 * the ownership of the type.
 */

#define NEX_IMPORT_TYPE_FROM_FOUNDATION(type)                                   \
    using NEX_PREPEND_NAMESPACE(type);
#define NEX_IMPORT_TYPE_FROM_LAYER(layer, type)                                 \
    using NEX_PREPEND_LAYER_NAMESPACE(layer, type);
#define NEX_IMPORT_TYPE_FROM_PROJECT_LAYER(project, layer, type)                \
    using NEX_PREPEND_PROJECT_LAYER_NAMESPACE(project, layer, type);

/**
 * @section Cross-layer type aliasing macros
 * @brief   Define macros for aliasing types from other layers and projects/layers
 * @note
 * The NEX_ALIAS_TYPE_FROM_LAYER macro is used to create an alias for a type from another layer within the same project, 
 * while the NEX_ALIAS_TYPE_FROM_PROJECT_LAYER macro is used to create an alias for a type from another project/layer.
 * The NEX_ALIAS_TYPE_AS_FROM_LAYER and NEX_ALIAS_TYPE_AS_FROM_PROJECT_LAYER macros are used to create an alias with a 
 * different name for a type from another layer or project/layer, respectively.
 * These macros help manage dependencies, allowing to use the types directly inside the current scope with a different 
 * name, while behaving as if the current layer is the owner of the type.
 */

#define NEX_ALIAS_TYPE_FROM_FOUNDATION(type)                                    \
    using type = NEX_PREPEND_NAMESPACE(type);
#define NEX_ALIAS_TYPE_FROM_LAYER(layer, type)                                  \
    using type = NEX_PREPEND_LAYER_NAMESPACE(layer, type);
#define NEX_ALIAS_TYPE_FROM_PROJECT_LAYER(project, layer, type)                 \
    using type = NEX_PREPEND_PROJECT_LAYER_NAMESPACE(project, layer, type);

#define NEX_ALIAS_TYPE_AS_FROM_FOUNDATION(alias, type)                          \
    using alias = NEX_PREPEND_NAMESPACE(type);
#define NEX_ALIAS_TYPE_AS_FROM_LAYER(alias, layer, type)                        \
    using alias = NEX_PREPEND_LAYER_NAMESPACE(layer, type);
#define NEX_ALIAS_TYPE_AS_FROM_PROJECT_LAYER(alias, project, layer, type)       \
    using alias = NEX_PREPEND_PROJECT_LAYER_NAMESPACE(project, layer, type);

/**
 * @section Cross-layer namespace aliasing macros
 * @brief   Define macros for aliasing namespaces from other layers and projects/layers
 * @note
 * The NEX_ALIAS_NAMESPACE_FROM_LAYER macro is used to create an alias for a namespace from another layer within 
 * the same project, while the NEX_ALIAS_NAMESPACE_FROM_PROJECT_LAYER macro is used to create an alias for 
 * a namespace from another project/layer.
 * The NEX_ALIAS_NAMESPACE_AS_FROM_LAYER and NEX_ALIAS_NAMESPACE_AS_FROM_PROJECT_LAYER macros are used to create 
 * an alias with a different name for a namespace from another layer or project/layer, respectively.
 * These macros help manage dependencies, allowing to use the namespaces directly inside the current scope with 
 * a different name, while behaving as if the current layer is the owner of the namespace.
 */

#define NEX_ALIAS_NAMESPACE_FROM_FOUNDATION(ns)                                  \
    namespace ns = NEX_PREPEND_NAMESPACE(ns);
#define NEX_ALIAS_NAMESPACE_FROM_LAYER(layer, ns)                                \
    namespace ns = NEX_PREPEND_LAYER_NAMESPACE(layer, ns);
#define NEX_ALIAS_NAMESPACE_FROM_PROJECT_LAYER(project, layer, ns)               \
    namespace ns = NEX_PREPEND_PROJECT_LAYER_NAMESPACE(project, layer, ns);

#define NEX_ALIAS_NAMESPACE_AS_FROM_FOUNDATION(alias, ns)                        \
    namespace alias = NEX_PREPEND_NAMESPACE(ns);
#define NEX_ALIAS_NAMESPACE_AS_FROM_LAYER(alias, layer, ns)                      \
    namespace alias = NEX_PREPEND_LAYER_NAMESPACE(layer, ns);
#define NEX_ALIAS_NAMESPACE_AS_FROM_PROJECT_LAYER(alias, project, layer, ns)     \
    namespace alias = NEX_PREPEND_PROJECT_LAYER_NAMESPACE(project, layer, ns);

/**
 * @section Cross-layer function aliasing macros
 * @brief   Define macros for aliasing functions from other layers and projects/layers
 * @note
 * The NEX_ALIAS_FUNCTION_FROM_LAYER macro is used to create an alias for a function from another layer within 
 * the same project, while the NEX_ALIAS_FUNCTION_FROM_PROJECT_LAYER macro is used to create an alias for 
 * a function from another project/layer.
 * The NEX_ALIAS_FUNCTION_AS_FROM_LAYER and NEX_ALIAS_FUNCTION_AS_FROM_PROJECT_LAYER macros are used to create 
 * an alias with a different name for a function from another layer or project/layer, respectively.
 * These macros help manage dependencies, allowing to use the functions directly inside the current scope with 
 * a different name, while behaving as if the current layer is the owner of the function.
 */

#define NEX_ALIAS_FUNCTION_FROM_LAYER(layer, func)                                \
    using NEX_PREPEND_LAYER_NAMESPACE(layer, func);
#define NEX_ALIAS_FUNCTION_FROM_PROJECT_LAYER(project, layer, func)               \
    using NEX_PREPEND_PROJECT_LAYER_NAMESPACE(project, layer, func);
#define NEX_ALIAS_FUNCTION_AS_FROM_LAYER(alias, layer, func)                      \
    using alias = NEX_PREPEND_LAYER_NAMESPACE(layer, func);
#define NEX_ALIAS_FUNCTION_AS_FROM_PROJECT_LAYER(alias, project, layer, func)     \
    using alias = NEX_PREPEND_PROJECT_LAYER_NAMESPACE(project, layer, func);

/**
 * @section NexCore module/layer namespace macros
 * @brief   Define macros for the NexCore module/layer namespace and helper macros for namespace usage
 */

/**
 * @brief  NexCore's core module/layer namespace macros
 */

#define NEX_CORE_NAMESPACE_BEGIN                                                  \
    NEX_LAYER_NAMESPACE_BEGIN(core)
#define NEX_CORE_NAMESPACE_END                                                    \
    NEX_LAYER_NAMESPACE_END(core)
#define NEX_USING_CORE_NAMESPACE                                                  \
    NEX_USING_LAYER_NAMESPACE(core)
#define NEX_PREPEND_CORE_NAMESPACE(name)                                          \
    NEX_PREPEND_LAYER_NAMESPACE(core, name)

/**
 * @brief  NexCore's infrastructure module/layer namespace macros
 */

#define NEX_INFRA_NAMESPACE_BEGIN                                                  \
    NEX_LAYER_NAMESPACE_BEGIN(infra)
#define NEX_INFRA_NAMESPACE_END                                                    \
    NEX_LAYER_NAMESPACE_END(infra)
#define NEX_USING_INFRA_NAMESPACE                                                  \
    NEX_USING_LAYER_NAMESPACE(infra)
#define NEX_PREPEND_INFRA_NAMESPACE(name)                                          \
    NEX_PREPEND_LAYER_NAMESPACE(infra, name)
