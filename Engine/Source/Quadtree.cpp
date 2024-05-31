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

bool Quadtree::AddObject(const MeshRendererComponent& meshRenderer)
{
	const GameObject& object = *meshRenderer.GetOwner();
	const AABB& objectAABB = meshRenderer.GetAABB();


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
		mChildren[0]->AddObject(meshRenderer);
		mChildren[1]->AddObject(meshRenderer);
		mChildren[2]->AddObject(meshRenderer);
		mChildren[3]->AddObject(meshRenderer);
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
		mGameObjects.erase(std::remove_if(mGameObjects.begin(), mGameObjects.end(),
			[&](const GameObject* ptr) { return ptr->GetID() == object.GetID(); }), mGameObjects.end());
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
		MeshRendererComponent* mesh = reinterpret_cast<MeshRendererComponent*>(child->GetComponent(ComponentType::MESHRENDERER));
		if (mesh)
		{
			AddObject(*mesh);
		}
		AddHierarchyObjects(*child);
	}
}

void Quadtree::RayCast(const Ray& ray, std::map<float, Hit>& map) const
{
	if (!IsLeaf()) 
	{
		mChildren[0]->RayCast(ray, map);
		mChildren[1]->RayCast(ray, map);
		mChildren[2]->RayCast(ray, map);
		mChildren[3]->RayCast(ray, map);
	}
	else
	{
		for (const GameObject* child : mGameObjects) 
		{
			MeshRendererComponent* rMesh = reinterpret_cast<MeshRendererComponent*>(child->GetComponent(ComponentType::MESHRENDERER));
			if (rMesh != nullptr) 
			{
				float3 pos = child->GetWorldTransform().Inverted().MulPos(ray.pos);
				float3 dir = child->GetWorldTransform().Inverted().MulDir(ray.dir).Normalized();
				Ray localRay(pos, dir);

				if (localRay.Intersects(rMesh->GetAABB()))
				{
					const unsigned int* indices = rMesh->GetResourceMesh()->GetIndices();
					const float* vertices = rMesh->GetResourceMesh()->GetAttributeData(Attribute::POS);

					for (unsigned int i = 0; i < rMesh->GetResourceMesh()->GetNumberIndices(); i += 3) 
					{
						float3 vertexA = float3(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
						float3 vertexB = float3(vertices[indices[i + 1] * 3], vertices[indices[i + 1] * 3 + 1], vertices[indices[i + 1] * 3 + 2]);
						float3 vertexC = float3(vertices[indices[i + 2] * 3], vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 2] * 3 + 2]);

						float distance;
						float3 hitPoint;
						if (localRay.Intersects(Triangle(vertexA, vertexB, vertexC), &distance, &hitPoint))
						{
							Hit hit;
							hit.mDistance = distance;
							hit.mGameObject = child;
							hit.mHitPoint = hitPoint;
							map.insert({ distance, hit });
						}
					}
				}
			}
		}
	}
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
		MeshRendererComponent* mesh = reinterpret_cast<MeshRendererComponent*>(object->GetComponent(ComponentType::MESHRENDERER));
		assert(mesh && "Game object does not have mesh renderer component");
		mChildren[0]->AddObject(*mesh);
		mChildren[1]->AddObject(*mesh);
		mChildren[2]->AddObject(*mesh);
		mChildren[3]->AddObject(*mesh);
	}

	mGameObjects.clear();
}
