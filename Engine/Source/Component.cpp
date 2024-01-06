#include "Application.h"
#include "Component.h"
#include "imgui.h"
#include "GameObject.h"


int Component::lastcomponentIndex = 0;

Component::Component(GameObject* owner, ComponentType type) : mOwner(owner), mType(type), mIsEnabled(true), mPopupID("ComponentOptions_"+ lastcomponentIndex++)
{
	//mIndex = lastcomponentIndex++;
}

void Component::RightClickPopup() {

	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup(mPopupID);
	}

	if (ImGui::BeginPopup(mPopupID)) {
		if (ImGui::MenuItem("Delete Component")) {
			mOwner->AddComponentToDelete(this);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Reset Component")) {
			Reset();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

}

bool Component::IsComponentOpen()
{
	ImGui::PushID(mPopupID);
	bool isOpen = ImGui::CollapsingHeader(mName, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |ImGuiTreeNodeFlags_AllowItemOverlap);

	ImGui::PopID();

	return isOpen;
}