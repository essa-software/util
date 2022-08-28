#include "File.hpp"

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

}
