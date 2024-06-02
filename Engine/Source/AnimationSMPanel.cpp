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
            for (AnimationState state : mStateMachine->GetStates())
            {
                ed::BeginNode(state.mEditorId);
                ImGui::Text(state.mName.c_str());
                ed::BeginPin(state.mEditorId + 1, ed::PinKind::Input);
                ImGui::Text("-> In");
                ed::EndPin();
                ImGui::SameLine();
                ed::BeginPin(state.mEditorId + 2, ed::PinKind::Output);
                ImGui::Text("Out ->");
                ed::EndPin();
                float start = state.mStartTime;
                float end = state.mEndTime;
                bool loop = state.mLoop;
                if (ImGui::DragFloat("Start time", &start, 0.01f, 0.0f, end))
                {
                    state.mStartTime = start; // works with const?
                }
                if (ImGui::DragFloat("End time", &end, 0.01f, start, 30.0f))
                {
                    state.mEndTime = end; // works with const?
                }
                if (ImGui::Checkbox("Loop", &loop))
                {
                    state.mLoop = loop; // works with const?
                }
                ed::EndNode();
            }

            //Draw transitions
            for (AnimationTransition transition : mStateMachine->GetTransitions())
            {
                ed::Link(transition.mEditorId, transition.mSourceId, transition.mTargetId);

            }

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
