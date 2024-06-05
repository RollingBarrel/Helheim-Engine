#pragma once
#include "Archive.h"
#include "float2.h"

class BezierCurve 
{
	friend class InspectorPanel;
public:
	BezierCurve();

	float GetValue(const float dt, const float initialValue) const;
	float CalculateInitialValue();

	void SaveJson(Archive& archive) const;
	void LoadJson(const rapidjson::Value& data);

private:
	void spline(const float* key, int num, int dim, float t, float* v) const;
	float CurveValueSmooth(float p) const;
	float CurveValue(float p) const;
	float CalculateRandomValue();

    bool mIsCurve = false;

	bool mIsValueRandom;
	float mValue;
	float mMaxValue;
	float mInitialValue;

    std::vector<float2> mPoints;
    float mFactor = 1.0f;
};

