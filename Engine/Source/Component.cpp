#include "Component.h"
#include "GameObject.h"

//CLASS FOR TESTING PURPOSE
#include "MeshRendererComponent.h"

Component* Component::CreateComponent(ComponentType type, GameObject* owner) {
    Component* newComponent = nullptr;

    // Creating the component according to its type
    switch (type) {
    case ComponentType::MESHRENDERER:
        //ONLY FOR TEST PURPOSE
        newComponent = new MeshRendererComponent();

        // Assigns the type to the Component
        newComponent->mType = type;
        break;
    case ComponentType::MATERIAL:
        //ONLY FOR TEST PURPOSE            
        newComponent = new MeshRendererComponent();
        newComponent->mType = type;
        break;
    default:
        break;
    }

    if (newComponent) {
        newComponent->SetOwner(owner);
        // Add the component to the owner GameObject
        owner->AddComponent(newComponent);
    }

    return newComponent;
}

void Component::DeleteComponent(Component* component) {
    if (component) {
        GameObject* owner = component->Owner;
        if (owner) {
            // Delete the component from the GameObject
            owner->RemoveComponent(component);
        }
        delete component;
    }
}