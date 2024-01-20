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
	//if (!mInsideFrustum)
	//{
	//	return;
	//}
	if (*mDrawBox)
	{
		App->GetDebugDraw()->DrawBoundingBox(mOBB);
	}
	App->GetOpenGL()->BindSceneFramebuffer();
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
	mInsideFrustum = false;
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
