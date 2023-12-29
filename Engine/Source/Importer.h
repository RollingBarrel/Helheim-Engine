#pragma once

namespace Importer
{

	inline void Import(const char* filePath);

	void Load(const char* filePath, char* &buffer);
	void Save(const char* filePath, const void* buffer, unsigned int size, bool append = false);

	void CreateBinaryFile();

}