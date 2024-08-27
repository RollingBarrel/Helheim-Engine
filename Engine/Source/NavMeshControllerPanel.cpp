#include "NavMeshControllerPanel.h"
#include "imgui.h"
#include "EngineApp.h"
#include "glew.h"
#include "NavMeshController.h"
#include "ModuleDetourNavigation.h"
#include "ModuleDebugDraw.h"
#include "ModuleOpenGL.h"
#include "ModuleEngineCamera.h"
#include "CameraComponent.h"
#include "float4x4.h"
NavMeshControllerPanel::NavMeshControllerPanel() : Panel(NAVMESHPANEL, false)
{
}

NavMeshControllerPanel::~NavMeshControllerPanel()
{
}

void NavMeshControllerPanel::Draw(int windowFlags)
{
    mNavController = EngineApp->GetNavController();
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{


        //IMGUI VALUES
        if (ImGui::Checkbox("Debug draw navmesh", &mDraw))
        {
        }
        float cellSize = mNavController->GetCellSize();
        if (ImGui::DragFloat("Cell Size", &cellSize, 0.01f, 0.1f, 1.0f))
        {
            mNavController->SetCellSize(cellSize);
        }

        float cellHeight = mNavController->GetCellHeight();
        if (ImGui::DragFloat("Cell Height", &cellHeight, 0.01f, 0.1f, 1.0f))
        {
            mNavController->SetCellHeight(cellHeight);
        }

        int maxSlopeAngle = mNavController->GetMaxSlopeAngle(); // Be careful with unsigned int?
        if (ImGui::DragInt("Max Slope Angle", &maxSlopeAngle, 1, 0, 90))
        {
            mNavController->SetMaxSlopeAngle(maxSlopeAngle);
        }

        float walkableRadius = mNavController->GetWalkableRadius();
        if (ImGui::DragFloat("Walkable Radius", &walkableRadius, 0.1f, 0.0f, 5.0f))
        {
            mNavController->SetWalkableRadius(walkableRadius);
        }

        int minRegionArea = mNavController->GetMinRegionArea();
        if (ImGui::DragInt("Min Region Area", &minRegionArea, 1, 0, 150))
        {
            mNavController->SetMinRegionArea(minRegionArea);
        }

        int mergeRegionArea = mNavController->GetMergeRegionArea();
        if (ImGui::DragInt("Merge Region Area", &mergeRegionArea, 1, 0, 150))
        {
            mNavController->SetMergeRegionArea(mergeRegionArea);
        }

        float maxSimplificationError = mNavController->GetMaxSimplificationError();
        if (ImGui::DragFloat("Max Simplification Error", &maxSimplificationError, 0.1f, 0.1f, 3.0f))
        {
            mNavController->SetMaxSimplificationError(maxSimplificationError);
        }

        int maxEdgeLen = mNavController->GetMaxEdgeLen();
        if (ImGui::DragInt("Max Edge Length", &maxEdgeLen, 1, 0, 50))
        {
            mNavController->SetMaxEdgeLen(maxEdgeLen);
        }

        int maxVertsPerPoly = mNavController->GetMaxVertsPerPoly();
        if (ImGui::DragInt("Max Verts Per Poly", &maxVertsPerPoly, 1, 3, 12))
        {
            mNavController->SetMaxVertsPerPoly(maxVertsPerPoly);
        }

        float detailSampleDist = mNavController->GetDetailSampleDist();
        if (ImGui::DragFloat("Detail Sample Distance", &detailSampleDist, 0.1f, 0.0f, 16.0f))
        {
            mNavController->SetDetailSampleDist(detailSampleDist);
        }

        float detailSampleMaxError = mNavController->GetDetailSampleMaxError();
        if (ImGui::DragFloat("Detail Sample Max Error", &detailSampleMaxError, 0.1f, 0.0f, 16.0f))
        {
            mNavController->SetDetailSampleMaxError(detailSampleMaxError);
        }
        if (ImGui::Button("Build Navigation"))
        {
            mVao = 0;
            mNavController->HandleBuild();
        }


        ImGui::Text("Player simulation");

        float3 playerPos = EngineApp->GetNavigation()->GetQueryCenter();
        if (ImGui::DragFloat3("Character position: ", &playerPos[0], 0.5f, -1000.0f, 1000.0f))
        {
            EngineApp->GetNavigation()->SetQueryCenter(playerPos);
        }

        float3 navQuerySize = EngineApp->GetNavigation()->GetQueryHalfSize();
        if (ImGui::DragFloat3("Query size: ", &navQuerySize[0], 0.5f, 0.0f, 500.0f))
        {
            EngineApp->GetNavigation()->SetQueryHalfSize(navQuerySize);
        }


	}
	ImGui::End();
    if (mDraw)
    {
        if (mVao == 0)
        {
          LoadMeshVAO();
        }
        App->GetOpenGL()->BindSceneFramebuffer();
        DrawDebugPolyMesh();
        App->GetOpenGL()->UnbindFramebuffer();
        App->GetNavigation()->FindDebugPoint();
    }
}

