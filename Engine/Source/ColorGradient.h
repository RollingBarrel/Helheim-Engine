#pragma once
#include "Globals.h"
#include "MathGeoLib.h"
#include "Archive.h"
#include <map>


class ENGINE_API ColorGradient
{
public:
	ColorGradient();
	ColorGradient(const ColorGradient& original);
	~ColorGradient();

	void AddColorGradientMark(float position, const float4& color);
	void RemoveColorGradientMark(float position);

	std::map<float, float4> GetColorMarks() const { return mColorMarks; };

	float4 CalculateColor(float position) const;

	void Save(JsonObject& archive) const;
	void Load(const JsonObject& data);
private:
	std::map<float, float4> mColorMarks;
};
