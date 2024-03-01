#include "TagsManager.h"
#include "GameObject.h"
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
    for (Tag* tag : mTags) {
        delete tag;
    }
}

void TagsManager::AddTag(std::string tagname)
{
    if (TagNameExistsByName(tagname) == nullptr) {
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

Tag* TagsManager::GetTagByName(std::string tagname)
{
    // Dublicate but public version of TagNameExistsByName
    return TagNameExistsByName(tagname);
}

Tag* TagsManager::GetTagByID(unsigned id)
{
    // Dublicate but public version of TagNameExistsByID
    return TagNameExistsByID(id);
}

void TagsManager::DeleteTag(Tag* tag)
{
    auto it = std::find(mTags.begin(), mTags.end(), tag);

    if (it != mTags.end()) {

        // 1. Set tags to untagged
        std::vector<GameObject*> objects = GameObject::FindGameObjectsWithTag(tag->GetName());
        for (auto object : objects) {
            object->SetTag(GetTagByName("Untagged"));
        }

        // 2. Delete it
        mTags.erase(it);
        delete tag;
    }
}

Tag* TagsManager::TagNameExistsByName(std::string tagname)
{
    for (Tag* tag : mTags) {
        if (std::strcmp(tag->GetName().c_str(), tagname.c_str()) == 0) {
            return tag;
        }
    }
    return nullptr; 
}

Tag* TagsManager::TagNameExistsByID(unsigned id)
{
    for (Tag* tag : mTags) {
        if (tag->GetID() == id) {
            return tag;
        }
    }
    return nullptr;
}


