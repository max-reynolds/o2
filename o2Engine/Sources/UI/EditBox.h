#pragma once

#include "Events/CursorEventsListener.h"
#include "Events/DrawableCursorEventsListener.h"
#include "Events/KeyboardEventsListener.h"
#include "Render/Mesh.h"
#include "Render/Sprite.h"
#include "Render/Text.h"
#include "UI/ScrollArea.h"

namespace o2
{
	// --------------------
	// Text edit box widget
	// --------------------
	class UIEditBox: public UIScrollArea, public DrawableCursorEventsListener, public KeyboardEventsListener
	{
	public:
		PROPERTIES(UIEditBox);
		PROPERTY(WString, text, SetText, GetText);			      // Text property
		PROPERTY(int, caret, SetCaretPosition, GetCaretPosition); // Caret position property

		PROPERTY(int, selectionBegin, SetSelectionBegin, GetSelectionBegin);  // Selection begin index property
		PROPERTY(int, selectionEnd, SetSelectionEnd, GetSelectionEnd);	      // Selection end index property

		Function<void(const WString&)> onChanged;         // Text changed event
		Function<void(const WString&)> onChangeCompleted; // Text changing completed event

		// Default constructor
		UIEditBox();

		// Copy-constructor
		UIEditBox(const UIEditBox& other);

		// Destructor
		~UIEditBox();

		// Copy-operator
		UIEditBox& operator=(const UIEditBox& other);

		// Draws widget
		void Draw() override;

		// Updates widget
		void Update(float dt) override;

		// Sets text
		void SetText(const WString& text);

		// returns text
		WString GetText() const;

		// Sets caret position
		void SetCaretPosition(int caretPosition);

		// Returns caret position
		int GetCaretPosition();

		// Sets selection begin and end indexes
		void Select(int begin, int end);

		// Sets selection begin position
		void SetSelectionBegin(int position);

		// Returns selection begin position
		int GetSelectionBegin() const;

		// Sets selection end position
		void SetSelectionEnd(int position);

		// Returns selection end position
		int GetSelectionEnd() const;

		// Resets selection
		void Deselect();

		// Selects all text
		void SelectAll();

		// Returns text drawable
		Text* GetTextDrawable();

		// Returns caret drawable
		Sprite* GetCaretDrawable();

		// Sets selection color
		void SetSelectionColor(const Color4& color);

		// Returns selection color
		Color4 GetSelectionColor() const;

		// Sets filter characters for integer numbers
		void SetFilterInteger();

		// Sets filter characters for decimal numbers
		void SetFilterFloat();

		//Sets filter characters for names
		void SetFilterNames();

		// Sets available characters
		void SetAvailableSymbols(const WString& availableSymbols);

		// Returns available characters
		WString GetAvailableSymbols() const;

		// Sets max sizes of line and lines count
		void SetMaxSizes(int maxLineCharactersCount, int maxLinesCount);

		// Sets maximum characters in line
		void SetMaxLineCharactersCount(int count);

		// Returns maximum characters in line
		int GetMaxLineCharactersCount() const;

		// Sets maximum lines count
		void SetMaxLinesCount(int count);

		// Returns maximum lines count
		int GetMaxLinesCount() const;

		// Sets text multiline
		void SetMultiLine(bool multiline);

		// Returns is text multiline
		bool IsMultiLine() const;

		// Sets word wrapping
		void SetWordWrap(bool wordWrap);

		// returns is word wrapping
		bool IsWordWrap() const;

		// Sets caret blinking delay in seconds
		void SetCaretBlinkingDelay(float delay);

		// Returns caret blinking delay in seconds
		float GetCaretBlinkingDelay() const;

		// Returns is listener scrollable
		bool IsScrollable() const override;

		// Returns is this widget can be selected
		bool IsFocusable() const override;

		// Updates layout
		void UpdateTransform(bool withChildren = true) override;

		// Returns true if point is under drawable
		bool IsUnderPoint(const Vec2F& point) override;

		SERIALIZABLE(UIEditBox);

	protected:
		Color4  mSelectionColor = Color4(0.1f, 0.2f, 0.6f, 0.3f); // Text selection color @SERIALIZABLE

		WString mLastText;                 // Last text
		WString mText;                     // Current text @SERIALIZABLE
		WString mAvailableSymbols;         // Available symbols @SERIALIZABLE

		Text*   mTextDrawable = nullptr;   // Text drawable @SERIALIZABLE
		Mesh*   mSelectionMesh = nullptr;  // Selection mesh
		Sprite* mCaretDrawable = nullptr;  // Caret drawable @SERIALIZABLE

		float   mCaretBlinkDelay = 1.0f;   // Caret blinking delay @SERIALIZABLE
		float   mCaretBlinkTime = 0.0f;    // Caret blinking timer

		int     mSelectionBegin = 0;       // Selection begin index
		int     mSelectionEnd = 0;	       // Selection end index
		bool    mSelectingByWords = false; // Selection works by solid words
		int     mSelWordBegin = 0;         // Selection by words begin index
		int     mSelWordEnd = 0;           // Selection by words end index

