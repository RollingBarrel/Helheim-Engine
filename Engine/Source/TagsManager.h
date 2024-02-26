#pragma once
#include "Tag.h"

#include <vector>
#include <string>
class TagsManager
{
public:
	TagsManager();
	~TagsManager();

	void AddTag(std::string tag);
	int GetSize() { return mTags.size(); };
	int GetCustomTagsSize();

	std::vector<Tag*> GetAllTags() { return mTags; };	
	std::vector<Tag*> GetSystemTag();
	std::vector<Tag*> GetCustomTag();

	Tag* GetTag(std::string tagname);
	void DeleteTag(Tag* tag);

private:
	Tag* TagNameExists(std::string tagname);

	std::vector<Tag*> mTags;

	unsigned lastIndex = 0;
};

