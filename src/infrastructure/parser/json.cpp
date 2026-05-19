/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include <fstream>
#include <sstream>
#include <algorithm>

#include "nex/core/text/encoding.h"
#include "nex/infrastructure/parser/json.h"

// Include nlohmann/json
#include <nlohmann/json.hpp>

NEX_INFRA_NAMESPACE_BEGIN

// Define the opaque node handle used by navigatePath()
struct Json::NodeHandle {
    const nlohmann::json* node = nullptr;
};

// Internal implementation of Json adapter
struct Json::Impl {
    nlohmann::json json;
    bool loaded = false;
    UniquePtr<NodeHandle> nodeHandle;
};

// Helper utilities for JSON parser implementation
namespace {

    // Convert StringView to UTF-8 string (std::string)
    Utf8String toUtf8String(StringView value) {
        using namespace NEX_PREPEND_NAMESPACE(encoding);
        const auto result = utf16ToUtf8(Utf16StringView(value.data(), value.size()));
        return result.isOk() ? result.value() : Utf8String();
    }

    // Convert UTF-8 string (std::string) to String
    String fromUtf8String(const Utf8String& value) {
        return String::fromUtf8(value);
    }

    // Navigate JSON using dot-separated key path (e.g., "application.name")
    const nlohmann::json* navigatePathImpl(const nlohmann::json& root, const Utf8String& keyPath) {
        if (keyPath.empty()) {
            return nullptr;
        }

        const nlohmann::json* current = &root;

        NEX_STD stringstream ss(keyPath);
        Utf8String segment;

        while (NEX_STD getline(ss, segment, '.')) {
            if (segment.empty()) {
                continue;
            }

            // Check if current node is an object and contains the segment as a key
            if (!current->is_object() || !current->contains(segment)) {
                return nullptr;
            }

            current = &(*current)[segment];
        }

        return current;
    }

} // namespace

// Constructor
Json::Json() 
    : impl_(NEX_STD make_unique<Impl>()) {}

// Default destructor
NEX_DEFINE_DEFAULT_DTOR(Json);

// Load JSON from file
Result<void> Json::loadFromFile(StringView filePath) {
    // Convert file path to UTF-8 string
    const Utf8String path = toUtf8String(filePath);
    if (path.empty()) {
        return Result<void>::error({
            ErrorCode::InvalidArgument, "Invalid file path"
        });
    }

    // Attempt to open and parse the file
    try {
        NEX_STD ifstream file(path);
        if (!file.is_open()) {
            return Result<void>::error({
                ErrorCode::FileNotFound, "File not found"
            });
        }

        nlohmann::json j;
        file >> j;
        impl_->json = NEX_STD move(j);
        impl_->loaded = true;
        
        return Result<void>::ok();
    } 
    catch (const nlohmann::json::parse_error& e) {
        // JSON parsing error -> file exists but content is not valid JSON
        // It might because the file is empty, malformed, having invalid format,
        // or contains invalid characters.
        // Perhaps we can distinguish this from deserialization errors, 
        // but for now we can use a general error code
        return Result<void>::error({
            ErrorCode::InvalidConfigFormat, "Invalid JSON format"
        });
    }
    catch (const nlohmann::json::type_error& e) {
        // Type mismatch error during parsing (e.g., expecting an object but found an array)
        return Result<void>::error({
            ErrorCode::InvalidType, "Type mismatch error"
        });
    }
    catch (const nlohmann::json::out_of_range& e) {
        // Required key does not exist in the JSON (e.g., accessing a non-existent key during parsing)
        return Result<void>::error({
            ErrorCode::MissingRequiredSetting, "Missing required setting"
        });
    }
    catch (const nlohmann::json::invalid_iterator& e) {
        // Rarely occurs during parsing, but still mapped to a general error code
        // This can happen if the JSON structure is unexpectedly modified during parsing,
        // or if there is an internal issue with the JSON library's iterator handling.
        return Result<void>::error({
            ErrorCode::InvalidConfig, "Invalid configuration"
        });
    }
    catch (const nlohmann::json::other_error& e) {
        // Catch-all for other JSON-related errors that don't fit the above categories
        // This can include errors related to memory allocation, encoding issues, or other 
        // unexpected conditions encountered by the JSON library.
        return Result<void>::error({
            ErrorCode::SerializationFailed, "Serialization failed"
        });
    }
    catch (const std::ios_base::failure& e) {   // catch more specific file I/O errors
        // This can occur if there are issues with reading the file, such as permission errors,
        // disk errors, or if the file is being accessed by another process. It indicates that
        // the file could not be read successfully, even if it exists.
        return Result<void>::error({
            ErrorCode::IoError, "I/O error"
        });
    }
    catch (const std::exception& e) {
        // Catch other exceptions (rarely occurs during parsing, 
        // but can happen due to unexpected conditions)
        return Result<void>::error({
            ErrorCode::DeserializationFailed, "Deserialization failed"
        });
    }
    catch (...) {
        // Catch-all for any other unforeseen errors
        return Result<void>::error({
            ErrorCode::UnknownError, "Unknown error"
        });
    }
}

