#include "Globals.h"
#include <vector>
#include <string>


struct AnimationState
{
public:
	AnimationState(const std::string& stateName) {
		mName = stateName;
		mStartTime = 0.0f;
		mEndTime = 100.0f; 
		mLoop = true;
	};

	std::string mName;
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
	AnimationStateMachine(unsigned int animUID);
	~AnimationStateMachine();

	//States
	void AddState(const std::string& stateName);
	void RemoveState(int index);

	int GetStateIndex(const std::string& stateName) const;
	const std::string& GetStateName(int index) const;
	float GetStateStartTime(int index) const;
	float GetStateEndTime(int index) const;
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
	void SetTransitionSource(int index, const std::string& sourceName);
	void SetTransitionTarget(int index, const std::string& targetName);


	//Save & Load
	const std::vector<AnimationState>& GetStates() const { return mStates; }
	const std::vector<AnimationTransition>& GetTransitions() const { return mTransitions; }

	void PushBackState(const AnimationState& state) { mStates.push_back(state); };
	void PushBackTransition(const AnimationTransition& transition) { mTransitions.push_back(transition); };

	void SaveResource(const char* path, bool isLibrary) const;
	void LoadResource(const char* fileName);

	void SetName(const std::string& name) { mName = name; }
	const std::string& GetName() const { return mName; }

	unsigned int GetUID() const { return mResourceUID; }
	void SetUID(unsigned int uid) { mResourceUID = uid; }
private:
	
	std::vector<AnimationState> mStates;
	std::vector<AnimationTransition> mTransitions;
	std::string mName = "Simple";
	unsigned int mResourceUID = 0;
	unsigned int mAnimationUID = 0;
	
	

};

