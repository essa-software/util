#include "Writer.hpp"

namespace Util {

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

}
