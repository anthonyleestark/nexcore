/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#include "nex/base/casts.h"
#include "nex/base/encoding.h"
#include "nex/infrastructure/parser/xml.h"

#include <pugixml.hpp>

NEX_INFRA_NAMESPACE_BEGIN

// =================================================================================
// Helper utilities for XML parser implementation
// =================================================================================

NEX_ANONYMOUS_NAMESPACE_BEGIN

// Convert StringView to UTF-8 string (std::string) (returns string)
Utf8String toUtf8String(StringView value) {
    using namespace NEX_PREPEND_NAMESPACE(encoding);
    const auto result = utf16ToUtf8(Utf16StringView(value.data(), value.size()));
    return result.isOk() ? result.value() : Utf8String();
}

// Convert StringView to UTF-8 string (std::string) (output parameter; returns boolean)
bool toUtf8String(StringView value, Utf8String& out) {
    const Utf8String result = toUtf8String(value);
    if (result.empty() && !value.empty()) {
        return false;
    }
    out = NEX_MOVE(result);
    return true;
}

// Convert UTF-8 string (std::string) to String
String fromUtf8String(const Utf8String& value) {
    return String::fromUtf8(value);
}

NEX_ANONYMOUS_NAMESPACE_END

// =================================================================================
// Implementation of XmlAttribute class methods
// =================================================================================

// Internal implementation of XmlAttribute
struct XmlAttribute::Impl {
    pugi::xml_attribute attr;
};

// Construct XmlAttribute with reference to a pugixml attribute
XmlAttribute::XmlAttribute()
    : impl_(nullptr) {}

// Default destructor
NEX_DEFINE_DEFAULT_DTOR(XmlAttribute);

// Check if attribute is valid
bool XmlAttribute::isValid() const noexcept {
    return impl_ && impl_->attr;
}

// Get the name of the attribute as a String
String XmlAttribute::name() const {
    if (!isValid()) return String();
    cstring name = impl_->attr.name();
    return name ? String::fromUtf8(name) : String();
}

// Get the value of the attribute as a String
String XmlAttribute::value() const {
    if (!isValid()) return String();
    cstring value = impl_->attr.value();
    return value ? String::fromUtf8(value) : String();
}

// Get the value of the attribute as an integer, with a default if conversion fails
int32 XmlAttribute::asInt(int32 defaultValue) const {
    if (!isValid()) return defaultValue;
    return impl_->attr.as_int(defaultValue);
}

// Get the value of the attribute as a float64, with a default if conversion fails
float64 XmlAttribute::asDouble(float64 defaultValue) const {
    if (!isValid()) return defaultValue;
    return impl_->attr.as_double(defaultValue);
}

// Get the value of the attribute as a boolean, with a default if conversion fails
bool XmlAttribute::asBool(bool defaultValue) const {
    if (!isValid()) return defaultValue;
    return impl_->attr.as_bool(defaultValue);
}

// =================================================================================
// Implementation of XmlNode class methods
// =================================================================================

// Internal implementation of XmlNode
struct XmlNode::Impl {
    pugi::xml_node node;
};

// Construct XmlNode
XmlNode::XmlNode()
    : impl_(nullptr) {}

// Default destructor
NEX_DEFINE_DEFAULT_DTOR(XmlNode);

// Check if node is valid
bool XmlNode::isValid() const noexcept {
    return impl_ && impl_->node;
}

// Get the name of the node as a String
String XmlNode::name() const {
    if (!isValid()) return String();
    cstring name = impl_->node.name();
    return name ? String::fromUtf8(name) : String();
}

// Get the text content of the node as a String
String XmlNode::text() const {
    if (!isValid()) return String();
    cstring text = impl_->node.text().as_string();
    return text ? String::fromUtf8(text) : String();
}

// Get the first child node regardless of name
XmlNode XmlNode::firstChild() const {
    XmlNode out;
    if (!isValid()) return out;

    // Get the first child node using pugixml
    pugi::xml_node child = impl_->node.first_child();
    if (!child) return out;

    // Construct node impl with reference to the child
    out.impl_ = NEX_STD make_unique<Impl>();
    out.impl_->node = child;
    return out;
}

// Get the first child node with the specified name
XmlNode XmlNode::child(StringView name) const {
    XmlNode out;
    if (!isValid()) return out;
    
    // Convert parameter to UTF-8 string (std::string)
    Utf8String utf8;
    if (!toUtf8String(name, utf8) || utf8.empty()) return out;

    // Get the child node with the specified name using pugixml
    pugi::xml_node child = impl_->node.child(utf8.c_str());
    if (!child) return out;

    // Construct node impl with reference to the child
    out.impl_ = NEX_STD make_unique<Impl>();
    out.impl_->node = child;
    return out;
}

