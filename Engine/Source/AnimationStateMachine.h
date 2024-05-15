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
	AnimationStateMachine(std::vector<unsigned int> animationUids);
	~AnimationStateMachine();
	void Update();
	void SetLoop(bool loop);

	
	//Clips
	void AddClip(unsigned int animationUID);
	void RemoveClip(int index);
	
	void SetClipName(int index, std::string& name);
	int  GetClipIndex(std::string& clipName);
	unsigned int GetClipResource(int index);

	//States
	void AddState(std::string& clipName, std::string& name);
	void RemoveState(int index);

	int GetStateIndex(std::string& stateName);
	std::string& GetStateClip(int index);
	std::string& GetStateName(int index);
	float GetStateStartTime(int index);
	float GetStateEndTime(int index);
	void SetStateClip(int index, std::string& clipName);
	void SetStateName(int index, std::string& name);
	void SetStateStartTime(int index, float time);
	void SetStateEndTime(int index, float time);
	int GetNumStates() { return mStates.size();}

	//Transitions
	void AddTransition(std::string& sourceName, std::string& targetName, std::string& trigger);
	void RemoveTransition(int index);

	int GetTransitionIndex(std::string& sourceName, std::string& trigger);
	std::string& GetTransitionTrigger(int index);
	std::string& GetTransitionSource(int index); 
	std::string& GeTransitionTarget(int index);
	unsigned int GetnNumTransitions() { return mTransitions.size();};

private:

	bool mLoop = true;
	
	std::vector<AnimationClip> mClips;
	std::vector<AnimationState> mStates;
	std::vector<AnimationTransition> mTransitions;
	
	

};

