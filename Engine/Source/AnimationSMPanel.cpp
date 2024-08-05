#include "AnimationSMPanel.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include "AnimationStateMachine.h"
#include "EngineApp.h"
#include "ModuleFileSystem.h"
#include "ModuleEngineResource.h"
#include "ResourceStateMachine.h"
#include "AnimationComponent.h"
#include "ImporterStateMachine.h"

namespace ed = ax::NodeEditor;

AnimationSMPanel::AnimationSMPanel() : Panel(ANIMATIONSMPANEL, false)
{
	ed::Config config;
	config.SettingsFile = "Simple.json";
	mEditorContext = ed::CreateEditor(&config);
    mStateMachine = nullptr;
}

AnimationSMPanel::~AnimationSMPanel()
{
	ed::DestroyEditor(mEditorContext);
    
}

void AnimationSMPanel::Draw(int windowFlags)
{
    if (mStateMachine && mOpen)
    {
	    if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	    {

            ed::SetCurrentEditor(mEditorContext);
            ed::Begin("My Editor", ImVec2(0.0, 0.0f));

            DrawNodes();
            DrawTransitions();
            ManageCreate();

            ShowContextMenus();

            ed::Suspend();
            ShowNodeMenu();
            ShowLinkMenu();
            ShowCreateNewNodeMenu();
            ed::Resume();

            ed::End();
            ed::SetCurrentEditor(nullptr);
		

	    }
	    ImGui::End();

    }
}

void AnimationSMPanel::Close()
{
    

    ed::DestroyEditor(mEditorContext);
    mStateMachine = nullptr; //Do a state machine save here when sm is a resource
    mOpen = false;
    
}

void AnimationSMPanel::LoadConfig()
{
    if (mEditorContext) // Necessary¿¿¿
    {
        ed::DestroyEditor(mEditorContext);

    }
    ed::Config config;
    config.SettingsFile = "Simple.json";
    mEditorContext = ed::CreateEditor(&config);

}

void AnimationSMPanel::DrawNodes()
{
    //Draw nodes
    for (int i = 0; i < mStateMachine->GetNumStates(); i++)
    {
        ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
        ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

        ed::BeginNode(i * 3 + 1);
        ImGui::Indent(1.0);
        ImGui::TextColored(ImVec4(255, 255, 0, 255), mStateMachine->GetStateName(i).c_str());

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f * ImGui::GetStyle().Alpha);

        ImVec2 size = ed::GetNodeSize(i * 3 + 1);


        ImGui::PopStyleVar();

        ImGui::Dummy(ImVec2(96.0, 8.0));
        if (i == 0)
        {
            ImGui::BulletText("Default");
        }

        ImGui::Dummy(ImVec2(96.0, 8.0));

        ImGui::Dummy(ImVec2(64.0, 8.0));

        // In Pin
        ed::PushStyleVar(ed::StyleVar_PinArrowSize, 8.0f);
        ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 8.0f);
        ed::PushStyleVar(ed::StyleVar_PinRadius, 10.0f);
        ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
        ed::BeginPin(i * 3 + 2, ed::PinKind::Input);
        ImGui::Text("In");
        ed::EndPin();
        ed::PopStyleVar(4);

        // Out Pin
        ImGui::SameLine(size.x - 40);
        ed::PushStyleVar(ed::StyleVar_PinArrowSize, 0.0f);
        ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 0.0f);
        ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
        ed::BeginPin(i * 3 + 3, ed::PinKind::Output);
        ImGui::Text("Out");

        ed::EndPin();

        ed::EndNode();

        ed::PopStyleVar(3);
        ed::PopStyleColor(2);
    }
}

void AnimationSMPanel::DrawTransitions()
{
    //Draw transitions
    ed::PushStyleVar(ed::StyleVar_LinkStrength, 4.0f);
    int num_nodes = mStateMachine->GetNumStates();
    for (int i = 0, count = mStateMachine->GetNumTransitions(); i < count; ++i)
    {
        int source = mStateMachine->GetStateIndex(mStateMachine->GetTransitionSource(i));
        int target = mStateMachine->GetStateIndex(mStateMachine->GetTransitionTarget(i));

        if (source < num_nodes && target < num_nodes)
        {
            ed::Link(num_nodes * 3 + i + 1, source * 3 + 3, target * 3 + 2);
        }
    }
    ed::PopStyleVar(1);
}



