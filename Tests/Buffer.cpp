#include "Suite.hpp"

#include <Util/Buffer.hpp>

TEST_CASE(constructors) {
    // Default
    {
        Buffer buffer;
        EXPECT_EQ(buffer.size(), 0ull);
    }

    // std::span
    {
        uint8_t data[] { 0x12, 0x34, 0x56 };
        Buffer buffer { data };
        EXPECT_EQ(buffer.size(), 3ull);
        EXPECT_EQ(buffer[0], 0x12);
        EXPECT_EQ(buffer[1], 0x34);
        EXPECT_EQ(buffer[2], 0x56);
    }

    // Initializer list
    {
        Buffer buffer { 0x12, 0x34, 0x56 };
        EXPECT_EQ(buffer.size(), 3ull);
        EXPECT_EQ(buffer[0], 0x12);
        EXPECT_EQ(buffer[1], 0x34);
        EXPECT_EQ(buffer[2], 0x56);
    }

    // Uninitialized
    {
        Buffer buffer = Buffer::uninitialized(40);
        EXPECT_EQ(buffer.size(), 40ull);
    }

    // Filled
    {
        Buffer buffer = Buffer::filled(40, 'a');
        EXPECT_EQ(buffer.size(), 40ull);
        EXPECT_EQ(buffer[0], 'a');
        EXPECT_EQ(buffer[39], 'a');
    }

    return {};
}

TEST_CASE(copy) {
    Buffer buffer1 = Buffer::filled(16, 'a');
    // Copy constructor
    {
        Buffer buffer2 { buffer1 };
        EXPECT_EQ(buffer2.size(), 16ull);
        EXPECT_EQ(buffer2[0], 'a');
        EXPECT_EQ(buffer2[15], 'a');
    }

    // Copy assignment
    {
        Buffer buffer2;
        buffer2 = buffer1;
        EXPECT_EQ(buffer2.size(), 16ull);
        EXPECT_EQ(buffer2[0], 'a');
        EXPECT_EQ(buffer2[15], 'a');
    }

    return {};
}

TEST_CASE(move) {
    // Move constructor
    {
        Buffer buffer1 = Buffer::filled(16, 'a');
        Buffer buffer2 { std::move(buffer1) };
        EXPECT_EQ(buffer1.size(), 0ull);
        EXPECT_EQ(buffer2.size(), 16ull);
        EXPECT_EQ(buffer2[0], 'a');
        EXPECT_EQ(buffer2[15], 'a');
    }

    // Move assignment
    {
        Buffer buffer1 = Buffer::filled(16, 'a');
        Buffer buffer2;
        buffer2 = std::move(buffer1);
        EXPECT_EQ(buffer1.size(), 0ull);
        EXPECT_EQ(buffer2.size(), 16ull);
        EXPECT_EQ(buffer2[0], 'a');
        EXPECT_EQ(buffer2[15], 'a');
    }

    return {};
}

TEST_CASE(append) {
    Buffer buffer { 0x12, 0x34, 0x56 };
    buffer.append(0x78);
    EXPECT_EQ(buffer.size(), 4ull);
    EXPECT_EQ(buffer[3], 0x78);
    return {};
}
