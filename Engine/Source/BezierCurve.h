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
	void SaveJson(Archive& archive) const;
	void LoadJson(const rapidjson::Value& data);

private:
	template <int steps>
	static void BezierTable(float2 P[], float2 results[]);
	static float BezierValue(float dt01, float4 P);

	bool mIsCurve = false;
	float mLineal = 1.0f;
	float4 mCurve;
	float mCurveFactor = 1.0f;
};