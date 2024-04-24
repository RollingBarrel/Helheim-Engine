#include "ModuleDetourNavigation.h"
#include "Application.h"
#include "GameObject.h"
#include "NavMeshController.h"
//#include "AIAgentComponent.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMesh.h"
#include "ModuleScene.h"
#include "DetourNavMeshQuery.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
//TODO: Remove recast from the coreLib
#include "Recast.h"
#include "Application.h"
#include "ModuleDebugDraw.h"
#include "ModuleOpenGL.h"
#define MAX_AMOUNT 999
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
		dtPolyRef result;
		dtQueryFilter temp;
		mNavQuery->findNearestPoly(&mQueryCenter[0], &mQueryHalfSize[0], &temp, &result, &mQueryResult[0]);
		//TODO: SEPARATE ENGINE
		//App->GetOpenGL()->BindSceneFramebuffer();
		//DrawDebug();
		//App->GetOpenGL()->UnbindSceneFramebuffer();
		

	}
	
	return UPDATE_CONTINUE;
}

std::vector<float3> ModuleDetourNavigation::FindNavPath(float3 startPos, float3 endPos) 
{
	if (!mNavQuery) {
		
		LOG("BUILD NAVMESH");
		std::vector<float3> breakVector(0);
		return breakVector;
	}
	float3 queryAreafSize = float3(5.0f);

	dtPolyRef startPolygon;
	dtQueryFilter startTemp;
	float3 queryEndPos = float3(0.0f);;
	mNavQuery->findNearestPoly(&startPos[0], &queryAreafSize[0], &startTemp, &startPolygon, &queryEndPos[0]);

	dtPolyRef endPolygon;
	dtQueryFilter endTemp;
	mNavQuery->findNearestPoly(&endPos[0], &queryAreafSize[0], &endTemp, &endPolygon, &queryEndPos[0]);

	dtQueryFilter pathFilter;
	dtPolyRef* polygonPath= new dtPolyRef[MAX_AMOUNT];
	int pathPoylgonCount=0;

	mNavQuery->findPath(startPolygon, endPolygon, &startPos[0], &endPos[0], &pathFilter, polygonPath, &pathPoylgonCount, MAX_AMOUNT);

	float*  positionPath = new float[MAX_AMOUNT];
	int numberOfPositions = 0;
	unsigned char* straightPathFlags = new unsigned char[MAX_AMOUNT] ;
	/*dtPolyRef* currentPoly = new dtPolyRef(MAX_AMOUNT);*/

	mNavQuery->findStraightPath(&startPos[0], &endPos[0], polygonPath, pathPoylgonCount, positionPath, straightPathFlags, nullptr, &numberOfPositions,MAX_AMOUNT,0);

	std::vector<float3> positionsPathResult;
	positionsPathResult.reserve(numberOfPositions);

	for (size_t i = 0; i < numberOfPositions; ++i)
	{
		positionsPathResult.push_back({ positionPath[i * 3],positionPath[i * 3 + 1],positionPath[i * 3 + 2] });
	}
	delete[] positionPath;
	delete[] polygonPath;
	delete[] straightPathFlags;
	return positionsPathResult;

}

update_status ModuleDetourNavigation::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleDetourNavigation::CleanUp()
{
	return true;
}


void ModuleDetourNavigation::CreateDetourData() 
{
	//const AIAgentComponent* agentComponent = mAIAgentComponents[0];

	NavMeshController* navController = App->GetScene()->GetNavController();
	rcPolyMesh* polyMesh = navController->getPolyMesh();
	rcPolyMeshDetail* polyMeshDetail = navController->getPolyMeshDetail();

	unsigned char* navData = 0;
	int navDataSize = 0;
	/*if (agentComponent) {*/
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
		mNavMeshParams->walkableHeight =  1.0f;
		mNavMeshParams->walkableRadius =0.5f;
		mNavMeshParams->walkableClimb = 0.0f;
		rcVcopy(mNavMeshParams->bmin, polyMesh->bmin);
		rcVcopy(mNavMeshParams->bmax, polyMesh->bmax);
		mNavMeshParams->cs = navController->GetCellSize();
		mNavMeshParams->ch = navController->GetCellHeight();
		mNavMeshParams->buildBvTree = true;
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

float3 ModuleDetourNavigation::FindNearestPoint(float3 center, float3 halfSize) 
{
	float3 queryResult = float3(0.0f);
	if (!mNavQuery->getNodePool()) {
		LOG("BUILD NAVMESH");
		return queryResult;
	}
	dtPolyRef result;
	dtQueryFilter temp;

	mNavQuery->findNearestPoly(&center[0], &halfSize[0], &temp, &result, &queryResult[0]);
	return queryResult;
}


//TODO: SEPARATE ENGINE
//void ModuleDetourNavigation::DrawDebug() 
//{
//	float3 color = float3(1.0f, 0.0f, 0.0f);
//	App->GetDebugDraw()->DrawSphere(&mQueryResult[0], &color[0], 1.0f);
//
//	float3 color2 = float3(1.0f, 1.0f, 0.0f);
//	App->GetDebugDraw()->DrawSphere(&mQueryCenter[0], &color2[0], 1.0f);
//
//	float3 color3 = float3(0.0f, 0.0f, 1.0f);
//	float3 minAABB = mQueryCenter - mQueryHalfSize;
//	float3 maxAABB = mQueryCenter + mQueryHalfSize;
//	OBB cube = OBB(AABB(minAABB, maxAABB));
//	App->GetDebugDraw()->DrawCube(cube, color3);
//}