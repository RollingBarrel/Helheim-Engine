#pragma once

#include "Component.h"
#include "Particle.h"
#include "EmitterShape.h"
#include <map>

class Material;
struct Mesh;

class ENGINE_API ParticleSystemComponent : public Component
{
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
	float3 InitParticlePosition();

	float mEmitterTime = 0.0f;
	float mEmitterDeltaTime = 0.0f;

	float mDuration = 0.0f;
	float mLifeTime = 0.0f;
	float mSpeed = 0.0f;
	float mEmissionRate = 0.0f;
	int mMaxParticles = 0.0f;
	bool mLooping = true;

	EmitterShape mShapeType;
	std::map<float, float4> mColorGradient;
	std::vector<Particle> particles;
	unsigned int mVAO;
};
