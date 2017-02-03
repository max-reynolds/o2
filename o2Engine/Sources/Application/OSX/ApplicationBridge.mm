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
    
    void ApplicationOSXBridge::ApplyInputMessages()
    {
        for (auto msg : mInputMessages)
        {
            msg->Apply();
            delete msg;
        }
        
        mInputMessages.Clear();
    }
    
    void ApplicationOSXBridge::CursorPressed(const Vec2F& pos)
    {
        InputCursorPressedMsg* msg = new InputCursorPressedMsg();
        msg->position = pos;
        mInputMessages.Add(msg);
    }
    
    void ApplicationOSXBridge::SetCursorPos(const Vec2F& pos)
    {
        InputCursorMovedMsg* msg = new InputCursorMovedMsg();
        msg->position = pos;
        mInputMessages.Add(msg);
    }

    void ApplicationOSXBridge::CursorReleased()
    {
        InputCursorReleasedMsg* msg = new InputCursorReleasedMsg();
        mInputMessages.Add(msg);
    }
    
    void ApplicationOSXBridge::SetWindowCaption(const String& caption)
    {
        [[mViewController window] setTitle:[NSString stringWithUTF8String:caption.Data()]];
    }
    
    void ApplicationOSXBridge::InputCursorPressedMsg::Apply()
    {
        o2Input.CursorPressed(position);
    }
    
    void ApplicationOSXBridge::InputCursorMovedMsg::Apply()
    {
        o2Input.SetCursorPos(position);
    }
    
    void ApplicationOSXBridge::InputCursorReleasedMsg::Apply()
    {
        o2Input.CursorReleased();
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
