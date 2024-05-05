#include "Globals.h"

#include "SaveLoadMesh.h"
#include "ImporterMesh.h"
#include "ResourceMesh.h"
#include "glew.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"



static float* GetInterleavedData(const std::vector<Attribute>& attributes, const std::vector<float*>& attributesData, unsigned int numVertices)
{
    unsigned int vertexSize = 0;
    for (int i = 0; i < attributes.size(); ++i)
    {
        vertexSize += attributes[i].size;
    }
    float* ret = new float[numVertices * vertexSize / sizeof(float)];
    unsigned int vertexFloats = vertexSize / sizeof(float);
    for (unsigned int vertexIdx = 0; vertexIdx < numVertices; ++vertexIdx)
    {
        for (unsigned int attributeIdx = 0; attributeIdx < attributes.size(); ++attributeIdx)
        {
            unsigned int attributeSize = attributes[attributeIdx].size / sizeof(float);
            unsigned int attributeOffset = attributes[attributeIdx].offset / sizeof(float);
            const float* attributeData = attributesData[attributeIdx];
            memcpy(&ret[vertexIdx * vertexFloats + attributeOffset], &attributeData[vertexIdx * attributeSize], attributeSize * sizeof(float));
        }
    }
    return ret;
}

static float* GetAttributeDataFromInterleavedBuffer(Attribute attr, float* interleavedBuffer, unsigned int bufferSize, unsigned int vertexSize)
{
    unsigned int numVertices = (bufferSize / vertexSize);
    unsigned int attributeNumFloats = (attr.size / sizeof(float));
    float* ret = new float[numVertices * attributeNumFloats];
    unsigned int floatOffset = attr.offset / sizeof(float);
    unsigned int j = 0;
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        const float* vert = &reinterpret_cast<const float*>(interleavedBuffer)[i * vertexSize / sizeof(float) + floatOffset];
        for (unsigned int j = 0; j < attributeNumFloats; ++j)
        {
            ret[i * attributeNumFloats + j] = vert[j];
        }

    }
    return ret;
}

#include "mikktspace.h"
#include "weldmesh.h"

typedef struct {
    int numVertices;
    int posOffset;
    int texCoordOffset;
    int normOffset;
    int vertexSize;
    char* vertices;
    char* tVertices;
} MikkTSpaceStruct;

static int GetNumFaces(const SMikkTSpaceContext* pContext)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    return ptr->numVertices / 3;
}
static int GetNumVerticesOfFace(const SMikkTSpaceContext* pContext, const int iFace) {
    return 3;
}
static void GetPosition(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    float* posOut = (float*)&ptr->vertices[(iFace * 3 + iVert) * ptr->vertexSize + ptr->posOffset];
    fvPosOut[0] = posOut[0];
    fvPosOut[1] = posOut[1];
    fvPosOut[2] = posOut[2];
}
static void GetTexCoord(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    float* texCOut = (float*)&ptr->vertices[(iFace * 3 + iVert) * ptr->vertexSize + ptr->texCoordOffset];
    fvTexcOut[0] = texCOut[0];
    fvTexcOut[1] = texCOut[1];
}
static void GetNormal(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    float* normalOut = (float*)&ptr->vertices[(iFace * 3 + iVert) * ptr->vertexSize + ptr->normOffset];
    fvNormOut[0] = normalOut[0];
    fvNormOut[1] = normalOut[1];
    fvNormOut[2] = normalOut[2];
}

static void SetTSpaceBasic(const SMikkTSpaceContext* pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert)
{
    MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
    //Escriure tota la info del vertex + les tangents
    const unsigned int vertexIdx = (iFace * 3 + iVert) * ptr->vertexSize;
    const unsigned int vertexTIdx = (iFace * 3 + iVert) * (ptr->vertexSize + sizeof(float) * 4);
    memcpy(&ptr->tVertices[vertexTIdx], &ptr->vertices[vertexIdx], ptr->vertexSize);
    memcpy(&ptr->tVertices[vertexTIdx + ptr->vertexSize], fvTangent, 3 * sizeof(float));
    memcpy(&ptr->tVertices[vertexTIdx + ptr->vertexSize + 3 * sizeof(float)], &fSign, sizeof(float));
}

