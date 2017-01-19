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
	DECLARE_SINGLETON(Application);

	Application::~Application()
	{
		DeinitializeSystems();
	}

	void Application::InitalizeSystems()
	{
		srand((UInt)time(NULL));

		mLog = mnew LogStream("Application");
		o2Debug.GetLog()->BindStream(mLog);

		mProjectConfig = mnew ProjectConfig();

		mAssets = mnew Assets();

		mInput = mnew Input();
		mTaskManager = mnew TaskManager();

		mTimer = mnew Timer();
		mTimer->Reset();

		mTime = mnew Time();

		mEventSystem = mnew EventSystem();

		mUIManager = mnew UIManager();

		mScene = mnew Scene();

		mLog->Out("Initialized");
	}

	void Application::DeinitializeSystems()
	{
		delete mScene;
		delete mUIManager;
		delete mRender;
		delete mInput;
		delete mTime;
		delete mTimer;
		delete mProjectConfig;
		delete mAssets;
		delete mEventSystem;
		delete mTaskManager;
	}

	void Application::ProcessFrame()
	{
		if (!mReady)
			return;

		if (mCursorInfiniteModeEnabled)
			CheckCursorInfiniteMode();

		float realdDt = mTimer->GetDeltaTime();
		float dt = Math::Clamp(realdDt, 0.001f, 0.05f);

		mTime->Update(realdDt);
		o2Debug.Update(dt);

		mTaskManager->Update(dt);

		mEventSystem->Update(dt);

		mScene->Update(dt);

		OnUpdate(dt);

		mUIManager->Update(dt);

		mRender->Begin();
		OnDraw();
		mScene->Draw();
		o2Debug.Draw();
		mUIManager->Draw();
		mRender->End();

		mInput->Update(dt);
	}

	LogStream* Application::GetLog() const
	{
		return mInstance->mLog;
	}

	Input* Application::GetInput() const
	{
		return mInstance->mInput;
	}

	ProjectConfig* Application::GetProjectConfig() const
	{
		return mInstance->mProjectConfig;
	}

	Time* Application::GetTime() const
	{
		return mInstance->mTime;
	}

	void Application::OnMoved()
	{}

	void Application::OnResizing()
	{}

	void Application::OnClosing()
	{}

	void Application::OnStarted()
	{}

	void Application::OnDeactivated()
	{}

	void Application::OnActivated()
	{}
    
	void Application::OnUpdate(float dt)
	{}

	void Application::OnDraw()
	{}
    
	bool Application::IsReady()
	{
		return IsSingletonInitialzed() && Application::Instance().mReady;
	}
    
	void Application::SetCursorInfiniteMode(bool enabled)
	{
		mCursorInfiniteModeEnabled = enabled;
	}

	bool Application::IsCursorInfiniteModeOn() const
	{
		return mCursorInfiniteModeEnabled;
	}

	bool Application::IsEditor() const
	{
		return IS_EDITOR;
	}

	void Application::InitializeProperties()
	{
		INITIALIZE_PROPERTY(Application, fullscreen, SetFullscreen, IsFullScreen);
		INITIALIZE_PROPERTY(Application, resizible, SetResizible, IsResizible);
		INITIALIZE_PROPERTY(Application, windowSize, SetWindowSize, GetWindowSize);
		INITIALIZE_PROPERTY(Application, windowContentSize, SetContentSize, GetContentSize);
		INITIALIZE_PROPERTY(Application, windowPosition, SetWindowPosition, GetWindowPosition);
		INITIALIZE_PROPERTY(Application, windowCaption, SetWindowCaption, GetWindowCaption);
	}

	o2StackWalker* o2StackWalker::mInstance = new o2StackWalker();
	MemoryManager* MemoryManager::mInstance = new MemoryManager();
	template<> Debug* Singleton<Debug>::mInstance = mnew Debug();
	template<> FileSystem* Singleton<FileSystem>::mInstance = mnew FileSystem();
}
