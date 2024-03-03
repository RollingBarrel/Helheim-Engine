#pragma once
#include "Component.h"

class Script;

class ENGINE_API ScriptComponent : public Component
{
public:

	ScriptComponent(GameObject* owner);
	//~ScriptComponent();
	void Update() override;
	Component* Clone(GameObject* owner) const override;	
	void Reset() override;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
	void LoadScript(const char* scriptName);

	//void Disable() override;
	//void Enable() override;

private:
	Script* mScript = nullptr;
	const char* mName = "";
	unsigned int ID = 0;
};

