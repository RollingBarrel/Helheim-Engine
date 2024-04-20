#pragma once
#include "Component.h"

#include <functional>
#include <vector>

enum class EventType {
    Click,
    Hover,
    Press
};

class ButtonComponent : public Component
{
public:
    ButtonComponent(GameObject* owner);
    ButtonComponent(GameObject* owner, bool active);
    ~ButtonComponent();

    void Reset() override {}
    void Update() override {}
    Component* Clone(GameObject* owner) const override;

    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    void OnClicked() const;
    void TriggerEvent(EventType eventType);
    void AddEventHandler(EventType eventType, std::function<void()> handler);

private:
    std::vector<std::function<void()>> eventHandlers[3];
};
