#include "Suite.hpp"

#include <Util/Matrix.hpp>

TEST_CASE(inverse)
{
    {
        Util::Matrix4x4d matrix {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
        EXPECT_EQ(matrix, matrix.inverted());
    }

    {
        Util::Matrix4x4d matrix {
            5, 1, 3, 7,
            3, -4, 5, 3,
            3, 2, -2, 4,
            6, 1, 7, 4
        };
        Util::Matrix4x4d matrix_inverted {
            -172.0 / 325, 64.0 / 325, 7.0 / 13, 6.0 / 25,
            51.0 / 325, -87.0 / 325, -2.0 / 13, 2.0 / 25,
            63.0 / 325, -31.0 / 325, -4.0 / 13, 1.0 / 25,
            27.0 / 65, -4.0 / 65, -3.0 / 13, -1.0 / 5
        };
        EXPECT_EQ(matrix.inverted(), matrix_inverted);
    }
    return {};
}
