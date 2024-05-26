#pragma once
#include "Component.h"
class AudioListenerComponent :
    public Component
{
public:
    AudioListenerComponent(GameObject* ownerGameObject);
    AudioListenerComponent(const AudioListenerComponent& original, GameObject* ownerGameObject);
    ~AudioListenerComponent();
    
    void Update();
    Component* Clone(GameObject* owner) const;
    void Save(JsonObject& obj) const override;
    void Load(const JsonObject& data, GameObject* owner) override;

    void Enable();
    void Disable();

protected:
    void Reset();

};

