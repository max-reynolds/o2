#pragma once

#include "Application/OSX/ApplicationBase.h"
#import <Cocoa/Cocoa.h>


namespace o2
{
    class ApplicationOSXBridge: public IApplicationOSXBridge
    {
    public:
        ApplicationOSXBridge(NSOpenGLView* viewController);
        Vec2I GetContentSize() const;
        void Shutdown() const;
        
    protected:
        NSOpenGLView* mViewController;
    };
}
