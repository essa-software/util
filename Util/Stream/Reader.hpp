#pragma once

#include "../UString.hpp"
#include "Stream.hpp"

namespace Util {

class Reader {
public:
    explicit Reader(ReadableStream& stream, UString::Encoding encoding = UString::Encoding::Utf8)
        : m_stream(stream)
        , m_encoding(encoding) {
    }

    ReadableStream& stream() const { return m_stream; }

    OsErrorOr<size_t> read(std::span<uint8_t>);
    OsErrorOr<bool> read_all(std::span<uint8_t>);

private:
    ReadableStream& m_stream;
    UString::Encoding m_encoding {};
};

}
