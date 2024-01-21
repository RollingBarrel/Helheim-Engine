#include "Panel.h"

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