void AnimationSMPanel::ShowCreateNewNodeMenu()
{
    if (ImGui::BeginPopup("Create New Node"))
    {
        ImGui::TextUnformatted("State Machine Menu");
        ImGui::Separator();

        DrawMenuBar();

        ImGui::Separator();

        //Draw add node panel
        char buffer[32];
        strncpy(buffer, mNewNodeName.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputText("Input new state name: ", buffer, sizeof(buffer))) {
            // Update mNewNodeName with the new value from the buffer
            mNewNodeName = std::string(buffer);
        }

        ImGui::SameLine();
        if (ImGui::Button("Create new state")) 
        {
            mStateMachine->AddState(mNewNodeName);
            mNewNodeName = "";
        }


        ImGui::EndPopup();
    }
}

void AnimationSMPanel::ShowNodeMenu()
{
    if (mSelectedNode == -1)
    {
        return;
    }
    if (ImGui::BeginPopup("State Context Menu"))
    {
        ImGui::TextUnformatted("Edit State Menu");
        ImGui::Separator();

        std::string name = mStateMachine->GetStateName(mSelectedNode);
        std::string oldName = name;
        char buffer[32];
        strncpy(buffer, name.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (mSelectedNode == 0)
        {
            ImGui::Text("State name: Default");
        }
        else
        {
            if (ImGui::InputText("State name: ", buffer, sizeof(buffer))) 
            {
                // Update mNewNodeName with the new value from the buffer
                name = std::string(buffer);
                mStateMachine->SetStateName(mSelectedNode, name);
                for (int i = 0; i < mStateMachine->GetNumTransitions(); ++i)
                {
                    if (mStateMachine->GetTransitionSource(i) == oldName)
                    {
                        mStateMachine->SetTransitionSource(i, name);
                    }
                    if (mStateMachine->GetTransitionTarget(i) == oldName)
                    {
                        mStateMachine->SetTransitionTarget(i, name);
                    }
                }
            }

        }

        float startTime = mStateMachine->GetStateStartTime(mSelectedNode);
        float endTime = mStateMachine->GetStateEndTime(mSelectedNode);
        if (ImGui::DragFloat("Start time: ", &startTime, 0.01f, 0.0f, endTime))
        {
            mStateMachine->SetStateStartTime(mSelectedNode, startTime);
        }
        if (ImGui::DragFloat("End time: ", &endTime, 0.01f, startTime, startTime + 5.0f))
        {
            mStateMachine->SetStateEndTime(mSelectedNode, endTime);
        }

        bool loop = mStateMachine->GetStateLoop(mSelectedNode);
        if (ImGui::Checkbox("Loop", &loop))
        {
            mStateMachine->SetStateLoop(mSelectedNode, loop);
        }
        if (mSelectedNode != 0)
        {
            ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                mStateMachine->DeleteState(mSelectedNode);
                mSelectedNode = -1;
            }
        }

        ImGui::EndPopup();
    }
}

