#include "ScenePanel.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "ProjectPanel.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "ImporterModel.h"
#include "ResourceModel.h"

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

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE");
			if (payload)
			{
				AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);

				Resource* resource = App->GetResource()->RequestResource(asset->mPath);
				if (resource)
				{
					switch (resource->GetType())
					{
					case Resource::Type::Texture:
						break;
					case Resource::Type::Mesh:
						break;
					case Resource::Type::Bone:
						break;
					case Resource::Type::Animation:
						break;
					case Resource::Type::Material:
						break;
					case Resource::Type::Model:
					{
						GameObject* nGO = new GameObject(asset->mName, App->GetScene()->GetRoot());
						for (auto it = reinterpret_cast<ResourceModel*>(resource)->GetUids().cbegin(); it != reinterpret_cast<ResourceModel*>(resource)->GetUids().cend(); ++it)
						{
							GameObject* go = new GameObject(nGO);
							MeshRendererComponent* cMesh = reinterpret_cast<MeshRendererComponent*>(go->CreateComponent(ComponentType::MESHRENDERER, it->meshUID, it->materialUID));
						}
						App->GetResource()->ReleaseResource(resource->GetUID());
						break;
					}
					case Resource::Type::Scene:
						break;
					case Resource::Type::NavMesh:
						break;
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
}
