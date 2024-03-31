#pragma once
#include "Component.h"
class AudioListenerComponent :
    public Component
{
    AudioListenerComponent(GameObject* ownerGameObject);
    ~AudioListenerComponent();
};

