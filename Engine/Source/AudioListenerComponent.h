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
    void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

    void Enable();
    void Disable();

protected:
    void Reset();

};

