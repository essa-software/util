#pragma once

#include "UString.hpp"

#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <span>

namespace Util {

class Buffer {
public:
    Buffer() = default;
    ~Buffer();
    Buffer(Buffer const&);
    Buffer& operator=(Buffer const&);
    Buffer(Buffer&&);
    Buffer& operator=(Buffer&&);

    Buffer(std::span<uint8_t const>);
    Buffer(std::initializer_list<uint8_t>);
    static Buffer uninitialized(size_t size);
    static Buffer filled(size_t size, uint8_t byte = 0);

    std::span<uint8_t> span() { return { m_data, m_size }; }
    std::span<uint8_t const> span() const { return { m_data, m_size }; }
    size_t size() const { return m_size; }
    auto begin() { return m_data; }
    auto begin() const { return m_data; }
    auto end() { return m_data + m_size; }
    auto end() const { return m_data + m_size; }

    uint8_t operator[](size_t idx) const { return m_data[idx]; }
    uint8_t& operator[](size_t idx) { return m_data[idx]; }

    void append(uint8_t);
    void append(std::span<uint8_t const>);
    void resize_uninitialized(size_t);
    UString decode(UString::Encoding = UString::Encoding::Utf8) const;

    bool operator==(Buffer const& other) const;

private:
    explicit Buffer(size_t size)
        : m_size(size) { resize_uninitialized(size); }

    uint8_t* m_data { nullptr };
    size_t m_size { 0 };
};

}

template<>
class fmt::formatter<Util::Buffer> : public fmt::formatter<std::string_view> {
public:
    template<typename FormatContext>
    constexpr auto format(Util::Buffer const& p, FormatContext& ctx) const {
        fmt::format_to(ctx.out(), "{{");
        constexpr size_t FirstBytesToPrint = 10;
        for (auto ptr = p.begin(); ptr < p.end() && ptr < p.begin() + FirstBytesToPrint; ptr++) {
            fmt::format_to(ctx.out(), "{:02x}", *ptr);
            if (ptr != p.end() - 1)
                fmt::format_to(ctx.out(), " ");
        }
        if (p.size() > FirstBytesToPrint) {
            fmt::format_to(ctx.out(), "...");
        }
        fmt::format_to(ctx.out(), "}}");
        return ctx.out();
    }
};
