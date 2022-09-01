#pragma once

#include "../Error.hpp"
#include "../NonCopyable.hpp"
#include "Stream.hpp"
#include <cstddef>
#include <cstdint>
#include <span>

namespace Util {

class File : public NonCopyable {
public:
    virtual ~File();
    File(File&& other);
    File& operator=(File&& other);

    int fd() const { return m_fd; }

protected:
    File(int fd, bool owned)
        : m_fd(fd)
        , m_owned(owned) { }

private:
    int m_fd {};
    bool m_owned { false };
};

class ReadableFileStream : public ReadableStream
    , public File {
public:
    static ReadableFileStream adopt_fd(int fd);
    static ReadableFileStream borrow_fd(int fd);

    virtual OsErrorOr<size_t> read(std::span<uint8_t>) override;
    virtual bool is_eof() const override;

private:
    ReadableFileStream(int fd, bool owned)
        : File(fd, owned) { }

    bool m_eof { false };
};

class WritableFileStream : public WritableStream
    , public File {
public:
    static WritableFileStream adopt_fd(int fd);
    static WritableFileStream borrow_fd(int fd);

    virtual OsErrorOr<size_t> write(std::span<uint8_t const>) override;

private:
    WritableFileStream(int fd, bool owned)
        : File(fd, owned) { }
};

}