// Check if the node has at least one child
bool XmlNode::hasChildren() const {
    if (!isValid()) return false;
    return static_cast<bool>(impl_->node.first_child());
}

// Check if a child with the specified name exists
bool XmlNode::hasChild(StringView name) const {
    if (!isValid()) return false;

    // Convert parameter to UTF-8 string (std::string)
    Utf8String utf8;
    if (!toUtf8String(name, utf8) || utf8.empty()) return false;

    // Check for child node with the specified name using pugixml
    return static_cast<bool>(impl_->node.child(utf8.c_str()));
}

// Get all child nodes with the specified name
Vec<XmlNode> XmlNode::children(StringView name) const {
    Vec<XmlNode> list;
    if (!isValid()) return list;
    
    // Convert parameter to UTF-8 string (std::string)
    Utf8String utf8;
    if (!toUtf8String(name, utf8) || utf8.empty()) return list;

    // Iterate over all child nodes with the specified name using pugixml
    for (pugi::xml_node child = impl_->node.child(utf8.c_str())
            ; child; child = child.next_sibling(utf8.c_str())) {
        // Construct node impl with reference to the child and add to list
        XmlNode item;
        item.impl_ = NEX_STD make_unique<Impl>();
        item.impl_->node = child;
        list.push_back(NEX_MOVE(item));
    }

    return list;
}

// Get the attribute with the specified name
XmlAttribute XmlNode::attribute(StringView name) const {
    XmlAttribute attr;
    if (!isValid()) return attr;
    
    // Convert parameter to UTF-8 string (std::string)
    Utf8String utf8;
    if (!toUtf8String(name, utf8) || utf8.empty()) return attr;

    // Get the attribute with the specified name using pugixml
    pugi::xml_attribute a = impl_->node.attribute(utf8.c_str());
    if (!a) return attr;

    // Construct attribute impl with reference to the attribute
    attr.impl_ = NEX_STD make_unique<XmlAttribute::Impl>();

    // Impl derives from XmlAttributeImplPrivate which has pugi::xml_attribute
    attr.impl_->attr = a;
    return attr;
}

// =================================================================================
// Implementation of XmlDocument class methods
// =================================================================================

// Internal implementation of XmlDocument
struct XmlDocument::Impl {
    pugi::xml_document doc;
    bool loaded = false;
};

// Construct XmlDocument
XmlDocument::XmlDocument()
    : impl_(NEX_STD make_unique<Impl>()) {}

// Default destructor
NEX_DEFINE_DEFAULT_DTOR(XmlDocument);

// Load XML from file
Result<void> XmlDocument::loadFromFile(StringView path) {
    // Convert StringView to UTF-8 std::string
    Utf8String utf8;
    const auto success = toUtf8String(path, utf8);
    if (!success || utf8.empty()) {
        return Result<void>::error({
            ErrorCode::DeserializationFailed,
            "Failed to convert file path to UTF-8 for XML parsing"
        });
    }

    // Load XML from file using pugixml
    pugi::xml_parse_result pr = impl_->doc.load_file(utf8.c_str());
    impl_->loaded = static_cast<bool>(pr);
    if (impl_->loaded) {
        return Result<void>::ok();
    } else {
        return Result<void>::error({
            ErrorCode::DeserializationFailed, "Failed to parse XML file"
        });
    }
}

// Load XML from string
Result<void> XmlDocument::loadFromString(const StringView content) {
    // Convert StringView to UTF-8 std::string
    Utf8String utf8;
    const auto success = toUtf8String(content, utf8);
    if (!success || utf8.empty()) {
        return Result<void>::error({
            ErrorCode::DeserializationFailed,
            "Failed to convert input string to UTF-8 for XML parsing"
        });
    }

    // Load XML from string using pugixml
    pugi::xml_parse_result pr = impl_->doc.load_string(utf8.c_str());
    impl_->loaded = static_cast<bool>(pr);
    if (impl_->loaded) {
        return Result<void>::ok();
    } else {
        return Result<void>::error({
            ErrorCode::DeserializationFailed, "Failed to parse XML string"
        });
    }
}

// Get the root node of the XML document
XmlNode XmlDocument::root() const {
    XmlNode node;
    if (!impl_ || !impl_->loaded) return node;

    // Construct node impl with reference to document's root
    node.impl_ = NEX_STD make_unique<XmlNode::Impl>();
    node.impl_->node = impl_->doc.document_element();
    return node;
}

// Check if the document is valid (i.e., successfully loaded)
bool XmlDocument::isValid() const noexcept {
    return impl_ && impl_->loaded;
}

NEX_INFRA_NAMESPACE_END