#include "Importer.h"
#include <fstream>  
#include <bitset>

Importer::Importer()
{

}

Importer::~Importer()
{

}

std::string str = "Holis";

void Importer::CreateBinaryFile()
{
	
	std::ofstream fs("text.txt", std::ios::binary | std::ios::out | std::ios::app);	
	fs.write(str.c_str(), strlen(str.c_str()));
	fs.close();
}