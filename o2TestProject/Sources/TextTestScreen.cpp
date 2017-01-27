#include "TextTestScreen.h"

#include "TestApplication.h"
#include "Render/Render.h"

TextTestScreen::TextTestScreen(TestApplication* application):
	ITestScreen(application)
{
}

TextTestScreen::~TextTestScreen()
{
}

void TextTestScreen::Load()
{
	mBackground.LoadFromImage("ui/UI_Background.png");
	mBackground.size = (o2::Vec2I)o2Render.resolution;

	mFakeWindow.LoadFromImage("ui/UI_window_frame_regular.png");
    mFakeWindow.size = o2::Vec2F(400, 400);

	mText.font = o2::FontRef("stdFont.ttf");
	//mText.font = FontRef("myriad.xml");
	mText.text = "Hello, I'm text\nmulti line\nand many symbols\nha h a h ah\n1 2 3 4 5 6 7 8 9 0";
	//mText.ctext = "abcdefghklmnopqrstuvxyz12345";

	mHandleMin.regularSprite = mnew o2::Sprite("ui/UI_Radio_bk.png");
	mHandleMin.hoverSprite = mnew o2::Sprite("ui/UI_Radio_bk_select.png");
	mHandleMin.pressedSprite = mnew o2::Sprite("ui/UI_Radio_bk_pressed.png");
	mHandleMin.onChangedPos += o2::Function<void(const o2::Vec2F&)>(this, &TextTestScreen::OnHandleMoved);

	mHandleMax.regularSprite = mnew o2::Sprite("ui/UI_Radio_bk.png");
	mHandleMax.hoverSprite = mnew o2::Sprite("ui/UI_Radio_bk_select.png");
	mHandleMax.pressedSprite = mnew o2::Sprite("ui/UI_Radio_bk_pressed.png");
	mHandleMax.onChangedPos += o2::Function<void(const o2::Vec2F&)>(this, &TextTestScreen::OnHandleMoved);

	mHandleMin.position = o2::Vec2F(-100, -100);
	mHandleMax.position = o2::Vec2F(100, 100);
}

void TextTestScreen::Unload()
{
}

void TextTestScreen::Update(float dt)
{
	if (o2Input.IsKeyPressed('Q'))
		mText.horAlign = o2::HorAlign::Left;

	if (o2Input.IsKeyPressed('W'))
		mText.horAlign = o2::HorAlign::Middle;

	if (o2Input.IsKeyPressed('E'))
		mText.horAlign = o2::HorAlign::Right;

	if (o2Input.IsKeyPressed('R'))
		mText.horAlign = o2::HorAlign::Both;

	if (o2Input.IsKeyPressed('A'))
		mText.verAlign = o2::VerAlign::Top;

	if (o2Input.IsKeyPressed('S'))
		mText.verAlign = o2::VerAlign::Middle;

	if (o2Input.IsKeyPressed('D'))
		mText.verAlign = o2::VerAlign::Bottom;

	if (o2Input.IsKeyPressed('F'))
		mText.verAlign = o2::VerAlign::Both;

	if (o2Input.IsKeyPressed('G'))
		mText.wordWrap = !mText.wordWrap;

	if (o2Input.IsKeyPressed(VK_ESCAPE))
		mApplication->GoToScreen("MainTestScreen");
}

void TextTestScreen::Draw()
{
	mBackground.Draw();
	mFakeWindow.Draw();
	mText.Draw();
	mHandleMin.Draw();
	mHandleMax.Draw();
	o2Render.DrawLine(o2::Vec2F(), o2::Vec2F(1000, 0));
	o2Render.DrawRectFrame(mHandleMin.position, mHandleMax.position);
}

o2::String TextTestScreen::GetId() const
{
	return "TextTestScreen";
}

void TextTestScreen::OnHandleMoved(const o2::Vec2F& pos)
{
	mText.SetRect(o2::RectF(mHandleMin.position, mHandleMax.position));
}