static void GenerateTangents(std::vector<Attribute>& attributes, std::vector<float*>& attributeData, unsigned int& numIndices, unsigned int*& indexData, unsigned int& vertexSize, unsigned int& numVertices, float* vertexData)
{
#ifdef _DEBUG
    bool foundPos = false;
    bool foundUv = false;
    bool foundNormal = false;
    for (const Attribute attribute : attributes)
    {
        switch (attribute.type)
        {
        case Attribute::POS:
            foundPos = true;
            break;
        case Attribute::UV:
            foundUv = true;
            break;
        case Attribute::NORMAL:
            foundNormal = true;
            break;
        }
        if (foundPos && foundUv && foundNormal)
            break;
    }
    if (!foundPos || !foundUv || !foundNormal)
        return;
#endif // _DEBUG

    const char* vertices = reinterpret_cast<const char*>(vertexData);
    char* unweldedVertices = new char[numIndices * vertexSize];

    for (unsigned int i = 0; i < numIndices; ++i)
    {
        memcpy(&unweldedVertices[i * vertexSize], &vertices[indexData[i] * vertexSize], vertexSize);
    }

    SMikkTSpaceInterface interfaceInput = {};
    interfaceInput.m_getNumFaces = GetNumFaces;
    interfaceInput.m_getNumVerticesOfFace = GetNumVerticesOfFace;
    interfaceInput.m_getNormal = GetNormal;
    interfaceInput.m_getPosition = GetPosition;
    interfaceInput.m_getTexCoord = GetTexCoord;
    interfaceInput.m_setTSpaceBasic = SetTSpaceBasic;
    MikkTSpaceStruct mikkInput = {};
    mikkInput.numVertices = numIndices;
    mikkInput.posOffset = 0;
    mikkInput.texCoordOffset = 3 * sizeof(float);
    mikkInput.normOffset = 5 * sizeof(float);
    mikkInput.vertexSize = 8 * sizeof(float);
    mikkInput.vertices = unweldedVertices;
    //Les mikktangents son vec4
    char* unweldedTVertices = new char[numIndices * (vertexSize + 4 * sizeof(float))];
    mikkInput.tVertices = unweldedTVertices;
    SMikkTSpaceContext tangContext = {};
    tangContext.m_pInterface = &interfaceInput;
    tangContext.m_pUserData = &mikkInput;
    if (!genTangSpaceDefault(&tangContext))
    {
        LOG("ERROR: Could not generate the tangent space");
        delete[] unweldedTVertices;
        delete[] unweldedVertices;
        return;
    }

    int* piRemapTable = new int[mikkInput.numVertices];
    float* pfVertexDataOut = new float[mikkInput.numVertices * 12];
    unsigned int uniqueVertices = WeldMesh(piRemapTable, pfVertexDataOut, reinterpret_cast<float*>(mikkInput.tVertices), mikkInput.numVertices, 12);
    delete[] unweldedTVertices;
    delete[] unweldedVertices;

    delete[] indexData;
    delete[] vertexData;
    attributes.clear();
    for (float* data : attributeData)
        delete[] data;
    attributeData.clear();

    numVertices = uniqueVertices;
    numIndices = mikkInput.numVertices;
    attributes.emplace_back(Attribute::POS, sizeof(float) * 3, 0);
    attributeData.push_back(GetAttributeDataFromInterleavedBuffer(attributes.back(), pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));
    attributes.emplace_back(Attribute::UV, sizeof(float) * 2, sizeof(float) * 3);
    attributeData.push_back(GetAttributeDataFromInterleavedBuffer(attributes.back(), pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));
    attributes.emplace_back(Attribute::NORMAL, sizeof(float) * 3, sizeof(float) * 5);
    attributeData.push_back(GetAttributeDataFromInterleavedBuffer(attributes.back(), pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));
    attributes.emplace_back(Attribute::TANGENT, sizeof(float) * 4, sizeof(float) * 8);
    attributeData.push_back(GetAttributeDataFromInterleavedBuffer(attributes.back(), pfVertexDataOut, uniqueVertices * 12 * sizeof(float), 12 * sizeof(float)));

    numIndices = mikkInput.numVertices;
    indexData = reinterpret_cast<unsigned int*>(piRemapTable);
    delete[] pfVertexDataOut;
}

