#pragma once

#include "Vector.hpp"
#include <cmath>
#include <random>

namespace Util::Random {

inline auto& default_engine() {
    static std::default_random_engine rng;
    return rng;
}

template<std::floating_point T>
T floating(float min, float max) {
    return std::uniform_real_distribution<T>(min, max)(default_engine());
}

template<class T>
Util::Vector2<T> vector2(float length) {
    double angle = floating<T>(0, 2 * M_PI);
    double x;
    double y;
    sincos(angle, &x, &y);
    return Util::Vector2<T> { x, y } * length;
}

}
