/**
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2026 Anthony Lee Stark. All rights reserved.
 */

#pragma once

#include <cmath>
#include <algorithm>
#include <string>
#include <cctype>
#include <sstream>
#include <iomanip>

#include "nex/base/macros.h"
#include "nex/base/types.h"
#include "nex/base/string.h"

NEX_CORE_NAMESPACE_BEGIN

// 32-bit RGBA color format (RGBA8888) (non-premultiplied). Common in OpenGL, WebGL, etc.
// The color component order is 0xRRGGBBAA. Similar to ARGB but with a different layout. 
// Use caution when converting between among these 32-bit color formats.
using RGBAColor = uint32;

// Return the red byte from an RGBAColor value.
NEX_NODISCARD inline constexpr uint8 RGBAColor_GetR(RGBAColor rgbaColor) { return (((rgbaColor) >> 24) & 0xFF); }
// Return the green byte from an RGBAColor value.
NEX_NODISCARD inline constexpr uint8 RGBAColor_GetG(RGBAColor rgbaColor) { return (((rgbaColor) >> 16) & 0xFF); }
// Return the blue byte from an RGBAColor value.
NEX_NODISCARD inline constexpr uint8 RGBAColor_GetB(RGBAColor rgbaColor) { return (((rgbaColor) >> 8) & 0xFF); }
// Return the alpha byte from an RGBAColor value.
NEX_NODISCARD inline constexpr uint8 RGBAColor_GetA(RGBAColor rgbaColor) { return (((rgbaColor) >> 0) & 0xFF); }

// Return an RGBAColor value value with the specified byte component values.
NEX_NODISCARD inline constexpr RGBAColor RGBAColorSet(uint8 r, uint8 g, uint8 b, uint8 a) {
    return static_cast<RGBAColor>((static_cast<uint32>(r) << 24) | (static_cast<uint32>(g) << 16) |
        (static_cast<uint32>(b) << 8) | (static_cast<uint32>(a) << 0));
}

// 32-bit ARGB color format (ARGB8888) (non-premultiplied). 
// The color component order is fixed: 0xAARRGGBB. Equivalent to the SkColor type.
using ARGBColor = uint32;

// Return the alpha byte from an ARGBColor value.
NEX_NODISCARD inline constexpr uint8 ARGBColor_GetA(ARGBColor argbColor) { return (((argbColor) >> 24) & 0xFF); }
// Return the red byte from an ARGBColor value.
NEX_NODISCARD inline constexpr uint8 ARGBColor_GetR(ARGBColor argbColor) { return (((argbColor) >> 16) & 0xFF); }
// Return the green byte from an ARGBColor value.
NEX_NODISCARD inline constexpr uint8 ARGBColor_GetG(ARGBColor argbColor) { return (((argbColor) >> 8) & 0xFF); }
// Return the blue byte from an ARGBColor value.
NEX_NODISCARD inline constexpr uint8 ARGBColor_GetB(ARGBColor argbColor) { return (((argbColor) >> 0) & 0xFF); }

// Return an ARGBColor value value with the specified byte component values.
NEX_NODISCARD inline constexpr ARGBColor ARGBColorSet(uint8 a, uint8 r, uint8 g, uint8 b) {
    return static_cast<ARGBColor>((static_cast<uint32>(a) << 24) | (static_cast<uint32>(r) << 16) |
        (static_cast<uint32>(g) << 8) | (static_cast<uint32>(b) << 0));
}

// 32-bit BGRA color format (BGRA8888) (non-premultiplied). Commonly used in Windows DIBs, some DirectX textures. 
// The color component order is 0xBBGGRRAA. Similar to ARGB but with a different layout. 
// Use caution when converting between among these 32-bit color formats.
using BGRAColor = uint32;

// Return the blue byte from an BGRAColor value.
NEX_NODISCARD inline constexpr uint8 BGRAColor_GetB(BGRAColor bgraColor) { return (((bgraColor) >> 24) & 0xFF); }
// Return the green byte from an BGRAColor value.
NEX_NODISCARD inline constexpr uint8 BGRAColor_GetG(BGRAColor bgraColor) { return (((bgraColor) >> 16) & 0xFF); }
// Return the red byte from an BGRAColor value.
NEX_NODISCARD inline constexpr uint8 BGRAColor_GetR(BGRAColor bgraColor) { return (((bgraColor) >> 8) & 0xFF); }
// Return the alpha byte from an BGRAColor value.
NEX_NODISCARD inline constexpr uint8 BGRAColor_GetA(BGRAColor bgraColor) { return (((bgraColor) >> 0) & 0xFF); }

// Return an BGRAColor value value with the specified byte component values.
NEX_NODISCARD inline constexpr BGRAColor BGRAColorSet(uint8 b, uint8 g, uint8 r, uint8 a) {
    return static_cast<BGRAColor>((static_cast<uint32>(b) << 24) | (static_cast<uint32>(g) << 16) |
        (static_cast<uint32>(r) << 8) | (static_cast<uint32>(a) << 0));
}

// 32-bit ABGR color format (ABGR8888) (non-premultiplied). Commonly used in some image loaders and GPU pipelines
// The color component order is 0xAABBGGRR. Similar to ARGB but with a different layout. 
// Use caution when converting between among these 32-bit color formats.
using ABGRColor = uint32;

// Return the alpha byte from an ABGRColor value.
NEX_NODISCARD inline constexpr uint8 ABGRColor_GetA(ABGRColor abgrColor) { return (((abgrColor) >> 24) & 0xFF); }
// Return the blue byte from an ABGRColor value.
NEX_NODISCARD inline constexpr uint8 ABGRColor_GetB(ABGRColor abgrColor) { return (((abgrColor) >> 16) & 0xFF); }
// Return the green byte from an ABGRColor value.
NEX_NODISCARD inline constexpr uint8 ABGRColor_GetG(ABGRColor abgrColor) { return (((abgrColor) >> 8) & 0xFF); }
// Return the red byte from an ABGRColor value.
NEX_NODISCARD inline constexpr uint8 ABGRColor_GetR(ABGRColor abgrColor) { return (((abgrColor) >> 0) & 0xFF); }

