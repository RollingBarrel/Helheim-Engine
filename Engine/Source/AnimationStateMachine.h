#include "Globals.h"
#include <vector>
#include <string>

struct AnimationClip
{
public:
	AnimationClip(unsigned int animationUID) 
	{
		mAnimationUID = animationUID;
	};

	std::string mName;
	unsigned int mAnimationUID;

};

struct AnimationState
{
public:
	AnimationState(std::string clipName, int id) {
		mClip = clipName;
		mStartTime = 0.0f;
		mEndTime = 10.0f;
		mLoop = true;
		mEditorId = id;
	};

	std::string mName;
	std::string mClip;
	float mStartTime, mEndTime;
	bool mLoop;
	int mEditorId;

};

struct AnimationTransition
{
public:
	AnimationTransition(std::string sourceName, std::string targetName, std::string trigger, int id, int sourceId, int tragetId) {
		mSource = sourceName;
		mTarget = targetName;
		mTrigger = trigger;
		mEditorId = id;
		mSourceId = sourceId;
		mTargetId = tragetId;
	};
	std::string mTrigger;
	std::string mSource;
	std::string mTarget;
	int mEditorId;
	int mSourceId;
	int mTargetId;
};

class ENGINE_API AnimationStateMachine
{

public:
	AnimationStateMachine(std::vector<unsigned int> animationUids);
	~AnimationStateMachine();
	
	//Clips
	void AddClip(unsigned int animationUID);
	void RemoveClip(int index);
	
	void SetClipName(int index, std::string& name);
	const int  GetClipIndex(const std::string& clipName) const;
	const unsigned int GetClipResource(int index) const;

	//States
	const AnimationState& AddState(std::string& clipName);
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
	const AnimationTransition& AddTransition(std::string& sourceName, std::string& targetName, std::string& trigger);
	void RemoveTransition(int index);

	const int GetTransitionIndex(std::string& sourceName, std::string& trigger) const;
	const std::string& GetTransitionTrigger(int index) const;
	const std::string& GetTransitionSource(int index) const;
	const std::string& GeTransitionTarget(int index) const;
	const unsigned int GetnNumTransitions() const { return mTransitions.size();};
	const std::string& GetJsonConfig() const { return mJsonConfig; }


	const std::vector<AnimationState>& GetStates() const { return mStates; }
	const std::vector<AnimationTransition>& GetTransitions() const { return mTransitions; }
private:
	
	std::vector<AnimationClip> mClips;
	std::vector<AnimationState> mStates;
	std::vector<AnimationTransition> mTransitions;
	std::string mJsonConfig = "Simple.json";
	int mCurrentId = 0;
	
	

};

