#include "ScenePanel.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ProjectPanel.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "ImporterModel.h"

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
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (size.x != prevSizeX || size.y != prevSizeY)
		{
			App->GetOpenGL()->SceneFramebufferResized(size.x, size.y);
			prevSizeX = size.x;
			prevSizeY = size.y;
		}
		ImGui::Image((void*)(intptr_t)App->GetOpenGL()->GetFramebufferTexture(), size, ImVec2(0, 1), ImVec2(1, 0));

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
				for (auto it = rModel->mUids.cbegin(); it != rModel->mUids.cend(); ++it)
				{

					GameObject* go = new GameObject(App->GetScene()->GetRoot());
					MeshRendererComponent* cMesh = reinterpret_cast<MeshRendererComponent*>(go->CreateComponent(ComponentType::MESHRENDERER));
					cMesh->Load(it->meshUID, it->materiaUID);
				}
				delete rModel;
				if (done)
					*path = '.';
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
}
