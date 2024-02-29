#pragma once

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ProjectPanel.h"

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
		if (ImGui::BeginChild("Favs##"))
			for (auto i = 0; i < root->mChildren.size(); ++i)
			{
				DrawAssetsFolder(*root);
			}
		ImGui::EndChild();

		if (ImGui::BeginChild("Assets"))
		{
			ImGui::Button("HI");
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

const void ProjectPanel::DrawAssetsFolder(const PathNode& current) const
{
	//Discard Meta file but, read .emeta data
	for (auto i = 0; i < current.mChildren.size(); ++i)
	{
		if (ImGui::TreeNodeEx(current.mChildren[i]->mName, ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto j = 0; j < current.mChildren[i]->assets.size(); ++j)
			{
				if (ImGui::TreeNodeEx(current.mChildren[i]->assets[j]->mName, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf))
				{
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

AssetDisplay::AssetDisplay(const char* name, const char* path, PathNode* parent) : mParent(parent)
{
	unsigned int sizeName = strlen(name) + 1;
	mName = new char[sizeName];
	strcpy_s(const_cast<char*>(mName), sizeName, name);

	unsigned int sizePath = strlen(path) + 1;
	mPath = new char[sizePath];
	strcpy_s(const_cast<char*>(mPath), sizePath, path);
}


