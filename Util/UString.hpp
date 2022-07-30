#pragma once

#include <compare>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

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
    explicit UString(std::span<uint32_t const>);

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

    template<class Callback>
    void for_each_line(Callback&& callback) const
    {
        size_t index = 0;
        while (true) {
            auto next_newline = find("\n", index);
            if (!next_newline.has_value()) {
                next_newline = size();
            }
            if (index > size() - 1)
                break;
            callback({ m_storage + index, *next_newline - index });
            if (next_newline == size())
                break;
            index = *next_newline + 1;
        }
    }

    std::strong_ordering operator<=>(UString const& other) const;
    bool operator==(UString const& other) const;

private:
    friend UString operator+(UString const& lhs, UString const& rhs);

    void reallocate(size_t);
    std::string dump() const;

    uint32_t* m_storage {};
    size_t m_size {};
};

template<typename T>
UString to_ustring(const T& to_convert){
    std::string str = std::to_string(to_convert);
    
    std::vector<uint32_t> vec;

    for(const auto& c : str){
        vec.push_back(c);
    }

    return UString(std::span{vec.data(), vec.size()});
}

}
