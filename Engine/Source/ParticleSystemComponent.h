#pragma once

#include "Component.h"
#include "BezierCurve.h"
#include "ColorGradient.h"
#include "BlendMode.h"
#include "MathConstants.h"
#include "RandomFloat.h"

class ResourceTexture;
class Particle;
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
	const char* GetFileName() const { return mFileName; }
	void Reset();

	void Init();
	void Update() override;
	void Draw() const;
	void Enable() override;
	void Disable() override;

	bool HasEnded() const;

	float3 ShapeInitPosition() const;

	float3 ShapeInitDirection(const float3& pos) const;

	Component* Clone(GameObject* owner) const override;
	
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
	void SetImage(unsigned int resourceId);
	void SetFileName(const char* fileName) { mFileName = fileName; }

	ResourceTexture* mImage = nullptr;
	unsigned int mResourceId = 148626881; // Default particle texture
	const char* mFileName = nullptr;

	float mEmitterTime = 0.0f;
	float mEmitterDeltaTime = 0.0f;

	float mDelay = 0.0f;
	float mDuration = 5.0f;

	RandomFloat mLifetime;

	BezierCurve mSpeedCurve = BezierCurve();
	BezierCurve mSizeCurve = BezierCurve();
	bool mStretchedBillboard = false;
	float mStretchedRatio = 0.0f;

	float mEmissionRate = 10.0f;
	int mMaxParticles = 1000;
	bool mLooping = true;

	//EmitterShape* mShape;
	//EmitterShape::Type mShapeType = EmitterShape::Type::CONE;

	EmitterType mShapeType = EmitterType::NONE;
	float mShapeRadius = 0.01f;
	float mShapeAngle = math::pi / 4.0f;
	float3 mShapeSize = float3(1.0f, 1.0f, 1.0f);
	bool mIsShapeAngleRand = false;
	float mShapeRandAngle = 0.0f;
	bool mShapeInverseDir = false;

	int mBlendMode = 0;

	ColorGradient mColorGradient;
	std::vector<Particle*> mParticles;
	unsigned int mVAO = 0;
	unsigned int mInstanceBuffer = 0;
	unsigned int mVBO = 0;
};
