#include <Util/Testing.hpp>

#include <Util/CoordinateSystem.hpp>

using namespace Cs;

TEST_CASE(magnitude) {
    Vector3f test { 3, 4, 5 };
    EXPECT_EQ(test.length_squared(), 50);

    Vector3f zero;
    EXPECT_EQ(zero.length_squared(), 0);
    EXPECT_EQ(zero.length(), 0);
    EXPECT_EQ(zero.inverted_length(), INFINITY);

    return {};
}

TEST_CASE(length) {
    auto test = [](Vector3f v, float length) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT_EQ_APPROX(v.length_squared(), length);
        return {};
    };
    TRY(test(Vector3f {}, 0));
    TRY(test(Vector3f { 1, 0, 0 }, 1));
    TRY(test(Vector3f { 0, 1, 0 }, 1));
    TRY(test(Vector3f { 0, 0, 1 }, 1));
    TRY(test(Vector3f { -1, 0, 0 }, 1));
    TRY(test(Vector3f { 1, 0, 1 }, 2));
    return {};
}

TEST_CASE(normalized) {
    auto test = [](Vector3f v, Vector3f n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.normalized().is_approximately_equal(n));
        return {};
    };
    TRY(test(Vector3f {}, Vector3f {}));
    TRY(test(Vector3f { 1, 0, 0 }, Vector3f { 1, 0, 0 }));
    TRY(test(Vector3f { 2, 0, 0 }, Vector3f { 1, 0, 0 }));
    TRY(test(Vector3f { 2, 2, 2 }, Vector3f { 0.57735026919, 0.57735026919, 0.57735026919 })); // 1/sqrt(3)
    return {};
}

TEST_CASE(dot) {
    auto test = [](float v, float n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT_EQ_APPROX(v, n);
        return {};
    };
    TRY(test(Vector2f {}.dot(Vector2f {}), 0));
    TRY(test(Vector2f { 1, 1 }.dot(Vector2f {}), 0));
    TRY(test(Vector2f {}.dot(Vector2f { 1, 1 }), 0));
    TRY(test(Vector2f { 1, 1 }.dot(Vector2f { -1, 1 }), 0));
    TRY(test(Vector2f { -1, 1 }.dot(Vector2f { 1, 1 }), 0));
    TRY(test(Vector2f { 1, 1 }.dot(Vector2f { -2, 1 }), -1));
    TRY(test(Vector2f { 1, 1 }.dot(Vector2f { 0, 1 }), 1));
    return {};
}

TEST_CASE(rounding) {
    Vector3f test { 1.2, 3.5, -2.7 };

    Vector3f floor_result { 1, 3, -3 };
    Vector3f ceil_result { 2, 4, -2 };
    Vector3f round_result { 1, 4, -3 };

    EXPECT_EQ(test.floored(), floor_result);
    EXPECT_EQ(test.ceiled(), ceil_result);
    EXPECT_EQ(test.rounded(), round_result);

    return {};
}

TEST_CASE(with_length) {
    // This is mostly tested in normalized, so we don't need it here that much.
    auto test = [](Vector3f v, float l, Vector3f n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.with_length(l).is_approximately_equal(n));
        return {};
    };

    TRY(test(Vector3f {}, 0, Vector3f {}));
    TRY(test(Vector3f {}, 1, Vector3f {}));
    TRY(test(Vector3f { 1, -3, 0 }, 0, Vector3f {}));
    TRY(test(Vector3f { 1, -3, 0 }, 100, Vector3f { 31.62277660168, -94.86832980505, 0 }));
    return {};
}

TEST_CASE(arithmetic) {
    Vector3f v1 { 5, 4, 2 };
    Vector3f v2 { -1, 4, 6 };
    Vector3f v1_plus_v2 { 4, 8, 8 };
    EXPECT_EQ(v1 + v2, v1_plus_v2);
    Vector3f v1_minus_v2 { 6, 0, -4 };
    EXPECT_EQ(v1 - v2, v1_minus_v2);

    Vector3f v1_multipled { 25, 20, 10 };
    Vector3f v1_divided { 1, 0.8, 0.4 };
    EXPECT_EQ(v1 * 5, v1_multipled);
    EXPECT_EQ(v1 / 5, v1_divided);

    return {};
}

TEST_CASE(create_polar) {
    auto test = [](Vector2f v, Vector2f n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.is_approximately_equal(n));
        return {};
    };
    TRY(test(Vector2f::create_polar(0_deg, 1), Vector2f { 1, 0 }));
    TRY(test(Vector2f::create_polar(90_deg, 1), Vector2f { 0, 1 }));
    TRY(test(Vector2f::create_polar(180_deg, 2), Vector2f { -2, 0 }));
    TRY(test(Vector2f::create_polar(270_deg, 2), Vector2f { 0, -2 }));
    return {};
}

TEST_CASE(angle) {
    auto test = [](Vector2f v, float n) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT_EQ_APPROX(v.angle().deg(), n);
        return {};
    };
    TRY(test(Vector2f {}, 0));
    TRY(test(Vector2f { 1, 0 }, 0));
    TRY(test(Vector2f { 0, 1 }, 90));
    TRY(test(Vector2f { -2, 0.001 }, 179.97134));
    TRY(test(Vector2f { -2, 0 }, 180));
    TRY(test(Vector2f { -2, -0.001 }, -179.97134));
    TRY(test(Vector2f { 0, -2 }, -90));
    return {};
}

