#include "RandomFloat.h"
#include <cstdlib>
#include <ctime>

RandomFloat::RandomFloat()
{
    mInitialValue = CalculateRandom();
}

RandomFloat::RandomFloat(const RandomFloat& original) :
    mIsRand(original.mIsRand), mMin(original.mMin), mMax(original.mMax)
{
    mInitialValue = CalculateRandom();
}

RandomFloat::~RandomFloat()
{
}

void RandomFloat::CalculateInitialValue()
{
    if (mIsRand)
    {
        float random_value = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        mInitialValue = random_value * (mMax - mMin) + mMin;
    }
    else
    {
        mInitialValue = mMin;
    }

}

float RandomFloat::CalculateRandom() const
{
    if (mIsRand)
    {
        float random_value = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return random_value * (mMax - mMin) + mMin;
    }
    else
    {
        return mMin;
    }

}

void RandomFloat::Save(JsonObject& obj) const
{
    obj.AddBool("IsValueRandom", mIsRand);
    obj.AddFloat("MinValue", mMin);
    obj.AddFloat("MaxValue", mMax);
}

void RandomFloat::Load(const JsonObject& data)
{
    mIsRand = data.GetBool("IsValueRandom");
    mMin = data.GetFloat("MinValue");
    mMax = data.GetFloat("MaxValue");
    mInitialValue = CalculateRandom();
}