#include "AnimationSMPanel.h"
#include "imgui.h"
#include "../Source/Dependencies/imgui-node-editor-0.9.3/imgui_node_editor.h"
#include "AnimationStateMachine.h"

namespace ed = ax::NodeEditor;

AnimationSMPanel::AnimationSMPanel() : Panel(ANIMATIONSMPANEL, false)
{
	ed::Config config;
	config.SettingsFile = "Simple.json";
	mEditorContext = ed::CreateEditor(&config);
}

AnimationSMPanel::~AnimationSMPanel()
{
	ed::DestroyEditor(mEditorContext);
}

void AnimationSMPanel::Draw(int windowFlags)
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
        ImGui::Separator();
        ed::SetCurrentEditor(mEditorContext);
        ed::Begin("My Editor", ImVec2(0.0, 0.0f));
        int uniqueId = 1;
        // Start drawing nodes.
        if (mStateMachine)
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
                ImGui::BulletText("Clip: %s", mStateMachine->GetStateClip(i).c_str());
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
            //Add transition
            if (ed::BeginCreate())
            {
                ed::PinId inputPinId, outputPinId;
                if (ed::QueryNewLink(&inputPinId, &outputPinId))
                {
                    // QueryNewLink returns true if editor want to create new link between pins.
                    //
                    // Link can be created only for two valid pins, it is up to you to
                    // validate if connection make sense. Editor is happy to make any.
                    //
                    // Link always goes from input to output. User may choose to drag
                    // link from output pin or input pin. This determine which pin ids
                    // are valid and which are not:
                    //   * input valid, output invalid - user started to drag new ling from input pin
                    //   * input invalid, output valid - user started to drag new ling from output pin
                    //   * input valid, output valid   - user dragged link over other pin, can be validated

                    if (inputPinId && outputPinId) // both are valid, let's accept link
                    {
                        // ed::AcceptNewItem() return true when user release mouse button.
                        if (ed::AcceptNewItem())
                        {
                            // Since we accepted new link, lets add one to our list of links.
                            // m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });

                            // Draw new link.
                            // ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                        }

                        // You may choose to reject connection between these nodes
                        // by calling ed::RejectNewItem(). This will allow editor to give
                        // visual feedback by changing link thickness and color.
                    }
                }
            }
        }
        ed::End();
        ed::SetCurrentEditor(nullptr);
		

        //Draw add node panel
        

	}
	ImGui::End();
}

void AnimationSMPanel::LoadConfig()
{
    if (mEditorContext) // Necessary¿¿¿
    {
        ed::DestroyEditor(mEditorContext);

    }
    ed::Config config;
    config.SettingsFile = mStateMachine->GetJsonConfig().c_str();
    mEditorContext = ed::CreateEditor(&config);

}
