#pragma once

#include "Component.h"

class ResourceTexture;
class Trail;

class ENGINE_API LineComponent : public Component
{
	friend class InspectorPanel;
public:
	explicit LineComponent(GameObject* ownerGameObject);
	LineComponent(const LineComponent& original, GameObject* owner);
	~LineComponent() final;
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
	GameObject* SearchLineFinal(GameObject* owner);

	Trail* mTrail = nullptr;
	GameObject* mFinalPoint = nullptr;
};
