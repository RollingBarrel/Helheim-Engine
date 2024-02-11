#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "GeometryBatch.h"
#include <cassert>
#include "glew.h"
#include <float3.h>
#include "MeshRendererComponent.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "ImporterTexture.h"




GeometryBatch::GeometryBatch(MeshRendererComponent* mesh)
{
	mMeshComponents.push_back(mesh);
	mUniqueMeshes.push_back(mesh->GetResourceMesh());
	mAttributes = mesh->GetResourceMesh()->GetAttributes();
	mVertexSize = mesh->GetResourceMesh()->GetVertexSize();

	

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glGenBuffers(1, &mVbo);
	glGenBuffers(1, &mEbo);
	glGenBuffers(1, &mSsboModels);
	glGenBuffers(1, &mSsboMaterials);
	glGenBuffers(1, &mIbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->GetResourceMesh()->GetNumVertices() * mVertexSize, mUniqueMeshes[0]->GetInterleavedData(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mUniqueMeshes[0]->mNumIndices * sizeof(unsigned int), mUniqueMeshes[0]->GetIndices(), GL_STATIC_DRAW);
	
	

	Material* material = new Material();

	material->diffuseColor = mesh->GetMaterial()->mDiffuseFactor;
	material->diffuseTexture = mesh->GetMaterial()->mDiffuseTexture->openGlId;
	material->specularColor =  float4(mesh->GetMaterial()->mSpecularFactor, 0);
	material->specularTexture = mesh->GetMaterial()->mSpecularGlossinessTexture->openGlId;
	material->normalTexture = mesh->GetMaterial()->mNormalTexture->openGlId;
	material->shininess = mesh->GetMaterial()->mGlossinessFactor;
	material->hasDiffuseMap = mesh->GetMaterial()->mEnableDiffuseTexture;
	material->hasSpecularMap = mesh->GetMaterial()->mEnableSpecularGlossinessTexture;
	material->hasShininessMap = mesh->GetMaterial()->mEnableShinessMap;
	material->hasNormalMap = mesh->GetMaterial()->mEnableNormalMap;

	mMaterials.push_back(material);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Material), mMaterials[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	unsigned int idx = 0;
	for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, (*it)->size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it)->offset);
		
		++idx;
	}


	glBindVertexArray(0);


}

GeometryBatch::~GeometryBatch()
{

}



void GeometryBatch::AddMesh(MeshRendererComponent* cMesh) //NEEDS A FULL REMAKE
{
	
}

void GeometryBatch::AddCommand(Command* command)
{
	mCommands.push_back(command);
}



void GeometryBatch::Draw()
{
	App->GetOpenGL()->BindSceneFramebuffer();

	glUseProgram(App->GetOpenGL()->GetPBRProgramId());

	glBindVertexArray(mVao);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, mCommands.size() * sizeof(Command), mCommands[0], GL_STATIC_DRAW);



	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModels);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mMeshComponents.size() * sizeof(float) * 16, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, mSsboModels);
	
	unsigned int offset = 0;

	for (auto mesh : mMeshComponents) {

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float) * 16, mesh->GetOwner()->GetWorldTransform().Transposed().ptr());

		offset += sizeof(float) * 16;
	}


	for (auto material : mMaterials) {
		glMakeTextureHandleResidentARB(material->diffuseTexture);
		glMakeTextureHandleResidentARB(material->specularTexture);
		glMakeTextureHandleResidentARB(material->normalTexture);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);

	//glBindBuffer(GL_ARRAY_BUFFER, mIbo);

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, mCommands.size(), 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	glUseProgram(0);
	glBindVertexArray(0);

	for (auto material : mMaterials) {
		glMakeTextureHandleNonResidentARB(material->diffuseTexture);
		glMakeTextureHandleNonResidentARB(material->specularTexture);
		glMakeTextureHandleNonResidentARB(material->normalTexture);
	}

	App->GetOpenGL()->UnbindSceneFramebuffer();

	mCommands.clear();
}

