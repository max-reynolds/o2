#include "stdafx.h"

#include "Core/EditorScope.h"
#include "Scene/UI/WidgetLayout.h"
#include "TrackControls/KeyFramesTrackControl.h"
#include "TrackControls/MapKeyFramesTrackControl.h"
#include "Tree.h"

namespace Editor
{
	AnimationTree::AnimationTree() :
		Tree()
	{
		mZebraBackLine = mnew Sprite();
	}

	AnimationTree::AnimationTree(const AnimationTree& other) :
		Tree(other), mZebraBackLine(other.mZebraBackLine->CloneAs<Sprite>())
	{}

	AnimationTree::~AnimationTree()
	{
		if (mRootValue)
			delete mRootValue;

		delete mZebraBackLine;
	}

	AnimationTree& AnimationTree::operator=(const AnimationTree& other)
	{
		delete mZebraBackLine;

		Tree::operator=(other);

		mZebraBackLine = other.mZebraBackLine->CloneAs<Sprite>();

		return *this;
	}

	void AnimationTree::Draw()
	{
		DrawZebraBack();
		Tree::Draw();
	}

	void AnimationTree::SetAnimation(Animation* animation, AnimationTimeline* timeline)
	{
		mTimeline = timeline;

		mAnimation = nullptr;
		RebuildAnimationTree();

		mAnimation = animation;
		RebuildAnimationTree();

		ExpandAll();
		SetRearrangeType(Tree::RearrangeType::Disabled);
	}

	void AnimationTree::SetTreeWidth(float width)
	{
		mTreeWidth = width;
		UpdateTreeWidth();
	}

	Sprite* AnimationTree::GetZebraBackLine() const
	{
		return mZebraBackLine;
	}

	void AnimationTree::RebuildAnimationTree()
	{
		if (mRootValue)
			delete mRootValue;

		mRootValue = nullptr;

		if (!mAnimation)
			return;

		mRootValue = mnew AnimationValueNode();
		mRootValue->name = "Track name";

		for (auto& value : mAnimation->GetAnimationsValues())
			AddAnimatedValue(value);
	}

	void AnimationTree::AddAnimatedValue(Animation::AnimatedValueDef& value)
	{
		AnimationValueNode* current = nullptr;

		int lastDel = 0;
		while (lastDel >= 0)
		{
			int del = value.mTargetPath.Find('/', lastDel);
			String subPath = value.mTargetPath.SubStr(lastDel, del);
			AnimationValueNode* next = (current ? current->children : mRootValue->children)
				.FindMatch([&](AnimationValueNode* x) { return x->name == subPath; });

			if (!next)
			{
				next = mnew AnimationValueNode();
				next->name = subPath;

				if (current)
				{
					next->parent = current;
					current->children.Add(next);
				}
				else mRootValue->children.Add(next);
			}

			current = next;
			lastDel = del >= 0 ? del + 1 : -1;
		}

		current->animatedValue = value.mAnimatedValue;
	}

#undef DrawText
	void AnimationTree::DrawZebraBack()
	{
		o2Render.EnableScissorTest(mAbsoluteClipArea);

		float pos = -mScrollPos.y;
		float lineSize = mNodeWidgetSample->layout->GetMinimalHeight();
		while (pos < -lineSize)
			pos += lineSize*2.0f;

		RectF layoutRect = layout->GetWorldRect();
		while (pos < layoutRect.Height())
		{
			mZebraBackLine->SetRect(RectF(layoutRect.left, layoutRect.top - pos, layoutRect.right, layoutRect.top - pos - lineSize));
			mZebraBackLine->Draw();
			pos += lineSize*2.0f;
		}

		o2Render.DisableScissorTest();
	}

	void AnimationTree::UpdateTreeWidth()
	{
		for (auto node : mVisibleNodes)
		{
			if (auto trackNode = dynamic_cast<AnimationTreeNode*>(node->widget))
				trackNode->SetTreeWidth(mTreeWidth);
		}
	}

	UnknownPtr AnimationTree::GetObjectParent(UnknownPtr object)
	{
		if (!object)
			return UnknownPtr((AnimationValueNode*)nullptr);

		auto treeNode = (AnimationValueNode*)object;
		return treeNode->parent;
	}

	Vector<UnknownPtr> AnimationTree::GetObjectChilds(UnknownPtr object)
	{
		if (!object)
			return { (UnknownPtr)mRootValue };

		auto treeNode = (AnimationValueNode*)object;
		return treeNode->children.Cast<UnknownPtr>();
	}

	String AnimationTree::GetObjectDebug(UnknownPtr object)
	{
		auto treeNode = (AnimationValueNode*)object;
		return treeNode ? treeNode->name : "Empty";
	}

	void AnimationTree::FillNodeDataByObject(TreeNode* nodeWidget, UnknownPtr object)
	{
		AnimationTreeNode* node = dynamic_cast<AnimationTreeNode*>(nodeWidget);
		node->Setup((AnimationValueNode*)object, mTimeline);
	}

