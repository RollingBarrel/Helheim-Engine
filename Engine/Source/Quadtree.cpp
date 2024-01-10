#include "Quadtree.h"
#include "GameObject.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "MeshRendererComponent.h"
#include "Math/float3.h"

#include "Application.h"
#include "ModuleDebugDraw.h"


Quadtree::Quadtree(const AABB& boundingBox) : Quadtree(boundingBox, 0)
{
}

Quadtree::Quadtree(const AABB& boundingBox, int depth)
{
	mBoundingBox = boundingBox;
	mDepthLevel = depth;
	mFilled = false;
	mChildren[0] = nullptr;
	mChildren[1] = nullptr;
	mChildren[2] = nullptr;
	mChildren[3] = nullptr;
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
	{
		return false;
	}

	if (mGameObjects.size() >= CAPACITY || mFilled) 
	{
		if (mDepthLevel >= MAX_DEPTH) 
		{
			mGameObjects.push_back(object);
			return true;
		}
		if(!mFilled)
		{
			SplitNode();
		}
		mChildren[0]->AddObject(object);
		mChildren[1]->AddObject(object);
		mChildren[2]->AddObject(object);
		mChildren[3]->AddObject(object);
		mFilled = true;
		return true;
	}

	mGameObjects.push_back(object);
	return true;
}

void Quadtree::RemoveObject(const GameObject* object)
{
	if (mFilled) 
	{
		mChildren[0]->RemoveObject(object);
		mChildren[1]->RemoveObject(object);
		mChildren[2]->RemoveObject(object);
		mChildren[3]->RemoveObject(object);
	}
	else 
	{
		mGameObjects.erase(std::remove_if(mGameObjects.begin(), mGameObjects.end(),
			[object](GameObject* ptr) { return ptr == object; }),
			mGameObjects.end());
	}
}

bool Quadtree::Intersects(const OBB* bounding_box) const
{
	return bounding_box->Intersects(mBoundingBox);
}

void Quadtree::CleanUp()
{
	if (mFilled) 
	{
		mChildren[0]->CleanUp();
		mChildren[1]->CleanUp();
		mChildren[2]->CleanUp();
		mChildren[3]->CleanUp();

		delete mChildren[0];
		delete mChildren[1];
		delete mChildren[2];
		delete mChildren[3];

		mFilled = false;
	}
	else 
	{
		mGameObjects.clear();
		return;
	}
}

void Quadtree::Draw() const
{
	App->GetDebugDraw()->DrawBoundingBox(OBB(mBoundingBox));

	if (mFilled)
	{
		mChildren[0]->Draw();
		mChildren[1]->Draw();
		mChildren[2]->Draw();
		mChildren[3]->Draw();
	}
}

void Quadtree::SplitNode()
{
	float3 minPoint = mBoundingBox.minPoint;
	float3 maxPoint = mBoundingBox.maxPoint;
	float3 center = (minPoint + maxPoint) * 0.5f;

	// Calculate points on each side
	float3 bf_x = float3(center.x, minPoint.y, minPoint.z);
	float3 bf_z = float3(minPoint.x, minPoint.y, center.z);
	float3 bf_center = float3(center.x, minPoint.y, center.z);
	float3 uf_center = float3(bf_center.x, maxPoint.y, bf_center.z);
	float3 uf_z = float3(maxPoint.x, maxPoint.y, center.z);
	float3 uf_x = float3(center.x, maxPoint.y, maxPoint.z);

	mChildren[0] = new Quadtree(AABB(minPoint, uf_center), mDepthLevel + 1);
	mChildren[1] = new Quadtree(AABB(bf_x, uf_z), mDepthLevel + 1);
	mChildren[2] = new Quadtree(AABB(bf_z, uf_x), mDepthLevel + 1);
	mChildren[3] = new Quadtree(AABB(bf_center, maxPoint), mDepthLevel + 1);

	for (const auto& object : mGameObjects)
	{
		mChildren[0]->AddObject(object);
		mChildren[1]->AddObject(object);
		mChildren[2]->AddObject(object);
		mChildren[3]->AddObject(object);
	}

	mGameObjects.clear();
}
