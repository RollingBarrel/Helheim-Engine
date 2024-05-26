#include "EmitterShape.h"

# define M_PI   3.14159265358979323846f

EmitterShape::EmitterShape()
{
    mType = Type::NONE;
	mShapeRadius = 1.0f;
	mShapeAngle = 3.1415f / 4.0f;
	mShapeSize = float2(1.0f, 1.0f);
}

EmitterShape::~EmitterShape()
{
}

void EmitterShape::Save(JsonObject& obj) const
{
    obj.AddInt("ShapeType", mType);
    obj.AddFloat("ShapeRadius", mShapeRadius);
    obj.AddFloat("ShapeAngle", mShapeAngle);
    obj.AddFloats("ShapeSize", mShapeSize.ptr(), 2);
}

void EmitterShape::Load(const JsonObject& data)
{
    mType = (Type) data.GetInt("ShapeType");
    mShapeRadius = data.GetFloat("ShapeRadius");
    mShapeAngle = data.GetFloat("ShapeAngle");
    float size[2];
    data.GetFloats("ShapeSize", size);
    mShapeSize = float2(size);
}

void EmitterShape::CopyShape(const EmitterShape& original)
{
    mShapeAngle = original.mShapeAngle;
    mShapeRadius = original.mShapeRadius;
    mShapeSize = original.mShapeSize;
}

float3 EmitterShape::RandomInitPosition()
{
	return float3(0,0,0);
}

float3 EmitterShape::RandomInitDirection()
{
	return float3(0, 0, 1);
}

EmitterShapeCone::EmitterShapeCone()
{
    mType = Type::CONE;
	mShapeRadius = 1.0f;
	mShapeAngle = 3.1415f / 4.0f;
}

float3 EmitterShapeCone::RandomInitPosition()
{
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float angle = r * 2 * M_PI;
    r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float distance = r * mShapeRadius;
    float x = distance * cos(angle);
    float y = distance * sin(angle);

    return float3(x, y, 0);
}

float3 EmitterShapeCone::RandomInitDirection()
{
    float r1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float theta = r1 * 2 * M_PI; // Angle in XY plane

    // Instead of a linear distribution for angleB, we use an angular distribution.
    float r2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float phi = std::acos(1 - r2 * (1 - std::cos(mShapeAngle))); // Angle from Z axis

    float x = std::sin(phi) * std::cos(theta);
    float y = std::sin(phi) * std::sin(theta);
    float z = std::cos(phi);

    return float3(x, y, z).Normalized(); // Normalize to ensure unit length
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
