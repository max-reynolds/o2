#include <gtest/gtest.h>

#include <o2/Utils/FileSystem/File.h>

#include <iostream>
#include <string_view>

TEST(TestFileImpl, test)
{
    static const std::string_view REF = "01234567890ABCDEF";
    static const o2::String name = "test_file.bin";

    {
        o2::OutFile f;
        ASSERT_FALSE(f.IsOpened());

        ASSERT_TRUE(f.Open(name));
        ASSERT_TRUE(f.IsOpened());

        const auto &filename = f.GetFilename();
        ASSERT_EQ(filename, name);

        f.WriteData(REF.data(), 16);

        f.Close();
        ASSERT_FALSE(f.IsOpened());
    }

    {
        o2::InFile f;
        ASSERT_FALSE(f.IsOpened());

        ASSERT_EQ((o2::UInt)-1, f.GetDataSize());

        ASSERT_TRUE(f.Open(name));
        ASSERT_TRUE(f.IsOpened());

        // before and after GetDataSize() file position shall be the same
        ASSERT_EQ(0, f.GetCaretPos());
        ASSERT_EQ(16, f.GetDataSize());
        ASSERT_EQ(0, f.GetCaretPos());

        const auto &filename = f.GetFilename();
        ASSERT_EQ(filename, name);

        ASSERT_EQ(0, f.GetCaretPos());

        uint8_t buffer[100] = {};
        f.ReadData(buffer, 4);

        ASSERT_EQ(0, memcmp(buffer, REF.data(), 4));
        ASSERT_EQ(4, f.GetCaretPos());

        f.SetCaretPos(0);
        ASSERT_EQ(16, f.ReadFullData(buffer));
        ASSERT_EQ(16, f.GetCaretPos());

        ASSERT_EQ(0, memcmp(buffer, REF.data(), 16));

        auto data = f.ReadFullData();
        ASSERT_EQ(data, o2::String(std::string(REF.data(), 16)));

        f.Close();
        ASSERT_FALSE(f.IsOpened());
    }
}