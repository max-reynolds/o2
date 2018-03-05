#include "stdafx.h"
#include "IPropertyField.h"

#include "SceneWindow/SceneEditScreen.h"
#include "UI/Widget.h"
#include "UI/Label.h"

namespace Editor
{
	void IPropertyField::SetValuePtr(const Vector<void*>& targets, bool isProperty)
	{
		SetValueAndPrototypePtr(targets.Select<Pair<void*, void*>>([](void* x) { return Pair<void*, void*>(x, nullptr); }), isProperty);
	}

	void IPropertyField::SetCaption(const WString& text)
	{
		if (mCaption)
			mCaption->text = text;

		auto captionWidget = dynamic_cast<UILabel*>(GetWidget()->FindChild("propertyName"));
		if (captionWidget)
			captionWidget->text = text;
		else
		{
			auto captionLayer = GetWidget()->GetLayerDrawable<Text>("caption");
			if (captionLayer)
				captionLayer->text = text;
		}
	}

	WString IPropertyField::GetCaption() const
	{
		if (mCaption)
			return mCaption->text;

		auto captionWidget = dynamic_cast<UILabel*>(GetWidget()->FindChild("propertyName"));
		if (captionWidget)
			return captionWidget->text;
		else
		{
			auto captionLayer = GetWidget()->GetLayerDrawable<Text>("caption");
			if (captionLayer)
				return captionLayer->text;
		}

		return "";
	}

	void IPropertyField::SetValuePath(const String& path)
	{
		mValuesPath = path;
	}

	const String& IPropertyField::GetValuePath() const
	{
		return mValuesPath;
	}

	void IPropertyField::SetCaptionLabel(UILabel* label)
	{
		mCaption = label;
	}

	UILabel* IPropertyField::GetCaptionLabel() const
	{
		return mCaption;
	}

	void IPropertyField::OnChanged()
	{
		CheckRevertableState();
		onChanged();
		o2EditorSceneScreen.OnSceneChanged();
	}
}

DECLARE_CLASS(Editor::IPropertyField);
