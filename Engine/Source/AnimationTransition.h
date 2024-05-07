#pragma once
#include <string>

class AnimationState;

class AnimationTransition
{
public:
	AnimationTransition(std::string sourceName, std::string targetName, std::string trigger);
	std::string mTrigger;
	std::string mSource;
	std::string mTarget;

};
