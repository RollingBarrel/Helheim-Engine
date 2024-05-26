#pragma once
#include "Component.h"

#include <functional>
#include <vector>

enum class EventType 
{
    CLICK,
    HOVER,
    HOVEROFF,
    PRESS,
    COUNT
};

class ENGINE_API ButtonComponent : public Component
{
public:
    ButtonComponent(GameObject* owner);
    ButtonComponent(GameObject* owner, bool active);
    ButtonComponent(const ButtonComponent& component, GameObject* owner);
    ~ButtonComponent();

    void Reset() override {}
    void Update() override {}
    Component* Clone(GameObject* owner) const override;

    bool GetHovered() { return mHovered; }
    void SetHovered(bool hovered) { mHovered = hovered; }

    void Save(JsonObject& obj) const override;
    void Load(const JsonObject& data, GameObject* owner) override;

    void TriggerEvent(EventType eventType);
    void AddEventHandler(EventType eventType, std::function<void()>* handler);

private:
    std::vector<std::function<void()>*> mEventHandlers[(int)EventType::COUNT];
    bool mHovered = false;
};