// Return an ABGRColor value value with the specified byte component values.
NEX_NODISCARD inline constexpr ABGRColor ABGRColorSet(uint8 a, uint8 b, uint8 g, uint8 r) {
    return static_cast<ABGRColor>((static_cast<uint32>(a) << 24) | (static_cast<uint32>(b) << 16) |
        (static_cast<uint32>(g) << 8) | (static_cast<uint32>(r) << 0));
}

// 24-bit RGB color stored in a 32-bit container (0xRRGGBB). Non-premultiplied.
// Similar to ARGB/RGBA but without the alpha byte (assumed 255 when needed).
// Stored in a uint32 for alignment and convenience.
using RGBColor = uint32;

// Return the red byte from an RGBColor value.
NEX_NODISCARD inline constexpr uint8 RGBColor_GetR(RGBColor rgbColor) { return (((rgbColor) >> 16) & 0xFF); }
// Return the green byte from an RGBColor value.
NEX_NODISCARD inline constexpr uint8 RGBColor_GetG(RGBColor rgbColor) { return (((rgbColor) >> 8) & 0xFF); }
// Return the blue byte from an RGBColor value.
NEX_NODISCARD inline constexpr uint8 RGBColor_GetB(RGBColor rgbColor) { return (((rgbColor) >> 0) & 0xFF); }

// Return an RGBColor value value with the specified byte component values.
NEX_NODISCARD inline constexpr RGBColor RGBColorSet(uint8 r, uint8 g, uint8 b) {
    return static_cast<RGBColor>((static_cast<uint32>(r) << 16) 
            | (static_cast<uint32>(g) << 8) | (static_cast<uint32>(b) << 0));
}

// 8-bit Grayscale color (single channel).
// Value range: 0–255.
using Grayscale8 = uint8;

// 16-bit Grayscale color (higher precision).
// Value range: 0–65535.
using Grayscale16 = uint16;

// 32-bit RGBE color format (RGBE8888) (non-premultiplied). Common used in Radiance HDR (.hdr) format.
// The color component order is 0xRRGGBBEE. Similar to RGBA but with a shared exponent instead of alpha. 
// Use caution when converting between among these 32-bit color formats.
using RGBEColor = uint32;

// Return the red byte from an RGBEColor value.
NEX_NODISCARD inline constexpr uint8 RGBEColor_GetR(RGBEColor rgbeColor) { return (((rgbeColor) >> 24) & 0xFF); }
// Return the green byte from an RGBAColor value.
NEX_NODISCARD inline constexpr uint8 RGBEColor_GetG(RGBEColor rgbeColor) { return (((rgbeColor) >> 16) & 0xFF); }
// Return the blue byte from an RGBAColor value.
NEX_NODISCARD inline constexpr uint8 RGBEColor_GetB(RGBEColor rgbeColor) { return (((rgbeColor) >> 8) & 0xFF); }
// Return the exponent byte from an RGBAColor value.
NEX_NODISCARD inline constexpr uint8 RGBEColor_GetE(RGBEColor rgbeColor) { return (((rgbeColor) >> 0) & 0xFF); }

// Return an RGBEColor value value with the specified byte component values.
NEX_NODISCARD inline constexpr RGBEColor RGBEColorSet(uint8 r, uint8 g, uint8 b, uint8 e) {
    return static_cast<RGBEColor>((static_cast<uint32>(r) << 24) | (static_cast<uint32>(g) << 16) |
        (static_cast<uint32>(b) << 8) | (static_cast<uint32>(e) << 0));
}

// Floating-point linear color in XYZ space with alpha.
// XYZ was designed to represent all colors visible to the human eye
// Unlike RGB, which varies by device, XYZ is a fixed, standard space. 
struct XYZAColor {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float a = 1.0f;

    constexpr XYZAColor() = default;
    constexpr XYZAColor(float xVal, float yVal, float zVal, float aVal = 1.0f)
        : x(xVal), y(yVal), z(zVal), a(aVal) {};
};

// CMYK color model is a subtractive color model.
// Used in color printing, and is also used to describe the printing process itself.
// The abbreviation CMYK refers to the four ink plates used: cyan, magenta, yellow, and key (most often black).
// Each component's value must be within the range [0.0, 1.0].
struct CMYKColor {
    float cyan = 0.0f;
    float magenta = 0.0f;
    float yellow = 0.0f;
    float key = 0.0f;

    constexpr CMYKColor() = default;
    constexpr CMYKColor(float c, float m, float y, float k) : cyan(c), magenta(m), yellow(y), key(k) {};
};

// HSV/HSL are the two most common cylindrical-coordinate representations of points in an RGB color model.
// The two representations rearrange the geometry of RGB in an attempt to be more intuitive and perceptually 
// relevant than the cartesian (cube) representation. Commonly used in color pickers and in image editing software.
struct HSVColor {
    float hue = 0;           // value in range [0, 360)
    float saturation = 0;    // value in range [0, 1]
    float value = 0;         // value in range [0, 1]
    
    constexpr HSVColor() = default;
    constexpr HSVColor(float h, float s, float v) : hue(h), saturation(s), value(v) {};
};

// HSL (Hue, Saturation, Lightness) color model is similar to HSV but uses lightness instead of value.
// HSL is often more intuitive for artists as lightness directly represents how much white or black is mixed in.
struct HSLColor {
    float hue = 0;           // value in range [0, 360)
    float saturation = 0;    // value in range [0, 1]
    float lightness = 0;     // value in range [0, 1]
    
    constexpr HSLColor() = default;
    constexpr HSLColor(float h, float s, float l) : hue(h), saturation(s), lightness(l) {};
};

