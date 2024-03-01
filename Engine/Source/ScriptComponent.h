#pragma once
#include "Component.h"

class Script;

class ScriptComponent : public Component
{
public:

	ScriptComponent(GameObject* owner);
	//~ScriptComponent();
	void Update() override;
	Component* Clone(GameObject* owner) const override;	
	void Reset() override;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	//void Disable() override;
	//void Enable() override;

private:
	Script* mScript = nullptr;
	unsigned int ID = 0;
};

