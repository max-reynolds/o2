#include "o2/stdafx.h"

#ifdef PLATFORM_LINUX
#include "o2/Utils/FileSystem/FileSystem.h"

#include "o2/Application/Application.h"
#include "o2/Utils/Debug/Debug.h"
#include "o2/Utils/Debug/Log/LogStream.h"

namespace o2
{
FolderInfo
FileSystem::GetFolderInfo(const String &path) const
{
    return {};
}

bool
FileSystem::FileCopy(const String &source, const String &dest) const
{
    return false;
}

bool
FileSystem::FileDelete(const String &file) const
{
    return false;
}

bool
FileSystem::FileMove(const String &source, const String &dest) const
{
    return false;
}

FileInfo
FileSystem::GetFileInfo(const String &path) const
{
    return {};
}

bool
FileSystem::SetFileEditDate(const String &path, const TimeStamp &time) const
{
    return false;
}

bool
FileSystem::FolderCreate(const String &path, bool recursive /*= true*/) const
{
    return false;
}

bool
FileSystem::FolderCopy(const String &from, const String &to) const
{
    return false;
}

bool
FileSystem::FolderRemove(const String &path, bool recursive /*= true*/) const
{
    return false;
}

bool
FileSystem::Rename(const String &old, const String &newPath) const
{
    return false;
}
bool
FileSystem::IsFolderExist(const String &path) const
{
    return false;
}

bool
FileSystem::IsFileExist(const String &path) const
{
    return false;
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