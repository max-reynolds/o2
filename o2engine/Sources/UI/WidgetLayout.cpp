#include "WidgetLayout.h"

#include "UI/Widget.h"

namespace o2
{
	IOBJECT_CPP(UIWidgetLayout);

	UIWidgetLayout::UIWidgetLayout()
	{
		InitializeProperties();
	}

	UIWidgetLayout::UIWidgetLayout(const UIWidgetLayout& other):
		mPivot(other.mPivot), mAnchorMin(other.mAnchorMin), mAnchorMax(other.mAnchorMax),
		mOffsetMin(other.mOffsetMin), mOffsetMax(other.mOffsetMax)
	{
		InitializeProperties();
	}

	UIWidgetLayout& UIWidgetLayout::operator=(const UIWidgetLayout& other)
	{
		mPivot = other.mPivot;
		mAnchorMin = other.mAnchorMin;
		mAnchorMax = other.mAnchorMax;
		mOffsetMin = other.mOffsetMin;
		mOffsetMax = other.mOffsetMax;

		mOwner->UpdateLayout();

		return *this;
	}

	void UIWidgetLayout::SetPosition(const Vec2F& position)
	{
		Vec2F delta = position - GetPosition();
		mOffsetMin += delta;
		mOffsetMax += delta;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetPosition()
	{
		Vec2F parentPivot;

		if (mOwner->mParent)
			parentPivot = mOwner->mParent->layout.mLocalRect.Size()*mOwner->mParent->layout.mPivot;

		return mLocalRect.LeftBottom() + mLocalRect.Size()*mPivot - parentPivot;
	}

	void UIWidgetLayout::SetSize(const Vec2F& size)
	{
		Vec2F szDelta = size - GetSize();
		mOffsetMax += szDelta*(Vec2F::One() - mPivot);
		mOffsetMin -= szDelta*mPivot;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetSize() const
	{
		return mAbsoluteRect.Size();
	}

	void UIWidgetLayout::SetWidth(float width)
	{
		float delta = width - GetWidth();
		mOffsetMax.x += delta*(1.0f - mPivot.x);
		mOffsetMin.x -= delta*mPivot.x;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetWidth() const
	{
		return mAbsoluteRect.Width();
	}

	void UIWidgetLayout::SetHeight(float height)
	{
		float delta = height - GetHeight();
		mOffsetMax.y += delta*(1.0f - mPivot.y);
		mOffsetMin.y -= delta*mPivot.y;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetHeight() const
	{
		return mAbsoluteRect.Height();
	}

	void UIWidgetLayout::SetRect(const RectF& rect)
	{
		mOffsetMin += rect.LeftBottom() - mLocalRect.LeftBottom();
		mOffsetMax += rect.RightTop() - mLocalRect.RightTop();
		mOwner->UpdateLayout();
	}

	RectF UIWidgetLayout::GetRect() const
	{
		return mLocalRect;
	}

	void UIWidgetLayout::SetAbsoluteLeft(float value)
	{
		mOffsetMin.x += value - mAbsoluteRect.left;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetAbsoluteLeft() const
	{
		return mAbsoluteRect.left;
	}

	void UIWidgetLayout::SetAbsoluteRight(float value)
	{
		mOffsetMax.x += value - mAbsoluteRect.right;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetAbsoluteRight() const
	{
		return mAbsoluteRect.right;
	}

	void UIWidgetLayout::SetAbsoluteBottom(float value)
	{
		mOffsetMin.y += value - mAbsoluteRect.bottom;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetAbsoluteBottom() const
	{
		return mAbsoluteRect.bottom;
	}

	void UIWidgetLayout::SetAbsoluteTop(float value)
	{
		mOffsetMax.y += value - mAbsoluteRect.top;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetAbsoluteTop() const
	{
		return mAbsoluteRect.top;
	}

	void UIWidgetLayout::SetAbsolutePosition(const Vec2F& absPosition)
	{
		Vec2F delta = position - GetAbsolutePosition();
		mOffsetMin += delta;
		mOffsetMax += delta;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetAbsolutePosition() const
	{
		return mAbsoluteRect.LeftBottom() + mAbsoluteRect.Size()*mPivot;
	}

	void UIWidgetLayout::SetAbsoluteRect(const RectF& rect)
	{
		mOffsetMin += rect.LeftBottom() - mAbsoluteRect.LeftBottom();
		mOffsetMax += rect.RightTop() - mAbsoluteRect.RightTop();
		mOwner->UpdateLayout();
	}

	RectF UIWidgetLayout::GetAbsoluteRect() const
	{
		return mAbsoluteRect;
	}

	void UIWidgetLayout::SetPivot(const Vec2F& pivot)
	{
		mPivot = pivot;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetPivot() const
	{
		return mPivot;
	}

	void UIWidgetLayout::SetPivotX(float x)
	{
		mPivot.x = x;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetPivotX() const
	{
		return mPivot.x;
	}

	void UIWidgetLayout::SetPivotY(float y)
	{
		mPivot.y = y;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetPivotY() const
	{
		return mPivot.y;
	}

	void UIWidgetLayout::SetAnchorMin(const Vec2F& min)
	{
		mAnchorMin = min;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetAnchorMin() const
	{
		return mAnchorMin;
	}

	void UIWidgetLayout::SetAnchorMax(const Vec2F& max)
	{
		mAnchorMax = max;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetAnchorMax() const
	{
		return mAnchorMax;
	}

	void UIWidgetLayout::SetAnchorLeft(float value)
	{
		mAnchorMin.x = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetAnchorLeft() const
	{
		return mAnchorMin.x;
	}

	void UIWidgetLayout::SetAnchorRight(float value)
	{
		mAnchorMax.x = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetAnchorRight() const
	{
		return mAnchorMax.x;
	}

	void UIWidgetLayout::SetAnchorBottom(float value)
	{
		mAnchorMin.y = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetAnchorBottom() const
	{
		return mAnchorMin.y;
	}

	void UIWidgetLayout::SetAnchorTop(float value)
	{
		mAnchorMax.y = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetAnchorTop() const
	{
		return mAnchorMax.y;
	}

	void UIWidgetLayout::SetOffsetMin(const Vec2F& min)
	{
		mOffsetMin = min;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetOffsetMin() const
	{
		return mOffsetMin;
	}

	void UIWidgetLayout::SetOffsetMax(const Vec2F& max)
	{
		mOffsetMax = max;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetOffsetMax() const
	{
		return mOffsetMax;
	}

	void UIWidgetLayout::SetOffsetLeft(float value)
	{
		mOffsetMin.x = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetOffsetLeft() const
	{
		return mOffsetMin.x;
	}

	void UIWidgetLayout::SetOffsetRight(float value)
	{
		mOffsetMax.x = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetOffsetRight() const
	{
		return mOffsetMax.x;
	}

	void UIWidgetLayout::SetOffsetBottom(float value)
	{
		mOffsetMin.y = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetOffsetBottom() const
	{
		return mOffsetMin.y;
	}

	void UIWidgetLayout::SetOffsetTop(float value)
	{
		mOffsetMax.y = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetOffsetTop() const
	{
		return mOffsetMax.y;
	}

	void UIWidgetLayout::SetMinimalSize(const Vec2F& minSize)
	{
		mMinSize = minSize;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetMinimalSize() const
	{
		return mMinSize;
	}

	void UIWidgetLayout::SetMinimalWidth(float value)
	{
		mMinSize.x = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetMinimalWidth() const
	{
		return mMinSize.x;
	}

	void UIWidgetLayout::SetMinimalHeight(float value)
	{
		mMinSize.y = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetMinimalHeight() const
	{
		return mMinSize.y;
	}

	void UIWidgetLayout::SetMaximalSize(const Vec2F& maxSize)
	{
		mMaxSize = maxSize;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetMaximalSize() const
	{
		return mMaxSize;
	}

	void UIWidgetLayout::SetMaximalWidth(float value)
	{
		mMaxSize.x = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetMaximalWidth() const
	{
		return mMaxSize.x;
	}

	void UIWidgetLayout::SetMaximalHeight(float value)
	{
		mMaxSize.y = value;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetMaximalHeight() const
	{
		return mMaxSize.y;
	}

	void UIWidgetLayout::SetWeight(const Vec2F& weight)
	{
		mWeight = weight;
		mOwner->UpdateLayout();
	}

	Vec2F UIWidgetLayout::GetWeight() const
	{
		return mWeight;
	}

	void UIWidgetLayout::SetWidthWeight(float widthWeigth)
	{
		mWeight.x = widthWeigth;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetWidthWeight()
	{
		return mWeight.x;
	}

	void UIWidgetLayout::SetHeightWeight(float heigthWeigth)
	{
		mWeight.y = heigthWeigth;
		mOwner->UpdateLayout();
	}

	float UIWidgetLayout::GetHeightWeight()
	{
		return mWeight.y;
	}

	void UIWidgetLayout::InitializeProperties()
	{
		INITIALIZE_PROPERTY(UIWidgetLayout, pivot, SetPivot, GetPivot);
		INITIALIZE_PROPERTY(UIWidgetLayout, position, SetPosition, GetPosition);
		INITIALIZE_PROPERTY(UIWidgetLayout, size, SetSize, GetSize);
		INITIALIZE_PROPERTY(UIWidgetLayout, width, SetWidth, GetWidth);
		INITIALIZE_PROPERTY(UIWidgetLayout, height, SetHeight, GetHeight);
		INITIALIZE_PROPERTY(UIWidgetLayout, absPosition, SetAbsolutePosition, GetAbsolutePosition);
		INITIALIZE_PROPERTY(UIWidgetLayout, absLeft, SetAbsoluteLeft, GetAbsoluteLeft);
		INITIALIZE_PROPERTY(UIWidgetLayout, absRight, SetAbsoluteRight, GetAbsoluteRight);
		INITIALIZE_PROPERTY(UIWidgetLayout, absBottom, SetAbsoluteBottom, GetAbsoluteBottom);
		INITIALIZE_PROPERTY(UIWidgetLayout, absTop, SetAbsoluteTop, GetAbsoluteTop);
		INITIALIZE_PROPERTY(UIWidgetLayout, pivotX, SetPivotX, GetPivotX);
		INITIALIZE_PROPERTY(UIWidgetLayout, pivotY, SetPivotY, GetPivotY);
		INITIALIZE_PROPERTY(UIWidgetLayout, anchorMin, SetAnchorMin, GetAnchorMin);
		INITIALIZE_PROPERTY(UIWidgetLayout, anchorMax, SetAnchorMax, GetAnchorMax);
		INITIALIZE_PROPERTY(UIWidgetLayout, offsetMin, SetOffsetMin, GetOffsetMin);
		INITIALIZE_PROPERTY(UIWidgetLayout, offsetMax, SetOffsetMax, GetOffsetMax);
		INITIALIZE_PROPERTY(UIWidgetLayout, anchorLeft, SetAnchorLeft, GetAnchorLeft);
		INITIALIZE_PROPERTY(UIWidgetLayout, anchorRight, SetAnchorRight, GetAnchorRight);
		INITIALIZE_PROPERTY(UIWidgetLayout, anchorBottom, SetAnchorBottom, GetAnchorBottom);
		INITIALIZE_PROPERTY(UIWidgetLayout, anchorTop, SetAnchorTop, GetAnchorTop);
		INITIALIZE_PROPERTY(UIWidgetLayout, offsetLeft, SetOffsetLeft, GetOffsetLeft);
		INITIALIZE_PROPERTY(UIWidgetLayout, offsetRight, SetOffsetRight, GetOffsetRight);
		INITIALIZE_PROPERTY(UIWidgetLayout, offsetBottom, SetOffsetBottom, GetOffsetBottom);
		INITIALIZE_PROPERTY(UIWidgetLayout, offsetTop, SetOffsetTop, GetOffsetTop);
		INITIALIZE_PROPERTY(UIWidgetLayout, minSize, SetMinimalSize, GetMinimalSize);
		INITIALIZE_PROPERTY(UIWidgetLayout, minWidth, SetMinimalWidth, GetMinimalWidth);
		INITIALIZE_PROPERTY(UIWidgetLayout, minHeight, SetMinimalHeight, GetMinimalHeight);
		INITIALIZE_PROPERTY(UIWidgetLayout, maxSize, SetMaximalSize, GetMaximalSize);
		INITIALIZE_PROPERTY(UIWidgetLayout, maxWidth, SetMaximalWidth, GetMaximalWidth);
		INITIALIZE_PROPERTY(UIWidgetLayout, maxHeight, SetMaximalHeight, GetMaximalHeight);
		INITIALIZE_PROPERTY(UIWidgetLayout, weight, SetWeight, GetWeight);
		INITIALIZE_PROPERTY(UIWidgetLayout, widthWeight, SetWidthWeight, GetWidthWeight);
		INITIALIZE_PROPERTY(UIWidgetLayout, heigthWeight, SetHeightWeight, GetHeightWeight);
	}
}