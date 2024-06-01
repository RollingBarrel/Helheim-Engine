#pragma once
#include "MathGeoLib.h"
#include "Archive.h"
#include "RandomFloat.h"

class BezierCurve 
{
	friend class InspectorPanel;
public:
	BezierCurve();

	float GetValue(float dt) const;
	float GetInitialValue() const { return mInitialValue.CalculateRandom(); }

	void SaveJson(Archive& archive) const;
	void LoadJson(const rapidjson::Value& data);

private:
	void spline(const float* key, int num, int dim, float t, float* v) const;
	float CurveValueSmooth(float p) const;
	float CurveValue(float p) const;

    bool mIsCurve = false;
    RandomFloat mInitialValue;
    std::vector<float2> mPoints;
    float mFactor = 1.0f;
};

