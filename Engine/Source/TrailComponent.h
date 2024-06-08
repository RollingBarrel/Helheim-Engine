#pragma once

#include "Component.h"
#include "ColorGradient.h"

class ResourceTexture;
class Trail; 

class ENGINE_API TrailComponent : public Component
{
	friend class InspectorPanel;
public:
	explicit TrailComponent(GameObject* ownerGameObject);
	TrailComponent(const TrailComponent& original, GameObject* owner);
	~TrailComponent() final;
	const char* GetFileName() const { return mFileName; }
	void Reset() override;

	void Init();
	void Update() override;
	void Draw();
	void Enable() override;
	void Disable() override;

	Component* Clone(GameObject* owner) const override;
	void Save(JsonObject& archive) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
	void SetImage(unsigned int resourceId);
	void SetFileName(const char* fileName) { mFileName = fileName; }

	unsigned int mResourceId = 452546727; // Default particle texture
	const char* mFileName = nullptr;

	float mMinDistance = 0.1f;
	int mMaxPoints = 1000.0f;

	Trail* mTrail = nullptr;
};
