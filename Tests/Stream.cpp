#include "Suite.hpp"

#include <Util/Stream.hpp>
#include <Util/UString.hpp>
#include <vector>

// TODO: Make these testcases independent of standard streams

TEST_CASE(output) {
    auto out = Util::std_out();
    EXPECT_NO_ERROR(Writer { out }.write("testąę"));

    return {};
}

TEST_CASE(input) {
    return {};

    // TODO: Make these testcases independent of standard streams
    std::vector<uint8_t> buffer;
    buffer.resize(15);
    EXPECT_NO_ERROR(Reader { Util::std_in() }.read_all(buffer));
    EXPECT_NO_ERROR(Util::std_out().write(buffer));

    return {};
}
