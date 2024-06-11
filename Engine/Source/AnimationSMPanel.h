#pragma once
#include "Panel.h"
#include "imgui_node_editor.h"
#define ANIMATIONSMPANEL "AnimationSM##"
#include "string"

namespace ed = ax::NodeEditor;
class AnimationStateMachine;
class AnimationComponent;
class AnimationSMPanel :
	public Panel
{
public:
	AnimationSMPanel();
	~AnimationSMPanel();

	void Draw(int windowFlags) override;
	void Close() override;
	void SetStateMachine(AnimationStateMachine* stateMachine) { mStateMachine = stateMachine; LoadConfig(); }
	void SetComponent(AnimationComponent* component) { mComponent = component; }
	void SetIsSpine(bool isSpine) { mIsSpine = isSpine; }

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
	AnimationComponent* mComponent;
	bool mIsSpine = false;


	ed::EditorContext* mEditorContext;
	std::string mNewNodeName;

	int mSelectedNode = 0;
	int mSelectedLink = 0;
	bool mUpToDate = true;
	
};

