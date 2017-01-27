#pragma once

#include "ITestScreen.h"
#include "Application/Application.h"

class TestApplication: public o2::Application
{
protected:
	// Calls when application is starting
	void OnStarted();

	// Called on updating
	void OnUpdate(float dt);

	// Called on drawing
	void OnDraw();

public:
	// Turns on test screen with id
	void GoToScreen(const o2::String& id);

protected:
	TestScreensVec mTestScreens;
	ITestScreen*   mCurrentScreen;
	ITestScreen*   mNextCurrentScreen;

	void CheckArialFontEffects();
};
