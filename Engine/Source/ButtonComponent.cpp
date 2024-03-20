#include "Application.h"

#include "GameObject.h"
#include "ButtonComponent.h"
#include "ScriptComponent.h"
#include "Component.h"
#include "Script.h"


ButtonComponent::ButtonComponent(GameObject* owner, bool active) : Component(owner, ComponentType::BUTTON) {
}

ButtonComponent::ButtonComponent(GameObject* owner) : Component(owner, ComponentType::BUTTON) {
}

ButtonComponent:: ~ButtonComponent() {
}

Component* ButtonComponent::Clone(GameObject* owner) const
{
    return nullptr;
}

void ButtonComponent::OnClicked() const {
    std::vector<Component*> componentList = GetOwner()->GetComponents(ComponentType::SCRIPT);
    for (Component* scriptComponent : componentList) {
        Script* script = ((ScriptComponent *)scriptComponent)->GetScriptInstance();
        if (script != nullptr) {
            script->OnButtonClick();
        }
    }
}

void ButtonComponent::Save(Archive& archive) const
{
}

void ButtonComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
