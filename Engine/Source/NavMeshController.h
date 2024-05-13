#pragma once
#include "vector"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"

class MeshRendererComponent;
class AIAgentComponent;
class GameObject;
struct dtNavMeshCreateParams;
struct rcHeightfield;
struct rcCompactHeightfield;
struct rcContourSet;
struct rcPolyMesh;
struct rcPolyMeshDetail;
class rcContext;
class dtNavMesh;
struct ObstacleTriangle {
	int startIndicePos=0;
	int numberOfIndices=0;
};
class ENGINE_API NavMeshController
{
public:
	NavMeshController();
	void Reset();
	~NavMeshController();
	void HandleBuild();
	void CreateDetourData();


	rcPolyMesh* getPolyMesh()const { return mPolyMesh; }
	void setPolyMesh(rcPolyMesh* polyMesh) { mPolyMesh = polyMesh; }

	rcPolyMeshDetail* getPolyMeshDetail()const { return mPolyMeshDetail; }
	void setPolyMeshDetail(rcPolyMeshDetail* polyMeshDetail) { mPolyMeshDetail = polyMeshDetail; }

	//IMGUI VALUES
	float GetCellSize() const { return mCellSize; }
	void SetCellSize(float value) { mCellSize = value; }

	float GetCellHeight() const { return mCellHeight; }
	void SetCellHeight(float value) { mCellHeight = value; }

	unsigned int GetMaxSlopeAngle() const { return mMaxSlopeAngle; }
	void SetMaxSlopeAngle(unsigned int value) { mMaxSlopeAngle = value; }

	float GetWalkableRadius() const { return mWalkableRadius; }
	void SetWalkableRadius(float value) { mWalkableRadius = value; }

	int GetMinRegionArea() const { return mMinRegionArea; }
	void SetMinRegionArea(int value) { mMinRegionArea = value; }

	int GetMergeRegionArea() const { return mMergeRegionArea; }
	void SetMergeRegionArea(int value) { mMergeRegionArea = value; }

	float GetMaxSimplificationError() const { return mMaxSimplificationError; }
	void SetMaxSimplificationError(float value) { mMaxSimplificationError = value; }

	int GetMaxEdgeLen() const { return mMaxEdgeLen; }
	void SetMaxEdgeLen(int value) { mMaxEdgeLen = value; }

	int GetMaxVertsPerPoly() const { return mMaxVertsPerPoly; }
	void SetMaxVertsPerPoly(int value) { mMaxVertsPerPoly = value; }

	float GetDetailSampleDist() const { return mDetailSampleDist; }
	void SetDetailSampleDist(float value) { mDetailSampleDist = value; }

	float GetDetailSampleMaxError() const { return mDetailSampleMaxError; }
	void SetDetailSampleMaxError(float value) { mDetailSampleMaxError = value; }

	dtNavMesh* GetDetourNavMesh()const { return mDetourNavMesh; }

	std::vector<float3>& GetVertices() { return mVertices; }
	void SetIndices(std::vector<int> indices) { mIndices = indices; };
	std::vector<int>& GetIndices() { return mIndices; }
	void SetVertices(std::vector<float3> vertices) { mVertices = vertices; };

private:

	void GetGOMeshes(const GameObject* gameObj);
	std::vector<GameObject*> mGameObjects;
	std::vector<ObstacleTriangle> mObstaclesTriangles;
	void TranslateIndices();
	int FindVertexIndex(float3 vert);

	rcPolyMesh* mPolyMesh = nullptr;
	rcPolyMeshDetail* mPolyMeshDetail = nullptr;

	int mWalkableClimb = 1;   // no imgui
	int mWalkableHeight = 0; // no imgui

	OBB mOBB;
	AABB mAABB;
	AABB mAABBWorld;

	std::vector<float3> mVertices;
	std::vector<int> mIndices;
	//IMGUI VALUES
	float mCellSize = 0.30f;  // 0.1 - 1.0
	float mCellHeight = 0.20f; // 0.1 - 1.0

	unsigned int mMaxSlopeAngle = 20; // 0 - 90
	float mWalkableRadius = 0.6f; // 0.0 - 5.0
	int mMinRegionArea = 8; // 0 - 150
	int mMergeRegionArea = 20; // 0 - 150
	float mMaxSimplificationError = 1.3f; // 0.1 - 3.0
	int mMaxEdgeLen = 12;  // 0 - 50
	int mMaxVertsPerPoly = 6; // 3 - 12
	float mDetailSampleDist = 6; // 0 - 16
	float mDetailSampleMaxError = 1; // 0 - 16

	dtNavMesh* mDetourNavMesh = nullptr;
};


