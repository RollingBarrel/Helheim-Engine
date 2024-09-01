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

float Particle::Update(float DeltaTime, float gravity)
{
    mLifeTime += DeltaTime;
    float dt01 = mLifeTime / mMaxLifeTime;
    mDirection = mDirection + float3(0, 1, 0) * DeltaTime * -9.81 * gravity;
    mPosition = mPosition + mDirection * mSpeed * DeltaTime;
    return dt01;
}