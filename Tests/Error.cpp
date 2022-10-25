#include "Suite.hpp"
#include <Util/Error.hpp>
#include <Util/GenericParser.hpp>

Util::ErrorOr<int, Util::OsError> os_error(bool error) {
    if (error)
        return Util::OsError { 0, "test" };
    return 21;
}
Util::ErrorOr<int, Util::ParseError> parse_error(bool error) {
    if (error)
        return Util::ParseError { "test", {} };
    return 37;
}

Util::ErrorOr<int, Util::OsError, Util::ParseError> test(bool os, bool parse) {
    auto _os = TRY(os_error(os));
    auto _parse = TRY(parse_error(parse));
    return _os + _parse;
}

struct MyError {
    std::string value;
};

Util::ErrorOr<int, Util::OsError, Util::ParseError, MyError> subset_test(bool os, bool parse, bool my) {
    if (my)
        return MyError { "test1234" };
    return TRY(test(os, parse));
}

TEST_CASE(basic) {
    EXPECT_EQ(test(false, false).release_value(), 58);
    EXPECT_EQ(test(true, false).release_error_of_type<Util::OsError>().function, "test");
    EXPECT_EQ(test(false, true).release_error_of_type<Util::ParseError>().message, "test");
    EXPECT_EQ(subset_test(false, true, false).release_error_of_type<Util::ParseError>().message, "test");
    EXPECT_EQ(subset_test(false, false, true).release_error_of_type<MyError>().value, "test1234");

    return {};
}