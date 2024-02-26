#include "TagsManager.h"

TagsManager::TagsManager()
{
	// System
	mSystemTags.push_back(new Tag(0, "Untagged"));
	mSystemTags.push_back(new Tag(1, "Respawn"));
	mSystemTags.push_back(new Tag(2, "Finish"));
	mSystemTags.push_back(new Tag(3, "EditorOnly"));
	mSystemTags.push_back(new Tag(4, "Main Camera"));
	mSystemTags.push_back(new Tag(5, "Player"));

}

TagsManager::~TagsManager()
{
}

void TagsManager::AddTag(char* tagname)
{
    if (!TagNameExists(tagname)) {
        Tag* newTag = new Tag(lastIndex, tagname);
        mCustomTags.push_back(newTag);

        ++lastIndex;
    }
}

void TagsManager::DeleteTag(Tag* tag)
{
    auto it = std::find(mCustomTags.begin(), mCustomTags.end(), tag);

    if (it != mCustomTags.end()) {
        mCustomTags.erase(it);

        delete tag;
    }
}

bool TagsManager::TagNameExists( const char* tagname)
{
    for (const Tag* tag : mCustomTags) {
        if (std::strcmp(tag->GetName().c_str(), tagname) == 0) {
            return true; 
        }
    }
    return false; 
}


