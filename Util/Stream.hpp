#pragma once

#include "Error.hpp"
#include "UString.hpp"

#include <cstdint>
#include <errno.h>
#include <span>
#include <string_view>

namespace Util {

class Stream {
public:
    virtual ~Stream() = default;

    // Try to read data to buffer. Returns number of bytes read (0 on EOF)
    virtual OsErrorOr<size_t> read(std::span<uint8_t>) = 0;

    // Try to write data to buffer. Returns number of bytes written.
    virtual OsErrorOr<size_t> write(std::span<uint8_t const>) = 0;

    // Return true if data read failed because of end-of-file.
    virtual bool is_eof() const = 0;
};

class FileStream : public Stream {
public:
    static FileStream adopt_fd(int fd);
    static FileStream borrow_fd(int fd);

    virtual ~FileStream() override;

    virtual OsErrorOr<size_t> read(std::span<uint8_t>) override;
    virtual OsErrorOr<size_t> write(std::span<uint8_t const>) override;
    virtual bool is_eof() const override;

    int fd() const { return m_fd; }

private:
    int m_fd {};
    bool m_eof = false;
    bool m_owned = false;
};

class Reader {
public:
    explicit Reader(Stream& stream, UString::Encoding encoding = UString::Encoding::Utf8)
        : m_stream(stream)
        , m_encoding(encoding)
    {
    }

    Stream& stream() const { return m_stream; }

    OsErrorOr<size_t> read(std::span<uint8_t>);
    OsErrorOr<bool> read_all(std::span<uint8_t>);

private:
    Stream& m_stream;
    UString::Encoding m_encoding {};
};

class Writer {
public:
    explicit Writer(Stream& stream, UString::Encoding encoding = UString::Encoding::Utf8)
        : m_stream(stream)
        , m_encoding(encoding)
    {
    }

    Stream& stream() const { return m_stream; }

    OsErrorOr<size_t> write(std::span<uint8_t const>);
    OsErrorOr<void> write_all(std::span<uint8_t const>);
    OsErrorOr<void> write(UString const&);

private:
    Stream& m_stream;
    UString::Encoding m_encoding {};
};

Reader std_in(UString::Encoding = UString::Encoding::Utf8);
Writer std_out(UString::Encoding = UString::Encoding::Utf8);
Writer std_err(UString::Encoding = UString::Encoding::Utf8);

}
