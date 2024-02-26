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

void TagsManager::AddTag(std::string tagname)
{
    if (TagNameExists(tagname) == nullptr) {
        Tag* newTag = new Tag(lastIndex, tagname);
        mCustomTags.push_back(newTag);

        ++lastIndex;
    }
}

Tag* TagsManager::GetTag(std::string tagname)
{
    // Dublicate but public version of TagNameExists
    return TagNameExists(tagname);
}

void TagsManager::DeleteTag(Tag* tag)
{
    auto it = std::find(mCustomTags.begin(), mCustomTags.end(), tag);

    if (it != mCustomTags.end()) {
        mCustomTags.erase(it);

        delete tag;
    }
}

Tag* TagsManager::TagNameExists(std::string tagname)
{
    for (Tag* tag : mCustomTags) {
        if (std::strcmp(tag->GetName().c_str(), tagname.c_str()) == 0) {
            return tag;
        }
    }
    return nullptr; 
}


