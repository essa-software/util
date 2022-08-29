#pragma once

#include "Stream.hpp"

#include <vector>

namespace Util {

class ReadableMemoryStream : public ReadableStream {
public:
    explicit ReadableMemoryStream(std::span<uint8_t const> data);

    template<size_t S>
    static ReadableMemoryStream from_string(char const (&data)[S]) {
        return ReadableMemoryStream{{ reinterpret_cast<uint8_t const*>(data), S - 1 }};
    }

    virtual OsErrorOr<size_t> read(std::span<uint8_t>) override;
    virtual bool is_eof() const override;

private:
    std::vector<uint8_t> m_data;
    size_t m_offset = 0;
};

class WritableMemoryStream : public WritableStream {
public:
    std::span<uint8_t const> data() const { return { m_data.data(), m_data.size() }; }

    virtual OsErrorOr<size_t> write(std::span<uint8_t const>) override;

private:
    std::vector<uint8_t> m_data;
};

}
