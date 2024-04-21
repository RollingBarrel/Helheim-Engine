#include "EmitterShape.h"

EmitterShape::EmitterShape()
{
    mPosition = float3::zero;
	mDirection = float3::unitX;
	mShapeRadius = 1.0f;
	mShapeAngle = 3.1415f / 4.0f;
	mShapeSize = float2(1.0f, 1.0f);
}

EmitterShape::~EmitterShape()
{
}

float3 EmitterShape::RandomInitPosition()
{
	return mPosition;
}

float3 EmitterShape::RandomInitDirection()
{
	return mDirection;
}

float3 EmitterShape::positionTransform(float3 position)
{
    float3 newX = Cross(float3(0, 0, 1), mDirection.Normalized());
    float3 newY = Cross(newX, mDirection.Normalized());
    return mPosition + newX * position.x + newY * position.y;
}

EmitterShapeCone::EmitterShapeCone(float3 position, float3 direction)
{
	mPosition = position;
	mDirection = direction;
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
	float3 position = positionTransform(float3(x, y, 0));

    return position;
}

float3 EmitterShapeCone::RandomInitDirection()
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float angleA = r * 2 * 3.1415f;
    float angleB = r * mShapeAngle;
    float3 dirA = positionTransform(float3(cos(angleA), sin(angleA), 0));
    float3 rotationAxis = Cross(mDirection, dirA);
    float3 direction = mDirection * float3x3::RotateAxisAngle(rotationAxis, angleB);

    return direction;
}

EmitterShapeSquare::EmitterShapeSquare(float3 position, float3 direction)
{
    mPosition = position;
    mDirection = direction;
    mShapeSize = float2(1.0f, 1.0f);
}

float3 EmitterShapeSquare::RandomInitPosition()
{
    float randX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    randX -= 0.5f;
    float randY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    randY -= 0.5f;
    float3 position = positionTransform(float3(mShapeSize.x * randX, mShapeSize.y * randY, 0));

    return position;
}

float3 EmitterShapeSquare::RandomInitDirection()
{
    return mDirection;
}

EmitterShapeCircle::EmitterShapeCircle(float3 position, float3 direction)
{
    mPosition = position;
    mDirection = direction;
    mShapeRadius = 1.0f;
}

float3 EmitterShapeCircle::RandomInitPosition()
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float angle = r * 2 * 3.1415f;
    r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float distance = r * mShapeRadius;
    float x = distance * cos(angle);
    float y = distance * sin(angle);
    float3 position = positionTransform(float3(x, y, 0));

    return position;
}

float3 EmitterShapeCircle::RandomInitDirection()
{
    return mDirection;
}
