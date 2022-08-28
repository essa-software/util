#pragma once

#include "../UString.hpp"
#include "Stream.hpp"

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

private:
    WritableStream& m_stream;
    UString::Encoding m_encoding {};
};

}
