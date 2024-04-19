#include "HierarchyPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

HierarchyPanel::HierarchyPanel() : Panel(HIERARCHYPANEL, true) {}

void HierarchyPanel::Draw(int windowFlags)
{
	GameObject* root = App->GetScene()->GetRoot();
	if (mLastClickedObject == 0) { mLastClickedObject = root->GetID(); }
	if (mFocusId == 0) { mFocusId = mLastClickedObject; }
	ImGui::SetNextWindowPos(ImVec2(-100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Once);
	ImGui::Begin(GetName(), &mOpen, windowFlags);
	ImGui::BeginChild("##tree");
	if (!ImGui::GetIO().MouseDown[0] && mUnmarkFlag) 
	{
		mMarked.clear();
		mUnmarkFlag = false;
	}
	if (mLastClickedObject != root->GetID()) 
	{ 
		GameObject* marked = root->Find(mLastClickedObject);
		mMarked.insert(marked);
	}
	mLastMarkSeen = 0; mShiftClicked = 0;
	DrawTree(root);
	ImGui::InvisibleButton("##", ImVec2(-1, -1));
	OnRightClickNode(root);
	ImGui::EndChild();
	DragAndDropTarget(root);
	
	ImGui::End();
}

void HierarchyPanel::SetFocus(GameObject* focusedObject) 
{ 
	mUnmarkFlag = true;
	mFocusId = focusedObject->GetID();
	mLastClickedObject = focusedObject->GetID();
	GameObject* parent = focusedObject->GetParent();
	while (parent != nullptr)
	{
		mNodesToOpen.insert(parent->GetID());
		parent = parent->GetParent();
	}
}

void HierarchyPanel::OnLeftCkickNode(GameObject* node) 
{    
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) 
	{
        if (ImGui::GetIO().KeyShift) 
		{
            if (mLastMarkSeen != node->GetID()) 
			{
                mShiftClicked = node->GetID();
                if (mLastMarkSeen != 0) 
				{
                    mShiftMarking[0] = mLastMarkSeen;
                    mShiftMarking[1] = mShiftClicked;
                }
            }
        }
        else if (ImGui::GetIO().KeyCtrl) {}
        else 
		{
            if (mMarked.find(node) != mMarked.end()) { mUnmarkFlag = true; }
            else { mMarked.clear(); }
        }
        mLastClickedObject = node->GetID();
    }
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered(ImGuiHoveredFlags_None) && !ImGui::IsItemToggledOpen())
	{
        mFocusId = node->GetID();
    }
}

void HierarchyPanel::OnRightClickNode(GameObject* node) 
{
	ImGui::PushID(node->GetID());
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) 
	{
		ImGui::OpenPopup("OptionsGO");
		if (mMarked.find(node) == mMarked.end()) 
		{
			mMarked.clear();
		}
		mLastClickedObject = node->GetID();
	}
	if (ImGui::BeginPopup("OptionsGO")) 
	{
		if (ImGui::Selectable("Create GameObject")) 
		{
			GameObject* gameObject = new GameObject(node);
			//node->AddChild(gameObject);
			mLastClickedObject = gameObject->GetID();
			mFocusId = gameObject->GetID();
			mMarked.clear();
		}
		bool isPrefabRoot = (App->GetScene()->IsPrefabScene() && node->mParent->mIsRoot);
		if (!node->mIsRoot && !isPrefabRoot) 
		{
			if (ImGui::Selectable("Duplicate")) 
			{
				std::unordered_set<GameObject*> selectAfter;
				for (auto object : FilterMarked()) 
				{
					GameObject* gameObject = new GameObject(*object);
					App->GetScene()->AddGameObjectToDuplicate(gameObject);
					mLastClickedObject = gameObject->GetID();
					mFocusId = gameObject->GetID();
					selectAfter.insert(gameObject);
				}
				mMarked = selectAfter;
			}

			if (ImGui::Selectable("Delete")) 
			{
				for (auto object : FilterMarked()) 
				{
					App->GetScene()->AddGameObjectToDelete(object);
					mLastClickedObject = App->GetScene()->GetRoot()->GetID();
					mFocusId = App->GetScene()->GetRoot()->GetID();
				}
				mMarked.clear();
			}
			if (ImGui::Selectable("Save as Prefab")) 
			{
				for (auto object : FilterMarked()) 
				{
					std::string file = "Assets/Prefabs/";
					file.append('/' + object->GetName() + ".prfb");
					App->GetScene()->SavePrefab(object, file.c_str());
				}
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
}

void HierarchyPanel::DrawTree(GameObject* node)
{
	ImGui::PushID(node->GetID());
	bool nodeOpen = true;
	
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	if (!node->mIsRoot) 
	{
		bool selected = mMarked.find(node) != mMarked.end();
		if (!node->IsActive())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 0.50f, 0.50f, 1.00f));
		}
		ShiftClick(node, selected);
		ImGui::InvisibleButton("##", ImVec2(-1, 5)); //This can be changed to modify the spacing between elements
		DragAndDropTarget(node, true);
		ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (selected) {
			baseFlags |= ImGuiTreeNodeFlags_Selected;
			ImVec4 pressedColor = ImVec4{ 0.37f, 0.29f, 0.49f, 1.0f };
			ImGui::PushStyleColor(ImGuiCol_Header, pressedColor);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, pressedColor);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, pressedColor);
		}			
		if (node->mChildren.size() == 0) 
		{
			baseFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}
		if (mNodesToOpen.find(node->GetID()) != mNodesToOpen.end())
		{
			mNodesToOpen.erase(node->GetID());
			ImGui::SetNextItemOpen(true);
		}
		nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)node->mID, baseFlags, node->mName.c_str()) && (node->mChildren.size() > 0);
		ImGui::PopStyleVar();
		if (!node->IsActive())
		{
			ImGui::PopStyleColor();
		}
		if (selected)
		{
			ImGui::PopStyleColor(3);
		}
		DragAndDropSource(node);
		OnLeftCkickNode(node);
		OnRightClickNode(node);
	}
	else 
	{
		if (App->GetScene()->IsPrefabScene()) 
		{
			if (ImGui::ArrowButton("Go Back", ImGuiDir_Left)) 
			{
				App->GetScene()->ClosePrefabScreen();
			}
			ImGui::SameLine();
		}
		nodeOpen = ImGui::CollapsingHeader(node->mName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
		ImGui::PopStyleVar();
		OnRightClickNode(node);
	}
	DragAndDropTarget(node);
	if (nodeOpen) 
	{
		for (auto child : node->mChildren) 
		{
			DrawTree(child);
		}

		if (!node->mIsRoot) 
		{
			ImGui::TreePop();
		}
	}

	ImGui::PopID();
}

