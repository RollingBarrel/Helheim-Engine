#ifndef _MODULE_GAME_H_
#define _MODULE_GAME_H_
#include "Module.h"

class ModuleGame: public Module
{
	bool Init() override;
	update_status PreUpdate(float dt);
	update_status Update(float dt) override;
	bool CleanUp() override;

private:
	unsigned int mVAO = 0;
	unsigned int mScreenPrevSizeX;
	unsigned int mScreenPrevSizeY;
};

#endif //_MODULE_GAME_H_