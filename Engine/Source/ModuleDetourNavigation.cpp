#include "ModuleDetourNavigation.h"
#include "Application.h"
#include "GameObject.h"
#include "NavMeshController.h"
#include "AIAgentComponent.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMesh.h"
#include "ModuleScene.h"
#include "DetourNavMeshQuery.h"
#include "ModuleDebugDraw.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "ModuleOpenGL.h"

ModuleDetourNavigation::ModuleDetourNavigation()
{
	mNavMeshParams = new dtNavMeshCreateParams();
	mNavQuery = new dtNavMeshQuery();
}

ModuleDetourNavigation::~ModuleDetourNavigation()
{
	delete mNavMeshParams;
	delete mNavQuery;
}

bool ModuleDetourNavigation::Init()
{

	return true;
}

update_status ModuleDetourNavigation::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleDetourNavigation::Update(float dt)
{

	if (mNavQuery && mDetourNavMesh)
	{
		App->GetOpenGL()->BindSceneFramebuffer();
		dtPolyRef result;
		dtQueryFilter temp;
		//mNavQuery->findNearestPoly(&mQueryCenter[0], &mQueryHalfSize[0], &temp, &result, &mQueryResult[0]);
		//float3 color = float3(1.0f, 0.0f, 0.0f);
		//App->GetDebugDraw()->DrawSphere(&mQueryResult[0], &color[0], 1.0f);

		float3 color2 = float3(1.0f, 1.0f, 0.0f);
		App->GetDebugDraw()->DrawSphere(&mQueryCenter[0], &color2[0], 1.0f);

		float3 color3 = float3(0.0f, 0.0f, 1.0f);
		float3 minAABB = mQueryCenter - mQueryHalfSize;
		float3 maxAABB = mQueryCenter + mQueryHalfSize;
		OBB cube = OBB(AABB(minAABB, maxAABB));
		App->GetDebugDraw()->DrawCube(cube, color3);
		App->GetOpenGL()->UnbindSceneFramebuffer();


	}
	return UPDATE_CONTINUE;
}

update_status ModuleDetourNavigation::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleDetourNavigation::CleanUp()
{
	return true;
}


void ModuleDetourNavigation::CreateDetourData() {
	const AIAgentComponent* agentComponent = mAIAgentComponents[0];

	NavMeshController* navController = App->GetScene()->GetNavController();
	rcPolyMesh* polyMesh = navController->getPolyMesh();
	rcPolyMeshDetail* polyMeshDetail = navController->getPolyMeshDetail();

	unsigned char* navData = 0;
	int navDataSize = 0;
	if (agentComponent) {
		mNavMeshParams->verts = polyMesh->verts;
		mNavMeshParams->vertCount = polyMesh->nverts;
		mNavMeshParams->polys = polyMesh->polys;
		mNavMeshParams->polyAreas = polyMesh->areas;
		mNavMeshParams->polyFlags = polyMesh->flags;
		mNavMeshParams->polyCount = polyMesh->npolys;
		mNavMeshParams->nvp = polyMesh->nvp;
		mNavMeshParams->detailMeshes = polyMeshDetail->meshes;
		mNavMeshParams->detailVerts = polyMeshDetail->verts;
		mNavMeshParams->detailVertsCount = polyMeshDetail->nverts;
		mNavMeshParams->detailTris = polyMeshDetail->tris;
		mNavMeshParams->detailTriCount = polyMeshDetail->ntris;
		mNavMeshParams->offMeshConVerts = nullptr;
		mNavMeshParams->offMeshConRad = nullptr;
		mNavMeshParams->offMeshConDir = nullptr;
		mNavMeshParams->offMeshConAreas = nullptr;
		mNavMeshParams->offMeshConFlags = nullptr;
		mNavMeshParams->offMeshConUserID = nullptr;
		mNavMeshParams->offMeshConCount = 0;
		mNavMeshParams->walkableHeight = agentComponent->GetHeight();
		mNavMeshParams->walkableRadius = agentComponent->GetRadius();
		mNavMeshParams->walkableClimb = agentComponent->GetMaxSlope();
		rcVcopy(mNavMeshParams->bmin, polyMesh->bmin);
		rcVcopy(mNavMeshParams->bmax, polyMesh->bmax);
		mNavMeshParams->cs = navController->GetCellSize();
		mNavMeshParams->ch = navController->GetCellHeight();
		mNavMeshParams->buildBvTree = true;
	}
	if (!dtCreateNavMeshData(mNavMeshParams, &navData, &navDataSize))
	{
		LOG("Could not build Detour navmesh.");
		return;
	}
	mDetourNavMesh = dtAllocNavMesh();
	if (!mDetourNavMesh)
	{
		dtFree(navData);
		LOG("Could not create Detour navmesh");
		return;
	}

	dtStatus status;
	status = mDetourNavMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navData);
		LOG("Could not init Detour navmesh");
		return;
	}

	status = mNavQuery->init(mDetourNavMesh, 2048);
	if (dtStatusFailed(status))
	{
		LOG("Could not init Detour navmesh query");
		return;
	}
}