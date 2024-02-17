#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"
#include <vector>


struct ResourceMesh;
class GeometryBatch;

namespace Importer
{
	namespace Mesh
	{
		void Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* ourMesh);

		void Save(const ResourceMesh* ourMesh);

		void Load(ResourceMesh* ourMesh, const char* fileName);
	}
}

typedef struct Attribute {
	enum Type : unsigned char {
		POS = 1 << 0,
		UV = 1 << 1,
		NORMAL = 1 << 2,
		TANGENT = 1 << 3,
		COLOR = 1 << 4,
	};
	Attribute(Type iType, unsigned int iSize, unsigned int iOffset) : size(iSize), offset(iOffset), type(iType) {}
	Attribute(const Attribute& other): size(other.size), offset(other.offset), type(other.type) {}
	unsigned int size;
	unsigned int offset;
	Type type;
}Attribute;

float* GetAttributeDataFromInterleavedBuffer(Attribute attr, float* interleavedBuffer, unsigned int bufferSize, unsigned int vertexSize);
struct ResourceMesh
{
	//ResourceMesh(const ResourceMesh& other);
	~ResourceMesh() { CleanUp(); }
	//TODO: Posar tot aixo privat !!
	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;

	unsigned int* mIndices = nullptr;
	std::vector<float*> mAttributesData;


	const char* mMeshName = nullptr;
	unsigned int mUID = 0;

	void GenerateTangents();
	float* GetInterleavedData() const;
	bool LoadInterleavedAttribute(float* fillBuffer, const Attribute& attribute, unsigned int vertexSize) const;
	int GetAttributeIdx(Attribute::Type type) const;
	void AddAttribute(const Attribute& attribute, float* attributeData);
	const float* GetAttributeData(Attribute::Type type) const;
	bool HasAttribute(Attribute::Type type) const;
	unsigned int NumAttributes() const { return mAttributes.size(); }
	const std::vector<Attribute*>& GetAttributes() const { return mAttributes; };
	unsigned int GetVertexSize() const { return mVertexSize; }
	unsigned int GetNumVertices() const { return mNumVertices; }
	unsigned int GetNumIndices() const { return mNumIndices; }
	unsigned int* GetIndices() const { return mIndices; }

	unsigned int GetVboPosition() const { return mVboPosition; }
	unsigned int GetEboPosition() const { return mEboPosition; }
	void SetVboPosition(unsigned int VboPos) { mVboPosition = VboPos; }
	void SetEboPosition(unsigned int EboPos) { mEboPosition = EboPos; }
	unsigned int GetVao() const { return mVao; }
	const unsigned int GetNumIndices() { return mNumIndices; }


	void CleanUp();

private:

	unsigned int mVboPosition = 0;
	unsigned int mEboPosition = 0;
	std::vector<Attribute*> mAttributes;
	unsigned int mVertexSize = 0;

	//TODO: Feo, provar de treure
	friend void Importer::Mesh::Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* mesh);
	friend void Importer::Mesh::Load(ResourceMesh* mesh, const char* fileName);
};
