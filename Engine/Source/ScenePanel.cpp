#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "EngineApp.h"
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
				cAnimation->SetModelUUID(rModel.GetUID());

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

	return gameObject;
}

ScenePanel::ScenePanel() : Panel(SCENEPANEL, true)
{
}

ScenePanel::~ScenePanel()
{
}

void ScenePanel::Draw(int windowFlags)
{
	windowFlags |= ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Game", &mOpen, windowFlags))
	{
		if (ImGui::IsWindowAppearing())
		{
			GameObject* cameraGameObject = EngineApp->GetScene()->FindGameObjectWithTag("MainCamera");
			if (cameraGameObject)
			{
				EngineApp->GetCamera()->SetCurrentCamera(cameraGameObject);
			}
		}

		DrawScene();
	}
	ImGui::End();


	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		if (ImGui::IsWindowAppearing())
		{
			EngineApp->GetCamera()->ActivateEditorCamera();

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
		EngineApp->GetOpenGL()->SceneFramebufferResized(size.x, size.y);
		prevSizeX = size.x;
		prevSizeY = size.y;
	}
	if (currentScene == 0)
	{
		currentScene = EngineApp->GetOpenGL()->GetFramebufferTexture();
		currentSceneName = "SCENE";
	}
	ImGui::Image((void*)(intptr_t)currentScene, size, ImVec2(0, 1), ImVec2(1, 0));
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE");
		if (payload)
		{
			AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);

			Resource* resource = EngineApp->GetResource()->RequestResource(asset->mPath);
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

					EngineApp->GetFileSystem()->SplitPath(asset->mPath, &name);

					GameObject* gameObjectRoot = new GameObject(name.c_str(), EngineApp->GetScene()->GetRoot());

					std::vector<GameObject*> tempVec;
					ResourceModel* rModel = reinterpret_cast<ResourceModel*>(resource);
					
					tempVec.reserve(rModel->GetNodes().size());

					for (int i = 0; i < rModel->GetNodes().size(); ++i)
					{
						ModelNode node = rModel->GetNodes()[i];
						if (node.mParentIndex == -1)
							tempVec.push_back(DragToScene(node, i, *rModel, gameObjectRoot, true));
						else
							tempVec.push_back(DragToScene(node, i, *rModel, tempVec.at(node.mParentIndex), false));

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

					EngineApp->GetResource()->ReleaseResource(resource->GetUID());
					break;
				}
				case Resource::Type::Scene:
					break;
				case Resource::Type::Object:
				{
					EngineApp->GetScene()->LoadPrefab(asset->mPath, resource->GetUID());
					break;
				}
				case Resource::Type::NavMesh:
					break;
				}
			}
		}

		ImGui::EndDragDropTarget();
	}
	if (ImGui::BeginCombo("##Screen Output", currentSceneName.c_str(), ImGuiComboFlags_PopupAlignLeft))
	{
		if (ImGui::Selectable("SCENE")) {
			currentScene = EngineApp->GetOpenGL()->GetFramebufferTexture();
			currentSceneName = "SCENE";
		}
		//if (ImGui::Selectable("DEPTH")) {
		//	currentScene = EngineApp->GetOpenGL()->GetGBufferDepth();
		//	currentSceneName = "DEPTH";
		//}
		if (ImGui::Selectable("DIFFUSE")) {
			currentScene = EngineApp->GetOpenGL()->GetGBufferDiffuse();
			currentSceneName = "DIFFUSE";
		}
		if (ImGui::Selectable("SPECULAR")) {
			currentScene = EngineApp->GetOpenGL()->GetGBufferSpecularRough();
			currentSceneName = "SPECULAR";
		}
		if (ImGui::Selectable("EMISSIVE")) {
			currentScene = EngineApp->GetOpenGL()->GetGBufferEmissive();
			currentSceneName = "EMISSIVE";
		}
		if (ImGui::Selectable("NORMALS")) {
			currentScene = EngineApp->GetOpenGL()->GetGBufferNormals();
			currentSceneName = "NORMALS";
		}
		if (ImGui::Selectable("POSITIONS")) {
			currentScene = EngineApp->GetOpenGL()->GetGBufferPositions();
			currentSceneName = "POSITIONS";
		}
		ImGui::EndCombo();
	}

	mWindowsPosition = float2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
	EngineApp->GetWindow()->SetGameWindowsPosition(mWindowsPosition);
	mWindowsSize = float2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
	EngineApp->GetWindow()->GameWindowsResized(mWindowsSize);
	mMousePosition = float2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	if (!mMousePosition.Equals(float2(-FLT_MAX, -FLT_MAX)))
	{
		EngineApp->GetInput()->SetGameMousePosition(mMousePosition);
	}


	ImGuizmo::OPERATION currentGuizmoOperation = ((EditorControlPanel*)EngineApp->GetEditor()->GetPanel(EDITORCONTROLPANEL))->GetGuizmoOperation();
	ImGuizmo::MODE currentGuizmoMode = ((EditorControlPanel*)EngineApp->GetEditor()->GetPanel(EDITORCONTROLPANEL))->GetGuizmoMode();
	bool useSnap = ((EditorControlPanel*)EngineApp->GetEditor()->GetPanel(EDITORCONTROLPANEL))->GetUseSnap();
	float3 snap = ((EditorControlPanel*)EngineApp->GetEditor()->GetPanel(EDITORCONTROLPANEL))->GetSnap();

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(windowPos.x, windowPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

	const CameraComponent* camera = EngineApp->GetCamera()->GetEditorCamera();
	float4x4 cameraView = camera->GetViewMatrix().Transposed();
	float4x4 cameraProjection = camera->GetProjectionMatrix().Transposed();

	GameObject* selectedGameObject = ((HierarchyPanel*)EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();

	//If there's a selected object in the hierarchy and it's not the root
	if (selectedGameObject && (selectedGameObject != EngineApp->GetScene()->GetRoot()))
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

	//TODO: Find the way to only EngineApply the LookAt when pressing the ViewManipulateCube, if not, it causes issues with the free movement camera
	/*
	float viewManipulateRight = windowPos.x + size.x;
	float viewManipulateTop = windowPos.y;
	float viewManipulateSize = 100;

	ImGuizmo::ViewManipulate(cameraView.ptr(), 4, ImVec2(viewManipulateRight - viewManipulateSize, viewManipulateTop), ImVec2(viewManipulateSize, viewManipulateSize), 0x10101010);
	if (ImGui::IsWindowFocused()) {
		float4x4 newCameraView = cameraView.InverseTransposed();
		EngineApp->GetCamera()->LookAt(float3(newCameraView.Col(3).xyz()), float3(- newCameraView.Col(2).xyz()), float3(newCameraView.Col(1).xyz()));
	}
	*/


}