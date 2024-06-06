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

GeometryBatch::GeometryBatch(const MeshRendererComponent& cMesh)
{
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &mSsboAligment);

	for (int i = 0; i < NUM_BUFFERS; ++i)
	{
		mSsboModelMatricesData[i] = nullptr;
		mSsboIndicesData[i] = nullptr;
	}
	memset(mSync, 0, sizeof(mSync)*NUM_BUFFERS);

	cMesh.GetResourceMesh()->GetAttributes(mAttributes, Attribute::Usage::RENDER);
	mVertexSize = cMesh.GetResourceMesh()->GetVertexSize(Attribute::Usage::RENDER);

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
	glGenBuffers(1, &mSkinSsbo);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 15, mPaletteSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 20, mSkinSsbo);

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
	glDeleteBuffers(1, &mSkinSsbo);
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
bool GeometryBatch::EditMaterial(const MeshRendererComponent& cMesh)
{
	const ResourceMaterial* rMat = cMesh.GetResourceMaterial();
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

void GeometryBatch::RecreateSkinningSsbos()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSkinSsbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mSkinBufferSize, nullptr, GL_STATIC_DRAW);
	char* memory = reinterpret_cast<char*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));
	unsigned int offset = 0;
	for (BatchMeshResource& mesh : mUniqueMeshes)
	{
		if (mesh.HasSkinning())
		{
			unsigned int size;
			mesh.resource->GetInterleavedData(Attribute::Usage::ANIMATION, reinterpret_cast<float*>(memory + offset), &size);
			mesh.skinOffset = offset / mesh.resource->GetVertexSize(Attribute::Usage::ANIMATION);
			offset += size;
		}
	}
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	mSkinningFlag = false;
}

void GeometryBatch::RecreateVboAndEbo()
{
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mVboDataSize, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboDataSize, nullptr, GL_STATIC_DRAW);
	char* vboBuffer = reinterpret_cast<char*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	unsigned int vboOffset = 0;
	unsigned int eboOffset = 0;
	for (int i = 0; i < mUniqueMeshes.size(); ++i)
	{
		BatchMeshResource& res = mUniqueMeshes[i];
		res.baseVertex = vboOffset / mVertexSize;
		res.firstIndex = eboOffset / sizeof(unsigned int);
		res.baseInstance = i;
		unsigned int size;
		res.resource->GetInterleavedData(Attribute::Usage::RENDER, reinterpret_cast<float*>(vboBuffer + vboOffset), &size);
		vboOffset += size;
		size = res.resource->GetNumberIndices() * sizeof(unsigned int);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, eboOffset, size, res.resource->GetIndices());
		eboOffset += size;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
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


void GeometryBatch::AddUniqueMesh(const MeshRendererComponent& cMesh, unsigned int& meshIdx)
{
	meshIdx = mUniqueMeshes.size();
	const ResourceMesh& rMesh = *cMesh.GetResourceMesh();
	mUniqueMeshes.emplace_back(&rMesh, 0, 0);
	mVBOFlag = true;
	mVboDataSize += rMesh.GetNumberVertices() * mVertexSize;
	mEboDataSize += rMesh.GetNumberIndices() * sizeof(unsigned int);
}

void GeometryBatch::AddMeshComponent(const MeshRendererComponent& cMesh)
{
	bool foundMaterial = false;
	uint32_t matIdx = 0;
	const ResourceMaterial& resourceMaterial = *cMesh.GetResourceMaterial();
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
			resourceMaterial.GetBaseColorTexture()->GenerateMipmaps();
			resourceMaterial.GetBaseColorTexture()->MakeTextutureBindless();
		}
		if (resourceMaterial.GetEmissiveTexture())
		{
			resourceMaterial.GetEmissiveTexture()->GenerateMipmaps();
			resourceMaterial.GetEmissiveTexture()->MakeTextutureBindless();
		}
		if (resourceMaterial.GetMetallicRoughnessTexture())
		{
			resourceMaterial.GetMetallicRoughnessTexture()->GenerateMipmaps();
			resourceMaterial.GetMetallicRoughnessTexture()->MakeTextutureBindless();
		}
		if (resourceMaterial.GetNormalTexture())
		{
			resourceMaterial.GetNormalTexture()->GenerateMipmaps();
			resourceMaterial.GetNormalTexture()->MakeTextutureBindless();
		}
		mMaterialFlag = true;  
	}

	unsigned int meshIdx = 0;
	//const AnimationComponent* cAnim = cMesh->GetOwner()->FindAnimationComponent();
	
	const ResourceMesh& rMesh = *cMesh.GetResourceMesh();
	
	if (rMesh.HasAttribute(Attribute::JOINT) && rMesh.HasAttribute(Attribute::WEIGHT))
	{
		AddUniqueMesh(cMesh, meshIdx);
		mUniqueMeshes.back().skinOffset = mSkinBufferSize;
		mSkinBufferSize += rMesh.GetVertexSize(Attribute::Usage::ANIMATION) * rMesh.GetNumberVertices();
		mSkinningFlag = true;
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
	mMeshComponents[cMesh.GetID()] = { meshIdx, matIdx };
	mPersistentsFlag = true;
}

