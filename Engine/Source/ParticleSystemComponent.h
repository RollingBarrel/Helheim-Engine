#pragma once

#include "Component.h"
#include "Particle.h"
#include "EmitterShape.h"
#include "ColorGradient.h"

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

	float mEmitterTime;
	float mEmitterDeltaTime;

	float mDuration;
	float mLifeTime;
	float mSpeed;
	float mEmissionRate;
	int mMaxParticles;
	bool mLooping;

	EmitterShape mShapeType;
	ColorGradient mColorGradient;

	std::vector<Particle> particles;
};
