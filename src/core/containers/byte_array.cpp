// Sandyera Development Core
// Module: Stone::Core (Implementation)
// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Anthony Lee Stark

#include <cctype>
#include <sstream>
#include <iomanip>

#include "nex/base/types.h"
#include "nex/core/containers/byte_array.h"

NEX_CORE_NAMESPACE_BEGIN

// ============================================================================
// Internal helper functions for ByteArray
// ============================================================================

namespace {
    //  Convert hex character to value
    static uint8 hexCharToValue(char c) {
        if (c >= '0' && c <= '9') return static_cast<uint8>(c - '0');
        if (c >= 'A' && c <= 'F') return static_cast<uint8>(c - 'A' + 10);
        if (c >= 'a' && c <= 'f') return static_cast<uint8>(c - 'a' + 10);
        return 0;
    }

    // Check if character is hex
    static bool isHexChar(char c) {
        return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
    }
}

////// Constructors --------------------------------------------------

// Construct from C-style array
ByteArray::ByteArray(const_byte_ptr data, usize size) {
    if (data && size > 0) {
        buffer_.assign(data, data + size);
    }
}

// Construct from C-style array (unsigned char)
ByteArray::ByteArray(const unsigned char* data, usize size) {
    if (data && size > 0) {
        buffer_.assign(data, data + size);
    }
}

// Construct from ArrayList<unsigned char>
ByteArray::ByteArray(const ArrayList<unsigned char>& data) {
    buffer_.assign(data.begin(), data.end());
}

// Construct from StdString (treat as binary data)
ByteArray::ByteArray(const StdString& str) {
    buffer_.assign(reinterpret_cast<const_byte_ptr>(str.data()),
                    reinterpret_cast<const_byte_ptr>(str.data() + str.size()));
}

// Construct from C-string (treat as binary data)
ByteArray::ByteArray(const_char_ptr str) {
    if (str) {
        usize len = NEX_STD strlen(str);
            buffer_.assign(reinterpret_cast<const_byte_ptr>(str),
                          reinterpret_cast<const_byte_ptr>(str + len));
    }
}

// Copy assignment operator
ByteArray& ByteArray::operator=(const ByteArray& other) {
    if (this != &other)
        buffer_ = other.buffer_;
    return *this;
}

// Move constructor
ByteArray::ByteArray(ByteArray&& other) noexcept : buffer_(NEX_STD move(other.buffer_)) {}

// Move assignment operator
ByteArray& ByteArray::operator=(ByteArray&& other) noexcept {
    if (this != &other)
        buffer_ = NEX_STD move(other.buffer_);
    return *this;
}

////// Factory methods --------------------------------------------------

// Create from raw data
ByteArray ByteArray::fromRawData(const_void_ptr data, usize size) {
    if (!data || size == 0) return ByteArray();
    const_byte_ptr bytes = static_cast<const_byte_ptr>(data);
    return ByteArray(bytes, size);
}

// Create from hex string
ByteArray ByteArray::fromHex(const_char_ptr hexString) {
    if (!hexString) return ByteArray();
    
    ByteArray result;
    usize len = NEX_STD strlen(hexString);
    
    // Skip whitespace and process pairs
    for (usize i = 0; i < len; ++i) {
        // Skip whitespace
        if (NEX_STD isspace(static_cast<unsigned char>(hexString[i]))) {
            continue;
        }
        
        // Check if we have a valid hex character
        if (!isHexChar(hexString[i])) {
            // Invalid character, skip or return empty
            continue;
        }
        
        // Get first hex digit
        uint8 high = hexCharToValue(hexString[i]);
        
        // Check if there's a second digit
        if (i + 1 < len && isHexChar(hexString[i + 1])) {
            uint8 low = hexCharToValue(hexString[i + 1]);
            result.appendByte((high << 4) | low);
            ++i; // Skip the second character
        } else {
            // Single digit, treat as low nibble
            result.appendByte(high);
        }
    }
    
    return result;
}

// Create from hex string (StdString overload)
ByteArray ByteArray::fromHex(const StdString& hexString) {
    return fromHex(hexString.c_str());
}

