#pragma once

#include "Component.h"
#include "float3.h"
#include "float2.h"


class ResourceTexture;
class Trail;

class ENGINE_API TrailComponent : public Component
{
	friend class InspectorPanel;
public:
	explicit TrailComponent(GameObject* ownerGameObject);
	TrailComponent(const TrailComponent& original, GameObject* owner);
	~TrailComponent() final;
	void Reset() override;

	Trail* GetTrail() const { return mTrail; }

	void Update() override;
	void Draw() const;
	void Enable() override;
	void Disable() override;

	Component* Clone(GameObject* owner) const override;
	void Save(JsonObject& archive) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
	Trail* mTrail = nullptr;
};
