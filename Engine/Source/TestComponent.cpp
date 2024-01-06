#include "TestComponent.h"
#include "imgui.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"

TestComponent::TestComponent(GameObject* ownerGameObject) 
	:Component(ownerGameObject, ComponentType::TEST)
{
	mName = "Test Component";
}

TestComponent::TestComponent(const TestComponent& original)
	:Component(original.GetOwner(), ComponentType::TEST)
{
	mName = "Test Component";
}
void TestComponent::Reset() {
	//Change Variables 

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

Component* TestComponent::Clone()
{
    return new TestComponent(*this);
}

void TestComponent::RightClickPopup()
{
	Component::RightClickPopup(); //Required for the right click popup to work


	//Here Add Custom Options
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




