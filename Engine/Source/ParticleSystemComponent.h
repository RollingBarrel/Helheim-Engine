#pragma once

#include "Component.h"
#include <map>
#include "EmitterShape.h"

class ResourceTexture;
class Particle;
//class EmitterShape;


class ENGINE_API ParticleSystemComponent : public Component
{
	friend class InspectorPanel;
public:
	ParticleSystemComponent(GameObject* ownerGameObject);
	ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner);
	~ParticleSystemComponent();
	ResourceTexture* GetImage() const { return mImage; }
	const char* GetFileName() const { return mFileName; }
	void Reset();

	void Init();
	void Update() override;
	void Draw() const;
	void Enable() override;
	void Disable() override;

	Component* Clone(GameObject* owner) const override;
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, GameObject* owner) override;

	void InitEmitterShape();

private:
	void SetImage(unsigned int resourceId);
	void SetFileName(const char* fileName) { mFileName = fileName; }

	ResourceTexture* mImage = nullptr;
	unsigned int mResourceId = 452546727; // Default particle texture
	const char* mFileName = nullptr;

	float mEmitterTime = 0.0f;
	float mEmitterDeltaTime = 0.0f;

	float mDuration = 5.0f;
	float mMaxLifeTime = 3.0f;

	bool mIsSpeedCurve;
	float mSpeedLineal = 1.0f;
	float4 mSpeedCurve;
	float mSpeedCurveFactor = 1.0f;

	bool mIsSizeCurve;
	float mSizeLineal = 1.0f;
	float4 mSizeCurve;
	float mSizeCurveFactor = 1.0f;

	float mEmissionRate = 1.0f;
	int mMaxParticles = 1000.0f;
	bool mLooping = true;

	EmitterShape* mShape;
	EmitterShape::Type mShapeType = EmitterShape::Type::CONE;

	std::map<float, float4> mColorGradient;
	std::vector<Particle*> mParticles;
	unsigned int mVAO = 0;
	unsigned int mInstanceBuffer = 0;
	unsigned int mVBO = 0;
};
