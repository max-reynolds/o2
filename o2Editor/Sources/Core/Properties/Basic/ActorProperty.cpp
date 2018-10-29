#include "stdafx.h"
#include "ActorProperty.h"

#include "Assets/Assets.h"
#include "AssetsWindow/AssetsIconsScroll.h"
#include "AssetsWindow/AssetsWindow.h"
#include "Core/Properties/Properties.h"
#include "PropertiesWindow/PropertiesWindow.h"
#include "TreeWindow/SceneTree.h"
#include "TreeWindow/TreeWindow.h"
#include "UI/UIManager.h"
#include "UI/Widget.h"
#include "UI/WidgetLayer.h"

namespace Editor
{
	ActorProperty::ActorProperty()
	{
		mCommonValue = nullptr;
	}

	ActorProperty::ActorProperty(const ActorProperty& other) :
		TPropertyField<Actor*>(other)
	{
		InitializeControls();
	}

	ActorProperty& ActorProperty::operator=(const ActorProperty& other)
	{
		TPropertyField<Actor*>::operator=(other);
		return *this;
	}

	void ActorProperty::CopyData(const Actor& otherActor)
	{
		TPropertyField<Actor*>::CopyData(otherActor);
		InitializeControls();
	}

	void ActorProperty::InitializeControls()
	{
		mCommonValue = nullptr;

		mBox = GetChildWidget("layout/box");
		if (mBox)
		{
			mBox->SetFocusable(true);
			mBox->onDraw += Func<DragDropArea, void>(this, &DragDropArea::OnDrawn);

			mNameText = mBox->GetLayerDrawable<Text>("caption");
			if (mNameText)
				mNameText->text = "--";
		}
	}

	void ActorProperty::Revert()
	{
		auto propertyObjects = o2EditorPropertiesWindow.GetTargets();

		for (int i = 0; i < mValuesProxies.Count() && i < propertyObjects.Count(); i++)
			RevertoToPrototype(mValuesProxies[i].first, mValuesProxies[i].second, propertyObjects[i]);

		Refresh();
	}

	bool ActorProperty::IsUnderPoint(const Vec2F& point)
	{
		return mBox->IsUnderPoint(point);
	}

	void ActorProperty::RevertoToPrototype(IAbstractValueProxy* target, IAbstractValueProxy* source,
										   IObject* targetOwner)
	{
		if (!source || !targetOwner || targetOwner->GetType().IsBasedOn(TypeOf(Component)))
			return;

		Actor* sourceActor = GetProxy<Actor*>(source);
		Actor* topSourceActor = sourceActor;
		while (topSourceActor->GetParent())
			topSourceActor = topSourceActor->GetParent();

		Actor* actorOwner = dynamic_cast<Actor*>(targetOwner);

		if (actorOwner)
		{
			Actor* topTargetActor = actorOwner;
			while (topTargetActor->GetPrototypeLink() != topSourceActor && topTargetActor->GetParent())
				topTargetActor = topTargetActor->GetParent();

			Actor* sameToProtoSourceActor = topTargetActor->FindLinkedActor(sourceActor);

			if (sameToProtoSourceActor)
			{
				SetProxy<Actor*>(target, sameToProtoSourceActor);
				return;
			}
		}

		if (sourceActor->IsOnScene())
			SetProxy<Actor*>(target, sourceActor);
	}

	bool ActorProperty::IsValueRevertable() const
	{
		bool revertable = false;

		for (auto ptr : mValuesProxies)
		{
			if (ptr.second)
			{
				Actor* value = GetProxy<Actor*>(ptr.first);
				Actor* proto = GetProxy<Actor*>(ptr.second);

				if (value && value->GetPrototypeLink())
				{
					if (value->GetPrototypeLink() != proto)
					{
						revertable = true;
						break;
					}
				}
				else
				{
					if (value != proto)
					{
						revertable = true;
						break;
					}
				}
			}
		}

		return revertable;
	}

	void ActorProperty::UpdateValueView()
	{
		if (!mValuesDifferent)
		{
			if (!mCommonValue)
			{
				mNameText->text = "Null:Actor";
				mBox->layer["caption"]->transparency = 0.5f;
			}
			else
			{
				mNameText->text = mCommonValue->GetName();
				mBox->layer["caption"]->transparency = 1.0f;
			}
		}
		else
		{
			mNameText->text = "--";
			mBox->layer["caption"]->transparency = 1.0f;
		}
	}

	void ActorProperty::OnCursorEnter(const Input::Cursor& cursor)
	{
		mBox->SetState("hover", true);
	}

