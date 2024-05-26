#ifndef _COMPONENT_POINTLIGHT_H_
#define _COMPONENT_POINTLIGHT_H_
#include "Component.h"

typedef struct PointLight {
	float pos[3];
	float radius;
	float col[3];
	float intensity;
	//PointLight(const PointLight& other)  = delete;
}PointLight;

class ENGINE_API PointLightComponent : public Component {
public:
	PointLightComponent(GameObject* owner, const PointLight& light);
	~PointLightComponent();

	void Enable() override;
	void Disable() override;
	void Update() override;

	Component* Clone(GameObject* owner) const override;
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, GameObject* owner) override;
	void Reset() override {}

	const PointLight& GetData() const { return mData; };
	const float* GetPosition() const;
	void SetPosition(const float pos[3]);
	const float* GetColor() const { return mData.col; }
	void SetColor(float col[3]);
	float GetIntensity() const { return mData.intensity; }
	void SetIntensity(float intensity);
	float GetRadius() const { return mData.radius; }
	void SetRadius(float radius);
	//Todo: Variable not necesary for the game mode
	//bool debugDraw = false;

private:
	PointLight mData;
};

#endif //_COMPONENT_POINTLIGHT_H_