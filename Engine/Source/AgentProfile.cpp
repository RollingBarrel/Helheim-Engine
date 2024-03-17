#include "AgentProfile.h"

AgentProfile::AgentProfile(const std::string& name, float radius, float height, float stepHeight, unsigned int maxSlope) :
	mName(name), mRadius(radius), mHeight(height), mStepHeight(stepHeight), mMaxSlope(maxSlope)
{

}

AgentProfile::~AgentProfile()
{
}
