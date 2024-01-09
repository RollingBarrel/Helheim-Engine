#include "Importer.h"
#include "ImporterModel.h"
#include "ImporterTexture.h"
#include "ImporterShader.h"
#include "Globals.h"
#include <iostream>
#include <fstream>
#include <string>

void Importer::Import(const char* filePath)
{
	//TODO work with the file path to call other importers
	std::string extension = filePath;
	unsigned pos = extension.find_last_of('.');
	extension = extension.substr(++pos);

	// .gltf IMPORT Model
	if (extension.compare("gltf") == 0)
	{
		Importer::Model::Import(filePath);
	}
	// .dds Texture
	else if (extension.compare("dds") == 0)
	{
		Importer::Texture::Import(filePath);
	}
	// .glsl Shaders
	else if ((extension.compare("vs") == 0) || (extension.compare("fs") == 0))
	{
		Importer::Shader::Import(filePath);
	}
	// Others don't know
	else
	{
		LOG("The file %s has a unsupported extension type %s", filePath, extension);
	}
}

void Importer::CreateBinaryFile()
{
	int x = 2147483647;
	std::string buff = "dsqafdasDDDASfdsagfdsafsdafds fsdafsadfsadf sdf asdf sadfsa df a";
	size_t  buffSize = buff.size();

	//Write in File
	std::ofstream outf("text.bin", std::ios::binary | std::ios::trunc);

	if (outf.is_open())
	{
		outf.write(reinterpret_cast<char*>(&x), sizeof(x));

		//outf.write((char*)&buffSize, sizeof(buffSize));
		//outf.write(buff.c_str(), buffSize);
		outf.close();
	}

	std::ifstream inf("text.bin", std::ios::binary);

	x = 0;

	size_t  rBuffSize = 0;
	std::string data;

	if (inf.is_open())
	{
		inf.read((char*)&x, sizeof(x));

		//inf.read((char*)&rBuffSize, sizeof(rBuffSize));
		//data.resize(rBuffSize);
		////Cannot assume strings take continuous memory
		//for (int i = 0; i < rBuffSize; i++)
		//{
		//	inf.read((char*)(&data[i]), sizeof(data[i]));
		//}
	}

	inf.close();

	LOG("%u", x);
	LOG("%s", data.c_str());
}