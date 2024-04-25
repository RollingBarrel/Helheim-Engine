#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "ModuleFileSystem.h"
#include "ProjectPanel.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "Component.h"
#include "CameraComponent.h"
#include "EditorControlPanel.h"
#include "MeshRendererComponent.h"
#include "ImporterModel.h"
#include "ResourceModel.h"
#include "debugdraw.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "Geometry/LineSegment.h"
#include "Quadtree.h"

#include "AnimationComponent.h"


#include "Math/float2.h"
#include "imgui.h"

GameObject* DragToScene(const ModelNode& node, int nodeNumber, ResourceModel& rModel, GameObject* parent, bool isRoot)
{
	const char* name = "";
	static int nodeIt = 0;
	static AnimationComponent* cAnimation = nullptr;

	//Reset nodeIt and cAnimation for the new model dragged to the scene
	if (nodeNumber == 0)
	{
		nodeIt = 0;
		cAnimation = nullptr;
	}

	if (node.mName == name)
		name = "GameObject";
	else
		name = node.mName.c_str();

	GameObject* gameObject = new GameObject(name, parent);

	gameObject->SetPosition(node.mTranslation);
	gameObject->SetRotation(node.mRotation);
	gameObject->SetScale(node.mScale);
	gameObject->RecalculateMatrices();

	if (isRoot && nodeNumber == 0)
	{
		if (!rModel.mAnimationUids.empty())
		{
			if (rModel.mAnimationUids[0] != 0)
			{
				//Defined once by parent after creating the animation component (the first time the function is called parent is gameobjectRoot)
				cAnimation = reinterpret_cast<AnimationComponent*>(gameObject->GetParent()->CreateComponent(ComponentType::ANIMATION));
				cAnimation->SetAnimation(rModel.mAnimationUids[0]);
			}
		}
	}

	if (node.mMeshId > -1)
	{
		for (auto it = node.mUids.cbegin(); it != node.mUids.cend(); ++it)
		{
			if (name == "GameObject") 
			{
				name = "MeshRenderer";
			}
			GameObject* gO = new GameObject(name, gameObject);
			MeshRendererComponent* cMesh = reinterpret_cast<MeshRendererComponent*>(gO->CreateComponent(ComponentType::MESHRENDERER));
			cMesh->SetMesh(it->first);
			cMesh->SetMaterial(it->second);
		}
	}

	if (cAnimation) 
	{
		if (rModel.mJoints[nodeIt].first == nodeNumber)
		{
			std::pair<GameObject*, float4x4> gameobjectPallete = { gameObject, rModel.mJoints[nodeIt].second };
			cAnimation->mGameobjectsInverseMatrices.push_back(gameobjectPallete);
			nodeIt++;
		}
	}

	return gameObject;
}

ScenePanel::ScenePanel() : Panel(SCENEPANEL, true)
{
}

ScenePanel::~ScenePanel()
{
}

