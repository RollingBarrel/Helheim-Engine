#pragma once

#include "Component.h"
#include <map>
#include "EmitterShape.h"

class Material;
class ResourceTexture;
class Particle;
//class EmitterShape;
struct Mesh;

class ENGINE_API ParticleSystemComponent : public Component
{
	friend class InspectorPanel;
public:
	ParticleSystemComponent(GameObject* ownerGameObject);
	ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner);
	~ParticleSystemComponent();
	void Reset();

	void Init();
	void Update() override;
	void Draw() const;

	Component* Clone(GameObject* owner) const override;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;


private:
	void SetImage(unsigned int resourceId);

	ResourceTexture* mImage = nullptr;
	unsigned int mResourceId = 818189439; // Temporary default image 

	float mEmitterTime = 0.0f;
	float mEmitterDeltaTime = 0.0f;

	float mDuration = 5.0f;
	float mLifeTime = 10.0f;

	bool mIsSpeedCurve;
	float mSpeedLineal = 100.0f;
	float4 mSpeedCurve;

	bool mIsSizeCurve;
	float mSizeLineal = 1.0f;
	float4 mSizeCurve;

	float mEmissionRate = 1.0f;
	int mMaxParticles = 1000.0f;
	bool mLooping = true;

	EmitterShape mShape;
	EmitterShape::Type mShapeType = EmitterShape::Type::CONE;

	std::map<float, float4> mColorGradient;
	std::vector<Particle*> mParticles;
	unsigned int mVAO;
	unsigned int mInstanceBuffer;
	unsigned int mVBO;
};
