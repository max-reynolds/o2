#include "o2Editor/Core/Dialogs/System/OpenSaveDialog.h"
#include "o2Editor/stdafx.h"

#ifdef PLATFORM_LINUX

namespace Editor
{
String
GetOpenFileNameDialog(
    const String &title,
    const Map<String, String> &extensions,
    const String &defaultPath /*= ""*/)
{
    return {};
}

String
GetSaveFileNameDialog(
    const String &title,
    const Map<String, String> &extensions,
    const String &defaultPath /*= ""*/)
{
    return {};
}
}
#endif