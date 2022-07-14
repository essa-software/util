#pragma once

#include <array>
#include <cmath>
#include <initializer_list>
#include <ostream>

namespace Util {

namespace Detail {

template<size_t C, class T>
class Vector {
public:
    static constexpr size_t Components = C;
    
    constexpr Vector()
    {
        if constexpr (Components == 4)
            components[3] = 1;
    }

    template<class... T2>
    constexpr Vector(T2... packed_c) requires(sizeof...(packed_c) == Components || (Components == 4 && sizeof...(packed_c) == 3))
    {
        auto c = std::initializer_list<T> { static_cast<T>(packed_c)... };
        std::copy(std::begin(c), std::end(c), components.begin());
        if constexpr (Components == 4 && sizeof...(packed_c) == 3)
            components[3] = 1;
    }

    template<size_t OtherC, class OtherT, class... MoreT>
    requires(OtherC + sizeof...(MoreT) == Components || (Components == 4 && OtherC + sizeof...(MoreT) == 3)) constexpr explicit Vector(Vector<OtherC, OtherT> const& other, MoreT... more)
    {
        std::copy(other.components.begin(), other.components.end(), components.begin());
        auto c = std::initializer_list<T> { static_cast<T>(more)... };
        std::copy(std::begin(c), std::end(c), components.begin() + OtherC);
        if constexpr (Components == 4 && OtherC + sizeof...(MoreT) == 3)
            components[3] = 1;
    }

    T& x() requires(Components > 0) { return this->components[0]; };
    T& y() requires(Components > 1) { return this->components[1]; };
    T& z() requires(Components > 2) { return this->components[2]; };
    T& w() requires(Components > 3) { return this->components[3]; };

    T const& x() const requires(Components > 0) { return this->components[0]; };
    T const& y() const requires(Components > 1) { return this->components[1]; };
    T const& z() const requires(Components > 2) { return this->components[2]; };
    T const& w() const requires(Components > 3) { return this->components[3]; };

    auto length_squared() const
    {
        double result = 0;
        for (size_t s = 0; s < Components; s++)
            result += components[s] * components[s];
        return result;
    }

    auto length() const
    {
        return std::sqrt(length_squared());
    }

    double inverted_length() const
    {
        return 1 / length();
    }

    auto normalized() const
    {
        return *this * inverted_length();
    }

    bool is_normalized() const
    {
        auto length = length_squared();
        return length == 1 || length == 0;
    }

    double dot(Vector const& a) const
    {
        double result = 0;
        for (size_t s = 0; s < Components; s++)
            result += components[s] * a.components[s];
        return result;
    }

    inline Vector operator+(Vector const& b) const
    {
        Vector ab;
        for (size_t s = 0; s < Components; s++)
            ab.components[s] = components[s] + b.components[s];
        return ab;
    }

    inline Vector& operator+=(Vector const& b)
    {
        return *this = *this + b;
    }

    inline Vector operator-(Vector const& b) const
    {
        Vector ab;
        for (size_t s = 0; s < Components; s++)
            ab.components[s] = components[s] - b.components[s];
        return ab;
    }

    inline Vector& operator-=(Vector const& b)
    {
        return *this = *this - b;
    }

    inline Vector operator*(double x) const
    {
        Vector ab;
        for (size_t s = 0; s < Components; s++)
            ab.components[s] = components[s] * x;
        return ab;
    }

    inline Vector& operator*=(double x)
    {
        return *this = *this * x;
    }

    inline Vector operator/(double x) const
    {
        Vector ab;
        for (size_t s = 0; s < Components; s++)
            ab.components[s] = components[s] / x;
        return ab;
    }

    inline Vector& operator/=(double x)
    {
        return *this = *this / x;
    }

    inline Vector operator-() const
    {
        Vector ap;
        for (size_t s = 0; s < Components; s++)
            ap.components[s] = -components[s];
        return ap;
    }

    //// Vector2 ////
    template<size_t OtherC, class OtherT>
    requires(Components == 2 && OtherC >= 2) constexpr explicit Vector(Vector<OtherC, OtherT> other)
        : Vector { other.x(), other.y() }
    {
    }

