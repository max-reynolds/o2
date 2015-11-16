#pragma once

#include "Animation/AnimatedValue.h"
#include "Animation/IAnimation.h"
#include "Utils/Memory/Ptr.h"
#include "Utils/Property.h"
#include "Utils/Reflection/Attribute.h"
#include "Utils/String.h"

namespace o2
{
	class IObject;

	// -----------------------------------------------------------
	// Animation. Can animate anything object derived from IObject
	// -----------------------------------------------------------
	class Animation: public IAnimation
	{
	public:
		// Default constructor
		Animation();

		// Copy-constructor
		Animation(const Animation& other);

		// Destructor
		~Animation();

		// Assign operator
		Animation& operator=(const Animation& other);

		// Sets animation target
		// Bind all animation values to target's child fields (if it possible)
		void SetTarget(IObject* target);

		// Removes alnd clears all animated values
		void Clear();

		// Returns animated value by path (some like "path/abc/cde")
		template<typename _type>
		Ptr<AnimatedValue<_type>> GetAnimationValue(const String& path);

		// Returns animation value for target (animating target must be setted)
		template<typename _type>
		Ptr<AnimatedValue<_type>> GetAnimationValue(_type* target);

		// Adds animation value by target(animating target must be setted)
		template<typename _type>
		Ptr<AnimatedValue<_type>> AddAnimationValue(_type* target);


		// Adds animation value by target(animating target must be setted)
		template<typename _type>
		Ptr<AnimatedValue<_type>> AddAnimationValue(Setter<_type>* target);


		// Adds animation value by target(animating target must be setted)
		template<typename _type>
		Ptr<AnimatedValue<_type>> AddAnimationValue(Property<_type>* target);

		// Adds animation value with specified path
		template<typename _type>
		Ptr<AnimatedValue<_type>> AddAnimationValue(const String& path);

		// Removes animated value for target
		template<typename _type>
		bool RemoveAnimationValue(_type* target);

		// Removes animated value by path
		bool RemoveAnimationValue(const String& path);

		// Adds key in animated value by path and position
		template<typename _type>
		void AddKey(const String& targetPath, float position, typename const AnimatedValue<_type>::Key& key);

		// Adds key in animated value for target by position
		template<typename _type>
		void AddKey(_type* target, float position, typename const AnimatedValue<_type>::Key& key);

		// Sets keys in animated value by path
		template<typename _type>
		void SetKeys(const String& targetPath, typename const AnimatedValue<_type>::KeysVec& key);

		// Sets keys in animated value for target 
		template<typename _type>
		void SetKeys(_type* target, typename const AnimatedValue<_type>::KeysVec& key);

		// Returns animated value keys by path
		template<typename _type>
		typename AnimatedValue<_type>::KeysVec GetKeys(const String& path);

		// Returns animated value keys by target
		template<typename _type>
		typename AnimatedValue<_type>::KeysVec GetKeys(_type* target);

		// Removes key in animated value by path and position
		template<typename _type>
		bool Removekey(const String& targetPath, float position);

		// Removes key in animated value for target by position
		template<typename _type>
		bool Removekey(_type* target, float position);

		// Removes all keys in animated value for target
		template<typename _type>
		void RemoveAllKeys(_type* target);

		// Removes all keys in animated value by path
		template<typename _type>
		void RemoveAllKeys(const String& targetPath);

		SERIALIZABLE_IMPL(Animation);

		IOBJECT(Animation)
		{
			BASE_CLASS(IAnimation);
			SRLZ_FIELD(mAnimatedValues);
		}

	public:
		// -----------------------------------
		// Animated value definition structure
		// -----------------------------------
		struct AnimatedValueDef: public ISerializable
		{
			String              mTargetPath;    // Target path
			void*               mTargetPtr;     // Target pointer
			Ptr<IAnimatedValue> mAnimatedValue; // Animated value

			// Check equals operator
			bool operator==(const AnimatedValueDef& other) const;

			SERIALIZABLE_IMPL(AnimatedValueDef);

