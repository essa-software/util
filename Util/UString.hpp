#pragma once

#include <compare>
#include <cstdint>
#include <optional>
#include <span>
#include <string>

namespace Util {

// A string that properly supports Unicode. It stores data as UTF-32
// internally, like SFML strings. This string is NOT null-terminated
// (because why is should be, you have size and this is sufficient
// in MODERN languages like C++ :^))
// It is immutable like in JS or Python.
class UString {
public:
    UString() = default;
    ~UString();
    UString(UString const& other);
    UString& operator=(UString const& other);
    UString(UString&& other);
    UString& operator=(UString&& other);
    UString(uint32_t codepoint);

    enum class Encoding {
        ASCII,
        Utf8
    };

    UString(char const* data, Encoding = Encoding::Utf8, uint32_t replacement = 0xfffd);

    std::string encode(Encoding = Encoding::Utf8) const;

    uint32_t at(size_t) const;
    size_t size() const { return m_size; }
    bool is_empty() const { return m_size == 0; }

    // If you really want... there is a footgun for you.
    uint32_t const* storage() const { return m_storage; }
    std::span<uint32_t const> span() const { return { m_storage, m_size }; }

    std::strong_ordering operator<=>(UString const& other) const;
    bool operator==(UString const& other) const;

private:
    void reallocate(size_t);

    uint32_t* m_storage {};
    size_t m_size {};
};

}
