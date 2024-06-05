#pragma once
#include "Panel.h"
#include "imgui_node_editor.h"
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

	void DrawNodes();
	void DrawTransitions();
	void ShowContextMenus();
	void ShowCreateNewNodeMenu();
	void ShowNodeMenu();
	void ShowLinkMenu();
	void ManageCreate();
	void GetResourcesList();
	void DrawMenuBar();


	AnimationStateMachine* mStateMachine;
	ed::EditorContext* mEditorContext;
	std::string mNewNodeName;

	int mSelectedNode = 0;
	int mSelectedLink = 0;
	bool mUpToDate = true;
	
};

