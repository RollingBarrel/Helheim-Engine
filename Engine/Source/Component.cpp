#include "Component.h"
#include "imgui.h"
#include "GameObject.h"

int Component::lastcomponentIndex = 0;

Component::Component(GameObject* owner, ComponentType type) : mOwner(owner), mType(type), mIsEnabled(true)
{
	componentIndex = lastcomponentIndex++;
}

bool Component::IsComponentOpen()
{
	ImGui::PushID(componentIndex);
	bool isOpen = ImGui::CollapsingHeader(componentName, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |ImGuiTreeNodeFlags_AllowItemOverlap);
	
	if (GetOwner()) {
		GetOwner()->DeletePopup(this);
	}

	ImGui::PopID();

	return isOpen;
}