TEST_CASE(directed_angle_to) {
    auto test = [](Vector2f v1, Vector2f v2, float a) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT_EQ_APPROX(v1.directed_angle_to(v2).deg(), a);
        return {};
    };
    TRY(test(Vector2f { 0, 1 }, Vector2f { 1, 0 }, -90));
    TRY(test(Vector2f { 1, 0 }, Vector2f { 0, 1 }, 90));
    TRY(test(Vector2f { 1, 0 }, Vector2f { -1, 0 }, -180));
    TRY(test(Vector2f { -1, 0 }, Vector2f { 1, 0 }, 180));
    TRY(test(Vector2f { 0, -1 }, Vector2f { 1, -1 }, 45));
    TRY(test(Vector2f { -2, 0.001 }, Vector2f { -2, -0.001 }, 0.05730893));
    TRY(test(Vector2f { -2, -0.001 }, Vector2f { -2, 0.001 }, -0.05730893));
    return {};
}

TEST_CASE(rotate_2d) {
    auto test = [](Vector2f v, float a, Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.rotate(Angle::degrees(a)).is_approximately_equal(result));
        return {};
    };
    TRY(test(Vector2f {}, 90, Vector2f {}));
    TRY(test(Vector2f { 1, 0 }, 180, Vector2f { -1, 0 }));
    TRY(test(Vector2f { 0, 1 }, 180, Vector2f { 0, -1 }));
    TRY(test(Vector2f { 2, 0 }, 180, Vector2f { -2, 0 }));
    TRY(test(Vector2f { 0, 2 }, 180, Vector2f { 0, -2 }));
    TRY(test(Vector2f { M_SQRT1_2, M_SQRT1_2 }, 45, Vector2f { 0, 1 }));
    TRY(test(Vector2f { M_SQRT1_2, M_SQRT1_2 }, -45, Vector2f { 1, 0 }));
    TRY(test(Vector2f { M_SQRT1_2, M_SQRT1_2 }, 135, Vector2f { -1, 0 }));
    TRY(test(Vector2f { M_SQRT1_2, M_SQRT1_2 }, -135, Vector2f { 0, -1 }));
    // FIXME: Add more interesting cases
    return {};
}

TEST_CASE(perpendicular) {
    auto test = [](Vector2f v, Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.perpendicular().is_approximately_equal(result));
        return {};
    };
    TRY(test(Vector2f { 0, 1 }, Vector2f { -1, 0 }));
    TRY(test(Vector2f { 1, 0 }, Vector2f { 0, 1 }));
    TRY(test(Vector2f { 1, 1 }, Vector2f { -1, 1 }));
    TRY(test(Vector2f { -1, -1 }, Vector2f { 1, -1 }));
    return {};
}

TEST_CASE(mirror_against_tangent) {
    auto test = [](Vector2f v, Vector2f axis, Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.mirror_against_tangent(axis.normalized()).is_approximately_equal(result));
        return {};
    };
    TRY(test(Vector2f { -1, 1 }, Vector2f { 1, 0 }, Vector2f { 1, 1 }));
    TRY(test(Vector2f { 1, 0 }, Vector2f { 1, 1 }, Vector2f { 0, -1 }));
    // FIXME: Add more interesting cases
    return {};
}

TEST_CASE(reflect_against_tangent) {
    auto test = [](Vector2f v, Vector2f axis, Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.reflect_against_tangent(axis.normalized()).is_approximately_equal(result));
        return {};
    };
    TRY(test(Vector2f { 0, 1 }, Vector2f { 1, 0 }, Vector2f { 0, -1 }));
    TRY(test(Vector2f { 1, 1 }, Vector2f { 0, 1 }, Vector2f { -1, 1 }));
    // FIXME: Add more interesting cases
    return {};
}

TEST_CASE(tangent_part) {
    auto test = [](Vector2f v, Vector2f normal, Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.tangent_part(normal.normalized()).is_approximately_equal(result));
        return {};
    };
    TRY(test(Vector2f { 1, -1 }, Vector2f { 0, 1 }, Vector2f { 0, -1 }));
    TRY(test(Vector2f { -1, 1 }, Vector2f { 1, 1 }, Vector2f {}));
    TRY(test(Vector2f { -1, 1 }, Vector2f { -1, -1 }, Vector2f {}));
    // FIXME: Add more interesting cases
    return {};
}

TEST_CASE(normal_part) {
    auto test = [](Vector2f v, Vector2f normal, Vector2f result) -> ErrorOr<void, __TestSuite::TestError> {
        EXPECT(v.normal_part(normal.normalized()).is_approximately_equal(result));
        return {};
    };
    TRY(test(Vector2f { 1, -1 }, Vector2f { 0, 1 }, Vector2f { 1, 0 }));
    TRY(test(Vector2f { -1, 1 }, Vector2f { 1, 1 }, Vector2f { -1, 1 }));
    TRY(test(Vector2f { -1, 1 }, Vector2f { -1, -1 }, Vector2f { -1, 1 }));
    // FIXME: Add more interesting cases
    return {};
}

// TODO: Add tests for:
// - Point
// - Size
// - Interoperability
