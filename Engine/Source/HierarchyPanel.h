#pragma once
#pragma once
#include "Panel.h"
#define HIERARCHYPANEL "Hierarchy##"

class GameObject;

class HierarchyPanel : public Panel
{
public:
	HierarchyPanel();
	//~HierarchyPanel();

	void Draw(int windowFlags) override;
private:
	void DrawTree(GameObject* node, const int selected);
	void DragAndDropSource(GameObject* source);
	void DragAndDropTarget(GameObject* target, bool reorder = false);
	void OnRightClickNode(GameObject* node);
};
