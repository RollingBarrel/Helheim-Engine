#include "ScenePanel.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleFileSystem.h"
#include "ProjectPanel.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "ImporterModel.h"

#include "ResourceModel.h"

#include "imgui.h"

#include "json.hpp"

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
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE"))
			{
				AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);

				std::string path = asset->mPath;
				path += ".meta";

				char* fileBuffer = nullptr;
				App->GetFileSystem()->Load(path.c_str(), &fileBuffer);

				rapidjson::Document document;
				rapidjson::ParseResult result = document.Parse(fileBuffer);
				if (!result) {
					// Handle parsing error
					LOG("Not able to load .meta file");
				}

				Resource* resource = nullptr;
				unsigned int uid;
				if (document.HasMember("uid"))
				{
					uid = document["uid"].GetInt();
				}
				unsigned int type;
				if (document.HasMember("type"))
				{
					type = document["type"].GetInt();		
				}

				switch (static_cast<Resource::Type>(type))
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
					resource = Importer::Model::Load(std::string(LIBRARY_MODEL_PATH + std::to_string(uid) + ".model").c_str(), uid);
					break;
				case Resource::Type::Scene:
					break;
				case Resource::Type::NavMesh:
					break;
				}

				GameObject* nGO = new GameObject(asset->mName,App->GetScene()->GetRoot());
				for (auto it = ((ResourceModel*)resource)->GetUids().cbegin(); it != ((ResourceModel*)resource)->GetUids().cend(); ++it)
				{
					GameObject* go = new GameObject(nGO);
					MeshRendererComponent* cMesh = reinterpret_cast<MeshRendererComponent*>(go->CreateComponent(ComponentType::MESHRENDERER));
					cMesh->Load(it->meshUID, it->materialUID);
				}

				delete resource;
				RELEASE_ARRAY(fileBuffer);
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();
}
