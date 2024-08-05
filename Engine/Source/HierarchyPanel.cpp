#include "HierarchyPanel.h"
#include "imgui.h"
#include "EngineApp.h"
#include "ModuleScene.h"
#include "ModuleEngineResource.h"
#include "ModuleOpenGL.h"
#include "GameObject.h"
#include "ModuleFileSystem.h"
#include "MeshRendererComponent.h"
#include <regex>

static void AddSuffix(GameObject& gameObject)
{
	bool found = true;
	int count = 0;
	std::regex regularExpression(".+\\s\\(\\d+\\)$");
	std::string nameWithoutSuffix = gameObject.GetName();
	std::string name = nameWithoutSuffix;

	if (std::regex_match(nameWithoutSuffix, regularExpression))
	{
		nameWithoutSuffix.erase(nameWithoutSuffix.rfind(" ("));
	}
	while (found)
	{
		found = false;
		if (count > 0)
		{
			name = nameWithoutSuffix + " (" + std::to_string(count) + ")";
		}
		else
		{
			name = nameWithoutSuffix;
		}
		for (GameObject* child : gameObject.GetParent()->GetChildren())
		{
			if (child != &gameObject && child->GetName() == name)
			{
				found = true;
			}
		}
		count++;
	}
	gameObject.SetName(name.c_str());
}

HierarchyPanel::HierarchyPanel() : Panel(HIERARCHYPANEL, true) {}

void HierarchyPanel::Draw(int windowFlags)
{
	GameObject* root = EngineApp->GetScene()->GetRoot();
	if (mLastClickedObject == 0)
	{ 
		mLastClickedObject = root->GetID();
	}
	if (mFocusId == 0) 
	{ 
		mFocusId = mLastClickedObject;
	}

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
		//GameObject* marked = root->Find(mLastClickedObject);
		GameObject* marked = App->GetScene()->Find(mLastClickedObject);
		mMarked.insert(marked);
	}
	mLastMarkSeen = 0; mShiftClicked = 0;
	DrawTree(root);
	ImGui::InvisibleButton("##", ImVec2(-1, -1));
	OnRightClickNode(root);
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		mMarked.clear();
		mLastClickedObject = 0;
		InternalSetFocus(root);
	}
	ImGui::EndChild();
	DragAndDropTarget(root);
	
	ImGui::End();
}

void HierarchyPanel::SetFocus(const GameObject& focusedObject) 
{ 
	//App->GetOpenGL()->RemoveHighLight(*GetFocusedObject());
	mUnmarkFlag = true;
	mFocusId = focusedObject.GetID();
	mLastClickedObject = focusedObject.GetID();
	GameObject* parent = focusedObject.GetParent();
	while (parent != nullptr)
	{
		mNodesToOpen.insert(parent->GetID());
		parent = parent->GetParent();
	}
	//App->GetOpenGL()->AddHighLight(focusedObject);
}

