// No #pragma once, this is expected to be included only once per test file

#include <Util/Error.hpp>
#include <iostream>
#include <map>
#include <string_view>

using namespace Util;

namespace __TestSuite {

struct TestError {
    std::string_view expression;
    std::string_view file;
    int line;
};

ErrorOr<void, TestError> expect(bool condition, std::string_view expression, std::string_view file, int line)
{
    if (!condition)
        return TestError { expression, file, line };
    return {};
}

using Test = ErrorOr<void, TestError>();

std::map<std::string_view, Test*> tests;

}

#define EXPECT(...) TRY(__TestSuite::expect(__VA_ARGS__, #__VA_ARGS__, __FILE__, __LINE__))
constexpr bool Fail = false;
#define FAIL() EXPECT(Fail)

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
            std::cout << "\e[31m ✗ \e[m" << test.first << ": \e[1m" << result.error().expression << "\e[m at \e[36m" << result.error().file << ":" << result.error().line << "\e[m\n";
            num_failed++;
        } else {
            std::cout << "\e[32m ✓ \e[m" << test.first << "\n";
            num_passed++;
        }
    }
    std::cout << "\e[1;32m" << num_passed << "\e[m passed, \e[1;31m" << num_failed << "\e[m failed" << std::endl;
}
