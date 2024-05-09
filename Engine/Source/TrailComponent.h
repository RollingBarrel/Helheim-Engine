#pragma once

#include "Component.h"
#include "ColorGradient.h"

class Material;
class ResourceTexture;
class Particle;
struct Mesh;

class ENGINE_API ParticleSystemComponent : public Component
{
	friend class InspectorPanel;
public:
	ParticleSystemComponent(GameObject* ownerGameObject);
	ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner);
	~ParticleSystemComponent();
	ResourceTexture* GetImage() const { return mImage; }
	const char* GetFileName() const { return mFileName; }
	void Reset();

	void Init();
	void Update() override;
	void Draw() const;

	Component* Clone(GameObject* owner) const override;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
	void InitEmitterShape();

private:
	void SetImage(unsigned int resourceId);
	void SetFileName(const char* fileName) { mFileName = fileName; }

	ResourceTexture* mImage = nullptr;
	unsigned int mResourceId = 452546727; // Default particle texture
	const char* mFileName = nullptr;

	float mMaxLifeTime = 3.0f;

	bool mIsSizeCurve;
	float mSizeLineal = 3.0f;
	float4 mSizeCurve;
	float mSizeCurveFactor = 1.0f;

	int mMaxParticles = 1000.0f;

	ColorGradient* mColorGradient;
	std::vector<Particle*> mParticles;
	unsigned int mVAO = 0;
	unsigned int mInstanceBuffer = 0;
	unsigned int mVBO = 0;
};
