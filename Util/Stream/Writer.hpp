#pragma once

#include "../Endianness.hpp"
#include "../UString.hpp"
#include "Stream.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

namespace Util {

class Writer {
public:
    explicit Writer(WritableStream& stream, UString::Encoding encoding = UString::Encoding::Utf8)
        : m_stream(stream)
        , m_encoding(encoding) {
    }

    WritableStream& stream() const { return m_stream; }

    OsErrorOr<size_t> write(std::span<uint8_t const>);
    OsErrorOr<void> write_all(std::span<uint8_t const>);
    OsErrorOr<void> write(UString const&);

    // TODO: Make this fallible
    template<class... Args>
    void writeff(fmt::format_string<Args...> fmt, Args&&... args) {
        return vwriteff(fmt, fmt::make_format_args(args...));
    }

    template<class T>
    requires requires(T t) {
        { convert_from_host_to_little_endian(t) } -> std::same_as<T>;
    }
    OsErrorOr<void> write_little_endian(T const value) {
        auto converted_value = convert_from_host_to_little_endian(value);
        return write_all({ reinterpret_cast<uint8_t const*>(&converted_value), sizeof(T) });
    }

    template<class T>
    requires requires(T t) {
        { convert_from_host_to_big_endian(t) } -> std::same_as<T>;
    }
    OsErrorOr<void> write_big_endian(T const value) {
        auto converted_value = convert_from_host_to_big_endian(value);
        return write_all({ reinterpret_cast<uint8_t const*>(&converted_value), sizeof(T) });
    }

private:
    // TODO: Make this fallible
    void vwriteff(fmt::string_view fmtstr, fmt::format_args args);

    WritableStream& m_stream;
    UString::Encoding m_encoding {};
};

}
