#pragma once

#include "../Buffer.hpp"
#include "../Endianness.hpp"
#include "../SourceLocation.hpp"
#include "../UString.hpp"
#include "Stream.hpp"
#include <type_traits>

namespace Util {

class BufferedReader {
public:
    explicit BufferedReader(ReadableStream& stream)
        : m_stream(stream) {
    }

    ReadableStream& stream() const { return m_stream; }

    bool is_eof() const;
    OsErrorOr<size_t> read(std::span<uint8_t>);
    OsErrorOr<bool> read_all(std::span<uint8_t>);
    OsErrorOr<std::optional<uint8_t>> get();
    OsErrorOr<std::optional<uint8_t>> peek();

private:
    bool buffer_is_empty() const { return m_buffer_offset >= m_buffer.size(); }
    [[nodiscard]] size_t read_from_buffer(std::span<uint8_t>);
    OsErrorOr<size_t> refill_buffer();

    ReadableStream& m_stream;

    Buffer m_buffer;
    size_t m_buffer_offset = 0;
};

class BinaryReader : public BufferedReader {
public:
    explicit BinaryReader(ReadableStream& stream)
        : BufferedReader(stream) { }

    template<class T>
    requires requires(T t) {
        { convert_from_little_to_host_endian(t) } -> std::same_as<T>;
    }
    OsErrorOr<T> read_little_endian() {
        T value;
        if (!TRY(read_all({ reinterpret_cast<uint8_t*>(&value), sizeof(T) })))
            return OsError { 0, "EOF in read_little_endian" };
        return convert_from_little_to_host_endian(value);
    }

    template<class T>
    requires requires(T t) {
        { convert_from_big_to_host_endian(t) } -> std::same_as<T>;
    }
    OsErrorOr<T> read_big_endian() {
        T value;
        if (!TRY(read_all({ reinterpret_cast<uint8_t*>(&value), sizeof(T) })))
            return OsError { 0, "EOF in read_big_endian" };
        return convert_from_big_to_host_endian(value);
    }

    // This reads `delim` but doesn't include it in the buffer.
    OsErrorOr<Buffer> read_until(uint8_t delim);

    template<class Callback>
    OsErrorOr<Buffer> read_while(Callback&& callback) {
        Buffer result;
        auto c = TRY(peek());
        while (c && callback(*c)) {
            result.append(*c);
            TRY(get());
            c = TRY(peek());
        }
        return result;
    }
};

class TextReader : public BufferedReader {
public:
    explicit TextReader(ReadableStream& stream, UString::Encoding encoding = UString::Encoding::Utf8)
        : BufferedReader(stream)
        , m_encoding(encoding) { }

    SourceLocation location() { return m_location; }

    OsErrorOr<std::optional<char>> consume() {
        auto result = TRY(get());
        if (!result)
            return result;
        if (*result == '\n') {
            m_location.line++;
            m_location.column = 0;
        }
        else {
            m_location.column++;
        }
        m_location.offset++;
        return result;
    }

    // Consumes delim but doesn't include in returned value.
    OsErrorOr<UString> consume_until(char delim);

    template<class Callback>
    OsErrorOr<UString> consume_while(Callback&& callback) {
        Buffer result;
        auto c = TRY(peek());
        while (c && callback(*c)) {
            result.append(*c);
            TRY(consume());
            c = TRY(peek());
        }
        return result.decode(m_encoding);
    }

    OsErrorOr<UString> consume_line();

    // True if there was any whitespace actually read
    OsErrorOr<bool> ignore_whitespace() {
        auto result = TRY(consume_while([](uint8_t c) { return isspace(c); }));
        return result.size() > 0;
    }

private:
    SourceLocation m_location;
    UString::Encoding m_encoding {};
};

}
