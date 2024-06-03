#pragma once
#include "Panel.h"
#include "../Source/Dependencies/imgui-node-editor-0.9.3/imgui_node_editor.h"
#define ANIMATIONSMPANEL "AnimationSM##"
#include "string"

namespace ed = ax::NodeEditor;
class AnimationStateMachine;
class AnimationSMPanel :
	public Panel
{
public:
	AnimationSMPanel();
	~AnimationSMPanel();

	void Draw(int windowFlags) override;
	void Close() override;
	void SetStateMachine(AnimationStateMachine* stateMachine) { mStateMachine = stateMachine; LoadConfig(); }

private:

	void LoadConfig();

	AnimationStateMachine* mStateMachine;
	ed::EditorContext* mEditorContext;
	std::string mNewNodeName;
	
};

