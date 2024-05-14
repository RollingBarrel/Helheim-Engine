#ifndef _COMPONENT_SPOTLIGHT_H_
#define _COMPONENT_SPOTLIGHT_H_
#include "Component.h"
#include "Geometry/Frustum.h"

typedef struct SpotLight {
	float range;
	float padding[3];
	float pos[4]; //w intensity
	float aimD[4];//w cos inner angle
	float col[4];//w cos outer angle
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
	const float* GetColor() const { return mData.col; }
	void SetColor(float col[3]);
	float GetIntensity() const { return mData.pos[3]; }
	void SetIntensity(float intensity);
	float GetRange() const { return mData.range; }
	void SetRange(float range);
	float GetOuterAngle() const; 
	void SetOuterAngle(float angle);
	float GetInnerAngle() const;
	void SetInnerAngle(float angle);
	const Frustum& GetFrustum() const { return mShadowFrustum; }
	//Todo: Variable not necesary for the game mode
	//bool debugDraw = false;

private:
	SpotLight mData;
	Frustum mShadowFrustum;
};

#endif //_COMPONENT_SPOTLIGHT_H_