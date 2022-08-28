#include "Suite.hpp"

#include <Util/Stream.hpp>
#include <Util/UString.hpp>
#include <vector>

ErrorOr<void, __TestSuite::TestError> expect_buffers_equal(std::span<uint8_t const> got, std::initializer_list<uint8_t> expected) {
    EXPECT_EQ(got.size(), expected.size());
    EXPECT(std::equal(got.begin(), got.end(), expected.begin()));
    return {};
}

TEST_CASE(writable_memory_stream) {

    Util::WritableMemoryStream out;

    EXPECT_NO_ERROR(Writer { out }.write("testąę"));
    TRY(expect_buffers_equal(out.data(), { 0x74, 0x65, 0x73, 0x74, 0xc4, 0x85, 0xc4, 0x99 }));
    EXPECT_NO_ERROR(Writer { out }.write("siema tej"));
    TRY(expect_buffers_equal(out.data(), { 0x74, 0x65, 0x73, 0x74, 0xc4, 0x85, 0xc4, 0x99, 0x73, 0x69, 0x65, 0x6d, 0x61, 0x20, 0x74, 0x65, 0x6a }));

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
        TRY(expect_buffers_equal(read_buffer, { 0x65, 0x73, 0x73, 0x61, 0x67, 0x75 }));
    }

    {
        uint8_t read_buffer[20];
        EXPECT_EQ(reader.read_all(read_buffer).release_value(), 7);
        EXPECT_EQ(in.is_eof(), true);
        TRY(expect_buffers_equal(read_buffer, { 0x69, 0x20, 0x69, 0x73, 0x20, 0x6f, 0x6b }));
    }

    return {};
}
