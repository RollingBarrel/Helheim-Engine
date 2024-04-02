#pragma once
#include "vector"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
class Material;
struct ResourceMesh;
class MeshRendererComponent;
class AIAgentComponent;
class GameObject;
struct dtNavMeshCreateParams;
class rcHeightfield;
class rcCompactHeightfield;
class rcContourSet;
class rcPolyMesh;
class rcPolyMeshDetail;
class rcContext;

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
	void Update();


	rcPolyMesh* getPolyMesh()const { return mPolyMesh; }
	rcPolyMeshDetail* getPolyMeshDetail()const { return mPolyMeshDetail; }

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

	float3 FindNearestPoint(float3 center, float3 halfsize) const;

	float3 GetQueryCenter() const { return mQueryCenter; }
	float3 GetQueryHalfSize() const { return mQueryHalfSize; }

	void SetQueryCenter(float3 center) { mQueryCenter = center; }
	void SetQueryHalfSize(float3 halfsize) { mQueryHalfSize = halfsize; }

	bool GetShouldDraw() const { return mDraw; }
	void SetShouldDraw(bool draw) { mDraw = draw; }



private:
	void GetGOMeshes(const GameObject* gameObj);
	std::vector<GameObject*>mGameObjects;
	std::vector<const AIAgentComponent*>mAIAgentComponents;
	std::vector<ObstacleTriangle> mObstaclesTriangles;
	void TranslateIndices();
	void DebugDrawPolyMesh();
	void LoadDrawMesh();
	int FindVertexIndex(float3 vert);

	dtNavMeshCreateParams* mNavMeshParams;
	rcHeightfield* mHeightField = nullptr;
	rcCompactHeightfield* mCompactHeightField = nullptr;
	rcContourSet* mContourSet = nullptr;
	rcPolyMesh* mPolyMesh = nullptr;
	rcPolyMeshDetail* mPolyMeshDetail = nullptr;
	unsigned char* mTriangleAreas = nullptr;
	rcContext* mRecastContext;
	bool mKeepInterResults = false;
	bool mFilterLowHangingObstacles=true;
	bool mFilterLedgeSpans=true;
	bool mFilterWalkableLowHeightSpans=true;



	int mWalkableClimb = 1;   // no imgui
	int mWalkableHeight = 0; // no imgui


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
	OBB mOBB;
	AABB mAABB;
	AABB mAABBWorld;
	//DEBUG DRAW VARIABLES
	bool mDraw = true;
	unsigned int mVao = 0;
	unsigned int mVbo = 0;
	unsigned int mEbo = 0;
	std::vector<float3> mVertices;
	std::vector<int> mIndices;


	float3 mQueryCenter = float3(10.0f, 0.0f, -3.0f);
	float3 mQueryHalfSize = float3(5.0f);
	float3 mQueryNearestPoint = float3(0.0f);
};

