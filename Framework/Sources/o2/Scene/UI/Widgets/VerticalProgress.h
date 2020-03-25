#pragma once

#include "o2/Scene/UI/Widget.h"
#include "o2/Events/DrawableCursorEventsListener.h"

namespace o2
{
	// ---------------------
	// Vertical progress bar
	// ---------------------
	class VerticalProgress: public Widget, public DrawableCursorEventsListener
	{
	public:
		enum class Orientation { Down, Up };

	public:
		PROPERTIES(VerticalProgress);
		PROPERTY(float, value, SetValue, GetValue); // Current value property

		PROPERTY(float, minValue, SetMinValue, GetMinValue); // Minimal value property
		PROPERTY(float, maxValue, SetMaxValue, GetMaxValue); // Maximal value property

		PROPERTY(float, scrollSense, SetScrollSense, GetScrollSense); // Scroll sense coefficient

	public:
		Function<void(float)> onChange; // On Value changing event

	public:
		// Constructor
		VerticalProgress();

		// Copy-constructor
		VerticalProgress(const VerticalProgress& other);

		// Destructor
		~VerticalProgress();

		// Copy-operator
		VerticalProgress& operator=(const VerticalProgress& other);

		// Updates widget and smooth value changing
		void Update(float dt) override;

		// Sets value
		void SetValue(float value);

		// Sets value forcible, without smoothing
		void SetValueForcible(float value);

		// Returns value
		float GetValue() const;

		// Sets minimal value
		void SetMinValue(float minValue);

		// Returns minimal value
		float GetMinValue() const;

		// Sets maximal value
		void SetMaxValue(float maxValue);

		// Returns maximal value
		float GetMaxValue() const;

		// Sets value range
		void SetValueRange(float minValue, float maxValue);

		// Sets scroll sense
		void SetScrollSense(float coef);

		// Returns scroll sense
		float GetScrollSense() const;

		// Sets bar orientation
		void SetOrientation(Orientation orientation);

		// Returns bar orientation
		Orientation GetOrientation() const;

		// Returns true if point is in this object
		bool IsUnderPoint(const Vec2F& point) override;

		// Returns is listener scrollable
		bool IsScrollable() const override;

		SERIALIZABLE(VerticalProgress);

	protected:
		float        mValue = 0.0f;                    // Current value @SERIALIZABLE
		float        mSmoothValue = 0.0f;              // Current smoothed value
		float        mMinValue = 0.0f;                 // Minimal value @SERIALIZABLE
		float        mMaxValue = 1.0f;                 // Maximal value @SERIALIZABLE
		float        mScrollSense = 1.0f;              // Scroll sense coefficient @SERIALIZABLE
		Orientation  mOrientation = Orientation::Down; // Bar orientation @SERIALIZABLE
		WidgetLayer* mBarLayer = nullptr;              // Bar layer
		WidgetLayer* mBackLayer = nullptr;             // Background layer

	protected:
		// Copies data of actor from other to this
		void CopyData(const Actor& otherActor) override;

		// It is called when new layer was added. Here searching bar, back and handle layers
		void OnLayerAdded(WidgetLayer* layer) override;

		// It is called when deserialized
		void OnDeserialized(const DataNode& node) override;

		// It is called when visible was changed
		void OnEnableInHierarchyChanged() override;

		// Updates layers layouts, calls after updating widget layout
		void UpdateLayersLayouts() override;

		// Updates bar, back and handle layers layout by value
		void UpdateProgressLayersLayouts();

		// Gets value from cursor position, depends on orientation
		void GetValueFromCursor(const Input::Cursor& cursor);

		// It is called when cursor pressed on this
		void OnCursorPressed(const Input::Cursor& cursor) override;

		// It is called when cursor released (only when cursor pressed this at previous time)
		void OnCursorReleased(const Input::Cursor& cursor) override;

		// It is called when cursor pressing was broken (when scrolled scroll area or some other)
		void OnCursorPressBreak(const Input::Cursor& cursor) override;

		// It is called when cursor moved on this (or moved outside when this was pressed)
		void OnCursorStillDown(const Input::Cursor& cursor) override;

		// It is called when cursor enters this object
		void OnCursorEnter(const Input::Cursor& cursor) override;

		// It is called when cursor exits this object
		void OnCursorExit(const Input::Cursor& cursor) override;

		// It is called when scrolling
		void OnScrolled(float scroll) override;
	};
}

PRE_ENUM_META(o2::VerticalProgress::Orientation);

CLASS_BASES_META(o2::VerticalProgress)
{
	BASE_CLASS(o2::Widget);
	BASE_CLASS(o2::DrawableCursorEventsListener);
}
END_META;
CLASS_FIELDS_META(o2::VerticalProgress)
{
	PUBLIC_FIELD(value);
	PUBLIC_FIELD(minValue);
	PUBLIC_FIELD(maxValue);
	PUBLIC_FIELD(scrollSense);
	PUBLIC_FIELD(onChange);
	PROTECTED_FIELD(mValue).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mSmoothValue);
	PROTECTED_FIELD(mMinValue).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mMaxValue).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mScrollSense).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mOrientation).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mBarLayer);
	PROTECTED_FIELD(mBackLayer);
}
END_META;
CLASS_METHODS_META(o2::VerticalProgress)
{

	PUBLIC_FUNCTION(void, Update, float);
	PUBLIC_FUNCTION(void, SetValue, float);
	PUBLIC_FUNCTION(void, SetValueForcible, float);
	PUBLIC_FUNCTION(float, GetValue);
	PUBLIC_FUNCTION(void, SetMinValue, float);
	PUBLIC_FUNCTION(float, GetMinValue);
	PUBLIC_FUNCTION(void, SetMaxValue, float);
	PUBLIC_FUNCTION(float, GetMaxValue);
	PUBLIC_FUNCTION(void, SetValueRange, float, float);
	PUBLIC_FUNCTION(void, SetScrollSense, float);
	PUBLIC_FUNCTION(float, GetScrollSense);
	PUBLIC_FUNCTION(void, SetOrientation, Orientation);
	PUBLIC_FUNCTION(Orientation, GetOrientation);
	PUBLIC_FUNCTION(bool, IsUnderPoint, const Vec2F&);
	PUBLIC_FUNCTION(bool, IsScrollable);
	PROTECTED_FUNCTION(void, CopyData, const Actor&);
	PROTECTED_FUNCTION(void, OnLayerAdded, WidgetLayer*);
	PROTECTED_FUNCTION(void, OnDeserialized, const DataNode&);
	PROTECTED_FUNCTION(void, OnEnableInHierarchyChanged);
	PROTECTED_FUNCTION(void, UpdateLayersLayouts);
	PROTECTED_FUNCTION(void, UpdateProgressLayersLayouts);
	PROTECTED_FUNCTION(void, GetValueFromCursor, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorPressed, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorReleased, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorPressBreak, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorStillDown, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorEnter, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorExit, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnScrolled, float);
}
END_META;