#pragma once
#include "Component.h"
class AudioListenerComponent :
    public Component
{
public:
    AudioListenerComponent(GameObject* ownerGameObject);
    ~AudioListenerComponent();
    
    void Update();
    Component* Clone(GameObject* owner) const;
    void Save(Archive& archive) const;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner);

    void Enable();
    void Disable();

protected:
    void Reset();

};

