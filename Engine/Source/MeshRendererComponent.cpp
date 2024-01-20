#include "MeshRendererComponent.h"
#include "ImporterMesh.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "ModuleRenderTest.h"



MeshRendererComponent::MeshRendererComponent(GameObject* owner) 
	:Component("Mesh Renderer" ,owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh())
{

	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner)
	:Component(owner->GetName()->c_str(), owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh())
{


	
	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();


}

void MeshRendererComponent::Draw()
{
	if (!mInsideFrustum && App->GetScene()->GetApplyFrustumCulling())
	{
		return;
	}
	App->GetOpenGL()->BindSceneFramebuffer();

	if (mDrawBox)
	{
		App->GetDebugDraw()->DrawBoundingBox(mOBB);
	}
	glUseProgram(App->GetTest()->GetProgramId());
	glUniformMatrix4fv(0, 1, GL_TRUE, mOwner->GetWorldTransform().ptr());
	glBindVertexArray(mMesh->GetVao());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, App->GetTest()->GetDifuseTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, App->GetTest()->GetNormalTexture());
	glDrawElements(GL_TRIANGLES, mMesh->mNumIndices, GL_UNSIGNED_INT, 0);
	glUseProgram(0);
	glBindVertexArray(0);
	App->GetOpenGL()->UnbindSceneFramebuffer();
}

void MeshRendererComponent::Load(const char* uid)
{
	Importer::Mesh::Load(mMesh, uid);

	
	float3* positions = (float3*)(mMesh->GetAttributeData(Attribute::POS));

	mAABB.SetFrom(positions, mMesh->mNumVertices);

	float4x4 model = mOwner->GetWorldTransform();

	mOBB.SetFrom(mAABB, model);

}

void MeshRendererComponent::Update()
{
	mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());
	Draw();
}

Component* MeshRendererComponent::Clone(GameObject* owner)
{
	return new MeshRendererComponent(*this, owner);
}


void MeshRendererComponent::Save(Archive& archive) const {
	archive.AddInt("ID", mID);
	archive.AddInt("MeshID",mMesh->mUID);
	archive.AddInt("MaterialID", mMesh->mUID);
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddBool("isEnabled", IsEnabled());
}

MeshRendererComponent* MeshRendererComponent::LoadFromJSON(const rapidjson::Value& componentJson, GameObject* owner) {
	int ID = { 0 };
	int meshID = { 0 };
	int materialID = { 0 };
	if (componentJson.HasMember("ID") && componentJson["ID"].IsInt()) {
		meshID = componentJson["ID"].GetInt();
	}
	if (componentJson.HasMember("MeshID") && componentJson["MeshID"].IsInt()) {
		meshID = componentJson["MeshID"].GetInt();
	}
	if (componentJson.HasMember("MaterialID") && componentJson["MaterialID"].IsInt()) {
		materialID = componentJson["MaterialID"].GetInt();
	}

	MeshRendererComponent* m = new MeshRendererComponent(owner);
	if (meshID != 0) {
		m->Load(std::to_string(meshID).c_str());
	}
	if (materialID != 0) {
		//TODO check if we separate load function from each Component to load specific resources
		//m->Load(std::to_string(materialID).c_str());
	}

	return m;

}

