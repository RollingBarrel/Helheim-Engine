#ifndef _COMPONENT_POINTLIGHT_H_
#define _COMPONENT_POINTLIGHT_H_
#include "Component.h"

typedef struct PointLight {
	float pos[4]; //w is radius
	float col[4]; //w is Intensity
	//PointLight(const PointLight& other)  = delete;
}PointLight;

class PointLightComponent : public Component {
public:
	~PointLightComponent();

	void Enable() override;
	void Disable() override;
	void Update() override;

	Component* Clone(GameObject* owner) const override;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
	void Reset() override {}

	const float* GetPosition() const;
	void SetPosition(const float pos[3]);
	const float* GetColor() const { return mData.col; }
	void SetColor(float col[3]);
	float GetIntensity() const { return mData.col[3]; }
	void SetIntensity(float intensity);
	float GetRadius() const { return mData.pos[3]; }
	void SetRadius(float radius);
	//Todo: Variable not necesary for the game mode
	bool debugDraw = false;

private:
	PointLight mData;
	PointLightComponent(GameObject* owner, const PointLight& light);
	friend class ModuleOpenGL;
};

#endif //_COMPONENT_POINTLIGHT_H_