#include "Reader.hpp"

namespace Util {

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

}
