#include "Reader.hpp"

namespace Util {

constexpr auto BufferSize = 4096;

OsErrorOr<size_t> Reader::read(std::span<uint8_t> data) {
    auto read = read_from_buffer(data);
    if (read < data.size() && !m_stream.is_eof()) {
        if (data.size() - read > BufferSize) {
            return m_stream.read(data.subspan(read));
        }
        TRY(refill_buffer());
        return read_from_buffer(data.subspan(read));
    }

    return read;
}

size_t Reader::read_from_buffer(std::span<uint8_t> data) {
    if (buffer_is_empty())
        return 0;
    auto to_read = std::min(m_buffer.size() - m_buffer_offset, data.size());
    std::copy(m_buffer.begin() + m_buffer_offset, m_buffer.begin() + m_buffer_offset + to_read, data.begin());
    m_buffer_offset += to_read;
    return to_read;
}

OsErrorOr<size_t> Reader::refill_buffer() {
    m_buffer.resize(BufferSize);
    m_buffer_offset = 0;
    auto read = TRY(m_stream.read(m_buffer));
    m_buffer.resize(read);
    return read;
}

OsErrorOr<bool> Reader::read_all(std::span<uint8_t> data) {
    size_t bytes_read = 0;
    while (bytes_read < data.size()) {
        auto bytes = TRY(read(data.subspan(bytes_read)));
        if (bytes == 0)
            return false;
        bytes_read += bytes;
    }
    return true;
}

OsErrorOr<std::optional<uint8_t>> Reader::get() {
    uint8_t byte;
    auto bytes_read = TRY(read_all({ &byte, 1 }));
    if (!bytes_read)
        return std::optional<uint8_t> {};
    return byte;
}

OsErrorOr<std::optional<uint8_t>> Reader::peek() {
    if (buffer_is_empty()) {
        TRY(refill_buffer());
    }
    if (!buffer_is_empty()) {
        return m_buffer[m_buffer_offset];
    }
    return std::optional<uint8_t> {};
}

OsErrorOr<Buffer> Reader::read_until(uint8_t delim) {
    Buffer buffer;
    while (true) {
        auto ch = TRY(get());
        if (!ch || ch == delim) {
            break;
        }
        buffer.append(*ch);
    }
    return buffer;
}

OsErrorOr<UString> Reader::read_line() {
    auto buffer = TRY(read_until('\n'));
    return buffer.decode(m_encoding);
}

}
