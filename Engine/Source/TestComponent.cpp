#include "TestComponent.h"
#include "imgui.h"

TestComponent::TestComponent(GameObject* ownerGameObject) 
	:Component(ownerGameObject, ComponentType::TEST)
{
	
}

TestComponent::TestComponent(const TestComponent& original)
	:Component(original.GetOwner(), ComponentType::TEST)
{
	
}
void TestComponent::Reset() {

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
    Draw();
}

void TestComponent::DrawEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
		// SIMULATED CONTENT FOR TEST PURPOSES:
		ImGui::Text("Color: (R: 255, G: 0, B: 0) (TEST)");
		ImGui::Text("Texture: DefaultTexture (TEST)");
	}

	if (this->GetOwner()) {
		this->GetOwner()->DeletePopup(this, 38);
	}
}

Component* TestComponent::Clone()
{
    return new TestComponent(*this);
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
