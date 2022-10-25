#pragma once

#include "Stream/Reader.hpp"
#include "Stream/Stream.hpp"

#include <string>
#include <type_traits>
#include <vector>

namespace Util {

struct SourceLocation {
    size_t line = 0;
    size_t column = 0;
    size_t offset = 0;
};

struct SourceRange {
    SourceLocation start;
    SourceLocation end;
};

template<class T>
requires(std::is_enum_v<T>) class Token {
public:
    using Type = T;

    Token(Type type, std::string value, SourceRange range)
        : m_type(type)
        , m_value(std::move(value))
        , m_range(range) { }

    Type type() const { return m_type; }
    std::string value() const { return m_value; }
    SourceRange range() const { return m_range; }
    SourceLocation start() const { return m_range.start; }
    SourceLocation end() const { return m_range.end; }

private:
    Type m_type;
    std::string m_value;
    SourceRange m_range;
};

class Lexer {
public:
    explicit Lexer(ReadableStream& stream)
        : m_reader(stream) { }

    SourceLocation location() { return m_location; }

    bool is_eof() const { return m_reader.is_eof(); }

    auto peek() { return m_reader.peek(); }
    OsErrorOr<std::optional<char>> consume() {
        auto result = TRY(m_reader.get());
        if (!result)
            return result;
        if (*result == '\n') {
            m_location.line++;
            m_location.column = 0;
        }
        else {
            m_location.column++;
        }
        m_location.offset++;
        return result;
    }

    // Consumes delim but doesn't include in returned value, like Stream API
    OsErrorOr<std::string> consume_until(char delim) {
        std::string result;
        while (true) {
            auto ch = TRY(consume());
            if (!ch || *ch == delim) {
                break;
            }
            result += *ch;
        }
        return result;
    }

    template<class Callback>
    OsErrorOr<std::string> consume_while(Callback&& callback) {
        std::string result;
        auto c = TRY(peek());
        while (c && callback(*c)) {
            result += (char)*c;
            TRY(consume());
            c = TRY(peek());
        }
        return result;
    }

    // True if there was any whitespace actually read
    OsErrorOr<bool> ignore_whitespace() {
        auto result = TRY(consume_while([](uint8_t c) { return isspace(c); }));
        return result.size() > 0;
    }

private:
    Reader m_reader;
    SourceLocation m_location;
};

template<class T>
class GenericLexer : protected Lexer {
public:
    explicit GenericLexer(ReadableStream& stream)
        : Lexer(stream) { }

protected:
    auto create_token(T type, std::string value, SourceLocation start) requires(!std::is_same_v<T, void>) {
        return Token<T> { type, std::move(value), { start, location() } };
    }
};

struct ParseError {
    std::string message;
    SourceRange location;
};

template<class T>
using ParseErrorOr = ErrorOr<T, ParseError>;

template<class T>
class GenericParser {
public:
    GenericParser(std::vector<Token<T>> tokens)
        : m_tokens(std::move(tokens)) { }

protected:
    bool is_eof() const {
        return m_offset >= m_tokens.size();
    }

    size_t offset() const { return m_offset; }

    SourceRange range(size_t start, size_t size) {
        assert(start + size < m_tokens.size());
        return SourceRange { .start = m_tokens[start].start(), .end = m_tokens[start + size - 1].end() };
    }

    Token<T> const* get() {
        if (is_eof())
            return nullptr;
        return &m_tokens[m_offset++];
    }
    Token<T> const* peek() const {
        if (is_eof())
            return nullptr;
        return &m_tokens[m_offset];
    }

    bool next_token_is(T type) const {
        return peek() && peek()->type() == type;
    }

    ParseErrorOr<Token<T>> expect(T type) {
        auto token = get();
        if (!token) {
            return error("Unexpected EOF");
        }
        if (token->type() != type) {
            return expected(fmt::format("token of type {}", (int)type), *token);
        }
        return *token;
    }

    ParseError error(std::string message) {
        if (m_tokens.empty())
            return ParseError { .message = message, .location = {} };
        return ParseError {
            .message = message,
            .location = is_eof()
                ? m_tokens[m_offset - 1].range()
                : m_tokens[m_offset].range()
        };
    }

    ParseError error(std::string message, size_t token) {
        assert(token < m_tokens.size());
        return ParseError {
            .message = message,
            .location = m_tokens[token].range()
        };
    }

    ParseError error_in_already_read(std::string message) {
        if (m_tokens.empty())
            return ParseError { .message = message, .location = {} };
        assert(m_offset > 0);
        return ParseError {
            .message = message,
            .location = m_tokens[m_offset - 1].range()
        };
    }

    ParseError expected(std::string what, Token<T> got) {
        return error("Expected " + what + ", got '" + got.value() + "'");
    }

private:
    std::vector<Token<T>> m_tokens;
    size_t m_offset { 0 };
};

}
