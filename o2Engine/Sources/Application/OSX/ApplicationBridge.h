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
        
        void CursorPressed(const Vec2F& pos);
        void SetCursorPos(const Vec2F& pos);
        void CursorReleased();
        
        void SetWindowCaption(const String& caption);
        
        void ApplyInputMessages();
        
    protected:
        struct IInputMsg
        {
            virtual ~IInputMsg() {}
            virtual void Apply() = 0;
        };
        typedef Vector<IInputMsg*> InputMsgsVec;
        
        struct InputCursorPressedMsg: public IInputMsg
        {
            Vec2F position;
            void Apply();
        };
        
        struct InputCursorMovedMsg: public IInputMsg
        {
            Vec2F position;
            void Apply();
        };
        
        struct InputCursorReleasedMsg: public IInputMsg
        {
            void Apply();
        };
        
    protected:
        NSOpenGLView* mViewController;
        InputMsgsVec  mInputMessages;
    };
}
