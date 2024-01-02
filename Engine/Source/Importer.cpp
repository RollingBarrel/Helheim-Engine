#include "Importer.h"
#include "ImporterModel.h"
#include "Globals.h"
#include <iostream>
#include <fstream>
#include <string>

void Importer::Import(const char* filePath)
{
	//TODO work with the file path to call other importers

	// .gltf IMPORT Model
	Importer::Model::Import(filePath);

	// .dds Texture
	 

	// .glsl Shaders

	// Others don't know
}

void Importer::Load(const char* filePath, char*& buffer) 
{

}

void Importer::Save(const char* file_path, const void* buffer, unsigned int size, bool append)
{

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