		bool    mMultiLine = true;		   // True if text is multiline @SERIALIZABLE
		bool    mWordWrap = false;		   // True if text words wrapping @SERIALIZABLE
		int     mMaxLineChars = INT_MAX;   // Count of maximum characters in line @SERIALIZABLE
		int     mMaxLinesCount = INT_MAX;  // Count of maximum lines count @SERIALIZABLE

		bool    mJustSelected = false;     // Is edit box selected at current frame
		float   mLastClickTime = -1.0f;    // Time of last clicking
		Vec2F   mLastCursorPos;            // Last pressed cursor position

	protected:
		// Copies data of actor from other to this
		void CopyData(const Actor& otherActor) override;

		// Updates transparency for this and children widgets
		void UpdateTransparency() override;

		// It is called when visible was changed
		void OnResEnableInHierarchyChanged() override;

		// It is called when widget was selected
		void OnFocused() override;

		// It is called when widget was deselected
		void OnUnfocused() override;

		// Updates mouse control
		void UpdateControls(float dt) override;

		// Updates scroll parameters: clip area, scroll size
		void UpdateScrollParams() override;

		// It is called when cursor pressed on this
		void OnCursorPressed(const Input::Cursor& cursor) override;

		// It is called when cursor released (only when cursor pressed this at previous time)
		void OnCursorReleased(const Input::Cursor& cursor) override;

		// It is called when cursor pressing was broken (when scrolled scroll area or some other)
		void OnCursorPressBreak(const Input::Cursor& cursor) override;

		// It is called when cursor stay down during frame
		void OnCursorStillDown(const Input::Cursor& cursor) override;

		// It is called when cursor enters this object
		void OnCursorEnter(const Input::Cursor& cursor) override;

		// It is called when cursor exits this object
		void OnCursorExit(const Input::Cursor& cursor) override;

		// It is called when right mouse button was pressed on this
		void OnCursorRightMousePressed(const Input::Cursor& cursor) override;

		// It is called when right mouse button stay down on this
		void OnCursorRightMouseStayDown(const Input::Cursor& cursor) override;

		// It is called when right mouse button was released (only when right mouse button pressed this at previous time)
		void OnCursorRightMouseReleased(const Input::Cursor& cursor) override;

		// It is called when scrolling
		void OnScrolled(float scroll) override;

		// It is called when key was pressed
		void OnKeyPressed(const Input::Key& key) override;

		// It is called when key was released
		void OnKeyReleased(const Input::Key& key) override;

		// It is called when key stay down during frame
		void OnKeyStayDown(const Input::Key& key) override;

		// Returns text filtered by available characters set
		WString GetFilteredText(const WString& text);

		// Checks text for maximal characters in line and lines count
		void CheckCharactersAndLinesBounds();

		// Updates selection mesh and fixing selection bounds
		void UpdateSelectionAndCaret();

		// Returns coordinates of caret position by caret text position
		Vec2F GetTextCaretPosition(int position);

		//Returns of caret text position caret position by caret coordinates
		int GetTextCaretPosition(const Vec2F& point);

		// Updates cursor blinking
		void UpdateCaretBlinking(float dt);

		// Adds rectangle geometry to selection mesh
		void AddSelectionRect(const RectF& rect);

		// Checks scroll to cursor
		void CheckScrollingToCaret();

		// Jump selection over word
		void JumpSelection(bool forward, bool selecting);

		// Checks character typing by keyboard key
		void CheckCharacterTyping(KeyboardKey key);

		// Check for erasing commands
		void CheckCharactersErasing(KeyboardKey key);

		// Checks caret moving commands
		void CheckCaretMoving(KeyboardKey key);

		// Checks clipboard commands (copy cut paste)
		void CheckClipboard(KeyboardKey key);

		// Moves mSelectionEnd position and mSelectionBegin, when selecting
		void MoveCaret(int newPosition, bool selecting);
	};
}

