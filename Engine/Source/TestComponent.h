#pragma once
#include "Component.h"

class Material;
struct Mesh;

class TestComponent : public Component
{
public:
	TestComponent(GameObject* ownerGameObject);
	TestComponent(const TestComponent& original, GameObject* owner);
	void Reset();
	//~TestComponent();

	void Update() override;
	Component* Clone(GameObject* owner) override;

	void Save(Archive& archive) const override;
	TestComponent* LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:
	int number;
};

