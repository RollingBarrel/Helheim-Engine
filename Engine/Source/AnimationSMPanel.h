#pragma once
#include "Panel.h"
#include "../Source/Dependencies/imgui-node-editor-0.9.3/imgui_node_editor.h"
#define ANIMATIONSMPANEL "AnimationSM##"


class AnimationSMPanel :
	public Panel
{
public:
	AnimationSMPanel();
	~AnimationSMPanel();

	void Draw(int windowFlags) override;

private:

	ax::NodeEditor::EditorContext* mEditorContext;
};

