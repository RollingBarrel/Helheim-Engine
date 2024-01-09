#include "TestComponent.h"
#include "imgui.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"

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

void TestComponent::Update()
{
	MeshRendererComponent* c = nullptr;
		//c = mOwner->GetComponent<MeshRendererComponent>();
	c = (MeshRendererComponent*)mOwner->GetComponent(ComponentType::MESHRENDERER);
}

void TestComponent::DrawEditor()
{
	
	if (IsComponentOpen()) {
		RightClickPopup(); //Required for the right click popup to work
		ImGui::Text("Demo Text");
		ImGui::Text("Demo Text 2 ");
	}
	else {
		RightClickPopup();
	}
}

Component* TestComponent::Clone(GameObject* owner)
{
    return new TestComponent(*this, owner); //Calls the copy contrustctor of your component
}

void TestComponent::RightClickPopup()
{
	Component::RightClickPopup(); //Required for the right click popup to work


	//Here Add Custom ImGui for your component RightClick Menu
	if (ImGui::BeginPopup(mPopupID)) {
		if (ImGui::MenuItem("Custom Test Component Option")) {
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Custom Test Component Option 2")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

}




