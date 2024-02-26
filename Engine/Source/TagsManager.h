#pragma once
#include "Tag.h"

#include <vector>
class TagsManager
{
public:
	TagsManager();
	~TagsManager();

	void AddTag(char* tag);
	int GetSize() { return (mSystemTags.size() + mCustomTags.size()); };
	int GetCustomTagsSize() { return mCustomTags.size(); };
	std::vector<Tag*> GetSystemTag() { return mSystemTags; };
	std::vector<Tag*> GetCustomTag() { return mCustomTags; };

	void DeleteTag(Tag* tag);

	//GetSystemTagNames();
private:
	bool TagNameExists(const char* tagname);
	std::vector<Tag*> mSystemTags;
	std::vector<Tag*> mCustomTags;

	unsigned lastIndex = 0;
};