ResourceMesh* Importer::Mesh::Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, unsigned int uid)
{
    unsigned int numVertices = 0;
    unsigned int numIndices = 0;
    unsigned int numJoints = 0;
    unsigned int numWeights = 0;
    unsigned int vertexSize = 0;
    unsigned int* indices = nullptr;
    unsigned int* joints = nullptr;
    float* weights = nullptr;
    std::vector<Attribute>attributes;
    std::vector<float*>attributesData;

    const auto& itPos = primitive.attributes.find("POSITION");
    const auto& itTexCoord = primitive.attributes.find("TEXCOORD_0");
    const auto& itNorm = primitive.attributes.find("NORMAL");
    const auto& itTang = primitive.attributes.find("TANGENT");
    const auto& itJoints = primitive.attributes.find("JOINTS_0");
    const auto& itWeights = primitive.attributes.find("WEIGHTS_0");

    if (itPos != primitive.attributes.end())
    {
        const tinygltf::Accessor& posAcc = model.accessors[itPos->second];
        assert(posAcc.type == TINYGLTF_TYPE_VEC3);
        assert(posAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView];
        const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];

        numVertices = posAcc.count;

        const float* bufferPos = reinterpret_cast<const float*>(& posBuffer.data[posView.byteOffset + posAcc.byteOffset]);

        attributes.emplace_back(Attribute::POS, sizeof(float) * 3, vertexSize);
        vertexSize += sizeof(float) * 3;
        float* data = new float[posAcc.count * 3];
        //Add vertices Pos to this buffer taking into acc byteStride
        for (unsigned int i = 0; i < posAcc.count; ++i)
        {
            data[i * 3] = bufferPos[0];
            data[i * 3 + 1] = bufferPos[1];
            data[i * 3 + 2] = bufferPos[2];

            if (posView.byteStride != 0) {
                bufferPos = reinterpret_cast<const float*>(reinterpret_cast<const char*>(bufferPos) + posView.byteStride);
            }
            else {
                bufferPos += 3;
            }
        }
        attributesData.push_back(data);
    }

    if (itTexCoord != primitive.attributes.end())
    {
        const tinygltf::Accessor& texCoordAcc = model.accessors[itTexCoord->second];
        assert(texCoordAcc.type == TINYGLTF_TYPE_VEC2);
        assert(texCoordAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& texCoordView = model.bufferViews[texCoordAcc.bufferView];
        const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordView.buffer];

        const float* bufferTexCoord = reinterpret_cast<const float*>(&texCoordBuffer.data[texCoordView.byteOffset + texCoordAcc.byteOffset]);

        attributes.emplace_back(Attribute::UV, sizeof(float) * 2, vertexSize);
        vertexSize += sizeof(float) * 2;
        float* data = new float[texCoordAcc.count * 2];
        //Add vertices TexCoord to this buffer taking into acc byteStride
        for (unsigned int i = 0; i < texCoordAcc.count; ++i)
        {
            data[i*2] = bufferTexCoord[0];
            data[i*2 + 1] = bufferTexCoord[1];

            if (texCoordView.byteStride != 0)
            {
                bufferTexCoord = reinterpret_cast<const float*>(reinterpret_cast<const char*>(bufferTexCoord) + texCoordView.byteStride);
            }
            else
            {
                bufferTexCoord += 2;
            }
        }
        attributesData.push_back(data);
    }

    if (itNorm != primitive.attributes.end())
    {
        const tinygltf::Accessor& normAcc = model.accessors[itNorm->second];
        assert(normAcc.type == TINYGLTF_TYPE_VEC3);
        assert(normAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& normView = model.bufferViews[normAcc.bufferView];
        const tinygltf::Buffer& normBuffer = model.buffers[normView.buffer];

        const float* bufferNorm = reinterpret_cast<const float*>(& normBuffer.data[normView.byteOffset + normAcc.byteOffset]);

        attributes.emplace_back(Attribute::NORMAL, sizeof(float) * 3, vertexSize);
        vertexSize += sizeof(float) * 3;
        float* data = new float[normAcc.count * 3];

        //Add vertices Normal to this buffer taking into acc byteStride
        for (unsigned int i = 0; i < normAcc.count; ++i)
        {
            data[i*3] = bufferNorm[0];
            data[i*3 + 1] = bufferNorm[1];
            data[i*3 + 2] = bufferNorm[2];

            if (normView.byteStride != 0)
            {
                bufferNorm = reinterpret_cast<const float*>(reinterpret_cast<const char*>(bufferNorm) + normView.byteStride);
            }
            else
            {
                bufferNorm += 3;
            }
        }
        attributesData.push_back(data);
    }

    //Indices part
    if (primitive.indices >= 0)
    {
        const tinygltf::Accessor& indAcc = model.accessors[primitive.indices];
        numIndices = indAcc.count;
        LOG("Tri: %u", numIndices/3);
        const tinygltf::BufferView& indView = model.bufferViews[indAcc.bufferView];
        const unsigned char* buffer = &(model.buffers[indView.buffer].data[indAcc.byteOffset + indView.byteOffset]);

        indices = new unsigned int[numIndices];

        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT)
        {
            const uint32_t* bufferInd = reinterpret_cast<const uint32_t*>(buffer);
            for (unsigned int i = 0; i < indAcc.count; ++i)
            {
                indices[i] = bufferInd[i];
            }
        }
        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
        {
            const uint16_t* bufferInd = reinterpret_cast<const uint16_t*>(buffer);
            for (unsigned int i = 0; i < indAcc.count; ++i)
            {
                indices[i] = bufferInd[i];
            }
        }
        if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
        {
            const uint8_t* bufferInd = reinterpret_cast<const uint8_t*>(buffer);
            for (unsigned int i = 0; i < indAcc.count; ++i)
            {
                indices[i] = bufferInd[i];
            }
        }
    }

    if (itTang != primitive.attributes.end())
    {
        const tinygltf::Accessor& tangAcc = model.accessors[itTang->second];
        assert(tangAcc.type == TINYGLTF_TYPE_VEC4);
        assert(tangAcc.componentType == GL_FLOAT);
        const tinygltf::BufferView& tangView = model.bufferViews[tangAcc.bufferView];
        const tinygltf::Buffer& tangBuffer = model.buffers[tangView.buffer];

        const float* bufferTang = reinterpret_cast<const float*>(&tangBuffer.data[tangView.byteOffset + tangAcc.byteOffset]);

        attributes.emplace_back(Attribute::TANGENT, sizeof(float) * 4, vertexSize);
        vertexSize += sizeof(float) * 4;
        float* data = new float[tangAcc.count * 4];

        //Add vertices Tangent to this buffer taking into acc byteStride
        for (unsigned int i = 0; i < tangAcc.count; ++i)
        {
            data[i * 4] = bufferTang[0];
            data[i * 4 + 1] = bufferTang[1];
            data[i * 4 + 2] = bufferTang[2];
            data[i * 4 + 3] = bufferTang[3];

            if (tangView.byteStride != 0)
            {
                bufferTang = reinterpret_cast<const float*>(reinterpret_cast<const char*>(bufferTang) + tangView.byteStride);
            }
            else
            {
                bufferTang += 4;
            }
        }
        attributesData.push_back(data);
    }
    else
    {
        //Generate Tangents
        float* interleavedVertices = GetInterleavedData(attributes, attributesData, numVertices);
        GenerateTangents(attributes, attributesData, numIndices, indices, vertexSize, numVertices, interleavedVertices);
    }

    if (!model.skins.empty())
    {
        if (itJoints != primitive.attributes.end())
        {
            const tinygltf::Accessor& jointsAcc = model.accessors[itJoints->second];
            assert(jointsAcc.type == TINYGLTF_TYPE_VEC4);
            assert(jointsAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT || jointsAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT || jointsAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE);

            const tinygltf::BufferView& jointsView = model.bufferViews[jointsAcc.bufferView];
            const tinygltf::Buffer& jointsBuffer = model.buffers[jointsView.buffer];

            numJoints = jointsAcc.count * 4;
            joints = new unsigned int[jointsAcc.count * 4];
            switch (jointsAcc.componentType) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            {
                const unsigned short* bufferJointsShort = reinterpret_cast<const unsigned short*>(&jointsBuffer.data[jointsView.byteOffset + jointsAcc.byteOffset]);

                //attributes.emplace_back(Attribute::JOINTS, sizeof(float) * 4, vertexSize);
                //vertexSize += sizeof(float) * 4;

                for (unsigned int i = 0; i < jointsAcc.count; ++i)
                {
                    joints[i * 4] = bufferJointsShort[0];
                    joints[i * 4 + 1] = bufferJointsShort[1];
                    joints[i * 4 + 2] = bufferJointsShort[2];
                    joints[i * 4 + 3] = bufferJointsShort[3];

                    if (jointsView.byteStride != 0) 
                    {
                        bufferJointsShort = reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(bufferJointsShort) + jointsView.byteStride);
                    }
                    else {
                        bufferJointsShort += 4;
                    }
                }
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            {
                const unsigned int* bufferJointsInt = reinterpret_cast<const unsigned int*>(&jointsBuffer.data[jointsView.byteOffset + jointsAcc.byteOffset]);

                for (unsigned int i = 0; i < jointsAcc.count; ++i)
                {
                    joints[i * 4] = bufferJointsInt[0];
                    joints[i * 4 + 1] = bufferJointsInt[1];
                    joints[i * 4 + 2] = bufferJointsInt[2];
                    joints[i * 4 + 3] = bufferJointsInt[3];

                    if (jointsView.byteStride != 0) 
                    {
                        bufferJointsInt = reinterpret_cast<const unsigned int*>(reinterpret_cast<const char*>(bufferJointsInt) + jointsView.byteStride);
                    }
                    else {
                        bufferJointsInt += 4;
                    }
                }
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            {
                const unsigned char* bufferJointsChar = reinterpret_cast<const unsigned char*>(&jointsBuffer.data[jointsView.byteOffset + jointsAcc.byteOffset]);

                for (unsigned int i = 0; i < jointsAcc.count; ++i)
                {
                    joints[i * 4] = bufferJointsChar[0];
                    joints[i * 4 + 1] = bufferJointsChar[1];
                    joints[i * 4 + 2] = bufferJointsChar[2];
                    joints[i * 4 + 3] = bufferJointsChar[3];

                    if (jointsView.byteStride != 0) 
                    {
                        bufferJointsChar = reinterpret_cast<const unsigned char*>(reinterpret_cast<const char*>(bufferJointsChar) + jointsView.byteStride);
                    }
                    else {
                        bufferJointsChar += 4;
                    }
                }
                break;
            }
            default:
                //ERROR handling
                break;
            }
        }

        if (itWeights != primitive.attributes.end())
        {
            const tinygltf::Accessor& weightsAcc = model.accessors[itWeights->second];
            assert(weightsAcc.type == TINYGLTF_TYPE_VEC4);
            assert(weightsAcc.componentType == GL_FLOAT);
            const tinygltf::BufferView& weightsView = model.bufferViews[weightsAcc.bufferView];
            const tinygltf::Buffer& weightsBuffer = model.buffers[weightsView.buffer];

            const float* bufferWeights = reinterpret_cast<const float*>(&weightsBuffer.data[weightsView.byteOffset + weightsAcc.byteOffset]);
            numWeights = weightsAcc.count * 4;
            weights = new float[numWeights];
            for (unsigned int i = 0; i < weightsAcc.count; ++i)
            {
                weights[i * 4] = bufferWeights[0];
                weights[i * 4 + 1] = bufferWeights[1];
                weights[i * 4 + 2] = bufferWeights[2];
                weights[i * 4 + 3] = bufferWeights[3];

                if (weightsView.byteStride != 0) {
                    bufferWeights = reinterpret_cast<const float*>(reinterpret_cast<const char*>(bufferWeights) + weightsView.byteStride);
                }
                else {
                    bufferWeights += 4;
                }
            }
        }
    }

    ResourceMesh* rMesh = new ResourceMesh(uid, numIndices, std::move(indices), numJoints, std::move(joints), numWeights, std::move(weights), numVertices, std::move(attributes), std::move(attributesData));
    Importer::Mesh::Save(rMesh);
    return rMesh;
}