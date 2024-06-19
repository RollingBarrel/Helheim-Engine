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
    mColorMarks.erase(position);
}

float* ColorGradient::GetColor(float pos)
{
    //assert(mColorMarks.find(pos) != mColorMarks.end() && "Position not found in ColorGradient");
    if (mColorMarks.find(pos) == mColorMarks.end()) return nullptr;
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

void ColorGradient::Save(JsonObject& obj) const 
{

    JsonArray arr = obj.AddNewJsonArray("Color Gradient");
    for (const auto& mark : mColorMarks)
    {
        JsonObject colorObj = arr.PushBackNewObject();
        float time = mark.first;
        float4 color = mark.second;
        colorObj.AddFloat("Time", mark.first);
        colorObj.AddFloats("Color", mark.second.ptr(), 4);
    }
}


void ColorGradient::Load(const JsonObject& data) 
{
    // Clear existing marks before loading new ones
    mColorMarks.clear();

    if (data.HasMember("Color Gradient"))
    {
        JsonArray colorArray = data.GetJsonArray("Color Gradient");
        for (unsigned int i = 0; i < colorArray.Size(); ++i)
        {
            JsonObject color = colorArray.GetJsonObject(i);
            float time = 0.0f;
            if (color.HasMember("Time")) time = color.GetFloat("Time");
            float col[4];
            if (color.HasMember("Color")) color.GetFloats("Color", col);
            mColorMarks[time] = float4(col);
        }
    }
}