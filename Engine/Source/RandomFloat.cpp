#include "RandomFloat.h"
#include <cstdlib>
#include <ctime>

RandomFloat::RandomFloat()
{
}

RandomFloat::RandomFloat(const RandomFloat& original) :
	mMin(original.mMin), mMax(original.mMax)
{
}

RandomFloat::~RandomFloat()
{
}

float RandomFloat::CalculateRandom() const
{
    if (mIsRand) 
    {
        //std::srand(static_cast<unsigned int>(std::time(nullptr)));
        float random_value = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return random_value * (mMax - mMin) + mMin;
    }
    else
    {
        return mMin;
    }

}

void RandomFloat::SaveJson(Archive& archive) const
{
    archive.AddBool("IsRandom", mIsRand);
    archive.AddFloat("Min Lifetime", mMin);
    archive.AddFloat("Max Lifetime", mMax);
}

void RandomFloat::LoadJson(const rapidjson::Value& data)
{
    if (data.HasMember("IsRandom") && data["IsRandom"].IsBool())
    {
        mIsRand = data["IsRandom"].GetBool();
    }
    if (data.HasMember("Min Lifetime") && data["Min Lifetime"].IsFloat())
    {
        mMin = data["Min Lifetime"].GetFloat();
    }
    if (data.HasMember("Max Lifetime") && data["Max Lifetime"].IsFloat())
    {
        mMax = data["Max Lifetime"].GetFloat();
    }
}