static void CheckRaycast()
{
	ScenePanel* scenePanel = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL));

	int mouseAbsoluteX = scenePanel->GetMousePosition().x;
	int mouseAbsoluteY = scenePanel->GetMousePosition().y;

	float normalizedX = -1.0 + 2.0 * (float)(mouseAbsoluteX - scenePanel->GetWindowsPos().x) / (float)scenePanel->GetWindowsSize().x;
	float normalizedY = 1.0 - 2.0 * (float)(mouseAbsoluteY - scenePanel->GetWindowsPos().y) / (float)scenePanel->GetWindowsSize().y;

	LineSegment raySegment = App->GetCamera()->GetCurrentCamera()->GetFrustum().UnProjectLineSegment(normalizedX, normalizedY);

	Ray ray;
	ray.pos = raySegment.a;
	ray.dir = (raySegment.b - raySegment.a);

	bool intersects = false;
	bool intersectsTriangle = false;

	Quadtree* root = App->GetScene()->GetQuadtreeRoot();

	if (!reinterpret_cast<ScenePanel*>(App->GetEditor()->GetPanel(SCENEPANEL))->IsGuizmoUsing())
	{

		std::map<float, GameObject*> hits = root->RayCast(&ray);
		if (!hits.empty())
		{
			const std::pair<float, GameObject*> intersectGameObjectPair = std::pair<float, GameObject*>(hits.begin()->first, hits.begin()->second);
			if (intersectGameObjectPair.second != nullptr)
			{
				GameObject* parentGameObject = intersectGameObjectPair.second;
				while (!parentGameObject->GetParent()->IsRoot())
				{
					parentGameObject = parentGameObject->GetParent();
				}

				GameObject* focusedGameObject = ((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();

				if (focusedGameObject->GetID() == parentGameObject->GetID())
				{
					((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(intersectGameObjectPair.second);
				}
				else
				{
					((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(parentGameObject);
				}
			}
		}
	}
}

void ScenePanel::Draw(int windowFlags)
{
	//TODO: SEPARATE GAME ENGINE
	if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
	{
		CheckRaycast();
	}
	
	
	windowFlags |= ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Game", &mOpen, windowFlags))
	{
		if (ImGui::IsWindowAppearing())
		{
			GameObject* cameraGameObject = App->GetScene()->FindGameObjectWithTag("MainCamera");
			if (cameraGameObject)
			{
				App->GetCamera()->SetCurrentCamera(cameraGameObject);
			}
		}

		DrawScene();
	}
	ImGui::End();


	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		if (ImGui::IsWindowAppearing())
		{
			App->GetCamera()->ActivateEditorCamera();
			
		}

		DrawScene();
		
	}
	ImGui::End();
	
}

void ScenePanel::DrawScene()
{
	isWindowsHovered = ImGui::IsWindowHovered();
	ImVec2 size = ImGui::GetContentRegionAvail();
	if (size.x != prevSizeX || size.y != prevSizeY)
	{
		App->GetOpenGL()->SceneFramebufferResized(size.x, size.y);
		prevSizeX = size.x;
		prevSizeY = size.y;
	}
	ImGui::Image((void*)(intptr_t)App->GetOpenGL()->GetFramebufferTexture(), size, ImVec2(0, 1), ImVec2(1, 0));

	mWindowsPosition = float2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	App->GetWindow()->SetGameWindowsPosition(mWindowsPosition);
	mWindowsSize = float2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	App->GetWindow()->GameWindowsResized(mWindowsSize);
	mMousePosition = float2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	if (!mMousePosition.Equals(float2(-FLT_MAX, -FLT_MAX)))
	{
		App->GetInput()->SetGameMousePosition(mMousePosition);
	}
	

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE");
		if (payload)
		{
			AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);

			Resource* resource = App->GetResource()->RequestResource(asset->mPath);
			if (resource)
			{
				switch (resource->GetType())
				{
				case Resource::Type::Texture:
					break;
				case Resource::Type::Mesh:
					break;
				case Resource::Type::Bone:
					break;
				case Resource::Type::Animation:
					break;
				case Resource::Type::Material:
					break;
				case Resource::Type::Model:
				{
					std::string name;

					App->GetFileSystem()->SplitPath(asset->mPath, &name);

					GameObject* gameObjectRoot = new GameObject(name.c_str(), App->GetScene()->GetRoot());

					std::vector<GameObject*> tempVec;

					tempVec.reserve(reinterpret_cast<ResourceModel*>(resource)->GetNodes().size());

					for (int i = 0; i < tempVec.capacity(); ++i)
					{
						ModelNode node = reinterpret_cast<ResourceModel*>(resource)->GetNodes()[i];
						if (node.mParentIndex == -1)
							tempVec.push_back(DragToScene(node, i, *(reinterpret_cast<ResourceModel*>(resource)), gameObjectRoot, true));
						else
							tempVec.push_back(DragToScene(node, i, *(reinterpret_cast<ResourceModel*>(resource)), tempVec.at(node.mParentIndex), false));
					
						//for (int j = 0; j < reinterpret_cast<ResourceModel*>(resource)->mJoints.size(); ++j)
						//{
						//	if (reinterpret_cast<ResourceModel*>(resource)->mJoints[j].first == i)
						//	{
						//		LOG("Current Node: %u, %s", i, node.mName.c_str());
						//		LOG("Joint Index: %d", reinterpret_cast<ResourceModel*>(resource)->mJoints[j].first);
						//		break;
						//	}
						//}
					}

					tempVec.clear();

					App->GetResource()->ReleaseResource(resource->GetUID());
					break;
				}
				case Resource::Type::Scene:
					break;
				case Resource::Type::Object:
				{
					App->GetScene()->LoadPrefab(asset->mPath, resource->GetUID());
					break;
				}
				case Resource::Type::NavMesh:
					break;
				}
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGuizmo::OPERATION currentGuizmoOperation = ((EditorControlPanel*)App->GetEditor()->GetPanel(EDITORCONTROLPANEL))->GetGuizmoOperation();
	ImGuizmo::MODE currentGuizmoMode = ((EditorControlPanel*)App->GetEditor()->GetPanel(EDITORCONTROLPANEL))->GetGuizmoMode();
	bool useSnap = ((EditorControlPanel*)App->GetEditor()->GetPanel(EDITORCONTROLPANEL))->GetUseSnap();
	float3 snap = ((EditorControlPanel*)App->GetEditor()->GetPanel(EDITORCONTROLPANEL))->GetSnap();

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(windowPos.x, windowPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

	const CameraComponent* camera = App->GetCamera()->GetEditorCamera();
	float4x4 cameraView = camera->GetViewMatrix().Transposed();
	float4x4 cameraProjection = camera->GetProjectionMatrix().Transposed();

	GameObject* selectedGameObject = ((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();

	//If there's a selected object in the hierarchy and it's not the root
	if (selectedGameObject && (selectedGameObject != App->GetScene()->GetRoot())) 
	{
		const float4x4* transform = &selectedGameObject->GetWorldTransform();
		float4x4 modelMatrix = selectedGameObject->GetWorldTransform().Transposed();

		//Draws the Guizmo axis
		ImGuizmo::Manipulate(cameraView.ptr(), cameraProjection.ptr(), currentGuizmoOperation, currentGuizmoMode, modelMatrix.ptr(), NULL, useSnap ? &snap[0] : nullptr);

		if (ImGuizmo::IsUsing()) 
		{
			mIsGuizmoUsing = true;
			GameObject* parent = selectedGameObject->GetParent();
			float4x4 inverseParentMatrix = float4x4::identity;
			float3 translation;
			Quat rotation;
			float3 scale;

			if (parent != nullptr) 
			{
				const float4x4* parentTransform = &parent->GetWorldTransform();
				inverseParentMatrix = parent->GetWorldTransform().Inverted();
			}

			float4x4 localMatrix = inverseParentMatrix * modelMatrix.Transposed();
			localMatrix.Decompose(translation, rotation, scale);

			switch (currentGuizmoOperation) 
			{
			case ImGuizmo::TRANSLATE:
				selectedGameObject->SetPosition(translation);
				break;
			case ImGuizmo::ROTATE:
				selectedGameObject->SetRotation(rotation);
				break;
			case ImGuizmo::SCALE:
				selectedGameObject->SetScale(scale);
				break;
			}
		}
		else 
		{
			mIsGuizmoUsing = false;
		}
	}

	//TODO: Find the way to only apply the LookAt when pressing the ViewManipulateCube, if not, it causes issues with the free movement camera
	/*
	float viewManipulateRight = windowPos.x + size.x;
	float viewManipulateTop = windowPos.y;
	float viewManipulateSize = 100;

	ImGuizmo::ViewManipulate(cameraView.ptr(), 4, ImVec2(viewManipulateRight - viewManipulateSize, viewManipulateTop), ImVec2(viewManipulateSize, viewManipulateSize), 0x10101010);
	if (ImGui::IsWindowFocused()) {
		float4x4 newCameraView = cameraView.InverseTransposed();
		App->GetCamera()->LookAt(float3(newCameraView.Col(3).xyz()), float3(- newCameraView.Col(2).xyz()), float3(newCameraView.Col(1).xyz()));
	}
	*/


}
