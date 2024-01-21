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
#include "ModuleProgram.h"





MeshRendererComponent::MeshRendererComponent(GameObject* owner) 
	:Component("Mesh Renderer" ,owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh()), mMaterial(new ResourceMaterial())
{

	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();
	//mMesh->mUID = LCG().Int();
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner)
	:Component("Mesh Renderer", owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh(*original.mMesh)), mMaterial(new ResourceMaterial(*original.mMaterial))
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
	//for (Mesh* mesh : mMeshes)
	//{
	//	App->GetOpenGL()->BindSceneFramebuffer();
	//	mesh->Render(lightDir, lightColor, lightIntensity, ambientColor);
	//	App->GetOpenGL()->UnbindSceneFramebuffer();
	//}

	App->GetOpenGL()->BindSceneFramebuffer();

	if (mDrawBox)
	{
		App->GetDebugDraw()->DrawBoundingBox(mOBB);
	}

	unsigned int program = App->GetProgram()->GetPBRProgramId();
	glUseProgram(program);
	glUniformMatrix4fv(0, 1, GL_TRUE, mOwner->GetWorldTransform().ptr());
	glBindVertexArray(mMesh->GetVao());
	//TODO: Put all this with imgui
	//Dont update uniforms it every frame
	glUniform3fv(glGetUniformLocation(program, "material.diffuseColor"), 1, &mMaterial->GetDiffuseFactor().xyz()[0]);
	glUniform3fv(glGetUniformLocation(program, "material.specularColor"), 1, &mMaterial->GetSpecularFactor()[0]);
	glUniform1f(glGetUniformLocation(program, "material.shininess"), mMaterial->GetGlossinessFactor());
	if (mMaterial->GetEnableDiffuseTexture() && mMaterial->GetDiffuseMap() != nullptr)
	{
		glUniform1i(glGetUniformLocation(program, "material.hasDiffuseMap"), 1);
		GLint diffuseTextureLoc = glGetUniformLocation(program, "material.diffuseTexture");
		glUniform1i(diffuseTextureLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mMaterial->GetDiffuseMap()->openGlId);
	}
	else {
		glUniform1i(glGetUniformLocation(program, "material.hasDiffuseMap"), 0);
	}

	if (mMaterial->GetEnableSpecularGlossinessTexture() && mMaterial->GetSpecularMap() != nullptr)
	{
		glUniform1i(glGetUniformLocation(program, "material.hasSpecularMap"), 1);
		GLint specularTextureLoc = glGetUniformLocation(program, "material.specularTexture");
		glUniform1i(specularTextureLoc, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mMaterial->GetSpecularMap()->openGlId);
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.hasSpecularMap"), 0);
	}

	if (mMaterial->GetEnableNormalMap() && mMaterial->GetNormalMap() != nullptr)
	{
		glUniform1i(glGetUniformLocation(program, "material.hasNormalMap"), 1);
		GLint normalTextureLoc = glGetUniformLocation(program, "material.normalTexture");
		glUniform1i(normalTextureLoc, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mMaterial->GetNormalMap()->openGlId);
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.hasNormalMap"), 0);
	}

	if (mMaterial->GetEnableShinessMap())
	{
		glUniform1i(glGetUniformLocation(program, "material.hasShininessMap"), 1);
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.hasShininessMap"), 0);
	}
	glDrawElements(GL_TRIANGLES, mMesh->mNumIndices, GL_UNSIGNED_INT, 0);
	glUseProgram(0);
	glBindVertexArray(0);
	App->GetOpenGL()->UnbindSceneFramebuffer();
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
	Draw();
}


Component* MeshRendererComponent::Clone(GameObject* owner) const
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

	if (meshID != 0) {
		Load(std::to_string(meshID).c_str());
	}
	if (materialID != 0) {
		//TODO check if we separate load function from each Component to load specific resources
		//m->Load(std::to_string(materialID).c_str());
	}

}

