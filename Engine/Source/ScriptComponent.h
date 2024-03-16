#pragma once
#include "Component.h"

class Script;

struct ScriptVariable;

class ENGINE_API ScriptComponent : public Component
{
	friend class InspectorPanel;
public:

	ScriptComponent(GameObject* owner);
	~ScriptComponent();
	void Update() override;
	Component* Clone(GameObject* owner) const override;	
	void Reset() override;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
	void LoadScript(const char* scriptName);
	const char* GetScriptName() const { return mName.c_str(); }

	void Enable() override;
	void Disable() override;
	

private:
	Script* mScript = nullptr;
	std::string mName = "";

	float mPlayerSpeed = 1;
	float mCoolDown = 2;
	float mHeight = 1;
	float mObjectSpeed = 1;

	std::vector<ScriptVariable*> mData;
};

