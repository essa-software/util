#include "Stream.hpp"

#include <unistd.h>

namespace Util {

File::~File() {
    if (m_owned)
        ::close(m_fd);
}

ReadableFileStream ReadableFileStream::adopt_fd(int fd) {
    return ReadableFileStream { fd, true };
}

ReadableFileStream ReadableFileStream::borrow_fd(int fd) {
    return ReadableFileStream { fd, false };
}

OsErrorOr<size_t> ReadableFileStream::read(std::span<uint8_t> data) {
    // TODO: Buffering
    auto result = ::read(fd(), data.data(), data.size_bytes());
    if (result < 0) {
        return OsError { .error = static_cast<error_t>(-result), .function = "FileStream::read_all" };
    }
    if (result == 0) {
        m_eof = true;
    }
    return static_cast<size_t>(result);
}

WritableFileStream WritableFileStream::adopt_fd(int fd) {
    return WritableFileStream { fd, true };
}

WritableFileStream WritableFileStream::borrow_fd(int fd) {
    return WritableFileStream { fd, false };
}

OsErrorOr<size_t> WritableFileStream::write(std::span<uint8_t const> data) {
    // TODO: Buffering
    auto result = ::write(fd(), data.data(), data.size_bytes());
    if (result < 0) {
        return OsError { .error = static_cast<error_t>(-result), .function = "FileStream::write_all" };
    }
    return static_cast<size_t>(result);
}

bool ReadableFileStream::is_eof() const {
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

ReadableFileStream& std_in() {
    static ReadableFileStream stream = ReadableFileStream::borrow_fd(0);
    return stream;
}

WritableFileStream& std_out() {
    static WritableFileStream stream = WritableFileStream::borrow_fd(1);
    return stream;
}

WritableFileStream& std_err() {
    static WritableFileStream stream = WritableFileStream::borrow_fd(2);
    return stream;
}

}
