#include "Stream.hpp"

#include <unistd.h>

namespace Util {

FileStream FileStream::adopt_fd(int fd) {
    FileStream stream;
    stream.m_fd = fd;
    stream.m_owned = true;
    return stream;
}

FileStream FileStream::borrow_fd(int fd) {
    FileStream stream;
    stream.m_fd = fd;
    stream.m_owned = false;
    return stream;
}

FileStream::~FileStream() {
    if (m_owned)
        close(m_fd);
}

OsErrorOr<size_t> FileStream::read(std::span<uint8_t> data) {
    // TODO: Buffering
    auto result = ::read(m_fd, data.data(), data.size_bytes());
    if (result < 0) {
        return OsError { .error = static_cast<error_t>(-result), .function = "FileStream::read_all" };
    }
    if (result == 0) {
        m_eof = true;
    }
    return static_cast<size_t>(result);
}

OsErrorOr<size_t> FileStream::write(std::span<uint8_t const> data) {
    // TODO: Buffering
    auto result = ::write(m_fd, data.data(), data.size_bytes());
    if (result < 0) {
        return OsError { .error = static_cast<error_t>(-result), .function = "FileStream::write_all" };
    }
    return static_cast<size_t>(result);
}

bool FileStream::is_eof() const {
    return m_eof;
}

OsErrorOr<size_t> Reader::read(std::span<uint8_t> data) {
    return m_stream.read(data);
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

OsErrorOr<size_t> Writer::write(std::span<uint8_t const> data) {
    return m_stream.write(data);
}

OsErrorOr<void> Writer::write_all(std::span<uint8_t const> data) {
    size_t bytes_written = 0;
    while (bytes_written < data.size()) {
        bytes_written += TRY(write(data.subspan(bytes_written)));
    }
    return {};
}

OsErrorOr<void> Writer::write(UString const& string) {
    auto encoded = string.encode(m_encoding);
    return write_all({ reinterpret_cast<uint8_t const*>(encoded.data()), encoded.size() });
}

FileStream& std_in() {
    static FileStream stream = FileStream::borrow_fd(0);
    return stream;
}

FileStream& std_out() {
    static FileStream stream = FileStream::borrow_fd(1);
    return stream;
}

FileStream& std_err() {
    static FileStream stream = FileStream::borrow_fd(2);
    return stream;
}

}
