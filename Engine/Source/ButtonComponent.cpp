#include "Application.h"

#include "GameObject.h"
#include "ButtonComponent.h"
#include "ScriptComponent.h"
#include "Component.h"
#include "Script.h"

ButtonComponent::ButtonComponent(GameObject* owner, bool active) : Component(owner, ComponentType::BUTTON) 
{
}

ButtonComponent::ButtonComponent(GameObject* owner) : Component(owner, ComponentType::BUTTON) 
{
}

ButtonComponent:: ~ButtonComponent() 
{
}

Component* ButtonComponent::Clone(GameObject* owner) const
{
    return nullptr;
}

/*void ButtonComponent::OnClicked() const
{
    std::vector<Component*> componentList = GetOwner()->GetComponents(ComponentType::SCRIPT);
    for (Component* scriptComponent : componentList)
    {
        Script* script = static_cast<ScriptComponent*>(scriptComponent)->GetScriptInstance();
        if (script != nullptr)
        {
            script->OnButtonClick();
        }
    }
}*/

void ButtonComponent::AddEventHandler(EventType eventType, std::function<void()> handler) {
    eventHandlers[static_cast<int>(eventType)].push_back(handler);
}

void ButtonComponent::TriggerEvent(EventType eventType) {
    for (auto& handler : eventHandlers[static_cast<int>(eventType)]) {
        handler();
    }
}

void ButtonComponent::Save(Archive& archive) const
{
    Component::Save(archive);
}

void ButtonComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    Component::LoadFromJSON(data, owner);
}
