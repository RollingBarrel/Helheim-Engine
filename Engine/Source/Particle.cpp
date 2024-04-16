#include "Particle.h"

Particle::Particle()
{
    mPosition = float3::zero;
    mRotation = 0.0f;
    mSpeed = 0.0f;
    mSize = 0.0f;
    mLifetime = 0.0f;
    mColor = float4::zero;
}

Particle::Particle(float mLifeTime, float3 position, float speed)
    : mPosition(position), 
    mRotation(0.0f), 
    mSpeed(speed), 
    mSize(1.0f), 
    mLifetime(mLifeTime), 
    mColor(float4::zero)
{
}

Particle::Particle(const Particle& other)
{
	mPosition = other.mPosition;
	mRotation = other.mRotation;
	mSpeed = other.mSpeed;
	mSize = other.mSize;
	mLifetime = other.mLifetime;
	mColor = other.mColor;
}

Particle::~Particle()
{
}

void Particle::Update()
{
    mPosition = mPosition + mDirection * mSpeed;
}
