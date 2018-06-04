#pragma once

#include "Core/Properties/IPropertyField.h"
#include "Scene/Tags.h"

namespace o2
{
	class UIButton;
	class UIContextMenu;
	class UIEditBox;
	class UIWidget;
}

namespace Editor
{
	// ------------------------
	// Editor tags property box
	// ------------------------
	class TagsProperty: public TPropertyField<TagGroup>
	{
	public:
		// Default constructor
		TagsProperty();

		// Copy constructor
		TagsProperty(const TagsProperty& other);

		// Copy operator
		TagsProperty& operator=(const TagsProperty& other);

		IOBJECT(TagsProperty);

	protected:
		UIEditBox*     mEditBox = nullptr;        // Edit box 
		UIContextMenu* mTagsContext = nullptr;    // tags context
		bool           mPushingTag = false;       // Is pushing tag and we don't need to check edit text

	protected:
		// Copies data of actor from other to this
		void CopyData(const Actor& otherActor) override;

		// Updates value view
		void UpdateValueView() override;

		// Sets common value
		void SetCommonValue(const TagGroup& value) override;

		// Searches controls widgets and layers and initializes them
		void InitializeControls();

		// Updates context menu data with filter
		void UpdateContextData(const WString& filter);

		// It is called when edit box changed
		void OnEditBoxChanged(const WString& text);

		// It is called when edit box changed
		void OnEditBoxChangeCompleted(const WString& text);

		// Sets tags from string
		void SetTags(const WString& text);

		// Push tag at the end
		void PushTag(String name);
	};
}

CLASS_BASES_META(Editor::TagsProperty)
{
	BASE_CLASS(Editor::TPropertyField<TagGroup>);
}
END_META;
CLASS_FIELDS_META(Editor::TagsProperty)
{
	PROTECTED_FIELD(mEditBox);
	PROTECTED_FIELD(mTagsContext);
	PROTECTED_FIELD(mPushingTag);
}
END_META;
CLASS_METHODS_META(Editor::TagsProperty)
{

	PROTECTED_FUNCTION(void, CopyData, const Actor&);
	PROTECTED_FUNCTION(void, UpdateValueView);
	PROTECTED_FUNCTION(void, SetCommonValue, const TagGroup&);
	PROTECTED_FUNCTION(void, InitializeControls);
	PROTECTED_FUNCTION(void, UpdateContextData, const WString&);
	PROTECTED_FUNCTION(void, OnEditBoxChanged, const WString&);
	PROTECTED_FUNCTION(void, OnEditBoxChangeCompleted, const WString&);
	PROTECTED_FUNCTION(void, SetTags, const WString&);
	PROTECTED_FUNCTION(void, PushTag, String);
}
END_META;
