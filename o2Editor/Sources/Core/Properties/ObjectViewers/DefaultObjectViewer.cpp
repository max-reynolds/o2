#include "stdafx.h"
#include "DefaultObjectViewer.h"

#include "Core/EditorScope.h"
#include "Core/Properties/Properties.h"
#include "Scene/UI/Widgets/VerticalLayout.h"

namespace Editor
{
	DefaultObjectViewer::DefaultObjectViewer()
	{}

	void DefaultObjectViewer::Refresh(const TargetsVec& targetObjets)
	{
		PushEditorScopeOnStack scope;

		if (targetObjets.IsEmpty())
			return;

		const Type* objectsType = &(targetObjets[0].first)->GetType();

		if (mRealObjectType != objectsType || mBuiltWithHiddenProperties != o2EditorProperties.IsPrivateFieldsVisible())
		{
			mRealObjectType = objectsType;

			if (mRealObjectType)
				o2EditorProperties.FreeProperties(mPropertiesContext);

			if (mRealObjectType)
			{
				o2EditorProperties.BuildObjectProperties(dynamic_cast<VerticalLayout*>(mLayout), mRealObjectType,
														 mPropertiesContext, "", mOnChildFieldChangeCompleted, onChanged);

				mBuiltWithHiddenProperties = o2EditorProperties.IsPrivateFieldsVisible();
			}
		}

		mPropertiesContext.Set(targetObjets);
	}

	const Type* DefaultObjectViewer::GetViewingObjectType() const
	{
		return mRealObjectType;
	}
}

DECLARE_CLASS(Editor::DefaultObjectViewer);
