#include "NavMeshController.h"
#include "GameObject.h"
#include "ResourceMesh.h"
#include "MeshRendererComponent.h"
#include "ModuleScene.h"
#include "ModuleOpenGL.h"
#include "Application.h"
#include "glew.h"
#include "float4x4.h"
#include "ImporterMesh.h"
#include "ModuleDebugDraw.h"

NavMeshController::NavMeshController()
{
	mRecastContext = rcContext(false);

	//HandleBuild(); No se llama al inicar ya que no hay escena a�n, llamar solo con boton imgui


}

NavMeshController::~NavMeshController()
{
	mVertices.clear();
	mIndices.clear();
	mMeshesToNavMesh.clear();
	mMeshRendererComponents.clear();

	delete mPolyMeshDetail;
	delete mPolyMesh;
	delete mContourSet;
	delete mCompactHeightField;
	delete mHeightField;
	delete mTriangleAreas;

}

void NavMeshController::Reset() {
	//Change variables to default values.
}

void NavMeshController::TranslateIndices()
{
	for (int i = 0; i < mPolyMeshDetail->nmeshes; ++i)
	{
		const unsigned int* m = &mPolyMeshDetail->meshes[i * 4];
		const unsigned int bverts = m[0];
		const unsigned int btris = m[2];
		const int ntris = (int)m[3];
		const float* verts = &mPolyMeshDetail->verts[bverts * 3];
		const unsigned char* tris = &mPolyMeshDetail->tris[btris * 4];

		for (int j = 0; j < ntris; ++j)
		{
			float3 a = float3(verts[tris[j * 4 + 0] * 3], verts[tris[j * 4 + 0] * 3 + 1], verts[tris[j * 4 + 0] * 3 + 2]);
			float3 b = float3(verts[tris[j * 4 + 1] * 3], verts[tris[j * 4 + 1] * 3 + 1], verts[tris[j * 4 + 1] * 3 + 2]);
			float3 c = float3(verts[tris[j * 4 + 2] * 3], verts[tris[j * 4 + 2] * 3 + 1], verts[tris[j * 4 + 2] * 3 + 2]);

			// Check and update indices
			int indexA = FindVertexIndex(a);
			if (indexA == -1) {
				mVertices.push_back(a);
				indexA = mVertices.size() - 1;
			}

			int indexB = FindVertexIndex(b);
			if (indexB == -1) {
				mVertices.push_back(b);
				indexB = mVertices.size() - 1;
			}

			int indexC = FindVertexIndex(c);
			if (indexC == -1) {
				mVertices.push_back(c);
				indexC = mVertices.size() - 1;
			}

			// Add indices to mIndices
			mIndices.push_back(indexA);
			mIndices.push_back(indexB);
			mIndices.push_back(indexC);

		}
	}
}

void NavMeshController::DebugDrawPolyMesh()
{

	if (mPolyMeshDetail == nullptr)
		return;
	if (mPolyMeshDetail->nmeshes < 1)
		return;
	
	
	unsigned int program = App->GetOpenGL()->GetPBRProgramId();
	glBindVertexArray(0);
	glUseProgram(0);
	glUseProgram(program);
	float4x4 identity = float4x4::identity;
	glUniformMatrix4fv(0, 1, GL_TRUE, identity.ptr());
	glBindVertexArray(mVao);
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);


	/*	Old draw polymesh with debug draw

	for (int i = 0; i < mPolyMeshDetail->nmeshes; ++i)
	{
		const unsigned int* m = &mPolyMeshDetail->meshes[i * 4];
		const unsigned int bverts = m[0];
		const unsigned int btris = m[2];
		const int ntris = (int)m[3];
		const float* verts = &mPolyMeshDetail->verts[bverts * 3];
		const unsigned char* tris = &mPolyMeshDetail->tris[btris * 4];


		for (int j = 0; j < ntris; ++j)
		{
			float3 a = float3(verts[tris[j * 4 + 0] * 3], verts[tris[j * 4 + 0] * 3 + 1], verts[tris[j * 4 + 0] * 3 + 2]);
			float3 b = float3(verts[tris[j * 4 + 1] * 3], verts[tris[j * 4 + 1] * 3 + 1], verts[tris[j * 4 + 1] * 3 + 2]);
			float3 c = float3(verts[tris[j * 4 + 2] * 3], verts[tris[j * 4 + 2] * 3 + 1], verts[tris[j * 4 + 2] * 3 + 2]);
			App->GetDebugDraw()->DrawTriangle(a, b, c);

		}
	}
	*/

}

void NavMeshController::Update()
{
	if (mPolyMesh == nullptr)
		return;
	App->GetOpenGL()->BindSceneFramebuffer();

	DebugDrawPolyMesh();
	App->GetOpenGL()->UnbindSceneFramebuffer();

}

