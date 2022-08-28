#pragma once

#include "Error.hpp"
#include "UString.hpp"

#include <cstdint>
#include <errno.h>
#include <span>
#include <string_view>

namespace Util {

class ReadableStream {
public:
    virtual ~ReadableStream() = default;

    // Try to read data to buffer. Returns number of bytes read (0 on EOF)
    virtual OsErrorOr<size_t> read(std::span<uint8_t>) = 0;

    // Return true if data read failed because of end-of-file.
    virtual bool is_eof() const = 0;
};

class WritableStream {
public:
    virtual ~WritableStream() = default;

    // Try to write data to buffer. Returns number of bytes written.
    virtual OsErrorOr<size_t> write(std::span<uint8_t const>) = 0;
};

class File {
public:
    virtual ~File();

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

ReadableFileStream& std_in();
WritableFileStream& std_out();
WritableFileStream& std_err();

}