bool GeometryBatch::RemoveMeshComponent(const MeshRendererComponent& component)
{
	unsigned int bMeshIdx = 0;
	unsigned int bMaterialIdx = 0;
	bool found = false;
	auto mapIterator = mMeshComponents.find(component.GetID());
	if (mapIterator != mMeshComponents.end())
	{
		found = true;
		bMeshIdx = mapIterator->second.bMeshIdx;
		bMaterialIdx = mapIterator->second.bMaterialIdx;
		if (component.IsAnimated())
			mSkinningFlag = true;
		mMeshComponents.erase(mapIterator);
	}
	if (!found)
		return false;

	const ResourceMesh& rMesh = *component.GetResourceMesh();
	if (!--mUniqueMeshes[bMeshIdx].referenceCount)
	{
		if (mUniqueMeshes[bMeshIdx].HasSkinning())
		{
			mSkinBufferSize -= rMesh.GetNumberVertices() * rMesh.GetVertexSize(Attribute::Usage::ANIMATION);
			mSkinningFlag = true;
		}
		mUniqueMeshes.erase(mUniqueMeshes.begin() + bMeshIdx);
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

bool GeometryBatch::AddToDraw(const MeshRendererComponent& component)
{
	if (mMeshComponents.find(component.GetID()) == mMeshComponents.end())
	{
		return false;
	}
	if (mComandsMap.find(component.GetID()) != mComandsMap.end())
	{
		mCommands.emplace_back(mComandsMap[component.GetID()]);
		mIboFlag = true;
		return true;
	}

	if (mVBOFlag)
		RecreateVboAndEbo();
	if (mMaterialFlag)
		RecreateMaterials();
	if (mPersistentsFlag)
		RecreatePersistentSsbos();
	if (mSkinningFlag)
		RecreateSkinningSsbos();
	
	ComputeSkinning(component);

	unsigned int idx = mDrawCount % NUM_BUFFERS;

	const BatchMeshRendererComponent& batchMeshRenderer = mMeshComponents[component.GetID()];
	const BatchMeshResource& bRes = mUniqueMeshes[batchMeshRenderer.bMeshIdx];
	if (component.IsAnimated())
	{
		memcpy(mSsboModelMatricesData[idx] + 16 * bRes.baseInstance, float4x4::identity.ptr(), sizeof(float) * 16);
	}
	else
	{
		memcpy(mSsboModelMatricesData[idx] + 16 * bRes.baseInstance, component.GetOwner()->GetWorldTransform().ptr(), sizeof(float) * 16);
	}

	memcpy(mSsboIndicesData[idx] + bRes.baseInstance, &batchMeshRenderer.bMaterialIdx, sizeof(uint32_t));
	
	mCommands.emplace_back(component.GetResourceMesh()->GetNumberIndices(), 1, bRes.firstIndex, bRes.baseVertex, bRes.baseInstance);
	mComandsMap[component.GetID()] = mCommands.back();
	mIboFlag = true;
	return true;
}

void GeometryBatch::Draw()
{
	if (mMeshComponents.size() == 0 || mCommands.size() == 0)
		return;

	if (mSkinningApplied)
	{
		glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
		mSkinningApplied = false;
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

void GeometryBatch::ComputeSkinning(const MeshRendererComponent& cMesh)
{
	BatchMeshRendererComponent& batchMeshRenderer = mMeshComponents[cMesh.GetID()];
	const ResourceMesh* rMesh = cMesh.GetResourceMesh();
	const unsigned int idx = mDrawCount % NUM_BUFFERS;
	if (cMesh.IsAnimated())
	{
		glUseProgram(App->GetOpenGL()->GetSkinningProgramId());
		const BatchMeshResource& bRes = mUniqueMeshes[batchMeshRenderer.bMeshIdx];
		//TODO: El buffer range de los vertices dskin entre 2 batches no funcionara
		assert(bRes.skinOffset != -1 && "Skin mesh does not have the offset set");
		glUniform1ui(0, bRes.skinOffset);

		const std::vector<float4x4>& palette = cMesh.GetPalette();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPaletteSsbo);
		if (palette.size() > mBiggestPaletteSize)
		{
			glBufferData(GL_SHADER_STORAGE_BUFFER, palette.size() * sizeof(float) * 16, palette.data(), GL_DYNAMIC_DRAW);
			mBiggestPaletteSize = palette.size();
		}
		else
		{
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, palette.size() * sizeof(float) * 16, palette.data());
		}
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 21, mVbo, bRes.baseVertex * mVertexSize, mVertexSize * rMesh->GetNumberVertices());
		glUniform1i(25, rMesh->GetNumberVertices());
		glDispatchCompute((rMesh->GetNumberVertices() + 63) / 64, 1, 1);
		mSkinningApplied = true;
	}
	glUseProgram(0);
}