			IOBJECT(AnimatedValueDef)
			{
				SRLZ_FIELD(mTargetPath);
				SRLZ_FIELD(mAnimatedValue);
			}
		};
		typedef Vector<AnimatedValueDef> AnimatedValuesVec;

	protected:
		AnimatedValuesVec mAnimatedValues; // Animated value
		IObject*          mTarget;         // Target object

	protected:
		// Evaluates all animated values by time
		void Evaluate();

		// Returns animated value by path
		template<typename _type>
		Ptr<AnimatedValue<_type>> FindValue(const String& path);

		// Returns animated value by target
		template<typename _type>
		Ptr<AnimatedValue<_type>> FindValue(_type* target);

		// Recalculates maximum duration by animated values
		void RecalculateDuration();

		// Completion deserialization callback
		void OnDeserialized(const DataNode& node);
	};

	template<typename _type>
	Ptr<AnimatedValue<_type>> Animation::FindValue(_type* target)
	{
		for (auto val : mAnimatedValues)
			if (val.mTargetPtr == target)
				return val.mAnimatedValue.Cast<AnimatedValue<_type>>();

		return nullptr;
	}

	template<typename _type>
	Ptr<AnimatedValue<_type>> Animation::FindValue(const String& path)
	{
		for (auto val : mAnimatedValues)
			if (val.mTargetPath == path)
				return val.mAnimatedValue.Cast<AnimatedValue<_type>>();

		return nullptr;
	}

