#include "o2/stdafx.h"
#include "AnimationAsset.h"

#include "o2/Assets/Assets.h"

namespace o2
{
	AnimationAsset::AnimationAsset()
	{}

	AnimationAsset::AnimationAsset(const AnimationAsset& other):
		AssetWithDefaultMeta<AnimationAsset>(other), animation(other.animation)
	{}

	AnimationAsset& AnimationAsset::operator=(const AnimationAsset& other)
	{
		Asset::operator=(other);
		animation = other.animation;

		return *this;
	}

	const char* AnimationAsset::GetFileExtensions() const
	{
		return "anim";
	}
}

DECLARE_CLASS_MANUAL(o2::AssetWithDefaultMeta<o2::AnimationAsset>);
DECLARE_CLASS_MANUAL(o2::DefaultAssetMeta<o2::AnimationAsset>);
