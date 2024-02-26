#include "Tag.h"


Tag::Tag(unsigned id, std::string name): mId(0), mName("default")
{
	mId = id;
	mName = name;
}

Tag::~Tag()
{
}
