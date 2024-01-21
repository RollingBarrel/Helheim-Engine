#include "Importer.h"
#include "ImporterModel.h"
#include "ImporterTexture.h"
#include "ImporterShader.h"
#include "Globals.h"
#include <iostream>
#include <fstream>
#include <string>

#include "Algorithm/Random/LCG.h"

void Importer::Import(const char* filePath)
{
	//TODO work with the file path to call other importers
	std::string extension = filePath;
	unsigned pos = extension.find_last_of('.');
	extension = extension.substr(++pos);

	// .gltf IMPORT Model
	if (extension.compare("gltf") == 0)
	{
		ResourceModel* rModel = new ResourceModel();
		rModel->mUID = math::LCG().Int();
		Importer::Model::Import(filePath, rModel);
	}
	// .dds Texture
	else if (extension.compare("dds") == 0)
	{
		//Importer::Texture::Import(filePath);
	}
	// .glsl Shaders
	else if ((extension.compare("vs") == 0) || (extension.compare("fs") == 0))
	{
		//Importer::Shader::Import(filePath);
	}
	// Others don't know
	else
	{
		LOG("The file %s has a unsupported extension type %s", filePath, extension);
	}
}
