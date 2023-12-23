#include "ModuleProgram.h"
#include "Globals.h"
#include "Application.h"
#include "SDL.h"
#include "glew.h"


ModuleProgram::ModuleProgram()
{

}
ModuleProgram::~ModuleProgram()
{

}

bool ModuleProgram::Init()
{
	// Add to mPrograms any shader you need
	mPrograms["default"] = CreateShaderProgramFromPaths("default_vertex.glsl", "default_fragment.glsl");

	return true;
}
update_status ModuleProgram::Update()
{

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
	std::string fullVertexShaderPath = "shaders/" + std::string(vertexShaderPath);
	std::string fullFragmentShaderPath = "shaders/" + std::string(fragmentShaderPath);
	char* vertexSource = LoadShaderSource(fullVertexShaderPath.c_str());
	char* fragmentSource = LoadShaderSource(fullFragmentShaderPath.c_str());
	unsigned vertexShaderID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned fragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	free(vertexSource);
	free(fragmentSource);
	return CreateShaderProgramFromIDs(vertexShaderID, fragmentShaderID);
}