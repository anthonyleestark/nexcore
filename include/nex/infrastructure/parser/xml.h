/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/linear.h"
#include "nex/base/wrappers.h"
#include "nex/core/text/string.h"
#include "nex/core/text/string_view.h"
#include "nex/base/result.h"
#include "nex/base/error.h"

NEX_INFRA_NAMESPACE_BEGIN

// Access common core types
NEX_IMPORT_TYPE_FROM_LAYER(core, String);
NEX_IMPORT_TYPE_FROM_LAYER(core, StringView);

// Forward declarations
class XmlAttribute;
class XmlNode;
class XmlDocument;

/**
 * @class   XmlAttribute
 * @brief   Lightweight XML attribute wrapper. Non-owning handle into an XmlNode's attributes.
 * 
 * @note
 * Do not use an attribute after its originating node or document is destroyed or modified.
 * 
 * @details
 * This class provides access to the name and value of an XML attribute, as well as convenient 
 * type conversion methods (e.g., `asInt`, `asDouble`, `asBool`) that return a default value 
 * if conversion fails.
 * 
 * @note
 * - The `isValid()` method should be used to check if the attribute exists before accessing 
 *   its properties.  
 * - The class uses the Pimpl idiom to hide the underlying pugixml implementation details, 
 *   and is designed to be a lightweight handle that does not own the underlying XML data.
 * - XmlAttribute instances are only valid while the originating XmlNode and XmlDocument remain valid.
 * - This class is intended for read-only access to XML attributes. Modifying attributes should be done
 *   through the XmlNode interface or directly via pugixml if necessary.
 * - The class is non-copyable but supports move semantics for efficient handling of attribute instances.
 * - The type conversion methods (`asInt`, `asDouble`, `asBool`) provide a convenient way to retrieve
 *   attribute values in the desired type, with a fallback default value if the conversion is not successful.
 * - The `name()` and `value()` methods return the attribute's name and value as strings, respectively.
 * - The class is designed to be used in conjunction with the XmlNode class, which provides access 
 *   to the attributes of an XML node.
 * 
 * Example usage:
 * ```
 * XmlDocument doc;
 * if (doc.loadFromFile("config.xml")) {
 *     XmlNode root = doc.root();
 *     XmlAttribute attr = root.attribute("version");
 *     if (attr.isValid()) {
 *         int version = attr.asInt(1); // Get version as int, default to 1 if conversion fails
 *         std::cout << "Config version: " << version << std::endl;
 *     }
 * }
 * ```
 */
class NEX_EXPORT XmlAttribute {
public:
    // Constructor
    XmlAttribute();

    // Destructor
    NEX_DECLARE_DEFAULT_DTOR(XmlAttribute);

    // Default copy semantics
    NEX_DEFAULT_COPY(XmlAttribute);

    // Default move semantics
    NEX_DEFAULT_MOVE(XmlAttribute);

    ////// Attribute accessors -----------------------

    // Check if the attribute is valid (i.e., it exists in the XML node)
    bool isValid() const noexcept;

    // Get the name of the attribute
    String name() const;

    // Get the value of the attribute as a string
    String value() const;

    // Get the value of the attribute as an integer, with a default if conversion fails
    int asInt(int defaultValue = 0) const;

    // Get the value of the attribute as a double, with a default if conversion fails
    double asDouble(double defaultValue = 0.0) const;

    // Get the value of the attribute as a boolean, with a default if conversion fails
    bool asBool(bool defaultValue = false) const;

private:
    // Private implementation using the Pimpl idiom to hide pugixml details
    struct Impl;
    UniquePtr<Impl> impl_;

    // Allow XmlNode implementation to construct attributes
    friend class XmlNode;
};

/**
 * @class   XmlNode
 * @brief   Lightweight XML node wrapper. Non-owning handle into an XmlDocument's nodes.
 * 
 * @note
 * Do not use a node after its originating document is destroyed or modified.
 * 
 * @details
 * This class provides access to the name and text of an XML node, as well as methods to retrieve 
 * child nodes and attributes. The class uses the Pimpl idiom to hide the underlying pugixml 
 * implementation details, and is designed to be a lightweight handle that does not own the underlying XML data.
 * 
 * @note
 * - XmlNode instances are only valid while the originating XmlDocument remains valid.
 * - This class is intended for read-only access to XML nodes. Modifying the XML structure should be done 
 *   through the XmlDocument interface or directly via pugixml if necessary.
 * - The `name()` method returns the name of the XML node as a string, while the `text()` method returns 
 *   the text content of the node. 
 * - The `child()` method allows retrieval of a single child node by name, and the `children()` method 
 *   returns a list of all child nodes with the specified name. 
 * - The `attribute()` method allows retrieval of an attribute of the node by name, returning an XmlAttribute 
 *   instance that provides access to the attribute's name and value.
 * - The class is non-copyable but supports move semantics for efficient handling of node instances.
 * - The underlying XML parsing is performed using the pugixml library, but the implementation details 
 *   are hidden from users of this class through the use of the Pimpl idiom.
 * - This class is designed to be used in conjunction with the XmlDocument class, which provides access 
 *   to the root node of the XML document and manages the lifetime of the XML data.
 * 
 * Example usage:
 * ```
 * XmlDocument doc;
 * if (doc.loadFromFile("config.xml")) {
 *     XmlNode root = doc.root();
 *     std::cout << "Root node name: " << root.name() << std::endl;
 *     XmlNode child = root.child("settings");
 *     if (child.isValid()) {
 *         std::cout << "Settings node text: " << child.text() << std::endl;
 *     }
 * }
 * ```
 */
