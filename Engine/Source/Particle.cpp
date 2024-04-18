#include "Particle.h"

Particle::Particle()
{
    mPosition = float3::zero;
    mDirection = float3::zero;
    mRotation = 0.0f;
    mIsSpeedCurve = false;
    mSpeedLinear = 0.0f;
    mIsSizeCurve = false;
    mSizeLinear = 0.0f;
    mLifetime = 0.0f;
    mColor = float4::zero;
}

Particle::Particle(float3 position, float3 direction, float rotation, float lifeTime, bool isSpeedCurve, bool isSizeCurve)
    : mPosition(position), 
    mDirection(direction),
    mRotation(rotation),
    mIsSpeedCurve(isSpeedCurve),
    mIsSizeCurve(isSizeCurve),
    mSizeLinear(1.0f), 
    mMaxLifeTime(lifeTime),
    mColor(float4::zero)
{
}

Particle::~Particle()
{
}

void Particle::Update()
{
    mPosition = mPosition + mDirection * mSpeedLinear;
}

float Particle::BezierValue(float dt01, float4 P) {
    enum { STEPS = 256 };
    float2 Q[4] = { {0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1} };
    float2 results[STEPS + 1];
    bezier_table<STEPS>(Q, results);
    return results[(int)((dt01 < 0 ? 0 : dt01 > 1 ? 1 : dt01) * STEPS)].y;
}