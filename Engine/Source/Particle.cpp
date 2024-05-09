#include "Particle.h"

Particle::Particle()
{
}

Particle::Particle(float3 position, float3 direction, float4 color, float rotation, float lifeTime)
    : mPosition(position), 
    mDirection(direction),
    mRotation(rotation),
    mMaxLifeTime(lifeTime),
    mColor(color)
{
}

Particle::~Particle()
{
    //delete mColorGradient;
}

float Particle::Update(float DeltaTime)
{
    mLifeTime += DeltaTime;
    float dt01 = mLifeTime / mMaxLifeTime;
    mPosition = mPosition + mDirection * mSpeed * DeltaTime;
    return true;
}