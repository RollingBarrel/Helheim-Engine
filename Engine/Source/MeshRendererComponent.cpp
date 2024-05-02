#include "MeshRendererComponent.h"
#include "AnimationComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "glew.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "DebugPanel.h"
#include "GeometryBatch.h"

#include "ImporterMaterial.h"

#include "float4.h"
#include "float3.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"



MeshRendererComponent::MeshRendererComponent(GameObject* owner) : Component(owner, ComponentType::MESHRENDERER), mMesh(nullptr), mMaterial(nullptr)
{
	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();
	//mDrawBox = ((DebugPanel*)App->GetEditor()->GetPanel(DEBUGPANEL))->ShouldDrawColliders();

	mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());

	GameObject* root = mOwner->FindFirstParent(mOwner);
	mAnimationComponent = reinterpret_cast<AnimationComponent*>(root->GetComponent(ComponentType::ANIMATION));
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& other, GameObject* owner) : Component(owner, ComponentType::MESHRENDERER)
{
	mMesh = (other.mMesh) ? reinterpret_cast<ResourceMesh*>(App->GetResource()->RequestResource(other.mMesh->GetUID(), Resource::Type::Mesh)) : nullptr;
	mMaterial = (other.mMaterial) ? reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(other.mMaterial->GetUID(), Resource::Type::Material)) : nullptr;
	mOBB = other.mOBB;
	mAABB = other.mAABB;
	//mDrawBox = ((DebugPanel*)App->GetEditor()->GetPanel(DEBUGPANEL))->ShouldDrawColliders();

	App->GetOpenGL()->BatchAddMesh(this);
	mAABBWorld = mOBB.MinimalEnclosingAABB();

	mAnimationComponent = reinterpret_cast<AnimationComponent*>(mOwner->FindFirstParent(mOwner)->GetComponent(ComponentType::ANIMATION));
}

MeshRendererComponent::~MeshRendererComponent()
{
	if (mMesh)
	{
		App->GetOpenGL()->BatchRemoveMesh(this);
		App->GetResource()->ReleaseResource(mMesh->GetUID());
		mMesh = nullptr;
	}
	if (mMaterial)
	{
		App->GetResource()->ReleaseResource(mMaterial->GetUID());
		mMaterial = nullptr;
	}
}

void MeshRendererComponent::SetMesh(unsigned int uid)
{
	ResourceMesh* tmpMesh = reinterpret_cast<ResourceMesh*>(App->GetResource()->RequestResource(uid, Resource::Type::Mesh));
	if (tmpMesh && mMesh)
	{
		if (mMaterial)
			App->GetOpenGL()->BatchRemoveMesh(this);
		App->GetResource()->ReleaseResource(mMesh->GetUID());
		mMesh = nullptr;
	}
	if (tmpMesh)
	{
		mMesh = tmpMesh;

		const float3* positions = reinterpret_cast<const float3*>((mMesh->GetAttributeData(Attribute::POS)));
		mAABB.SetFrom(positions, mMesh->GetNumberVertices());
		mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());
		mAABBWorld = mOBB.MinimalEnclosingAABB();
		if (mMaterial)
			App->GetOpenGL()->BatchAddMesh(this);
	}
}


void MeshRendererComponent::SetMaterial(unsigned int uid)
{
	ResourceMaterial* tmpMaterial = reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(uid, Resource::Type::Material));
	if (tmpMaterial && mMaterial)
	{
		if (mMesh)
			App->GetOpenGL()->BatchRemoveMesh(this);
		App->GetResource()->ReleaseResource(mMaterial->GetUID());
		mMaterial = nullptr;
	}
	if (tmpMaterial)
	{
		mMaterial = tmpMaterial;
		if (mMesh)
			App->GetOpenGL()->BatchAddMesh(this);
	}
	//TODO: Material Default
	//else
	//{
	//	mMaterial = new ResourceMaterial(0, float4(0.1f,0.1f,0.1f,0.1f), float3(1.0f), 1.0f ,-1,-1,-1);
	//}
}

void MeshRendererComponent::LoadAnimatedMesh(bool isAnimated) {

	mPalette.clear();
	std::vector<std::pair<GameObject*, float4x4>>& inverseBindMats = mAnimationComponent->mGameobjectsInverseMatrices;
	mPalette.reserve(inverseBindMats.size());
	for (unsigned i = 0; i < inverseBindMats.size(); ++i)
	{
		mPalette.push_back((inverseBindMats[i].first->TranformInFirstGameObjectSpace() * inverseBindMats[i].second).Transposed());
	}
	//int o = 1;
	//LOG("Palette:\n %f\t%f\t%f\t%f\n%f\t%f\t%f\t%f\n%f\t%f\t%f\t%f\n%f\t%f\t%f\t%f", palette[o][0][0], palette[o][0][1], palette[o][0][2], palette[o][0][3], palette[o][1][0], palette[o][1][1], palette[o][1][2], palette[o][1][3], palette[o][2][0], palette[o][2][1], palette[o][2][2], palette[o][2][3], palette[o][3][0], palette[o][3][1], palette[o][3][2], palette[o][3][3]);

	//unsigned int programId = App->GetOpenGL()->GetPBRProgramId();
	//glUseProgram(programId);
	//glUniformMatrix4fv(glGetUniformLocation(programId, "palette"), palette.size(), GL_TRUE, palette[0].ptr());
	//
	//glUniform1i(glGetUniformLocation(programId, "hasAnimation"), isAnimated);
	//glUseProgram(0);

}

void MeshRendererComponent::Update() {
	if (mAnimationComponent && mAnimationComponent->GetIsPlaying())
	{
		LoadAnimatedMesh(mIsAnimated);
	}
}

void MeshRendererComponent::Enable()
{
	if(mMaterial && mMesh)
		App->GetOpenGL()->BatchAddMesh(this);
}

void MeshRendererComponent::Disable()
{
	if (mMaterial && mMesh)
		App->GetOpenGL()->BatchRemoveMesh(this);
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
	archive.AddInt("MeshID", mMesh->GetUID());
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

	SetMesh(meshID);
	SetMaterial(materialID);
}

