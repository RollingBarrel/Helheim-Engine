#include "Panel.h"
#include <string>
#include <algorithm>

Panel::Panel(){}

Panel::Panel(const char* name, bool open)
{
	mName = name;
	mOpen = open;
}

Panel::~Panel()
{
}

bool Panel::IsOpen() const
{
	return mOpen;
}

void Panel::Draw(int windowFlags)
{

}

static bool CharCompare(char a, char b)
{
	return std::tolower(a) == std::tolower(b);
}

bool Panel::CaseInsensitiveSubstringSearch(const std::string& str, const std::string& sub)
{
	std::string::const_iterator it = std::search(str.begin(), str.end(), sub.begin(), sub.end(), CharCompare);

	return it != str.end();
}