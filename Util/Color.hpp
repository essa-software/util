#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>

namespace Util {

struct Colorf;

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

    constexpr Color(Colorf const&);

    constexpr Color operator+(Color const& right) const
    {
        return { static_cast<uint8_t>(std::clamp(r + right.r, 0, 255)),
            static_cast<uint8_t>(std::clamp(g + right.g, 0, 255)),
            static_cast<uint8_t>(std::clamp(b + right.b, 0, 255)) };
    }

    constexpr Color operator-(Color const& right) const
    {
        return { static_cast<uint8_t>(std::clamp(r - right.r, 0, 255)),
            static_cast<uint8_t>(std::clamp(g - right.g, 0, 255)),
            static_cast<uint8_t>(std::clamp(b - right.b, 0, 255)) };
    }

    friend std::ostream& operator<<(std::ostream& out, Color const& color)
    {
        return out << "rgba(" << static_cast<int>(color.r) << ", " << static_cast<int>(color.g) << ", " << static_cast<int>(color.b) << ", " << static_cast<int>(color.a) << ")";
    }
};

constexpr Color operator*(Color const& left, auto fac)
{
    using Fac = decltype(fac);
    return { static_cast<uint8_t>(std::min<Fac>(255, left.r * fac)),
        static_cast<uint8_t>(std::min<Fac>(255, left.g * fac)),
        static_cast<uint8_t>(std::min<Fac>(255, left.b * fac)),
        left.a };
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
    Colorf() = default;
    Colorf(float r_, float g_, float b_, float a_ = 1)
        : r(r_)
        , g(g_)
        , b(b_)
        , a(a_)
    {
    }

    Colorf(Color const& color)
        : r(color.r / 255.f)
        , g(color.g / 255.f)
        , b(color.b / 255.f)
        , a(color.a / 255.f)
    {
    }

    float r, g, b, a;
};

constexpr Color::Color(Colorf const& color)
    : r(color.r * 255)
    , g(color.g * 255)
    , b(color.b * 255)
    , a(color.a * 255)
{
}

}