// YUV/YCbCr color is a family of color spaces used as a part of the color image pipeline in digital video, 
// broadcasting and photography systems; and it is based on RGB primaries.
// Y is the luma component, Cb/Cr are the blue/red offset chroma channels.
struct YCbCrColor {
    float y = 0.0f;                 // Luma
    float cb = 0.0f, cr = 0.0f;     // Chrominance (blue/red offset)
    
    constexpr YCbCrColor() = default;
    constexpr YCbCrColor(float yVal, float cbVal, float crVal) : y(yVal), cb(cbVal), cr(crVal) {};
};

/**
 * @class Color
 * @brief Color class, using RGBA8888 as the internal format
 * 
 * This class provides a convenient way to store and manipulate colors. It uses RGBAColor
 * (RGBA8888 format, 0xRRGGBBAA) as the internal representation to store color data.
 * 
 * Color supports:
 * - Construction from RGBA components (red, green, blue, alpha)
 * - Factory methods to create from various color formats (ARGB, BGRA, ABGR, RGB, Grayscale, RGBE, XYZ, CMYK, HSV, HSL, YCbCr)
 * - Factory methods to create from hex strings (#RRGGBB, #AARRGGBB, #RGB) and color names
 * - Conversion to/from various color formats
 * - Named color constants (Transparent, Black, White, Red, Green, Blue, etc.)
 * - Arithmetic operations (addition, subtraction, scalar multiplication/division)
 * - Color manipulation operations (darken, lighten, brighten, dim, adjustSaturation, adjustHue, invert, complement)
 * - Color interpolation (linear interpolation between two colors)
 * - Utility methods (luminance calculation, contrast ratio calculation, opacity checking)
 * - Comparison operations
 */
class NEX_EXPORT Color {
private:
    // Internal color value in RGBA format
    RGBAColor clr_;

public:
    // Construction
    constexpr explicit Color() : clr_(0) {}

    // Construct from RGBA components
    constexpr Color(uint8 r, uint8 g, uint8 b, uint8 a = 255) 
        : clr_(RGBAColorSet(r, g, b, a)) {}

    // Copy constructor
    constexpr Color(const Color& other) : clr_(other.clr_) {}

    // Copy assignment operator
    Color& operator=(const Color& other) {
        if (this != &other)
            clr_ = other.clr_;
        return *this;
    }

    // Move constructor
    constexpr Color(Color&& other) noexcept : clr_(other.clr_) {}

    // Move assignment operator
    Color& operator=(Color&& other) noexcept {
        if (this != &other)
            clr_ = other.clr_;
        return *this;
    }

    // Named color constants
public:
    static constexpr Color Transparent() { return Color(0, 0, 0, 0); }
    static constexpr Color Black() { return Color(0, 0, 0, 255); }
    static constexpr Color White() { return Color(255, 255, 255, 255); }
    static constexpr Color Red() { return Color(255, 0, 0, 255); }
    static constexpr Color Green() { return Color(0, 255, 0, 255); }
    static constexpr Color Blue() { return Color(0, 0, 255, 255); }
    static constexpr Color Yellow() { return Color(255, 255, 0, 255); }
    static constexpr Color Cyan() { return Color(0, 255, 255, 255); }
    static constexpr Color Magenta() { return Color(255, 0, 255, 255); }
    static constexpr Color Gray() { return Color(128, 128, 128, 255); }
    static constexpr Color LightGray() { return Color(192, 192, 192, 255); }
    static constexpr Color DarkGray() { return Color(64, 64, 64, 255); }
    static constexpr Color Orange() { return Color(255, 165, 0, 255); }
    static constexpr Color Pink() { return Color(255, 192, 203, 255); }
    static constexpr Color Purple() { return Color(128, 0, 128, 255); }
    static constexpr Color Brown() { return Color(165, 42, 42, 255); }
    static constexpr Color Navy() { return Color(0, 0, 128, 255); }
    static constexpr Color Olive() { return Color(128, 128, 0, 255); }
    static constexpr Color Lime() { return Color(0, 255, 0, 255); }
    static constexpr Color Aqua() { return Color(0, 255, 255, 255); }
    static constexpr Color Silver() { return Color(192, 192, 192, 255); }
    static constexpr Color Maroon() { return Color(128, 0, 0, 255); }
    static constexpr Color Teal() { return Color(0, 128, 128, 255); }

