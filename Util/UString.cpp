#include "UString.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <compare>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

namespace Util {

UString::UString(UString const& other)
{
    reallocate(other.m_size);
    std::copy(other.m_storage, other.m_storage + other.m_size, m_storage);
}

UString::~UString()
{
    delete[] m_storage;
}

UString& UString::operator=(UString const& other)
{
    if (this == &other)
        return *this;
    reallocate(other.m_size);
    std::copy(other.m_storage, other.m_storage + other.m_size, m_storage);
    return *this;
}

UString::UString(UString&& other)
{
    m_size = std::exchange(other.m_size, 0);
    m_storage = std::exchange(other.m_storage, nullptr);
}

UString& UString::operator=(UString&& other)
{
    if (this == &other)
        return *this;
    delete[] m_storage;
    m_size = std::exchange(other.m_size, 0);
    m_storage = std::exchange(other.m_storage, nullptr);
    return *this;
}

UString::UString(uint32_t codepoint)
{
    reallocate(1);
    m_storage[0] = codepoint;
}

namespace Utf8 {

static int bytes_required_to_store_codepoint(uint32_t codepoint)
{
    if (codepoint < 0x80)
        return 1;
    if (codepoint < 0x800)
        return 2;
    if (codepoint < 0x10000)
        return 3;
    if (codepoint < 0x200000)
        return 4;
    if (codepoint < 0x4000000)
        return 5;
    return 6;
}

template<class Callback>
static bool decode_impl(std::string_view string, uint32_t replacement, Callback callback)
{
    bool error = false;
    for (size_t s = 0; s < string.size(); s++) {
        auto byte = std::bit_cast<uint8_t>(string[s]);
        uint32_t codepoint = 0;
        int additional_bytes_to_expect = 0;
        if (byte >= 0b1111'1110) {
            std::cout << "invalid utf8 character: 0x" << std::hex << static_cast<uint16_t>(byte) << std::dec << std::endl;
            error = true;
            codepoint = replacement;
        } else if ((byte & 0b1111'1110) == 0b1111'1100) {
            additional_bytes_to_expect = 5;
            codepoint = byte & 0b1;
        } else if ((byte & 0b1111'1100) == 0b1111'1000) {
            additional_bytes_to_expect = 4;
            codepoint = byte & 0b11;
        } else if ((byte & 0b1111'1000) == 0b1111'0000) {
            additional_bytes_to_expect = 3;
            codepoint = byte & 0b111;
        } else if ((byte & 0b1111'0000) == 0b1110'0000) {
            additional_bytes_to_expect = 2;
            codepoint = byte & 0b1111;
        } else if ((byte & 0b1110'0000) == 0b1100'0000) {
            additional_bytes_to_expect = 1;
            codepoint = byte & 0b11111;
        } else {
            codepoint = byte & 0x7f;
        }

        for (int i = 0; i < additional_bytes_to_expect; i++) {
            s++;
            if (s > string.size()) {
                std::cout << "unfinished utf8 sequence" << std::endl;
                return false;
            }
            codepoint <<= 6;
            codepoint |= (string[s] & 0b111111);
        }

        // Check if codepoint was stored optimally
        auto required_bytes = bytes_required_to_store_codepoint(codepoint);
        auto got_bytes = additional_bytes_to_expect + 1;
        if (required_bytes > got_bytes) {
            std::cout << "got more bytes than required to encode codepoint " << codepoint << " (" << got_bytes << " > " << required_bytes << ")\n";
        }

        callback(codepoint);
    }
    return !error;
}

static size_t strlen(std::string_view string)
{
    size_t size = 0;
    decode_impl(string, 0x0, [&size](auto) { size++; });
    return size;
}

static bool decode(std::span<uint32_t> storage, std::string_view string, uint32_t replacement)
{
    size_t offset = 0;
    return decode_impl(string, replacement, [&offset, &storage](auto cp) {
        assert(offset < storage.size());
        storage[offset] = cp;
        offset++;
    });
}

static std::string encode(std::span<uint32_t const> storage)
{
    std::vector<uint8_t> result;
    for (auto codepoint : storage) {
        if (codepoint < 0x80) {
            result.push_back(codepoint);
        } else if (codepoint < 0x800) {
            result.push_back(0b1100'0000 | ((codepoint & 0b11111'000000) >> 6));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111)));
        } else if (codepoint < 0x10000) {
            result.push_back(0b1110'0000 | ((codepoint & 0b1111'000000'000000) >> 12));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000) >> 6));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111)));
        } else if (codepoint < 0x200000) {
            result.push_back(0b1111'0000 | ((codepoint & 0b1110'000000'000000'000000) >> 18));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000'000000) >> 12));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000) >> 6));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111)));
        } else if (codepoint < 0x4000000) {
            result.push_back(0b1111'1000 | ((codepoint & 0b1100'000000'000000'000000'000000) >> 24));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000'000000'000000) >> 18));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000'000000) >> 12));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000) >> 6));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111)));
        } else {
            result.push_back(0b1111'1100 | ((codepoint & 0b1000'000000'000000'000000'000000'000000) >> 30));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000'000000'000000'000000) >> 24));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000'000000'000000) >> 18));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000'000000) >> 12));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111'000000) >> 6));
            result.push_back(0b1000'0000 | ((codepoint & 0b111111)));
        }
    }
    return std::string { reinterpret_cast<char const*>(result.data()), result.size() };
}

}

UString::UString(std::string_view string, Encoding encoding, uint32_t replacement)
{
    switch (encoding) {
        case Encoding::ASCII:
            reallocate(string.size());
            std::copy(string.begin(), string.end(), m_storage);
            break;
        case Encoding::Utf8:
            reallocate(Utf8::strlen(string));
            Utf8::decode({ m_storage, m_size }, string, replacement);
            break;
    }
}

std::string UString::encode(Encoding encoding) const
{
    switch (encoding) {
        case Encoding::ASCII: {
            std::string str;
            for (auto cp : span()) {
                if (cp > 0x7f)
                    continue;
                str += static_cast<char>(cp);
            }
            return str;
        }
        case Encoding::Utf8: {
            return Utf8::encode(span());
        }
    }
    return "";
}

uint32_t UString::at(size_t p) const
{
    assert(p < m_size);
    return m_storage[p];
}

void UString::reallocate(size_t size)
{
    auto old_storage = m_storage;
    if (size > 0) {
        auto old_size = m_size;

        m_storage = new uint32_t[size];
        if (old_storage) {
            std::copy(old_storage, old_storage + old_size, m_storage);
        }
    }
    delete[] old_storage;
    m_size = size;
}

std::strong_ordering UString::operator<=>(UString const& other) const
{
    return std::lexicographical_compare_three_way(m_storage, m_storage + m_size, other.m_storage, other.m_storage + other.m_size);
}

bool UString::operator==(UString const& other) const
{
    return (*this <=> other) == std::strong_ordering::equal;
}

}
