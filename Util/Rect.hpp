#pragma once

#include <concepts>

#include "Vector.hpp"

namespace Util {

template<class T>
class Rect {
public:
    T left {};
    T top {};
    T width {};
    T height {};

    Rect() = default;

    Rect(T left_, T top_, T width_, T height_)
        : left(left_)
        , top(top_)
        , width(width_)
        , height(height_)
    {
    }

    Rect(Vector2<T> position, Vector2<T> size)
        : left(position.x())
        , top(position.y())
        , width(size.x())
        , height(size.y())
    {
    }

    Vector2<T> position() const { return { left, top }; }
    Vector2<T> size() const { return { width, height }; }

    template<class U>
    bool contains(Vector2<U> pos) const
    {
        return pos.x >= left && pos.x <= left + width && pos.y >= top && pos.y <= top + height;
    }
};

using Recti = Rect<int>;
using Rectf = Rect<float>;
using Rectd = Rect<double>;

}
