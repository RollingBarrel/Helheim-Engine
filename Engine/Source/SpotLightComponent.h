#ifndef _COMPONENT_SPOTLIGHT_H_
#define _COMPONENT_SPOTLIGHT_H_
#include "Component.h"
#include "Geometry/Frustum.h"

typedef struct SpotLight {

	float pos[4]; //w intensity
	float aimD[4];//w cos inner angle
	float color[4];//w cos outer angle
	//float4x4 viewProjMatrix;
	//uint64_t shadowMapHandle;
	float range;
	int shadowIndex;
	float padding[2];
	

}SpotLight;

class ENGINE_API SpotLightComponent : public Component 
{
public:
	SpotLightComponent(GameObject* owner);
	SpotLightComponent(const SpotLightComponent* original, GameObject* owner);
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
	const float* GetColor() const { return mData.color; }
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
	float GetBias() const { return mBias; }
	void SetBias(float bias);
	int GetIndex() const { return mData.shadowIndex; }
	void SetShadowIndex(int index);

	const Frustum& GetFrustum() const { return mShadowFrustum; }

	//unsigned int GetShadowMap() const { return mShadowMapId; }
	//unsigned int GetShadowMapSize() const { return mShadowMapSize; }
	void SetShadowMapSize(unsigned int shadowMapSize);

private:
	SpotLight mData;
	Frustum mShadowFrustum;
	//unsigned int mShadowMapSize = 0;
	//unsigned int mShadowMapId = 0;
	bool mCastShadow = false;
	float mBias;
};

#endif //_COMPONENT_SPOTLIGHT_H_