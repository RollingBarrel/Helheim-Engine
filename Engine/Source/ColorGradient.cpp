#include "ColorGradient.h"

ColorGradient::ColorGradient()
{
}

ColorGradient::ColorGradient(const ColorGradient& original)
{
    mColorMarks = original.mColorMarks;
}

ColorGradient::~ColorGradient()
{
	mColorMarks.clear();
}

void ColorGradient::AddColorGradientMark(float position, const float4& color)
{
    mColorMarks[position] = color;
}

void ColorGradient::RemoveColorGradientMark(float position)
{
    //assert(mColorMarks.find(position) != mColorMarks.end() && "Position not found in ColorGradient");
    mColorMarks.erase(position);
}

//template<typename T> static inline T MyClamp(T v, T mn, T mx) { return (v < mn) ? mn : (v > mx) ? mx : v; }

float* ColorGradient::GetColor(float pos)
{
    assert(mColorMarks.find(pos) != mColorMarks.end() && "Position not found in ColorGradient");
    return mColorMarks[pos].ptr();
}

float4 ColorGradient::CalculateColor(float position) const
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (mColorMarks.empty()) return color;

    position = (position < 0) ? 0 : (position > 1) ? 1 : position;

    auto upper = mColorMarks.upper_bound(position);
    auto lower = upper;
    --lower;
    if (upper == mColorMarks.begin())
    {
        return upper->second;
    }
    if (upper == mColorMarks.end())
    {
        return lower->second;
    }
    else
    {
        float distance = upper->first - lower->first;
        float delta = (position - lower->first) / distance;

        color.x = ((1.0f - delta) * lower->second.x) + ((delta)*upper->second.x);
        color.y = ((1.0f - delta) * lower->second.y) + ((delta)*upper->second.y);
        color.z = ((1.0f - delta) * lower->second.z) + ((delta)*upper->second.z);
    }
    return color;
}

void ColorGradient::Save(Archive& archive) const {
    std::vector<Archive> objectArray;

    for (const auto& mark : mColorMarks) {
        float time = mark.first;
        float4 color = mark.second;
        Archive colorArchive;
        colorArchive.AddFloat("Time", time);

        const float c[4] = { color.x, color.y, color.z, color.w };
        colorArchive.AddFloat4("Color", c);

        objectArray.push_back(colorArchive);
    }

    archive.AddObjectArray("Color Gradient", objectArray);
}


void ColorGradient::LoadFromJSON(const rapidjson::Value& data) {
    const auto& colorArray = data["Color Gradient"].GetArray();

    // Clear existing marks before loading new ones
    mColorMarks.clear();

    // Iterate over the JSON array
    for (const auto& color : colorArray) {
        float time = 0.0f;
        if (color.HasMember("Time") && color["Time"].IsFloat()) {
            time = color["Time"].GetFloat();
        }

        float colorVec[4] = { 0 };
        if (color.HasMember("Color") && color["Color"].IsArray()) {
            const auto& colArray = color["Color"].GetArray();
            if (colArray.Size() == 4) {
                for (unsigned int j = 0; j < colArray.Size(); ++j) {
                    if (colArray[j].IsFloat() && j < 4) {
                        colorVec[j] = colArray[j].GetFloat();
                    }
                }
            }
        }

        // Create a new ColorGradientMark and add it to the list
        mColorMarks[time] = float4(colorVec);
    }
}