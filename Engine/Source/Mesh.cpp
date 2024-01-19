#include "Mesh.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ResourceMaterial.h"
#include "Texture.h"

#include "glew.h"
#include "SDL.h"
#include "MathGeoLib.h"
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_IMPLEMENTATION /* Only in one of the includes */
#include "tiny_gltf.h"

Mesh::Mesh()
{

}
Mesh::~Mesh()
{

}

void Mesh::LoadVBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
	const auto& itPos = primitive.attributes.find("POSITION");
	const auto& itNorm = primitive.attributes.find("NORMAL");
	const auto& itTex_0 = primitive.attributes.find("TEXCOORD_0");

	const unsigned char* bufferPos = nullptr;
	const unsigned char* bufferNorm = nullptr;
	const unsigned char* bufferTex_0 = nullptr;
	size_t byteStride = 0;
	int vertSize = 0;

	if (itPos != primitive.attributes.end())
	{
		const tinygltf::Accessor& posAcc = model.accessors[itPos->second];
		m_numVertices = posAcc.count;
		SDL_assert(posAcc.type == TINYGLTF_TYPE_VEC3);
		SDL_assert(posAcc.componentType == GL_FLOAT);
		const tinygltf::BufferView& posView = model.bufferViews[posAcc.bufferView];
		const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
		bufferPos = &(posBuffer.data[posAcc.byteOffset + posView.byteOffset]);
		byteStride = posView.byteStride;
		vertSize += 3;
	}
	if (itNorm != primitive.attributes.end())
	{
		const tinygltf::Accessor& normAcc = model.accessors[itNorm->second];
		m_numVertices = normAcc.count;
		SDL_assert(normAcc.type == TINYGLTF_TYPE_VEC3);
		SDL_assert(normAcc.componentType == GL_FLOAT);
		const tinygltf::BufferView& normView = model.bufferViews[normAcc.bufferView];
		const tinygltf::Buffer& normBuffer = model.buffers[normView.buffer];
		bufferNorm = &(normBuffer.data[normAcc.byteOffset + normView.byteOffset]);
		SDL_assert(normView.byteStride == byteStride);
		vertSize += 3;
	}
	if (itTex_0 != primitive.attributes.end())
	{
		const tinygltf::Accessor& tex0Acc = model.accessors[itTex_0->second];
		m_numVertices = tex0Acc.count;
		SDL_assert(tex0Acc.type == TINYGLTF_TYPE_VEC2);
		SDL_assert(tex0Acc.componentType == GL_FLOAT);
		const tinygltf::BufferView& tex0View = model.bufferViews[tex0Acc.bufferView];
		const tinygltf::Buffer& tex0Buffer = model.buffers[tex0View.buffer];
		bufferTex_0 = &(tex0Buffer.data[tex0Acc.byteOffset + tex0View.byteOffset]);
		SDL_assert(tex0View.byteStride == byteStride);
		vertSize += 2;
	}

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertSize * m_numVertices, nullptr, GL_STATIC_DRAW);
	float* ptr = reinterpret_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	for (size_t i = 0; i < (m_numVertices); ++i)
	{
		size_t offset = 0;
		if (bufferPos != nullptr) {
			ptr[i * 3 + 0] = *reinterpret_cast<const float*>(bufferPos + sizeof(float) * 0);
			ptr[i * 3 + 1] = *reinterpret_cast<const float*>(bufferPos + sizeof(float) * 1);
			ptr[i * 3 + 2] = *reinterpret_cast<const float*>(bufferPos + sizeof(float) * 2);
			if (byteStride != 0) bufferPos += byteStride;
			else bufferPos += sizeof(float) * 3;
			offset += (size_t)(m_numVertices * 3);
		}
		if (bufferNorm != nullptr) {
			ptr[offset + i * 3 + 0] = *reinterpret_cast<const float*>(bufferNorm + sizeof(float) * 0);
			ptr[offset + i * 3 + 1] = *reinterpret_cast<const float*>(bufferNorm + sizeof(float) * 1);
			ptr[offset + i * 3 + 2] = *reinterpret_cast<const float*>(bufferNorm + sizeof(float) * 2);
			if (byteStride != 0) bufferNorm += byteStride;
			else bufferNorm += sizeof(float) * 3;
			offset += (size_t)(m_numVertices * 3);
		}
		if (bufferTex_0 != nullptr) {
			ptr[offset + i * 2 + 0] = *reinterpret_cast<const float*>(bufferTex_0 + sizeof(float) * 0);
			ptr[offset + i * 2 + 1] = *reinterpret_cast<const float*>(bufferTex_0 + sizeof(float) * 1);
			if (byteStride != 0) bufferTex_0 += byteStride;
			else bufferTex_0 += sizeof(float) * 2;
			offset += (size_t)(m_numVertices * 2);
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Mesh::LoadEBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
	if (primitive.indices >= 0)
	{
		const tinygltf::Accessor& indAcc = model.accessors[primitive.indices];
		m_numIndices = indAcc.count;
		glGenBuffers(1, &m_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indAcc.count, nullptr, GL_STATIC_DRAW);
		unsigned int* ptr = reinterpret_cast<unsigned int*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
		const tinygltf::BufferView& indView = model.bufferViews[indAcc.bufferView];
		const unsigned char* buffer = &(model.buffers[indView.buffer].data[indAcc.byteOffset +
			indView.byteOffset]);
		if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT)
		{
			const uint32_t* bufferInd = reinterpret_cast<const uint32_t*>(buffer);
			for (uint32_t i = 0; i < m_numIndices; ++i) ptr[i] = bufferInd[i];
		}
		else if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
		{
			const uint16_t* bufferInd = reinterpret_cast<const uint16_t*>(buffer);
			for (uint32_t i = 0; i < m_numIndices; ++i) ptr[i] = bufferInd[i];
		}
		else if (indAcc.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
		{
			const uint8_t* bufferInd = reinterpret_cast<const uint8_t*>(buffer);
			for (uint32_t i = 0; i < m_numIndices; ++i) ptr[i] = bufferInd[i];
		}
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	}
}

void Mesh::CreateVAO()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 3 * m_numVertices));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 6 * m_numVertices));
	glBindVertexArray(0);
}

