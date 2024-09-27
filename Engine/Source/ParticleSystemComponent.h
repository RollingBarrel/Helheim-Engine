#pragma once

#include "Component.h"
#include "BezierCurve.h"
#include "ColorGradient.h"
#include "BlendMode.h"
#include "MathConstants.h"
#include "RandomFloat.h"

class ResourceTexture;
class Particle;
class Trail;
//class EmitterShape;

class ENGINE_API ParticleSystemComponent : public Component
{
	friend class InspectorPanel;
public:
	enum class EmitterType
	{
		NONE = 0,
		CONE,
		BOX,
		SPHERE
	};
	explicit ParticleSystemComponent(GameObject* ownerGameObject);
	ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner);
	~ParticleSystemComponent();
	const ResourceTexture* GetImage() const { return mImage; }
	const char* GetFileName() const { return mImageName; }
	void Reset();

	void Init();
	void Update() override;
	void Draw();
	void Enable() override;
	void Disable() override;

	bool HasEnded() const;

	const void SetDuration(float duration) { mDuration = duration; }
	const void SetLoop(bool looping) { mLooping = looping; }
	const void SetEmmiterTime(float time) { mEmitterTime = time; }
	
	const float GetDuration() { return mDuration; }
	const bool IsLooping() { return mLooping; }
	void RestartParticles() { mEmitterTime = 0.0f; }

	float3 ShapeInitPosition() const;

	float3 ShapeInitDirection(const float3& pos) const;

	Component* Clone(GameObject* owner) const override;
	
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
	void SetImage(unsigned int resourceId);
	void SetFileName(const char* fileName) { mImageName = fileName; }
	void CreateNewParticle();

	// Recursos
	ResourceTexture* mImage = nullptr;
	const char* mImageName = nullptr;
	unsigned int mVAO = 0;
	unsigned int mInstanceBuffer = 0;
	unsigned int mVBO = 0;

	// Configuraci�n de Emisi�n
	float mEmitterTime = 0.0f;
	float mEmitterDeltaTime = 0.0f;
	float mEmissionRate = 10.0f;
	int mMaxParticles = 1000;
	int mBurst = 0;
	bool mIsInBurst = false;
	bool mLooping = true;
	bool mDisabling = false;

	// Atributos del Emisor
	bool mFollowEmitter = false;
	float mSpinSpeed = 0.0f;
	float mGravity = 0.0f;
	EmitterType mShapeType = EmitterType::CONE;
	float mShapeRadius = 0.01f;
	float mShapeAngle = math::pi / 4.0f;
	float3 mShapeSize = float3(1.0f, 1.0f, 1.0f);
	bool mIsShapeAngleRand = false;
	float mShapeRandAngle = 0.0f;
	bool mShapeInverseDir = false;

	// Control de Tiempo
	float mDelay = 0.0f;
	float mDuration = 5.0f;
	RandomFloat mLifetime;

	// Curvas y Modificadores de Part�culas
	BezierCurve mSpeedCurve = BezierCurve();
	BezierCurve mSizeCurve = BezierCurve();
	bool mStretchedBillboard = false;
	float mStretchedRatio = 0.0f;
	ColorGradient mColorGradient;

	// Part�culas
	std::vector<Particle*> mParticles; // Reemplazar con un contenedor m�s eficiente si es necesario.

	// Renderizado
	int mBlendMode = 0;

	// Trails
	bool mHasTrails = false;
	Trail* mTrail = nullptr;
};
