#include "Globals.h"
#include <vector>
#include <string>

struct AnimationClip
{
public:
	AnimationClip(unsigned int animationUID);

	std::string mName;
	unsigned int mAnimationUID;
	bool loop = true;

};

struct AnimationState
{
public:
	AnimationState(std::string clipName, std::string name, float start, float end) {
		mName = name;
		mClip = clipName;
		mStartTime = start;
		mEndTime = end;
		mLoop = true;
	};

	std::string mName;
	std::string mClip;
	float mStartTime, mEndTime;
	bool mLoop;

};

struct AnimationTransition
{
public:
	AnimationTransition(std::string sourceName, std::string targetName, std::string trigger) {
		mSource = sourceName;
		mTarget = targetName;
		mTrigger = trigger;
	};
	std::string mTrigger;
	std::string mSource;
	std::string mTarget;
};

class ENGINE_API AnimationStateMachine
{

public:
	AnimationStateMachine(const std::vector<unsigned int>& animationUids);
	~AnimationStateMachine();
	
	//Clips
	void AddClip(unsigned int animationUID);
	void RemoveClip(int index);
	
	void SetClipName(int index, std::string& name);
	const int  GetClipIndex(const std::string& clipName) const;
	const unsigned int GetClipResource(int index) const;

	//States
	void AddState(std::string& clipName, std::string& name);
	void RemoveState(int index);

	const int GetStateIndex(std::string& stateName) const;
	const std::string& GetStateClip(int index) const;
	const std::string& GetStateName(int index) const;
	const float GetStateStartTime(int index) const;
	const float GetStateEndTime(int index) const;
	void SetStateClip(int index, std::string& clipName);
	void SetStateName(int index, std::string& name);
	void SetStateStartTime(int index, float time);
	void SetStateEndTime(int index, float time);
	const int GetNumStates() const { return mStates.size();}

	//Transitions
	void AddTransition(std::string& sourceName, std::string& targetName, std::string& trigger);
	void RemoveTransition(int index);

	const int GetTransitionIndex(std::string& sourceName, std::string& trigger) const;
	const std::string& GetTransitionTrigger(int index) const;
	const std::string& GetTransitionSource(int index) const;
	const std::string& GeTransitionTarget(int index) const;
	const unsigned int GetnNumTransitions() const { return mTransitions.size();};

private:

	bool mLoop = true;
	
	std::vector<AnimationClip> mClips;
	std::vector<AnimationState> mStates;
	std::vector<AnimationTransition> mTransitions;
	
	

};

