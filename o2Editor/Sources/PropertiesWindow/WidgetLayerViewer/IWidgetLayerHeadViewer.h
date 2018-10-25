#pragma once

#include "Utils/Basic/IObject.h"
#include "Utils/Reflection/Reflection.h"
#include "Utils/Types/Containers/Vector.h"

using namespace o2;

namespace o2
{
	class UIWidget;
	class UIWidgetLayer;
}

namespace Editor
{
	// -------------------------------------------
	// Editor widget layer header viewer interface
	// -------------------------------------------
	class IWidgetLayerHeaderViewer : public IObject
	{
	public:
		// Virtual destructor
		virtual ~IWidgetLayerHeaderViewer() {}

		// Sets target actors
		virtual void SetTargetLayers(const Vector<UIWidgetLayer*>& layers) {}

		// Returns data widget
		virtual UIWidget* GetWidget() const { return nullptr; }

		// Updates properties values
		virtual void Refresh() {}

		IOBJECT(IWidgetLayerHeaderViewer);
	};
}

CLASS_BASES_META(Editor::IWidgetLayerHeaderViewer)
{
	BASE_CLASS(o2::IObject);
}
END_META;
CLASS_FIELDS_META(Editor::IWidgetLayerHeaderViewer)
{
}
END_META;
CLASS_METHODS_META(Editor::IWidgetLayerHeaderViewer)
{

	PUBLIC_FUNCTION(void, SetTargetLayers, const Vector<UIWidgetLayer*>&);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(void, Refresh);
}
END_META;