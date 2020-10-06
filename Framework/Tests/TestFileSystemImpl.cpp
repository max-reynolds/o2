#include <gtest/gtest.h>

#include <o2/Utils/FileSystem/File.h>
#include <o2/Utils/FileSystem/FileSystem.h>

bool
make_test_file(const o2::String &name)
{
    static const std::string REF = "01234567890ABCDEF";

    o2::FileSystem fs;
    fs.FileDelete(name);

    {
        o2::OutFile f;
        if (!f.Open(name))
        {
            return false;
        }

        f.WriteData(REF.data(), 16);
        f.Close();
    }

    return true;
}

TEST(TestFileSystemImpl, test_delete)
{
    static const o2::String name = "test_file.bin";

    ASSERT_TRUE(make_test_file(name));

    o2::FileSystem fs;
    ASSERT_TRUE(fs.IsFileExist(name));

    ASSERT_TRUE(fs.FileDelete(name));
    ASSERT_FALSE(fs.IsFileExist(name));
}

TEST(TestFileSystemImpl, test_file_copy)
{
    static const o2::String source_name = "source.bin";
    static const o2::String dest_name = "dest.bin";

    {
        o2::FileSystem fs;
        fs.FileDelete(dest_name);
    }

    ASSERT_TRUE(make_test_file(source_name));

    o2::FileSystem fs;
    ASSERT_TRUE(fs.IsFileExist(source_name));
    ASSERT_FALSE(fs.IsFileExist(dest_name));

    ASSERT_TRUE(fs.FileCopy(source_name, dest_name));
    ASSERT_TRUE(fs.IsFileExist(source_name));
    ASSERT_TRUE(fs.IsFileExist(dest_name));

    ASSERT_TRUE(fs.FileDelete(source_name));
    ASSERT_FALSE(fs.IsFileExist(source_name));

    ASSERT_TRUE(fs.FileDelete(dest_name));
    ASSERT_FALSE(fs.IsFileExist(dest_name));
}

TEST(TestFileSystemImpl, test_file_move)
{
    static const o2::String source_name = "source.bin";
    static const o2::String dest_name = "dest.bin";

    {
        o2::FileSystem fs;
        fs.FileDelete(dest_name);
    }

    ASSERT_TRUE(make_test_file(source_name));

    o2::FileSystem fs;
    ASSERT_TRUE(fs.IsFileExist(source_name));
    ASSERT_FALSE(fs.IsFileExist(dest_name));

    ASSERT_TRUE(fs.FileMove(source_name, dest_name));
    ASSERT_FALSE(fs.IsFileExist(source_name));
    ASSERT_TRUE(fs.IsFileExist(dest_name));

    ASSERT_TRUE(fs.FileDelete(dest_name));
    ASSERT_FALSE(fs.IsFileExist(dest_name));
}

TEST(TestFileSystemImpl, test_file_info)
{
    static const o2::String source_name = "source.bin";
    ASSERT_TRUE(make_test_file(source_name));

    o2::FileSystem fs;
    auto info = fs.GetFileInfo(source_name);
}
