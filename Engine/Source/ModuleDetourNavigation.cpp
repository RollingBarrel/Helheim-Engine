#include "ModuleDetourNavigation.h"
#include "Application.h"
#include "GameObject.h"
#include "NavMeshController.h"
#include "AIAgentComponent.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMesh.h"

ModuleDetourNavigation::ModuleDetourNavigation()
{
	mNavMeshParams = new dtNavMeshCreateParams();
}

ModuleDetourNavigation::~ModuleDetourNavigation()
{
	delete mNavMeshParams;
}

bool ModuleDetourNavigation::Init()
{

	return false;
}

update_status ModuleDetourNavigation::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleDetourNavigation::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleDetourNavigation::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}


void NavMeshController::CreateDetourData() {
	const AIAgentComponent* agentComponent = mAIAgentComponents[0];
	unsigned char* navData = 0;
	int navDataSize = 0;
	if (agentComponent) {
		mNavMeshParams->verts = mPolyMesh->verts;
		mNavMeshParams->vertCount = mPolyMesh->nverts;
		mNavMeshParams->polys = mPolyMesh->polys;
		mNavMeshParams->polyAreas = mPolyMesh->areas;
		mNavMeshParams->polyFlags = mPolyMesh->flags;
		mNavMeshParams->polyCount = mPolyMesh->npolys;
		mNavMeshParams->nvp = mPolyMesh->nvp;
		mNavMeshParams->detailMeshes = mPolyMeshDetail->meshes;
		mNavMeshParams->detailVerts = mPolyMeshDetail->verts;
		mNavMeshParams->detailVertsCount = mPolyMeshDetail->nverts;
		mNavMeshParams->detailTris = mPolyMeshDetail->tris;
		mNavMeshParams->detailTriCount = mPolyMeshDetail->ntris;
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
		rcVcopy(mNavMeshParams->bmin, mPolyMesh->bmin);
		rcVcopy(mNavMeshParams->bmax, mPolyMesh->bmax);
		mNavMeshParams->cs = mCellSize;
		mNavMeshParams->ch = mCellHeight;
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