void HierarchyPanel::DragAndDropSource(GameObject* source)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		mUnmarkFlag = false;
		ImGui::SetDragDropPayload("_TREENODE", &source, sizeof(*source));
		if (mMarked.size() <= 1) 
		{
			ImGui::Text(source->mName.c_str());
		}
		else 
		{
			ImGui::Text("%d Elements", mMarked.size());
		}
		
		ImGui::EndDragDropSource();
	}
}

void HierarchyPanel::DragAndDropTarget(GameObject* target, bool reorder) 
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
		{
			if (mMarked.find(target) == mMarked.end()) 
			{ //Don't drag object into itself
				for (auto movedObject : FilterMarked()) 
				{
					bool isParent = false;

					GameObject* parent = target->mParent;
					while (parent != nullptr) 
					{
						if (parent->mID == movedObject->mID) 
						{
							isParent = true;
						}
						parent = parent->mParent;
					}


					if (!isParent) 
					{
						GameObject* pMovedObject = movedObject->mParent->RemoveChild(movedObject->GetID());
						if (reorder) { target->mParent->AddChild(pMovedObject, target->mID); }
						else { target->AddChild(pMovedObject); }
					}
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void HierarchyPanel::ShiftClick(GameObject* node, bool selected, bool click) 
{
	if (mShiftMarking[0] == 0 && mShiftMarking[1] == 0) 
	{
		if (selected) 
		{ 
			mLastMarkSeen = node->GetID();
			if (mShiftClicked != 0) 
			{ 
				mShiftMarking[0] = mShiftClicked;
				mShiftMarking[1] = mLastMarkSeen;
			}
		}
	}
	else if (mShiftMarking[0] != 0 && mShiftMarking[1] != 0) 
	{
		if (mShiftMarking[0] == node->GetID()) 
		{ 
			mShiftMarking[0] = 0; 
		}
	}
	else if (mShiftMarking[0] == 0) 
	{
		mMarked.insert(node);
		if (mShiftMarking[1] == node->GetID()) 
		{
			mShiftMarking[1] = 0;
		}
	}
}

// Excludes from the list of selected objects any that is the child (directly or indirectly) of another selected item.
// Use this before doing any operation on all selected items that would already apply to all children
// Ex. When you duplicate an object all it's children are allways duplicated too.
const std::vector<GameObject*> HierarchyPanel::FilterMarked() const 
{
	std::vector<GameObject*> filteredList;
	for (auto object : mMarked) 
	{
		GameObject* parent = object->mParent;
		while (parent != nullptr) 
		{
			if (mMarked.find(parent) != mMarked.end()) 
			{
				break;
			}
			parent = parent->mParent;
		}
		if (parent == nullptr) 
		{ 
			filteredList.push_back(object); 
		}
	}
	return filteredList;
}

GameObject* HierarchyPanel::GetFocusedObject() const 
{
	GameObject* root = App->GetScene()->GetRoot();
	GameObject* focus = root->Find(mFocusId);
	if (focus == nullptr) { return root; }
	else { return focus; }
}