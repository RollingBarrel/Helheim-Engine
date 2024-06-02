#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleScene.h"
#include "GeometryBatch.h"
#include <cassert>
#include "glew.h"
#include "float3.h"
#include "float4x4.h"
#include "MeshRendererComponent.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "AnimationComponent.h"

GeometryBatch::GeometryBatch(const MeshRendererComponent* cMesh)
{
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &mSsboAligment);

	memset(mSsboModelMatricesData, 0, sizeof(mSsboModelMatricesData));
	memset(mSsboIndicesData, 0, sizeof(mSsboIndicesData));
	memset(mSync, 0, sizeof(mSync));

	cMesh->GetResourceMesh()->GetAttributes(mAttributes, Attribute::Usage::RENDER);
	mVertexSize = cMesh->GetResourceMesh()->GetVertexSize(Attribute::Usage::RENDER);

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glGenBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glGenBuffers(1, &mEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glGenBuffers(1, &mSsboModelMatrices);
	glGenBuffers(1, &mSsboMaterials);
	glGenBuffers(1, &mSsboIndices);
	glGenBuffers(1, &mIbo);

	glGenBuffers(1, &mPaletteSsbo);
	glGenBuffers(1, &mAnimSsbo);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 15, mPaletteSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 20, mAnimSsbo);

	for (std::vector<Attribute>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		if (!(it->usage & Attribute::Usage::RENDER))
			continue;
		unsigned int idx = 0;
		switch (it->type)
		{
		case Attribute::POS:
			idx = 0;
			break;
		case Attribute::UV:
			idx = 1;
			break;
		case Attribute::NORMAL:
			idx = 2;
			break;
		case Attribute::TANGENT:
			idx = 3;
			break;
		case Attribute::JOINT:
		case Attribute::WEIGHT:
			break;
		default:
			assert(false && "Unsuported Vertex Attribute");
			break;
		}

		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, (*it).size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it).offset); 
	}

	glBindVertexArray(0);
	AddMeshComponent(cMesh);
}

GeometryBatch::~GeometryBatch()
{
	delete[] mVboData;
	delete[] mEboData;
	glDeleteVertexArrays(1, &mVao);
	glDeleteBuffers(1, &mVbo);
	glDeleteBuffers(1, &mEbo);
	glDeleteBuffers(1, &mSsboModelMatrices);
	glDeleteBuffers(1, &mSsboMaterials);
	glDeleteBuffers(1, &mSsboIndices);
	glDeleteBuffers(1, &mIbo);

	glDeleteBuffers(1, &mPaletteSsbo);
	glDeleteBuffers(1, &mAnimSsbo);

	mMeshComponents.clear();
	mUniqueMeshes.clear();
	mAttributes.clear();
	mCommands.clear();
}

void GeometryBatch::GetAttributes(std::vector<Attribute>& attributes) const
{
	for (Attribute attribute : mAttributes)
	{
		if(attribute.usage & Attribute::Usage::RENDER)
			attributes.push_back(attribute);
	}
}

#define ALIGNED_STRUCT_SIZE(STRUCT_SIZE, ALIGNMENT) ((STRUCT_SIZE + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
bool GeometryBatch::EditMaterial(const MeshRendererComponent* cMesh)
{
	const ResourceMaterial* rMat = cMesh->GetResourceMaterial();
	unsigned int offset = 0;
	int idx = 0;
	unsigned int materialSize = ALIGNED_STRUCT_SIZE(sizeof(Material), sizeof(float)*3);
	for (BatchMaterialResource bRMaterial : mUniqueMaterials)
	{
		if (rMat->GetUID() == bRMaterial.resource->GetUID())
		{
			break;
		}
		++idx;
		offset += materialSize;
	}
	if (mUniqueMaterials.size() == 0 || idx == mUniqueMaterials.size())
		return false;

	mMaterialFlag = true;

	return true;
}

void GeometryBatch::RecreatePersistentSsbos()
{
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glDeleteBuffers(1, &mSsboModelMatrices);
	glGenBuffers(1, &mSsboModelMatrices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModelMatrices);
	unsigned int size = mMeshComponents.size() * ALIGNED_STRUCT_SIZE(sizeof(float) * 16, mSsboAligment);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
	mSsboModelMatricesData[0] = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
	for (int i = 1; i < NUM_BUFFERS; ++i)
	{
		mSsboModelMatricesData[i] = mSsboModelMatricesData[0] + ((size * i) / sizeof(float));
	}

	glDeleteBuffers(1, &mSsboIndices);
	glGenBuffers(1, &mSsboIndices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboIndices);
	size = mMeshComponents.size() * ALIGNED_STRUCT_SIZE(sizeof(BufferIndices), mSsboAligment);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
	mSsboIndicesData[0] = reinterpret_cast<uint32_t*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
	for (int i = 1; i < NUM_BUFFERS; ++i)
	{
		mSsboIndicesData[i] = mSsboIndicesData[0] + ((size * i) / sizeof(uint32_t));
	}

	mPersistentsFlag = false;
}

