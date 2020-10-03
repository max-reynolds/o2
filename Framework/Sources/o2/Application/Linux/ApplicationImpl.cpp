#include "o2/stdafx.h"

#ifdef PLATFORM_LINUX

#include "o2/Application/Application.h"
#include "o2/Events/EventSystem.h"
#include "o2/Utils/Debug/Log/LogStream.h"
#include "o2/Utils/FileSystem/FileSystem.h"

namespace o2
{
void
Application::Initialize()
{
}

void
Application::Launch()
{
}

void
Application::Shutdown()
{
}

void
Application::SetFullscreen(bool fullscreen /*= true*/)
{
    if (fullscreen)
    {
        mLog->Out("Setting full screen");
    }
    else
    {
        mLog->Out("Setting windowed..");
    }
}

bool
Application::IsFullScreen() const
{
    return false;
}

void
Application::Maximize()
{
}

bool
Application::IsMaximized() const
{
    return false;
}

void
Application::SetResizible(bool resizible)
{
}

bool
Application::IsResizible() const
{
    return false;
}

void
Application::SetWindowSize(const Vec2I &size)
{
}

Vec2I
Application::GetWindowSize() const
{
    return {};
}

void
Application::SetWindowPosition(const Vec2I &position)
{
}

Vec2I
Application::GetWindowPosition() const
{
    return {};
}

void
Application::SetWindowCaption(const String &caption)
{
}

String
Application::GetWindowCaption() const
{
    return {};
}

void
Application::SetContentSize(const Vec2I &size)
{
}

Vec2I
Application::GetContentSize() const
{
    return {};
}

Vec2I
Application::GetScreenResolution() const
{
    return {};
}

void
Application::SetCursor(CursorType type)
{
}

void
Application::SetCursorPosition(const Vec2F &position)
{
}

String
Application::GetBinPath() const
{
    return {};
}
}
#endif
