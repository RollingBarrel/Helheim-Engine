#include "Application.h"

#include "GameObject.h"
#include "ButtonComponent.h"
#include "ImageComponent.h"
#include "ScriptComponent.h"
#include "Component.h"

ButtonComponent::ButtonComponent(GameObject* owner, bool active) : Component(owner, ComponentType::BUTTON) 
{
}

ButtonComponent::ButtonComponent(GameObject* owner) : Component(owner, ComponentType::BUTTON) 
{
}

ButtonComponent::ButtonComponent(const ButtonComponent& component, GameObject* owner) : Component(owner, ComponentType::BUTTON)
{
    for (int i = 0; i < (int)EventType::COUNT; ++i)
    {
        for (const auto& handler : component.mEventHandlers[i]) 
        {
            mEventHandlers[i].push_back(handler);
        }
    }
}

ButtonComponent:: ~ButtonComponent() 
{
    for (int i = 0; i < (int)EventType::COUNT; i++)
    {
        for (int j = 0; j < mEventHandlers->size(); j++) 
        {
            mEventHandlers[i][j] = nullptr;
        }
        mEventHandlers[i].clear();
    }
    mEventHandlers->clear();
}

Component* ButtonComponent::Clone(GameObject* owner) const
{
    return new ButtonComponent(*this, owner);
}

void ButtonComponent::AddEventHandler(EventType eventType, std::function<void()>* handler) 
{
    mEventHandlers[static_cast<int>(eventType)].push_back(handler);
}

void ButtonComponent::TriggerEvent(EventType eventType) 
{
    for (auto& handler : mEventHandlers[static_cast<int>(eventType)]) 
    {
        (*handler)();
    }
}

void ButtonComponent::Save(JsonObject& obj) const 
{
    Component::Save(obj);
}

void ButtonComponent::Load(const JsonObject& data)
{
    Component::Load(data);
}



