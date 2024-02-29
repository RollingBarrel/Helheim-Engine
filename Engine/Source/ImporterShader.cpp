#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterShader.h"

#include "glew.h"

#include <string>


void Importer::Shader::Import(const char* filePath)
{
	ResourceShader* shader = new ResourceShader;
	char* fileBuffer = nullptr;
	App->GetFileSystem()->Load(filePath, &fileBuffer);

	//LOG("%s", fileBuffer);

	std::string name = filePath;
	int pos = name.find_last_of("/");
	name = name.substr(++pos);
	shader->mShaderName = name.c_str();

	Shader::Save(shader, &fileBuffer);

	ResourceShader* loadedShader = new ResourceShader; //Move to render
	Shader::Load(shader->mShaderName, loadedShader);
	delete shader;
	shader = nullptr;
}

void Importer::Shader::Save(const ResourceShader* ourShader, char** buffer)
{
	std::string path = LIBRARY_SHADER_PATH;
	path += ourShader->mShaderName;

	App->GetFileSystem()->Save(path.c_str(),(*buffer), strlen((*buffer)));
}

void Importer::Shader::Load(const char* fileName, ResourceShader* shader)
{
	std::string path = LIBRARY_SHADER_PATH;
	path += fileName;

	std::string name = fileName;
	int extensionPos = name.find_last_of(".");
	name = name.substr(0, extensionPos);

	if (name.compare("vs") == 0)
		CreateShader(GL_VERTEX_SHADER, path.c_str(), shader);
	else if (name.compare("fs") == 0)
		CreateShader(GL_FRAGMENT_SHADER, path.c_str(), shader);
}

void Importer::Shader::CreateShader(unsigned int type, const char* shaderSource, ResourceShader* shader)
{
	strcpy_s(const_cast<char*>(shader->mShaderName), sizeof(strlen(shader->mShaderName)), shaderSource);
	shader->mType = type;
	shader->mId = glCreateShader(type);
	glShaderSource(shader->mId, 1, &shaderSource, 0);
	glCompileShader(shader->mId);

	int success = GL_FALSE;
	glGetShaderiv(shader->mId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		int len = 0;
		glGetShaderiv(shader->mId, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = new char[len];
			glGetShaderInfoLog(shader->mId, len, &written, info);
			LOG("Log Info: %s", info);
			delete[] info;
			info = nullptr;
		}
	}
}

void Importer::Shader::CreateProgram(const ResourceShader* vertex, const ResourceShader* fragment, ResourceProgram& program)
{
	program.mProgramId = glCreateProgram();

	glAttachShader(program.mProgramId, vertex->mId);
	glAttachShader(program.mProgramId, fragment->mId);
	glLinkProgram(program.mProgramId);
	int res;
	glGetProgramiv(program.mProgramId, GL_LINK_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len = 0;
		glGetProgramiv(program.mProgramId, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = new char[len];
			glGetProgramInfoLog(program.mProgramId, len, &written, info);
			LOG("Program Log Info: %s", info);
			delete[] info;
			info = nullptr;
		}
	}
	glDetachShader(program.mProgramId, vertex->mId);
	glDetachShader(program.mProgramId, fragment->mId);
	glDeleteShader(vertex->mId);
	glDeleteShader(fragment->mId);

	int attributeCount;
	char attributeName[128];
	int attributeNameLenght;
	int attributeSize;
	GLenum attributeType;

	unsigned int attributeLocation;

	glGetProgramiv(program.mProgramId, GL_ACTIVE_ATTRIBUTES, &attributeCount);

	for (int i = 0; i <= attributeCount; ++i)
	{
		glGetActiveAttrib(program.mProgramId, i, ((sizeof(attributeName))/(sizeof(attributeName[0]))), &attributeNameLenght, &attributeSize, &attributeType, attributeName);

		attributeLocation = glGetAttribLocation(program.mProgramId, attributeName);

		unsigned int realCount = 0;

		switch (attributeType)
		{
		case GL_FLOAT:
			realCount = 1;
			break;
		case GL_FLOAT_VEC2:
			realCount = 2;
			break;
		case GL_FLOAT_VEC3:
			realCount = 3;
			break;
		default:
			realCount = 1;
			break;
		}
		
		program.mAttributes.push_back({attributeLocation, realCount});
	}

}
