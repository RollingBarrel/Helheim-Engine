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

	float CalculateRandom() const;

	void SaveJson(Archive& archive) const;
	void LoadJson(const rapidjson::Value& data);
private:
	bool mIsRand = false;
	float mMin = 0.0f;
	float mMax = 1.0f;
};
