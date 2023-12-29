#pragma once
class Importer
{
public:
	Importer();
	virtual ~Importer();

	virtual void Import(const char* filePath) const;

	virtual void Load(const char* filePath, char* &buffer) const;
	virtual void Save(const char* filePath, const void* buffer, unsigned int size, bool append = false) const;

	void CreateBinaryFile();

private:

};