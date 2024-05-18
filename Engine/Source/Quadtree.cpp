#include "Quadtree.h"
#include "GameObject.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "MeshRendererComponent.h"
#include "Math/float3.h"
#include "Geometry/Frustum.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ResourceMesh.h"
#include "Geometry/Triangle.h"
#include "Physics.h"
#include <utility>


Quadtree::Quadtree(const AABB& boundingBox) : Quadtree(boundingBox, 0, "R")
{
	mChildren[0] = nullptr;
	mChildren[1] = nullptr;
	mChildren[2] = nullptr;
	mChildren[3] = nullptr;
}

Quadtree::Quadtree(const AABB& boundingBox, int depth, std::string&& name) : mBoundingBox(boundingBox), mDepthLevel(depth), mName(std::move(name))
{
	mChildren[0] = nullptr;
	mChildren[1] = nullptr;
	mChildren[2] = nullptr;
	mChildren[3] = nullptr;
}

Quadtree::~Quadtree()
{
	CleanUp();
}

bool Quadtree::AddObject(const GameObject& object)
{
	MeshRendererComponent* meshRenderer = reinterpret_cast<MeshRendererComponent*>(object.GetComponent(ComponentType::MESHRENDERER));
	if (meshRenderer == nullptr)
		return false;
	const AABB& objectAABB = meshRenderer->GetAABB();


	if (!mBoundingBox.Intersects(objectAABB))
	{
		return false;
	}

	if (mGameObjects.size() >= CAPACITY || !IsLeaf())
	{
		if (mDepthLevel >= MAX_DEPTH)
		{
			mGameObjects.push_back(&object);
			return true;
		}
		if (IsLeaf())
		{
			SplitNode();
		}
		mChildren[0]->AddObject(object);
		mChildren[1]->AddObject(object);
		mChildren[2]->AddObject(object);
		mChildren[3]->AddObject(object);
		return true;
	}

	mGameObjects.push_back(&object);
	return true;
}

void Quadtree::RemoveObject(const GameObject& object)
{
	if (!IsLeaf())
	{
		mChildren[0]->RemoveObject(object);
		mChildren[1]->RemoveObject(object);
		mChildren[2]->RemoveObject(object);
		mChildren[3]->RemoveObject(object);
	}
	else
	{
		std::remove_if(mGameObjects.begin(), mGameObjects.end(),
			[&](const GameObject* ptr) { return ptr->GetID() == object.GetID(); }), mGameObjects.end();
	}
}

bool Quadtree::Intersects(const OBB& boundingBox) const
{
	return boundingBox.Intersects(mBoundingBox);
}

bool Quadtree::Intersects(const Ray& ray) const
{
	return ray.Intersects(mBoundingBox);
}

void Quadtree::CleanUp()
{
	if (!IsLeaf()) 
	{
		mChildren[0]->CleanUp();
		mChildren[1]->CleanUp();
		mChildren[2]->CleanUp();
		mChildren[3]->CleanUp();

		delete mChildren[0];
		mChildren[0] = nullptr;
		delete mChildren[1];
		mChildren[1] = nullptr;
		delete mChildren[2];
		mChildren[2] = nullptr;
		delete mChildren[3];
		mChildren[3] = nullptr;
	}
	else
	{
		mGameObjects.clear();
	}
}

void Quadtree::GetRenderComponentsInFrustum(const Frustum& cam, std::vector<const MeshRendererComponent*>& components) const
{
	if (!cam.Intersects(mBoundingBox))
	{
		return;
	}

	if (!IsLeaf())
	{
		mChildren[0]->GetRenderComponentsInFrustum(cam, components);
		mChildren[1]->GetRenderComponentsInFrustum(cam, components);
		mChildren[2]->GetRenderComponentsInFrustum(cam, components);
		mChildren[3]->GetRenderComponentsInFrustum(cam, components);
	}
	else
	{
		for (const GameObject* object : mGameObjects)
		{
			if (object->IsActive())
			{
				const MeshRendererComponent* comp = reinterpret_cast<const MeshRendererComponent*>(object->GetComponent(ComponentType::MESHRENDERER));
				if (comp != nullptr && comp->IsEnabled())
				{
					if (cam.Intersects(comp->GetOBB()))
					{
						bool alreadyFound = false;
						for (const MeshRendererComponent* component : components)
						{
							if (comp->GetID() == component->GetID())
							{
								alreadyFound = true;
								break;
							}
						}
						if (!alreadyFound)
						{
							components.push_back(comp);
						}
					}
				}
			}
		}
	}
}