	template<typename _type>
	void Animation::RemoveAllKeys(_type* target)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(target);
		if (animVal)
			animVal->RemoveAllKeys();
	}

	template<typename _type>
	void Animation::RemoveAllKeys(const String& targetPath)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(targetPath);
		if (animVal)
			animVal->RemoveAllKeys();
	}

	template<typename _type>
	bool Animation::Removekey(_type* target, float position)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(target);
		if (animVal)
			return animVal->RemoveKey(position);

		return false;
	}

	template<typename _type>
	bool Animation::Removekey(const String& targetPath, float position)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(targetPath);
		if (animVal)
			return animVal->RemoveKey(position);

		return false;
	}

	template<typename _type>
	typename AnimatedValue<_type>::KeysVec Animation::GetKeys(_type* target)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(target);
		if (animVal)
			return animVal->GetKeys();

		return AnimatedValue<_type>::KeysVec();
	}

	template<typename _type>
	typename AnimatedValue<_type>::KeysVec Animation::GetKeys(const String& path)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(path);
		if (animVal)
			return animVal->GetKeys();

		return AnimatedValue<_type>::KeysVec();
	}

	template<typename _type>
	void Animation::SetKeys(_type* target, typename const AnimatedValue<_type>::KeysVec& key)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(target);
		if (!animVal)
			animVal = AddAnimationValue(target);

		animVal->SetKeys(key);
	}

	template<typename _type>
	void Animation::SetKeys(const String& targetPath, typename const AnimatedValue<_type>::KeysVec& key)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(targetPath);
		if (!animVal)
			animVal = AddAnimationValue(targetPath);

		animVal->SetKeys(key);
	}

	template<typename _type>
	void Animation::AddKey(_type* target, float position, typename const AnimatedValue<_type>::Key& key)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(target);
		if (!animVal)
			animVal = AddAnimationValue(target);

		animVal->AddKey(key, position);
	}

	template<typename _type>
	void Animation::AddKey(const String& targetPath, float position, typename const AnimatedValue<_type>::Key& key)
	{
		Ptr<AnimatedValue<_type>> animVal = FindValue<_type>(targetPath);
		if (!animVal)
			animVal = AddAnimationValue<_type>(targetPath);

		animVal->AddKey(key, position);
	}

	template<typename _type>
	bool Animation::RemoveAnimationValue(_type* target)
	{
		for (auto& val : mAnimatedValues)
		{
			if (val.mTargetPtr == target)
			{
				val.mAnimatedValue.Release();
				mAnimatedValues.Remove(val);
				return true;
			}
		}

		return false;
	}

	template<typename _type>
	Ptr<AnimatedValue<_type>> Animation::AddAnimationValue(_type* target)
	{
		if (mTarget)
		{
			String path;
			FieldInfo* fieldInfo = mTarget->GetType().FindFieldInfo(mTarget, target, path);

			if (!fieldInfo)
			{
				o2Debug.LogWarning("Can't create animated value: field info not found");
				return nullptr;
			}

			AnimatedValueDef def;

			def.mAnimatedValue = mnew AnimatedValue<_type>();
			def.mAnimatedValue->onKeysChanged += Function<void()>(this, &Animation::RecalculateDuration);

			if (fieldInfo->IsProperty())
				def.mAnimatedValue->SetTargetPropertyVoid(target);
			else
				def.mAnimatedValue->SetTargetVoid(target);

			def.mTargetPath = path;
			def.mTargetPtr = target;
			mAnimatedValues.Add(def);

			return def.mAnimatedValue.Cast<AnimatedValue<_type>>();
		}

		return nullptr;
	}

	template<typename _type>
	Ptr<AnimatedValue<_type>> Animation::AddAnimationValue(const String& path)
	{
		AnimatedValueDef def;
		def.mAnimatedValue = mnew AnimatedValue<_type>();
		def.mAnimatedValue->onKeysChanged += Function<void()>(this, &Animation::RecalculateDuration);

		if (mTarget)
		{
			FieldInfo* fieldInfo = nullptr;
			def.mTargetPtr = mTarget->GetType().GetFieldPtr<_type>(mTarget, path, fieldInfo);

			if (!fieldInfo)
			{
				o2Debug.LogWarning("Can't create animated value: field info not found");
				return nullptr;
			}

			if (fieldInfo->IsProperty())
				def.mAnimatedValue->SetTargetPropertyVoid(def.mTargetPtr);
			else
				def.mAnimatedValue->SetTargetVoid(def.mTargetPtr);
		}

		def.mTargetPath = path;
		mAnimatedValues.Add(def);

		return def.mAnimatedValue.Cast<AnimatedValue<_type>>();
	}

	template<typename _type>
	Ptr<AnimatedValue<_type>> Animation::AddAnimationValue(Setter<_type>* target)
	{
		if (mTarget)
		{
			String path;
			FieldInfo* fieldInfo = mTarget->GetType().FindFieldInfo(mTarget, target, path);

			if (!fieldInfo)
			{
				o2Debug.LogWarning("Can't create animated value: field info not found");
				return nullptr;
			}

			AnimatedValueDef def;

			def.mAnimatedValue = mnew AnimatedValue<_type>();
			def.mAnimatedValue->onKeysChanged += Function<void()>(this, &Animation::RecalculateDuration);

			if (fieldInfo->IsProperty())
				def.mAnimatedValue->SetTargetPropertyVoid(target);
			else
				def.mAnimatedValue->SetTargetVoid(target);

			def.mTargetPath = path;
			def.mTargetPtr = target;
			mAnimatedValues.Add(def);

			return def.mAnimatedValue.Cast<AnimatedValue<_type>>();
		}

		return nullptr;
	}

	template<typename _type>
	Ptr<AnimatedValue<_type>> Animation::AddAnimationValue(Property<_type>* target)
	{
		return AddAnimationValue(static_cast<Setter<_type>*>(target));
	}

	template<typename _type>
	Ptr<AnimatedValue<_type>> Animation::GetAnimationValue(_type* target)
	{
		for (auto& val : mAnimatedValues)
		{
			if (val.mTargetPtr == target)
				return val.mAnimatedValue;
		}

		return nullptr;
	}

	template<typename _type>
	Ptr<AnimatedValue<_type>> Animation::GetAnimationValue(const String& path)
	{
		for (auto& val : mAnimatedValues)
		{
			if (val.mTargetPath == path)
				return val.mAnimatedValue;
		}

		return nullptr;
	}

	template<typename _type>
	class AnimAttribute: public IAttribute
	{
	public:
		IAttribute* Clone() const { return new AnimAttribute(*this); }
	};

#define ANIMATABLE(TYPE) .AddAttribute<AnimAttribute<TYPE>>()
}