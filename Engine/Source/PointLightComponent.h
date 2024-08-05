#ifndef _COMPONENT_POINTLIGHT_H_
#define _COMPONENT_POINTLIGHT_H_
#include "Component.h"

typedef struct PointLight {
	float pos[3];
	float radius;
	float color[3];
	float intensity;
}PointLight;

class ENGINE_API PointLightComponent : public Component {
public:
	explicit PointLightComponent(GameObject* owner);

	PointLightComponent(const PointLightComponent& original) = delete;
	~PointLightComponent();

	PointLightComponent& operator=(const PointLightComponent& other) = delete;

	void Enable() override;
	void Disable() override;
	void Update() override;

	Component* Clone(GameObject* owner) const override;
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;
	void Reset() override;

	const PointLight& GetData() const { return mData; };
	const float* GetPosition() const;
	const float* GetColor() const { return mData.color; }
	void SetColor(float col[3]);
	float GetIntensity() const { return mData.intensity; }
	void SetIntensity(float intensity);
	float GetRadius() const { return mData.radius; }
	void SetRadius(float radius);

private:
	PointLightComponent(const PointLightComponent* original, GameObject* owner);
	PointLight mData;
};

#endif //_COMPONENT_POINTLIGHT_H_