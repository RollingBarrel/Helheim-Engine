#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "ProjectPanel.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "Component.h"
#include "HierarchyPanel.h"
#include "MeshRendererComponent.h"
#include "ImporterModel.h"
#include "ResourceModel.h"
#include "debugdraw.h"

#include "AnimationComponent.h"


#include "Math/float2.h"
#include "imgui.h"

static void DragToScene(const ModelNode& node, std::vector<unsigned int>& animationUids,GameObject* parent)
{
	const char* name = "";

	if (node.mName == name)
		name = "GameObject";
	else
		name = node.mName.c_str();

	GameObject* gameObject = new GameObject(name, parent);

	gameObject->SetPosition(node.mTranslation);
	gameObject->SetRotation(node.mRotation);
	gameObject->SetScale(node.mScale);
	gameObject->RecalculateMatrices();

	dd::axisTriad(gameObject->GetWorldTransform(),0.1f,1.0f);

	if (node.mMeshId > -1)
	{
		for (int i = 0; i < node.mUids.size(); ++i)
		{
			GameObject* gO = new GameObject(name, gameObject);
			MeshRendererComponent* cMesh = reinterpret_cast<MeshRendererComponent*>(gO->CreateComponent(ComponentType::MESHRENDERER, node.mUids[i].first, node.mUids[i].second));
		}
	}

	if (strcmp(name,"Root") == 0)
	{
		if (!animationUids.empty())
		{
			AnimationComponent* cAnimation = reinterpret_cast<AnimationComponent*>(gameObject->CreateComponent(ComponentType::ANIMATION,0,0,animationUids[0]));
		}
	}

	for (int i = 0; i < node.mChildren.size(); ++i)
	{
		DragToScene(node.mChildren[i], animationUids, gameObject);
	}
}

ScenePanel::ScenePanel() : Panel(SCENEPANEL, true)
{

}

ScenePanel::~ScenePanel()
{
}

void ScenePanel::Draw(int windowFlags)
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
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
		mWindowsSize = float2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
		mMousePosition = float2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);


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
						DragToScene(reinterpret_cast<ResourceModel*>(resource)->GetRoot(), reinterpret_cast<ResourceModel*>(resource)->mAnimationUids, App->GetScene()->GetRoot());
						App->GetResource()->ReleaseResource(resource->GetUID());
						break;
					}
					case Resource::Type::Scene:
						break;
					case Resource::Type::NavMesh:
						break;
					}
				}
			}

			ImGui::EndDragDropTarget();
		}

		//Change the Guizmo operation using W,E & R keywords and the coordinate mode with G
		if (!ImGui::IsKeyDown(ImGuiKey_MouseRight)) {
			if (ImGui::IsKeyPressed(ImGuiKey_W))
				mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
			if (ImGui::IsKeyPressed(ImGuiKey_E))
				mCurrentGuizmoOperation = ImGuizmo::ROTATE;
			if (ImGui::IsKeyPressed(ImGuiKey_R))
				mCurrentGuizmoOperation = ImGuizmo::SCALE;
			if (ImGui::IsKeyPressed(ImGuiKey_G))
				mCurrentGuizmoMode = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
		}
		

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(windowPos.x, windowPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

		const ModuleCamera* camera = App->GetCamera();
		float4x4 cameraView = camera->GetViewMatrix().Transposed();
		float4x4 cameraProjection = camera->GetProjectionMatrix().Transposed();

		GameObject* selectedGameObject = ((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();

		//If there's a selected object in the hierarchy and it's not the root
		if (selectedGameObject && (selectedGameObject != App->GetScene()->GetRoot())) {
			const float4x4* transform = &selectedGameObject->GetWorldTransform();
			float4x4 modelMatrix = selectedGameObject->GetWorldTransform().Transposed();

			//Draws the Guizmo axis
			ImGuizmo::Manipulate(cameraView.ptr(), cameraProjection.ptr(), mCurrentGuizmoOperation, mCurrentGuizmoMode, modelMatrix.ptr(), NULL, mUseSnap ? &mSnap[0] : nullptr);

			if (ImGuizmo::IsUsing()) {
				mIsGuizmoUsign = true;
				GameObject* parent = selectedGameObject->GetParent();
				float4x4 inverseParentMatrix = float4x4::identity;
				float3 translation;
				Quat rotation;
				float3 scale;

				if (parent != nullptr) {
					const float4x4* parentTransform = &parent->GetWorldTransform();
					inverseParentMatrix = parent->GetWorldTransform().Inverted();
				}

				float4x4 localMatrix = inverseParentMatrix * modelMatrix.Transposed();
				localMatrix.Decompose(translation, rotation, scale);

				switch (mCurrentGuizmoOperation) {
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
			else {
				mIsGuizmoUsign = false;
			}
		}

		float viewManipulateRight = windowPos.x + size.x;
		float viewManipulateTop = windowPos.y;
		float viewManipulateSize = 100;

		//TODO: Find the way to only apply the LookAt when pressing the ViewManipulateCube, if not, it causes issues with the free movement camera
		/*
		ImGuizmo::ViewManipulate(cameraView.ptr(), 4, ImVec2(viewManipulateRight - viewManipulateSize, viewManipulateTop), ImVec2(viewManipulateSize, viewManipulateSize), 0x10101010);
		if (ImGui::IsWindowFocused()) {
			float4x4 newCameraView = cameraView.InverseTransposed();
			App->GetCamera()->LookAt(float3(newCameraView.Col(3).xyz()), float3(- newCameraView.Col(2).xyz()), float3(newCameraView.Col(1).xyz()));
		}
		*/
	}
	ImGui::End();
}
