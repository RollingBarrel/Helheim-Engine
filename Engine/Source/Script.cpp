#include "Script.h"

Script::Script(GameObject* owner) : mGameObject(owner)
{

}

void Script::SetName(const std::string name)
{
	mName = name;
}
