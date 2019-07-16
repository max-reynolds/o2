#pragma once

#include "Utils/Editor/DragHandle.h"
#include "Events/CursorEventsListener.h"
#include "TrackControls/KeyHandles.h"

namespace o2
{
	class Animation;
	class Sprite;
}

using namespace o2;

namespace Editor
{
	class AnimationTimeline;
	class AnimationTree;
	class ITrackControl;

	// -------------------------------------------
	// Handles sheet, manages selection of handles
	// -------------------------------------------
	class KeyHandlesSheet : public Widget, public SelectableDragHandlesGroup, public CursorAreaEventsListener
	{
	public:
		// Default constructor
		KeyHandlesSheet();

		// Copy-constructor
		KeyHandlesSheet(const KeyHandlesSheet& other);

		// Destructor
		~KeyHandlesSheet();

		// Copy-operator
		KeyHandlesSheet& operator=(const KeyHandlesSheet& other);

		// Sets timeline and tree
		void Initialize(AnimationTimeline* timeline, AnimationTree* tree);

		// Sets animation. Used for batch change of keys
		void SetAnimation(Animation* animation);

		// Updates selection frame
		void Update(float dt) override;

		// Draws selection
		void Draw() override;

		// Updates draw order for correct handles and sheet input processing
		void UpdateInputDrawOrder();

		// Returns true if point is in this object
		bool IsUnderPoint(const Vec2F& point) override;

		// Registers animation value track control
		void RegTrackControl(ITrackControl* trackControl);

		// Unregisters animation value track control
		void UnregTrackControl(ITrackControl* trackControl);

		// Unregisters all tracks controls
		void UnregAllTrackControls();

		// Adds selectable handle to group
		void AddHandle(DragHandle* handle) override;

		// Removes selectable handle from group
		void RemoveHandle(DragHandle* handle) override;

		SERIALIZABLE(KeyHandlesSheet);

	private:
		RectF mSelectionFrameOffsets = RectF(-9, -3, 5, 2);
		RectF mSelectionFrameCursorOffsets = RectF(-2, -3, 2, 2);

		Animation* mAnimation = nullptr; // Editing animation

		AnimationTimeline* mTimeline = nullptr; // Timeline pointer, used for calculation world and local timeline positions
		AnimationTree*     mTree = nullptr;     // Animated values tree pointer, used for calculation handles lines numbers

		Vector<ITrackControl*> mTrackControls; // List of actual track controls

		Dictionary<IAnimatedValue*, Vector<AnimationKeyDragHandle*>> mHandlesGroups; // All handles grouped by animated value, used for fast searching handles for same animated value

		bool mNeedUpdateSelectionFrame = false; // True when selection frame required to update

		Sprite* mSelectionFrame = nullptr; // Selected handles frame drawing sprite
		RectF   mSelectionRect;            // Current selected handles rectangle. The right and left is minimum and maximum handles positions, top and bottom is minimum and maximum handles lines

		Vec2F   mBeginSelectPoint;         // Begin frame selection point, where x is position on timeline, y is line number
		bool    mIsFrameSelecting = false; // It is true when user selection by frame now

		SelectableDragHandlesVec mBeginSelectHandles; // handles list, that were selected before frame selecting

		DragHandle mLeftFrameDragHandle;   // Left frame border drag handle, resizing selected handles rect
		DragHandle mRightFrameDragHandle;  // Right frame border drag handle, resizing selected handles rect
		DragHandle mCenterFrameDragHandle; // Center frame drag handle, moves selected handles

	private:
		// Initializes frame handles
		void InitializeHandles();

		// Initializes center handle, that moves selected keys on timeline
		void InitializeCenterHandle();

		// Initializes left handle, that moves selected keys on timeline relative to right selection rect position
		void InitializeLeftHandle();

		// Initializes right handle, that moves selected keys on timeline relative to left selection rect position
		void InitializeRightHandle();

		// Updates selection rectangle and drawing sprite
		void UpdateSelectionFrame();

		// It is called when selection is changed - some handle was added or removed from selection
		// Updating selection frame
		void OnSelectionChanged() override;

		// It is called when selectable draggable handle was pressed, sends to track control that drag has began
		void OnHandleCursorPressed(DragHandle* handle, const Input::Cursor& cursor) override;

		// It is called when selectable draggable handle was released, sends to track control that drag has completed
		void OnHandleCursorReleased(DragHandle* handle, const Input::Cursor& cursor) override;

		// It is called when selectable handle moved, moves all selected handles position
		// Enables keys batch change
		void OnHandleMoved(DragHandle* handle, const Vec2F& cursorPos) override;

		// It is called when cursor pressed on this
		void OnCursorPressed(const Input::Cursor& cursor) override;

		// It is called when cursor released (only when cursor pressed this at previous time)
		void OnCursorReleased(const Input::Cursor& cursor) override;

		// It is called when cursor pressing was broken (when scrolled scroll area or some other)
		void OnCursorPressBreak(const Input::Cursor& cursor) override;

		// It is called when cursor pressed outside this
		void OnCursorPressedOutside(const Input::Cursor& cursor) override;

