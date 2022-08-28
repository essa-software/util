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
    OsErrorOr<std::optional<uint8_t>> get();
    OsErrorOr<std::optional<uint8_t>> peek();

private:
    bool buffer_is_empty() const { return m_buffer_offset >= m_buffer.size(); }
    [[nodiscard]] size_t read_from_buffer(std::span<uint8_t>);
    OsErrorOr<size_t> refill_buffer();

    ReadableStream& m_stream;
    UString::Encoding m_encoding {};

    std::vector<uint8_t> m_buffer;
    size_t m_buffer_offset = 0;
};

}
