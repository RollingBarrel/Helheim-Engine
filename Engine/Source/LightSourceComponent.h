#ifndef _COMPONENT_LIGHTSOURCE_H_
#define _COMPONENT_LIGHTSOURCE_H_
#include "Component.h"

typedef struct PointLight {
	float pos[4]; //w is radius
	float col[4]; //w is Intensity
	//PointLight(const PointLight& other)  = delete;
}PointLight;

class LightSourceComponent : public Component {
public:
	LightSourceComponent(GameObject* owner, PointLight& light);
	~LightSourceComponent();

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
	float GetRadius() const { return mData.pos[3]; }
	void SetRadius(float radius);

private:
	PointLight& mData;
};

#endif //_COMPONENT_LIGHTSOURCE_H_