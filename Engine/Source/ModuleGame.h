#ifndef _MODULE_GAME_H_
#define _MODULE_GAME_H_
#include "Module.h"

class ModuleGame: public Module
{
	bool Init() override;
	update_status PostUpdate(float dt) override;

private:
	unsigned int mGameProgramId = 0;
};

#endif //_MODULE_GAME_H_