void NavMeshControllerPanel::DrawDebugPolyMesh()
{
    NavMeshController* navController = EngineApp->GetNavController();
    unsigned int program = App->GetOpenGL()->GetDebugDrawProgramId();
    float4x4 identity = float4x4::identity;
    float4x4 view = EngineApp->GetEngineCamera()->GetEditorCamera()->GetViewMatrix();

    float4x4 proj = EngineApp->GetEngineCamera()->GetEditorCamera()->GetProjectionMatrix();

    GLint viewLoc = glGetUniformLocation(program, "view");
    GLint projLoc = glGetUniformLocation(program, "proj");
    GLint modelLoc = glGetUniformLocation(program, "model");
    glBindVertexArray(0);
    glUseProgram(0);
    glUseProgram(program);

    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, &proj[0][0]);
    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, &identity[0][0]);

    glBindVertexArray(mVao);
    glDrawElements(GL_TRIANGLES, mNavController->GetIndices().size(), GL_UNSIGNED_INT, 0);
    float3 color = float3(1.0f, 0.0f, 0.0f);
    EngineApp->GetDebugDraw()->DrawSphere(&EngineApp->GetNavigation()->GetQueryResult()[0], &color[0], 1.0f);
    float3 color2 = float3(1.0f, 1.0f, 0.0f);
    EngineApp->GetDebugDraw()->DrawSphere(&EngineApp->GetNavigation()->GetQueryCenter()[0], &color2[0], 1.0f);
    float3 color3 = float3(0.0f, 0.0f, 1.0f);
    float3 minAABB = EngineApp->GetNavigation()->GetQueryCenter() - EngineApp->GetNavigation()->GetQueryHalfSize();
    float3 maxAABB = EngineApp->GetNavigation()->GetQueryCenter() + EngineApp->GetNavigation()->GetQueryHalfSize();
    OBB cube = OBB(AABB(minAABB, maxAABB));
    EngineApp->GetDebugDraw()->DrawCube(cube, color3);


}

void NavMeshControllerPanel::LoadMeshVAO()
{

    if (mNavController->GetVertices().size()>0)
    {
        // Now you can create the VAO and fill it with the mesh data
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);


        glGenBuffers(1, &mVbo);
        glBindBuffer(GL_ARRAY_BUFFER, mVbo);
        glBufferData(GL_ARRAY_BUFFER, 3 * mNavController->GetVertices().size() * sizeof(float), &mNavController->GetVertices()[0], GL_STATIC_DRAW);


        glGenBuffers(1, &mEbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNavController->GetIndices().size() * sizeof(unsigned int), &mNavController->GetIndices()[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);


        glBindVertexArray(0);

    }
    return;
}