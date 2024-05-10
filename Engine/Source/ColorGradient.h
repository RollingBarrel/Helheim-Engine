#pragma once
#include "Globals.h"
#include "MathGeoLib.h"
#include "Archive.h"
#include <unordered_map>


class ENGINE_API ColorGradient
{
public:
	ColorGradient();
	~ColorGradient();

	void AddColorGradientMark(float position, const float4& color);
	void RemoveColorGradientMark(float position);

	std::unordered_map<float, float4> GetColorMarks() const { return mColorMarks; };

	float4 CalculateColor(float position) const;

	void Save(Archive& archive) const;
	void LoadFromJSON(const rapidjson::Value& data);
private:
	std::unordered_map<float, float4> mColorMarks;
};
