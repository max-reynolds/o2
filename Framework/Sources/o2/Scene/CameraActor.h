#pragma once
#include "o2/Scene/Actor.h"
#include "o2/Scene/SceneLayersList.h"

namespace o2
{
	// -------------------------------------------------------------------------------
	// Camera actor. Can works with different types, renders layers or itself children
	// -------------------------------------------------------------------------------
	class CameraActor: public Actor
	{
	public:
		enum class Type { Default, FreeSize, FixedSize, FittedSize, PhysicalCorrect };

	public:
		SceneLayersList drawLayers; // List of drawing layers @SERIALIZABLE

		bool   fillBackground = true;       // Is background filling with solid color @SERIALIZABLE
		Color4 fillColor = Color4::White(); // Background fill color @SERIALIZABLE

	public:
		// Default constructor
		CameraActor();

		// Copy constructor
		CameraActor(const CameraActor& other);

		// Destructor
		~CameraActor() override;

		// Copy operator
		CameraActor& operator=(const CameraActor& other);

		// Sets camera to render
		void Setup();

		// Sets camera to render and renders content (layers or children)
		void SetupAndDraw();

		// Returns rendering camera
		Camera GetRenderCamera() const;

		// Sets default camera
		void SetDefault();

		// Sets camera with fixed size
		void SetFixedSize(const Vec2F& size);

		// Sets camera with fixed aspect
		void SetFittedSize(const Vec2F& size);

		// Sets camera with physical correct units
		void SetPhysicalCorrect(Units units);

		// Returns camera type
		Type GetCameraType() const;

		// Returns fitter or fixed camera size
		const Vec2F& GetFittedOrFixedSize() const;

		// Returns current camera units
		Units GetUnits() const;

		SERIALIZABLE(CameraActor);

	protected:
		Type  mType = Type::Default;       // Type of camera @SERIALIZABLE
		Vec2F mFixedOrFittedSize;          // Fitted or fixed types size @SERIALIZABLE
		Units mUnits = Units::Centimeters; // Physical camera units @SERIALIZABLE

	protected:
		// Is is called when actor has added to scene
		void OnAddedToScene() override;

		// It is called when actor has removed from scene
		void OnRemovedFromScene() override;
	};
}

PRE_ENUM_META(o2::CameraActor::Type);

CLASS_BASES_META(o2::CameraActor)
{
	BASE_CLASS(o2::Actor);
}
END_META;
CLASS_FIELDS_META(o2::CameraActor)
{
	PUBLIC_FIELD(drawLayers).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(fillBackground).DEFAULT_VALUE(true).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(fillColor).DEFAULT_VALUE(Color4::White()).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mType).DEFAULT_VALUE(Type::Default).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mFixedOrFittedSize).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mUnits).DEFAULT_VALUE(Units::Centimeters).SERIALIZABLE_ATTRIBUTE();
}
END_META;
CLASS_METHODS_META(o2::CameraActor)
{

	PUBLIC_FUNCTION(void, Setup);
	PUBLIC_FUNCTION(void, SetupAndDraw);
	PUBLIC_FUNCTION(Camera, GetRenderCamera);
	PUBLIC_FUNCTION(void, SetDefault);
	PUBLIC_FUNCTION(void, SetFixedSize, const Vec2F&);
	PUBLIC_FUNCTION(void, SetFittedSize, const Vec2F&);
	PUBLIC_FUNCTION(void, SetPhysicalCorrect, Units);
	PUBLIC_FUNCTION(Type, GetCameraType);
	PUBLIC_FUNCTION(const Vec2F&, GetFittedOrFixedSize);
	PUBLIC_FUNCTION(Units, GetUnits);
	PROTECTED_FUNCTION(void, OnAddedToScene);
	PROTECTED_FUNCTION(void, OnRemovedFromScene);
}
END_META;