#include "Particle.h"

Particle::Particle()
{
    mPosition = float3::zero;
    mDirection = float3::unitX;
    mRotation = 0.0f;
    mSpeed = 0.001f;
    mSize = 0.0f;
    mLifetime = 1000.0f;
    mColor = { 1.0f, 1.0f, 1.0f, 1.0f };
}

Particle::Particle(float3 position, float3 direction, float rotation, float lifeTime, float speed)
    : mPosition(position), 
    mDirection(direction),
    mRotation(rotation),
    mSpeed(speed), 
    mSize(1.0f), 
    mLifetime(lifeTime),
    mColor(float4::zero)
{
}

Particle::Particle(const Particle& other)
{
	mPosition = other.mPosition;
    mDirection = other.mDirection;
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
