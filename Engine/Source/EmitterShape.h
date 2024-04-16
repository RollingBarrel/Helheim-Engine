#pragma once

#include "MathGeoLib.h"

class EmitterShape
{
public:
    EmitterShape();
    EmitterShape(const EmitterShape& orig);
    ~EmitterShape();

    virtual float3 RandomInitPosition();
    virtual float3 RandomInitDirection();

protected:
    float3 positionTransform(float3 position);

    float3 mPosition;
    float3 mDirection;
    float mShapeRadius;
    float mShapeAngle;
};

class EmitterShapeCone : public EmitterShape
{
public:
	float3 RandomInitPosition() override;
    float3 RandomInitDirection() override;
};

//class EmitterShapeBox : public EmitterShape
//{
//public:
//    EmitterShapeBox();
//	EmitterShapeBox(const EmitterShapeBox& orig);
//	~EmitterShapeBox() override;
//
//	float3 RandomInitPosition() override;
//    float3 RandomInitDirection() override;
//
//private:
//    float2 mShapeBox;
//};
//
//class EmitterShapeCone : public EmitterShape
//{
//public:
//    EmitterShapeCone();
//    EmitterShapeCone(const EmitterShapeCone& orig);
//    ~EmitterShapeCone() override;
//
//    float3 RandomInitPosition() override;
//    float3 RandomInitDirection() override;
//
//private:
//    float mShapeAngle;
//    float mShapeRadius;
//};