void GeometryBatch::RecreateVboAndEbo()
{
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mVboDataSize, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboDataSize, nullptr, GL_STATIC_DRAW);
	unsigned int vboOffset = 0;
	unsigned int eboOffset = 0;
	for (const BatchMeshResource& res : mUniqueMeshes)
	{
		unsigned int size = res.resource->GetNumberVertices() * res.resource->GetVertexSize(Attribute::Usage::RENDER);
		float* data = res.resource->GetInterleavedData(Attribute::Usage::RENDER);
		glBufferSubData(GL_ARRAY_BUFFER, vboOffset, size, data);
		vboOffset += size;
		delete[] data;
		size = res.resource->GetNumberIndices() * sizeof(unsigned int);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, eboOffset, size, res.resource->GetIndices());
		eboOffset += size;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	mVBOFlag = false;
}

void GeometryBatch::RecreateMaterials()
{
	unsigned int materialSize = ALIGNED_STRUCT_SIZE(sizeof(Material), sizeof(float)*3);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mUniqueMaterials.size() * materialSize, nullptr, GL_STATIC_DRAW);
	unsigned int offset = 0;
	for (const BatchMaterialResource rMaterial : mUniqueMaterials) {
		Material material;
		memcpy(material.baseColor, rMaterial.resource->GetBaseColorFactor().ptr(), sizeof(float) * 3);
		memcpy(material.emissiveFactor, rMaterial.resource->GetEmissiveFactor().ptr(), sizeof(float) * 3);
		material.baseColorTex = (rMaterial.resource->GetBaseColorTexture()) ? rMaterial.resource->GetBaseColorTexture()->GetTextureHandle() : 0;
		material.metalRoughTex = (rMaterial.resource->GetMetallicRoughnessTexture()) ? rMaterial.resource->GetMetallicRoughnessTexture()->GetTextureHandle() : 0;
		material.normalTex = (rMaterial.resource->GetNormalTexture()) ? rMaterial.resource->GetNormalTexture()->GetTextureHandle() : 0;
		material.emissiveTex = (rMaterial.resource->GetEmissiveTexture()) ? rMaterial.resource->GetEmissiveTexture()->GetTextureHandle() : 0;
		material.hasMetalRoughTex = rMaterial.resource->IsMetallicRoughnessEnabled();
		material.hasBaseColorTex = rMaterial.resource->IsBaseColorEnabled();
		material.hasNormalMap = rMaterial.resource->IsNormalMapEnabled();
		material.hasEmissiveTex = rMaterial.resource->IsEmissiveEnabled();
		material.metalness = rMaterial.resource->GetMetallicFactor();
		material.roughness = rMaterial.resource->GetRoughnessFactor();

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), &material);
		offset += materialSize;
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	mMaterialFlag = false;
}


void GeometryBatch::AddUniqueMesh(const MeshRendererComponent* cMesh, unsigned int& meshIdx)
{
	const ResourceMesh& rMesh = *cMesh->GetResourceMesh();
	mUniqueMeshes.emplace_back(&rMesh, mEboDataSize / sizeof(unsigned int), mVboDataSize / rMesh.GetVertexSize(Attribute::Usage::RENDER));
	meshIdx = mUniqueMeshes.size() - 1;
	mVBOFlag = true;
	mVboDataSize += rMesh.GetNumberVertices() * rMesh.GetVertexSize(Attribute::Usage::RENDER);
	mEboDataSize += rMesh.GetNumberIndices() * sizeof(unsigned int);
}

