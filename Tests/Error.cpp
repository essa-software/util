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

TEST_CASE(basic) {
    EXPECT_EQ(test(false, false).release_value(), 58);
    EXPECT_EQ(test(true, false).release_error_of_type<Util::OsError>().function, "test");
    EXPECT_EQ(test(false, true).release_error_of_type<Util::ParseError>().message, "test");

    return {};
}
