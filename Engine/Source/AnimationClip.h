#pragma once

#include <string>
#include "ResourceAnimation.h"
class AnimationClip
{
public:
	AnimationClip(unsigned int animationUID);


	std::string mName;
	unsigned int mAnimationUID;
	bool loop;

};
