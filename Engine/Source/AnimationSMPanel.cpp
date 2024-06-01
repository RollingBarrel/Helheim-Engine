#include "AnimationSMPanel.h"
#include "imgui.h"
#include "../Source/Dependencies/imgui-node-editor-0.9.3/imgui_node_editor.h"


namespace ed = ax::NodeEditor;

AnimationSMPanel::AnimationSMPanel() : Panel(ANIMATIONSMPANEL, true)
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
        ed::BeginNode(uniqueId++);
        ImGui::Text("Node A");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();
        ed::End();
        ed::SetCurrentEditor(nullptr);
		

	}
	ImGui::End();
}
