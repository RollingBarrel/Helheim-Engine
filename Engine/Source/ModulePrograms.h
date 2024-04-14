#pragma once
#include "Module.h"

class ModulePrograms :
    public Module
{
	public:
		ModulePrograms();
		~ModulePrograms();

		bool Init() override;
		bool CleanUp() override;

		unsigned int CreateShaderProgramFromIDs(unsigned vertexShaderID, unsigned fragmentShaderID) const;
		unsigned int CreateShaderProgramFromPaths(const char* vertexShaderPath, const char* fragmentShaderPath) const;
		char* LoadShaderSource(const char* shaderFileName) const;
		unsigned int CompileShader(unsigned type, const char* source) const;

		unsigned int GetUIImageProgram() const { return mUIImageProgram; }

private:
	unsigned int mUIImageProgram = 0;
};

