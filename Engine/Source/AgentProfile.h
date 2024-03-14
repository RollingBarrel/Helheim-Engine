#pragma once
#include <string>
class AgentProfile
{
public:
	AgentProfile(std::string name, float radius, float height, float stepHeight, unsigned int maxSlope);
	~AgentProfile();
	std::string getName() { return mName; };
	const float getRadius() const { return mRadius; };
	float getHeight() { return mHeight; };
	float getStepHeight() { return mStepHeight; };
	unsigned int getMaxSlope() { return mMaxSlope; };

private:
	std::string mName = ""; 
	float mRadius = 0.0f;
	float mHeight = 0.0f;
	float mStepHeight = 0.0f;
	unsigned int mMaxSlope = 0;
	


};

