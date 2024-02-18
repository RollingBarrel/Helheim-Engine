#include "ScenePanel.h"
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

		ImVec2 windowPos = ImGui::GetWindowPos();
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(windowPos.x, windowPos.y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

		const ModuleCamera* camera = App->GetCamera();
		float4x4 cameraView = camera->GetViewMatrix().Transposed();
		float4x4 cameraProjection = camera->GetProjectionMatrix().Transposed();

		GameObject* selectedGameObject = ((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();

		if (selectedGameObject && (selectedGameObject != App->GetScene()->GetRoot())) {
			const float4x4* transform = &selectedGameObject->GetWorldTransform();
			float4x4 modelMatrix = selectedGameObject->GetWorldTransform().Transposed();

			ImGuizmo::Manipulate(cameraView.ptr(), cameraProjection.ptr(), mCurrentGuizmoOperation, mCurrentGuizmoMode, modelMatrix.ptr(), NULL, useSnap ? &snap[0] : nullptr);

			if (ImGuizmo::IsUsing()) {
				GameObject* parent = selectedGameObject->GetParent();
				float4x4 inverseParentMatrix = float4x4::identity;
				if (parent != nullptr) {
					const float4x4* parentTransform = &parent->GetWorldTransform();
					inverseParentMatrix = parent->GetWorldTransform().Inverted();
				}
				float4x4 localMatrix = inverseParentMatrix * modelMatrix.Transposed();

				float3 translation;
				Quat rotation;
				float3 scale;
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
	}
	ImGui::End();
}
