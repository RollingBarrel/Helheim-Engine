#include "ModuleProgram.h"
#include "Globals.h"
#include "Application.h"
#include "SDL.h"
#include "glew.h"
#include <filesystem>
#include <fstream>
#include <sstream>

#include "ModuleCamera.h"
#include "imgui.h"

ModuleProgram::ModuleProgram()
{

}
ModuleProgram::ModuleProgram(const char* vertexShader, const char* fragmentShader)
{
	mVertexShader = vertexShader;
	mFragmentShader = fragmentShader;
}
ModuleProgram::~ModuleProgram()
{

}

bool ModuleProgram::Init()
{
	
	//for (const auto& entry : std::filesystem::recursive_directory_iterator("Assets/Shaders/")) {
	//	if (entry.is_regular_file()) {
	//		std::string path = entry.path().generic_u8string();
	//		std::ifstream strm(path.c_str());
	//		if (!strm.is_open()) {
	//			LOG("Error opening file: %s", path.c_str())49;
	//			continue;
	//		}
	//		std::stringstream buffer;
	//		buffer << strm.rdbuf();
	//		size_t pos = path.find_first_of("/", 0);
	//		if (pos != std::string::npos)
	//		{
	//			if (glNamedStringARB) {
	//				glNamedStringARB(GL_SHADER_INCLUDE_ARB, -1, &path.c_str()[pos], -1, buffer.str().c_str());
	//			}
	//			else {
	//				LOG("glNamedStringARB no está disponible en este contexto OpenGL.");
	//			}
	//			
	//		}
	//	}
	//}

	// Add to mPrograms any shader you need
	//mPrograms["default"] = CreateShaderProgramFromPaths(mVertexShader, mFragmentShader);

	//Temorary to do a render for the delivery
	mPbrProgramId = CreateShaderProgramFromPaths(mVertexShader, mFragmentShader);
	glUseProgram(mPbrProgramId);
	glUniform3fv(1, 1, mLightDir);
	glUniform3fv(2, 1, App->GetCamera()->GetPos().ptr());
	glUniform3fv(3, 1, mLightCol);
	glUniform3fv(4, 1, mAmbientCol);
	glUniform1f(5, mLightIntensity);
	glUseProgram(0);

	return true;
}
update_status ModuleProgram::Update()
{
	//Temorary to do a render for the delivery
	glUseProgram(mPbrProgramId);
	glUniform3fv(2, 1, App->GetCamera()->GetPos().ptr());
	//ImGui::Begin("Lighting");
	//if (ImGui::DragFloat("LightIntensity", &lightIntensity, 0.05f, 0.0f, 5.0f))
	//	glUniform1f(5, lightIntensity);
	//if (ImGui::DragFloat3("LightDir", lightDir, 0.05f, -1.0f, 1.0f))
	//	glUniform3fv(1, 1, lightDir);
	//if (ImGui::ColorPicker3("LightCol", lightCol))
	//	glUniform3fv(3, 1, lightCol);
	//if (ImGui::ColorPicker3("AmbientCol", ambientCol))
	//	glUniform3fv(4, 1, ambientCol);
	//ImGui::End();
	if (mModified)
	{
		glUniform1f(5, mLightIntensity);
		glUniform3fv(1, 1, mLightDir);
		glUniform3fv(3, 1, mLightCol);
		glUniform3fv(4, 1, mAmbientCol);
	}
	glUseProgram(0);
	return UPDATE_CONTINUE;
}
bool ModuleProgram::CleanUp()
{
	return true;
}

const unsigned ModuleProgram::GetProgramID(std::string programName) const
{
	auto iter = mPrograms.find(programName);
	if (iter != mPrograms.end())
	{
		return iter->second;
	}
	else 
	{
		LOG("[error] There is no program %s.");
		return NULL;
	}
}

char* ModuleProgram::LoadShaderSource(const char* shaderFileName) const
{
	char* data = nullptr;
	FILE* file = nullptr;
	auto info = fopen_s(&file, shaderFileName, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		data = (char*)malloc(size + 1);
		fseek(file, 0, SEEK_SET);
		fread(data, 1, size, file);
		data[size] = 0;
		fclose(file);
	}
	return data;
}

unsigned ModuleProgram::CompileShader(unsigned type, const char* source) const
{

	unsigned shaderID = glCreateShader(type);

	glShaderSource(shaderID, 1, &source, 0);
	glCompileShader(shaderID);
	int resolution = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &resolution);
	if (resolution == GL_FALSE)
	{
		int length = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		if (length > 0)
		{
			int written = 0;
			char* info = (char*)malloc(length);
			glGetShaderInfoLog(shaderID, length, &written, info);
			LOG("Log Info: %s", info);
			free(info);
		}
	}
	return shaderID;
}

unsigned ModuleProgram::CreateShaderProgramFromIDs(unsigned vertexShaderID, unsigned fragmentShaderID) const
{
	unsigned programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);
	int resolution;
	glGetProgramiv(programID, GL_LINK_STATUS, &resolution);
	if (resolution == GL_FALSE)
	{
		int length = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
		if (length > 0)
		{
			int written = 0;
			char* info = (char*)malloc(length);
			glGetProgramInfoLog(programID, length, &written, info);
			LOG("Program Log Info: %s", info);
			free(info);
		}
	}
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	return programID;
}
unsigned ModuleProgram::CreateShaderProgramFromPaths(const char* vertexShaderPath, const char* fragmentShaderPath) const
{
	std::string fullVertexShaderPath = "Assets/Shaders/" + std::string(vertexShaderPath);
	std::string fullFragmentShaderPath = "Assets/Shaders/" + std::string(fragmentShaderPath);
	char* vertexSource = LoadShaderSource(fullVertexShaderPath.c_str());
	char* fragmentSource = LoadShaderSource(fullFragmentShaderPath.c_str());
	unsigned vertexShaderID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned fragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	free(vertexSource);
	free(fragmentSource);
	return CreateShaderProgramFromIDs(vertexShaderID, fragmentShaderID);
}