		// It is called when cursor released outside this(only when cursor pressed this at previous time)
		void OnCursorReleasedOutside(const Input::Cursor& cursor) override;

		// It is called when cursor stay down during frame
		void OnCursorStillDown(const Input::Cursor& cursor) override;

		// It is called when cursor moved on this (or moved outside when this was pressed)
		void OnCursorMoved(const Input::Cursor& cursor) override;

		// It is called when cursor enters this object
		void OnCursorEnter(const Input::Cursor& cursor) override;

		// It is called when cursor exits this object
		void OnCursorExit(const Input::Cursor& cursor) override;

		// It is called when cursor double clicked
		void OnCursorDblClicked(const Input::Cursor& cursor) override;

		// It is called when right mouse button was pressed on this
		void OnCursorRightMousePressed(const Input::Cursor& cursor) override;

		// It is called when right mouse button stay down on this
		void OnCursorRightMouseStayDown(const Input::Cursor& cursor) override;

		// It is called when right mouse button was released (only when right mouse button pressed this at previous time)
		void OnCursorRightMouseReleased(const Input::Cursor& cursor) override;

		// It is called when middle mouse button was pressed on this
		void OnCursorMiddleMousePressed(const Input::Cursor& cursor) override;

		// It is called when middle mouse button stay down on this
		void OnCursorMiddleMouseStayDown(const Input::Cursor& cursor) override;

		// It is called when middle mouse button was released (only when middle mouse button pressed this at previous time)
		void OnCursorMiddleMouseReleased(const Input::Cursor& cursor) override;
	};
}

CLASS_BASES_META(Editor::KeyHandlesSheet)
{
	BASE_CLASS(o2::Widget);
	BASE_CLASS(o2::SelectableDragHandlesGroup);
	BASE_CLASS(o2::CursorAreaEventsListener);
}
END_META;
CLASS_FIELDS_META(Editor::KeyHandlesSheet)
{
	PRIVATE_FIELD(mSelectionFrameOffsets);
	PRIVATE_FIELD(mSelectionFrameCursorOffsets);
	PRIVATE_FIELD(mAnimation);
	PRIVATE_FIELD(mTimeline);
	PRIVATE_FIELD(mTree);
	PRIVATE_FIELD(mTrackControls);
	PRIVATE_FIELD(mHandlesGroups);
	PRIVATE_FIELD(mNeedUpdateSelectionFrame);
	PRIVATE_FIELD(mSelectionFrame);
	PRIVATE_FIELD(mSelectionRect);
	PRIVATE_FIELD(mBeginSelectPoint);
	PRIVATE_FIELD(mIsFrameSelecting);
	PRIVATE_FIELD(mBeginSelectHandles);
	PRIVATE_FIELD(mLeftFrameDragHandle);
	PRIVATE_FIELD(mRightFrameDragHandle);
	PRIVATE_FIELD(mCenterFrameDragHandle);
}
END_META;
CLASS_METHODS_META(Editor::KeyHandlesSheet)
{

	PUBLIC_FUNCTION(void, Initialize, AnimationTimeline*, AnimationTree*);
	PUBLIC_FUNCTION(void, SetAnimation, Animation*);
	PUBLIC_FUNCTION(void, Update, float);
	PUBLIC_FUNCTION(void, Draw);
	PUBLIC_FUNCTION(void, UpdateInputDrawOrder);
	PUBLIC_FUNCTION(bool, IsUnderPoint, const Vec2F&);
	PUBLIC_FUNCTION(void, RegTrackControl, ITrackControl*);
	PUBLIC_FUNCTION(void, UnregTrackControl, ITrackControl*);
	PUBLIC_FUNCTION(void, UnregAllTrackControls);
	PUBLIC_FUNCTION(void, AddHandle, DragHandle*);
	PUBLIC_FUNCTION(void, RemoveHandle, DragHandle*);
	PRIVATE_FUNCTION(void, InitializeHandles);
	PRIVATE_FUNCTION(void, InitializeCenterHandle);
	PRIVATE_FUNCTION(void, InitializeLeftHandle);
	PRIVATE_FUNCTION(void, InitializeRightHandle);
	PRIVATE_FUNCTION(void, UpdateSelectionFrame);
	PRIVATE_FUNCTION(void, OnSelectionChanged);
	PRIVATE_FUNCTION(void, OnHandleCursorPressed, DragHandle*, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnHandleCursorReleased, DragHandle*, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnHandleMoved, DragHandle*, const Vec2F&);
	PRIVATE_FUNCTION(void, OnCursorPressed, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorReleased, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorPressBreak, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorPressedOutside, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorReleasedOutside, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorStillDown, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorMoved, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorEnter, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorExit, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorDblClicked, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorRightMousePressed, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorRightMouseStayDown, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorRightMouseReleased, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorMiddleMousePressed, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorMiddleMouseStayDown, const Input::Cursor&);
	PRIVATE_FUNCTION(void, OnCursorMiddleMouseReleased, const Input::Cursor&);
}
END_META;