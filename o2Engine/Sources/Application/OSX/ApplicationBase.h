#pragma once

#include "Application/OSX/VKCodes.h"
#include "Utils/Math/Vector2.h"
#include "Utils/String.h"

namespace o2 {
    class IApplicationOSXBridge
    {
    public:
        virtual Vec2I GetContentSize() const = 0;
        virtual void Shutdown() const = 0;
        virtual void OnViewDidLayout();
        virtual void OnViewUnload();
    };
    
    class ApplicationBase
    {
    protected:
        bool                   mWindowed;        // True if app in windowed mode, false if in fullscreen mode
        bool                   mWindowResizible; // True, if window can be sized by user
        Vec2I                  mWindowedSize;    // Size of window
        Vec2I                  mWindowedPos;     // Position of window
        String                 mWndCaption;      // Window caption
        bool                   mActive;          // True, if window is active
        
        IApplicationOSXBridge* mOSXBridge;       // OSX objective-c application connection bridge
        
    public:
        void ConnectOSXViewController(IApplicationOSXBridge* bridge);
    };
}
