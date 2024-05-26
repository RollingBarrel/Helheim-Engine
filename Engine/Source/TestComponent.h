#pragma once
#include "Component.h"

class Mesh;

class ENGINE_API TestComponent : public Component
{
public:
	TestComponent(GameObject* ownerGameObject);
	TestComponent(const TestComponent& original, GameObject* owner);
	void Reset();
	//~TestComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void Save(JsonObject& obj) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	// Just for test
	int mTestSavedTag1 = 0;

private:
	int number = 0;
};

