#pragma once

#include "Component.h"
#include "Particle.h"
#include "EmitterShape.h"
#include <map>

class Material;
struct Mesh;

class ENGINE_API ParticleSystemComponent : public Component
{
	friend class InspectorPanel;
public:
	ParticleSystemComponent(GameObject* ownerGameObject);
	ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner);
	void Reset();

	void Init();
	void Update() override;
	void Draw() const;

	Component* Clone(GameObject* owner) const override;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;


private:

	float mEmitterTime = 0.0f;
	float mEmitterDeltaTime = 0.0f;

	float mDuration = 0.0f;
	float mLifeTime = 0.0f;

	bool mIsSpeedCurve;
	float mSpeedLineal = 0.0f;
	float4 mSpeedCurve;

	bool mIsSizeCurve;
	float mSizeLineal = 0.0f;
	float4 mSizeCurve;

	float mEmissionRate = 0.0f;
	int mMaxParticles = 1000.0f;
	bool mLooping = true;

	EmitterShape mShapeType;
	std::map<float, float4> mColorGradient;
	std::vector<Particle> particles;
	unsigned int mVAO;
};
