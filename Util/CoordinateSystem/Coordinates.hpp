#pragma once

#include "../Angle.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <initializer_list>

namespace Util {

namespace Detail {

// A generic class containing some numbers.
template<size_t C, class T, template<size_t, class> class Derived>
class Coordinates {
public:
    static constexpr size_t Components = C;

    constexpr Coordinates() {
        if constexpr (Components == 4) {
            m_components[3] = 1;
        }
    }

    template<class... T2>
    constexpr Coordinates(T2... packed_c)
        requires(sizeof...(packed_c) == Components || (Components == 4 && sizeof...(packed_c) == 3))
    {
        auto c = std::initializer_list<T> { static_cast<T>(packed_c)... };
        std::copy(std::begin(c), std::end(c), m_components.begin());
        if constexpr (Components == 4 && sizeof...(packed_c) == 3) {
            m_components[3] = 1;
        }
        assert_components_are_finite();
    }

    template<size_t OtherC, class OtherT, class... MoreT>
        requires(OtherC + sizeof...(MoreT) == Components || (Components == 4 && OtherC + sizeof...(MoreT) == 3))
    constexpr explicit Coordinates(Coordinates<OtherC, OtherT, Derived> const& other, MoreT... more) {
        std::copy(other.components.begin(), other.components.end(), m_components.begin());
        auto c = std::initializer_list<T> { static_cast<T>(more)... };
        std::copy(std::begin(c), std::end(c), m_components.begin() + OtherC);
        if constexpr (Components == 4 && OtherC + sizeof...(MoreT) == 3) {
            m_components[3] = 1;
        }
        assert_components_are_finite();
    }

    template<class OtherT>
    Derived<C, OtherT> cast() const {
        Derived<C, OtherT> v;
        for (size_t s = 0; s < C; s++) {
            v.set_component(s, static_cast<OtherT>(this->component(s)));
        }
        return v;
    }

    // Component getters
    template<size_t I>
    constexpr T component() const {
        static_assert(I < Components);
        return this->m_components[I];
    }
    constexpr T component(size_t s) const {
        assert(s < Components);
        return this->m_components[s];
    }
    constexpr T x() const
        requires(Components > 0)
    { return this->m_components[0]; };
    constexpr T y() const
        requires(Components > 1)
    { return this->m_components[1]; };
    constexpr T z() const
        requires(Components > 2)
    { return this->m_components[2]; };
    constexpr T w() const
        requires(Components > 3)
    { return this->m_components[3]; };

    // Component setters
    template<size_t I>
    constexpr void set_component(float v)
        requires(Components > 0)
    {
        assert(std::isfinite(v));
        static_assert(I < Components);
        this->m_components[I] = v;
    };
    constexpr void set_component(size_t s, float v)
        requires(Components > 0)
    {
        assert(std::isfinite(v));
        assert(s < Components);
        this->m_components[s] = v;
    };
    constexpr void set_x(float v)
        requires(Components > 0)
    { set_component(0, v); }
    constexpr void set_y(float v)
        requires(Components > 1)
    { set_component(1, v); }
    constexpr void set_z(float v)
        requires(Components > 2)
    { set_component(2, v); }
    constexpr void set_w(float v)
        requires(Components > 3)
    { set_component(3, v); }

    bool is_zero() const {
        return std::ranges::all_of(m_components, [](auto n) { return n == 0; });
    }

    bool is_approximately_equal(Derived<C, T> const& other, float epsilon = 10e-6) {
        for (size_t s = 0; s < Components; s++) {
            if (std::abs(other.m_components[s] - m_components[s]) > epsilon) {
                return false;
            }
        }
        return true;
    }

    auto floored() const {
        Derived<C, T> output;
        for (size_t s = 0; s < Components; s++) {
            output.set_component(s, std::floor(this->component(s)));
        }
        return output;
    }

    auto ceiled() const {
        Derived<C, T> output;
        for (size_t s = 0; s < Components; s++) {
            output.set_component(s, std::ceil(this->component(s)));
        }
        return output;
    }

    auto rounded() const {
        Derived<C, T> output;
        for (size_t s = 0; s < Components; s++) {
            output.set_component(s, std::round(this->component(s)));
        }
        return output;
    }

    bool operator==(Coordinates const&) const = default;

private:
    void assert_components_are_finite() const {
        for (auto c : m_components) {
            assert(std::isfinite(c));
        }
    }

    std::array<T, Components> m_components {};
};

} // Detail

} // Util