// Load JSON from string
Result<void> Json::loadFromString(StringView jsonString) {
    // Convert JSON string to UTF-8 std::string
    const Utf8String text = toUtf8String(jsonString);
    if (text.empty()) {
        return Result<void>::error({
            ErrorCode::InvalidArgument, "Invalid argument"
        });
    }

    // Attempt to parse the JSON string
    try {
        nlohmann::json j = nlohmann::json::parse(text);
        impl_->json = NEX_STD move(j);
        impl_->loaded = true;
        
        // Successfully loaded and parsed JSON
        return Result<void>::ok();
    } 
    catch (const nlohmann::json::parse_error& e) {
        // JSON parsing error -> content is not valid JSON
        // It might be because the string is empty, malformed, having invalid format,
        // or contains invalid characters.
        return Result<void>::error({
            ErrorCode::InvalidFormat, "Invalid format"
        });
    }
    catch (const nlohmann::json::type_error& e) {
        // Type mismatch error during parsing (e.g., expecting an object but found an array)
        return Result<void>::error({
            ErrorCode::InvalidConfig, "Invalid configuration"
        });
    }
    catch (const nlohmann::json::out_of_range& e) {
        // Required key does not exist in the JSON (e.g., accessing a non-existent key during parsing)
        return Result<void>::error({
            ErrorCode::MissingRequiredSetting, "Missing required setting"
        });
    }
    catch (const nlohmann::json::invalid_iterator& e) {
        // Rarely occurs during parsing, but still mapped to a general error code
        // This can happen if the JSON structure is unexpectedly modified during parsing,
        // or if there is an internal issue with the JSON library's iterator handling.
        return Result<void>::error({
            ErrorCode::InvalidConfig, "Invalid configuration"
        });
    }
    catch (const nlohmann::json::other_error& e) {
        // Catch-all for other JSON-related errors that don't fit the above categories
        // This can include errors related to memory allocation, encoding issues, or other 
        // unexpected conditions encountered by the JSON library.
        return Result<void>::error({
            ErrorCode::SerializationFailed, "Serialization failed"
        });
    }
    catch (const std::exception& e) {
        // Catch other exceptions (rarely occurs during parsing, 
        // but can happen due to unexpected conditions)
        return Result<void>::error({
            ErrorCode::DeserializationFailed, "Deserialization failed"
        });
    }
    catch (...) {
        // Catch-all for any other unforeseen errors
        return Result<void>::error({
            ErrorCode::UnknownError, "Unknown error"
        });
    }
}

// Navigate nested JSON by key path and return internal node handle.
// The handle is only valid while the adapter remains unchanged.
const Json::NodeHandle* Json::navigatePath(StringView keyPath) const {
    if (!isValid()) {
        return nullptr;
    }

    // Convert key path to UTF-8 string and navigate the JSON structure
    const Utf8String path = toUtf8String(keyPath);
    const nlohmann::json* node = navigatePathImpl(impl_->json, path);
    if (!node) {
        impl_->nodeHandle.reset();
        return nullptr;
    }

    // Store the node pointer in the node handle for reuse
    if (!impl_->nodeHandle) impl_->nodeHandle = NEX_STD make_unique<NodeHandle>();
    impl_->nodeHandle->node = node;
    return impl_->nodeHandle.get();
}

// Get string value by key path
String Json::getString(StringView keyPath, StringView defaultValue /* = StringView() */) const {
    if (!isValid()) {
        return defaultValue.toString();
    }

    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;
    if (!node || !node->is_string()) {
        return defaultValue.toString();
    }

    // Convert the JSON string to String and return
    return fromUtf8String(node->get<Utf8String>());
}

// Get integer value by key path
int32 Json::getInt(StringView keyPath, int32 defaultValue /* = 0 */) const {
    if (!isValid()) {
        return defaultValue;
    }

    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;
    if (!node || !node->is_number_integer()) {
        return defaultValue;
    }

    // Return the integer value
    return node->get<int32>();
}

// Get unsigned integer value by key path
uint32 Json::getUInt(StringView keyPath, uint32 defaultValue /* = 0 */) const {
    if (!isValid()) {
        return defaultValue;
    }

    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Handle both unsigned and signed integers (if non-negative)
    if (!node || !node->is_number_unsigned()) {
        // Try to get as signed int and convert
        if (node && node->is_number_integer()) {
            int32 val = node->get<int32>();
            if (val >= 0) {
                return static_cast<uint32>(val);
            }
        }
        return defaultValue;
    }

    // Return the unsigned integer value
    return node->get<uint32>();
}

