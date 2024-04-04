#include "NavMeshControllerPanel.h"
#include "imgui.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleScene.h"
#include "NavMeshController.h"
#include "ModuleDetourNavigation.h"


NavMeshControllerPanel::NavMeshControllerPanel() : Panel(NAVMESHPANEL, false)
{
}

NavMeshControllerPanel::~NavMeshControllerPanel()
{
}

void NavMeshControllerPanel::Draw(int windowFlags)
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
        NavMeshController* navController = App->GetScene()->GetNavController();

        //IMGUI VALUES
        bool draw = navController->GetShouldDraw();
        if (ImGui::Checkbox("Debug draw navmesh", &draw))
        {
            navController->SetShouldDraw(draw);
        }


        float cellSize = navController->GetCellSize();
        if (ImGui::DragFloat("Cell Size", &cellSize, 0.01f, 0.1f, 1.0f))
        {
            navController->SetCellSize(cellSize);
        }

        float cellHeight = navController->GetCellHeight();
        if (ImGui::DragFloat("Cell Height", &cellHeight, 0.01f, 0.1f, 1.0f))
        {
            navController->SetCellHeight(cellHeight);
        }

        int maxSlopeAngle = navController->GetMaxSlopeAngle(); // Be careful with unsigned int?
        if (ImGui::DragInt("Max Slope Angle", &maxSlopeAngle, 1, 0, 90))
        {
            navController->SetMaxSlopeAngle(maxSlopeAngle);
        }

        float walkableRadius = navController->GetWalkableRadius();
        if (ImGui::DragFloat("Walkable Radius", &walkableRadius, 0.1f, 0.0f, 5.0f))
        {
            navController->SetWalkableRadius(walkableRadius);
        }

        int minRegionArea = navController->GetMinRegionArea();
        if (ImGui::DragInt("Min Region Area", &minRegionArea, 1, 0, 150))
        {
            navController->SetMinRegionArea(minRegionArea);
        }

        int mergeRegionArea = navController->GetMergeRegionArea();
        if (ImGui::DragInt("Merge Region Area", &mergeRegionArea, 1, 0, 150))
        {
            navController->SetMergeRegionArea(mergeRegionArea);
        }

        float maxSimplificationError = navController->GetMaxSimplificationError();
        if (ImGui::DragFloat("Max Simplification Error", &maxSimplificationError, 0.1f, 0.1f, 3.0f))
        {
            navController->SetMaxSimplificationError(maxSimplificationError);
        }

        int maxEdgeLen = navController->GetMaxEdgeLen();
        if (ImGui::DragInt("Max Edge Length", &maxEdgeLen, 1, 0, 50))
        {
            navController->SetMaxEdgeLen(maxEdgeLen);
        }

        int maxVertsPerPoly = navController->GetMaxVertsPerPoly();
        if (ImGui::DragInt("Max Verts Per Poly", &maxVertsPerPoly, 1, 3, 12))
        {
            navController->SetMaxVertsPerPoly(maxVertsPerPoly);
        }

        float detailSampleDist = navController->GetDetailSampleDist();
        if (ImGui::DragFloat("Detail Sample Distance", &detailSampleDist, 0.1f, 0.0f, 16.0f))
        {
            navController->SetDetailSampleDist(detailSampleDist);
        }

        float detailSampleMaxError = navController->GetDetailSampleMaxError();
        if (ImGui::DragFloat("Detail Sample Max Error", &detailSampleMaxError, 0.1f, 0.0f, 16.0f))
        {
            navController->SetDetailSampleMaxError(detailSampleMaxError);
        }
        if (ImGui::Button("Build Navigation"))
        {
            navController->HandleBuild();
        }


        ImGui::Text("Player simulation");

        float3 playerPos = App->GetNavigation()->GetQueryCenter();
        if (ImGui::DragFloat3("Character position: ", &playerPos[0], 0.5f, -1000.0f, 1000.0f))
        {
            App->GetNavigation()->SetQueryCenter(playerPos);
        }

        float3 navQuerySize = App->GetNavigation()->GetQueryHalfSize();
        if (ImGui::DragFloat3("Query size: ", &navQuerySize[0], 0.5f, 0.0f, 500.0f))
        {
            App->GetNavigation()->SetQueryHalfSize(navQuerySize);
        }


	}
	ImGui::End();
}

