#include "Suite.hpp"

#include <Util/UString.hpp>
#include <algorithm>
#include <vector>

TEST_CASE(construction)
{
    // Default constructor
    UString str1;
    EXPECT(str1.size() == 0);
    EXPECT(str1.storage() == nullptr);

    // 1-char constructor
    UString str2 { 'a' };
    EXPECT(str2.size() == 1);
    EXPECT(str2.at(0) == 'a');

    // 1-char constructor with unicode character
    UString str3 { L'ą' };
    EXPECT(str3.size() == 1);
    EXPECT(str3.at(0) == L'ą');

    return {};
}

TEST_CASE(copy)
{
    // Copy constructor
    UString str1 { "abcdef" };
    auto str2 = str1;
    EXPECT(str2 == str1);

    // Copy assignment
    UString str3 { "ghijkl" };
    str2 = str3;
    EXPECT(str2 == str3);

    return {};
}

TEST_CASE(move)
{
    // Move constructor
    UString str1 { "abcdef" };
    auto str2 = std::move(str1);
    EXPECT(str2 == "abcdef");
    EXPECT(str1.is_empty());

    // Move assignment
    UString str3 { "ghijkl" };
    str2 = std::move(str3);
    EXPECT(str2 == "ghijkl");
    EXPECT(str3.is_empty());

    return {};
}

TEST_CASE(utf8)
{
    struct Testcase {
        char const* string;
        std::vector<uint32_t> expected;
    };

    Testcase testcases[] = {
        { "abcdef", { 'a', 'b', 'c', 'd', 'e', 'f' } },
        { "ąęłπę", { L'ą', L'ę', L'ł', L'π', L'ę' } },
        { "test你能？abceąę", { 't', 'e', 's', 't', L'你', L'能', L'？', L'a', L'b', L'c', L'e', L'ą', L'ę' } },
        { "😀 😌 😔 😑 😆 😞", { L'😀', ' ', L'😌', ' ', L'😔', ' ', L'😑', ' ', L'😆', ' ', L'😞' } }
    };

    for (auto const& testcase : testcases) {
        // Decoding
        UString string { testcase.string };
        EXPECT(std::equal(string.storage(), string.storage() + string.size(), testcase.expected.data()));

        // Encoding
        auto string_in_utf8 = string.encode();
        EXPECT(std::equal(string_in_utf8.begin(), string_in_utf8.end(), testcase.string));
    }

    // FIXME: Add some invalid cases

    return {};
}

TEST_CASE(concatenate)
{
    UString str1 { "abc" };
    UString str2 { "ąęł" };
    EXPECT_EQ((str1 + str2).encode(), "abcąęł");

    return {};
}
