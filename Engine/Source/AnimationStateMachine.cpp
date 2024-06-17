#include "AnimationStateMachine.h"
#include "ResourceAnimation.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResource.h"
#include "AnimationController.h"
#include "ModuleFileSystem.h"


#define ASSETS_PATH_STATEMACHINE "Assets/StateMachines/"

AnimationStateMachine::AnimationStateMachine(const std::vector<unsigned int>& animationUids)
{
	for (const auto& resourceAnimation : animationUids)
	{
		mClips.push_back(AnimationClip(resourceAnimation));
	}
	mStates.push_back(AnimationState(std::string("Default")));
}

AnimationStateMachine::AnimationStateMachine()
{
}

AnimationStateMachine::~AnimationStateMachine()
{
}

void AnimationStateMachine::AddClip(unsigned int animationUID)
{
	mClips.push_back(AnimationClip(animationUID));
}

void AnimationStateMachine::RemoveClip(int index)
{
	assert(!(index >= mClips.size()));
	mClips.erase(mClips.begin()+index);
}

void AnimationStateMachine::SetClipName(int index, const std::string& name)
{
	assert(!(index >= mClips.size()));
	mClips[index].mName = name;
}

int AnimationStateMachine::GetClipIndex(const std::string& clipName) const
{
	int index = -1;
	for (size_t i = 0; i < mClips.size(); i++)
	{
		if (clipName == mClips[i].mName)
		{
			index = i;
			break;
		}
	}
	return index;
}

const std::string& AnimationStateMachine::GetClipName(int index) const
{
	// // O: insert return statement here
	assert(!(index >= mClips.size()));
	return mClips[index].mName;
}

unsigned int AnimationStateMachine::GetClipResource(int index) const
{
	assert(!(index >= mClips.size()));
	return mClips[index].mAnimationUID;
}

void AnimationStateMachine::AddState(const std::string& stateName, const std::string& clipName)
{
	int resource_idx = GetClipResource(GetClipIndex(clipName));
	AnimationState newState = AnimationState(stateName);
	mStates.push_back(newState);
}

void AnimationStateMachine::RemoveState(int index)
{
	assert(!(index >= mStates.size()));
	mStates.erase(mStates.begin() + index);
}

int AnimationStateMachine::GetStateIndex(const std::string& stateName) const
{
	int index = -1;

	for (size_t i = 0; i < mStates.size(); i++)
	{
		if (stateName == mStates[i].mName)
		{
			index = i;
			break;
		}

	}
	return index;
}

const std::string& AnimationStateMachine::GetStateClip(int index) const
{
	assert(!(index >= mStates.size()));
	return mStates[index].mClip;
}

const std::string& AnimationStateMachine::GetStateName(int index) const
{
	assert(!(index >= mStates.size()));
	return mStates[index].mName;
}

float AnimationStateMachine::GetStateStartTime(int index) const
{
	assert(!(index >= mStates.size()));
	return mStates[index].mStartTime;
}

float AnimationStateMachine::GetStateEndTime(int index) const
{
	assert(!(index >= mStates.size()));
	return mStates[index].mEndTime;
}

void AnimationStateMachine::SetStateClip(int index, const std::string& clipName)
{
	assert(!(index >= mStates.size()));
	mStates[index].mClip = clipName;
}

void AnimationStateMachine::SetStateName(int index, const std::string& stateName)
{
	assert(!(index >= mStates.size()));
	mStates[index].mName = stateName;
}

void AnimationStateMachine::SetStateStartTime(int index, float time)
{
	assert(!(index >= mStates.size()));
	mStates[index].mStartTime = time;
}

void AnimationStateMachine::SetStateEndTime(int index, float time)
{
	assert(!(index >= mStates.size()));
	mStates[index].mEndTime = time;
}

bool AnimationStateMachine::GetStateLoop(int index) const
{
	assert(!(index >= mStates.size()));
	return mStates[index].mLoop;
}

void AnimationStateMachine::SetStateLoop(int index, bool loop)
{
	assert(!(index >= mStates.size()));
	mStates[index].mLoop = loop;
}

void AnimationStateMachine::DeleteState(int index)
{
	assert(!(index >= mStates.size()));
	std::string state = mStates[index].mName;
	for (int i = 0; i<mTransitions.size(); ++i)
	{
		if (mTransitions[i].mSource == state || mTransitions[i].mTarget == state)
		{
			DeleteTransition(i);
		}
	}

	mStates.erase(mStates.begin() + index);
}

void AnimationStateMachine::AddTransition(const std::string& sourceName, const std::string& targetName, const std::string& trigger)
{
	AnimationTransition newTransition = AnimationTransition(sourceName, targetName, trigger);
	mTransitions.push_back(newTransition);
}

