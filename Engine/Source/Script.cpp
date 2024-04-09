#include "Script.h"

Script::Script(GameObject* owner) : mGameObject(owner)
{

}

Script::~Script()
{
	for (Member* member : mMembers) {
		delete member;
	}
	mMembers.clear();
}


