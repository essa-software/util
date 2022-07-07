#include "Suite.hpp"

#include <Util/Vector3.hpp>

TEST_CASE(arithmetic)
{
    Vector3 v1 { 5, 4, 2 };
    Vector3 v2 { -1, 4, 6 };
    Vector3 v1_plus_v2 { 4, 8, 8 };
    EXPECT_EQ(v1 + v2, v1_plus_v2);
    Vector3 v1_minus_v2 { 6, 0, -4 };
    EXPECT_EQ(v1 - v2, v1_minus_v2);

    Vector3 v1_multipled { 25, 20, 10 };
    Vector3 v1_divided { 1, 0.8, 0.4 };
    EXPECT_EQ(v1 * 5, v1_multipled);
    EXPECT_EQ(v1 / 5, v1_divided);

    return {};
}

TEST_CASE(magnitude)
{
    Vector3 test { 3, 4, 5 };
    EXPECT_EQ(test.magnitude_squared(), 50);

    Vector3 zero;
    EXPECT_EQ(zero.magnitude_squared(), 0);
    EXPECT_EQ(zero.magnitude(), 0);
    EXPECT_EQ(zero.inverted_magnitude(), INFINITY);

    return {};
}

// TODO: angle, rotate, normalization, 
