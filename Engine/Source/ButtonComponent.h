#pragma once
#include "Component.h"

#include <functional>
#include <vector>

class ImageComponent;
class Transform2DComponent;

enum class EventType 
{
    CLICK,
    HOVER,
    HOVEROFF,
    PRESS,
    COUNT,
    NONE,
};

class ENGINE_API ButtonComponent : public Component
{
public:
    ButtonComponent(GameObject* owner);
    ButtonComponent(GameObject* owner, bool active);
    ButtonComponent(const ButtonComponent& component, GameObject* owner);
    ~ButtonComponent();

    void Reset() override {}
    void Update();
    Component* Clone(GameObject* owner) const override;

    void Save(JsonObject& obj) const override;
    void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

    void TriggerEvent(EventType eventType);
    void AddEventHandler(EventType eventType, std::function<void()>* handler);

private:
    std::vector<std::function<void()>*> mEventHandlers[(int)EventType::COUNT];

    EventType mCurrentEvent = EventType::NONE;

    ImageComponent* mImage = nullptr;
    Transform2DComponent* mTransform2D = nullptr;
};
