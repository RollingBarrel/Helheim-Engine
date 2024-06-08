#pragma once
#include "Component.h"
#include <string>

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
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
	void Enable() override;
	void Disable() override;

	ResourceTexture* mDiffuseTexture = nullptr;
	ResourceTexture* mSpecularTexture = nullptr;
	ResourceTexture* mNormalTexture = nullptr;

	std::string mDiffuseName = "";
	std::string mSpecularName = "";
	std::string mNormalName = "";

};