void Quadtree::AddHierarchyObjects(const GameObject& node)
{
	for (const GameObject* child : node.GetChildren()) {
		//TODO Detect if the child is already inside to avoid duplicates when pressing button more than twice in a row
		if (child->GetComponent(ComponentType::MESHRENDERER) != nullptr)
		{
			AddObject(*child);
		}
		AddHierarchyObjects(*child);
	}
}

const std::map<float, Hit> Quadtree::RayCast(Ray* ray) const
{
	if (!IsLeaf()) 
	{

		std::map<float, Hit> map;


		const std::map<float, Hit> p1 = mChildren[0]->RayCast(ray);
		const std::map<float, Hit> p2 = mChildren[1]->RayCast(ray);
		const std::map<float, Hit> p3 = mChildren[2]->RayCast(ray);
		const std::map<float, Hit> p4 = mChildren[3]->RayCast(ray);

		if (!p1.empty()) {
			map.insert(p1.begin(), p1.end());
		}
		if (!p2.empty()) {
			map.insert(p2.begin(), p2.end());
		}
		if (!p3.empty()) {
			map.insert(p3.begin(), p3.end());
		}
		if (!p4.empty()) {
			map.insert(p4.begin(), p4.end());
		}
			
		if (!map.empty()) {
			return map;
		}
		
	}
	else
	{
		std::map<float, Hit> map;
		bool intersects = false;
		bool intersectsTriangle = false;

		for (const GameObject* child : mGameObjects) {

			MeshRendererComponent* rMesh = reinterpret_cast<MeshRendererComponent*>(child->GetComponent(ComponentType::MESHRENDERER));

			if (rMesh != nullptr) {

				Ray localRay(*ray);
				localRay.pos = child->GetWorldTransform().Inverted().MulPos(ray->pos);
				localRay.dir = child->GetWorldTransform().Inverted().MulDir(ray->dir).Normalized();

				intersects = localRay.Intersects(rMesh->GetAABB());
				if (intersects)
				{
					const unsigned int* indices = rMesh->GetResourceMesh()->GetIndices();
					const float* triangles = rMesh->GetResourceMesh()->GetAttributeData(Attribute::POS);

					for (unsigned int i = 0; i < rMesh->GetResourceMesh()->GetNumberIndices(); i += 3) {
						float3 verticeA = float3(triangles[indices[i]*3], triangles[indices[i]*3 + 1], triangles[indices[i]*3 + 2]);
						float3 verticeB = float3(triangles[indices[i + 1]*3], triangles[indices[i + 1]*3 + 1], triangles[indices[i + 1]*3 + 2]);
						float3 verticeC = float3(triangles[indices[i + 2]*3], triangles[indices[i + 2]*3 + 1], triangles[indices[i + 2]*3 + 2]);

						float distance;
						float3 hitPoint;
						intersectsTriangle = localRay.Intersects(Triangle(verticeA, verticeB, verticeC), &distance, &hitPoint);

						if (intersectsTriangle) {
							Hit hit;
							hit.mDistance = distance;
							hit.mGameObject = child;
							hit.mHitPoint = hitPoint;
							map.insert( std::pair<float, Hit>(distance, hit));							
						}
					}
				}
			}
		}
		if (!map.empty()) {
			return map;
		}
	}

	return std::map<float, Hit>();
}

void Quadtree::UpdateTree()
{
	CleanUp();
	AddHierarchyObjects(*App->GetScene()->GetRoot());
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


	mChildren[0] = new Quadtree(AABB(minPoint, uf_center), mDepthLevel + 1, (mName + "_A").c_str());
	mChildren[1] = new Quadtree(AABB(bf_x, uf_z), mDepthLevel + 1, (mName + "_B").c_str());
	mChildren[2] = new Quadtree(AABB(bf_z, uf_x), mDepthLevel + 1, (mName + "_C").c_str());
	mChildren[3] = new Quadtree(AABB(bf_center, maxPoint), mDepthLevel + 1, (mName + "_D").c_str());

	for (const GameObject* object : mGameObjects)
	{
		mChildren[0]->AddObject(*object);
		mChildren[1]->AddObject(*object);
		mChildren[2]->AddObject(*object);
		mChildren[3]->AddObject(*object);
	}

	mGameObjects.clear();
}
