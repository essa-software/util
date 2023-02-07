#include <Util/Testing.hpp>

#include <Util/Color.hpp>

TEST_CASE(to_html_string) {
    EXPECT_EQ(Colors::White.to_html_string().encode(), "#ffffff");
    EXPECT_EQ(Colors::AliceBlue.to_html_string().encode(), "#f0f8ff");
    EXPECT_EQ(Color { 0x030303ff }.to_html_string().encode(), "#030303");
    return {};
}
