#include "MeshRendererComponent.h"
#include "ImporterMesh.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "ModuleRenderTest.h"
#include <iostream>
#include <random>

MeshRendererComponent::MeshRendererComponent(GameObject* owner) 
	:Component("Mesh Renderer" ,owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh())
{
	//Create Random BBO
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define the distribution for values between -100 and 100
	std::uniform_int_distribution<int> distribution(-100, 90);

	// Generate three random values
	float rv1 = distribution(gen)/10;
	float rv2 = distribution(gen)/10;
	float rv3 = distribution(gen)/10;
	mOBB = OBB(AABB(float3(rv1, rv2, rv3), float3(rv1+1.0f, rv2 + 1.0f, rv3 + 1.0f)));

}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner)
	:Component(owner->GetName()->c_str(), owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh())
{
	//Create Random BBO
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define the distribution for values between -100 and 100
	std::uniform_int_distribution<int> distribution(-100, 90);

	// Generate three random values
	float rv1 = distribution(gen) / 10;
	float rv2 = distribution(gen) / 10;
	float rv3 = distribution(gen) / 10;
	mOBB = OBB(AABB(float3(rv1, rv2, rv3), float3(rv1 + 1.0f, rv2 + 1.0f, rv3 + 1.0f)));

}

void MeshRendererComponent::Draw()
{
	if (!mInsideFrustum)
	{
		return;
	}
	if (mDrawBox)
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
}

void MeshRendererComponent::Update()
{
	Draw();
}

Component* MeshRendererComponent::Clone(GameObject* owner)
{
	return new MeshRendererComponent(*this, owner);
}