void HierarchyPanel::OnLeftClickNode(GameObject* node) 
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
		InternalSetFocus(node);
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
			AddSuffix(*gameObject);
			mLastClickedObject = gameObject->GetID();
			InternalSetFocus(gameObject);
			mMarked.clear();
		}
		bool isPrefabRoot = (EngineApp->GetScene()->IsPrefabScene() && node->mIsRoot);
		if (!node->mIsRoot && !isPrefabRoot) 
		{
			if (ImGui::Selectable("Duplicate")) 
			{
				std::unordered_set<GameObject*> selectAfter;
				for (GameObject* object : FilterMarked()) 
				{
					GameObject* gameObject = App->GetScene()->DuplicateGameObject(object);
					AddSuffix(*gameObject);
					mLastClickedObject = gameObject->GetID();
					InternalSetFocus(gameObject);
					selectAfter.insert(gameObject);
				}
				mMarked = selectAfter;
			}

			if (ImGui::Selectable("Delete")) 
			{
				for (GameObject* object : FilterMarked()) 
				{
					EngineApp->GetScene()->AddGameObjectToDelete(object);
					mLastClickedObject = EngineApp->GetScene()->GetRoot()->GetID();
					InternalSetFocus(EngineApp->GetScene()->GetRoot());
				}
				mMarked.clear();
			}
			if (ImGui::Selectable("Save as Prefab")) 
			{
				for (auto object : FilterMarked()) 
				{
					std::string file = ASSETS_PREFABS_PATH;
					file.append(object->GetName() + ".prfb");
					EngineApp->GetEngineResource()->ImportFile(file.c_str());
				}
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
}

void HierarchyPanel::InternalSetFocus(GameObject* focusedObject)
{
	//App->GetOpenGL()->RemoveHighLight(*GetFocusedObject());
	mFocusId = focusedObject->GetID();
	//App->GetOpenGL()->AddHighLight(*focusedObject);
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
		ImGui::InvisibleButton("##", ImVec2(-1, 5));
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
		std::string treeNodeName = node->mName.c_str();
		treeNodeName += "##" + std::to_string(node->mUid);
		nodeOpen = ImGui::TreeNodeEx(treeNodeName.c_str(), baseFlags) && (node->mChildren.size() > 0);
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
		OnLeftClickNode(node);
		OnRightClickNode(node);
	}
	else 
	{
		if (EngineApp->GetScene()->IsPrefabScene()) 
		{
			if (ImGui::ArrowButton("Go Back", ImGuiDir_Left)) 
			{
				EngineApp->GetScene()->ClosePrefabScreen();
			}
			ImGui::SameLine();
		}
		nodeOpen = ImGui::CollapsingHeader(node->mName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
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
			{ 
				std::vector<GameObject*> gameObjects = FilterMarked();
				for (GameObject* movedObject : gameObjects) 
				{
					bool isParent = false;
					GameObject* targetParent = target->mParent;
					while (targetParent != nullptr)
					{
						if (targetParent->mUid == movedObject->mUid)
						{
							isParent = true;
							break;
						}
						targetParent = targetParent->mParent;
					}

					if (!isParent) 
					{
						
						if (reorder) 
						{ 
							movedObject->mParent->RemoveChild(movedObject->mUid);
							GameObject* parent = target->mParent;
							if (parent != nullptr) 
							{
								for (std::vector<GameObject*>::const_iterator it = parent->mChildren.cbegin(); it != parent->mChildren.cend(); ++it)
								{
									if ((*it)->mUid == target->mUid)
									{
										parent->mChildren.insert(it, movedObject);
										movedObject->mParent = parent;

										// Reorder in scene vector
										App->GetScene()->SwitchGameObjectsFromScene(target, movedObject);

										break;
									}
								}
							}
						}
						else 
						{ 
							App->GetScene()->SwitchGameObjectsFromScene(target, movedObject);
							movedObject->SetParent(target);
						}
					}
				}
			}
		}
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE"))
		{
			AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);
			Resource* resource = EngineApp->GetResource()->RequestResource(asset->mPath);
			if (resource)
			{
				switch (resource->GetType())
				{
				case Resource::Type::Prefab:
				{
					GameObject* newGO = EngineApp->GetScene()->LoadPrefab(asset->mPath, target);
					if (reorder && newGO != nullptr)
					{
						target->RemoveChild(newGO->mUid);
						//target->mParent->AddChild(newGO, target->mUid);
					}
					break;
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
// Use this before doing any operation on all selected items that would already EngineApply to all children
// Ex. When you duplicate an object all it's children are allways duplicated too.
const std::vector<GameObject*> HierarchyPanel::FilterMarked() const 
{
	std::vector<GameObject*> filteredList;
	for (GameObject* object : mMarked) 
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
	GameObject* focus = App->GetScene()->Find(mFocusId);
	if (focus == nullptr) 
	{ 
		return EngineApp->GetScene()->GetRoot();
	}
	else 
	{ 
		return focus; 
	}
}