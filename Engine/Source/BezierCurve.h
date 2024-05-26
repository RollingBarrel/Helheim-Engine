#pragma once
#include "MathGeoLib.h"
#include "Archive.h"

class BezierCurve 
{
	friend class InspectorPanel;
public:
	BezierCurve();
	float GetValue(float dt) const;
	float GetInitialValue() const { return mLineal; }

	void Save(JsonObject& archive) const;
	void Load(const JsonObject& data);

private:
	template <int steps>
	static void BezierTable(float2 P[], float2 results[]);
	static float BezierValue(float dt01, float4 P);

	bool mIsCurve = false;
	float mLineal = 1.0f;
	float4 mCurve;
	float mCurveFactor = 1.0f;
};