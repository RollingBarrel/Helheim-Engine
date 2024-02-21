#include "TestComponent.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"

TestComponent::TestComponent(GameObject* ownerGameObject) 
	:Component(ownerGameObject, ComponentType::TEST)
{
	
}

TestComponent::TestComponent(const TestComponent& original, GameObject* owner)
	:Component(owner, ComponentType::TEST)
{

}
void TestComponent::Reset() {
	//Change variables to default values.
	number = 0;
}

void TestComponent::Update()
{
	MeshRendererComponent* c = nullptr;
		//c = mOwner->GetComponent<MeshRendererComponent>();
	c = (MeshRendererComponent*)mOwner->GetComponent(ComponentType::MESHRENDERER);
}

Component* TestComponent::Clone(GameObject* owner) const
{
    return new TestComponent(*this, owner); //Calls the copy contrustctor of your component
}

void TestComponent::Save(Archive& archive) const {
	archive.AddString("type", GetNameFromOwnType());
}

void TestComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner) {
	
}