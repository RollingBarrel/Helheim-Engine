#include "GameObject.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "InspectorPanel.h"
#include "imgui.h"
#include "Component.h"

void GameObject::DrawEditor() {
	const InspectorPanel* inspector = App->GetEditor()->GetInspectorPanel();
	if (inspector->IsOpen()) {
		if (ImGui::Begin(inspector->GetName())) {
			// Draw Transform Component

			for (Component* component : mComponents) {
				component->DrawEditor();
			}
		}
		ImGui::End();
	}
}