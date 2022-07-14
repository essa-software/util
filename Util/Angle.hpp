#pragma once

#include <cmath>
#include <ostream>

namespace Util {

template<class T>
constexpr T degrees_in_radian = 180 / M_PI;

constexpr auto rad_to_deg(auto radians)
{
    return radians * degrees_in_radian<decltype(radians)>;
}

constexpr auto deg_to_rad(auto degrees)
{
    return degrees / degrees_in_radian<decltype(degrees)>;
}

class Angle {
public:
    enum Unit {
        Rad,
        Deg
    };

    constexpr Angle() = default;

    constexpr Angle(float v, Unit unit)
        : m_value_in_radians(unit == Deg ? deg_to_rad(v) : v)
        , m_unit(unit)
    {
    }

    constexpr float deg() const { return rad_to_deg(m_value_in_radians); }
    constexpr float rad() const { return m_value_in_radians; }

    constexpr Angle operator-() const
    {
        return Angle(-m_value_in_radians, m_unit);
    }

    friend std::ostream& operator<<(std::ostream& out, Angle alfa)
    {
        if (alfa.m_unit == Rad)
            return out << alfa.rad() << " [rad]";
        else
            return out << alfa.rad() << " [deg]";
    }

private:
    float m_value_in_radians = 0;
    Unit m_unit = Rad;
};

}

constexpr Util::Angle operator""_deg(long double v)
{
    return Util::Angle(v, Util::Angle::Deg);
}

constexpr Util::Angle operator""_rad(long double v)
{
    return Util::Angle(v, Util::Angle::Rad);
}
