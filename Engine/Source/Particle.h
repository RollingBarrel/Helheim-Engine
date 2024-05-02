#pragma once
#include "MathGeoLib.h"

class Particle
{
public:
    Particle();
    Particle(float3 position, float3 direction, float4 color, float rotation, float lifeTime, bool isSpeedCurve, bool isSizeCurve);
    ~Particle();

    bool Update(float deltaTime, float3 cameraPosition);

    float3 GetPosition() const { return mPosition; }
    float3 GetDirection() const { return mDirection; }
    float GetRotation() const { return mRotation; }
    bool IsSpeedCurve() const { return mIsSpeedCurve; }
    float GetSpeedLineal() const { return mSpeedLinear; }
    float4 GetSpeedCurve() const { return mSpeedCurve; }
    bool IsSizeCurve() const { return mIsSizeCurve; }
    float GetSize() const { return mSizeLinear; }
    float4 GetSizeCurve() const { return mSizeCurve; }
    float GetLifetime() const { return mLifeTime; }
    float GetMaxLifetime() const { return mMaxLifeTime; }
    float4 GetColor() const { return mColor; }

    void SetPosition(const float3& position) { mPosition = position; }
    void SetDirection(const float3& direction) { mDirection = direction; }
    void SetRotation(float rotation) { mRotation = rotation; }
    void SetIsSpeedCurve(bool isSpeedCurve) { mIsSpeedCurve = isSpeedCurve; }
    void SetSpeedLineal(float speedLineal) { mSpeedLinear = speedLineal; }
    void SetSpeedCurve(const float4& speedCurve) { mSpeedCurve = speedCurve; }
    void SetIsSizeCurve(bool isSizeCurve) { mIsSizeCurve = isSizeCurve; }
    void SetSize(float size) { mSizeLinear = size; }
    void SetSizeCurve(const float4& sizeCurve) { mSizeCurve = sizeCurve; }
    void SetLifetime(float lifetime) { mLifeTime = lifetime; }
    void SetMaxLifetime(float maxLifetime) { mMaxLifeTime = maxLifetime; }
    void SetColor(const float4& color) { mColor = color; }

    template <int steps>
    void bezier_table(float2 P[], float2 results[])
    {
        static float C[(steps + 1) * 4], * K = 0;
        if (!K)
        {
            K = C;
            for (unsigned step = 0; step <= steps; ++step)
            {
                float t = (float)step / (float)steps;
                C[step * 4 + 0] = (1 - t) * (1 - t) * (1 - t); // * P0
                C[step * 4 + 1] = 3 * (1 - t) * (1 - t) * t;   // * P1
                C[step * 4 + 2] = 3 * (1 - t) * t * t;         // * P2
                C[step * 4 + 3] = t * t * t;                   // * P3
            }
        }
        for (unsigned step = 0; step <= steps; ++step)
        {
            float2 point = {
                K[step * 4 + 0] * P[0].x + K[step * 4 + 1] * P[1].x + K[step * 4 + 2] * P[2].x + K[step * 4 + 3] * P[3].x,
                K[step * 4 + 0] * P[0].y + K[step * 4 + 1] * P[1].y + K[step * 4 + 2] * P[2].y + K[step * 4 + 3] * P[3].y };
            results[step] = point;
        }
    }

    float BezierValue(float dt01, float4 P);

private:
    float3 mPosition; // Relativa al emisor de particulas
    float3 mDirection = float3::unitX; // Relativa al emisor de particulas
    float mRotation;

    bool mIsSpeedCurve;
    float mSpeedLinear;
    float4 mSpeedCurve;

    bool mIsSizeCurve;
    float mSizeLinear;
    float4 mSizeCurve;

    float mLifeTime;
    float mMaxLifeTime;
    float4 mColor;
};