class NEX_EXPORT XmlNode {
public:
    // Constructor
    XmlNode();

    // Destructor
    NEX_DECLARE_DEFAULT_DTOR(XmlNode);

    // Default copy semantics
    NEX_DEFAULT_COPY(XmlNode);

    // Default move semantics
    NEX_DEFAULT_MOVE(XmlNode);

    ////// Accessors -----------------------

    // Check if node is valid (i.e., points to an actual XML node)
    bool isValid() const noexcept;

    // Get the name of the node as a String
    String name() const;

    // Get the text content of the node as a String
    String text() const;

    // Get the first child node regardless of name
    XmlNode firstChild() const;

    // Get the first child node with the specified name
    XmlNode child(StringView name) const;

    // Check if the node has at least one child
    bool hasChildren() const;

    // Check if a child with the specified name exists
    bool hasChild(StringView name) const;

    // Get all child nodes with the specified name
    Vec<XmlNode> children(StringView name) const;

    // Get the attribute with the specified name
    XmlAttribute attribute(StringView name) const;

private:
    // Private implementation using the Pimpl idiom to hide pugixml details
    struct Impl;
    UniquePtr<Impl> impl_;

    // Allow XmlDocument implementation to construct nodes
    friend class XmlDocument;
};

/**
 * @class   XmlDocument
 * @brief   Lightweight XML document wrapper. Owns the XML tree and provides access to the root node.
 * 
 * @details
 * This class provides methods to load XML content from files or strings, and to access the root node 
 * of the XML document. It uses the Pimpl idiom to hide the underlying pugixml implementation details, 
 * and is designed to be a lightweight wrapper that owns the XML data and provides read-only access 
 * to the XML structure through XmlNode instances.
 * 
 * @note
 * - The `loadFromFile()` and `loadFromString()` methods return a boolean indicating 
 *   success or failure of the loading operation. 
 * - The `isValid()` method can be used to check if the document is successfully loaded and valid.
 * - The `root()` method returns an XmlNode representing the root element of the XML document. 
 *   If the document is not valid or does not contain a root element, an invalid XmlNode will be returned.
 * - The class is non-copyable but supports move semantics for efficient handling of XML document instances.
 * - The class is designed to be used in conjunction with the XmlNode class, which provides access 
 *   to the nodes and attributes of the XML document.
 * - The underlying XML parsing is performed using the pugixml library, but the implementation details 
 *   are hidden from users of this class through the use of the Pimpl idiom.
 * - This class is intended for read-only access to XML documents. Modifying the XML structure 
 *   should be done through the XmlNode interface or directly via pugixml if necessary.
 * 
 * Example usage:
 * ```
 * XmlDocument doc;
 * if (doc.loadFromFile("config.xml")) {
 *     XmlNode root = doc.root();
 *     std::cout << "Root node name: " << root.name() << std::endl;
 * } 
 * ```
 */
class NEX_EXPORT XmlDocument {
public:
    // Constructor
    XmlDocument();

    // Destructor
    NEX_DECLARE_DEFAULT_DTOR(XmlDocument);

    // Non-copyable
    NEX_DISALLOW_COPY(XmlDocument);

    // Default move semantics
    NEX_DEFAULT_MOVE(XmlDocument);

    ////// Load XML from external sources -----------------------

    // Load XML from file
    Result<void, Error> loadFromFile(StringView path);

    // Load XML from string
    Result<void, Error> loadFromString(StringView content);

    ////// Accessors -----------------------

    // Get the root node of the XML document
    XmlNode root() const;

    // Check if the document is valid (i.e., successfully loaded)
    bool isValid() const noexcept;

private:
    // Internal implementation using the Pimpl idiom to hide pugixml details
    struct Impl;
    UniquePtr<Impl> impl_;
};

NEX_INFRA_NAMESPACE_END
