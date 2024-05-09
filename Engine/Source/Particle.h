#pragma once
#include "MathGeoLib.h"
#include "ColorGradient.h"

class Particle
{
public:
    Particle();
    Particle(float3 position, float3 direction, float4 color, float rotation, float lifeTime);
    ~Particle();

    float Update(float deltaTime);

    float3 GetPosition() const { return mPosition; }
    float3 GetDirection() const { return mDirection; }
    float GetRotation() const { return mRotation; }
    float GetSpeed() const { return mSpeed; }
    float GetSize() const { return mSize; }
    float4 GetColor() const { return mColor; }

    float GetLifetime() const { return mLifeTime; }
    float GetMaxLifetime() const { return mMaxLifeTime; }

    void SetPosition(const float3& position) { mPosition = position; }
    void SetDirection(const float3& direction) { mDirection = direction; }
    void SetRotation(float rotation) { mRotation = rotation; }
    void SetSpeed(float speed) { mSpeed = speed; }
    void SetSize(float size) { mSize = size; }
    void SetColor(float4 color) { mColor = color; }

    void SetLifetime(float lifetime) { mLifeTime = lifetime; }
    void SetMaxLifetime(float maxLifetime) { mMaxLifeTime = maxLifetime; }

private:
    float3 mPosition; // Relativa al emisor de particulas
    float3 mDirection = float3::unitX; // Relativa al emisor de particulas
    float4 mColor = float4::one;
    float mRotation = 0.0f;

    float mSpeed = 1.0f;

    float mSize = 1.0f;


    float mLifeTime = 0.0f;
    float mMaxLifeTime = 3.0f;
};