void NavMeshController::HandleBuild() {
	mIndices.clear();
	mVertices.clear();
	//mMeshesToNavMesh.clear();
	mMeshRendererComponents.clear();

	GameObject* root = App->GetScene()->GetRoot();
	GetGOMeshes(root);
	if (mMeshRendererComponents.empty())
		return;
	std::vector<rcPolyMeshDetail*> myPolyMeshDetails;
	std::vector<rcPolyMesh*> myPolyMeshes;


	const MeshRendererComponent* testMesh;
	

	for (int index = 0; index < mMeshRendererComponents.size(); index++) {
		
		testMesh = mMeshRendererComponents[index];
		if (!testMesh)
		{
			LOG("A mesh was not correctly loaded to the navigation controller.");
			break;
		}

		float3 meshMax = testMesh->GetAABB().maxPoint;
		float3 meshMin = testMesh->GetAABB().minPoint;
		const float maxPoint[3] = { meshMax.x, meshMax.y, meshMax.z };
		const float minPoint[3] = { meshMin.x, meshMin.y, meshMin.z };
		int gridWidth = 0;
		int gridHeight = 0;

		rcCalcGridSize(minPoint, maxPoint, mCellSize, &gridWidth, &gridHeight);
		mHeightField = rcAllocHeightfield();
		if (!mHeightField)
		{
			LOG("buildNavigation: Out of memory 'mHeightField'.");
			return;
		}
		if (!rcCreateHeightfield(&mRecastContext, *mHeightField, gridWidth, gridHeight, minPoint, maxPoint, mCellSize, mCellHeight))
		{
			LOG("buildNavigation: Could not create solid rcCreateHeightfield.");
			return;
		}

		// Allocate array that can hold triangle area types.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		unsigned int numberOfTriangles = testMesh->GetResourceMesh()->GetNumberIndices() / 3;
		mTriangleAreas = new unsigned char[numberOfTriangles];
		if (!mTriangleAreas)
		{
			LOG("buildNavigation: Out of memory 'mTriangleAreas'");
			return;
		}
		float* vertices = (float*)(testMesh->GetResourceMesh()->GetAttributeData(Attribute::POS)); // TODO: Translate using testMesh->WorldTransform
		float4x4 objectTransform = testMesh->GetOwner()->GetWorldTransform();
		std::vector<float> transformedVerts;

		int numberOfVertices = testMesh->GetResourceMesh()->GetNumberVertices();

		for (int i = 0; i < numberOfVertices * 3; i += 3) 
		{
			float4 operationTemp = objectTransform * float4(vertices[i], vertices[i + 1], vertices[i + 2], 1.0f) ;
			operationTemp = operationTemp / operationTemp.w;
			transformedVerts.push_back(operationTemp.x);
			transformedVerts.push_back(operationTemp.y);
			transformedVerts.push_back(operationTemp.z);


		}
		


		const int* triangle = (const int*)(testMesh->GetResourceMesh()->mIndices);



		memset(mTriangleAreas, 0, numberOfTriangles * sizeof(unsigned char));
		rcMarkWalkableTriangles(&mRecastContext, mMaxSlopeAngle, &transformedVerts[0], numberOfVertices, triangle, numberOfTriangles, mTriangleAreas);

		if (!rcRasterizeTriangles(&mRecastContext, &transformedVerts[0], numberOfVertices, triangle, mTriangleAreas, numberOfTriangles, *mHeightField, 1))
		{
			LOG("buildNavigation: Could not rasterize triangles.");
			return;
		}

		if (!mKeepInterResults)
		{
			delete[] mTriangleAreas;
			mTriangleAreas = 0;
		}

		//
		// Step 3. Filter walkable surfaces.
		//

		// Once all geometry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.
		if (mFilterLowHangingObstacles)
			rcFilterLowHangingWalkableObstacles(&mRecastContext, mWalkableClimb, *mHeightField);
		if (mFilterLedgeSpans)
			rcFilterLedgeSpans(&mRecastContext, mWalkableHeight, mWalkableClimb, *mHeightField);
		if (mFilterWalkableLowHeightSpans)
			rcFilterWalkableLowHeightSpans(&mRecastContext, mWalkableHeight, *mHeightField);

		//
		// Step 4. Partition walkable surface to simple regions.
		//

		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		mCompactHeightField = rcAllocCompactHeightfield();
		if (!mCompactHeightField)
		{
			LOG("buildNavigation: Out of memory 'mCompactHeightField'.");
			return;
		}
		if (!rcBuildCompactHeightfield(&mRecastContext, mWalkableHeight, mWalkableClimb, *mHeightField, *mCompactHeightField))
		{
			LOG("buildNavigation: Could not build compact data.");
			return;
		}

		if (!mKeepInterResults)
		{
			rcFreeHeightField(mHeightField);
			mHeightField = 0;
		}
		// Erode the walkable area by agent radius.
		if (!rcErodeWalkableArea(&mRecastContext, mWalkableRadius, *mCompactHeightField))
		{
			LOG("buildNavigation: Could not erode.");
		}

		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(&mRecastContext, *mCompactHeightField))
		{
			LOG("buildNavigation: Could not build distance field.");
			return;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(&mRecastContext, *mCompactHeightField, 0, mMinRegionArea, mMergeRegionArea))
		{
			LOG("buildNavigation: Could not build watershed regions.");
			return;
		}

		//
		// Step 5. Trace and simplify region contours.
		//

		// Create contours.
		mContourSet = rcAllocContourSet();
		if (!mContourSet)
		{
			LOG("buildNavigation: Out of memory 'mContourSet'.");
			return;
		}
		if (!rcBuildContours(&mRecastContext, *mCompactHeightField, mMaxSimplificationError, mMaxEdgeLen, *mContourSet))
		{
			LOG("buildNavigation: Could not create contours.");
			return;
		}


		//
		// Step 6. Build polygons mesh from contours.
		//

		// Build polygon navmesh from the contours.
		rcPolyMesh* tempPolyMesh = rcAllocPolyMesh();
		if (!tempPolyMesh)
		{
			LOG("buildNavigation: Out of memory 'tempPolyMesh'.");
			return;
		}
		if (!rcBuildPolyMesh(&mRecastContext, *mContourSet, mMaxVertsPerPoly, *tempPolyMesh))
		{
			LOG("buildNavigation: Could not triangulate contours.");
			return;
		}
		//
		// Step 7. Create detail mesh which allows to access approximate height on each polygon.
		//

		rcPolyMeshDetail* tempPolyMeshDetail = rcAllocPolyMeshDetail();
		if (!tempPolyMeshDetail)
		{
			LOG("buildNavigation: Out of memory 'tempPolyMeshDetail'.");
			return;
		}

		if (!rcBuildPolyMeshDetail(&mRecastContext, *tempPolyMesh, *mCompactHeightField, mDetailSampleDist, mDetailSampleMaxError, *tempPolyMeshDetail))
		{
			LOG("buildNavigation: Could not build detail mesh.");
			return;
		}

		myPolyMeshDetails.push_back(tempPolyMeshDetail);
		myPolyMeshes.push_back(tempPolyMesh);

		if (!mKeepInterResults)
		{
			rcFreeCompactHeightfield(mCompactHeightField);
			mCompactHeightField = 0;
			rcFreeContourSet(mContourSet);
			mContourSet = 0;
		}

	}
	mPolyMesh = rcAllocPolyMesh();
	if (!mPolyMesh)
	{
		LOG("buildNavigation: Out of memory 'mPolyMesh'.");
		return;
	}
	mPolyMeshDetail = rcAllocPolyMeshDetail();
	if (!mPolyMeshDetail)
	{
 		LOG("buildNavigation: Out of memory 'mPolyMeshDetail'.");
		return;
	}

	if (!rcMergePolyMeshes(&mRecastContext, &myPolyMeshes[0], myPolyMeshes.size(), *mPolyMesh))
	{
		LOG("mergePolymeshes: Failed to merge polymeshes.");
		return;

	}

	if (!rcMergePolyMeshDetails(&mRecastContext, &myPolyMeshDetails[0], myPolyMeshDetails.size(), *mPolyMeshDetail))
	{
		LOG("mergePolymeshdetails: Failed to merge polymeshdetails.");
		return;

	}
	

	LoadDrawMesh();

}