    // Angle is CCW starting from positive X axis.
    constexpr static Vector create_polar(double angle_radians, double length) requires(Components == 2)
    {
        return { std::sin(angle_radians) * length, std::cos(angle_radians) * length };
    }

    constexpr double angle() const requires(Components == 2)
    {
        return std::atan2(this->y(), this->x());
    }

    template<class OtherT = T>
    requires(Components == 2) constexpr Vector<2, OtherT> rotate(double theta) const
    {
        double t_cos = std::cos(theta), t_sin = std::sin(theta);
        return { this->x() * t_cos - this->y() * t_sin, this->x() * t_sin + this->y() * t_cos };
    }

    template<class OtherT = T>
    requires(Components == 2) constexpr auto perpendicular()
    {
        return Vector<2, OtherT> { -this->y(), this->x() };
    }

    template<class OtherT>
    requires(Components == 2) constexpr auto mirror(Vector<2, OtherT> axis)
    {
        return *this - (decltype(this->x()))2 * dot(axis.normalized()) * axis;
    }

    //// Vector3 ////
    template<size_t OtherC, class OtherT>
    requires(OtherC >= 3) constexpr explicit Vector(Vector<OtherC, OtherT> other)
        : Vector { other.x(), other.y(), other.z() }
    {
    }

    constexpr static Vector create_spheric(double lat_radians, double lon_radians, double radius) requires(Components == 3)
    {
        return {
            static_cast<T>(radius * std::cos(lat_radians) * std::sin(lon_radians)),
            static_cast<T>(radius * std::sin(lat_radians) * std::sin(lon_radians)),
            static_cast<T>(radius * std::cos(lon_radians)),
        };
    }

    template<class OtherT = T>
    requires(Components == 3) constexpr Vector<3, OtherT> rotate(double theta) const
    {
        double t_cos = std::cos(theta), t_sin = std::sin(theta);
        return { this->x() * t_cos - this->y() * t_sin, this->x() * t_sin + this->y() * t_cos, this->z() };
    }

    template<class OtherT = T>
    requires(Components == 3) constexpr Vector<3, OtherT> cross(Vector<3, T> const& a) const
    {
        Vector<3, OtherT> result;
        result.x() = this->y() * a.z() - this->z() * a.y();
        result.y() = this->z() * a.x() - this->x() * a.z();
        result.z() = this->x() * a.y() - this->y() * a.x();
        return result;
    }

    //// Vector4 ////

    template<size_t OtherC, class OtherT>
    requires(Components == 4 && OtherC >= 4) constexpr explicit Vector(Vector<OtherC, OtherT> other)
        : Vector { other.x(), other.y(), other.z(), other.w() }
    {
    }

    bool operator==(Vector const&) const = default;

    friend std::ostream& operator<<(std::ostream& out, Vector const& v)
    {
        out << "[";
        for (size_t s = 0; s < Components; s++) {
            out << v.components[s];
            if (s != Components - 1)
                out << ", ";
        }
        return out << "]";
    }

    std::array<T, Components> components {};
};

}

template<class T>
using Vector2 = Detail::Vector<2, T>;
using Vector2i = Vector2<int>;
using Vector2u = Vector2<unsigned>;
using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

template<class T>
using Vector3 = Detail::Vector<3, T>;
using Vector3i = Vector3<int>;
using Vector3u = Vector3<unsigned>;
using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

template<class T>
using Vector4 = Detail::Vector<4, T>;
using Vector4i = Vector4<int>;
using Vector4u = Vector4<unsigned>;
using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;

template<size_t S, class T>
inline double get_distance(Detail::Vector<S, T> const& a, Detail::Vector<S, T> const& b)
{
    return (a - b).length();
}

template<size_t S, class T>
requires(S == 2 || S == 3) inline double get_distance_to_line(Detail::Vector<S, T> line_start, Detail::Vector<S, T> line_end, Detail::Vector<S, T> point)
{
    auto d = (point - line_end) / get_distance(point, line_end);
    auto v = line_start - line_end;
    double t = v.dot(d);
    auto p = line_end + t * d;

    return get_distance(p, line_start);
}

}
