#include "Quadtree.h"
#include "GameObject.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "MeshRendererComponent.h"
#include "Math/float3.h"

Quadtree::Quadtree(int depth, AABB bounding_box)
{
	has_been_filled = false;
	depth_level = depth;
	mBoundingBox = bounding_box;
}

Quadtree::~Quadtree()
{
	CleanUp();
	mGameObjects.clear();

}

bool Quadtree::AddObject(GameObject* object)
{
	OBB object_BB = object->getMeshRenderer()->getOBB();
	if (!mBoundingBox.Intersects(object_BB))
		return false;


	if (mGameObjects.size() >= CAPACITY || has_been_filled) 
	{

		if (depth_level >= MAX_DEPTH) 
		{
			mGameObjects.push_back(object);
			return true;
		}
		if(!has_been_filled)
			SplitNode();
		children[0]->AddObject(object);
		children[1]->AddObject(object);
		children[2]->AddObject(object);
		children[3]->AddObject(object);
		has_been_filled = true;
		return true;

	}

	mGameObjects.push_back(object);
	return true;

}

void Quadtree::RemoveObject(GameObject* object)
{
	if (has_been_filled) 
	{
		children[0]->RemoveObject(object);
		children[1]->RemoveObject(object);
		children[2]->RemoveObject(object);
		children[3]->RemoveObject(object);

	}
	else 
	{
		mGameObjects.erase(std::remove_if(mGameObjects.begin(), mGameObjects.end(),
			[object](GameObject* ptr) { return ptr == object; }),
			mGameObjects.end());

		
	}

}

bool Quadtree::Intersects(OBB* bounding_box)
{
	return bounding_box->Intersects(mBoundingBox);
}

void Quadtree::CleanUp()
{
	if (has_been_filled) 
	{
		children[0]->CleanUp();
		children[1]->CleanUp();
		children[2]->CleanUp();
		children[3]->CleanUp();

		delete children[0];
		delete children[1];
		delete children[2];
		delete children[3];

		has_been_filled = false;
	}
	else 
	{
		mGameObjects.clear();
		return;
	}
}

void Quadtree::SplitNode()
{
	float3 minPoint = mBoundingBox.minPoint;
	float3 maxPoint = mBoundingBox.maxPoint;

	float3 center = (minPoint + maxPoint) * 0.5f;

	// Calculate points on each side
	float3 bf_x = float3((minPoint.x + maxPoint.x) / 2, minPoint.y, minPoint.z);
	float3 bf_z = float3(minPoint.x, minPoint.y, (minPoint.z + maxPoint.z) / 2);
	float3 bf_center = float3((minPoint.x + maxPoint.x) / 2, minPoint.y, (minPoint.z + maxPoint.z) / 2);
	float3 uf_center = float3(bf_center.x, maxPoint.y, bf_center.z);
	float3 uf_z = float3(maxPoint.x, maxPoint.y, (minPoint.z + maxPoint.z) / 2);
	float3 uf_x = float3((minPoint.x + maxPoint.x) / 2, maxPoint.y, maxPoint.z);



	children[0] = new Quadtree(depth_level + 1, AABB(minPoint, uf_center));
	children[1] = new Quadtree(depth_level + 1, AABB(bf_x, uf_z));
	children[2] = new Quadtree(depth_level + 1, AABB(bf_z, uf_x));
	children[3] = new Quadtree(depth_level + 1, AABB(bf_center, maxPoint));

	for (const auto& object : mGameObjects) 
	{
		children[0]->AddObject(object);
		children[1]->AddObject(object);
		children[2]->AddObject(object);
		children[3]->AddObject(object);

	}

	mGameObjects.clear();

}
