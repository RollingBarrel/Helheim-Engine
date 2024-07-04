#pragma once
#include "Globals.h"
#include "Archive.h"


class ENGINE_API RandomFloat
{
	friend class InspectorPanel;
public:
	RandomFloat();
	RandomFloat(const RandomFloat& original);
	~RandomFloat();

	float GetInitialValue() const { return mInitialValue; }
	void CalculateInitialValue();
	float CalculateRandom() const;

	void Save(JsonObject& obj) const;
	void Load(const JsonObject& data);
private:
	bool mIsRand = false;
	float mMin = 1.0f;
	float mMax = 1.0f;
	float mInitialValue = 0.0f;
};