#pragma once
#include "Panel.h"
#include <unordered_set>
#include <vector>
#define HIERARCHYPANEL "Hierarchy##"

class GameObject;

class HierarchyPanel : public Panel
{
public:
	HierarchyPanel();
	//~HierarchyPanel();

	void Draw(int windowFlags) override;
	GameObject* GetFocusedObject() const;
	void DrawObjectAxis() const;
	void SetFocus(const GameObject& focusedObject);
	const std::vector<GameObject*> FilterMarked() const;

private:
	void InternalSetFocus(GameObject* focusedObject);
	void DrawTree(GameObject* node);
	void DragAndDropSource(GameObject* source);
	void DragAndDropTarget(GameObject* target, bool reorder = false);
	void OnRightClickNode(GameObject* node);
	void OnLeftCkickNode(GameObject* node);
	void ShiftClick(GameObject* node, bool selected, bool click = false);

	std::unordered_set<GameObject*> mMarked;
	std::unordered_set<unsigned int> mNodesToOpen;
	int mFocusId = 0;
	int mLastClickedObject = 0;
	int mShiftMarking[2] = { 0 };
	int mShiftClicked = 0;
	int mLastMarkSeen = 0;
	bool mUnmarkFlag = false;
};
