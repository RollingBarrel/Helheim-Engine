#include "ScenePanel.h"
#include "HierarchyPanel.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ProjectPanel.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "Component.h"
#include "HierarchyPanel.h"
#include "MeshRendererComponent.h"
#include "ImporterModel.h"

#include "Math/float2.h"
#include "imgui.h"

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
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE"))
			{
				AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);
				char* path = const_cast<char*>(asset->mName);
				//TODO; Molt malament, fer split del path be!!!
				bool done = false;
				while (*path != '\0')
				{
					if (*path == '.')
					{
						*path = '\0';
						done = true;
						break;
					}
					++path;
				}
				ResourceModel* rModel = new ResourceModel();
				Importer::Model::Load(rModel, asset->mName);
				GameObject* nGO = new GameObject(asset->mName,App->GetScene()->GetRoot());
				for (auto it = rModel->mUids.cbegin(); it != rModel->mUids.cend(); ++it)
				{
					GameObject* go = new GameObject(nGO);
					MeshRendererComponent* cMesh = reinterpret_cast<MeshRendererComponent*>(go->CreateComponent(ComponentType::MESHRENDERER));
					cMesh->Load(it->meshUID, it->materiaUID);
				}
				((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(nGO);
				delete rModel;
				if (done)
					*path = '.';
			}
			ImGui::EndDragDropTarget();
		}

		//Change the Guizmo operation using W,E & R keywords
		if (ImGui::IsKeyPressed(ImGuiKey_W))
			mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_E))
			mCurrentGuizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R))
			mCurrentGuizmoOperation = ImGuizmo::SCALE;
		if (ImGui::IsKeyPressed(ImGuiKey_G))
			mCurrentGuizmoMode = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

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
			ImGuizmo::Manipulate(cameraView.ptr(), cameraProjection.ptr(), mCurrentGuizmoOperation, mCurrentGuizmoMode, modelMatrix.ptr(), NULL, useSnap ? &snap[0] : nullptr);

			if (ImGuizmo::IsUsing()) {
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
				selectedGameObject->RecalculateMatrices();
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
