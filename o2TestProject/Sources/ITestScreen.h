#pragma once

#include "Utils/IObject.h"
#include "Utils/Reflection/Reflection.h"

class TestApplication;

class Test: public o2::IObject
{
public:
	IOBJECT(Test);
};

class ITestScreen
{
public:
	ITestScreen(TestApplication* application):mApplication(application) {}
	virtual ~ITestScreen() {}

	virtual void Load() = 0;
	virtual void Unload() = 0;

	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;
	virtual o2::String GetId() const = 0;

protected:
	TestApplication* mApplication;
};
typedef o2::Vector<ITestScreen*> TestScreensVec;
