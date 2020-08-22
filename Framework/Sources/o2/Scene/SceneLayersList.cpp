#include "o2/stdafx.h"
#include "SceneLayersList.h"
#include "Scene.h"

namespace o2
{
	SceneLayersList::SceneLayersList()
	{}

	SceneLayersList::SceneLayersList(const Vector<SceneLayer*>& layers)
	{
		mAllLayers = o2Scene.GetLayers() == layers;
		mLayers = layers.Convert<String>([](SceneLayer* x) { return x->GetName(); });
	}

	SceneLayersList::SceneLayersList(const Vector<String>& layers):
		mLayers(layers), mAllLayers(false)
	{
		mAllLayers = mLayers == o2Scene.GetLayersNames();
	}

	SceneLayersList::SceneLayersList(const SceneLayersList& other):
		mLayers(other.mLayers), mAllLayers(other.mAllLayers)
	{}

	bool SceneLayersList::operator==(const SceneLayersList& other) const
	{
		if (mAllLayers && other.mAllLayers)
			return true;

		return GetLayers() == other.GetLayers();
	}

	bool SceneLayersList::operator!=(const SceneLayersList& other) const
	{
		return !(*this == other);
	}

	SceneLayersList& SceneLayersList::operator=(const SceneLayersList& other)
	{
		mLayers = other.mLayers;
		mAllLayers = other.mAllLayers;

		return *this;
	}

	bool SceneLayersList::HasLayer(SceneLayer* layer) const
	{
		return mAllLayers || mLayers.Contains(layer->GetName());
	}

	bool SceneLayersList::HasLayer(const String& layerName) const
	{
		return mAllLayers || mLayers.Contains(layerName);
	}

	void SceneLayersList::AddLayer(SceneLayer* layer)
	{
		mLayers.Add(layer->GetName());
		mAllLayers = false;
	}

	void SceneLayersList::AddLayer(const String& layerName)
	{
		mLayers.Add(layerName);
		mAllLayers = false;
	}

	void SceneLayersList::RemoveLayer(SceneLayer* layer)
	{
		mLayers.Remove(layer->GetName());
		mAllLayers = false;
	}

	void SceneLayersList::RemoveLayer(const String& layerName)
	{
		mLayers.Remove(layerName);
		mAllLayers = false;
	}

	void SceneLayersList::SetLayers(const Vector<SceneLayer*>& layers)
	{
		mLayers = layers.Convert<String>([](SceneLayer* x) { return x->GetName(); });
		mAllLayers = false;
	}

	void SceneLayersList::SetLayers(const Vector<String>& layerNames)
	{
		mLayers = layerNames;
		mAllLayers = false;
	}

	void SceneLayersList::SetAllLayers()
	{
		mAllLayers = true;
	}

	Vector<SceneLayer*> SceneLayersList::GetLayers() const
	{
		if (mAllLayers)
			return o2Scene.GetLayers();

		return mLayers.Convert<SceneLayer*>([](const String& x) { return o2Scene.GetLayer(x); });
	}

	const Vector<String>& SceneLayersList::GetLayersNames() const
	{
		if (mAllLayers)
			mLayers = o2Scene.GetLayersNames();

		return mLayers;
	}
}

DECLARE_CLASS(o2::SceneLayersList);
