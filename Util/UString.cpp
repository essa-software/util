#include "UString.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <compare>
#include <cstring>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>

namespace Util {

UString::UString(UString const& other)
{
    // std::cout << __PRETTY_FUNCTION__ << std::endl;
    reallocate(other.m_size);
    std::copy(other.m_storage, other.m_storage + other.m_size, m_storage);
}

UString::~UString()
{
    // std::cout << __PRETTY_FUNCTION__ << ": " << dump() << std::endl;
    delete[] m_storage;
}

UString& UString::operator=(UString const& other)
{
    // std::cout << __PRETTY_FUNCTION__ << std::endl;
    if (this == &other)
        return *this;
    reallocate(other.m_size);
    std::copy(other.m_storage, other.m_storage + other.m_size, m_storage);
    return *this;
}

UString::UString(UString&& other)
{
    // std::cout << __PRETTY_FUNCTION__ << ": " << dump() << " << " << other.dump() << std::endl;
    m_size = std::exchange(other.m_size, 0);
    m_storage = std::exchange(other.m_storage, nullptr);
}

UString& UString::operator=(UString&& other)
{
    // std::cout << __PRETTY_FUNCTION__ << ": " << dump() << " << " << other.dump() << std::endl;
    if (this == &other)
        return *this;
    delete[] m_storage;
    m_size = std::exchange(other.m_size, 0);
    m_storage = std::exchange(other.m_storage, nullptr);
    return *this;
}

UString::UString(uint32_t codepoint)
{
    // std::cout << __PRETTY_FUNCTION__ << std::endl;
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
    // std::cout << __PRETTY_FUNCTION__ << ": " << dump() << std::endl;
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

UString UString::substring(size_t start) const
{
    return substring(start, size() - start);
}

UString UString::substring(size_t start, size_t size) const
{
    assert(start + size <= m_size);
    UString result;
    result.reallocate(size);
    if (m_storage)
        std::copy(m_storage + start, m_storage + start + size, result.m_storage);
    return result;
}

std::optional<size_t> UString::find(UString needle, size_t start) const
{
    assert(start <= m_size);
    for (size_t s = start; s < m_size; s++) {
        if (m_storage[s] == needle.at(0)) {
            bool found = true;
            for (size_t t = s; t - s < needle.m_size; t++) {
                if (t == m_size) {
                    found = false;
                    break;
                }
                if (m_storage[t] != needle.at(t - s)) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return s;
            }
        }
    }
    return {};
}

UString UString::erase(size_t start, size_t size) const
{
    if (start + size > m_size) {
        size = m_size - start;
    }
    // std::cout << "erase " << start << " +" << size << " from US[" << m_size << "]" << std::endl;
    UString result;
    result.reallocate(m_size - size);
    if (m_storage) {
        std::copy(m_storage, m_storage + start, result.m_storage);
        std::copy(m_storage + start + size, m_storage + m_size, result.m_storage + start);
    }
    return result;
}

UString UString::insert(UString other, size_t where) const
{
    // std::cout << "insert US[" << other.m_size << "] at " << where << " into US[" << m_size << "]" << std::endl;
    assert(where <= m_size);
    UString result;
    result.reallocate(m_size + other.m_size);
    if (m_storage) {
        std::copy(m_storage, m_storage + where, result.m_storage);
        std::copy(m_storage + where, m_storage + m_size, result.m_storage + where + other.m_size);
    }
    if (other.m_storage) {
        std::copy(other.m_storage, other.m_storage + other.m_size, result.m_storage + where);
    }
    return result;
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
    // std::cout << __PRETTY_FUNCTION__ << " with size = " << size << " result = " << dump() << std::endl;
}

std::string UString::dump() const
{
    std::ostringstream oss;
    oss << "US[" << m_storage << " +" << m_size << "]";
    return oss.str();
}

std::strong_ordering UString::operator<=>(UString const& other) const
{
    return std::lexicographical_compare_three_way(m_storage, m_storage + m_size, other.m_storage, other.m_storage + other.m_size);
}

bool UString::operator==(UString const& other) const
{
    return (*this <=> other) == std::strong_ordering::equal;
}

UString operator+(UString const& lhs, UString const& rhs)
{
    UString result;
    result.reallocate(lhs.m_size + rhs.m_size);
    std::copy(lhs.m_storage, lhs.m_storage + lhs.m_size, result.m_storage);
    std::copy(rhs.m_storage, rhs.m_storage + rhs.m_size, result.m_storage + lhs.m_size);
    return result;
}

}
