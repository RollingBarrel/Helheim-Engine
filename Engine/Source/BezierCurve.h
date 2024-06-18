#pragma once
#include "Archive.h"
#include "float2.h"
#include "RandomFloat.h"

class BezierCurve 
{
	friend class InspectorPanel;
public:
	BezierCurve();

	float CalculateValue(const float dt, const float initialValue) const;
	RandomFloat GetValue() const { return mValue; }

	void Save(JsonObject& archive) const;
	void Load(const JsonObject& data);


private:
	void spline(const float* key, int num, int dim, float t, float* v) const;
	float CurveValueSmooth(float p) const;
	float CurveValue(float p) const;

    bool mIsCurve = false;

	RandomFloat mValue;

    std::vector<float2> mPoints;
};

