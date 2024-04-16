#pragma once

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ProjectPanel.h"
#include "HierarchyPanel.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "imgui.h"

ProjectPanel::ProjectPanel() : Panel(PROJECTPANEL, true)
{
}

ProjectPanel::~ProjectPanel()
{
}

void ProjectPanel::Draw(int windowFlags)
{

	PathNode* root = App->GetFileSystem()->GetRootNode();




	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		ImGui::Columns(2);
		ImGui::BeginChild("Folders");
		DrawAssetsFolder(*root);
		ImGui::EndChild();
		ImGui::NextColumn();
		ImGui::BeginChild("Assets");
		//Paint Assets
		ImGui::EndChild();
	}
	ImGui::End();
}

const void ProjectPanel::DrawAssetsFolder(const PathNode& current) const
{
	std::string bar = "/";
	
	//Discard Meta file but, read .emeta data
	for (auto i = 0; i < current.mChildren.size(); ++i)
	{		
		bool open = ImGui::TreeNodeEx(current.mChildren[i]->mName, ImGuiTreeNodeFlags_DefaultOpen);
		SavePrefab(*current.mChildren[i]);
		if (open)
		{
			for (auto j = 0; j < current.mChildren[i]->assets.size(); ++j)
			{
				if (ImGui::TreeNodeEx(current.mChildren[i]->assets[j]->mName, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
					{
						AssetDisplay* asset = current.mChildren[i]->assets[j];
						App->GetScene()->OpenPrefabScreen(asset->mPath);
					}
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						ImGui::SetDragDropPayload("_SCENE", current.mChildren[i]->assets[j], sizeof(*current.mChildren[i]->assets[j]));

						ImGui::Text(current.mChildren[i]->assets[j]->mName);
						ImGui::EndDragDropSource();
					}
					ImGui::TreePop();
				}
			}
			DrawAssetsFolder(*current.mChildren[i]);
			ImGui::TreePop();
		}
	}
}

void ProjectPanel::SavePrefab(const PathNode& dir) const
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
		{
			HierarchyPanel* hierarchyPanel = (HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL);
			for (auto object : hierarchyPanel->FilterMarked()) {
				std::string file = dir.mName;
				file.append('/' + object->GetName() + ".prfb");
				object->SetPrefabId(App->GetScene()->SavePrefab(object, file.c_str()));
			}
		}
		ImGui::EndDragDropTarget();
	}
}

AssetDisplay::AssetDisplay(const char* name, const char* path, PathNode* parent) : mParent(parent)
{
	unsigned int sizeName = strlen(name) + 1;
	mName = new char[sizeName];
	strcpy_s(const_cast<char*>(mName), sizeName, name);

	unsigned int sizePath = strlen(path) + 1;
	mPath = new char[sizePath];
	strcpy_s(const_cast<char*>(mPath), sizePath, path);
}

AssetDisplay::~AssetDisplay()
{
	delete mName;
	delete mPath;
}


