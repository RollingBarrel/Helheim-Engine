#include "ColorGradient.h"

ColorGradient::ColorGradient()
{
}

ColorGradient::~ColorGradient()
{
	for (auto mark : mColorMarks) {
		delete mark;
	}

	mColorMarks.clear();
}

ColorGradient::ColorGradient(const ColorGradient& original)
{
    for (ColorGradientMark* mark : original.mColorMarks)
    {
        ColorGradientMark* newMark = new ColorGradientMark();
        newMark->position = mark->position;
        newMark->color = mark->color;
        mColorMarks.push_back(newMark);
    }
}

void ColorGradient::AddColorGradientMark(float position, const float4& color)
{
	ColorGradientMark* mark = new ColorGradientMark();
	mark->position = position;
	mark->color = color;
	mColorMarks.push_back(mark);
}

void ColorGradient::RemoveColorGradientMark(ColorGradientMark* mark)
{
    mColorMarks.remove(mark);
}

template<typename T> static inline T MyClamp(T v, T mn, T mx) { return (v < mn) ? mn : (v > mx) ? mx : v; }

float4 ColorGradient::CalculateColor(float position) const
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    position = (position < 0) ? 0 : (position > 1) ? 1 : position;

    ColorGradientMark* lower = nullptr;
    ColorGradientMark* upper = nullptr;

    for (ColorGradientMark* mark : mColorMarks)
    {
        if (mark->position < position)
        {
            if (!lower || lower->position < mark->position)
            {
                lower = mark;
            }
        }

        if (mark->position >= position)
        {
            if (!upper || upper->position > mark->position)
            {
                upper = mark;
            }
        }
    }

    if (upper && !lower)
    {
        lower = upper;
    }
    else if (!upper && lower)
    {
        upper = lower;
    }
    else if (!lower && !upper)
    {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (upper == lower)
    {
        color.x = upper->color.x;
        color.y = upper->color.y;
        color.z = upper->color.z;
    }
    else
    {
        float distance = upper->position - lower->position;
        float delta = (position - lower->position) / distance;

        color.x = ((1.0f - delta) * lower->color.x) + ((delta)*upper->color.x);
        color.y = ((1.0f - delta) * lower->color.y) + ((delta)*upper->color.y);
        color.z = ((1.0f - delta) * lower->color.z) + ((delta)*upper->color.z);
    }
    return color;
}

void ColorGradient::Save(Archive& archive) const {
    std::vector<Archive> objectArray;

    for (const auto& mark : mColorMarks) {
        float time = mark->position;
        float4 color = mark->color;
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
    for (auto mark : mColorMarks) {
        delete mark;
    }
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
        ColorGradientMark* newMark = new ColorGradientMark();
        newMark->position = time;
        newMark->color = float4(colorVec);
        mColorMarks.push_back(newMark);
    }
}