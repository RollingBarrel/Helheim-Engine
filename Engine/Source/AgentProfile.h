#pragma once
#include <string>
class AgentProfile
{
public:
	AgentProfile(const std::string& name, float radius, float height, float stepHeight, unsigned int maxSlope);
	~AgentProfile();
	const std::string& getName() const { return mName; };
	const float getRadius() const { return mRadius; };
	float getHeight() const { return mHeight; };
	float getStepHeight() const { return mStepHeight; };
	unsigned int getMaxSlope() const { return mMaxSlope; };

private:
	std::string mName = ""; 
	float mRadius = 0.0f;
	float mHeight = 0.0f;
	float mStepHeight = 0.0f;
	unsigned int mMaxSlope = 0;
};

