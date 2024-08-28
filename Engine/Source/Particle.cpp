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
}

float Particle::Update(float DeltaTime, bool gravity)
{
    mLifeTime += DeltaTime;
    float dt01 = mLifeTime / mMaxLifeTime;
    float3 fall = float3(0,0,0);
    if (gravity) fall = float3(0, 1, 0) * mLifeTime * -9.81 * DeltaTime;
    mPosition = mPosition + mDirection * mSpeed * DeltaTime + fall;
    return dt01;
}