#ifndef _COMPONENT_SPOTLIGHT_H_
#define _COMPONENT_SPOTLIGHT_H_
#include "Component.h"

typedef struct SpotLight {
	float radius;
	float padding[3];
	float pos[4]; //w intensity
	float aimD[4];//w cos inner angle
	float col[4];//w cos outer angle
	float range;
}SpotLight;

class ENGINE_API SpotLightComponent : public Component 
{
public:
	SpotLightComponent(GameObject* owner, const SpotLight& light);
	//SpotLightComponent(const SpotLightComponent& other, GameObject* owner);
	~SpotLightComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
	void Reset() override {}
	void Enable() override;
	void Disable() override;

	const SpotLight& GetData() const { return mData; }
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
	float GetRange() { return mData.range; }
	void SetRange(float range);
	//Todo: Variable not necesary for the game mode
	//bool debugDraw = false;

private:
	SpotLight mData;
};

#endif //_COMPONENT_SPOTLIGHT_H_