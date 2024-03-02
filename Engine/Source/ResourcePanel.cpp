#include "ResourcePanel.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ResourceModel.h"

#include "imgui.h"

ResourcePanel::ResourcePanel() : Panel(RESOURCEPANEL, true)
{
}

void ResourcePanel::Draw(int windowFlags)
{
	if(ImGui::Begin(GetName(), &mOpen, windowFlags))
	{	
		for (std::map<unsigned int, Resource*>::iterator it = App->GetResource()->mResources.begin(); it != App->GetResource()->mResources.end(); ++it)
		{
			if(it->second != nullptr)
			{
				switch (it->second->GetType())
				{
					case Resource::Type::Texture:
					{
						ImGui::Text("Texture");
						ImGui::SameLine();
						ImGui::Text(std::to_string(it->first).c_str());
						ImGui::SameLine();
						ImGui::Text(std::to_string(it->second->GetReferenceCount()).c_str());
						break;
					}
					case Resource::Type::Mesh:
					{
						ImGui::Text("Mesh");
						ImGui::SameLine();
						ImGui::Text(std::to_string(it->first).c_str());
						ImGui::SameLine();
						ImGui::Text(std::to_string(it->second->GetReferenceCount()).c_str());
						break;
					}
					case Resource::Type::Material:
					{
						ImGui::Text("Material");
						ImGui::SameLine();
						ImGui::Text(std::to_string(it->first).c_str());
						ImGui::SameLine();
						ImGui::Text(std::to_string(it->second->GetReferenceCount()).c_str());
						break;
					}
					case Resource::Type::Model:
					{
						ImGui::Text("Model");
						ImGui::SameLine();
						ImGui::Text(std::to_string(it->first).c_str());
						ImGui::SameLine();
						ImGui::Text(std::to_string(it->second->GetReferenceCount()).c_str());
						break;
					}
				}
			}

		}
	}
	ImGui::End();
}