void NavMeshController::GetGOMeshes(const GameObject* gameObj) {
	if (!(gameObj->GetChildren().empty())) {
		for (const auto& child : gameObj->GetChildren()) {
			MeshRendererComponent* meshRendererComponent = child->GetMeshRenderer();
			if (meshRendererComponent) {
				//mMeshesToNavMesh.push_back(meshRendererComponent->GetResourceMesh());
				mMeshRendererComponents.push_back(meshRendererComponent);

			}
			GetGOMeshes(child);
		}
	}

}



void NavMeshController::LoadDrawMesh()
{
	if (mPolyMeshDetail != nullptr)
	{
		if (mPolyMeshDetail->nmeshes < 1)
			return; // Maybe remove the vao from memory until new call? Warn user?
		TranslateIndices();
		// Now you can create the VAO and fill it with the mesh data
		glGenVertexArrays(1, &mVao);
		glBindVertexArray(mVao);


		glGenBuffers(1, &mVbo);
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);


		glGenBuffers(1, &mEbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);


		glBindVertexArray(0);

	}
	return;
}

int NavMeshController::FindVertexIndex(float3 vert)
{

	for (int i = 0; i < mVertices.size(); ++i) {
		if (mVertices[i].x == vert.x && mVertices[i].y == vert.y && mVertices[i].z == vert.z) {
			return i;
		}
	}
	return -1; // Not found

}
