#ifndef _RESOURCE_MESH_H_
#define _RESOURCE_MESH_H_

#include <vector>
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

class ResourceMesh : public Resource
{
public:

	ResourceMesh(unsigned int uid, unsigned int numIndices, const unsigned int* indices, unsigned int numJoints, const unsigned int* joints, unsigned int numWeights, const float* weights, unsigned int numVertices, const std::vector<Attribute>& attributes, const std::vector<float*>& attributesData);
	ResourceMesh(unsigned int uid, unsigned int numIndices, unsigned int*&& indices, unsigned int numJoints, unsigned int*&& joints, unsigned int numWeights, float*&&weights, unsigned int numVertices, std::vector<Attribute>&& attributes, std::vector<float*>&& attributesData);
	ResourceMesh(const ResourceMesh& other);
	ResourceMesh(ResourceMesh&& other);
	~ResourceMesh();

	unsigned int GetNumberVertices() const { return mNumVertices; }
	unsigned int GetNumberIndices() const { return mNumIndices; }
	unsigned int GetNumberJoints() const { return mNumJoints; }
	unsigned int GetNumberWeights() const { return mNumWeights; }
	const unsigned int* GetIndices() const { return mIndices; }
	const unsigned int* GetJoints() const { return mJoints; }
	const float* GetWeights() const { return mWeights; }
	void GetAttributes(std::vector<Attribute>&) const;
	unsigned int GetNumberAttributes() const { return static_cast<unsigned int>(mAttributes.size()); }
	bool HasAttribute(Attribute::Type type) const;
	unsigned int GetVertexSize() const { return mVertexSize; }
	const float* GetAttributeData(Attribute::Type type) const;
	int GetAttributeIdx(Attribute::Type type) const;
	//This allocates memory in the return pointer that you must delete
	float* GetInterleavedData() const;

private:
	unsigned int* mIndices = nullptr;
	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;

	unsigned int mNumJoints = 0;
	unsigned int* mJoints = nullptr;
	unsigned int mNumWeights = 0;
	float* mWeights = nullptr;

	std::vector<float*> mAttributesData;
	std::vector<Attribute> mAttributes;
	unsigned int mVertexSize = 0;
};

#endif // _RESOURCE_MESH_H_