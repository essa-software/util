#pragma once

#include "../Endianness.h"
#include "../UString.hpp"
#include "Stream.hpp"
#include <type_traits>

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

    template<class T>
    requires requires(T t) {
        { convert_from_little_to_host_endian(t) } -> std::same_as<T>;
    }
    OsErrorOr<T> read_little_endian() {
        T value;
        auto bytes_read = TRY(read({ reinterpret_cast<uint8_t*>(&value), sizeof(T) }));
        if (bytes_read != sizeof(T))
            return OsError { 0, "EOF in read_little_endian" };
        return convert_from_little_to_host_endian(value);
    }

    template<class T>
    requires requires(T t) {
        { convert_from_big_to_host_endian(t) } -> std::same_as<T>;
    }
    OsErrorOr<T> read_big_endian() {
        T value;
        auto bytes_read = TRY(read({ reinterpret_cast<uint8_t*>(&value), sizeof(T) }));
        if (bytes_read != sizeof(T))
            return OsError { 0, "EOF in read_big_endian" };
        return convert_from_big_to_host_endian(value);
    }

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
