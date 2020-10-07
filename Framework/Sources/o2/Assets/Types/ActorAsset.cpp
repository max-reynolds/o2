#include "o2/stdafx.h"
#include "ActorAsset.h"

#include "o2/Assets/Assets.h"
#include "o2/Scene/Actor.h"

namespace o2
{
	ActorAsset::ActorAsset():
		mActor(mnew Actor(ActorCreateMode::NotInScene))
	{
		mActor->mIsAsset = true;
		mActor->mAssetId = ID();
	}

	ActorAsset::ActorAsset(const ActorAsset& other):
		AssetWithDefaultMeta<ActorAsset>(other), mActor(other.mActor->CloneAs<Actor>())
	{}

	ActorAsset::~ActorAsset()
	{
		delete mActor;
	}

	ActorAsset& ActorAsset::operator=(const ActorAsset& other)
	{
		Asset::operator=(other);
		*mActor = *other.mActor;

		return *this;
	}

	ActorAsset::Meta* ActorAsset::GetMeta() const
	{
		return (Meta*)mInfo.meta;
	}

	const char* ActorAsset::GetFileExtensions()
	{
		return "proto";
	}

	Actor* ActorAsset::GetActor() const 
	{
		return mActor;
	}
}

template<>
DECLARE_CLASS_MANUAL(o2::AssetWithDefaultMeta<o2::ActorAsset>);
template<>
DECLARE_CLASS_MANUAL(o2::DefaultAssetMeta<o2::ActorAsset>);
template<>
DECLARE_CLASS_MANUAL(o2::Ref<o2::ActorAsset>);

DECLARE_CLASS(o2::ActorAsset);
