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
				ImGui::TreePop();
			}

		}

	}
	ImGui::End();
}

AssetDisplay::AssetDisplay(const char* name, PathNode* parent) : mName(name), mParent(parent)
{
}
