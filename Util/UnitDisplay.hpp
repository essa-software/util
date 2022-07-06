#pragma once

#include <string>

namespace Util
{

enum class Quantity
{
    Length,
    Mass,
    Velocity,
    Time,
    FileSize,

    None,
};

// All strings here are UTF-8 strings.
// FIXME: Add some replacement for SFML sf::String which is a *true* unicode string.
std::string to_exponent_string(double value);

struct UnitValue
{
    std::string value {};
    std::string unit {};

    std::string to_string() const { return value + " " + unit; }
};

UnitValue unit_display(double value, Quantity);

}