void AnimationStateMachine::RemoveTransition(int index)
{
	assert(!(index >= mTransitions.size()));
	mTransitions.erase(mTransitions.begin() + index);
}

int AnimationStateMachine::GetTransitionIndex(const std::string& sourceName, const std::string& trigger) const
{
	int index = -1;

	for (size_t i = 0; i < mTransitions.size(); i++)
	{
		if (sourceName == mTransitions[i].mSource  && sourceName == mTransitions[i].mTrigger) 
		{
			index = i;
			break;

		}
	}
	return index;
}

const std::string& AnimationStateMachine::GetTransitionTrigger(int index) const
{
	assert(!(index >= mTransitions.size()));
	return mTransitions[index].mTrigger;
}

const std::string& AnimationStateMachine::GetTransitionSource(int index) const
{
	assert(!(index >= mTransitions.size()));
	return mTransitions[index].mSource;
}

const std::string& AnimationStateMachine::GetTransitionTarget(int index) const
{
	assert(!(index >= mTransitions.size()));
	return mTransitions[index].mTarget;
}

void AnimationStateMachine::SetTransitionTrigger(int index, const std::string& trigger)
{
	assert(!(index >= mTransitions.size()));
	mTransitions[index].mTrigger = trigger;
}

void AnimationStateMachine::DeleteTransition(int index)
{
	assert(!(index >= mTransitions.size()));
	mTransitions.erase(mTransitions.begin() + index);
}

void AnimationStateMachine::SaveResource(const char* path) const
{
	unsigned int header[3] = { GetNumClips(), GetNumStates(), GetNumTransitions() };
	unsigned int size = sizeof(header);

	size += sizeof(unsigned int) + sizeof(char) * (mName.length() + 1);

	for (AnimationState state : GetStates())
	{
		size += sizeof(float) * 2 + sizeof(unsigned int) * 2 + sizeof(bool); // float (mStartTime, mEndTime) : int(name len, clip index) : bool(mLoop)
		size += sizeof(char) * (state.mName.length() + 1); // char (mName)
	}

	for (AnimationClip clip : GetClips())
	{
		size += sizeof(unsigned int) * 2 + sizeof(char) * (clip.mName.length() + 1); // int (mUid, name len) : char  (mName)
	}

	for (AnimationTransition transition : GetTransitions())
	{
		size += sizeof(unsigned int) * 3 + sizeof(char) * (transition.mTrigger.length() + 1); //int (mTarget id, mSource id, trigger name length) : char(mTrigger)
	}

	char* fileBuffer = new char[size];
	char* cursor = fileBuffer;

	memcpy(cursor, header, sizeof(header));
	cursor += sizeof(header);

	unsigned int smname = mName.length() + 1;
	memcpy(cursor, &smname, sizeof(unsigned int));
	cursor += sizeof(int);

	memcpy(cursor, mName.c_str(), sizeof(char) * smname);
	cursor += sizeof(char) * smname;

	for (AnimationClip clip : GetClips())
	{
		memcpy(cursor, &clip.mAnimationUID, sizeof(unsigned int));
		cursor += sizeof(unsigned int);

		unsigned int namelen = clip.mName.length() + 1;
		memcpy(cursor, &namelen, sizeof(unsigned int));
		cursor += sizeof(unsigned int);

		memcpy(cursor, clip.mName.c_str(), sizeof(char) * namelen);
		cursor += sizeof(char) * namelen;
	}

	for (AnimationState state : GetStates())
	{
		unsigned int clipPos = GetClipIndex(state.mClip);
		memcpy(cursor, &clipPos, sizeof(unsigned int));
		cursor += sizeof(unsigned int);

		unsigned int statename = state.mName.length() + 1;
		memcpy(cursor, &statename, sizeof(unsigned int));
		cursor += sizeof(unsigned int);

		memcpy(cursor, state.mName.c_str(), sizeof(char) * statename);
		cursor += sizeof(char) * statename;

		memcpy(cursor, &state.mStartTime, sizeof(float));
		cursor += sizeof(float);

		memcpy(cursor, &state.mEndTime, sizeof(float));
		cursor += sizeof(float);

		memcpy(cursor, &state.mLoop, sizeof(bool));
		cursor += sizeof(bool);


	}


	for (AnimationTransition transition : GetTransitions())
	{
		unsigned int sourceID = GetStateIndex(transition.mSource);
		memcpy(cursor, &sourceID, sizeof(unsigned int));
		cursor += sizeof(unsigned int);

		unsigned int targetID = GetStateIndex(transition.mTarget);
		memcpy(cursor, &targetID, sizeof(unsigned int));
		cursor += sizeof(unsigned int);

		unsigned int triggername = transition.mTrigger.length() + 1;
		memcpy(cursor, &triggername, sizeof(unsigned int));
		cursor += sizeof(unsigned int);

		memcpy(cursor, transition.mTrigger.c_str(), sizeof(char) * triggername);
		cursor += sizeof(char) * triggername;

	}

	std::string strpath = std::string(path);
	strpath += mName;
	strpath += ".smbin";
	App->GetFileSystem()->Save(strpath.c_str(), fileBuffer, size);

	delete[] fileBuffer;
}

