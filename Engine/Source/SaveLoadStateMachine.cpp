#include "SaveLoadStateMachine.h"
#include "AnimationStateMachine.h"
#include "Application.h"
#include "ModuleFileSystem.h"

ENGINE_API void Importer::StateMachine::Save(AnimationStateMachine* ourStateMachine)
{
    unsigned int header[3] = { ourStateMachine->GetNumClips(), ourStateMachine->GetNumStates(), ourStateMachine->GetNumTransitions() };
    unsigned int size = sizeof(header);

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

    memcpy(cursor, header, sizeof(header));
    cursor += sizeof(header);
    
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

ENGINE_API AnimationStateMachine* Importer::StateMachine::Load(const char* fileName, unsigned int uid)
{
    char* fileBuffer = nullptr;
    AnimationStateMachine* ourStateMachine = new AnimationStateMachine();

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

        AnimationClip clip = AnimationClip(0);
        AnimationState state = AnimationState(std::string(""));
        AnimationTransition transition = AnimationTransition(std::string(""), std::string(""), std::string(""));
        for (unsigned int i = 0; i < numClips; ++i)
        {
            unsigned int uid = 0;
            memcpy(&uid, cursor, sizeof(unsigned int));
            cursor += sizeof(unsigned int);

            unsigned int namelen = 0;
            memcpy(&namelen, cursor, sizeof(unsigned int));
            cursor += sizeof(unsigned int);

            char* name = new char[namelen];
            memcpy(name, cursor, sizeof(char) * namelen);
            cursor += sizeof(char) * namelen;

            clip.mName = std::string(name);
            clip.mAnimationUID = uid;
            ourStateMachine->PushBackClip(clip);
            delete[] name;
        }

        for (unsigned int i = 0; i<numClips; ++i)
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

            state.mClip = ourStateMachine->GetClipName(clipPos);
            state.mName = std::string(name);
            state.mStartTime = start;
            state.mEndTime = end;
            state.mLoop = loop;
            ourStateMachine->PushBackState(state);
            delete[] name;
        }


        for (unsigned int i = 0; i< numTransitions; ++i)
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

            transition.mSource = ourStateMachine->GetStateName(sourceID);
            transition.mTarget = ourStateMachine->GetStateName(targetID);
            transition.mTrigger = std::string(name);
            delete[] name;
        }
        delete[] fileBuffer;
    }

    return ourStateMachine;
}
