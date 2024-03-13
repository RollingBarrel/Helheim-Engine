#include "MeshRendererComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "glew.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "GeometryBatch.h"

#include "ImporterMaterial.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"



MeshRendererComponent::MeshRendererComponent(GameObject* owner, unsigned int meshUid, unsigned int materialUid) : Component(owner, ComponentType::MESHRENDERER)
{
	mMesh = reinterpret_cast<ResourceMesh*>(App->GetResource()->RequestResource(meshUid, Resource::Type::Mesh));
	mMaterial = reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(materialUid, Resource::Type::Material));
	assert(mMesh && "Component Mesh without resource mesh");
	assert(mMaterial && "Component Mesh without resource material");
	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();
	
	if (meshUid != 0 && materialUid != 0) {
		mMesh = reinterpret_cast<ResourceMesh*>(App->GetResource()->RequestResource(meshUid, Resource::Type::Mesh));
		mMaterial = reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(materialUid, Resource::Type::Material));
		const float3* positions = reinterpret_cast<const float3*>((mMesh->GetAttributeData(Attribute::POS)));
		mAABB.SetFrom(positions, mMesh->GetNumberVertices());
		mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());
		mAABBWorld = mOBB.MinimalEnclosingAABB();

	}

	mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());

	App->GetOpenGL()->BatchAddMesh(this);
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& other, GameObject* owner) : Component(owner, ComponentType::MESHRENDERER)
{
	mMesh = reinterpret_cast<ResourceMesh*>(App->GetResource()->RequestResource(other.mMesh->GetUID(), Resource::Type::Mesh));
	mMaterial = reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(other.mMaterial->GetUID(), Resource::Type::Material));
	mOBB = other.mOBB;
	mAABB = other.mAABB;

	App->GetOpenGL()->BatchAddMesh(this);
	mAABBWorld = mOBB.MinimalEnclosingAABB();

}

MeshRendererComponent::~MeshRendererComponent()
{
	App->GetOpenGL()->BatchRemoveMesh(this);
	if (mMesh)
	{
		App->GetResource()->ReleaseResource(mMesh->GetUID());
		mMesh = nullptr;
	}
	if (mMaterial)
	{
		App->GetResource()->ReleaseResource(mMaterial->GetUID());
		mMesh = nullptr;
	}
}

Component* MeshRendererComponent::Clone(GameObject* owner) const
{
	return new MeshRendererComponent(*this, owner);
}

void MeshRendererComponent::RefreshBoundingBoxes()
{
	mOBB = OBB(mAABB);
	mOBB.Transform(mOwner->GetWorldTransform());
	mAABBWorld = mOBB.MinimalEnclosingAABB();
}

void MeshRendererComponent::Save(Archive& archive) const {
	archive.AddInt("ID", GetID());
	archive.AddInt("MeshID",mMesh->GetUID());
	archive.AddInt("MaterialID", mMaterial->GetUID());
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddBool("isEnabled", IsEnabled());
}

void MeshRendererComponent::LoadFromJSON(const rapidjson::Value& componentJson, GameObject* owner) {
	int ID = { 0 };
	int meshID = { 0 };
	int materialID = { 0 };
	if (componentJson.HasMember("ID") && componentJson["ID"].IsInt()) {
		ID = componentJson["ID"].GetInt();
	}
	if (componentJson.HasMember("MeshID") && componentJson["MeshID"].IsInt()) {
		meshID = componentJson["MeshID"].GetInt();
	}
	if (componentJson.HasMember("MaterialID") && componentJson["MaterialID"].IsInt()) {
		materialID = componentJson["MaterialID"].GetInt();
	}

	mMesh = reinterpret_cast<ResourceMesh*>(App->GetResource()->RequestResource(meshID, Resource::Type::Mesh));
	mMaterial = reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(materialID, Resource::Type::Material));

	const float3* positions = reinterpret_cast<const float3*>((mMesh->GetAttributeData(Attribute::POS)));
	mAABB.SetFrom(positions, mMesh->GetNumberVertices());
	mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());
}

