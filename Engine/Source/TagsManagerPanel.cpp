#include "TagsManagerPanel.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
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
    if (ImGui::Begin(GetName(), &mOpen, windowFlags))
    {
        if (ImGui::CollapsingHeader("Tags")) {
            DrawTagsList();
            DrawDeleteButton();
            DrawAddButton();
        }
    }
    ImGui::End(); 
}

void TagsManagerPanel::DrawTagsList()
{
    int yMax;
    if (18 * App->GetScene()->GetCustomTagsSize() >= 300) {
        yMax = 300;
    }
    else if (18 * App->GetScene()->GetCustomTagsSize() == 0) {
        yMax = 18;
    }
    else {
        yMax = 18 * App->GetScene()->GetCustomTagsSize();
    }
    ImVec2 size = ImVec2(200.0f, yMax);

    std::vector<Tag*> customTags = App->GetScene()->GetCustomTag();

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
            App->GetScene()->DeleteTag(mSelectedTag);
        }
    }
}

void TagsManagerPanel::DrawAddButton()
{
    ImGui::SeparatorText("ADD");
    static char inputBuffer[256] = ""; 
    ImGui::InputText("##Add a new tag", inputBuffer, IM_ARRAYSIZE(inputBuffer));

    if (ImGui::Button("Add") && std::strlen(inputBuffer) > 0)
    {
        App->GetScene()->AddTag(inputBuffer);
        strcpy_s(inputBuffer, "");
    }
}
