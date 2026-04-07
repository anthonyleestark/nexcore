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

////// C/C++ name mangling macros =========================================================

#define NEX_MANGLE_JOIN(a, b)                       a##_##b
#define NEX_MANGLE_NAME(a, b)                       NEX_MANGLE_JOIN(a, b)
#define NEX_PREPEND_NAME(a, b)                      a::b

////// Nex-ecosystem global namespace macros =========================================================

#define NEX_NAMESPACE                               nex
#define NEX_NAMESPACE_BEGIN                         namespace NEX_NAMESPACE {
#define NEX_NAMESPACE_END                           } // namespace nex
#define NEX_USING_NAMESPACE                         using namespace NEX_NAMESPACE;
#define NEX_PREPEND_NAMESPACE(name)                 ::NEX_PREPEND_NAME(NEX_NAMESPACE, name)

////// Inline namespace macros =========================================================

#define NEX_INLINE_NAMESPACE_BEGIN(name)            inline namespace name {
#define NEX_INLINE_NAMESPACE_END(name)              } // inline namespace

/** 
 * @brief Using global ::std:: namespace
 * @note Do not expose std:: anywhere in the public API of NexSuite. 
 *       Only use std:: in the implementation files and private members of classes.
 *       This is to avoid potential conflicts with other libraries that may be used by the users of NexSuite, 
 *       and to keep the public API clean and consistent.
 */
#define NEX_STD                                      ::std::
#define NEX_STD_BEGIN                                namespace ::std {
#define NEX_STD_END                                  } // namespace ::std
#define NEX_USING_STD_NAMESPACE                      using namespace ::std;

////// NexSuite layer namespace macros =========================================================

#define NEX_LAYER_NAMESPACE_BEGIN(layer)             \
    NEX_NAMESPACE_BEGIN                              \
    namespace layer {
#define NEX_LAYER_NAMESPACE_END(layer)               \
    }                                                \
    NEX_NAMESPACE_END
#define NEX_USING_LAYER_NAMESPACE(layer)             \
    NEX_USING_NAMESPACE                              \
    using namespace NEX_PREPEND_NAMESPACE(layer);
#define NEX_PREPEND_LAYER_NAMESPACE(layer, name)     \
    NEX_PREPEND_NAMESPACE(layer)::name
