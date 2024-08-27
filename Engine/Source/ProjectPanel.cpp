#pragma once

#include "EngineApp.h"
#include "ModuleFileSystem.h"
#include "ModuleEngineResource.h"
#include "ProjectPanel.h"
#include "HierarchyPanel.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "IconsFontAwesome6.h"

#include "imgui.h"

ProjectPanel::ProjectPanel() : Panel(PROJECTPANEL, true)
{
}

ProjectPanel::~ProjectPanel()
{
}

void ProjectPanel::Draw(int windowFlags)
{

	PathNode* root = EngineApp->GetFileSystem()->GetRootNode();

	//if (EngineApp->GetFileSystem()->IsClean())
	//{
	//	mSelectedNode = nullptr;
	//	EngineApp->GetFileSystem()->SetIsClean(false);
	//}

	windowFlags |= ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		if (ImGui::BeginMenuBar())
		{
			static char tmp[128] = "";
			if (ImGui::InputText("##", tmp, IM_ARRAYSIZE(tmp)))
			{
				mSearchAsset = tmp;
			}
			ImGui::Text(ICON_FA_MAGNIFYING_GLASS);
			ImGui::EndMenuBar();
		}


		ImGui::Columns(2);
		ImGui::BeginChild("Folders");
		if(root != nullptr)
			DrawFolders(*root);
		ImGui::EndChild();
		ImGui::NextColumn();
		ImGui::BeginChild("Assets");
		if (mSelectedNode)
		{
			DrawAssets(*mSelectedNode);
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

const void ProjectPanel::DrawFolders(const PathNode& current)
{
	//Discard Meta file but, read .emeta data
	for (auto i = 0; i < current.mChildren.size(); ++i)
	{	
		bool selected = false;

		if (mSelectedNode) 
		{
			selected = strcmp(mSelectedNode->mName, current.mChildren[i]->mName) == 0;
		}
		
		std::string nameWithoutPath = current.mChildren[i]->mName;
		nameWithoutPath = nameWithoutPath.substr(nameWithoutPath.find_last_of('/')+1);
		std::string nameWithIconClose = ICON_FA_FOLDER;
		std::string nameWithIconOpen = ICON_FA_FOLDER_OPEN;
		nameWithIconClose += " " + nameWithoutPath;
		nameWithIconOpen += " " + nameWithoutPath;

		int flags = ImGuiTreeNodeFlags_FramePadding;
		if (current.mChildren[i]->mChildren.empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		if (selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
			ImVec4 pressedColor = ImVec4{ 0.37f, 0.29f, 0.49f, 1.0f };
			ImGui::PushStyleColor(ImGuiCol_Header, pressedColor);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, pressedColor);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, pressedColor);
		}
		bool open = ImGui::TreeNodeEx(nameWithIconClose.c_str(), flags);
		if (selected)
		{
			ImGui::PopStyleColor(3);
		}
		SavePrefab(*current.mChildren[i]);
		if (open)
		{
			DrawFolders(*current.mChildren[i]);
			ImGui::TreePop();
		}
		
		if (ImGui::IsItemClicked()) 
		{
			mSelectedNode = current.mChildren[i];
		}

	}
}

const void ProjectPanel::DrawAssets(const PathNode& current)
{
	for (unsigned int i = 0; i < current.assets.size(); ++i)
	{
		int flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding;
		std::string nameWithIcon = ICON_FA_FILE;
		nameWithIcon += " " + std::string(current.assets[i]->mName);

		bool selected = false;
		if (mSelectedAsset) 
		{
			selected = strcmp(mSelectedAsset->mName, current.assets[i]->mName) == 0;
		}

		if (selected)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
			ImVec4 pressedColor = ImVec4{ 0.37f, 0.29f, 0.49f, 1.0f };
			ImGui::PushStyleColor(ImGuiCol_Header, pressedColor);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, pressedColor);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, pressedColor);
		}

		if (CaseInsensitiveSubstringSearch(current.assets[i]->mName, mSearchAsset))
		{
			if (ImGui::TreeNodeEx(nameWithIcon.c_str(), flags))
			{
				if (ImGui::IsItemClicked())
				{
					mSelectedAsset = current.assets[i];
				}
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					//App->GetScene()->Load(current.assets[i]->mName);
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					AssetDisplay* asset = current.assets[i];
					std::string path = asset->mPath;
					std::string token = path.substr(path.find_last_of('.'), path.size());
					if (token == ".prfb")
					{
						EngineApp->GetScene()->OpenPrefabScreen(asset->mPath);
					}
				}
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					ImGui::SetDragDropPayload("_SCENE", current.assets[i], sizeof(*current.assets[i]));

					ImGui::Text(current.assets[i]->mName);
					ImGui::EndDragDropSource();
				}
				ImGui::TreePop();
			}
		}
		if (selected)
		{
			ImGui::PopStyleColor(3);
		}
	}
}

void ProjectPanel::SavePrefab(const PathNode& dir) const
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
		{
			HierarchyPanel* hierarchyPanel = (HierarchyPanel*)EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL);
			for (auto object : hierarchyPanel->FilterMarked()) 
			{
				std::string file = dir.mName;
				file.append('/' + object->GetName() + ".prfb");
				EngineApp->GetEngineResource()->ImportFile(file.c_str());

			}
		}
		ImGui::EndDragDropTarget();
	}
}