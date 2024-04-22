#pragma once

#include "MathGeoLib.h"

class EmitterShape
{
    friend class InspectorPanel;
public:
    enum Type
    {
        NONE = 0,
		CONE,
        SQUARE,
        CIRCLE
	};
    EmitterShape();
    ~EmitterShape();

    virtual float3 RandomInitPosition();
    virtual float3 RandomInitDirection();

protected:
    float mShapeRadius;
    float mShapeAngle;
    float2 mShapeSize;
};

class EmitterShapeCone : public EmitterShape
{
public:
    EmitterShapeCone();

	float3 RandomInitPosition() override;
    float3 RandomInitDirection() override;
};

//class EmitterShapeSquare : public EmitterShape
//{
//public:
//    EmitterShapeSquare();
//
//    float3 RandomInitPosition() override;
//    float3 RandomInitDirection() override;
//};
//
//class EmitterShapeCircle : public EmitterShape
//{
//public:
//    EmitterShapeCircle();
//
//    float3 RandomInitPosition() override;
//    float3 RandomInitDirection() override;
//};
