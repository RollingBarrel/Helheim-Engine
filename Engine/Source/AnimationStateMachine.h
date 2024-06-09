#include "Globals.h"
#include <vector>
#include <string>

struct AnimationClip
{
public:
	AnimationClip(unsigned int animationUID) 
	{
		mAnimationUID = animationUID;
		mName = std::string("Default");
	};

	std::string mName;
	unsigned int mAnimationUID;

};

struct AnimationState
{
public:
	AnimationState(const std::string& stateName) {
		mClip = "Default";
		mName = stateName;
		mStartTime = 0.0f;
		mEndTime = 10.0f;
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
	AnimationTransition(const std::string& sourceName, const std::string& targetName, const std::string& trigger) {
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
	AnimationStateMachine();
	~AnimationStateMachine();
	
	//Clips
	void AddClip(unsigned int animationUID);
	void RemoveClip(int index);
	
	void SetClipName(int index, const std::string& name);
	int  GetClipIndex(const std::string& clipName) const;
	const std::string& GetClipName(int index) const;
	unsigned int GetClipResource(int index) const;
	unsigned int GetNumClips() const { return mClips.size(); }

	//States
	void AddState(const std::string& stateName, const std::string& clipName);
	void RemoveState(int index);

	int GetStateIndex(const std::string& stateName) const;
	const std::string& GetStateClip(int index) const;
	const std::string& GetStateName(int index) const;
	float GetStateStartTime(int index) const;
	float GetStateEndTime(int index) const;
	void SetStateClip(int index, const std::string& clipName);
	void SetStateName(int index, const std::string& name);
	void SetStateStartTime(int index, float time);
	void SetStateEndTime(int index, float time);
	unsigned int GetNumStates() const { return mStates.size();}
	bool GetStateLoop(int index) const;
	void SetStateLoop(int index, bool loop);
	void DeleteState(int index);

	//Transitions
	void AddTransition(const std::string& sourceName, const std::string& targetName, const std::string& trigger);
	void RemoveTransition(int index);

	int GetTransitionIndex(const std::string& sourceName, const std::string& trigger) const;
	const std::string& GetTransitionTrigger(int index) const;
	const std::string& GetTransitionSource(int index) const;
	const std::string& GetTransitionTarget(int index) const;
	unsigned int GetNumTransitions() const { return mTransitions.size();};
	void SetTransitionTrigger(int index, const std::string& trigger);
	void DeleteTransition(int index);


	//Save & Load
	const std::vector<AnimationClip>& GetClips() const { return mClips; }
	const std::vector<AnimationState>& GetStates() const { return mStates; }
	const std::vector<AnimationTransition>& GetTransitions() const { return mTransitions; }

	void PushBackClip(const AnimationClip& clip) { mClips.push_back(clip); };
	void PushBackState(const AnimationState& state) { mStates.push_back(state); };
	void PushBackTransition(const AnimationTransition& transition) { mTransitions.push_back(transition); };

	void SaveResource(const char* path) const;
	void LoadResource(const char* fileName);

	void SetName(const std::string& name) { mName = name; }
	const std::string& GetName() const { return mName; }

	unsigned int GetUID() const { return mResourceUID; }
	void SetUID(unsigned int uid) { mResourceUID = uid; }
private:
	
	std::vector<AnimationClip> mClips;
	std::vector<AnimationState> mStates;
	std::vector<AnimationTransition> mTransitions;
	std::string mName = "Simple";
	unsigned int mResourceUID = 0;
	
	

};

