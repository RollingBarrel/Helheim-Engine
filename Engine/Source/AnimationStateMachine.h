#pragma once
#include <vector>
#include <string>

class AnimationState;
class ResourceAnimation;
class GameObject;
class AnimationTransition;
class AnimationClip;

class AnimationStateMachine
{

public:
	AnimationStateMachine(std::vector<unsigned int> animationUids);
	~AnimationStateMachine();
	void Update();
	void SetLoop(bool loop);

	
	//Clips
	void AddClip(unsigned int animationUID);
	void RemoveClip(int index);

	int  GetClipIndex(std::string& clipName);
	unsigned int GetClipResource(int index);

	//States
	void AddState(std::string& clipName, std::string& name);
	void RemoveState(int index);

	int GetStateIndex(std::string& stateName);
	std::string& GetStateClip(int index);
	std::string& GetStateName(int index);
	void SetStateClip(int index, std::string& clipName);
	void SetStateName(int index, std::string& name);
	int GetNumStates() { return mStates.size();}

	//Transitions
	void AddTransition(std::string& sourceName, std::string& targetName, std::string& trigger);
	void RemoveTransition(int index);

	int GetTransitionIndex(std::string& sourceName, std::string& trigger);
	std::string& GetTransitionTrigger(int index);
	std::string& GetTransitionSource(int index); 
	std::string& GeTransitionTarget(int index);
	int GetnNumTransitions() const{ return mTransitions.size(); };

private:

	bool mLoop = true;
	
	std::vector<AnimationClip> mClips;
	std::vector<AnimationState> mStates;
	std::vector<AnimationTransition> mTransitions;
	
	

};
