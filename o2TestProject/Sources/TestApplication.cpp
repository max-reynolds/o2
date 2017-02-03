#include "TestApplication.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include "Assets/VectorFontAsset.h"
#include "MainTestScreen.h"
#include "Render/Camera.h"
#include "Render/Render.h"
#include "Render/VectorFontEffects.h"
#include "TextTestScreen.h"
#include "UITestScreen.h"
#include "UI/UIManager.h"

#undef DrawText

void TestApplication::OnStarted()
{
	//CheckArialFontEffects();
	o2UI.LoadStyle("ui_style.xml");
	mTestScreens.Add(mnew TextTestScreen(this));
	mTestScreens.Add(mnew UITestScreen(this));
	mTestScreens.Add(mnew MainTestScreen(this));

	//GoToScreen("MainTestScreen");
	GoToScreen("UITestScreen");
	//GoToScreen("TextTestScreen");
}

void TestApplication::OnUpdate(float dt)
{
	o2Application.windowCaption = o2::String::Format("FPS: %i DC: %i", (int)o2Time.GetFPS(), o2Render.GetDrawCallsCount());

	/*if (o2Input.IsKeyPressed('Z'))
	{
		o2Debug.Log("Collect garbage...");
		o2Memory.CollectGarbage();
		o2Debug.Log("Collect garbage done!");
	}*/

	if (mCurrentScreen)
		mCurrentScreen->Update(dt);

	if (mNextCurrentScreen)
	{
		if (mCurrentScreen)
			mCurrentScreen->Unload();

		mCurrentScreen = mNextCurrentScreen;
		mCurrentScreen->Load();

		mNextCurrentScreen = nullptr;
	}
}

void TestApplication::OnDraw()
{
	o2Render.Clear();
	o2Render.camera = o2::Camera::Default();

	if (mCurrentScreen)
		mCurrentScreen->Draw();
}

void TestApplication::GoToScreen(const o2::String& id)
{
	mNextCurrentScreen = mTestScreens.FindMatch([&](ITestScreen* x) { return x->GetId() == id; });
}

void TestApplication::CheckArialFontEffects()
{
}

