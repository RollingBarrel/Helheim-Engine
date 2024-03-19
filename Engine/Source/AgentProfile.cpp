#include "AgentProfile.h"

AgentProfile::AgentProfile(std::string name, float radius, float height, float stepHeight, unsigned int maxSlope) :
	mName(name), mRadius(radius), mHeight(height), mStepHeight(stepHeight), mMaxSlope(maxSlope)
{

}

AgentProfile::~AgentProfile()
{
}
