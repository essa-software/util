#pragma once

#include "UString.hpp"

namespace Util
{

struct Unit {
    UString string;
    double multiplier;
};

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
UString to_exponent_string(double value);

struct UnitValue
{
    UString value {};
    UString unit {};

    UString to_string() const { return value + " " + unit; }
};

// Display value in the form "value[ × 10^n] <unit>", using predefined units.
UnitValue unit_display(double value, Quantity);

// Display value in the form "value[ × 10^n] <unit>", using a user-given unit.
UnitValue unit_display(double value, std::vector<Unit>);

}
