#pragma once

#include <ostream>
namespace Util {

class Length {
public:
    enum Unit {
        Initial,
        Auto,
        Px,
        Percent
    };

    constexpr Length() = default;

    constexpr Length(Unit unit)
        : m_unit(unit) {
    }

    constexpr Length(float v, Unit unit)
        : m_value(v)
        , m_unit(unit) {
    }

    constexpr Unit unit() const { return m_unit; }
    constexpr float value() const { return m_value; }

    constexpr bool operator==(Length const& other) const { return m_unit == other.m_unit && m_value == other.m_value; }

private:
    friend std::ostream& operator<<(std::ostream& out, Length const& l) { return out << l.m_value << "U" << (int)l.unit(); }

    float m_value = 0;
    Unit m_unit = Auto;
};

}

constexpr Util::Length operator""_px(long double v) {
    return Util::Length(v, Util::Length::Px);
}

constexpr Util::Length operator""_perc(long double v) {
    return Util::Length(v, Util::Length::Percent);
}
