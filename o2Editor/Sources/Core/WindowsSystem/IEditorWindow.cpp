#include "IEditorWindow.h"

IEditorWindow::IEditorWindow()
{
	if (Application::IsReady())
	{
		mWindow = o2UI.CreateWidget<UIWindow>();
		o2UI.AddWidget(mWindow);
	}
}

IEditorWindow::IEditorWindow(const IEditorWindow& other)
{
	if (Application::IsReady())
	{
		mWindow = o2UI.CreateWidget<UIDockableWindow>();
		mWindow->layout.size = Vec2F(200, 200);
		o2UI.AddWidget(mWindow);
	}
}

IEditorWindow::~IEditorWindow()
{
	mWindow.Release();
}

void IEditorWindow::Show()
{
	SetVisible(true);
}

void IEditorWindow::Hide()
{
	SetVisible(false);
}

void IEditorWindow::SetVisible(bool visible)
{
	mWindow->SetVisible(visible);
}

void IEditorWindow::Update(float dt)
{}

void IEditorWindow::Draw()
{}

bool IEditorWindow::IsVisible()
{
	return mWindow->IsVisible();
}
