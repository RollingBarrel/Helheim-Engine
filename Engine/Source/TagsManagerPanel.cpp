#include "TagsManagerPanel.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "Tag.h"
#include "imgui.h"

#include <vector>



TagsManagerPanel::TagsManagerPanel() : Panel(TAGSMANAGERPANEL, true)
{
}

TagsManagerPanel::~TagsManagerPanel()
{
}

void TagsManagerPanel::Draw(int windowFlags)
{
    if (ImGui::CollapsingHeader("Tags")) {
        DrawTagsList();
        DrawDeleteButton();
        DrawAddButton();
    }
    
}

void TagsManagerPanel::DrawTagsList()
{
    int yMax = (20 * App->GetTags()->GetCustomTagsSize() >= 300) ? 300 : 20 * App->GetTags()->GetCustomTagsSize();
    ImVec2 size = ImVec2(200.0f, yMax);

    std::vector<Tag*> systemTags = App->GetTags()->GetSystemTag();
    std::vector<Tag*> customTags = App->GetTags()->GetCustomTag();

    if (ImGui::BeginListBox("##Tags", size))
    {
        for (auto i = 0; i < customTags.size(); i++) {
            bool isSelected = (mSelectedIdx == i);
            if (ImGui::Selectable(customTags[i]->GetName().c_str(), isSelected))
            {
                mSelectedIdx = i;
                mSelectedTag = customTags[i];
            }
        }

        ImGui::EndListBox();
    }
}

void TagsManagerPanel::DrawDeleteButton()
{
    ImGui::SeparatorText("DELETE");
    if (ImGui::Button("Delete selected tag"))
    {
        if (mSelectedTag != nullptr) {
            App->GetTags()->DeleteTag(mSelectedTag);
        }
    }
}

void TagsManagerPanel::DrawAddButton()
{
    ImGui::SeparatorText("ADD");
    static char inputBuffer[256] = ""; 
    //strcpy_s(inputBuffer, mTemporalTagName);
    ImGui::InputText("##Add a new tag", inputBuffer, IM_ARRAYSIZE(inputBuffer));

    if (ImGui::Button("Add") && std::strlen(inputBuffer) > 0)
    {
        App->GetTags()->AddTag(inputBuffer);
        strcpy_s(inputBuffer, "");
    }
}
