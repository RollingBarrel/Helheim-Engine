#pragma once
#include "Component.h"

class ResourceTexture;

class DecalComponent : public Component
{
	friend class InspectorPanel;
public:
	DecalComponent() = delete;
	explicit DecalComponent(GameObject* owner);
	DecalComponent(const DecalComponent& other, GameObject* owner);
	~DecalComponent();

	DecalComponent& operator=(const DecalComponent& other) = delete;

	void Reset() override {}
	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data) override;


	

private:
	ResourceTexture* mDiffuseTexture = nullptr;
	ResourceTexture* mSpecularTexture = nullptr;
	ResourceTexture* mNormalTexture = nullptr;

	const char* mDiffuseName;
	const char* mSpecularName;
	const char* mNormalName;

};

