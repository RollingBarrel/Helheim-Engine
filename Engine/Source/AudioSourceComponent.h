#pragma once
#include "Component.h"

#include "fmod_studio.hpp"

class AudioSourceComponent :
    public Component
{
public:
    AudioSourceComponent(GameObject* ownerGameObject);
    ~AudioSourceComponent();
private:
    FMOD::Studio::EventInstance* currentInstance = nullptr; 

};

