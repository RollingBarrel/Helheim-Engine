#pragma once

#include "Component.h"
#include "Archive.h"
#include "float3.h"
#include "float2.h"


class ResourceTexture;
class Trail;

class ENGINE_API TrailComponent : public Component
{
	friend class InspectorPanel;
public:
	struct BufferVertex
	{
		float3 mPosition;
		float2 mTexCoord;
		float mDistance;
		float3 mDirection;
	};
	struct TrailPoint
	{
		TrailPoint(float3 position, float3 direction, float time, float mDistanceUV);
		float3 mPosition;
		float3 mDirection;
		float mCreationTime;
		float mDistanceUV;
	};
	explicit TrailComponent(GameObject* ownerGameObject);
	TrailComponent(const TrailComponent& original, GameObject* owner);
	~TrailComponent() final;
	void Reset() override;

	Trail* GetTrail() const { return mTrail; }

	void Init();
	void Update() override;
	void Draw() const;
	void Enable() override;
	void Disable() override;

	void AddFirstTrailPoint();
	void RemoveLastTrailPoint();

	Component* Clone(GameObject* owner) const override;
	void Save(JsonObject& archive) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
    float3 CalculateDirection(const float3& position, const float3& norm) const;
	void SetImage(unsigned int resourceId);

	Trail* mTrail;
};
