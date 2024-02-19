#include "MeshRendererComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleFileSystem.h"	//Remove Just for the Library definition
#include "glew.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"

#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"



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
	
}

void MeshRendererComponent::Draw()
{
	if (!mInsideFrustum && App->GetScene()->GetApplyFrustumCulling())
	{
		return;
	}
	if (mDrawBox)
	{
		App->GetDebugDraw()->DrawBoundingBox(mOBB);
	}
	mInsideFrustum = false;

	App->GetOpenGL()->BindSceneFramebuffer();

	if (mDrawBox)
	{
		App->GetDebugDraw()->DrawBoundingBox(mOBB);
	}

	unsigned int program = App->GetOpenGL()->GetPBRProgramId();
	glUseProgram(program);
	glUniformMatrix4fv(0, 1, GL_TRUE, mOwner->GetWorldTransform().ptr());
	glBindVertexArray(mMesh->GetVao());
	//TODO: Put all this with imgui
	//Dont update uniforms it every frame
	glUniform3fv(glGetUniformLocation(program, "material.diffuseColor"), 1, &mMaterial->mDiffuseFactor.xyz()[0]);
	glUniform3fv(glGetUniformLocation(program, "material.specularColor"), 1, &mMaterial->mSpecularFactor[0]);
	glUniform1f(glGetUniformLocation(program, "material.shininess"), mMaterial->mGlossinessFactor);
	if (mMaterial->mEnableDiffuseTexture && mMaterial->mDiffuseTexture != nullptr)
	{
		glUniform1i(glGetUniformLocation(program, "material.hasDiffuseMap"), 1);
		GLint diffuseTextureLoc = glGetUniformLocation(program, "material.diffuseTexture");
		glUniform1i(diffuseTextureLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mMaterial->mDiffuseTexture->GetOpenGLId());
	}
	else {
		glUniform1i(glGetUniformLocation(program, "material.hasDiffuseMap"), 0);
	}

	if (mMaterial->mEnableSpecularGlossinessTexture && mMaterial->mSpecularGlossinessTexture != nullptr)
	{
		glUniform1i(glGetUniformLocation(program, "material.hasSpecularMap"), 1);
		GLint specularTextureLoc = glGetUniformLocation(program, "material.specularTexture");
		glUniform1i(specularTextureLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mMaterial->mSpecularGlossinessTexture->GetOpenGLId());
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.hasSpecularMap"), 0);
	}

	if (mMaterial->mEnableNormalMap && mMaterial->mNormalTexture != nullptr)
	{
		glUniform1i(glGetUniformLocation(program, "material.hasNormalMap"), 1);
		GLint normalTextureLoc = glGetUniformLocation(program, "material.normalTexture");
		glUniform1i(normalTextureLoc, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mMaterial->mNormalTexture->GetOpenGLId());
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.hasNormalMap"), 0);
	}

	if (mMaterial->mEnableShinessMap)
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

void MeshRendererComponent::Load(unsigned int meshUid, unsigned int materialUid)
{
	mMesh = Importer::Mesh::Load(std::string(LIBRARY_MESH_PATH + std::to_string(meshUid) + ".mesh").c_str(), meshUid);
	//TODO
	//mMaterial = Importer::Material::Load(std::string(LIBRARY_MATERIAL_PATH + std::to_string(materialUid) + ".mat").c_str(), std::to_string(materialUid).c_str());

	const float3* positions = (float3*)(mMesh->GetAttributeData(Attribute::POS));
	mAABB.SetFrom(positions, mMesh->GetNumberVertices());

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

	if (meshID != 0 && materialID != 0) {
		Load(meshID, materialID);
	}

}

