#pragma once
#include "Module.h"
#include "Globals.h"
#include <unordered_map>
#include <string>
class ModuleProgram : public Module
{
public:

	ModuleProgram();
	ModuleProgram(const char* mVertexShader, const char* mFragmentShader);

	~ModuleProgram();

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	const std::unordered_map<std::string, unsigned>& GetPrograms() const { return mPrograms; }
	const unsigned GetProgramID(std::string programName) const;

	const char* GetVertexShader() const { return mVertexShader; }
	const char* GetFragmentShader() const { return mFragmentShader; }

private:
	char* LoadShaderSource(const char* shaderFileName) const;
	unsigned CompileShader(unsigned type, const char* source) const;
	unsigned CreateShaderProgramFromIDs(unsigned vertexShaderID, unsigned fragmentShaderID) const;
	unsigned CreateShaderProgramFromPaths(const char* vertexShaderPath, const char* fragmentShaderPath) const;

private:
	std::unordered_map<std::string, unsigned> mPrograms;

	const char* mVertexShader = "PBR_VertexShader.glsl";
	const char* mFragmentShader = "PBR_PixelShader.glsl";
};
