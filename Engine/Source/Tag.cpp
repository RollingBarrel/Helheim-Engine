#include "Tag.h"


Tag::Tag(unsigned id, std::string name, TagType type)
{
	mId = id;
	mName = name;
	mType = type;
}

Tag::~Tag()
{
}
