#include "Buffer.hpp"
#include <algorithm>
#include <fmt/core.h>
#include <unistd.h>
#include <utility>

namespace Util {

Buffer::~Buffer() {
    delete[] m_data;
}

Buffer::Buffer(Buffer const& other) {
    reallocate(other.m_size);
    m_size = other.m_size;
    std::copy(other.m_data, other.m_data + other.m_size, m_data);
}

Buffer& Buffer::operator=(Buffer const& other) {
    if (this == &other)
        return *this;
    reallocate(other.m_size);
    m_size = other.m_size;
    std::copy(other.m_data, other.m_data + other.m_size, m_data);
    return *this;
}

Buffer::Buffer(Buffer&& other) {
    delete[] m_data;
    m_data = std::exchange(other.m_data, nullptr);
    m_size = std::exchange(other.m_size, 0);
}

Buffer& Buffer::operator=(Buffer&& other) {
    if (this == &other)
        return *this;
    delete[] m_data;
    m_data = std::exchange(other.m_data, nullptr);
    m_size = std::exchange(other.m_size, 0);
    return *this;
}

Buffer::Buffer(std::span<uint8_t const> data) {
    reallocate(data.size());
    std::copy(data.begin(), data.end(), m_data);
}

Buffer::Buffer(std::initializer_list<uint8_t> data) {
    reallocate(data.size());
    std::copy(data.begin(), data.end(), m_data);
}

Buffer Buffer::uninitialized(size_t size) {
    return Buffer(size);
}

Buffer Buffer::filled(size_t size, uint8_t byte) {
    Buffer buffer(size);
    std::fill(buffer.m_data, buffer.m_data + buffer.m_size, byte);
    return buffer;
}

void Buffer::append(uint8_t byte) {
    reallocate(m_size + 1);
    m_data[m_size - 1] = byte;
}

void Buffer::append(std::span<uint8_t const> data) {
    reallocate(m_size + data.size());
    std::copy(data.begin(), data.end(), &m_data[m_size - data.size()]);
}

UString Buffer::decode(UString::Encoding encoding) const {
    return UString { std::string_view { reinterpret_cast<char const*>(m_data), m_size }, encoding };
}

void Buffer::reallocate(size_t size) {
    auto old_storage = m_data;
    if (size > 0) {
        auto old_size = m_size;

        m_data = new uint8_t[size];
        if (old_storage) {
            std::copy(old_storage, old_storage + old_size, m_data);
        }
    }
    else {
        m_data = nullptr;
    }
    delete[] old_storage;
    m_size = size;
}

bool Buffer::operator==(Buffer const& other) const {
    if (m_size != other.m_size)
        return false;
    return std::equal(begin(), end(), other.begin());
}

}
