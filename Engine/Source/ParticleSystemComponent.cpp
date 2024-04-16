#include "ParticleSystemComponent.h"
#include "Application.h"

ParticleSystemComponent::ParticleSystemComponent(GameObject* ownerGameObject)
{
}

ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner)
{

}

void ParticleSystemComponent::Update() 
{
	mEmitterTime += App->GetGameDt();
	mEmitterDeltaTime += App->GetGameDt();

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i].Update();
	}

	if (mEmitterDeltaTime > 1 / mEmissionRate)// deltaTime in seconds Use Timer
	{
		mEmitterDeltaTime = mEmitterDeltaTime - 1 / mEmissionRate;
		if (particles.size() < mMaxParticles) 
		{
			float3 position = InitParticlePosition();
			particles.push_back(Particle(mLifeTime, position, mSpeed));
		}
	}
}

float3 ParticleSystemComponent::InitParticlePosition()
{
	float3 position = mShapeType.RandomInitPosition();
	return position;
}
