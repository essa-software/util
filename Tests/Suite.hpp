// No #pragma once, this is expected to be included only once per test file

#include <Util/Error.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string_view>
#include <unistd.h>

using namespace Util;

namespace __TestSuite {

struct TestError {
    std::string expression;
    std::string_view file;
    int line;
};

ErrorOr<void, TestError> expect(bool condition, std::string_view expression, std::string_view file, int line) {
    if (!condition)
        return TestError { std::string { expression }, file, line };
    return {};
}

ErrorOr<void, TestError> expect_equal(auto v1, auto v2, std::string_view expr1, std::string_view expr2, std::string_view file, int line) {
    if (v1 != v2) {
        std::ostringstream oss;
        oss << expr1 << " == " << expr2 << " (lhs = " << v1 << ", rhs = " << v2 << ")";
        return TestError { oss.str(), file, line };
    }
    return {};
}

template<class T, class E>
ErrorOr<void, TestError> expect_no_error(ErrorOr<T, E> value, std::string_view expr, std::string_view file, int line) {
    if (value.is_error()) {
        std::ostringstream oss;
        oss << expr << " is error";
        return TestError { oss.str(), file, line };
    }
    return {};
}

using Test = ErrorOr<void, TestError>();

std::map<std::string_view, Test*> tests;

}

#define EXPECT(...) TRY(__TestSuite::expect((__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__))
constexpr bool Fail = false;
#define FAIL() EXPECT(Fail)

#define EXPECT_EQ(e1, e2) TRY(__TestSuite::expect_equal((e1), (e2), #e1, #e2, __FILE__, __LINE__))

#define EXPECT_NO_ERROR(...) TRY(__TestSuite::expect_no_error((__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__))

#define TEST_CASE(name)                                                                 \
    ErrorOr<void, __TestSuite::TestError> __test_##name##_func();                       \
    struct __Test_##name {                                                              \
        __Test_##name() { __TestSuite::tests.insert({ #name, __test_##name##_func }); } \
    } __test_##name##_adder;                                                            \
    ErrorOr<void, __TestSuite::TestError> __test_##name##_func()

int main(int, char** argv) {
    bool failed = false;
    for (auto const& test : __TestSuite::tests) {
        std::string test_name = (std::string_view { argv[0] }.starts_with("./") ? std::string { argv[0] }.substr(2) : argv[0]) + "/" + std::string { test.first };
        std::cout << "\r\e[2K\e[33m . \e[m" << test_name << std::flush;
        auto result = test.second();
        if (result.is_error()) {
            std::cout << "\r\e[2K\e[31m ✗ \e[m" << test_name << ": expected \e[1m" << result.error().expression << "\e[m at \e[36m" << result.error().file << ":" << result.error().line << "\e[m\n";
            failed = true;
        }
    }
    std::cout << "\r\e[2K";
    return failed ? 1 : 0;
}
