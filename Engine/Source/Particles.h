#pragma once

#include "MathGeoLib.h"

class Particle
{
public:
    Particle();
    Particle(const Particle& other);
    ~Particle();

    float3 getPosition() const { return mPosition; }
    float getRotation() const { return mRotation; }
    float3 getSpeed() const { return mSpeed; }
    float getSize() const { return mSize; }
    float getLifetime() const { return mLifetime; }
    float4 getcolor() const { return mColor; }
    //Material getmaterial() const { return mMaterial; }

    void setPosition(const float3& position) { mPosition = position; }
    void setRotation(float rotation) { mRotation = rotation; }
    void setSpeed(const float3& speed) { mSpeed = speed; }
    void setSize(float size) { mSize = size; }
    void setLifetime(float lifetime) { mLifetime = lifetime; }
    void setColor(const float4& color) { mColor = color; }
    //void setMaterial(const Material& material) { mMaterial = material; }

private:
    float3 mPosition;
    float mRotation;
    float3 mSpeed;
    float mSize;
    float mLifetime;
    float4 mColor;
    //Material mMaterial;
};