// Base64 encoding table
static const char base64Chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Helper function to find base64 character index
static int32 base64CharToValue(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    if (c == '=') return -1; // Padding
    return -2; // Invalid
}

// Create from base64 string
ByteArray ByteArray::fromBase64(const_char_ptr base64String) {
    if (!base64String) return ByteArray();

    ByteArray result;
    usize len = NEX_STD strlen(base64String);
    
    // Process in groups of 4 characters
    for (usize i = 0; i < len; i += 4) {
        // Get 4 base64 characters
        int32 values[4] = {0, 0, 0, 0};
        int32 validChars = 0;

        for (int32 j = 0; j < 4 && (i + j) < len; ++j) {
            char c = base64String[i + j];
            if (NEX_STD isspace(static_cast<unsigned char>(c))) {
                // Skip whitespace
                continue;
            }
            int32 val = base64CharToValue(c);
            if (val >= 0) {
                values[j] = val;
                validChars = j + 1;
            } else if (val == -1) {
                // Padding character
                values[j] = 0;
                validChars = j;
                break;
            } else {
                // Invalid character, skip this group
                validChars = 0;
                break;
            }
        }

        if (validChars < 2) continue; // Need at least 2 characters
        
        // Decode the 4 base64 characters into 3 bytes
        uint32 combined = (static_cast<uint32>(values[0]) << 18) |
                           (static_cast<uint32>(values[1]) << 12);

        if (validChars >= 3) {
            combined |= (static_cast<uint32>(values[2]) << 6);
        }
        if (validChars >= 4) {
            combined |= static_cast<uint32>(values[3]);
        }

        // Extract bytes
        result.appendByte(static_cast<uint8>((combined >> 16) & 0xFF));
        if (validChars >= 3) {
            result.appendByte(static_cast<uint8>((combined >> 8) & 0xFF));
        }
        if (validChars >= 4) {
            result.appendByte(static_cast<uint8>(combined & 0xFF));
        }
    }

    return result;
}

// Create from base64 string (StdString overload)
ByteArray ByteArray::fromBase64(const StdString& base64String) {
    return fromBase64(base64String.c_str());
}

// Create from ArrayList<uint8>
ByteArray ByteArray::fromArrayList(const ArrayList<uint8>& vec) {
    ByteArray result;
    for (usize i = 0; i < vec.size(); ++i) {
        result.appendByte(vec[i]);
    }
    return result;
}

////// Conversion methods --------------------------------------------------

// Convert to StdString (treat as binary data)
StdString ByteArray::toStdString() const {
    return StdString(reinterpret_cast<const_char_ptr>(buffer_.data()), buffer_.size());
}

// Convert to hex string
StdString ByteArray::toHex() const {
    if (buffer_.empty()) return StdString();

    NEX_STD ostringstream oss;
    oss << NEX_STD hex << NEX_STD uppercase << NEX_STD setfill('0');

    for (usize i = 0; i < buffer_.size(); ++i) {
        oss << NEX_STD setw(2) << static_cast<unsigned>(buffer_[i]);
    }

    return oss.str();
}

// Convert to base64 string
StdString ByteArray::toBase64() const {
    if (buffer_.empty()) return StdString();

    StdString result;
    usize len = buffer_.size();

    // Process in groups of 3 bytes
    for (usize i = 0; i < len; i += 3) {
        // Get 3 bytes (or remaining bytes)
        uint32 combined = 0;
        int32 bytesInGroup = 0;

        for (int32 j = 0; j < 3 && (i + j) < len; ++j) {
            combined |= (static_cast<uint32>(buffer_[i + j]) << (16 - j * 8));
            bytesInGroup++;
        }

        // Encode to 4 base64 characters
        result += base64Chars[(combined >> 18) & 0x3F];
        result += base64Chars[(combined >> 12) & 0x3F];
        
        if (bytesInGroup >= 2) {
            result += base64Chars[(combined >> 6) & 0x3F];
        } else {
            result += '=';
        }

        if (bytesInGroup >= 3) {
            result += base64Chars[combined & 0x3F];
        } else {
            result += '=';
        }
    }

    return result;
}

