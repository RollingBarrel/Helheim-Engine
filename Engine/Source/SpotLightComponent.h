#ifndef _COMPONENT_SPOTLIGHT_H_
#define _COMPONENT_SPOTLIGHT_H_
#include "Component.h"
#include "Geometry/Frustum.h"

typedef struct SpotLight {
	//float range;
	//float padding[3];
	//float pos[4]; //w intensity
	//float aimD[4];//w cos inner angle
	//float col[4];//w cos outer angle
	//float4x4 viewProjMatrix;
	//uint64_t shadowMapHandle;
	//float padding2[2];

	float pos[4]; //w intensity
	float aimD[4];//w cos inner angle
	float col[4];//w cos outer angle
	float4x4 viewProjMatrix;
	uint64_t shadowMapHandle;
	float range;
	float bias;

}SpotLight;

class ENGINE_API SpotLightComponent : public Component 
{
public:
	SpotLightComponent(GameObject* owner, const SpotLight& light);
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
	bool CanCastShadow() const { return mCastShadow; }
	void SetCastShadow(bool castShadow) { mCastShadow = castShadow; }
	float GetBias() { return mData.bias; }
	void SetBias(float bias);

	const Frustum& GetFrustum() const { return mShadowFrustum; }

	unsigned int GetShadowMap() const { return mShadowMapId; }
	unsigned int GetShadowMapSize() const { return mShadowMapSize; }
	void SetShadowMapSize(unsigned int shadowMapSize);

private:
	SpotLight mData;
	unsigned int mShadowMapSize = 0;
	Frustum mShadowFrustum;
	unsigned int mShadowMapId = 0;
	bool mCastShadow = false;
};

#endif //_COMPONENT_SPOTLIGHT_H_