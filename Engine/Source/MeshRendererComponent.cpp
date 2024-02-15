#include "MeshRendererComponent.h"
#include "ImporterMesh.h"
#include "ImporterTexture.h"
#include "ImporterMaterial.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "GeometryBatch.h"





MeshRendererComponent::MeshRendererComponent(GameObject* owner) 
	:Component(owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh()), mMaterial(new ResourceMaterial())
{
	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();
	//mMesh->mUID = LCG().Int();
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner)
	:Component(owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh(*original.mMesh)), mMaterial(new ResourceMaterial(*original.mMaterial))
{

	mOBB = original.mOBB;
	mAABB = original.mAABB;
	mBatch = original.mBatch;
	
}


void MeshRendererComponent::Load(unsigned int meshUid, unsigned int materialUid)
{
	mMesh->mUID = meshUid;
	mMaterial->mUID = materialUid;
	Importer::Mesh::Load(mMesh, std::to_string(meshUid).c_str());
	Importer::Material::Load(mMaterial, std::to_string(materialUid).c_str());
	const float3* positions = (float3*)(mMesh->GetAttributeData(Attribute::POS));

	mAABB.SetFrom(positions, mMesh->mNumVertices);

	float4x4 model = mOwner->GetWorldTransform();

	mOBB.SetFrom(mAABB, model);

	//ResourceMaterial Load

}

void MeshRendererComponent::Update()
{
	mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());
	//Draw();
}


Component* MeshRendererComponent::Clone(GameObject* owner) const
{
	return new MeshRendererComponent(*this, owner);
}

void MeshRendererComponent::AddCommand(unsigned int instanceCounter)
{
	Command command;
	command.mCount = mMesh->GetNumIndices();
	command.mInstanceCount = 1;
	command.firstIndex = mMesh->GetEboPosition() / sizeof(GLuint);
	command.baseVertex = mMesh->GetVboPosition() / mBatch->GetVertexSize();
	command.baseInstance = instanceCounter;

	mBatch->AddCommand(command);
}


void MeshRendererComponent::Save(Archive& archive) const {
	archive.AddInt("ID", mID);
	archive.AddInt("MeshID",mMesh->mUID);
	archive.AddInt("MaterialID", mMaterial->mUID);
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

	if (meshID != 0 && materialID != 0) {
		Load(meshID, materialID);
	}

}

