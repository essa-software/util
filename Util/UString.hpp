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

    explicit UString(uint32_t codepoint);

    UString(char ch)
        : UString(static_cast<uint32_t>(ch))
    {
    }

    UString(wchar_t ch)
        : UString(static_cast<uint32_t>(ch))
    {
    }

    enum class Encoding {
        ASCII,
        Utf8
    };

    UString(std::string_view, Encoding = Encoding::Utf8, uint32_t replacement = 0xfffd);

    template<size_t S>
    UString(char const (&string)[S], Encoding encoding = Encoding::Utf8, uint32_t replacement = 0xfffd)
        : UString({ string, S - 1 }, encoding, replacement)
    {
    }

    [[nodiscard]] std::string encode(Encoding = Encoding::Utf8) const;

    [[nodiscard]] uint32_t at(size_t) const;
    [[nodiscard]] size_t size() const { return m_size; }
    [[nodiscard]] bool is_empty() const { return m_size == 0; }

    [[nodiscard]] auto begin() const { return span().begin(); }
    [[nodiscard]] auto end() const { return span().end(); }

    // If you really want... there is a footgun for you.
    [[nodiscard]] uint32_t const* storage() const { return m_storage; }

    [[nodiscard]] std::span<uint32_t const> span() const { return { m_storage, m_size }; }

    // Substring from `start` to end of string
    [[nodiscard]] UString substring(size_t start) const;

    [[nodiscard]] UString substring(size_t start, size_t size) const;
    [[nodiscard]] std::optional<size_t> find(UString needle, size_t start = 0) const;
    [[nodiscard]] UString erase(size_t start, size_t size = 1) const;
    [[nodiscard]] UString insert(UString other, size_t where) const;

    std::strong_ordering operator<=>(UString const& other) const;
    bool operator==(UString const& other) const;

private:
    friend UString operator+(UString const& lhs, UString const& rhs);

    void reallocate(size_t);
    std::string dump() const;

    uint32_t* m_storage {};
    size_t m_size {};
};

}
