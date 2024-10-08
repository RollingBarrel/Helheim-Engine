#pragma once
#include "Script.h"
#include "Macros.h"

GENERATE_BODY(PlayAnimationScript);
class PlayAnimationScript : public Script
{
	FRIEND(PlayAnimationScript)
public:

    PlayAnimationScript(GameObject* owner) : Script(owner) {}
    ~PlayAnimationScript() {}
    void Start() override;

};

