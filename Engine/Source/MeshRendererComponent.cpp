#include "MeshRendererComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleFileSystem.h"	//Remove Just for the Library definition
#include "ModuleResource.h"
#include "glew.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "DebugPanel.h"

#include "ImporterMaterial.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"



MeshRendererComponent::MeshRendererComponent(GameObject* owner, unsigned int meshUid, unsigned int materialUid) : Component(owner, ComponentType::MESHRENDERER)
{
	
	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();
	mDrawBox = ((DebugPanel*)App->GetEditor()->GetPanel(DEBUGPANEL))->ShouldDrawColliders();
	
	if (meshUid != 0 && materialUid != 0) {
		mMesh = reinterpret_cast<ResourceMesh*>(App->GetResource()->RequestResource(meshUid, Resource::Type::Mesh));
		mMaterial = reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(materialUid, Resource::Type::Material));
		const float3* positions = reinterpret_cast<const float3*>((mMesh->GetAttributeData(Attribute::POS)));
		mAABB.SetFrom(positions, mMesh->GetNumberVertices());
		mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());
	}

}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& other, GameObject* owner) : Component(owner, ComponentType::MESHRENDERER)
{
	mMesh = reinterpret_cast<ResourceMesh*>(App->GetResource()->RequestResource(other.mMesh->GetUID(), Resource::Type::Mesh));
	mMaterial = reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(other.mMaterial->GetUID(), Resource::Type::Material));
	mOBB = other.mOBB;
	mAABB = other.mAABB;
}

MeshRendererComponent::~MeshRendererComponent()
{
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

void MeshRendererComponent::Draw()
{
	if (!mInsideFrustum && App->GetScene()->GetApplyFrustumCulling())
	{
		return;
	}
	mInsideFrustum = false;

	App->GetOpenGL()->BindSceneFramebuffer();

	if (mDrawBox)
	{
		App->GetDebugDraw()->DrawCube(mOBB, float3(0.0f, 0.0f, 1.0f)); // Blue
		App->GetDebugDraw()->DrawCube(mAABBWorld, float3(1.0f, 0.647059f, 0.0f)); // Orange
	}

	unsigned int program = App->GetOpenGL()->GetPBRProgramId();
	glUseProgram(program);
	glUniformMatrix4fv(0, 1, GL_TRUE, mOwner->GetWorldTransform().ptr());
	glBindVertexArray(mMesh->GetVao());
	//TODO: Put all this with imgui
	//Dont update uniforms it every frame
	glUniform3fv(glGetUniformLocation(program, "material.diffuseColor"), 1, &mMaterial->GetDiffuseFactor().xyz()[0]);
	glUniform3fv(glGetUniformLocation(program, "material.specularColor"), 1, &mMaterial->GetSpecularFactor()[0]);
	glUniform1f(glGetUniformLocation(program, "material.shininess"), mMaterial->GetGlossinessFactor());
	if (mMaterial->IsDiffuseTextureEnabled() && mMaterial->GetDiffuseTexture() != nullptr)
	{
		glUniform1i(glGetUniformLocation(program, "material.hasDiffuseMap"), 1);
		GLint diffuseTextureLoc = glGetUniformLocation(program, "material.diffuseTexture");
		glUniform1i(diffuseTextureLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mMaterial->GetDiffuseTexture()->GetOpenGLId());
	}
	else {
		glUniform1i(glGetUniformLocation(program, "material.hasDiffuseMap"), 0);
	}

	if (mMaterial->IsSpecularGlossinessTextureEnabled() && mMaterial->GetSpecularGlossinessTexture() != nullptr)
	{
		glUniform1i(glGetUniformLocation(program, "material.hasSpecularMap"), 1);
		GLint specularTextureLoc = glGetUniformLocation(program, "material.specularTexture");
		glUniform1i(specularTextureLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mMaterial->GetSpecularGlossinessTexture()->GetOpenGLId());
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.hasSpecularMap"), 0);
	}

	if (mMaterial->IsNormalMapEnabled() && mMaterial->GetNormalTexture() != nullptr)
	{
		glUniform1i(glGetUniformLocation(program, "material.hasNormalMap"), 1);
		GLint normalTextureLoc = glGetUniformLocation(program, "material.normalTexture");
		glUniform1i(normalTextureLoc, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mMaterial->GetNormalTexture()->GetOpenGLId());
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.hasNormalMap"), 0);
	}

	if (mMaterial->IsShininessMapEnabled())
	{
		glUniform1i(glGetUniformLocation(program, "material.hasShininessMap"), 1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.hasShininessMap"), 0);
	}
	glDrawElements(GL_TRIANGLES, mMesh->GetNumberIndices(), GL_UNSIGNED_INT, 0);
	glUseProgram(0);
	glBindVertexArray(0);
	App->GetOpenGL()->UnbindSceneFramebuffer();
}

void MeshRendererComponent::Update()
{
	//Draw();
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
	archive.AddInt("ID", mID);
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

