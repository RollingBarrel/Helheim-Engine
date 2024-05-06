#pragma once

#include <vector>
#include <string>

class ResourceAnimation;
class AnimationTransition;

class AnimationState

{
public:
	AnimationState(ResourceAnimation* mAnimation);
	AnimationState* GetNextAnimation()  const;

	std::string mName;
	std::string clipName;
	ResourceAnimation* mAnimation;
	float mStartTime, mEndTime;
	bool mLoop;

};

