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
	int GetSize() { return (mSystemTags.size() + mCustomTags.size()); };
	int GetCustomTagsSize() { return mCustomTags.size(); };
	std::vector<Tag*> GetSystemTag() { return mSystemTags; };
	std::vector<Tag*> GetCustomTag() { return mCustomTags; };

	Tag* GetTag(std::string tagname);
	void DeleteTag(Tag* tag);

private:
	Tag* TagNameExists(std::string tagname);
	std::vector<Tag*> mSystemTags;
	std::vector<Tag*> mCustomTags;

	unsigned lastIndex = 0;
};

