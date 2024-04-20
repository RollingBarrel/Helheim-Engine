#include "Application.h"

#include "GameObject.h"
#include "ButtonComponent.h"
#include "ScriptComponent.h"
#include "Component.h"

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

void ButtonComponent::AddEventHandler(EventType eventType, std::function<void()> handler) {
    eventHandlers[static_cast<int>(eventType)].push_back(handler);
}

void ButtonComponent::TriggerEvent(EventType eventType) {
    for (auto& handler : eventHandlers[static_cast<int>(eventType)]) {
        handler();
    }
}

void ButtonComponent::Save(Archive& archive) const {
    Component::Save(archive);

    // Save event handlers
    std::vector<std::pair<Script*, void(Script::*)()>> handlersInfo;
    for (const auto& handlers : eventHandlers) {
        for (const auto& handler : handlers) {
            auto info = Archive::SaveFunction(handler);
            handlersInfo.push_back(info);
        }
    }
    archive.AddObjectArray("EventHandlers", handlersInfo);
}

void ButtonComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner) {
    Component::LoadFromJSON(data, owner);

    // Load event handlers
    auto handlersArray = archive.GetObjectArray("EventHandlers");
    for (const auto& info : handlersArray) {
        auto objPtr = info.Get<Script*>("ObjectPtr");
        auto memFuncPtr = info.Get<void(Script::*)()>("MemberFuncPtr");
        auto handler = Archive::LoadFunction(objPtr, memFuncPtr);
        // Assuming eventType is defined somewhere
        eventHandlers[static_cast<int>(eventType)].push_back(handler);
    }
}



