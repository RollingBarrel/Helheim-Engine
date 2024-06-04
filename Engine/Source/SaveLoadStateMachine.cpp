#include "SaveLoadStateMachine.h"
#include "AnimationStateMachine.h"
#include "Application.h"
#include "ModuleFileSystem.h"

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
        size += sizeof(unsigned int) * 3 + sizeof(char) * (transition.mTrigger.length() + 1); //int (mTarget id, mSource id, trigger name length) : char(mTrigger)
    }
    
    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;
    
    for (AnimationClip clip : ourStateMachine->GetClips())
    {
        memcpy(cursor, &clip.mAnimationUID, sizeof(unsigned int));
        cursor += sizeof(unsigned int);
        
        unsigned int namelen = clip.mName.length() +1;
        memcpy(cursor, &namelen, sizeof(unsigned int));
        cursor += sizeof(unsigned int);

        memcpy(cursor, &clip.mName, sizeof(char) * namelen);
        cursor += sizeof(char) * namelen;
    }

    for (AnimationState state : ourStateMachine->GetStates())
    {
        unsigned int clipPos = ourStateMachine->GetClipIndex(state.mClip);
        memcpy(cursor, &clipPos, sizeof(unsigned int));
        cursor += sizeof(unsigned int);

        unsigned int statename = state.mName.length() + 1;
        memcpy(cursor, &statename, sizeof(unsigned int));
        cursor += sizeof(unsigned int);

        memcpy(cursor, &state.mName, sizeof(char) * statename);
        cursor += sizeof(char) * statename;

        memcpy(cursor, &state.mStartTime, sizeof(float));
        cursor += sizeof(float);

        memcpy(cursor, &state.mEndTime, sizeof(float));
        cursor += sizeof(float);

        memcpy(cursor, &state.mLoop, sizeof(bool));
        cursor += sizeof(bool);


    }


    for (AnimationTransition transition : ourStateMachine->GetTransitions())
    {
        unsigned int sourceID = ourStateMachine->GetStateIndex(transition.mSource);
        memcpy(cursor, &sourceID, sizeof(unsigned int));
        cursor += sizeof(unsigned int); 
        
        unsigned int targetID = ourStateMachine->GetStateIndex(transition.mTarget);
        memcpy(cursor, &targetID, sizeof(unsigned int));
        cursor += sizeof(unsigned int);

        unsigned int triggername = transition.mTrigger.length() + 1;
        memcpy(cursor, &triggername, sizeof(unsigned int));
        cursor += sizeof(unsigned int);

        memcpy(cursor, &transition.mTrigger, sizeof(char) * triggername);
        cursor += sizeof(char) * triggername;

    }

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(ourStateMachine->GetUID(), true);
    LOG("State machine:");
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}
