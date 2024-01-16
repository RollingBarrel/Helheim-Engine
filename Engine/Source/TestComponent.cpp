#include "TestComponent.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "imgui.h"

TestComponent::TestComponent(GameObject* ownerGameObject) 
	:Component("Test Component", ownerGameObject, ComponentType::TEST)
{
	
}

TestComponent::TestComponent(const TestComponent& original, GameObject* owner)
	:Component(original.mName, owner, ComponentType::TEST)
{

}
void TestComponent::Reset() {
	//Change variables to default values.
	number = 0;
}

void TestComponent::Draw()
{

}

void TestComponent::Load()
{
    LoadVBO();
}

void TestComponent::Update()
{
	MeshRendererComponent* c = nullptr;
		//c = mOwner->GetComponent<MeshRendererComponent>();
	c = (MeshRendererComponent*)mOwner->GetComponent(ComponentType::MESHRENDERER);
	Draw();
}

Component* TestComponent::Clone(GameObject* owner)
{
    return new TestComponent(*this, owner); //Calls the copy contrustctor of your component
}

void TestComponent::Save(Archive& archive) const {
	archive.AddString("Name", mName);
	archive.AddString("type", "Test");
}

void TestComponent::Load(const rapidjson::Value& data) {
}

void TestComponent::LoadVBO()
{

}

void TestComponent::LoadEBO()
{
}

void TestComponent::LoadVAO()
{
}
