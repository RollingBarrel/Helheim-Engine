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

		ImVec2 framebufferPosition = ImGui::GetWindowPos();
		framebufferPosition.y += (ImGui::GetWindowHeight() - size.y);

		float viewManipulateRight = framebufferPosition.x + ImGui::GetContentRegionAvail().x;
		float viewManipulateTop = framebufferPosition.y;

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(framebufferPosition.x, framebufferPosition.y, ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

		const Frustum* engineFrustum = &App->GetCamera()->GetFrustum();
		float4x4 cameraView = float4x4(engineFrustum->ViewMatrix()).Transposed();
		float4x4 cameraProjection = engineFrustum->ProjectionMatrix().Transposed();

		GameObject* selectedGameObject = ((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();

		if (selectedGameObject && (selectedGameObject != App->GetScene()->GetRoot())) {
			const float4x4* transform = &selectedGameObject->GetWorldTransform();
			float4x4 globalMatrix = selectedGameObject->GetWorldTransform().Transposed();

			ImGuizmo::Manipulate(cameraView.ptr(), cameraProjection.ptr(), mCurrentGuizmoOperation, mCurrentGuizmoMode, globalMatrix.ptr(), NULL, 0);

			if (ImGuizmo::IsUsing()) {
				GameObject* parent = selectedGameObject->GetParent();
				float4x4 inverseParentMatrix = float4x4::identity;
				if (parent != nullptr) {
					const float4x4* parentTransform = &parent->GetWorldTransform();
					inverseParentMatrix = parent->GetWorldTransform().Inverted();
				}
				float4x4 localMatrix = inverseParentMatrix * globalMatrix.Transposed();

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
			}
		}
	}
	ImGui::End();
}
