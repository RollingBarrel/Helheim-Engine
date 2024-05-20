#include "DebugPanel.h"
#include "imgui.h"
#include "EngineApp.h"
#include "ModuleEngineCamera.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "ModuleOpenGL.h"
#include "ModuleAudio.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "Panel.h"
#include "Timer.h"
#include "TimerPanel.h"
#include "HierarchyPanel.h"
#include "ResourceMesh.h"
#include "Quadtree.h"

DebugPanel::DebugPanel() : Panel(DEBUGPANEL, false)
{
}

DebugPanel::~DebugPanel()
{
}

static void RenderTreeImGui(const Quadtree* qTree)
{
    if (strcmp(qTree->GetName(), "") == 0)
        return;
    bool treeNodeOpened = ImGui::TreeNode(qTree->GetName());

    if (qTree->IsFilled() && treeNodeOpened)
    {
        const Quadtree* children = qTree->GetChildren();
        for (int i = 0; i < 4; ++i)
        {
            RenderTreeImGui(children + i);
        }
    }
    else
    {
        if (treeNodeOpened)
        {
            for (const GameObject* object : qTree->GetGameObjects())
            {
                ImGui::Text(object->GetName().c_str());
            }
        }
    }

    if (treeNodeOpened)
        ImGui::TreePop();
}

static void DrawQuadTree(const Quadtree* qTree)
{
    EngineApp->GetDebugDraw()->DrawCube(qTree->GetBoundingBox(), float3(0.980392f, 0.980392f, 0.823529f)); // LightGoldenYellow
    if (qTree->IsFilled())
    {
        const Quadtree* children = qTree->GetChildren();
        for (int i = 0; i < 4; ++i)
        {
            DrawQuadTree(children + i);
        }
    }
}

void DebugPanel::Draw(int windowFlags) {
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
        if (ImGui::TreeNode("Editor Scene##2"))
        {
            ImGui::Text("Render Mode");
            static const char* mRenderOptions[3] = { "Shaded", "Wireframe", "Shaded + Wireframe" };
            if (ImGui::Combo(" ", (int*)&mRenderMode, mRenderOptions, IM_ARRAYSIZE(mRenderOptions)))
            {
                switch (mRenderMode)
                {
                case RenderMode::Shaded:
                    EngineApp->GetOpenGL()->SetWireframe(false);
                    break;
                case RenderMode::Wireframe:
                    EngineApp->GetOpenGL()->SetWireframe(true);
                    break;
                case RenderMode::ShadedWireframe:
                    //TODO Shaded + Wireframe rendering
                    break;
                default:
                    break;
                }
            }
            if (ImGui::Checkbox("Draw Colliders", &mDrawColliders))
            {
                GameObject* root = EngineApp->GetScene()->GetRoot();
                SetShouldDrawForAll(root, mDrawColliders);
            }
            ImGui::Text("Total number of triangles on scene: %i", GetTotalTriangleCount(EngineApp->GetScene()->GetRoot()));
            ImGui::TreePop();
		}

        // TODO Frames section with a button to show the FPS on the editor window (top right corner)
        /*if (ImGui::TreeNode("Frames##2"))
        {
            if (ImGui::Checkbox("Show fps on editor", &mShowFpsOnEditor))
            {
                
            }
            ImGui::TreePop();
        }*/

        if (ImGui::TreeNode("Quadtree##2"))
        {
            static bool draw = false;
            ImGui::Checkbox("Draw quadtree", &draw);

            if (draw)
            {
                App->GetOpenGL()->BindSceneFramebuffer();
                DrawQuadTree(App->GetScene()->GetQuadtreeRoot());
                App->GetOpenGL()->UnbindSceneFramebuffer();
            }

            ImGui::Separator();
            ImGui::SameLine();
            if (ImGui::Button("Reload quadtree"))
            {
                EngineApp->GetScene()->GetQuadtreeRoot()->UpdateTree();
            }
            ImGui::Separator();
            ImGui::Text("Quadtree nodes:");
            RenderTreeImGui(EngineApp->GetScene()->GetQuadtreeRoot());
            ImGui::TreePop();
		}

        if (ImGui::TreeNode("Audio##2"))
        {
            ImGui::Text("FMOD Memory usage: %.6f MB", (float)App->GetAudio()->GetMemoryUsage() / (1000000));

            float value = App->GetAudio()->GetVolume("bus:/");
            if (ImGui::SliderFloat("Main Volume", &value, 0, 1, "%.1f"))
            {
                App->GetAudio()->SetVolume("bus:/", value);
            }

            ImGui::TreePop();
        }


        if (ImGui::TreeNode("Others##2"))
        {
            if (ImGui::Checkbox("Draw Mouse Picking RayCast", &mDrawRaycast)) 
            {
                EngineApp->GetEngineCamera()->DrawRaycast(mDrawRaycast);
            }
            ImGui::TreePop();
        }
	}
	ImGui::End();
    if (mDrawColliders) 
    {
        DrawColliders(EngineApp->GetScene()->GetRoot());
    }
}

void DebugPanel::SetShouldDrawForAll(GameObject* root, bool shouldDraw) 
{
    if (root != nullptr) 
    {
        //TODO: SEPARATE GAME ENGINE
        //MeshRendererComponent* renderer = (MeshRendererComponent*)root->GetComponent(ComponentType::MESHRENDERER);
        //if (renderer != nullptr) 
        //{
        //    renderer->SetShouldDraw(shouldDraw);
        //}

        for (int i = 0; i < root->GetChildren().size(); i++) 
        {
            SetShouldDrawForAll(root->GetChildren()[i], shouldDraw);
        }
    }
}

int DebugPanel::GetTotalTriangleCount(GameObject* root) 
{
	int total = 0;
	if (root != nullptr) 
    {
		MeshRendererComponent* renderer = (MeshRendererComponent*)root->GetComponent(ComponentType::MESHRENDERER);
		if (renderer != nullptr) 
        {
			total += renderer->GetResourceMesh()->GetNumberIndices() / 3;
		}

		for (int i = 0; i < root->GetChildren().size(); i++) 
        {
			total += GetTotalTriangleCount(root->GetChildren()[i]);
		}
	}
	return total;
}
void DebugPanel::DrawColliders(GameObject* root)
{
    if (root != nullptr)
    {
        MeshRendererComponent* meshRenderer = (MeshRendererComponent*)root->GetComponent(ComponentType::MESHRENDERER);
        //TODO: SEPARATE GAME ENGINE
        //if (meshRenderer != nullptr && meshRenderer->ShouldDraw()) 
        //{
        //    EngineApp->GetDebugDraw()->DrawCube(meshRenderer->getOBB(), float3(0.0f, 0.0f, 1.0f)); //Blue
        //    EngineApp->GetDebugDraw()->DrawCube(meshRenderer->GetAABBWorld(), float3(1.0f, 0.65f, 0.0f)); //Orange
        //}

        for (int i = 0; i < root->GetChildren().size(); i++)
        {
            DrawColliders(root->GetChildren()[i]);
        }
    }
}