void GeometryBatch::AddMeshComponent(const MeshRendererComponent* cMesh)
{
	bool foundMaterial = false;
	uint32_t matIdx = 0;
	const ResourceMaterial& resourceMaterial = *cMesh->GetResourceMaterial();
	for (const BatchMaterialResource material : mUniqueMaterials) {
		if (material.resource->GetUID() == resourceMaterial.GetUID()) {
			foundMaterial = true;
			++mUniqueMaterials[matIdx].referenceCount;
			break;
		}
		++matIdx;
	}
	if (!foundMaterial)
	{
        mUniqueMaterials.emplace_back(&resourceMaterial);
		matIdx = mUniqueMaterials.size() - 1;
		if (resourceMaterial.GetBaseColorTexture())
		{
			const_cast<ResourceTexture*>(resourceMaterial.GetBaseColorTexture())->GenerateMipmaps();
			const_cast<ResourceTexture*>(resourceMaterial.GetBaseColorTexture())->MakeTextutureBindless();
		}
		if (resourceMaterial.GetEmissiveTexture())
		{
			const_cast<ResourceTexture*>(resourceMaterial.GetEmissiveTexture())->GenerateMipmaps();
			const_cast<ResourceTexture*>(resourceMaterial.GetEmissiveTexture())->MakeTextutureBindless();
		}
		if (resourceMaterial.GetMetallicRoughnessTexture())
		{
			const_cast<ResourceTexture*>(resourceMaterial.GetMetallicRoughnessTexture())->GenerateMipmaps();
			const_cast<ResourceTexture*>(resourceMaterial.GetMetallicRoughnessTexture())->MakeTextutureBindless();
		}
		if (resourceMaterial.GetNormalTexture())
		{
			const_cast<ResourceTexture*>(resourceMaterial.GetNormalTexture())->GenerateMipmaps();
			const_cast<ResourceTexture*>(resourceMaterial.GetNormalTexture())->MakeTextutureBindless();
		}
		mMaterialFlag = true;  
	}

	unsigned int meshIdx = 0;
	//const AnimationComponent* cAnim = cMesh->GetOwner()->FindAnimationComponent();
	
	const ResourceMesh& rMesh = *cMesh->GetResourceMesh();
	
	if (rMesh.HasAttribute(Attribute::JOINT) && rMesh.HasAttribute(Attribute::WEIGHT))
	{
		AddUniqueMesh(cMesh, meshIdx);
	}
	else
	{
		bool foundMesh = false;
		for (; meshIdx < mUniqueMeshes.size(); ++meshIdx)
		{
			if (mUniqueMeshes[meshIdx].resource->GetUID() == rMesh.GetUID())
			{
				foundMesh = true;
				++mUniqueMeshes[meshIdx].referenceCount;
				break;
			}
		}
		if (!foundMesh)
		{
			AddUniqueMesh(cMesh, meshIdx);
		}
	}
	mMeshComponents[cMesh->GetID()] = {cMesh, meshIdx, matIdx};
	mPersistentsFlag = true;
}

bool GeometryBatch::RemoveMeshComponent(const MeshRendererComponent* component)
{
	unsigned int bMeshIdx = 0;
	unsigned int bMaterialIdx = 0;
	bool found = false;
	auto mapIterator = mMeshComponents.find(component->GetID());
	if (mapIterator != mMeshComponents.end())
	{
		found = true;
		bMeshIdx = mapIterator->second.bMeshIdx;
		bMaterialIdx = mapIterator->second.bMaterialIdx;
		mMeshComponents.erase(mapIterator);
	}
	if (!found)
		return false;

	const ResourceMesh& rMesh = *component->GetResourceMesh();
	if (!--mUniqueMeshes[bMeshIdx].referenceCount)
	{
		std::vector<BatchMeshResource>::iterator its = mUniqueMeshes.erase(mUniqueMeshes.begin() + bMeshIdx);
		for (std::vector<BatchMeshResource>::iterator it = its; it != mUniqueMeshes.end(); ++it)
		{
			it->baseVertex -= rMesh.GetNumberVertices();
			it->firstIndex -= rMesh.GetNumberIndices();
		}
		for (auto it = mMeshComponents.begin(); it != mMeshComponents.end(); ++it)
		{
			if (it->second.bMeshIdx > bMeshIdx)
				--(it->second.bMeshIdx);
		}
		mVboDataSize -= rMesh.GetNumberVertices() * rMesh.GetVertexSize(Attribute::Usage::RENDER);
		mEboDataSize -= rMesh.GetNumberIndices() * sizeof(unsigned int);
		mVBOFlag = true;
	}
	if (!--mUniqueMaterials[bMaterialIdx].referenceCount)
	{
		mUniqueMaterials.erase(mUniqueMaterials.begin() + bMaterialIdx);
		mMaterialFlag = true;
		for (auto it = mMeshComponents.begin(); it != mMeshComponents.end(); ++it)
		{
			if (it->second.bMaterialIdx > bMaterialIdx)
			{
				--(it->second.bMaterialIdx);
			}
		}
	}
	
	mPersistentsFlag = true;

	return true;
}

