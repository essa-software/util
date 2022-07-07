// No #pragma once, this is expected to be included only once per test file

#include <Util/Error.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string_view>

using namespace Util;

namespace __TestSuite {

struct TestError {
    std::string expression;
    std::string_view file;
    int line;
};

ErrorOr<void, TestError> expect(bool condition, std::string_view expression, std::string_view file, int line)
{
    if (!condition)
        return TestError { std::string { expression }, file, line };
    return {};
}

ErrorOr<void, TestError> expect_equal(auto v1, auto v2, std::string_view expr1, std::string_view expr2, std::string_view file, int line)
{
    if (v1 != v2) {
        std::ostringstream oss;
        oss << expr1 << " == " << expr2 << " (lhs = " << v1 << ", rhs = " << v2 << ")";
        return TestError { oss.str(), file, line };
    }
    return {};
}

using Test = ErrorOr<void, TestError>();

std::map<std::string_view, Test*> tests;

}

#define EXPECT(...) TRY(__TestSuite::expect(__VA_ARGS__, #__VA_ARGS__, __FILE__, __LINE__))
constexpr bool Fail = false;
#define FAIL() EXPECT(Fail)

#define EXPECT_EQ(e1, e2) TRY(__TestSuite::expect_equal((e1), (e2), #e1, #e2, __FILE__, __LINE__))

#define TEST_CASE(name)                                                                 \
    ErrorOr<void, __TestSuite::TestError> __test_##name##_func();                       \
    struct __Test_##name {                                                              \
        __Test_##name() { __TestSuite::tests.insert({ #name, __test_##name##_func }); } \
    } __test_##name##_adder;                                                            \
    ErrorOr<void, __TestSuite::TestError> __test_##name##_func()

int main()
{
    std::cout << "Running \e[1m" << __TestSuite::tests.size() << "\e[m test(s)" << std::endl;
    int num_passed = 0;
    int num_failed = 0;
    for (auto const& test : __TestSuite::tests) {
        auto result = test.second();
        if (result.is_error()) {
            std::cout << "\e[31m ✗ \e[m" << test.first << ": expected \e[1m" << result.error().expression << "\e[m at \e[36m" << result.error().file << ":" << result.error().line << "\e[m\n";
            num_failed++;
        } else {
            std::cout << "\e[32m ✓ \e[m" << test.first << "\n";
            num_passed++;
        }
    }
    std::cout << "\e[1;32m" << num_passed << "\e[m passed, \e[1;31m" << num_failed << "\e[m failed" << std::endl;
    return num_failed > 0 ? 1 : 0;
}
