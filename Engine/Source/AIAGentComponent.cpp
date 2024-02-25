#include "AIAGentComponent.h"

AIAGentComponent::AIAGentComponent(GameObject* ownerGameObject)
 : Component(ownerGameObject, ComponentType::AIAGENT), mMesh(new ResourceMesh()), mMaterial(new ResourceMaterial())
{
}