////// Modifiers --------------------------------------------------

// Append another ByteArray
ByteArray& ByteArray::append(const ByteArray& other) {
    if (this != &other) {
        buffer_.insert(buffer_.end(), other.buffer_.begin(), other.buffer_.end());
    }
    return *this;
}

// Append raw data
ByteArray& ByteArray::append(const_byte_ptr data, usize size) {
    if (data && size > 0) {
        buffer_.insert(buffer_.end(), data, data + size);
    }
    return *this;
}

// Prepend another ByteArray
ByteArray& ByteArray::prepend(const ByteArray& other) {
    if (this != &other) {
        buffer_.insert(buffer_.begin(), other.buffer_.begin(), other.buffer_.end());
    }
    return *this;
}

// Prepend raw data
ByteArray& ByteArray::prepend(const_byte_ptr data, usize size) {
    if (data && size > 0) {
        buffer_.insert(buffer_.begin(), data, data + size);
    }
    return *this;
}

// Remove all occurrences of a byte
ByteArray& ByteArray::remove(uint8 byte) {
    buffer_.erase(NEX_STD remove(buffer_.begin(), buffer_.end(), byte), buffer_.end());
    return *this;
}

// Remove bytes at position
int32 ByteArray::removeAt(usize pos, usize count /* = 1 */) {
    if (pos >= buffer_.size()) return static_cast<int32>(buffer_.size());
    usize actualCount = NEX_STD min(count, buffer_.size() - pos);
    buffer_.erase(buffer_.begin() + pos, buffer_.begin() + pos + actualCount);
    return static_cast<int32>(buffer_.size());
}

// Insert bytes at position
ByteArray& ByteArray::insert(usize pos, const ByteArray& other) {
    if (pos > buffer_.size()) pos = buffer_.size();
    buffer_.insert(buffer_.begin() + pos, other.buffer_.begin(), other.buffer_.end());
    return *this;
}

ByteArray& ByteArray::insert(usize pos, const_byte_ptr data, usize size) {
    if (pos > buffer_.size()) pos = buffer_.size();
    if (data && size > 0) {
        buffer_.insert(buffer_.begin() + pos, data, data + size);
    }
    return *this;
}

ByteArray& ByteArray::insert(usize pos, uint8 byte) {
    if (pos > buffer_.size()) pos = buffer_.size();
    buffer_.insert(buffer_.begin() + pos, byte);
    return *this;
}

// Replace bytes
ByteArray& ByteArray::replace(usize pos, usize count, const ByteArray& other) {
    if (pos >= buffer_.size()) return *this;
    usize actualCount = NEX_STD min(count, buffer_.size() - pos);
    buffer_.erase(buffer_.begin() + pos, buffer_.begin() + pos + actualCount);
    buffer_.insert(buffer_.begin() + pos, other.buffer_.begin(), other.buffer_.end());
    return *this;
}

ByteArray& ByteArray::replace(usize pos, usize count, const_byte_ptr data, usize size) {
    if (pos >= buffer_.size()) return *this;
    usize actualCount = NEX_STD min(count, buffer_.size() - pos);
    buffer_.erase(buffer_.begin() + pos, buffer_.begin() + pos + actualCount);
    if (data && size > 0) {
        buffer_.insert(buffer_.begin() + pos, data, data + size);
    }
    return *this;
}

////// Subarray operations --------------------------------------------------

// Get left part of the array
ByteArray ByteArray::left(usize count) const {
    if (count == 0) return ByteArray();
    if (count >= buffer_.size()) return ByteArray(buffer_.data(), buffer_.size());
    return ByteArray(buffer_.data(), count);
}

// Get right part of the array
ByteArray ByteArray::right(usize count) const {
    if (count == 0) return ByteArray();
    if (count >= buffer_.size()) return ByteArray(buffer_.data(), buffer_.size());
    usize start = buffer_.size() - count;
    return ByteArray(buffer_.data() + start, count);
}

