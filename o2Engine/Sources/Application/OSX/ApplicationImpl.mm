#include "Application/Application.h"

#include "Application/Input.h"
#include "Assets/Assets.h"
#include "Config/ProjectConfig.h"
#include "Events/EventSystem.h"
#include "Render/Render.h"
#include "Scene/Scene.h"
#include "UI/UIManager.h"
#include "Utils/Debug.h"
#include "Utils/FileSystem/FileSystem.h"
#include "Utils/Log/ConsoleLogStream.h"
#include "Utils/Log/FileLogStream.h"
#include "Utils/Log/LogStream.h"
#include "Utils/StackTrace.h"
#include "Utils/TaskManager.h"
#include "Utils/Time.h"
#include "Utils/Timer.h"
#include <time.h>

namespace o2
{
    void ApplicationBase::ConnectOSXViewController(IApplicationOSXBridge* bridge)
    {
        mOSXBridge = bridge;
    }
    
    Application::Application()
    {
        DataNode::RegBasicConverters();
        
        InitializeProperties();
        InitalizeSystems();
        
        mWindowed = true;
        mWindowedSize = Vec2I(800, 600);
        mWindowedPos = Vec2I(0, 0);
        mWindowResizible = true;
        mActive = false;
        
        mRender = mnew Render();
        
        o2Debug.InitializeFont();
        o2UI.TryLoadStyle();
        o2UI.UpdateRootSize();
        
        mReady = true;
    }
    
    void Application::CheckCursorInfiniteMode()
    {
        mCursorPositionCorrecting = true;
        
        /*int threshold = 10;
        POINT p;
        GetCursorPos(&p);
        
        Vec2I resolution = GetScreenResolution();
        
        if (p.x > resolution.x - threshold)
            p.x = threshold;
        else if (p.x < threshold)
            p.x = resolution.x - threshold;
        
        if (p.y > resolution.y - threshold)
            p.y = threshold;
        else if (p.y < threshold)
            p.y = resolution.y - threshold;
        
        SetCursorPos(p.x, p.y);*/
        
        mCursorPositionCorrecting = false;
    }
    
    void Application::Launch()
    {
        mLog->Out("Application launched!");
        
        OnStarted();
        onStartedEvent.Invoke();
        o2Events.OnApplicationStarted();
    }
    
    void Application::Shutdown()
    {
        //DestroyWindow(mHWnd);
    }
    
    void Application::SetFullscreen(bool fullscreen /*= true*/)
    {
        if (fullscreen)
        {
            //mRenderSystem->FrameResized();
            mLog->Out("Setting full screen");
        }
        else
        {
            mLog->Out("Setting windowed..");
            
            mWindowed = true;
            
            /*RECT rt = { mWindowedPos.x, mWindowedPos.y, mWindowedPos.x + mWindowedSize.x, mWindowedPos.y + mWindowedSize.y };
            AdjustWindowRect(&rt, mWndStyle, false);
            SetWindowPos(mHWnd, HWND_NOTOPMOST, mWindowedPos.x, mWindowedPos.y,
                         mWindowedSize.x, mWindowedSize.y, SWP_SHOWWINDOW);*/
            
            //mRenderSystem->FrameResized();
            mLog->Out("Complete");
        }
    }
    
    bool Application::IsFullScreen() const
    {
        return !mWindowed;
    }
    
    void Application::Maximize()
    {
        //ShowWindow(mHWnd, SW_MAXIMIZE);
    }
    
    bool Application::IsMaximized() const
    {
        /*WINDOWPLACEMENT pl;
        GetWindowPlacement(mHWnd, &pl);
        return pl.showCmd == SW_MAXIMIZE;*/
        return false;
    }
    
    void Application::SetResizible(bool resizible)
    {
        if (resizible == mWindowResizible)
            return;
        
        mWindowResizible = resizible;
        
        /*if (mWindowResizible)
            mWndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
        else
            mWndStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_MINIMIZEBOX;
        
        mLog->Out("set resizible: %s ", (mWindowResizible ? "true" : "false"));*/
        
        //SetWindowLong(mHWnd, GWL_STYLE, mWndStyle);
    }
    
    bool Application::IsResizible() const
    {
        return mWindowResizible;
    }
    
    void Application::SetWindowSize(const Vec2I& size)
    {
        mWindowedSize = size;
        mLog->Out("setWindowSize: %ix%i", mWindowedSize.x, mWindowedSize.y);
        SetFullscreen(!mWindowed);
    }
    
    Vec2I Application::GetWindowSize() const
    {
        return mWindowedSize;
    }
    
    void Application::SetWindowPosition(const Vec2I& position)
    {
        mWindowedPos = position;
        mLog->Out("set Window Position: %i, %i", mWindowedPos.x, mWindowedPos.y);
        SetFullscreen(!mWindowed);
    }
    
    Vec2I Application::GetWindowPosition() const
    {
        return mWindowedPos;
    }
    
    void Application::SetWindowCaption(const String& caption)
    {
        mWndCaption = caption;
        mOSXBridge->SetWindowCaption(caption);
    }
    
    String Application::GetWindowCaption() const
    {
        return mWndCaption;
    }
    
    void Application::SetContentSize(const Vec2I& size)
    {
        Vec2I clientRectSize = size;
        
        /*RECT clientRect;
        GetClientRect(mHWnd, &clientRect);
        clientRect.right = clientRect.left + size.x;
        clientRect.bottom = clientRect.top + size.y;
        
        AdjustWindowRect(&clientRect, mWndStyle, false);
        
        mWindowedPos = Vec2I(clientRect.left, clientRect.top);
        mWindowedSize = Vec2I(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);*/
        
        mLog->Out("Set Content Size: %ix%i", size.x, size.y);
        
        SetFullscreen(!mWindowed);
        
        mRender->OnFrameResized();
        onResizingEvent();
    }
    
    Vec2I Application::GetContentSize() const
    {
        if (mOSXBridge)
            return mOSXBridge->GetContentSize();
        /*RECT clientRect;
        GetClientRect(mHWnd, &clientRect);
        return Vec2I(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);*/
        return Vec2I();
    }
    
    Vec2I Application::GetScreenResolution() const
    {
        //int hor = GetSystemMetrics(SM_CXSCREEN), ver = GetSystemMetrics(SM_CYSCREEN);
        //return Vec2I(hor, ver);
        return Vec2I();
    }
    
    void Application::SetCursor(CursorType type)
    {
        /*LPSTR cursorsIds[] = { IDC_APPSTARTING, IDC_ARROW, IDC_CROSS, IDC_HAND, IDC_HELP, IDC_IBEAM, IDC_ICON, IDC_NO,
            IDC_SIZEALL, IDC_SIZENESW, IDC_SIZENS, IDC_SIZENWSE, IDC_SIZEWE, IDC_UPARROW, IDC_WAIT };
        
        mCurrentCursor = LoadCursor(NULL, cursorsIds[(int)type]);
        ::SetCursor(mCurrentCursor);
        SetClassLong(mHWnd, GCL_HCURSOR, (DWORD)mCurrentCursor);*/
    }
    
    void Application::SetCursorPosition(const Vec2F& position)
    {
        //SetCursorPos((int)position.x, (int)position.y);
    }
    
    
    String Application::GetBinPath() const
    {
        /*TCHAR szFileName[MAX_PATH];
        GetModuleFileName(NULL, szFileName, MAX_PATH);
        return o2FileSystem.GetParentPath((String)szFileName);*/
        return "";
    }
}
