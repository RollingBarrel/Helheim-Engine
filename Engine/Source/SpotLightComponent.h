#ifndef _COMPONENT_SPOTLIGHT_H_
#define _COMPONENT_SPOTLIGHT_H_
#include "Component.h"

typedef struct SpotLight {
	float radius;
	float padding[3];
	float pos[4]; //w intensity
	float aimD[4];//w cos inner angle
	float col[4];//w cos outer angle
}SpotLight;

class SpotLightComponent : public Component {
public:
	~SpotLightComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override { return nullptr; }
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
	void Reset() override {}

	const float* GetPosition() const;
	void SetPosition(const float pos[3]);
	const float* GetDirection() const { return mData.aimD; };
	void SetDirection(const float dir[3]);
	const float* GetColor() const { return mData.col; }
	void SetColor(float col[3]);
	float GetIntensity() const { return mData.pos[3]; }
	void SetIntensity(float intensity);
	float GetRadius() const { return mData.radius; }
	void SetRadius(float radius);
	float GetOuterAngle() const; 
	void SetOuterAngle(float angle);
	float GetInnerAngle() const;
	void SetInnerAngle(float angle);
	//Todo: Variable not necesary for the game mode
	bool debugDraw = false;

private:
	SpotLight mData;
	SpotLightComponent(GameObject* owner, const SpotLight& light);
	friend class ModuleOpenGL;
};

#endif //_COMPONENT_SPOTLIGHT_H_