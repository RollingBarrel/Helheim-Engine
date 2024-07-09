#include "ImporterIBL.h"
#include "SaveLoadIBL.h"
#include "ResourceIBL.h"
#include "DirectXTex.h"
#include <MathConstants.h>
#include "float3.h"
#include "Geometry/Frustum.h"
#include "glew.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleFileSystem.h"


ResourceIBL* Importer::IBL::Import(const char* hdrTexPath, unsigned int uid)
{
	//TODO: parameters to modify from meta file
	unsigned int irradianceSize = 256;
	unsigned int specEnvBRDFSize = 256;
	unsigned int specPrefilteredSize = 512;
	ResourceIBL* rIBL = nullptr;

	DirectX::ScratchImage image;

	size_t size = strlen(hdrTexPath) + 1;
	wchar_t* pathTex = new wchar_t[size];
	size_t outSize;
	mbstowcs_s(&outSize, pathTex, size, hdrTexPath, size - 1);

	HRESULT res = DirectX::LoadFromHDRFile(pathTex, nullptr, image);
	delete[] pathTex;

	if (res == S_OK)
	{
		const float3 front[6] = { float3::unitX, -float3::unitX, float3::unitY,
							 -float3::unitY, float3::unitZ, -float3::unitZ };

		const float3 up[6] = { -float3::unitY, -float3::unitY, float3::unitZ,
						 -float3::unitZ, -float3::unitY, -float3::unitY };

		Frustum frustum;
		frustum.type = FrustumType::PerspectiveFrustum;
		frustum.pos = float3::zero;
		frustum.nearPlaneDistance = 0.1f;
		frustum.farPlaneDistance = 100.0f;
		frustum.verticalFov = pi / 2.0f;
		frustum.horizontalFov = pi / 2.0f;

		unsigned int hdrTextureId = 0;
		unsigned int environmentTextureId = 0;
		unsigned int irradianceTextureId = 0;
		unsigned int specPrefilteredTexId = 0;
		unsigned int envBRDFTexId = 0;

		const unsigned int irradianceWidth = irradianceSize;
		const unsigned int irradianceHeight = irradianceSize;

		glGenTextures(1, &environmentTextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, environmentTextureId);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceWidth, irradianceHeight, 0,
				GL_RGB, GL_HALF_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



		const DirectX::TexMetadata& metadata = image.GetMetadata();

		glGenTextures(1, &hdrTextureId);
		glBindTexture(GL_TEXTURE_2D, hdrTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, metadata.width, metadata.height, 0, GL_RGBA, GL_FLOAT, image.GetPixels());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		glUseProgram(App->GetOpenGL()->GetEnvironmentProgramId());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hdrTextureId);

		unsigned int frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		glViewport(0, 0, irradianceWidth, irradianceHeight);
		glDepthMask(GL_FALSE);
		glBindVertexArray(App->GetOpenGL()->GetSkyboxVAO());
		for (unsigned int i = 0; i < 6; ++i)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentTextureId, 0);

			frustum.front = front[i];
			frustum.up = up[i];

			glUniformMatrix4fv(0, 1, GL_TRUE, frustum.ViewMatrix().ptr());
			glUniformMatrix4fv(1, 1, GL_TRUE, frustum.ProjectionMatrix().ptr());
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, environmentTextureId);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glUseProgram(App->GetOpenGL()->GetIrradianceProgramId());
		glUniform1ui(5, irradianceSize);
		glGenTextures(1, &irradianceTextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceTextureId);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, irradianceWidth, irradianceHeight, 0,
				GL_RGB, GL_HALF_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, environmentTextureId);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceTextureId, 0);

			frustum.front = front[i];
			frustum.up = up[i];

			glUniformMatrix4fv(0, 1, GL_TRUE, frustum.ViewMatrix().ptr());
			glUniformMatrix4fv(1, 1, GL_TRUE, frustum.ProjectionMatrix().ptr());
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//Specular IBL
		const unsigned int specWidth = specPrefilteredSize;
		const unsigned int specHeight = specPrefilteredSize;
		glGenTextures(1, &specPrefilteredTexId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, specPrefilteredTexId);

		for (int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, specWidth, specHeight, 0, GL_RGB, GL_HALF_FLOAT, nullptr);
		}
		//int numMipMaps = int(log(float(specWidth)) / log(2.0f));
		int numMipMaps = log2(std::max(specWidth, specHeight)) + 1;
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMipMaps);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glUseProgram(App->GetOpenGL()->GetSpecPrefilteredProgramId());
		glUniform1ui(5, specWidth);
		glBindTexture(GL_TEXTURE_CUBE_MAP, environmentTextureId);
		for (int currMipMap = 0; currMipMap <= numMipMaps; ++currMipMap)
		{
			float roughness = (static_cast<float>(currMipMap) / static_cast<float>(numMipMaps - 1));
			glUniform1f(3, roughness);
			float coolMath = specWidth * pow(0.5f, currMipMap);
			glViewport(0, 0, coolMath, coolMath);
			// Render each cube plane
			for (unsigned int i = 0; i < 6; ++i)
			{
				// TODO: Draw UnitCube using prefiltered environment map shader and roughness
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, specPrefilteredTexId, currMipMap);

				frustum.front = front[i];
				frustum.up = up[i];

				glUniformMatrix4fv(0, 1, GL_TRUE, frustum.ViewMatrix().ptr());
				glUniformMatrix4fv(1, 1, GL_TRUE, frustum.ProjectionMatrix().ptr());
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		//EnvironmentBRDFTex
		glGenTextures(1, &envBRDFTexId);
		glBindTexture(GL_TEXTURE_2D, envBRDFTexId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, specEnvBRDFSize, specEnvBRDFSize, 0, GL_RG, GL_HALF_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glUseProgram(App->GetOpenGL()->GetSpecEnvBRDFProgramId());
		glViewport(0, 0, specEnvBRDFSize, specEnvBRDFSize);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, envBRDFTexId, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
		glUseProgram(0);
		glDepthMask(GL_TRUE);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &frameBuffer);
		glDeleteTextures(1, &hdrTextureId);
		glViewport(0, 0, App->GetOpenGL()->GetSceneWidth(), App->GetOpenGL()->GetSceneHeight());

		rIBL = new ResourceIBL(uid, environmentTextureId, irradianceSize, irradianceTextureId, irradianceSize, specPrefilteredTexId, specPrefilteredSize, envBRDFTexId, specEnvBRDFSize);
		Save(rIBL);
	}

	return rIBL;
}