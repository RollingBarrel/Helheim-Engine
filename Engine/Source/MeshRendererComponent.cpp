#include "MeshRendererComponent.h"
#include "ImporterModel.h"
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
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner)
	:Component(owner->GetName()->c_str(), owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh()), mMaterial(new ResourceMaterial())
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
		glUniform1i(glGetUniformLocation(program, "material.hasNomalMap"), 1);
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

void MeshRendererComponent::Load(const char* uid)
{
	ResourceModel* rModel = new ResourceModel();
	Importer::Model::Load(rModel, uid);
	Importer::Mesh::Load(mMesh, std::to_string(rModel->meshUID).c_str());
	Importer::Material::Load(mMaterial, std::to_string(rModel->materiaUID).c_str());
	delete rModel;
	float3* positions = (float3*)(mMesh->GetAttributeData(Attribute::POS));

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

Component* MeshRendererComponent::Clone(GameObject* owner)
{
	return new MeshRendererComponent(*this, owner);
}

//void MeshRendererComponent::LoadPBR(const char* assetFileName)
//{
//	Clear();
//	tinygltf::TinyGLTF gltfContext;
//	tinygltf::Model srcModel;
//	std::string error, warning;
//	bool loadOk = gltfContext.LoadASCIIFromFile(&srcModel, &error, &warning, assetFileName);
//	if (!loadOk)
//	{
//		LOG("Error loading %s: %s", assetFileName, error.c_str());
//	}
//	for (const auto& srcMesh : srcModel.meshes)
//	{
//		for (const auto& primitive : srcMesh.primitives)
//		{
//			Mesh* mesh = new Mesh();
//			mesh->LoadVBO(srcModel, srcMesh, primitive);
//			mesh->LoadEBO(srcModel, srcMesh, primitive);
//			mesh->CreateVAO();
//			mMeshes.push_back(mesh);
//
//			int materialID = primitive.material;
//
//			//if (materialID != -1) {
//			//	ResourceMaterial* material = new ResourceMaterial();
//			//	material->SetTemporalID(mTemporalID);
//			//	material->LoadMaterial(srcModel, srcModel.materials[materialID]);
//			//	mesh->SetMaterial(material);
//			//}
//		}
//	}
//}

void MeshRendererComponent::LoadByIDTemporal(const int id)
{
	mTemporalID = id;
	switch (id) {
		case 0:
			LoadPBR("Assets\\Models\\Clock\\Clock.gltf");
			break;
		case 1:
			LoadPBR("Assets\\Models\\DollHouse\\Dollhouse.gltf");
			break;
		case 2:
			LoadPBR("Assets\\Models\\Drawers\\Drawers.gltf");
			break;
		case 3:
			LoadPBR("Assets\\Models\\Duck\\Duck.gltf");
			break;
		case 4:
			LoadPBR("Assets\\Models\\Firetruck\\Firetruck.gltf");
			break;
		case 5:
			LoadPBR("Assets\\Models\\Floor\\Floor.gltf");
			break;
		case 6:
			LoadPBR("Assets\\Models\\Hearse\\Hearse.gltf");
			break;
		case 7:
			LoadPBR("Assets\\Models\\Player\\Player.gltf");
			break;
		case 8:
			LoadPBR("Assets\\Models\\SpinningTop\\SpinningTop.gltf");
			break;
		case 9:
			LoadPBR("Assets\\Models\\testing\\Robot\\Robot.gltf");
			break;
		case 10:
			LoadPBR("Assets\\Models\\Wall\\Wall.gltf");
			break;
		case 11:
			LoadPBR("Assets\\Models\\ZomBunny\\Zombunny.gltf");
			break;
		default:
			// Handle the case when the ID is not found
			break;
		}
}

void MeshRendererComponent::Clear()
{
	mMeshes.clear();
}
