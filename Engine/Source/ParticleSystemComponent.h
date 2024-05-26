#pragma once

#include "Component.h"
#include "BezierCurve.h"
#include <map>
#include "EmitterShape.h"
#include "ColorGradient.h"

class ResourceTexture;
class Particle;
//class EmitterShape;

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
	void Enable() override;
	void Disable() override;

	Component* Clone(GameObject* owner) const override;
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data) override;

	void InitEmitterShape();

private:
	void SetImage(unsigned int resourceId);
	void SetFileName(const char* fileName) { mFileName = fileName; }
	template <int steps>
	static void BezierTable(float2 P[], float2 results[]);
	static float BezierValue(float dt01, float4 P);

	ResourceTexture* mImage = nullptr;
	unsigned int mResourceId = 452546727; // Default particle texture
	const char* mFileName = nullptr;

	float mEmitterTime = 0.0f;
	float mEmitterDeltaTime = 0.0f;

	float mDelay = 0.0f;
	float mDuration = 5.0f;
	float mMaxLifeTime = 3.0f;

	BezierCurve mSpeedCurve = BezierCurve();
	BezierCurve mSizeCurve = BezierCurve();
	bool mStretchedBillboard = false;

	float mEmissionRate = 10.0f;
	int mMaxParticles = 1000.0f;
	bool mLooping = true;

	EmitterShape* mShape;
	EmitterShape::Type mShapeType = EmitterShape::Type::CONE;

	ColorGradient* mColorGradient = new ColorGradient();
	std::vector<Particle*> mParticles;
	unsigned int mVAO = 0;
	unsigned int mInstanceBuffer = 0;
	unsigned int mVBO = 0;
};