void AnimationStateMachine::LoadResource(const char* fileName)
{
	mClips.clear();
	mStates.clear();
	mTransitions.clear();

	char* fileBuffer = nullptr;
	if (App->GetFileSystem()->Load(fileName, &fileBuffer))
	{
		char* cursor = fileBuffer;
		unsigned int header[3];
		unsigned int bytes = sizeof(header);
		memcpy(header, cursor, bytes);
		cursor += bytes;
		unsigned int numClips = header[0];
		unsigned int numStates = header[1];
		unsigned int numTransitions = header[2];

		unsigned int namelen = 0;
		memcpy(&namelen, cursor, sizeof(unsigned int));
		cursor += sizeof(unsigned int);

		char* smname = new char[namelen];
		memcpy(smname, cursor, sizeof(char) * namelen);
		cursor += sizeof(char) * namelen;
		mName = std::string(smname);
		

		AnimationClip clip = AnimationClip(0);
		AnimationState state = AnimationState(std::string(""));
		AnimationTransition transition = AnimationTransition(std::string(""), std::string(""), std::string(""));
		for (unsigned int i = 0; i < numClips; ++i)
		{
			unsigned int c_uid = 0;
			memcpy(&c_uid, cursor, sizeof(unsigned int));
			cursor += sizeof(unsigned int);

			unsigned int namelen = 0;
			memcpy(&namelen, cursor, sizeof(unsigned int));
			cursor += sizeof(unsigned int);

			char* name = new char[namelen];
			memcpy(name, cursor, sizeof(char) * namelen);
			cursor += sizeof(char) * namelen;

			clip.mName = std::string(name);
			clip.mAnimationUID = c_uid;
			PushBackClip(clip);
			delete[] name;
		}

		for (unsigned int i = 0; i < numStates; ++i)
		{
			unsigned int clipPos = 0;
			memcpy(&clipPos, cursor, sizeof(unsigned int));
			cursor += sizeof(unsigned int);

			unsigned int namelen = 0;
			memcpy(&namelen, cursor, sizeof(unsigned int));
			cursor += sizeof(unsigned int);

			char* name = new char[namelen];
			memcpy(name, cursor, sizeof(char) * namelen);
			cursor += sizeof(char) * namelen;

			float start = 0.0f;
			memcpy(&start, cursor, sizeof(float));
			cursor += sizeof(float);

			float end = 0.0f;
			memcpy(&end, cursor, sizeof(float));
			cursor += sizeof(float);

			bool loop = true;
			memcpy(&loop, cursor, sizeof(bool));
			cursor += sizeof(bool);

			state.mClip = GetClipName(clipPos);
			state.mName = std::string(name);
			state.mStartTime = start;
			state.mEndTime = end;
			state.mLoop = loop;
			PushBackState(state);
			delete[] name;
		}


		for (unsigned int i = 0; i < numTransitions; ++i)
		{
			unsigned int sourceID = 0;
			memcpy(&sourceID, cursor, sizeof(unsigned int));
			cursor += sizeof(unsigned int);

			unsigned int targetID = 0;
			memcpy(&targetID, cursor, sizeof(unsigned int));
			cursor += sizeof(unsigned int);

			unsigned int namelen = 0;
			memcpy(&namelen, cursor, sizeof(unsigned int));
			cursor += sizeof(unsigned int);

			char* name = new char[namelen];
			memcpy(name, cursor, sizeof(char) * namelen);
			cursor += sizeof(char) * namelen;

			if (sourceID >= numStates || targetID >= numStates)
			{
				assert(false); // Hi, you have a corrupted state machine, make sure all transitions exist for the state machine "smname"!
				//Specially check the transitions with trigger_name = "name".
				continue;
			}

			transition.mSource = GetStateName(sourceID);
			transition.mTarget = GetStateName(targetID);
			transition.mTrigger = std::string(name);
			PushBackTransition(transition);
			delete[] name;
		}
		delete[] smname;
		delete[] fileBuffer;
	}

}

