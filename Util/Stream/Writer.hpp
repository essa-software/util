#pragma once

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

private:
    // TODO: Make this fallible
    void vwriteff(fmt::string_view fmtstr, fmt::format_args args);

    WritableStream& m_stream;
    UString::Encoding m_encoding {};
};

}
