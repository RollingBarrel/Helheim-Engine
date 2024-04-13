#pragma once

#include "Component.h"
#include "ParticleEmitter.h"

class Material;
struct Mesh;

class ENGINE_API ParticleSystemComponent : public Component
{
public:
	ParticleSystemComponent(GameObject* ownerGameObject);
	ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner);
	void Reset();
	//~TestComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:
	std::vector<ParticleEmitter> mEmitters;	
};
