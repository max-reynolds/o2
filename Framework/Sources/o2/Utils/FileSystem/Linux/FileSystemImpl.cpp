#include "o2/stdafx.h"

#ifdef PLATFORM_LINUX
#include "o2/Utils/FileSystem/FileSystem.h"

#include "o2/Application/Application.h"
#include "o2/Utils/Debug/Debug.h"
#include "o2/Utils/Debug/Log/LogStream.h"

#include <ctime>
#include <filesystem>
#include <sys/stat.h>
#include <sys/time.h>

namespace o2
{
namespace
{
std::filesystem::path
convert(const String &path)
{
    std::filesystem::path p(path.Data(), path.Data() + path.Length());
    return p;
}

template<typename TP>
std::time_t
to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp =
        time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    return system_clock::to_time_t(sctp);
}

o2::TimeStamp
convert(const time_t *value)
{
    auto local_time = std::localtime(value);

    o2::TimeStamp ts;
    ts.mYear = 1900 + local_time->tm_year;
    ts.mMonth = local_time->tm_mon;
    ts.mDay = local_time->tm_mday;
    ts.mHour = local_time->tm_hour;
    ts.mMinute = local_time->tm_min;
    ts.mSecond = local_time->tm_sec;

    return ts;
}

o2::TimeStamp
convert(std::filesystem::file_time_type value)
{
    auto t = to_time_t(value);
    return convert(&t);
}
}

FolderInfo
FileSystem::GetFolderInfo(const String &path) const
{
    return {};
}

bool
FileSystem::FileCopy(const String &source, const String &dest) const
{
    FileDelete(dest);
    FolderCreate(ExtractPathStr(dest));

    auto s = convert(source);
    auto d = convert(dest);

    return std::filesystem::copy_file(s, d);
}

bool
FileSystem::FileDelete(const String &file) const
{
    auto p = convert(file);
    return std::filesystem::remove(p);
}

bool
FileSystem::FileMove(const String &source, const String &dest) const
{
    String destFolder = GetParentPath(dest);

    if (!IsFolderExist(destFolder))
    {
        FolderCreate(destFolder);
    }

    auto s = convert(source);
    auto d = convert(dest);

    std::error_code ec;
    std::filesystem::rename(s, d, ec);
    return !ec;
}

FileInfo
FileSystem::GetFileInfo(const String &path) const
{
    FileInfo info;
    info.path = path;

    auto p = convert(path);

    {
        std::error_code ec;
        info.size = std::filesystem::file_size(p, ec);
        if (ec)
        {
            info.size = 0;
        }
    }

    {
        struct stat64 buffer = {};

        auto err = stat64(path.Data(), &buffer);
        if (!err)
        {
            info.createdDate = convert(&buffer.st_ctim.tv_sec);
            info.accessDate = convert(&buffer.st_atim.tv_sec);
        }
    }

    info.editDate = convert(std::filesystem::last_write_time(p));

    return info;
}

bool
FileSystem::SetFileEditDate(const String &path, const TimeStamp &time) const
{
    return true;
}

bool
FileSystem::FolderCreate(const String &path, bool recursive /*= true*/) const
{
    if (path.IsEmpty())
    {
        return true;
    }

    if (IsFolderExist(path))
    {
        return true;
    }

    auto p = convert(path);
    if (!recursive)
    {
        return std::filesystem::create_directory(p);
    }

    return std::filesystem::create_directories(p);
}

bool
FileSystem::FolderCopy(const String &from, const String &to) const
{
    auto s = convert(from);
    auto d = convert(to);

    std::error_code ec;
    std::filesystem::copy(s, d, std::filesystem::copy_options::recursive, ec);

    return !ec;
}

bool
FileSystem::FolderRemove(const String &path, bool recursive /*= true*/) const
{
    auto p = convert(path);

    std::error_code ec;
    if (!recursive)
    {
        return std::filesystem::remove(p, ec);
    }

    return std::filesystem::remove_all(p, ec);
}

bool
FileSystem::Rename(const String &old, const String &newPath) const
{
    auto s = convert(old);
    auto d = convert(newPath);

    std::error_code ec;
    std::filesystem::rename(s, d, ec);
    return !ec;
}
bool
FileSystem::IsFolderExist(const String &path) const
{
    if (path.IsEmpty())
    {
        return true;
    }

    auto p = convert(path);
    if (!std::filesystem::exists(p))
    {
        return false;
    }

    return std::filesystem::is_directory(p);
}

bool
FileSystem::IsFileExist(const String &path) const
{
    auto p = convert(path);
    if (!std::filesystem::exists(p))
    {
        return false;
    }

    return false == std::filesystem::is_directory(p);
}

String
FileSystem::CanonicalizePath(const String &path)
{
    return {};
}

String
FileSystem::GetPathRelativeToPath(const String &from, const String &to)
{
    return {};
}
}

#endif