#pragma once
#include "Component.h"
#include <vector>
#include "float3.h"

class ENGINE_API AIAgentComponent :public Component
{
public:
	AIAgentComponent(GameObject* ownerGameObject);
	AIAgentComponent(const AIAgentComponent& original, GameObject* owner);
	~AIAgentComponent();

	Component* Clone(GameObject* owner) const override;

	void SetNavigationPath(const float3& destination);

	void MoveAgent(float speed) const;

	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
	std::vector<float3> mNavPositions;
};

