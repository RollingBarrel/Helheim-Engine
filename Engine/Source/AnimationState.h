#pragma once

#include <vector>
#include <string>

class ResourceAnimation;
class AnimationTransition;
class AnimationClip;

class AnimationState

{
public:
	AnimationState(AnimationClip* clip, std::string name);
	AnimationState* GetNextAnimation()  const;

	std::string mName;
	std::string mClip;
	ResourceAnimation* mAnimation;
	float mStartTime, mEndTime;
	bool mLoop;

};