// Get middle part of the array
ByteArray ByteArray::mid(usize start, usize count /* = NEX_STD numeric_limits<usize>::max() */) const {
    if (start >= buffer_.size()) return ByteArray();
    usize maxCount = buffer_.size() - start;
    usize actualCount = (count == NEX_STD numeric_limits<usize>::max()) ? maxCount : NEX_STD min(count, maxCount);
    return ByteArray(buffer_.data() + start, actualCount);
}

////// Search operations --------------------------------------------------

// Find first occurrence of byte
usize ByteArray::indexOf(uint8 byte, usize from /* = 0 */) const {
    static constexpr usize npos = NEX_STD numeric_limits<usize>::max();
    if (from >= buffer_.size()) return npos;
    auto it = NEX_STD find(buffer_.begin() + from, buffer_.end(), byte);
    return (it != buffer_.end()) ? static_cast<usize>(it - buffer_.begin()) : npos;
}

// Find last occurrence of byte
usize ByteArray::lastIndexOf(uint8 byte, usize from /* = NEX_STD numeric_limits<usize>::max() */) const {
    static constexpr usize npos = NEX_STD numeric_limits<usize>::max();
    if (buffer_.empty()) return npos;
    if (from >= buffer_.size()) from = buffer_.size() - 1;
    
    for (usize i = from + 1; i > 0; --i) {
        if (buffer_[i - 1] == byte) {
            return i - 1;
        }
    }
    return npos;
}

// Find first occurrence of subarray
usize ByteArray::indexOf(const ByteArray& other, usize from /* = 0 */) const {
    static constexpr usize npos = NEX_STD numeric_limits<usize>::max();
    if (other.empty() || from >= buffer_.size()) return npos;
    if (other.size() > buffer_.size() - from) return npos;
    
    auto it = NEX_STD search(buffer_.begin() + from, buffer_.end(),
                            other.buffer_.begin(), other.buffer_.end());
    return (it != buffer_.end()) ? static_cast<usize>(it - buffer_.begin()) : npos;
}
    
// Check if array contains byte
bool ByteArray::contains(uint8 byte) const {
    return NEX_STD find(buffer_.begin(), buffer_.end(), byte) != buffer_.end();
}

// Check if array contains subarray
bool ByteArray::contains(const ByteArray& other) const {
    static constexpr usize npos = NEX_STD numeric_limits<usize>::max();
    return indexOf(other) != npos;
}

// Count occurrences of byte
usize ByteArray::count(uint8 byte) const {
    return static_cast<usize>(NEX_STD count(buffer_.begin(), buffer_.end(), byte));
}

////// Comparison operations --------------------------------------------------

// Compare with another ByteArray
int32 ByteArray::compare(const ByteArray& other) const noexcept {
    usize minSize = NEX_STD min(buffer_.size(), other.buffer_.size());
    int32 result = NEX_STD memcmp(buffer_.data(), other.buffer_.data(), minSize);
    if (result != 0) return result;
    if (buffer_.size() < other.buffer_.size()) return -1;
    if (buffer_.size() > other.buffer_.size()) return 1;
    return 0;
}

// Comparison operators
bool ByteArray::operator==(const ByteArray& other) const noexcept {
    return buffer_.size() == other.buffer_.size() &&
            (buffer_.empty() || NEX_STD memcmp(buffer_.data(), other.buffer_.data(), buffer_.size()) == 0);
}

// Concatenation operator
ByteArray ByteArray::operator+(const ByteArray& other) const {
    ByteArray result = *this;
    result.append(other);
    return result;
}

////// Utility operations --------------------------------------------------

// Fill array with value
ByteArray& ByteArray::fill(uint8 value) {
    NEX_STD fill(buffer_.begin(), buffer_.end(), value);
    return *this;
}

ByteArray& ByteArray::fill(uint8 value, usize start, usize count) {
    if (start >= buffer_.size()) return *this;
    usize end = NEX_STD min(start + count, buffer_.size());
    NEX_STD fill(buffer_.begin() + start, buffer_.begin() + end, value);
    return *this;
}

// Reverse the array
ByteArray& ByteArray::reverse() {
    NEX_STD reverse(buffer_.begin(), buffer_.end());
    return *this;
}

NEX_CORE_NAMESPACE_END
