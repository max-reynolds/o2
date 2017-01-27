#pragma once

#include "ITestScreen.h"
#include "Render/Sprite.h"
#include "Render/Text.h"
#include "Utils/DragHandle.h"

class TextTestScreen: public ITestScreen
{
public:
	TextTestScreen(TestApplication* application);
	~TextTestScreen();

	void Load();
	void Unload();

	void Update(float dt);
	void Draw();
	o2::String GetId() const;

protected:
	o2::Sprite mBackground;
	o2::Sprite mFakeWindow;
	o2::DragHandle mHandleMin;
	o2::DragHandle mHandleMax;
	o2::Text mText;

	void OnHandleMoved(const o2::Vec2F& pos);
};
