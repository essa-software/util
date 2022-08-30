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

TEST_CASE(writer_fmt_integration) {
    Util::WritableMemoryStream out;
    Writer writer { out };
    writer.writeff("{}", 12.25);
    TRY(expect_buffers_equal(out.data(), std::span<uint8_t const> { { '1', '2', '.', '2', '5' } }));
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

TEST_CASE(reader_get_peek) {
    uint8_t buffer[] { 0x01, 0x02 };

    Util::ReadableMemoryStream in { buffer };
    Util::Reader reader { in };

    uint8_t read_buffer[1];
    EXPECT_EQ(reader.peek().release_value().value(), 0x01);
    EXPECT_NO_ERROR(reader.read(read_buffer));
    EXPECT_EQ(read_buffer[0], 0x01);
    EXPECT_EQ(reader.peek().release_value().value(), 0x02);
    EXPECT_EQ(reader.get().release_value().value(), 0x02);
    EXPECT(!reader.peek().release_value().has_value());
    EXPECT(!reader.get().release_value().has_value());

    return {};
}

TEST_CASE(reader_big_endian) {
    uint8_t buffer[] { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };

    {
        Util::ReadableMemoryStream in { buffer };
        Util::Reader reader { in };
        EXPECT_EQ(reader.read_big_endian<uint8_t>().release_value(), 0x01ull);
    }
    {
        Util::ReadableMemoryStream in { buffer };
        Util::Reader reader { in };
        EXPECT_EQ(reader.read_big_endian<uint16_t>().release_value(), 0x0123ull);
    }

    {
        Util::ReadableMemoryStream in { buffer };
        Util::Reader reader { in };
        EXPECT_EQ(reader.read_big_endian<uint32_t>().release_value(), 0x01234567ull);
    }
    {
        Util::ReadableMemoryStream in { buffer };
        Util::Reader reader { in };
        EXPECT_EQ(reader.read_big_endian<uint64_t>().release_value(), 0x0123456789abcdefull);
    }
    return {};
}

TEST_CASE(reader_little_endian) {
    uint8_t buffer[] { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };

    {
        Util::ReadableMemoryStream in { buffer };
        Util::Reader reader { in };
        EXPECT_EQ(reader.read_little_endian<uint8_t>().release_value(), 0x01ull);
    }
    {
        Util::ReadableMemoryStream in { buffer };
        Util::Reader reader { in };
        EXPECT_EQ(reader.read_little_endian<uint16_t>().release_value(), 0x2301ll);
    }

    {
        Util::ReadableMemoryStream in { buffer };
        Util::Reader reader { in };
        EXPECT_EQ(reader.read_little_endian<uint32_t>().release_value(), 0x67452301ll);
    }
    {
        Util::ReadableMemoryStream in { buffer };
        Util::Reader reader { in };
        EXPECT_EQ(reader.read_little_endian<uint64_t>().release_value(), 0xefcdab8967452301ll);
    }
    return {};
}

TEST_CASE(reader_read_until) {
    uint8_t buffer[] { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
    Util::ReadableMemoryStream in { buffer };
    Util::Reader reader { in };

    auto data_read = reader.read_until(0x67).release_value();
    Buffer expected { 0x01, 0x23, 0x45 };
    EXPECT_EQ(data_read, expected);
    EXPECT_EQ(reader.peek().release_value().value(), 0x89);

    return {};
}

TEST_CASE(reader_read_while) {
    uint8_t buffer[] { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
    Util::ReadableMemoryStream in { buffer };
    Util::Reader reader { in };

    auto data_read = reader.read_while([](uint8_t byte) { return byte < 0x67; }).release_value();
    Buffer expected { 0x01, 0x23, 0x45 };
    EXPECT_EQ(data_read, expected);
    EXPECT_EQ(reader.peek().release_value().value(), 0x67);

    return {};
}

TEST_CASE(reader_read_line) {
    Util::ReadableMemoryStream in = Util::ReadableMemoryStream::from_string("test\nhello");
    Util::Reader reader { in };

    EXPECT_EQ(reader.read_line().release_value().encode(), "test");
    EXPECT_EQ(reader.read_line().release_value().encode(), "hello");

    return {};
}
