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

    template<class U>
    explicit Rect(Rect<U> const& other)
        : left(other.left)
        , top(other.top)
        , width(other.width)
        , height(other.height)
    {
    }

    Vector2<T> position() const { return { left, top }; }
    Vector2<T> size() const { return { width, height }; }

    template<class U = T>
    bool contains(Vector2<U> pos) const
    {
        return pos.x() >= left && pos.x() <= left + width && pos.y() >= top && pos.y() <= top + height;
    }

    template<class U = T>
    Rect<U> intersection(Rect<U> const& other) const
    {
        T max_x = std::max<U>(left, other.left);
        T max_y = std::max<U>(top, other.top);

        T min_x = std::min<U>(left + width, other.left + other.width);
        T min_y = std::min<U>(top + height, other.top + other.height);

        return max_x < min_x && max_y < min_y ? Rect<U>({ max_x, max_y }, { min_x - max_x, min_y - max_y }) : Rect<U>();
    }
};

using Recti = Rect<int>;
using Rectf = Rect<float>;
using Rectd = Rect<double>;

}
