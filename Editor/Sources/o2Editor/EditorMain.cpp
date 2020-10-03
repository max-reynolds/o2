#include "o2/stdafx.h"
#include "o2/O2.h"
#include "o2Editor/Core/EditorApplication.h"

int main()
{
    INITIALIZE_O2;

    Editor::EditorApplication* app = mnew Editor::EditorApplication();
    app->Initialize();
    app->Launch();

    return 0;
}
