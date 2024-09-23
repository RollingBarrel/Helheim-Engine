#include "Particle.h"
#include "Trail.h"
#include "GameObject.h"

Particle::Particle()
{
}

Particle::Particle(
    float3 position, float3 direction, float4 color, 
    float rotation, float lifeTime, 
    float initialSize, float size, float initialSpeed, float speed, 
    bool hasTrail, const Trail* trail, bool followEmitter)
    : mPosition(position), 
    mDirection(direction),
    mRotation(rotation),
    mMaxLifeTime(lifeTime),
    mInitialSize(initialSize),
    mSize(size),
    mInitialSpeed(initialSpeed),
    mSpeed(speed),
    mColor(color),
    mHasTrail(hasTrail),
    mFollowEmitter(followEmitter)
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
    mPosition = mPosition + mDirection * mSpeed * DeltaTime + float3(0, 1, 0) * DeltaTime * mLifeTime * -9.81 * gravity;
    //if (mHasTrail) mTrail->UpdateTrailParticle(owner->GetLocalRotation().Transform(mPosition));
    if (mHasTrail and !mFollowEmitter) mTrail->UpdateTrailParticle(mPosition);
    else if (mHasTrail and mFollowEmitter) mTrail->UpdateTrailParticle((owner->GetWorldTransform() * float4(mPosition,1)).Float3Part());

    return dt01;
}

void Particle::DrawTrail()
{
    mTrail->Draw();
}