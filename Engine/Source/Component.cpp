#include "Component.h"
#include "imgui.h"
#include "GameObject.h"

int Component::lastcomponentIndex = 0;

Component::Component(GameObject* owner, ComponentType type) : mOwner(owner), mType(type), mIsEnabled(true)
{
	mIndex = lastcomponentIndex++;
}

bool Component::IsComponentOpen()
{
	ImGui::PushID(mIndex);
	bool isOpen = ImGui::CollapsingHeader(mName, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |ImGuiTreeNodeFlags_AllowItemOverlap);
	
	if (GetOwner()) {
		GetOwner()->ComponentRightClickPopup(this);
	}

	ImGui::PopID();

	return isOpen;
}