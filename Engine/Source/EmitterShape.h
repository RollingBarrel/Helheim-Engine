#pragma once

#include "MathGeoLib.h"

class EmitterShape
{
    friend class InspectorPanel;
public:
    enum Type
    {
        NONE,
		CONE,
        SQUARE,
        CIRCLE
	};
    EmitterShape();
    ~EmitterShape();

    virtual float3 RandomInitPosition();
    virtual float3 RandomInitDirection();

protected:
    float3 positionTransform(float3 position);

    float3 mPosition;
    float3 mDirection;
    float mShapeRadius;
    float mShapeAngle;
    float2 mShapeSize;
};

class EmitterShapeCone : public EmitterShape
{
public:
    EmitterShapeCone(float3 position, float3 direction);

	float3 RandomInitPosition() override;
    float3 RandomInitDirection() override;
};

class EmitterShapeSquare : public EmitterShape
{
public:
    EmitterShapeSquare(float3 position, float3 direction);

    float3 RandomInitPosition() override;
    float3 RandomInitDirection() override;
};

class EmitterShapeCircle : public EmitterShape
{
public:
    EmitterShapeCircle(float3 position, float3 direction);

    float3 RandomInitPosition() override;
    float3 RandomInitDirection() override;
};
