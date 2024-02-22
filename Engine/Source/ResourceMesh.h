#pragma once

#include <vector>

#include "ImporterMesh.h"
#include "Resource.h"

typedef struct Attribute {
	enum Type : unsigned char {
		POS = 1 << 0,
		UV = 1 << 1,
		NORMAL = 1 << 2,
		TANGENT = 1 << 3,
		COLOR = 1 << 4,
	};
	Attribute(Type iType, unsigned int iSize, unsigned int iOffset) : size(iSize), offset(iOffset), type(iType) {}
	Attribute(const Attribute& other) : size(other.size), offset(other.offset), type(other.type) {}
	unsigned int size;
	unsigned int offset;
	Type type;
}Attribute;

float* GetAttributeDataFromInterleavedBuffer(Attribute attr, float* interleavedBuffer, unsigned int bufferSize, unsigned int vertexSize);


class ResourceMesh : public Resource
{
public:

	ResourceMesh(unsigned int uid,
		const char* path,
		unsigned int numIndices,
		unsigned int numVertices);

	//ResourceMesh(const ResourceMesh& other);
	~ResourceMesh() { CleanUp(); }

	unsigned int GetNumberVertices() const { return mNumVertices; }
	unsigned int GetNumberIndices() const { return mNumIndices; }

	const void SetNumberVertices(unsigned int numVertices) { mNumVertices = numVertices; }
	const void SetNumberIndices(unsigned int numIndices) { mNumIndices = numIndices; }

	void AddIndices(unsigned int* indices);
	unsigned int* GetIndices() const { return mIndices; }

	unsigned int GetVao() const { return mVao; }

	const std::vector<Attribute*>& GetAttributes() const { return mAttributes; }
	unsigned int GetVertexSize() const { return mVertexSize; }

	const float* GetAttributeData(Attribute::Type type) const;
	int GetAttributeIdx(Attribute::Type type) const;
	void AddAttribute(const Attribute& attribute, float* attributeData);
	bool LoadInterleavedAttribute(float* fillBuffer, const Attribute& attribute, unsigned int vertexSize) const;
	float* GetInterleavedData() const;
	void LoadToMemory();


	void GenerateTangents();

	void CleanUp();

	//TODO Make it Private
	std::vector<float*> mAttributesData;
	unsigned int* mIndices = nullptr;

private:

	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;


	unsigned int mVao = 0;
	unsigned int mVbo = 0;
	unsigned int mEbo = 0;

	std::vector<Attribute*> mAttributes;
	unsigned int mVertexSize = 0;
};

