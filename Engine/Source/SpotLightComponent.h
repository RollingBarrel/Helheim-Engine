#ifndef _COMPONENT_SPOTLIGHT_H_
#define _COMPONENT_SPOTLIGHT_H_
#include "Component.h"

typedef struct SpotLight {
	float pos[4]; //w intensity
	float aimD[4];//w inner angle //The radius is the lenght of the aimD
	float col[4];//w outer angle
}PointLight;

class SpotLightComponent : public Component {
public:
	~SpotLightComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override { return nullptr; }
	void Save(Archive& archive) const override {}
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override {}
	void Reset() override {}

	const float* GetPosition() const;
	void SetPosition(const float poa[3]);
	const float* GetColor() const { return mData.col; }
	void SetColor(float col[3]);
	float GetIntensity() const { return mData.col[3]; }
	void SetIntensity(float intensity);
	float GetRadius() const { return mRadius; }
	void SetRadius(float radius);
	//Todo: Variable not necesary for the game mode
	bool debugDraw = false;

private:
	SpotLight mData;
	float mRadius = 1.0f;
	SpotLightComponent(GameObject* owner, const SpotLight& light);
	friend class ModuleOpenGL;
};

#endif //_COMPONENT_SPOTLIGHT_H_