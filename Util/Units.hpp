#pragma once

#include "Constants.hpp"

#include <cmath>
#include <ostream>
#include <string>

class Length
{
public:
    enum Unit
    {
        Initial,
        Auto,
        Px,
        PxOtherSide,
        Percent
    };

    constexpr Length() = default;

    constexpr Length(Unit unit)
        : m_unit(unit)
    {
    }

    constexpr Length(float v, Unit unit)
        : m_value(v)
        , m_unit(unit)
    {
    }

    constexpr Unit unit() const { return m_unit; }
    constexpr float value() const { return m_value; }

    constexpr bool operator==(Length const& other) const { return m_unit == other.m_unit && m_value == other.m_value; }

private:
    friend std::ostream& operator<<(std::ostream& out, Length const& l) { return out << l.m_value << "U" << (int)l.unit(); }

    float m_value = 0;
    Unit m_unit = Auto;
};

constexpr Length operator""_px(long double v)
{
    return Length(v, Length::Px);
}

constexpr Length operator""_px_o(long double v)
{
    return Length(v, Length::PxOtherSide);
}

constexpr Length operator""_perc(long double v)
{
    return Length(v, Length::Percent);
}

class Distance
{
public:
    enum Unit
    {
        Meter,
        Kilometer,
        Au
    };
    
    constexpr Distance() = default;

    constexpr Distance(float v, Unit unit)
        : m_unit(unit)
        , m_value(v)
    {
    }

    constexpr Unit unit() const { return m_unit; }
    constexpr float value() const { return m_value; }

    constexpr Distance operator-() const { return { -m_value, m_unit }; }
    constexpr bool operator==(Distance const& other) const { return m_unit == other.m_unit && m_value == other.m_value; }

    friend std::ostream& operator<<(std::ostream& out, Distance dist)
    {
        if (dist.m_unit == Meter)
            return out << dist.value() << " m";
        else if (dist.m_unit == Kilometer)
            return out << dist.value() / 1000 << " km";
        else
            return out << dist.value() / AU << " AU";
    }

private:
    Unit m_unit = Meter;
    float m_value = 0;
};

constexpr Distance operator""_m(long double v)
{
    return Distance(v, Distance::Meter);
}

constexpr Distance operator""_km(long double v)
{
    return Distance(v * 1000, Distance::Kilometer);
}

constexpr Distance operator""_AU(long double v)
{
    return Distance(v * AU, Distance::Au);
}
