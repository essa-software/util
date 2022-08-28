#include "Suite.hpp"

#include <Util/Stream.hpp>
#include <Util/UString.hpp>
#include <vector>

ErrorOr<void, __TestSuite::TestError> expect_buffers_equal(std::span<uint8_t const> got, std::span<uint8_t const> expected) {
    EXPECT_EQ(got.size(), expected.size());
    EXPECT(std::equal(got.begin(), got.end(), expected.begin()));
    return {};
}

TEST_CASE(writable_memory_stream) {

    Util::WritableMemoryStream out;

    EXPECT_NO_ERROR(Writer { out }.write("testąę"));
    TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x74, 0x65, 0x73, 0x74, 0xc4, 0x85, 0xc4, 0x99 } }));
    EXPECT_NO_ERROR(Writer { out }.write("siema tej"));
    TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { 0x74, 0x65, 0x73, 0x74, 0xc4, 0x85, 0xc4, 0x99, 0x73, 0x69, 0x65, 0x6d, 0x61, 0x20, 0x74, 0x65, 0x6a } }));

    return {};
}

TEST_CASE(readable_memory_stream) {
    return {};

    uint8_t buffer[] { 0x65, 0x73, 0x73, 0x61, 0x67, 0x75, 0x69, 0x20, 0x69, 0x73, 0x20, 0x6f, 0x6b };

    Util::ReadableMemoryStream in { buffer };
    Reader reader { in };

    {
        uint8_t read_buffer[6];
        EXPECT_NO_ERROR(reader.read(read_buffer));
        EXPECT_EQ(in.is_eof(), false);
        TRY(expect_buffers_equal(read_buffer, std::span<uint8_t const> { { 0x65, 0x73, 0x73, 0x61, 0x67, 0x75 } }));
    }

    {
        uint8_t read_buffer[20];
        EXPECT_EQ(reader.read_all(read_buffer).release_value(), 7);
        EXPECT_EQ(in.is_eof(), true);
        TRY(expect_buffers_equal(read_buffer, std::span<uint8_t const> { { 0x69, 0x20, 0x69, 0x73, 0x20, 0x6f, 0x6b } }));
    }

    return {};
}

TEST_CASE(reader_buffering) {
    uint8_t big_buffer[16384];
    for (size_t s = 0; s < 16384; s++) {
        big_buffer[s] = rand() & 0xff;
    }

    {
        Util::ReadableMemoryStream in { big_buffer };
        Util::Reader reader { in };

        uint8_t read_buffer[4000];
        EXPECT_NO_ERROR(reader.read(read_buffer));
        TRY(expect_buffers_equal(read_buffer, std::span { big_buffer, 4000 }));
        EXPECT_NO_ERROR(reader.read(read_buffer));
        TRY(expect_buffers_equal(read_buffer, std::span { big_buffer + 4000, 4000 }));
    }

    {
        Util::ReadableMemoryStream in { big_buffer };
        Util::Reader reader { in };

        uint8_t read_buffer[8292]; // 2*BufferSize+100
        EXPECT_NO_ERROR(reader.read(read_buffer));
        TRY(expect_buffers_equal(read_buffer, std::span { big_buffer, 8292 }));
    }

    return {};
}
