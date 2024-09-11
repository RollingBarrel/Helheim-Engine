#pragma once
#include "ColorGradient.h"

class Trail;
class GameObject;

class Particle
{
public:
    Particle();
    Particle(float3 position, float3 direction, float4 color, float rotation, float lifeTime, bool hasTrail, const Trail* trail);
    ~Particle();

    float Update(float deltaTime, float gravity, GameObject* owner);
    void DrawTrail();

    const float3& GetPosition() const { return mPosition; }
    const float3& GetDirection() const { return mDirection; }
    float GetRotation() const { return mRotation; }
    float GetSpeed() const { return mSpeed; }
    float GetInitialSpeed() const { return mInitialSpeed; }
    float GetSize() const { return mSize; }
    float GetInitialSize() const { return mInitialSize; }
    const float4& GetColor() const { return mColor; }
    float GetLifetime() const { return mLifeTime; }
    float GetMaxLifetime() const { return mMaxLifeTime; }

    void SetPosition(const float3& position) { mPosition = position; }
    void SetDirection(const float3& direction) { mDirection = direction; }
    void SetRotation(float rotation) { mRotation = rotation; }
    void SetSpeed(float speed) { mSpeed = speed; }
    void SetInitialSpeed(float speed) { mInitialSpeed = speed; }
    void SetSize(float size) { mSize = size; }
    void SetInitialSize(float size) { mInitialSize = size; }
    void SetColor(const float4& color) { mColor = color; }
    void SetLifetime(float lifetime) { mLifeTime = lifetime; }

private:
    float3 mPosition; // Relativa al emisor de particulas
    float3 mDirection = float3::unitX; // Relativa al emisor de particulas
    float4 mColor = float4::one;
    float mRotation = 0.0f;

    float mSpeed = 1.0f;
    float mInitialSpeed;
    float mSize = 1.0f;
    float mInitialSize;

    float mLifeTime = 0.0f;
    const float mMaxLifeTime = 3.0f;

    bool mHasTrail = false;
    Trail* mTrail = nullptr;
};