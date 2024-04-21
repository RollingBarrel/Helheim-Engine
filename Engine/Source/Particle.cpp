#include "Particle.h"

Particle::Particle()
{
    mPosition = float3::zero;
    mDirection = float3::unitX;
    mRotation = 0.0f;
    mIsSpeedCurve = false;
    mSpeedLinear = 0.01f;
    mIsSizeCurve = false;
    mSizeLinear = 1.0f;
    mMaxLifeTime = 1000.0f;
    mLifeTime = 1000.0f;
    mColor = { 1.0f, 1.0f, 1.0f, 1.0f };
}

Particle::Particle(float3 position, float3 direction, float rotation, float lifeTime, bool isSpeedCurve, bool isSizeCurve)
    : mPosition(position), 
    mDirection(direction),
    mRotation(rotation),
    mIsSpeedCurve(isSpeedCurve),
    mIsSizeCurve(isSizeCurve),
    mMaxLifeTime(lifeTime),
    mColor(float4::zero),
    mLifeTime(0.0f),
    mSizeLinear(0.0f),
    mSpeedLinear(0.0f)
{
}

Particle::~Particle()
{
}

bool Particle::Update(float DeltaTime)
{
    mLifeTime += DeltaTime;
    float dt01 = mLifeTime / mMaxLifeTime;
    // If the particle is dead exits and will be removed
    if (dt01 > 1.0f)
    {
		return false;
    }
    float size = mIsSizeCurve ? BezierValue(dt01, mSizeCurve) : mSizeLinear;
    float speed = mIsSpeedCurve ? BezierValue(dt01, mSpeedCurve) : mSpeedLinear;
    mPosition = mPosition + mDirection * mSpeedLinear * speed;
    return true;
}

float Particle::BezierValue(float dt01, float4 P) {
    enum { STEPS = 256 };
    float2 Q[4] = { {0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1} };
    float2 results[STEPS + 1];
    bezier_table<STEPS>(Q, results);
    return results[(int)((dt01 < 0 ? 0 : dt01 > 1 ? 1 : dt01) * STEPS)].y;
}