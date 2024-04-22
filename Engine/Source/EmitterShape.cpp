#include "EmitterShape.h"

EmitterShape::EmitterShape()
{
	mShapeRadius = 1.0f;
	mShapeAngle = 3.1415f / 4.0f;
	mShapeSize = float2(1.0f, 1.0f);
}

EmitterShape::~EmitterShape()
{
}

float3 EmitterShape::RandomInitPosition()
{
	return position;
}

float3 EmitterShape::RandomInitDirection()
{
	return direction;
}

EmitterShapeCone::EmitterShapeCone()
{
	mShapeRadius = 1.0f;
	mShapeAngle = 3.1415f / 4.0f;
}

float3 EmitterShapeCone::RandomInitPosition()
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float angle = r * 2 * 3.1415f;
    r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float distance = r * mShapeRadius;
    float x = distance * cos(angle);
    float y = distance * sin(angle);

    return float3(x, y, 0);
}

float3 EmitterShapeCone::RandomInitDirection()
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float angleA = r * 2 * 3.1415f;
    r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float angleB = r * mShapeAngle;
    float3 dirA = float3(cos(angleA), sin(angleA), cos(angleB)).Normalized();

    return dirA;
}

//EmitterShapeSquare::EmitterShapeSquare()
//{
//    mShapeSize = float2(1.0f, 1.0f);
//}
//
//float3 EmitterShapeSquare::RandomInitPosition()
//{
//    float randX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    randX -= 0.5f;
//    float randY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    randY -= 0.5f;
//    float3 position = positionTransform(float3(mShapeSize.x * randX, mShapeSize.y * randY, 0), position, direction);
//
//    return position;
//}
//
//float3 EmitterShapeSquare::RandomInitDirection()
//{
//    return direction;
//}
//
//EmitterShapeCircle::EmitterShapeCircle()
//{
//    mShapeRadius = 1.0f;
//}
//
//float3 EmitterShapeCircle::RandomInitPosition()
//{
//    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    float angle = r * 2 * 3.1415f;
//    r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//    float distance = r * mShapeRadius;
//    float x = distance * cos(angle);
//    float y = distance * sin(angle);
//    float3 position = positionTransform(float3(x, y, 0), position, direction);
//
//    return position;
//}
//
//float3 EmitterShapeCircle::RandomInitDirection()
//{
//    return direction;
//}