CLASS_BASES_META(o2::UIEditBox)
{
	BASE_CLASS(o2::UIScrollArea);
	BASE_CLASS(o2::DrawableCursorEventsListener);
	BASE_CLASS(o2::KeyboardEventsListener);
}
END_META;
CLASS_FIELDS_META(o2::UIEditBox)
{
	PUBLIC_FIELD(text);
	PUBLIC_FIELD(caret);
	PUBLIC_FIELD(selectionBegin);
	PUBLIC_FIELD(selectionEnd);
	PUBLIC_FIELD(onChanged);
	PUBLIC_FIELD(onChangeCompleted);
	PROTECTED_FIELD(mSelectionColor).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mLastText);
	PROTECTED_FIELD(mText).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mAvailableSymbols).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mTextDrawable).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mSelectionMesh);
	PROTECTED_FIELD(mCaretDrawable).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mCaretBlinkDelay).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mCaretBlinkTime);
	PROTECTED_FIELD(mSelectionBegin);
	PROTECTED_FIELD(mSelectionEnd);
	PROTECTED_FIELD(mSelectingByWords);
	PROTECTED_FIELD(mSelWordBegin);
	PROTECTED_FIELD(mSelWordEnd);
	PROTECTED_FIELD(mMultiLine).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mWordWrap).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mMaxLineChars).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mMaxLinesCount).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mJustSelected);
	PROTECTED_FIELD(mLastClickTime);
	PROTECTED_FIELD(mLastCursorPos);
}
END_META;
CLASS_METHODS_META(o2::UIEditBox)
{

	PUBLIC_FUNCTION(void, Draw);
	PUBLIC_FUNCTION(void, Update, float);
	PUBLIC_FUNCTION(void, SetText, const WString&);
	PUBLIC_FUNCTION(WString, GetText);
	PUBLIC_FUNCTION(void, SetCaretPosition, int);
	PUBLIC_FUNCTION(int, GetCaretPosition);
	PUBLIC_FUNCTION(void, Select, int, int);
	PUBLIC_FUNCTION(void, SetSelectionBegin, int);
	PUBLIC_FUNCTION(int, GetSelectionBegin);
	PUBLIC_FUNCTION(void, SetSelectionEnd, int);
	PUBLIC_FUNCTION(int, GetSelectionEnd);
	PUBLIC_FUNCTION(void, Deselect);
	PUBLIC_FUNCTION(void, SelectAll);
	PUBLIC_FUNCTION(Text*, GetTextDrawable);
	PUBLIC_FUNCTION(Sprite*, GetCaretDrawable);
	PUBLIC_FUNCTION(void, SetSelectionColor, const Color4&);
	PUBLIC_FUNCTION(Color4, GetSelectionColor);
	PUBLIC_FUNCTION(void, SetFilterInteger);
	PUBLIC_FUNCTION(void, SetFilterFloat);
	PUBLIC_FUNCTION(void, SetFilterNames);
	PUBLIC_FUNCTION(void, SetAvailableSymbols, const WString&);
	PUBLIC_FUNCTION(WString, GetAvailableSymbols);
	PUBLIC_FUNCTION(void, SetMaxSizes, int, int);
	PUBLIC_FUNCTION(void, SetMaxLineCharactersCount, int);
	PUBLIC_FUNCTION(int, GetMaxLineCharactersCount);
	PUBLIC_FUNCTION(void, SetMaxLinesCount, int);
	PUBLIC_FUNCTION(int, GetMaxLinesCount);
	PUBLIC_FUNCTION(void, SetMultiLine, bool);
	PUBLIC_FUNCTION(bool, IsMultiLine);
	PUBLIC_FUNCTION(void, SetWordWrap, bool);
	PUBLIC_FUNCTION(bool, IsWordWrap);
	PUBLIC_FUNCTION(void, SetCaretBlinkingDelay, float);
	PUBLIC_FUNCTION(float, GetCaretBlinkingDelay);
	PUBLIC_FUNCTION(bool, IsScrollable);
	PUBLIC_FUNCTION(bool, IsFocusable);
	PUBLIC_FUNCTION(void, UpdateTransform, bool);
	PUBLIC_FUNCTION(bool, IsUnderPoint, const Vec2F&);
	PROTECTED_FUNCTION(void, CopyData, const Actor&);
	PROTECTED_FUNCTION(void, UpdateTransparency);
	PROTECTED_FUNCTION(void, OnResEnableInHierarchyChanged);
	PROTECTED_FUNCTION(void, OnFocused);
	PROTECTED_FUNCTION(void, OnUnfocused);
	PROTECTED_FUNCTION(void, UpdateControls, float);
	PROTECTED_FUNCTION(void, UpdateScrollParams);
	PROTECTED_FUNCTION(void, OnCursorPressed, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorReleased, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorPressBreak, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorStillDown, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorEnter, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorExit, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorRightMousePressed, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorRightMouseStayDown, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorRightMouseReleased, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnScrolled, float);
	PROTECTED_FUNCTION(void, OnKeyPressed, const Input::Key&);
	PROTECTED_FUNCTION(void, OnKeyReleased, const Input::Key&);
	PROTECTED_FUNCTION(void, OnKeyStayDown, const Input::Key&);
	PROTECTED_FUNCTION(WString, GetFilteredText, const WString&);
	PROTECTED_FUNCTION(void, CheckCharactersAndLinesBounds);
	PROTECTED_FUNCTION(void, UpdateSelectionAndCaret);
	PROTECTED_FUNCTION(Vec2F, GetTextCaretPosition, int);
	PROTECTED_FUNCTION(int, GetTextCaretPosition, const Vec2F&);
	PROTECTED_FUNCTION(void, UpdateCaretBlinking, float);
	PROTECTED_FUNCTION(void, AddSelectionRect, const RectF&);
	PROTECTED_FUNCTION(void, CheckScrollingToCaret);
	PROTECTED_FUNCTION(void, JumpSelection, bool, bool);
	PROTECTED_FUNCTION(void, CheckCharacterTyping, KeyboardKey);
	PROTECTED_FUNCTION(void, CheckCharactersErasing, KeyboardKey);
	PROTECTED_FUNCTION(void, CheckCaretMoving, KeyboardKey);
	PROTECTED_FUNCTION(void, CheckClipboard, KeyboardKey);
	PROTECTED_FUNCTION(void, MoveCaret, int, bool);
}
END_META;