// Get boolean value by key path
bool Json::getBool(StringView keyPath, bool defaultValue /* = false */) const {
    if (!isValid()) {
        return defaultValue;
    }

    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;
    if (!node || !node->is_boolean()) {
        return defaultValue;
    }

    // Return the boolean value
    return node->get<bool>();
}

// Get double value by key path
float64 Json::getDouble(StringView keyPath, float64 defaultValue /* = 0.0 */) const {
    if (!isValid()) {
        return defaultValue;
    }

    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;
    if (!node || !node->is_number()) {
        return defaultValue;
    }

    // Return the double value
    return node->get<float64>();
}

// Try to get string value by key path
bool Json::tryGetString(StringView keyPath, String& value) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if node exists and is a string
    if (!node || !node->is_string()) return false;

    // Convert the JSON string to String and set the output parameter
    value = fromUtf8String(node->get<Utf8String>());
    return true;
}

// Try to get integer value by key path
bool Json::tryGetInt(StringView keyPath, int32& value) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if node exists and is an integer
    if (!node || !node->is_number_integer()) return false;

    // Set the output parameter to the integer value
    value = node->get<int32>();
    return true;
}

// Try to get unsigned integer value by key path
bool Json::tryGetUInt(StringView keyPath, uint32& value) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if not exists
    if (!node) return false;

    // Handle both unsigned and signed integers (if non-negative)
    if (node->is_number_unsigned()) {
        value = node->get<uint32>();
        return true;
    }
    if (node->is_number_integer()) {
        int32 v = node->get<int32>();
        if (v >= 0) { value = static_cast<uint32>(v); return true; }
    }
    return false;
}

// Try to get boolean value by key path
bool Json::tryGetBool(StringView keyPath, bool& value) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if node exists and is a boolean
    if (!node || !node->is_boolean()) return false;

    // Set the output parameter to the boolean value
    value = node->get<bool>();
    return true;
}

bool Json::tryGetDouble(StringView keyPath, float64& value) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if node exists and is a number
    if (!node || !node->is_number()) return false;

    // Set the output parameter to the double value
    value = node->get<float64>();
    return true;
}

// Check if JSON is valid/loaded
bool Json::isValid() const {
    return impl_ && impl_->loaded;
}

// Check if key path exists
bool Json::hasKey(StringView keyPath) const {
    if (!isValid()) {
        return false;
    }

    // Navigate to the node using the key path and check if it exists
    return navigatePath(keyPath) != nullptr;
}

// Check if key path is an object
bool Json::isObject(StringView keyPath) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if node exists and is an object
    return node && node->is_object();
}

// Check if key path is an array
bool Json::isArray(StringView keyPath) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if node exists and is an array
    return node && node->is_array();
}

// Get array size by key path
usize Json::arraySize(StringView keyPath) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if node exists and is an array
    if (!node || !node->is_array()) return 0;

    // Return the size of the array
    return static_cast<usize>(node->size());
}

// Try to get string value at array index by key path (e.g., "users[0].name")
bool Json::tryGetStringAt(StringView keyPath, usize index, String& value) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if node exists and is an array, and if index is within bounds
    if (!node || !node->is_array()) return false;
    if (index >= node->size()) return false;

    // Get the element at the specified index and check if it's a string
    const nlohmann::json& elem = (*node)[index];
    if (!elem.is_string()) return false;

    // Convert the JSON string to String and set the output parameter
    value = fromUtf8String(elem.get<Utf8String>());
    return true;
}

// Try to get integer value at array index by key path (e.g., "users[0].age")
bool Json::tryGetIntAt(StringView keyPath, usize index, int32& value) const {
    // Navigate to the node using the key path
    const NodeHandle* h = navigatePath(keyPath);
    const nlohmann::json* node = h ? h->node : nullptr;

    // Check if node exists and is an array, and if index is within bounds
    if (!node || !node->is_array()) return false;
    if (index >= node->size()) return false;

    // Get the element at the specified index and check if it's an integer
    const nlohmann::json& elem = (*node)[index];
    if (!elem.is_number_integer()) return false;

    // Set the output parameter to the integer value
    value = elem.get<int32>();
    return true;
}

// Get JSON as string (pretty if requested)
String Json::toString(bool pretty /* = false */) const {
    if (!isValid()) {
        return String(u"{}");
    }

    // Convert JSON to string with optional pretty-printing
    if (pretty) {
        return fromUtf8String(impl_->json.dump(2));
    }

    // Compact JSON string
    return fromUtf8String(impl_->json.dump());
}

NEX_INFRA_NAMESPACE_END
