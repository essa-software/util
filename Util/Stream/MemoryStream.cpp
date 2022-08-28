#include "MemoryStream.hpp"

namespace Util {

ReadableMemoryStream::ReadableMemoryStream(std::span<uint8_t const> data) {
    m_data.resize(data.size());
    std::copy(data.begin(), data.end(), m_data.begin());
}

OsErrorOr<size_t> ReadableMemoryStream::read(std::span<uint8_t> data) {
    size_t bytes_to_read = m_offset + data.size() > m_data.size() ? m_data.size() - m_offset : data.size();
    if (bytes_to_read == 0)
        return (size_t)0;
    std::copy(m_data.begin() + m_offset, m_data.begin() + m_offset + bytes_to_read, data.data());
    m_offset += bytes_to_read;
    return bytes_to_read;
}

bool ReadableMemoryStream::is_eof() const {
    return m_offset >= m_data.size();
}

OsErrorOr<size_t> WritableMemoryStream::write(std::span<uint8_t const> data) {
    m_data.insert(m_data.end(), data.begin(), data.end());
    return data.size();
}

}
