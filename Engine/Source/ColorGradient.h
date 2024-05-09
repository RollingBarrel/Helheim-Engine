#pragma once
#include "Globals.h"
#include "MathGeoLib.h"
#include "Archive.h"
#include <list>

struct  ColorGradientMark
{
	float4 color;
	float position; // from 0 to 1
};

class ENGINE_API ColorGradient
{
public:
	ColorGradient();
	~ColorGradient();

	void AddColorGradientMark(float position, const float4& color);
	void RemoveColorGradientMark(ColorGradientMark* mark);

	std::list<ColorGradientMark*> GetColorMarks() { return mColorMarks; }

	float4 CalculateColor(float position) const;

	void Save(Archive& archive) const;
	void LoadFromJSON(const rapidjson::Value& data);
private:
	std::list<ColorGradientMark*> mColorMarks;
};
