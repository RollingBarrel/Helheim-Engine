#pragma once
#include <string>
class Tag
{
public:
	Tag(unsigned id, std::string name);
	~Tag();

	std::string GetName() const { return mName; };

private:
	unsigned mId;
	std:: string mName;
};

