#include "stdafx.h"
#include "AssetIcon.h"

#include "AssetsWindow/AssetsIconsScroll.h"
#include "Render/Text.h"
#include "Scene/UI/Widgets/Label.h"
#include "Scene/UI/WidgetState.h"
#include "Utils/FileSystem/FileSystem.h"

namespace Editor
{
	AssetIcon::AssetIcon():
		Widget(), mNameText(nullptr)
	{
		onDraw += [&]() { CursorAreaEventsListener::OnDrawn(); };
		RetargetStatesAnimations();
	}

	AssetIcon::AssetIcon(const AssetIcon& other):
		Widget(other)
	{
		mNameText = FindChildByType<Label>();

		onDraw += [&]() { CursorAreaEventsListener::OnDrawn(); };
		RetargetStatesAnimations();
		SetLayoutDirty();
	}

	AssetIcon::~AssetIcon()
	{}

	AssetIcon& AssetIcon::operator=(const AssetIcon& other)
	{
		Widget::operator=(other);
		return *this;
	}

	void AssetIcon::SetAssetInfo(const AssetInfo& info)
	{
		mAssetInfo = info;

		if (mNameText)
			mNameText->text = o2FileSystem.GetPathWithoutDirectories(info.path);
	}

	const AssetInfo& AssetIcon::GetAssetInfo() const
	{
		return mAssetInfo;
	}

	bool AssetIcon::IsUnderPoint(const Vec2F& point)
	{
		return Widget::IsUnderPoint(point);
	}

	void AssetIcon::CopyData(const Actor& otherActor)
	{
		const AssetIcon& other = dynamic_cast<const AssetIcon&>(otherActor);

		Widget::CopyData(other);
		mNameText = FindChildByType<Label>();
	}

	void AssetIcon::SetSelected(bool selected)
	{
		SetState("selected", selected);
		if (mOwner)
			SetState("focused", mOwner->IsFocused());

		mIsSelected = selected;

		if (selected)
			OnSelected();
		else
			OnDeselected();
	}

	void AssetIcon::OnCursorDblClicked(const Input::Cursor& cursor)
	{
		if (mOwner)
			mOwner->OnAssetDblClick(this);
	}

	void AssetIcon::OnCursorRightMouseReleased(const Input::Cursor& cursor)
	{
		if (mOwner)
			mOwner->OnCursorRightMouseReleased(cursor);
	}

	void AssetIcon::OnCursorEnter(const Input::Cursor& cursor)
	{
		SetState("hover", true);
	}

	void AssetIcon::OnCursorExit(const Input::Cursor& cursor)
	{
		SetState("hover", false);
	}

	void AssetIcon::OnDragStart(const Input::Cursor& cursor)
	{
		if (mOwner)
			mOwner->BeginDragging(this);
	}

	void AssetIcon::OnDragged(const Input::Cursor& cursor, DragDropArea* area)
	{
		if (mOwner)
			mOwner->UpdateDraggingGraphics();
	}

	void AssetIcon::OnDragEnd(const Input::Cursor& cursor)
	{
		if (mOwner)
			mOwner->mDragEnded = true;
	}

	void AssetIcon::OnSelected()
	{
		if (mOwner)
			mOwner->Focus();
	}

	void AssetIcon::OnDeselected()
	{}

	void AssetIcon::OnDropped(ISelectableDragableObjectsGroup* group)
	{
		if (mOwner)
			mOwner->OnDropped(group);
	}

}

DECLARE_CLASS(Editor::AssetIcon);