void AnimationSMPanel::ShowLinkMenu()
{
    if (mSelectedLink == -1)
    {
        return;
    }
    if (ImGui::BeginPopup("Transition Context Menu"))
    {
        ImGui::TextUnformatted("Edit Transition Menu");
        ImGui::Separator();

        std::string name = mStateMachine->GetTransitionTrigger(mSelectedLink);
        char buffer[32];
        strncpy(buffer, name.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = '\0';

        if (ImGui::InputText("Transition name: ", buffer, sizeof(buffer))) {
            // Update mNewNodeName with the new value from the buffer
            name = std::string(buffer);
            mStateMachine->SetTransitionTrigger(mSelectedLink, name);
        }

        std::string from = mStateMachine->GetTransitionSource(mSelectedLink);
        std::string to = mStateMachine->GetTransitionTarget(mSelectedLink);

        ImGui::Text(from.c_str());
        ImGui::SameLine();
        ImGui::Text(" -> ");
        ImGui::SameLine();
        ImGui::Text(to.c_str());

        if (ImGui::Button("Delete"))
        {
            mStateMachine->DeleteTransition(mSelectedLink);
            mSelectedLink = -1;
        }


        ImGui::EndPopup();
    }

}

void AnimationSMPanel::ShowContextMenus()
{
    ed::Suspend();

    ed::NodeId contextNodeId = 0;
    ed::PinId contextPinId = 0;
    ed::LinkId contextLinkId = 0;
    if (ed::ShowNodeContextMenu(&contextNodeId))
    {
        mSelectedNode = int(contextNodeId.Get() - 1) / 3;
        ImGui::OpenPopup("State Context Menu");
    }
    else if (ed::ShowLinkContextMenu(&contextLinkId))
    {
        mSelectedLink = int(contextLinkId.Get()) - mStateMachine->GetNumStates() * 3 - 1;
        ImGui::OpenPopup("Transition Context Menu");
    }
    else if (ed::ShowBackgroundContextMenu())
    {
        ImGui::OpenPopup("Create New Node");
    }

    ed::Resume();

}


void AnimationSMPanel::ManageCreate()
{
    //Add transition
    if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
    {
        ed::PinId inputPinId = 0, outputPinId = 0;
        if (ed::QueryNewLink(&inputPinId, &outputPinId))
        {
            if (inputPinId && outputPinId)
            {
                
                bool startIsInput = unsigned int(inputPinId.Get() - 1) % 3 == 1;
                bool endIsInput = unsigned int(outputPinId.Get() - 1) % 3 == 1;
                unsigned int startNode = unsigned int(inputPinId.Get() - 1) / 3;
                unsigned int endNode = unsigned int(outputPinId.Get() - 1) / 3;

                if (outputPinId == inputPinId)
                {
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                else if (startIsInput == endIsInput)
                {
                    //showLabel("x Incompatible Pins. Must be In->Out", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                else if (startNode == endNode)
                {
                    //showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                }
                else
                {
                    //showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                    if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                    {
                        if (startIsInput)
                        {
                            mStateMachine->AddTransition(mStateMachine->GetStateName(endNode), mStateMachine->GetStateName(startNode), std::string("t"+ mStateMachine->GetStateName(startNode)));
                        }
                        else
                        {
                            mStateMachine->AddTransition(mStateMachine->GetStateName(startNode), mStateMachine->GetStateName(endNode), std::string("t"+ mStateMachine->GetStateName(endNode)));
                        }

                        //animation->Save();
                    }
                }
            }
        }
    }
    ed::EndCreate();
}

void AnimationSMPanel::GetResourcesList()
{
    const char* currentItem = mStateMachine->GetName().c_str();
    if (ImGui::BeginCombo("##combo", currentItem))
    {
        std::vector<std::string> modelNames;
        EngineApp->GetFileSystem()->DiscoverFiles("Assets/StateMachines", ".smbin", modelNames);
        for (int i = 0; i < modelNames.size(); ++i)
        {

            size_t slashPos = modelNames[i].find_last_of('/');
            if (slashPos != std::string::npos)
            {

                modelNames[i].erase(0, slashPos + 1);
            }

            size_t dotPos = modelNames[i].find_first_of('.');
            if (dotPos != std::string::npos)
            {

                modelNames[i].erase(dotPos);
            }
        }

        for (int n = 0; n < modelNames.size(); n++)
        {
            bool is_selected = (currentItem == modelNames[n]);
            if (ImGui::Selectable(modelNames[n].c_str(), is_selected))
            {
                currentItem = modelNames[n].c_str();
                std::string path = std::string("Assets/StateMachines/" + modelNames[n] + ".smbin");
                
                ResourceStateMachine* newSM = static_cast<ResourceStateMachine*>(EngineApp->GetResource()->RequestResource(path.c_str()));
                mStateMachine = newSM->GetStateMachine();
                mStateMachine->SetUID(newSM->GetUID());
                //mStateMachine->LoadResource(path.c_str());
                
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }

        }
        ImGui::EndCombo();
    }
}

void AnimationSMPanel::DrawMenuBar()
{
    std::string name = mStateMachine->GetName();
    char buffer[32];
    strncpy(buffer, name.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    if (ImGui::InputText("Current State Machine: ", buffer, sizeof(buffer))) {
        // Update mNewNodeName with the new value from the buffer
        name = std::string(buffer);
        mStateMachine->SetName(buffer);
        mStateMachine->SetUID(0);
    }
    ImGui::SameLine();
    if (!mUpToDate)
    {
        ImGui::Text("     *(Unsaved changes)     ");
        ImGui::SameLine();
    }
    ImGui::Text("Select resource state machine: ");
    ImGui::SameLine();
    GetResourcesList();
    
    ImGui::SameLine();
    if (ImGui::Button("Save StateMachine"))
    {
        //Check if asset existed to use its uid
        unsigned int uid = mStateMachine->GetUID();
        mStateMachine->SaveResource("Assets/StateMachines/", false);
        std::string filePath = "Assets/StateMachines/" + mStateMachine->GetName() + ".smbin";

        if (uid != 0)
        {
            ResourceStateMachine* existingRes = static_cast<ResourceStateMachine*>(App->GetResource()->RequestResource(uid, Resource::Type::StateMachine));
            existingRes->SetStateMachine(mStateMachine);
            //Importer::StateMachine::Save(existingRes);
            Importer::StateMachine::Import(filePath.c_str(), uid);
            App->GetResource()->ReleaseResource(uid);
        }
        else
        {
            uid = EngineApp->GetEngineResource()->CreateNewResource(filePath.c_str(), "", Resource::Type::StateMachine)->GetUID();
            mStateMachine->SetUID(uid);
            mUpToDate = true;

        }
    }


}
