#include "TagsManager.h"
#include <algorithm>
#include <iterator>

TagsManager::TagsManager()
{
	// System
    mTags.push_back(new Tag(0, "Untagged", TagType::SYSTEM));
    mTags.push_back(new Tag(1, "Respawn", TagType::SYSTEM));
    mTags.push_back(new Tag(2, "Finish", TagType::SYSTEM));
    mTags.push_back(new Tag(3, "EditorOnly", TagType::SYSTEM));
    mTags.push_back(new Tag(4, "MainCamera", TagType::SYSTEM));
    mTags.push_back(new Tag(5, "Player", TagType::SYSTEM));

}

TagsManager::~TagsManager()
{
}

void TagsManager::AddTag(std::string tagname)
{
    if (TagNameExists(tagname) == nullptr) {
        Tag* newTag = new Tag(lastIndex, tagname, TagType::CUSTOM);
        mTags.push_back(newTag);

        ++lastIndex;
    }
}

int TagsManager::GetCustomTagsSize()
{
    std::vector<Tag*> customs = GetCustomTag();

    return customs.size();
}

std::vector<Tag*> TagsManager::GetSystemTag()
{
    std::vector<Tag*> systemTags;
    std::copy_if(mTags.begin(), mTags.end(), std::back_inserter(systemTags),
        [](Tag* tag) { return tag->GetType() == TagType::SYSTEM; });

    return systemTags;
}

std::vector<Tag*> TagsManager::GetCustomTag()
{
    std::vector<Tag*> customTags;
    std::copy_if(mTags.begin(), mTags.end(), std::back_inserter(customTags),
        [](Tag* tag) { return tag->GetType() == TagType::CUSTOM; });

    return customTags;
}

Tag* TagsManager::GetTag(std::string tagname)
{
    // Dublicate but public version of TagNameExists
    return TagNameExists(tagname);
}

void TagsManager::DeleteTag(Tag* tag)
{
    std::vector<Tag*> customs = GetCustomTag();
    auto it = std::find(customs.begin(), customs.end(), tag);

    if (it != customs.end()) {
        customs.erase(it);

        delete tag;
    }
}

Tag* TagsManager::TagNameExists(std::string tagname)
{
    // Prevent delete system tags
    for (Tag* tag : mTags) {
        if (std::strcmp(tag->GetName().c_str(), tagname.c_str()) == 0) {
            return tag;
        }
    }
    return nullptr; 
}