    // Factory method to create Color from other color formats
public:
    // Create Color from RGBAColor
    static Color fromRGBA(const RGBAColor& rgba) { 
        Color clr{}; clr.clr_ = rgba; return clr;
    }
    // Create Color from ARGBColor
    static Color fromARGB(const ARGBColor& argb) {
        Color clr{};
        clr.clr_ = RGBAColorSet(ARGBColor_GetR(argb), ARGBColor_GetG(argb), 
                                ARGBColor_GetB(argb), ARGBColor_GetA(argb));
        return clr;
    }
    // Create Color from BGRAColor
    static Color fromBGRA(const BGRAColor& bgra) {
        Color clr{};
        clr.clr_ = RGBAColorSet(BGRAColor_GetR(bgra), BGRAColor_GetG(bgra), 
                                BGRAColor_GetB(bgra), BGRAColor_GetA(bgra));
        return clr;
    }
    // Create Color from ABGRColor
    static Color fromABGR(const ABGRColor& abgr) {
        Color clr{};
        clr.clr_ = RGBAColorSet(ABGRColor_GetR(abgr), ABGRColor_GetG(abgr), 
                                ABGRColor_GetB(abgr), ABGRColor_GetA(abgr));
        return clr;
    }
    // Create Color from RGBColor
    static Color fromRGB(const RGBColor& rgb) {
        Color clr{};
        clr.clr_ = RGBAColorSet(RGBColor_GetR(rgb), RGBColor_GetG(rgb), 
                                RGBColor_GetB(rgb), 255);
        return clr;
    }
    // Create Color from Grayscale8
    static Color fromGrayscale8(const Grayscale8& gray) {
        Color clr{}; clr.clr_ = RGBAColorSet(gray, gray, gray, 255);
        return clr;
    }
    // Create Color from Grayscale16
    static Color fromGrayscale16(const Grayscale16& gray) {
        uint8 gray8 = static_cast<uint8>(gray >> 8);
        Color clr{}; clr.clr_ = RGBAColorSet(gray8, gray8, gray8, 255);
        return clr;
    }
    // Create Color from RGBEColor
    static Color fromRGBE(const RGBEColor& rgbe) {
        Color clr{};
        clr.clr_ = RGBAColorSet(RGBEColor_GetR(rgbe), RGBEColor_GetG(rgbe), 
                                RGBEColor_GetB(rgbe), 255);
        return clr;
    }
    // Create Color from XYZAColor
    // Uses sRGB D65 white point conversion matrix
    static Color fromXYZA(const XYZAColor& xyza) {
        // XYZ to sRGB conversion matrix (D65 white point)
        float r =  3.2404542f * xyza.x - 1.5371385f * xyza.y - 0.4985314f * xyza.z;
        float g = -0.9692660f * xyza.x + 1.8760108f * xyza.y + 0.0415560f * xyza.z;
        float b =  0.0556434f * xyza.x - 0.2040259f * xyza.y + 1.0572252f * xyza.z;

        // Apply gamma correction (sRGB gamma)
        auto gammaCorrect = [](float c) -> float {
            if (c <= 0.0f) return 0.0f;
            if (c >= 1.0f) return 1.0f;
            if (c <= 0.0031308f) {
                return 12.92f * c;
            } else {
                return 1.055f * NEX_STD pow(c, 1.0f / 2.4f) - 0.055f;
            }
        };

        r = gammaCorrect(r);
        g = gammaCorrect(g);
        b = gammaCorrect(b);

        Color clr{}; 
        clr.clr_ = RGBAColorSet(
            static_cast<uint8>(NEX_STD clamp(r * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8>(NEX_STD clamp(g * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8>(NEX_STD clamp(b * 255.0f, 0.0f, 255.0f)),
            static_cast<uint8>(NEX_STD clamp(xyza.a * 255.0f, 0.0f, 255.0f)));
        return clr;
    }
    // Create Color from CMYKColor
    static Color fromCMYK(const CMYKColor& cmyk) {
        uint8 r = static_cast<uint8>(255 * (1 - cmyk.cyan) * (1 - cmyk.key));
        uint8 g = static_cast<uint8>(255 * (1 - cmyk.magenta) * (1 - cmyk.key));
        uint8 b = static_cast<uint8>(255 * (1 - cmyk.yellow) * (1 - cmyk.key));
        Color clr{}; clr.clr_ = RGBAColorSet(r, g, b, 255);
        return clr;
    }
    // Create Color from HSVColor
    static Color fromHSV(const HSVColor& hsv) {
        float c = hsv.value * hsv.saturation;
        float x = c * (1 - fabs(fmod(hsv.hue / 60.0f, 2) - 1));
        float m = hsv.value - c;
        float r1, g1, b1;

        if (hsv.hue < 60) { r1 = c; g1 = x; b1 = 0; }
        else if (hsv.hue < 120) { r1 = x; g1 = c; b1 = 0; }
        else if (hsv.hue < 180) { r1 = 0; g1 = c; b1 = x; }
        else if (hsv.hue < 240) { r1 = 0; g1 = x; b1 = c; }
        else if (hsv.hue < 300) { r1 = x; g1 = 0; b1 = c; }
        else { r1 = c; g1 = 0; b1 = x; }

        uint8 r = static_cast<uint8>((r1 + m) * 255);
        uint8 g = static_cast<uint8>((g1 + m) * 255);
        uint8 b = static_cast<uint8>((b1 + m) * 255);
        Color clr{}; clr.clr_ = RGBAColorSet(r, g, b, 255);
        return clr;
    }
    // Create Color from HSLColor
    static Color fromHSL(const HSLColor& hsl) {
        float c = (1.0f - fabs(2.0f * hsl.lightness - 1.0f)) * hsl.saturation;
        float x = c * (1.0f - fabs(fmod(hsl.hue / 60.0f, 2.0f) - 1.0f));
        float m = hsl.lightness - c / 2.0f;
        float r1, g1, b1;

        if (hsl.hue < 60.0f) { r1 = c; g1 = x; b1 = 0.0f; }
        else if (hsl.hue < 120.0f) { r1 = x; g1 = c; b1 = 0.0f; }
        else if (hsl.hue < 180.0f) { r1 = 0.0f; g1 = c; b1 = x; }
        else if (hsl.hue < 240.0f) { r1 = 0.0f; g1 = x; b1 = c; }
        else if (hsl.hue < 300.0f) { r1 = x; g1 = 0.0f; b1 = c; }
        else { r1 = c; g1 = 0.0f; b1 = x; }

        uint8 r = static_cast<uint8>((r1 + m) * 255.0f);
        uint8 g = static_cast<uint8>((g1 + m) * 255.0f);
        uint8 b = static_cast<uint8>((b1 + m) * 255.0f);
        Color clr{}; clr.clr_ = RGBAColorSet(r, g, b, 255);
        return clr;
    }
    // Create Color from YCbCrColor
    static Color fromYCbCr(const YCbCrColor& ycbcr) {
        float r = ycbcr.y + 1.402f * (ycbcr.cr - 128.0f);
        float g = ycbcr.y - 0.344136f * (ycbcr.cb - 128.0f) - 0.714136f * (ycbcr.cr - 128.0f);
        float b = ycbcr.y + 1.772f * (ycbcr.cb - 128.0f);

        uint8 r8 = static_cast<uint8>(NEX_STD clamp(r, 0.0f, 255.0f));
        uint8 g8 = static_cast<uint8>(NEX_STD clamp(g, 0.0f, 255.0f));
        uint8 b8 = static_cast<uint8>(NEX_STD clamp(b, 0.0f, 255.0f));
        Color clr{}; clr.clr_ = RGBAColorSet(r8, g8, b8, 255);
        return clr;
    }
    // Create Color from hex string (#RRGGBB or #AARRGGBB)
    static Color fromHex(const Utf8String& hex) {
        Utf8String hexClean = hex;
        // Remove leading '#' if present
        if (!hexClean.empty() && hexClean[0] == '#') {
            hexClean = hexClean.substr(1);
        }
        // Convert to uppercase for easier parsing
        for (char& c : hexClean) {
            c = static_cast<char>(NEX_STD toupper(static_cast<unsigned char>(c)));
        }
        // Parse hex string
        unsigned long value = 0;
        try {
            value = NEX_STD stoul(hexClean, nullptr, 16);
        } catch (...) {
            // Invalid hex string, return black
            return Color(0, 0, 0, 255);
        }
        if (hexClean.length() == 6) {
            // #RRGGBB format
            uint8 r = static_cast<uint8>((value >> 16) & 0xFF);
            uint8 g = static_cast<uint8>((value >> 8) & 0xFF);
            uint8 b = static_cast<uint8>(value & 0xFF);
            return Color(r, g, b, 255);
        } else if (hexClean.length() == 8) {
            // #AARRGGBB format
            uint8 a = static_cast<uint8>((value >> 24) & 0xFF);
            uint8 r = static_cast<uint8>((value >> 16) & 0xFF);
            uint8 g = static_cast<uint8>((value >> 8) & 0xFF);
            uint8 b = static_cast<uint8>(value & 0xFF);
            return Color(r, g, b, a);
        } else if (hexClean.length() == 3) {
            // #RGB format (short form, expand to #RRGGBB)
            uint8 r = static_cast<uint8>(((value >> 8) & 0xF) * 0x11);
            uint8 g = static_cast<uint8>(((value >> 4) & 0xF) * 0x11);
            uint8 b = static_cast<uint8>((value & 0xF) * 0x11);
            return Color(r, g, b, 255);
        }
        // Invalid length, return black
        return Color(0, 0, 0, 255);
    }
    static Color fromHex(const char* hex) {
        return fromHex(hex ? Utf8String(hex) : Utf8String());
    }
    // Create Color from name (case-insensitive)
    static Color fromName(const Utf8String& name) {
        Utf8String nameLower = name;
        for (char& c : nameLower) {
            c = static_cast<char>(NEX_STD tolower(static_cast<unsigned char>(c)));
        }
        // Common color names
        if (nameLower == "transparent") return Transparent();
        if (nameLower == "black") return Black();
        if (nameLower == "white") return White();
        if (nameLower == "red") return Red();
        if (nameLower == "green") return Green();
        if (nameLower == "blue") return Blue();
        if (nameLower == "yellow") return Yellow();
        if (nameLower == "cyan") return Cyan();
        if (nameLower == "magenta") return Magenta();
        if (nameLower == "gray" || nameLower == "grey") return Gray();
        if (nameLower == "lightgray" || nameLower == "lightgrey") return LightGray();
        if (nameLower == "darkgray" || nameLower == "darkgrey") return DarkGray();
        if (nameLower == "orange") return Orange();
        if (nameLower == "pink") return Pink();
        if (nameLower == "purple") return Purple();
        if (nameLower == "brown") return Brown();
        if (nameLower == "navy") return Navy();
        if (nameLower == "olive") return Olive();
        if (nameLower == "lime") return Lime();
        if (nameLower == "aqua") return Aqua();
        if (nameLower == "silver") return Silver();
        if (nameLower == "maroon") return Maroon();
        if (nameLower == "teal") return Teal();
        // If name not found, return black
        return Black();
    }
    static Color fromName(const char* name) {
        return fromName(name ? Utf8String(name) : Utf8String());
    }

    // Color conversion methods
public:
    // Convert to RGBAColor
    RGBAColor toRGBA() const { return clr_; }
    // Convert to ARGBColor
    ARGBColor toARGB() const {
        return ARGBColorSet(RGBAColor_GetA(clr_), RGBAColor_GetR(clr_), 
                            RGBAColor_GetG(clr_), RGBAColor_GetB(clr_));
    }
    // Convert to BGRAColor
    BGRAColor toBGRA() const {
        return BGRAColorSet(RGBAColor_GetB(clr_), RGBAColor_GetG(clr_), 
                            RGBAColor_GetR(clr_), RGBAColor_GetA(clr_));
    }
    // Convert to ABGRColor
    ABGRColor toABGR() const {
        return ABGRColorSet(RGBAColor_GetA(clr_), RGBAColor_GetB(clr_), 
                            RGBAColor_GetG(clr_), RGBAColor_GetR(clr_));
    }
    // Convert to RGBColor
    RGBColor toRGB() const {
        return RGBColorSet(RGBAColor_GetR(clr_), RGBAColor_GetG(clr_), 
                           RGBAColor_GetB(clr_));
    }
    // Convert to Grayscale8
    Grayscale8 toGrayscale8() const {
        uint8 r = RGBAColor_GetR(clr_);
        uint8 g = RGBAColor_GetG(clr_);
        uint8 b = RGBAColor_GetB(clr_);
        return static_cast<Grayscale8>(0.299f * r + 0.587f * g + 0.114f * b);
    }
    // Convert to Grayscale16
    Grayscale16 toGrayscale16() const {
        uint8 gray8 = toGrayscale8();
        return static_cast<Grayscale16>(gray8) << 8;
    }
    // Convert to RGBEColor
    RGBEColor toRGBE() const {
        return RGBEColorSet(RGBAColor_GetR(clr_), RGBAColor_GetG(clr_), 
                            RGBAColor_GetB(clr_), 255);
    }
    // Convert to XYZAColor
    // Uses sRGB D65 white point conversion matrix
    XYZAColor toXYZA() const {
        // Convert sRGB to linear RGB (inverse gamma correction)
        auto linearize = [](float c) -> float {
            if (c <= 0.0f) return 0.0f;
            if (c >= 1.0f) return 1.0f;
            if (c <= 0.04045f) {
                return c / 12.92f;
            } else {
                return NEX_STD pow((c + 0.055f) / 1.055f, 2.4f);
            }
        };

        float r = linearize(static_cast<float>(RGBAColor_GetR(clr_)) / 255.0f);
        float g = linearize(static_cast<float>(RGBAColor_GetG(clr_)) / 255.0f);
        float b = linearize(static_cast<float>(RGBAColor_GetB(clr_)) / 255.0f);

        // sRGB to XYZ conversion matrix (D65 white point)
        float x = 0.4124564f * r + 0.3575761f * g + 0.1804375f * b;
        float y = 0.2126729f * r + 0.7151522f * g + 0.0721750f * b;
        float z = 0.0193339f * r + 0.1191920f * g + 0.9503041f * b;
        float a = static_cast<float>(RGBAColor_GetA(clr_)) / 255.0f;

        return XYZAColor(x, y, z, a);
    }
    // Convert to CMYKColor
    CMYKColor toCMYK() const {
        float r = static_cast<float>(RGBAColor_GetR(clr_)) / 255.0f;
        float g = static_cast<float>(RGBAColor_GetG(clr_)) / 255.0f;
        float b = static_cast<float>(RGBAColor_GetB(clr_)) / 255.0f;

        float k = 1.0f - NEX_STD max({ r, g, b });
        float c = (1.0f - r - k) / (1.0f - k + 1e-10f);
        float m = (1.0f - g - k) / (1.0f - k + 1e-10f);
        float y = (1.0f - b - k) / (1.0f - k + 1e-10f);

        return CMYKColor(c, m, y, k);
    }
    // Convert to HSVColor
    HSVColor toHSV() const {
        float r = static_cast<float>(RGBAColor_GetR(clr_)) / 255.0f;
        float g = static_cast<float>(RGBAColor_GetG(clr_)) / 255.0f;
        float b = static_cast<float>(RGBAColor_GetB(clr_)) / 255.0f;

        float max = NEX_STD max({ r, g, b });
        float min = NEX_STD min({ r, g, b });
        float delta = max - min;

        float h = 0.0f;
        if (delta != 0.0f) {
            if (max == r) {
                h = 60.0f * fmod(((g - b) / delta), 6.0f);
            } else if (max == g) {
                h = 60.0f * (((b - r) / delta) + 2.0f);
            } else {
                h = 60.0f * (((r - g) / delta) + 4.0f);
            }
        }
        if (h < 0.0f) h += 360.0f;

        float s = (max == 0.0f) ? 0.0f : (delta / max);
        float v = max;

        return HSVColor(h, s, v);
    }
    // Convert to HSLColor
    HSLColor toHSL() const {
        float r = static_cast<float>(RGBAColor_GetR(clr_)) / 255.0f;
        float g = static_cast<float>(RGBAColor_GetG(clr_)) / 255.0f;
        float b = static_cast<float>(RGBAColor_GetB(clr_)) / 255.0f;

        float max = NEX_STD max({ r, g, b });
        float min = NEX_STD min({ r, g, b });
        float delta = max - min;

        float h = 0.0f;
        if (delta != 0.0f) {
            if (max == r) {
                h = 60.0f * fmod(((g - b) / delta), 6.0f);
            } else if (max == g) {
                h = 60.0f * (((b - r) / delta) + 2.0f);
            } else {
                h = 60.0f * (((r - g) / delta) + 4.0f);
            }
        }
        if (h < 0.0f) h += 360.0f;

        float l = (max + min) / 2.0f;
        float s = (delta == 0.0f) ? 0.0f : (delta / (1.0f - fabs(2.0f * l - 1.0f)));

        return HSLColor(h, s, l);
    }
    // Convert to YCbCrColor
    YCbCrColor toYCbCr() const {
        float r = static_cast<float>(RGBAColor_GetR(clr_));
        float g = static_cast<float>(RGBAColor_GetG(clr_));
        float b = static_cast<float>(RGBAColor_GetB(clr_));

        float y = 0.299f * r + 0.587f * g + 0.114f * b;
        float cb = 128.0f - 0.168736f * r - 0.331264f * g + 0.5f * b;
        float cr = 128.0f + 0.5f * r - 0.460525f * g - 0.081475f * b;

        return YCbCrColor(y, cb, cr);
    }
    // Convert to hex string (#RRGGBB format, alpha omitted)
    Utf8String toHex() const {
        NEX_STD ostringstream oss;
        oss << "#" << NEX_STD hex << NEX_STD uppercase << NEX_STD setfill('0') 
            << NEX_STD setw(2) << static_cast<int>(getR())
            << NEX_STD setw(2) << static_cast<int>(getG())
            << NEX_STD setw(2) << static_cast<int>(getB());
        return oss.str();
    }
    // Convert to hex string with alpha (#AARRGGBB format)
    Utf8String toHexWithAlpha() const {
        NEX_STD ostringstream oss;
        oss << "#" << NEX_STD hex << NEX_STD uppercase << NEX_STD setfill('0')
            << NEX_STD setw(2) << static_cast<int>(getA())
            << NEX_STD setw(2) << static_cast<int>(getR())
            << NEX_STD setw(2) << static_cast<int>(getG())
            << NEX_STD setw(2) << static_cast<int>(getB());
        return oss.str();
    }
    
public:
    // Get individual color components
    uint8 getR() const { return RGBAColor_GetR(clr_); }
    uint8 getG() const { return RGBAColor_GetG(clr_); }
    uint8 getB() const { return RGBAColor_GetB(clr_); }
    uint8 getA() const { return RGBAColor_GetA(clr_); }

    // Set individual color components
    void setR(uint8 r) { 
        clr_ = RGBAColorSet(r, RGBAColor_GetG(clr_), 
                            RGBAColor_GetB(clr_), RGBAColor_GetA(clr_)); 
    }
    void setG(uint8 g) { 
        clr_ = RGBAColorSet(RGBAColor_GetR(clr_), g, 
                            RGBAColor_GetB(clr_), RGBAColor_GetA(clr_)); 
    }
    void setB(uint8 b) { 
        clr_ = RGBAColorSet(RGBAColor_GetR(clr_), RGBAColor_GetG(clr_), 
                            b, RGBAColor_GetA(clr_)); 
    }
    void setA(uint8 a) { 
        clr_ = RGBAColorSet(RGBAColor_GetR(clr_), RGBAColor_GetG(clr_), 
                            RGBAColor_GetB(clr_), a); 
    }

    // Comparison operators
public:
    // Equality operators
    bool operator==(const Color& other) const noexcept {
        return this->clr_ == other.clr_;
    }
    // Inequality operators
    bool operator!=(const Color& other) const noexcept {
        return !(*this == other);
    }
    // Less-than operator for ordering
    bool operator<(const Color& other) const noexcept {
        return this->clr_ < other.clr_;
    }
    // Less-than-or-equal operator
    bool operator<=(const Color& other) const noexcept {
        return (*this < other || *this == other);
    }
    // Greater-than operator for ordering
    bool operator>(const Color& other) const noexcept {
        return !(*this <= other);
    }
    // Greater-than-or-equal operator
    bool operator>=(const Color& other) const noexcept {
        return !(*this < other);
    }

    // Arithmetic operators
public:
    // Blend two colors (simple alpha blending)
    Color operator+(const Color& other) const noexcept {
        uint8 a1 = this->getA();
        uint8 a2 = other.getA();
        // Compute output alpha (integer math)
        int aOutInt = static_cast<int>(a1) + (static_cast<int>(a2) * (255 - static_cast<int>(a1)) / 255);
        if (aOutInt <= 0) {
            return Color(0, 0, 0, 0);
        }
        uint8 aOut = static_cast<uint8>(aOutInt);

        // Blend each channel, protect against division by zero
        int rNum = (static_cast<int>(this->getR()) * static_cast<int>(a1)) +
                   (static_cast<int>(other.getR()) * static_cast<int>(a2) * (255 - static_cast<int>(a1)) / 255);
        int gNum = (static_cast<int>(this->getG()) * static_cast<int>(a1)) +
                   (static_cast<int>(other.getG()) * static_cast<int>(a2) * (255 - static_cast<int>(a1)) / 255);
        int bNum = (static_cast<int>(this->getB()) * static_cast<int>(a1)) +
                   (static_cast<int>(other.getB()) * static_cast<int>(a2) * (255 - static_cast<int>(a1)) / 255);

        uint8 rOut = static_cast<uint8>(NEX_STD clamp(rNum / aOutInt, 0, 255));
        uint8 gOut = static_cast<uint8>(NEX_STD clamp(gNum / aOutInt, 0, 255));
        uint8 bOut = static_cast<uint8>(NEX_STD clamp(bNum / aOutInt, 0, 255));

        return Color::fromRGBA(RGBAColorSet(rOut, gOut, bOut, aOut));
    }
    Color& operator+=(const Color& other) noexcept {
        *this = *this + other;
        return *this;
    }
    // Subtract color components
    Color operator-(const Color& other) const noexcept {
        uint8 rOut = static_cast<uint8>(NEX_STD max(0, this->getR() - other.getR()));
        uint8 gOut = static_cast<uint8>(NEX_STD max(0, this->getG() - other.getG()));
        uint8 bOut = static_cast<uint8>(NEX_STD max(0, this->getB() - other.getB()));
        uint8 aOut = static_cast<uint8>(NEX_STD max(0, this->getA() - other.getA()));

        return Color::fromRGBA(RGBAColorSet(rOut, gOut, bOut, aOut));
    }
    Color& operator-=(const Color& other) noexcept {
        *this = *this - other;
        return *this;
    }
    // Scale color components by a scalar
    Color operator*(float scalar) const noexcept {
        uint8 rOut = static_cast<uint8>(NEX_STD clamp(this->getR() * scalar, 0.0f, 255.0f));
        uint8 gOut = static_cast<uint8>(NEX_STD clamp(this->getG() * scalar, 0.0f, 255.0f));
        uint8 bOut = static_cast<uint8>(NEX_STD clamp(this->getB() * scalar, 0.0f, 255.0f));
        uint8 aOut = static_cast<uint8>(NEX_STD clamp(this->getA() * scalar, 0.0f, 255.0f));

        return Color::fromRGBA(RGBAColorSet(rOut, gOut, bOut, aOut));
    }
    Color& operator*=(float scalar) noexcept {
        *this = *this * scalar;
        return *this;
    }
    // Divide color components by a scalar
    Color operator/(float scalar) const noexcept {
        if (scalar == 0.0f) {
            return Color::fromRGBA(RGBAColorSet(0, 0, 0, 0));
        }
        uint8 rOut = static_cast<uint8>(NEX_STD clamp(this->getR() / scalar, 0.0f, 255.0f));
        uint8 gOut = static_cast<uint8>(NEX_STD clamp(this->getG() / scalar, 0.0f, 255.0f));
        uint8 bOut = static_cast<uint8>(NEX_STD clamp(this->getB() / scalar, 0.0f, 255.0f));
        uint8 aOut = static_cast<uint8>(NEX_STD clamp(this->getA() / scalar, 0.0f, 255.0f));

        return Color::fromRGBA(RGBAColorSet(rOut, gOut, bOut, aOut));
    }
    Color& operator/=(float scalar) noexcept {
        *this = *this / scalar;
        return *this;
    }

    // Color manipulation methods
public:
    // Darken the color by a factor (0.0 = no change, 1.0 = completely black)
    Color darken(float factor) const noexcept {
        factor = NEX_STD clamp(factor, 0.0f, 1.0f);
        HSLColor hsl = toHSL();
        hsl.lightness = NEX_STD max(0.0f, hsl.lightness - factor * hsl.lightness);
        Color result = fromHSL(hsl);
        result.setA(getA());
        return result;
    }

    // Lighten the color by a factor (0.0 = no change, 1.0 = completely white)
    Color lighten(float factor) const noexcept {
        factor = NEX_STD clamp(factor, 0.0f, 1.0f);
        HSLColor hsl = toHSL();
        hsl.lightness = NEX_STD min(1.0f, hsl.lightness + factor * (1.0f - hsl.lightness));
        Color result = fromHSL(hsl);
        result.setA(getA());
        return result;
    }

    // Brighten the color by adding a fixed amount to lightness
    Color brighten(int amount) const noexcept {
        HSLColor hsl = toHSL();
        hsl.lightness = NEX_STD min(1.0f, hsl.lightness + static_cast<float>(amount) / 255.0f);
        Color result = fromHSL(hsl);
        result.setA(getA());
        return result;
    }

    // Dim the color by subtracting a fixed amount from lightness
    Color dim(int amount) const noexcept {
        HSLColor hsl = toHSL();
        hsl.lightness = NEX_STD max(0.0f, hsl.lightness - static_cast<float>(amount) / 255.0f);
        Color result = fromHSL(hsl);
        result.setA(getA());
        return result;
    }

    // Adjust saturation by a factor (negative values desaturate, positive values saturate)
    Color adjustSaturation(float factor) const noexcept {
        HSLColor hsl = toHSL();
        hsl.saturation = NEX_STD clamp(hsl.saturation * (1.0f + factor), 0.0f, 1.0f);
        Color result = fromHSL(hsl);
        result.setA(getA());
        return result;
    }

    // Adjust hue by degrees (positive = clockwise, negative = counter-clockwise)
    Color adjustHue(float degrees) const noexcept {
        HSLColor hsl = toHSL();
        hsl.hue = fmod(hsl.hue + degrees + 360.0f, 360.0f);
        Color result = fromHSL(hsl);
        result.setA(getA());
        return result;
    }

    // Invert the color (complementary color)
    Color invert() const noexcept {
        uint8 r = 255 - getR();
        uint8 g = 255 - getG();
        uint8 b = 255 - getB();
        return Color(r, g, b, getA());
    }

    // Get a complementary color (opposite hue in HSL space)
    Color complement() const noexcept {
        HSLColor hsl = toHSL();
        hsl.hue = fmod(hsl.hue + 180.0f, 360.0f);
        Color result = fromHSL(hsl);
        result.setA(getA());
        return result;
    }

    // Color interpolation
public:
    // Linear interpolation between two colors (t should be in range [0.0, 1.0])
    static Color lerp(const Color& color1, const Color& color2, float t) noexcept {
        t = NEX_STD clamp(t, 0.0f, 1.0f);
        float r = color1.getR() * (1.0f - t) + color2.getR() * t;
        float g = color1.getG() * (1.0f - t) + color2.getG() * t;
        float b = color1.getB() * (1.0f - t) + color2.getB() * t;
        float a = color1.getA() * (1.0f - t) + color2.getA() * t;
        return Color(
            static_cast<uint8>(r),
            static_cast<uint8>(g),
            static_cast<uint8>(b),
            static_cast<uint8>(a)
        );
    }

    // Utility methods
public:
    // Calculate relative luminance (for accessibility, returns value in range [0.0, 1.0])
    // Uses sRGB relative luminance formula from WCAG 2.0
    float luminance() const noexcept {
        float r = static_cast<float>(getR()) / 255.0f;
        float g = static_cast<float>(getG()) / 255.0f;
        float b = static_cast<float>(getB()) / 255.0f;
        
        // Apply gamma correction
        r = (r <= 0.03928f) ? (r / 12.92f) : NEX_STD pow((r + 0.055f) / 1.055f, 2.4f);
        g = (g <= 0.03928f) ? (g / 12.92f) : NEX_STD pow((g + 0.055f) / 1.055f, 2.4f);
        b = (b <= 0.03928f) ? (b / 12.92f) : NEX_STD pow((b + 0.055f) / 1.055f, 2.4f);
        
        // Calculate relative luminance
        return 0.2126f * r + 0.7152f * g + 0.0722f * b;
    }

    // Calculate contrast ratio between two colors (for accessibility)
    // Returns a value typically between 1.0 (no contrast) and 21.0 (maximum contrast)
    // WCAG 2.0 requires at least 4.5:1 for normal text and 3:1 for large text
    static float contrastRatio(const Color& color1, const Color& color2) noexcept {
        float lum1 = color1.luminance();
        float lum2 = color2.luminance();
        float lighter = NEX_STD max(lum1, lum2);
        float darker = NEX_STD min(lum1, lum2);
        // Avoid division by zero
        if (darker == 0.0f) return (lighter > 0.0f) ? 21.0f : 1.0f;
        return (lighter + 0.05f) / (darker + 0.05f);
    }

    // Check if color is valid (alpha channel check, though all RGBA values are technically valid)
    // This method can be extended to check for specific validity rules
    bool isValid() const noexcept {
        // All RGBA values are technically valid, but we can check for special cases
        // For now, just return true. Can be extended to check for specific ranges or conditions
        return true;
    }

    // Check if color is fully opaque (alpha == 255)
    bool isOpaque() const noexcept {
        return getA() == 255;
    }

    // Check if color is fully transparent (alpha == 0)
    bool isTransparent() const noexcept {
        return getA() == 0;
    }
};

NEX_CORE_NAMESPACE_END
