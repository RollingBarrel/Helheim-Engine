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
		if(ImGui::Button("Refresh"))
		{
			App->GetFileSystem()->DiscoverFiles("Library", root);
		}

		for (auto i = 0; i < root->mChildren.size(); ++i)
		{
			if (ImGui::TreeNodeEx(root->mChildren[i]->mName, ImGuiTreeNodeFlags_DefaultOpen))
			{
				for (auto j = 0; j < root->mChildren[i]->assets.size(); ++j)
				{
					if (ImGui::TreeNodeEx(root->mChildren[i]->assets[j]->mName, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf))
					{
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("_SCENE", root->mChildren[i]->assets[j], sizeof(*root->mChildren[i]->assets[j]));

							ImGui::Text(root->mChildren[i]->assets[j]->mName);
							ImGui::EndDragDropSource();
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}


	}
	ImGui::End();
}

AssetDisplay::AssetDisplay(const char* name, PathNode* parent) : mParent(parent)
{
	unsigned int size = strlen(name) + 1;
	mName = new char[size];
	strcpy_s(const_cast<char*>(mName), size, name);
}
