#include "Particle.h"

Particle::Particle()
{
}

Particle::Particle(float3 position, float3 direction, float4 color, float rotation, float lifeTime, bool isSpeedCurve, bool isSizeCurve)
    : mPosition(position), 
    mDirection(direction),
    mRotation(rotation),
    mIsSpeedCurve(isSpeedCurve),
    mIsSizeCurve(isSizeCurve),
    mMaxLifeTime(lifeTime),
    mColor(color)
{
}

Particle::~Particle()
{
}

bool Particle::Update(float DeltaTime, float3 cameraPosition)
{
    mLifeTime += DeltaTime;
    float dt01 = mLifeTime / mMaxLifeTime;
    // If the particle is dead exits and will be removed
    if (dt01 > 1.0f)
    {
		return false;
    }
    mSize = mIsSizeCurve ? mSizeLinear + (BezierValue(dt01, mSizeCurve) * mSizeCurveFactor) : mSizeLinear;
    mSpeed = mIsSpeedCurve ? mSpeedLinear + (BezierValue(dt01, mSpeedCurve) * mSpeedCurveFactor) : mSpeedLinear;
    mPosition = mPosition + mDirection * mSpeed * DeltaTime;
    return true;
}

float Particle::BezierValue(float dt01, float4 P) {
    enum { STEPS = 256 };
    float2 Q[4] = { {0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1} };
    float2 results[STEPS + 1];
    bezier_table<STEPS>(Q, results);
    return results[(int)((dt01 < 0 ? 0 : dt01 > 1 ? 1 : dt01) * STEPS)].y;
}