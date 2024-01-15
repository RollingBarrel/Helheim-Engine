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
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		if(ImGui::Button("Refresh"))
		{
			mFiles.clear();
			mDirectories.clear();
			App->GetFileSystem()->DiscoverFiles("Assets", mFiles, mDirectories);
		}

		for (auto i = 0; i < mDirectories.size(); ++i)
		{
			if (ImGui::TreeNodeEx(mDirectories[i].c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::TreePop();
			}
		}

	}
	ImGui::End();
}