void Mesh::Render(float lightDir[3], float lightColor[3], float lightIntensity, float ambientColor[3])
{
	unsigned program = App->GetProgram()->GetProgramID("default");
	glUseProgram(program);

	// Fragment shader
	if (mMaterial != nullptr) {
		//glActiveTexture(GL_TEXTURE5);
		//glBindTexture(GL_TEXTURE_2D, textures[textureID]->getTextureID());
		glUniform3fv(glGetUniformLocation(program,"material.diffuseColor"), 1, &mMaterial->GetDiffuseFactor().xyz()[0]);
		glUniform3fv(glGetUniformLocation(program,"material.specularColor"), 1, &mMaterial->GetSpecularFactor()[0]);
		glUniform1f(glGetUniformLocation(program,"material.shininess"), mMaterial->GetGlossinessFactor());
		glUniform3f(glGetUniformLocation(program, "material.ambientColor"), ambientColor[0], ambientColor[1], ambientColor[2]);
		if (mMaterial->GetEnableDiffuseTexture() && mMaterial->GetDiffuseMap() != nullptr)
		{
			glUniform1i(glGetUniformLocation(program,"material.hasDiffuseMap"), 1);
			GLint diffuseTextureLoc = glGetUniformLocation(program, "material.diffuseTexture");
			glUniform1i(diffuseTextureLoc, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mMaterial->GetDiffuseMap()->getTextureID());
		}
		else {
			glUniform1i(glGetUniformLocation(program,"material.hasDiffuseMap"), 0);
		}

		if (mMaterial->GetEnableSpecularGlossinessTexture() && mMaterial->GetSpecularMap() != nullptr)
		{
			glUniform1i(glGetUniformLocation(program, "material.hasSpecularMap"), 1);
			GLint specularTextureLoc = glGetUniformLocation(program, "material.specularTexture");
			glUniform1i(specularTextureLoc, 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mMaterial->GetSpecularMap()->getTextureID());
		}
		else
		{
			glUniform1i(glGetUniformLocation(program, "material.hasSpecularMap"), 0);
		}

		if (mMaterial->GetEnableShinessMap())
		{
			glUniform1i(glGetUniformLocation(program, "material.hasShininessMap"), 1);
		}
		else 
		{
			glUniform1i(glGetUniformLocation(program, "material.hasShininessMap"), 0);
		}

		glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir[0], lightDir[1], lightDir[2]);
		glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor[0], lightColor[1], lightColor[2]);
		glUniform1f(glGetUniformLocation(program, "lightIntensity"), lightIntensity);
		

		float3 cameraPos = App->GetCamera()->GetCameraPos();
		glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	}

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, nullptr);

	//unsigned program = App->GetProgram()->GetProgramID("default");
	//glUseProgram(program);


	////glActiveTexture(GL_TEXTURE5);
	////glBindTexture(GL_TEXTURE_2D, textures[m_material]);
	////glUniform1i(glGetUniformLocation(program, "diffuse"), 0);
	//glBindVertexArray(m_vao);
	//glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, nullptr);
	////glDrawArrays(GL_TRIANGLES, 0, m_numVertices);

}