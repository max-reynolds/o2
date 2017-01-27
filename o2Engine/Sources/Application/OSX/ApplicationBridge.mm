#import "ApplicationBridge.h"

#include "Application/Application.h"
#include "Events/EventSystem.h"
#include "Render/Render.h"

namespace o2
{
    ApplicationOSXBridge::ApplicationOSXBridge(NSOpenGLView* viewController):
        mViewController(viewController)
    {}
    
    Vec2I ApplicationOSXBridge::GetContentSize() const
    {
        NSRect viewRectPoints = [mViewController bounds];
        NSRect viewRectPixels = [mViewController convertRectToBacking:viewRectPoints];
        
        return Vec2I(viewRectPixels.size.width, viewRectPixels.size.height);
    }
    
    void ApplicationOSXBridge::Shutdown() const
    {
        //[NSApp terminate:self];
    }
    
    void IApplicationOSXBridge::OnViewDidLayout()
    {
        o2Application.mWindowedSize = GetContentSize();
        o2Application.mRender->OnFrameResized();
        o2Application.onResizingEvent.Invoke();
        o2Application.OnResizing();
        o2Events.OnApplicationSized();
    }
    
    void IApplicationOSXBridge::OnViewUnload()
    {
        o2Events.OnApplicationClosing();
        o2Application.OnClosing();
        o2Application.onClosingEvent.Invoke();
    }
}
