#pragma once

#include <vector>
#include <string>

class ResourceAnimation;
class AnimationTransition;
class AnimationClip;

class AnimationState

{
public:
	AnimationState(std::string clipName, std::string name);
	AnimationState* GetNextAnimation()  const;

	std::string mName;
	std::string mClip;
	float mStartTime, mEndTime;
	bool mLoop;

};