	void ActorProperty::OnCursorExit(const Input::Cursor& cursor)
	{
		mBox->SetState("hover", false);
	}

	void ActorProperty::OnCursorPressed(const Input::Cursor& cursor)
	{
		o2UI.FocusWidget(mBox);

		if (mCommonValue)
		{
			if (mCommonValue->IsAsset())
				o2EditorAssets.ShowAssetIcon(o2Assets.GetAssetPath(mCommonValue->GetAssetID()));
			else if (mCommonValue->IsOnScene())
				o2EditorTree.HightlightObjectTreeNode(mCommonValue);
		}
	}

	void ActorProperty::OnKeyPressed(const Input::Key& key)
	{
		if (mBox && mBox->IsFocused() && (key == VK_DELETE || key == VK_BACK))
			SetValueByUser(nullptr);
	}

	void ActorProperty::OnDropped(ISelectableDragableObjectsGroup* group)
	{
		if (auto* actorsTree = dynamic_cast<UISceneTree*>(group))
			OnDroppedFromActorsTree(actorsTree);
		else if (auto* assetsScroll = dynamic_cast<UIAssetsIconsScrollArea*>(group))
			OnDroppedFromAssetsScroll(assetsScroll);
	}

	void ActorProperty::OnDragEnter(ISelectableDragableObjectsGroup* group)
	{
		if (auto* actorsTree = dynamic_cast<UISceneTree*>(group))
			OnDragEnterFromActorsTree(actorsTree);
		else if (auto* assetsScroll = dynamic_cast<UIAssetsIconsScrollArea*>(group))
			OnDragEnterFromAssetsScroll(assetsScroll);
	}

	void ActorProperty::OnDragExit(ISelectableDragableObjectsGroup* group)
	{
		if (auto* actorsTree = dynamic_cast<UISceneTree*>(group))
			OnDragExitFromActorsTree(actorsTree);
		else if (auto* assetsScroll = dynamic_cast<UIAssetsIconsScrollArea*>(group))
			OnDragExitFromAssetsScroll(assetsScroll);
	}

	void ActorProperty::OnDroppedFromActorsTree(UISceneTree* actorsTree)
	{
		if (actorsTree->GetSelectedObjects().Count() > 1)
			return;

		SetValueByUser(dynamic_cast<Actor*>(actorsTree->GetSelectedObjects()[0]));

		o2Application.SetCursor(CursorType::Arrow);
		mBox->Focus();
	}

	void ActorProperty::OnDragEnterFromActorsTree(UISceneTree* actorsTree)
	{
		if (actorsTree->GetSelectedObjects().Count() > 1)
			return;

		o2Application.SetCursor(CursorType::Hand);
		mBox->SetState("focused", true);
	}

	void ActorProperty::OnDragExitFromActorsTree(UISceneTree* actorsTree)
	{
		if (actorsTree->GetSelectedObjects().Count() > 1)
			return;

		o2Application.SetCursor(CursorType::Arrow);
		mBox->SetState("focused", false);
	}

	void ActorProperty::OnDroppedFromAssetsScroll(UIAssetsIconsScrollArea* assetsIconsScroll)
	{
		if (assetsIconsScroll->GetSelectedAssets().Count() > 1)
			return;

		auto lastSelectedAsset = assetsIconsScroll->GetSelectedAssets().Last();
		if (lastSelectedAsset.assetType != &TypeOf(ActorAsset))
			return;

		SetValueByUser(o2Scene.GetAssetActorByID(lastSelectedAsset.id));

		o2Application.SetCursor(CursorType::Arrow);
		mBox->Focus();
	}

	void ActorProperty::OnDragEnterFromAssetsScroll(UIAssetsIconsScrollArea* assetsIconsScroll)
	{
		if (assetsIconsScroll->GetSelectedAssets().Count() > 1)
			return;

		auto lastSelectedAsset = assetsIconsScroll->GetSelectedAssets().Last();
		if (lastSelectedAsset.assetType != &TypeOf(ActorAsset))
			return;

		o2Application.SetCursor(CursorType::Hand);
		mBox->SetState("focused", true);
	}

	void ActorProperty::OnDragExitFromAssetsScroll(UIAssetsIconsScrollArea* assetsIconsScroll)
	{
		if (assetsIconsScroll->GetSelectedAssets().Count() > 1)
			return;

		o2Application.SetCursor(CursorType::Arrow);
		mBox->SetState("focused", false);
	}
}
DECLARE_CLASS_MANUAL(Editor::TPropertyField<o2::Actor*>);

DECLARE_CLASS(Editor::ActorProperty);
