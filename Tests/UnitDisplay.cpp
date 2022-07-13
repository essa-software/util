#include "Suite.hpp"

#include <Util/UnitDisplay.hpp>

TEST_CASE(unit_display) {
    EXPECT_EQ(unit_display(1234, Quantity::Mass).to_string().encode(), "1.2 Tons");
    EXPECT_EQ(unit_display(10000000000, Quantity::Length).to_string().encode(), "1.0000 × 10^7 km");
    EXPECT_EQ(unit_display(1048576, Quantity::FileSize).to_string().encode(), "1 MiB");

    return {};
}