bool GeometryBatch::AddToDraw(const MeshRendererComponent* component)
{
	if (mMeshComponents.find(component->GetID()) == mMeshComponents.end())
	{
		return false;
	}
	if (mComandsMap.find(component->GetID()) != mComandsMap.end())
	{
		mCommands.emplace_back(mComandsMap[component->GetID()]);
		mIboFlag = true;
		return true;
	}

	if (mVBOFlag)
		RecreateVboAndEbo();
	if (mMaterialFlag)
		RecreateMaterials();
	if (mPersistentsFlag)
		RecreatePersistentSsbos();
	
	ComputeSkinning(component);

	unsigned int idx = mDrawCount % NUM_BUFFERS;

	const BatchMeshRendererComponent& batchMeshRenderer = mMeshComponents[component->GetID()];
	if (batchMeshRenderer.IsAnimated())
	{
		memcpy(mSsboModelMatricesData[idx] + 16 * mCommands.size(), float4x4::identity.ptr(), sizeof(float) * 16);
	}
	else
	{
		memcpy(mSsboModelMatricesData[idx] + 16 * mCommands.size(), component->GetOwner()->GetWorldTransform().ptr(), sizeof(float) * 16);
	}

	memcpy(mSsboIndicesData[idx] + mCommands.size(), &batchMeshRenderer.bMaterialIdx, sizeof(uint32_t));
	
	mCommands.emplace_back(component->GetResourceMesh()->GetNumberIndices(), 1, mUniqueMeshes[batchMeshRenderer.bMeshIdx].firstIndex, mUniqueMeshes[batchMeshRenderer.bMeshIdx].baseVertex, mCommands.size());
	mComandsMap[component->GetID()] = mCommands.back();
	mIboFlag = true;
	return true;
}

void GeometryBatch::Draw()
{
	if (mMeshComponents.size() == 0 || mCommands.size() == 0)
		return;

	if (mAnimationSkinning)
	{
		glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
		mAnimationSkinning = false;
	}

	//Wait for GPU
	unsigned int idx = mDrawCount % NUM_BUFFERS;
	if (mSync[idx])
	{
		GLenum waitReturn = GL_UNSIGNALED;
		while (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED)
		{
			waitReturn = glClientWaitSync(mSync[idx], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
		}
	}

	glBindVertexArray(mVao);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	if (mIboFlag)
	{
		glBufferData(GL_DRAW_INDIRECT_BUFFER, mCommands.size() * sizeof(Command), mCommands.data(), GL_STATIC_DRAW);
		mIboFlag = false;
	}

	unsigned int structSize = ALIGNED_STRUCT_SIZE(sizeof(float) * 16, mSsboAligment);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 10, mSsboModelMatrices, idx * mMeshComponents.size() * structSize, mMeshComponents.size() * structSize);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);
	structSize = ALIGNED_STRUCT_SIZE(sizeof(BufferIndices), mSsboAligment);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 12, mSsboIndices, idx * mMeshComponents.size() * structSize, mMeshComponents.size() * structSize);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, mCommands.size(), 0);

	//CleanUp
	glBindVertexArray(0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glUseProgram(0);
}

void GeometryBatch::CleanUpCommands()
{
	mCommands.clear();
}

void GeometryBatch::EndFrameDraw()
{
	CleanUpCommands();
	mComandsMap.clear();
	unsigned int idx = mDrawCount % NUM_BUFFERS;
	glDeleteSync(mSync[idx]);
	mSync[idx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	++mDrawCount;
}

void GeometryBatch::ComputeSkinning(const MeshRendererComponent* cMesh)
{
	BatchMeshRendererComponent& batchMeshRenderer = mMeshComponents[cMesh->GetID()];
	const MeshRendererComponent* meshRenderer = batchMeshRenderer.component;
	const ResourceMesh* rMesh = meshRenderer->GetResourceMesh();
	if (batchMeshRenderer.IsAnimated())
	{
		glUseProgram(App->GetOpenGL()->GetSkinningProgramId());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPaletteSsbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, batchMeshRenderer.component->GetPalette().size() * sizeof(float) * 16, batchMeshRenderer.component->GetPalette().data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mAnimSsbo);
		float* data = rMesh->GetInterleavedData(Attribute::Usage::ANIMATION);
		glBufferData(GL_SHADER_STORAGE_BUFFER, rMesh->GetNumberVertices() * rMesh->GetVertexSize(Attribute::Usage::ANIMATION), data, GL_STREAM_DRAW);
		delete[] data;
		unsigned int vertexSize = rMesh->GetVertexSize(Attribute::Usage::RENDER);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 21, mVbo, mUniqueMeshes[batchMeshRenderer.bMeshIdx].baseVertex * vertexSize, vertexSize * rMesh->GetNumberVertices());
		glUniform1i(25, rMesh->GetNumberVertices());
		//glUniform1i(26, batchMeshRenderer.bCAnim->GetIsPlaying());
		glDispatchCompute((rMesh->GetNumberVertices() + 63) / 64, 1, 1);
		mAnimationSkinning = true;
	}
	glUseProgram(0);
}

bool BatchMeshRendererComponent::IsAnimated() const
{
	assert(component != nullptr);
#ifdef _DEBUG
	if (component->GetPalette().size() != 0)
		assert(component->GetResourceMesh()->HasAttribute(Attribute::JOINT) && component->GetResourceMesh()->HasAttribute(Attribute::WEIGHT) && "Animated mesh does not have weights or joints");
#endif // _DEBUG
	return component->GetPalette().size() != 0;
}