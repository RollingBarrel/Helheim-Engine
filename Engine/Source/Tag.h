#pragma once
#include <string>
enum class TagType : unsigned int {
	SYSTEM, CUSTOM
};
class Tag
{
public:
	Tag(unsigned id, std::string name, TagType type);
	~Tag();

	std::string GetName() const { return mName; };
	TagType GetType() const { return mType; };
	unsigned GetID() const { return mId; };

private:
	unsigned mId;
	std:: string mName;
	TagType mType;
};

