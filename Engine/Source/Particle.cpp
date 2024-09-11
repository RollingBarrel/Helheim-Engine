#include "Particle.h"
#include "Trail.h"
#include "GameObject.h"

Particle::Particle()
{
}

Particle::Particle(float3 position, float3 direction, float4 color, float rotation, float lifeTime, bool hasTrail, const Trail* trail)
    : mPosition(position), 
    mDirection(direction),
    mRotation(rotation),
    mMaxLifeTime(lifeTime),
    mColor(color),
    mHasTrail(hasTrail)
{
    if (mHasTrail) mTrail = new Trail(*trail);
}

Particle::~Particle()
{
    delete mTrail;
}

float Particle::Update(float DeltaTime, float gravity, GameObject* owner)
{
    mLifeTime += DeltaTime;
    float dt01 = mLifeTime / mMaxLifeTime;
    mDirection = mDirection + float3(0, 1, 0) * DeltaTime * -9.81 * gravity;
    mPosition = mPosition + mDirection * mSpeed * DeltaTime;
    //if (mHasTrail) mTrail->UpdateTrailParticle(owner->GetWorldRotation().Transform(mPosition));
    if (mHasTrail) mTrail->UpdateTrailParticle(owner->GetWorldPosition() + owner->GetWorldRotation().Transform(mPosition));
    return dt01;
}

void Particle::DrawTrail()
{
    mTrail->Draw();
}