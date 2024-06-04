#include "SaveLoadStateMachine.h"
#include "AnimationStateMachine.h"

ENGINE_API void Importer::StateMachine::Save(AnimationStateMachine* ourStateMachine)
{
    unsigned int size = 0;
    for (AnimationState state : ourStateMachine->GetStates())
    {
        size += sizeof(float) * 2 + sizeof(unsigned int) * 2 + sizeof(bool); // float (mStartTime, mEndTime) : int(name len, clip index) : bool(mLoop)
        size += sizeof(char) * (state.mName.length() + 1); // char (mName)
    }

    for (AnimationClip clip : ourStateMachine->GetClips())
    {
        size += sizeof(unsigned int) * 2 + sizeof(char) * (clip.mName.length() + 1); // int (mUid, name len) : char  (mName)
    }

    for (AnimationTransition transition : ourStateMachine->GetTransitions())
    {
        size += sizeof(int) * 3 + sizeof(char) * (transition.mTrigger.length() + 1); //int (mTarget id, mSource id, trigger name length) : char(mTrigger)
    }
    
    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;


}
