#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ProjectPanel.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "ImporterModel.h"
#include "Math/float2.h"
#include "imgui.h"

ScenePanel::ScenePanel() : Panel(SCENEPANEL, true)
{

}

ScenePanel::~ScenePanel()
{
}

void ScenePanel::Draw(int windowFlags)
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		isWindowsHovered = ImGui::IsWindowHovered();
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (size.x != prevSizeX || size.y != prevSizeY)
		{
			App->GetOpenGL()->SceneFramebufferResized(size.x, size.y);
			prevSizeX = size.x;
			prevSizeY = size.y;
		}
		ImGui::Image((void*)(intptr_t)App->GetOpenGL()->GetFramebufferTexture(), size, ImVec2(0, 1), ImVec2(1, 0));

		mWindowsPosition = float2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		mWindowsSize = float2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		mMousePosition = float2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE"))
			{
				AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);
				char* path = const_cast<char*>(asset->mName);
				//TODO; Molt malament, fer split del path be!!!
				bool done = false;
				while (*path != '\0')
				{
					if (*path == '.')
					{
						*path = '\0';
						done = true;
						break;
					}
					++path;
				}
				ResourceModel* rModel = new ResourceModel();
				Importer::Model::Load(rModel, asset->mName);
				GameObject* nGO = new GameObject(asset->mName,App->GetScene()->GetRoot());
				for (auto it = rModel->mUids.cbegin(); it != rModel->mUids.cend(); ++it)
				{
					GameObject* go = new GameObject(nGO);
					MeshRendererComponent* cMesh = reinterpret_cast<MeshRendererComponent*>(go->CreateComponent(ComponentType::MESHRENDERER));
					cMesh->Load(it->meshUID, it->materiaUID);
					App->GetOpenGL()->AddMeshRendererComponent(cMesh);
				}
				((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(nGO);
				delete rModel;
				if (done)
					*path = '.';
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
}
