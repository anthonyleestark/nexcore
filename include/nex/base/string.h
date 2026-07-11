/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

/**
 * @file string.h
 * @brief Defines string types used throughout the codebase, including UTF-8, UTF-16, and UTF-32 encoded 
 *        string types, as well as forward declarations of core String types.
 */

#include <string>
#include <string_view>

#include "nex/base/namespace.h"

NEX_NAMESPACE_BEGIN

/**
 * @brief Standard narrow string types.
 * @details
 * The std::string is an alias for std::basic_string<char>, acting as a container for 8-bit narrow characters. 
 * In modern development, it is the industry standard for UTF-8 storage; however, it is "encoding-agnostic," meaning 
 * it treats data as raw bytes and does not inherently understand multi-byte character boundaries.
 */

using NString = NEX_STD string;
using NStringView = NEX_STD string_view;

/**
 * @brief Standard wide string types.
 * @details
 * The std::wstring is an alias for std::basic_string<wchar_t>, designed to hold wide characters. The size of 
 * wchar_t is platform-dependent (2 bytes on Windows, 4 bytes on Unix-like systems), which can lead to portability 
 * issues. While it can be used for UTF-16 or UTF-32 encoding, it does not guarantee a specific encoding and may not 
 * be suitable for cross-platform applications.
 */

using WString = NEX_STD wstring;
using WStringView = NEX_STD wstring_view;

/**
 * @brief UTF-8 encoded string types.
 * @details
 * UTF-8 is a variable-width encoding that represents Unicode characters using 1 to 4 bytes, maintaining 
 * full backward compatibility with ASCII. It uses a self-synchronizing bit pattern that ensures byte-order 
 * independence and prevents data corruption from misaligning the character stream.
 */

using Utf8String = NEX_STD string;
using Utf8StringView = NEX_STD string_view;

/**
 * @brief UTF-16 encoded string types.
 * @details
 * UTF-16 is a variable-width encoding that uses 2 or 4 bytes per character, primarily designed for systems 
 * where the most common characters fit within 16 bits. It relies on "surrogate pairs" to handle larger code points 
 * and requires a Byte Order Mark (BOM) to navigate Big-Endian or Little-Endian memory architectures.
 */

using Utf16String = NEX_STD u16string;
using Utf16StringView = NEX_STD u16string_view;

/**
 * @brief UTF-32 encoded string types.
 * @details
 * UTF-32 is a fixed-width encoding that uses exactly 4 bytes for every character, providing a direct 1:1 mapping 
 * between memory offsets and Unicode code points. While it simplifies string indexing and manipulation (since 
 * the n-th character is always at 4×n), it is memory-inefficient, often quadrupling the storage requirements 
 * for standard text.
 */

using Utf32String = NEX_STD u32string;
using Utf32StringView = NEX_STD u32string_view;

/**
 * @brief Forward declarations of core String types
 * 
 * @details
 * Forward declare the String and StringView types from the core layer to avoid including their full definitions
 * in this header, which helps reduce compile-time dependencies.
 * 
 * @see String class definition in nex/core/text/string.h
 * @see StringView class definition in nex/core/text/string_view.h
 */

// Forward declare common core types
class String;
class StringView;

NEX_NAMESPACE_END
