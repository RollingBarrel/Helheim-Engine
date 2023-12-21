#pragma once
class Importer
{
public:
	Importer();
	~Importer();

	void Save();
	void Load(const char* filePath, char* &buffer) const;
	void Save(const char* file_path, const void* buffer, unsigned int size, bool append = false) const;
	void CreateBinaryFile();

private:

};