	void AnimationTree::UpdateVisibleNodes()
	{
		Tree::UpdateVisibleNodes();
		UpdateTreeWidth();
	}

	TreeNode* AnimationTree::CreateTreeNodeWidget()
	{
		PushScopeEnterOnStack scope;
		return Tree::CreateTreeNodeWidget();
	}

	AnimationTreeNode::AnimationTreeNode() :
		TreeNode()
	{}

	AnimationTreeNode::AnimationTreeNode(const AnimationTreeNode& other) :
		TreeNode(other)
	{
		InitializeControls();
	}

	AnimationTreeNode& AnimationTreeNode::operator=(const AnimationTreeNode& other)
	{
		TreeNode::operator=(other);
		InitializeControls();

		return *this;
	}

	void AnimationTreeNode::Setup(AnimationTree::AnimationValueNode* node, AnimationTimeline* timeline)
	{
		mTimeline = timeline;

		mTimeline->onViewChanged -= THIS_FUNC(UpdateTrackControlView);
		mTimeline->onViewChanged += THIS_FUNC(UpdateTrackControlView);

		mData = node;
		mNameDrawable->text = node->name;

		InitilizeTrackControl();
	}

	void AnimationTreeNode::SetTreeWidth(float width)
	{
		if (mTrackControl)
			*mTrackControl->layout = WidgetLayout::BothStretch(width, 0, 0, 0);
	}

	void AnimationTreeNode::CopyData(const Actor& otherActor)
	{
		TreeNode::CopyData(otherActor);
		InitializeControls();
	}

	void AnimationTreeNode::OnDeserialized(const DataNode& node)
	{
		TreeNode::OnDeserialized(node);
		InitializeControls();
	}

	void AnimationTreeNode::InitializeControls()
	{
		mNameDrawable = GetLayerDrawable<Text>("name");
	}

	void AnimationTreeNode::InitilizeTrackControl()
	{
		PushScopeEnterOnStack scope;

		static Dictionary<const Type*, const Type*> animatedValueToControlTrackTypes = 
		{
			{ &TypeOf(AnimatedValue<float>), &TypeOf(KeyFramesTrackControl<AnimatedValue<float>>) },
			{ &TypeOf(AnimatedValue<bool>), &TypeOf(KeyFramesTrackControl<AnimatedValue<bool>>) },
			{ &TypeOf(AnimatedValue<Vec2F>), &TypeOf(KeyFramesTrackControl<AnimatedValue<Vec2F>>) },
			{ &TypeOf(AnimatedValue<Color4>), &TypeOf(KeyFramesTrackControl<AnimatedValue<Color4>>) }
		};

		static Dictionary<const Type*, Vector<ITrackControl*>> trackControlsCache;

		if (mTrackControl)
		{
			auto trackType = &mTrackControl->GetType();
			if (!trackControlsCache.ContainsKey(trackType))
				trackControlsCache.Add(trackType, Vector<ITrackControl*>());

			trackControlsCache[trackType].Add(mTrackControl);

			RemoveChild(mTrackControl, false);
		}

		mTrackControl = nullptr;

		if (!mData->animatedValue)
		{
			MapKeyFramesTrackControl* trackControl = nullptr;
			auto trackControlType = &TypeOf(MapKeyFramesTrackControl);
			if (trackControlsCache.ContainsKey(trackControlType) && !trackControlsCache[trackControlType].IsEmpty())
				trackControl = dynamic_cast<MapKeyFramesTrackControl*>(trackControlsCache[trackControlType].PopBack());
			else
				trackControl = mnew MapKeyFramesTrackControl();

			mTrackControl = trackControl;

			trackControl->SetTimeline(mTimeline);
			trackControl->SetMappedTracks(*mData);

			AddChild(mTrackControl);
		}
		else
		{
			auto animatedValueType = &mData->animatedValue->GetType();
			if (!animatedValueToControlTrackTypes.ContainsKey(animatedValueType))
			{
				o2Debug.LogWarning("Can't create control track for type:" + animatedValueType->GetName());
				return;
			}

			auto trackControlType = dynamic_cast<const ObjectType*>(animatedValueToControlTrackTypes[animatedValueType]);
			if (trackControlsCache.ContainsKey(trackControlType) && !trackControlsCache[trackControlType].IsEmpty())
				mTrackControl = trackControlsCache[trackControlType].PopBack();
			else
				mTrackControl = dynamic_cast<ITrackControl*>(trackControlType->DynamicCastToIObject(trackControlType->CreateSample()));

			mTrackControl->SetTimeline(mTimeline);
			mTrackControl->SetAnimatedValue(mData->animatedValue);

			AddChild(mTrackControl);
		}
	}

	void AnimationTreeNode::UpdateTrackControlView()
	{
		if (mTrackControl)
			mTrackControl->UpdateHandles();
	}

}

DECLARE_CLASS(Editor::AnimationTree);

DECLARE_CLASS(Editor::AnimationTreeNode);
