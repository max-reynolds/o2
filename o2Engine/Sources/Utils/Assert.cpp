#include "Assert.h"

#include <string>

#ifdef WINDOWS
#include <windows.h>
#endif

namespace o2
{
#ifdef WINDOWS
    void ErrorMessage(const char* desc, const char* file, long line)
    {
        char message[1024];
        sprintf(message, "Error at\n%s : %i\nDescription:\n%s", file, line, desc);
        
        MessageBox(nullptr, message, "Error", MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }
#endif
    
#ifdef OSX
    void ErrorMessage(const char* desc, const char* file, long line)
    {
        char message[1024];
        printf(message, "Error at\n%s : %i\nDescription:\n%s", file, line, desc);
    }
#endif
    
}
