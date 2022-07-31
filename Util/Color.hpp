#pragma once

#include "Config.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>

namespace Util {

struct Colorf;
struct HSV;

class Color {
public:
    uint8_t r {}, g {}, b {}, a { 255 };

    constexpr Color() = default;

    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r)
        , g(g)
        , b(b)
        , a(a)
    {
    }

    constexpr Color(uint32_t color)
        : r((color >> 24) & 0xff)
        , g((color >> 16) & 0xff)
        , b((color >> 8) & 0xff)
        , a((color >> 0) & 0xff)
    {
    }

    constexpr Color(Colorf const&);

    constexpr Color operator+(Color const& right) const
    {
        return { static_cast<uint8_t>(std::clamp(
                     static_cast<int>(r) + static_cast<int>(right.r), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(g) + static_cast<int>(right.g), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(b) + static_cast<int>(right.b), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(a) + static_cast<int>(right.a), 0, 255)) };
    }

    constexpr Color operator-(Color const& right) const
    {
        return { static_cast<uint8_t>(std::clamp(
                     static_cast<int>(r) - static_cast<int>(right.r), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(g) - static_cast<int>(right.g), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(b) - static_cast<int>(right.b), 0, 255)),
            static_cast<uint8_t>(std::clamp(
                static_cast<int>(a) - static_cast<int>(right.a), 0, 255)) };
    }

    constexpr HSV to_hsv() const;
    constexpr Color to_grayscale(float saturation = 0) const;

    friend std::ostream& operator<<(std::ostream& out, Color const& color)
    {
        return out << "rgba(" << static_cast<int>(color.r) << ", "
                   << static_cast<int>(color.g) << ", " << static_cast<int>(color.b)
                   << ", " << static_cast<int>(color.a) << ")";
    }
};

struct HSV {
    int hue;          // 0 - 360
    float saturation; // 0 - 1
    float value;      // 0 - 1

    constexpr Color to_rgb() const;
};

constexpr Color operator*(Color const& left, auto fac)
{
    using Fac = decltype(fac);
    return { static_cast<uint8_t>(std::min<Fac>(255, left.r * fac)),
        static_cast<uint8_t>(std::min<Fac>(255, left.g * fac)),
        static_cast<uint8_t>(std::min<Fac>(255, left.b * fac)), left.a };
}

class Colors {
public:
    static constexpr Color transparent { 0, 0, 0, 0 };

    static constexpr Color white { 255, 255, 255 };
    static constexpr Color black {};

    static constexpr Color red { 255, 0, 0 };
    static constexpr Color green { 0, 255, 0 };
    static constexpr Color blue { 0, 0, 255 };

    static constexpr Color cyan { 0, 255, 255 };
    static constexpr Color magenta { 255, 0, 255 };
    static constexpr Color yellow { 255, 255, 0 };
};

struct Colorf {
    constexpr Colorf() = default;
    constexpr Colorf(float r_, float g_, float b_, float a_ = 1)
        : r(r_)
        , g(g_)
        , b(b_)
        , a(a_)
    {
    }

    constexpr Colorf(Color const& color)
        : r(color.r / 255.f)
        , g(color.g / 255.f)
        , b(color.b / 255.f)
        , a(color.a / 255.f)
    {
    }

    float r, g, b, a;
};

constexpr Color HSV::to_rgb() const
{
    auto c = value * saturation;
    auto x = c * (1 - std::abs((hue / 60) % 2 - 1));
    auto m = value - c;
    Colorf color = [&]() -> Colorf {
        if (hue < 60)
            return { c, x, 0 };
        if (hue < 120)
            return { x, c, 0 };
        if (hue < 180)
            return { 0, c, x };
        if (hue < 240)
            return { 0, x, c };
        if (hue < 300)
            return { x, 0, c };
        return { c, 0, x };
    }();
    color.r += m;
    color.g += m;
    color.b += m;
    return color;
}

constexpr Color::Color(Colorf const& color)
    : r(color.r * 255)
    , g(color.g * 255)
    , b(color.b * 255)
    , a(color.a * 255)
{
}

constexpr HSV Color::to_hsv() const
{
    Colorf p { *this };
    auto cmax = std::max(p.r, std::max(p.g, p.b));
    auto cmin = std::min(p.r, std::min(p.g, p.b));
    auto delta = cmax - cmin;
    HSV hsv;
    hsv.hue = [&]() -> double {
        if (delta == 0)
            return 0;
        if (cmax == p.r)
            return 60 * std::fmod((p.g - p.b) / delta, 6);
        if (cmax == p.g)
            return 60 * ((p.b - p.r) / delta + 2);
        if (cmax == p.b)
            return 60 * ((p.b - p.g) / delta + 4);
        ESSA_UNREACHABLE;
    }();
    hsv.saturation = cmax == 0 ? 0 : delta / cmax;
    hsv.value = cmax;
    return hsv;
}

constexpr Color Color::to_grayscale(float saturation) const
{
    auto hsv = to_hsv();
    hsv.saturation = std::clamp<float>(saturation, 0, 1);
    return hsv.to_rgb();
}

} // namespace Util
